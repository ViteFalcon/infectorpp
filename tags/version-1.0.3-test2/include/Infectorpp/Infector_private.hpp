/* Copyright (c) 2013 Dario Oliveri

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#pragma once

namespace Infector{

    inline Container::~Container(){
        auto it2 = singleIstances.end();
        for( auto it=singleIstances.begin(); it!=it2; ++it){
            if(it->second!=nullptr)
                delete it->second;
        }
    }

    template<typename Dummy>
    bool Container::tryToGetSize( RecursionLimit *limit, int *size){
        (void)limit; (void)size; //fix unused parameter warning.
        return true;
    }

    template<typename Actual, typename Next, typename...Others>
    bool Container::tryToGetSize( RecursionLimit *limit, int *size){
        //get abstract type
        auto it = typeMap.find( std::type_index(typeid(Actual)) );
        if( it==typeMap.end())
            return false; //abstract type not registered

        auto it2 = recursionMap.find(  it->second.type ); //get concrete funct
        if( it2==recursionMap.end())
            return false; // concrete functor not created yet..

        bool horizontalExpansion = tryToGetSize<Next,Others...>(limit,size);

        (*size)+= sizeof(Actual);
        (*limit).increment();
        bool recursiveExpansion = (it2->second)(limit,size);
        (*limit).decrement();

        bool result = true&&horizontalExpansion&&recursiveExpansion;
        setKnownSizeForType(it->second.type, result, (*size));
        return result;
    }

    template <typename Dummy>
    void Container::setKnownSizeForType( std::type_index t, bool known, int size){
        if(known){
            typeInfoMap[t] = size; //only if outer returned true => biggest minimum size!
        }
    }

    template <typename Dummy>
    void Container::processRecursionWeb(){
        bool changes = true;
        while( changes){
            changes = false;
            auto it = recursionWaitList.begin();
            while (it!=recursionWaitList.end()){
                auto it2=it;
                it++;
                RecursionLimit limit;
                int            size = 0;
                bool gotSize = (*it2)( &limit,&size);
                if(gotSize) {
                    changes = true;
                    recursionWaitList.erase(it2);
                }
            }
        }
    }

    template <typename T, typename... Dependencies>
    void Container::wire(){
        bool tests = reduced_type_tests<T>();
        (void) tests; //fix unused variable warning

        auto it2 = callbacks.find(std::type_index(typeid(T)));
        if( it2!=callbacks.end())
            launch_exception<ExWireAgain>();

        auto it3 = recursionMap.find(std::type_index(typeid(T)));
        if( it3==recursionMap.end()){
            recursionMap[std::type_index(typeid(T))] =
                    [this] (RecursionLimit *limit, int *size){
                        (*size)+= sizeof(T);
                        bool result =
                            tryToGetSize<Dependencies...
                                        ,DummyClass>(limit,size);

                        return result;
                    }; //now the function exist.
            it3 = recursionMap.find(std::type_index(typeid(T)));
            recursionWaitList.push_back(it3->second);
        }

        processRecursionWeb(); //do lots of checks during "wire"
                               //find errors early!

        // THROW... ok exception safe! Indipendently of parameters order
        callbacks[std::type_index(typeid(T))] =
                    [this] (EmplaceContext * context) {
                        (void)context; //fix unused parameter warning.
                        return reinterpret_cast<void*>(
                            new T(UniqueOrShared<Dependencies>(this,context)...)
                        );
                    };
    }

} // namespace Infector

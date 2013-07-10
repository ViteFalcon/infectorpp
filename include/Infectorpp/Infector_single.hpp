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
THE SOFTWARE. */

#pragma once

namespace Infector{

    template <typename T>
    void Container::bindSingleAsNothing(){
        bool tests = reduced_type_tests<T>();
        (void) tests; //fix unused variable warning
        must_not_have<T>();

        typeMap[std::type_index(typeid(T))]
                = Binding(std::type_index(typeid(T)), true);

        try{
            if(singleIstances.find( std::type_index(typeid(T)) )
                                    ==singleIstances.end())
                singleIstances[std::type_index(typeid(T))]
                                = new AnyShared<T>();
        }catch(std::exception & ex){
            auto it2 = typeMap.find( std::type_index(typeid(T)) );
            typeMap.erase(it2);
            limit.reset();
            throw ex;
        }
    }

    template <typename T, typename... Contracts>
    void Container::bindSingleAs(){
        bool tests = type_tests<T,Contracts...>(); // Compile time tests.
        (void) tests;  //fix unused variable warning

        bool success = false;
        try{
            success = resolve_multiple_inheritance<T,Contracts...>();
        }catch(std::exception & ex){
            rollback_multiple_inheritance<T,Contracts...>(); //revert changes
            limit.reset();
            throw ex;
        }
        if(!success) //no changes.
            launch_exception<ExExistingInterface>();

        try{
            if(singleIstances.find( std::type_index(typeid(T)) )
                                    ==singleIstances.end())
                singleIstances[std::type_index(typeid(T))]
                               = new AnyShared<T>();
        }catch(std::exception & ex){
            rollback_multiple_inheritance<T,Contracts...>(); //revert changes
            limit.reset();
            throw ex;
        }
    }

    template <typename T>
    std::shared_ptr<T> Container::buildSingle_delegate(){
        auto it = typeMap.find( std::type_index(typeid(T)) );
        if( it==typeMap.end()) //"it" is concrete type, T abstract one
            launch_exception<ExBuildWhat>(); // TYPE NOT REGISTERED

        if( it->second.single == false)
            launch_exception<ExSingleMulti>(); // T must be single

        //at this point may happen that concrete type and abstract ones are the
        //same.that's ok. (bindSingleAsNothing)
        auto it2 = callbacks.find(  it->second.type ); //find constructor

        if( it2==callbacks.end())
            launch_exception<ExNotWired>(); // CONSTRUCTOR NOT WIRED

        auto it3 = singleIstances.find( it->second.type );

        IAnyShared * any = it3->second;
        if(any->getPtr()==nullptr){
            try{
                any->setPtr( (it2->second)() );
            }catch(std::exception & ex){
                limit.reset();
                throw ex;
            }
        }

        return std::shared_ptr<T> (
                                any->getReferenceCounter(),
                                reinterpret_cast<T*>(any->getPtr()) );
    }

    template <typename T>
    std::shared_ptr<T> Container::buildSingle(){
        limit.reset(); //need to cally only once, or in case of exception.
        return buildSingle_delegate<T>();
    }

} // namespace Infector

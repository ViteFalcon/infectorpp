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

    template <typename T, typename Contract >
    void Container::bindAs(){
        bool tests = type_tests<T,Contract>(); // Compile time tests.
        (void) tests; //fix unused variable warning
        must_not_have<Contract>(); // A contract can't be registered twice

        typeMap[std::type_index(typeid(Contract))]
                = Binding(std::type_index(typeid(T)), false);
    }

    template <typename T>
    void Container::bindAsNothing(){
        bool tests = reduced_type_tests<T>(); // Compile time tests.
        (void) tests; //fix unused variable warning
        must_not_have<T>(); // A Concrete type can't be registered twice.


        typeMap[std::type_index(typeid(T))]
                = Binding(std::type_index(typeid(T)), false);
    }

    template <typename T>
    std::unique_ptr<T> Container::build_delegate(){
        auto it = typeMap.find( std::type_index(typeid(T)) );
        if( it==typeMap.end())
            launch_exception<ExBuildWhat>(); // TYPE NOT REGISTERED

        if( it->second.single == true) ///!!!!!
            launch_exception<ExSingleMulti>(); // T must be multi

        auto it2 = callbacks.find( it->second.type ); //find constructor

        if( it2==callbacks.end() )
            launch_exception<ExNotWired>(); // CONSTRUCTOR NOT WIRED

        return std::move(
                    std::unique_ptr<T>(reinterpret_cast<T*>( (it2->second)(nullptr) ))
                    );
    }

    template <typename T>
    std::unique_ptr<T> Container::build(){
        return build_delegate<T>();
    }

} // namespace Infector

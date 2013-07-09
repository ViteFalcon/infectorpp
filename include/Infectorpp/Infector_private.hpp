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
#include <iostream>
namespace Infector{

    Container::~Container(){

    }

    template <typename T, typename... Dependencies>
    void Container::wire(){
        bool tests = reduced_type_tests<T>();
        (void) tests; //fix unused variable warning

        auto it2 = callbacks.find(std::type_index(typeid(T)));
        if( it2!=callbacks.end())
            launch_exception<ExWireWhat>();

        // THROW... ok exception safe! Indipendently of parameters order
        RecursionLimit * plimit = & limit;
        callbacks[std::type_index(typeid(T))] =
                    [this,plimit] () {
                        (*plimit).increment();
                        return reinterpret_cast<void*>(
                            new T(UniqueOrShared<Dependencies>(this)...)
                        );
                    };
    }

} // namespace Infector

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
AUTHORS OR */

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
        if( it==typeMap.end()) //"it" is concrete type, T's abstract (or concrete) one
            launch_exception<Something>(); // TYPE NOT REGISTERED

        if( it->second.single == true) ///!!!!!
            launch_exception<Something>(); // T is single but istantiated as multi
        //at this point may happen that concrete type and abstract ones are the
        //same.that's ok. (bindAsNothing)
        auto it2 = callbacks.find( it->second.type ); //find constructor for concrete type

        if( it2==callbacks.end() )
            launch_exception<Something>(); // CONSTRUCTOR NOT WIRED

        return std::unique_ptr<T>(reinterpret_cast<T*>( (it2->second)() )); // MAY THROW.. ok!
    }

    template <typename T>
    std::unique_ptr<T> Container::build(){
        limit.reset(); //need to cally only once, or in case of exception.
        return build_delegate<T>();
    }

} // namespace Infector

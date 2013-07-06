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
    bool Container::resolve_multiple_inheritance(){
        auto it = typeMap.find(   std::type_index(typeid(Contract)) );
        if( it!=typeMap.end())
            return false;

        typeMap[std::type_index(typeid(Contract))]
                = Binding(std::type_index(typeid(T)), true);
        return true;
    }

    template <typename T, typename Contract, typename... Others>
    bool Container::resolve_multiple_inheritance(){
        auto it = typeMap.find(  std::type_index(typeid(Contract)) );
        if( it!=typeMap.end())
            return false;

        if(resolve_multiple_inheritance<T,Others...>()){
            typeMap[std::type_index(typeid(Contract))]
                    = Binding(std::type_index(typeid(T)), true);
            return true;
        }

        return false;
    }

    template <typename T, typename Contract >
    void Container::rollback_multiple_inheritance(){
        auto it = typeMap.find( std::type_index(typeid(Contract)) );
        if( it==typeMap.end())
            return;

        typeMap.erase(it);
    }

    template <typename T, typename Contract, typename... Others>
    void Container::rollback_multiple_inheritance(){
        auto it = typeMap.find( std::type_index(typeid(Contract)) );
        if( it==typeMap.end())
            return;

        rollback_multiple_inheritance<T,Others...>();

        typeMap.erase(it); // all other iterators will still be valid.
    }

    template <typename T>
    void  Container::launch_exception(){
        limit.reset(); //basic rollback semantics
        throw T();
    }

    template <typename T>
    void Container::must_have(){
        auto it = typeMap.find( std::type_index(typeid(T)) );
        if( it==typeMap.end())
            launch_exception<ExMissingType>();
    }

    template <typename T>
    void Container::must_not_have(){
        auto it = typeMap.find( std::type_index(typeid(T)) );
        if( it!=typeMap.end())
            launch_exception<ExExistingType>();
    }

} // namespace Infector

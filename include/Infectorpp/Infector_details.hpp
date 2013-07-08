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
#include <cassert>
#include <iostream>
namespace Infector{

    template <typename Contract >
    bool Container::resolve_multiple_inheritance_inner(std::type_index T){
        auto it = typeMap.find(   std::type_index(typeid(Contract)) );
        if( it!=typeMap.end())
            return false;
        std::cout<<"registered "<<T.name()<<" as "<<typeid(Contract).name()<<std::endl;
        typeMap[std::type_index(typeid(Contract))]
                = Binding(T, true);
        return true;
    }

    template <typename Unused, typename Contract, typename... Others>
    bool Container::resolve_multiple_inheritance_inner(std::type_index T){

        if((typeid(Contract)!=typeid(Unused))||sizeof...(Others)>0){
            auto it = typeMap.find(  std::type_index(typeid(Contract)) );
            if( it!=typeMap.end())
                return false;

            if(resolve_multiple_inheritance_inner<Contract,Others...>(T)){
                std::cout<<"registered "<<T.name()<<" as "<<typeid(Contract).name()<<std::endl;
                typeMap[std::type_index(typeid(Contract))]
                        = Binding(T, true);
                return true;
            }
        }else
            return resolve_multiple_inheritance_inner<Contract>(T);

        return false;
    }

    template <typename T, typename Contract, typename... Others>
    bool Container::resolve_multiple_inheritance(){
        return resolve_multiple_inheritance_inner
            <ANotUsableClass,Contract,Others...>(std::type_index(typeid(T)));
    }

    template <typename Contract >
    void Container::rollback_multiple_inheritance_inner(){
        auto it = typeMap.find( std::type_index(typeid(Contract)) );
        if( it==typeMap.end())
            return;

        typeMap.erase(it);
    }
    template <typename Unused, typename Contract, typename... Others>
    void Container::rollback_multiple_inheritance_inner(){
        if((typeid(Contract)!=typeid(Unused))||sizeof...(Others)>0){
            auto it = typeMap.find( std::type_index(typeid(Contract)) );
            if( it==typeMap.end())
                return;

            rollback_multiple_inheritance_inner<Contract,Others...>();

            typeMap.erase(it); // all other iterators will still be valid.
        }else
            rollback_multiple_inheritance_inner<Contract>();
    }
    template <typename T, typename Contract, typename... Others>
    void Container::rollback_multiple_inheritance(){
        return rollback_multiple_inheritance_inner
                <ANotUsableClass,Contract,Others...>();
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

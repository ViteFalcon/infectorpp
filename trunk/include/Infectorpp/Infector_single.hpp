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
#include <stdexcept>
namespace Infector{

    template <typename T>
    void Container::bindSingleAsNothing(){
        bool tests = reduced_type_tests<T>();
        (void) tests; //fix unused variable warning
        must_not_have<T>();

        typeMap[std::type_index(typeid(T))]
                = Binding(std::type_index(typeid(T)), true);

        try{
            singleIstances[std::type_index(typeid(T))] = nullptr;
        }catch(std::exception & ex){
            auto it2 = typeMap.find( std::type_index(typeid(T)) );
            typeMap.erase(it2);
            launch_exception<Something>();
        }
    }

    template <typename T, typename... Contracts>
    void Container::bindSingleAs(){
        bool tests = type_tests<T,Contracts...>(); // Compile time tests.
        (void) tests; //Fix GCC warning

        bool success = false;
        try{
            success = resolve_multiple_inheritance<T,Contracts...>();
        }catch(std::exception & ex){
            rollback_multiple_inheritance<T,Contracts...>(); //revert changes
            launch_exception<Something>();
        }
        if(!success) //no changes.
            launch_exception<Something>();

        try{
            singleIstances[std::type_index(typeid(T))] = nullptr;
        }catch(std::exception & ex){
            rollback_multiple_inheritance<T,Contracts...>(); //revert changes
            launch_exception<Something>();
        }
    }

    template <typename T>
    std::shared_ptr<T> Container::buildSingle_delegate(){
        auto it = typeMap.find( std::type_index(typeid(T)) );
        if( it==typeMap.end()) //"it" is concrete type, T abstract one
            launch_exception<Something>(); // TYPE NOT REGISTERED

        if( it->second.single == false)
            launch_exception<Something>(); // T is multi but istantiated as single

        //at this point may happen that concrete type and abstract ones are the
        //same.that's ok. (bindSingleAsNothing)
        auto it2 = callbacks.find(  it->second.type ); //find constructor for concrete type

        if( it2==callbacks.end())
            launch_exception<Something>(); // CONSTRUCTOR NOT WIRED

        auto it3 = singleIstances.find( std::type_index(typeid(T)) );
        if(it3==singleIstances.end())
             launch_exception<Something>(); // ANY SHARED NOT CREATED?? (Infector bug then)

        if (it3->second == nullptr){
            std::unique_ptr<AnyShared<T>> any = nullptr;
            try{
                any = std::move(new AnyShared<T>());
                any->ist = reinterpret_cast<T*>( (it2->second)() ); // MAY THROW
                singleIstances[std::type_index(typeid(T))] = std::move(any); //MAY THROW
            }
            catch(std::exception & ex){
                auto ft = singleIstances.find(std::type_index(typeid(T)));
                if(ft!=singleIstances.end())
                    singleIstances.erase(ft);
                launch_exception<Something>();
            }
            return any->ist;
        }

        AnyShared<T> * any = reinterpret_cast<AnyShared<T>>((*it3));
        return any->ist;
    }

    template <typename T>
    std::shared_ptr<T> Container::buildSingle(){
        limit.reset(); //need to cally only once, or in case of exception.
        return buildSingle_delegate<T>();
    }

} // namespace Infector

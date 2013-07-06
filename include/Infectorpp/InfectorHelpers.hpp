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
#include <type_traits>
#include <memory>
#include "InfectorExceptions.hpp"

namespace Infector{

    template<typename... Others>
    struct recursiveTest;

    template<typename TEST>
    struct recursiveTest<TEST> : std::integral_constant<bool, TEST::value> { };

    template<typename TEST, typename... Others>
    struct recursiveTest<TEST, Others...> : std::integral_constant<
        bool, TEST::value && recursiveTest<Others...>::value> { };

    /** Type test, there are few type traits that are checked, you can add
    *   more tests if you need to do so. */
    template <typename T, typename... Contracts>
    bool type_tests(){

        static_assert(  sizeof...(Contracts)>0 //if no contracts don't use "As"
                      , " There must be at least 1 interface ");

        static_assert(  recursiveTest<std::is_abstract<Contracts>...>()
                      , " Contracts have to be abstract");

        static_assert(  recursiveTest<std::is_base_of<Contracts,T>...>()
                      , " Contracts must be base classes for T");

        static_assert(  std::is_destructible<T>::value
                      , " T must be destructible");

        return true;
    }

    template <typename T>
    bool reduced_type_tests(){

        static_assert(  std::is_destructible<T>::value
                      , " T must be destructible");

        return true;
    }

    class IAnyShared{

    public:
        virtual void nothing()=0;
        virtual ~IAnyShared() {
            static_assert(  std::is_abstract<IAnyShared>::value
                      , "IAnyShared must be abstract"); //strange.. a virtual method is required
        }
    };

    /** Allows to have containers with different shared_ptr to different types. */
    template <typename T>
    struct AnyShared: public virtual IAnyShared{
        virtual void nothing(){}
        std::shared_ptr<T> ist;
    };

    class RecursionLimit{
        static constexpr int max_depth = 20; //if you need to do so increase this limit to your will.
        int current_depth = 0;
    public:
        void increment(){
            current_depth++;
            if(current_depth>max_depth)
                throw ExRecursionLimit();
        }

        void reset(){
            current_depth = 0;
        }
    };

} // namespace Infector

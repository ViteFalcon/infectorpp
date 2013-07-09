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
#include <type_traits>
#include <memory>
#include <cassert>
#include "InfectorExceptions.hpp"

namespace Infector{

    class ANotUsableClass{
        ANotUsableClass()=delete;
        ~ANotUsableClass()=delete;
    };

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
        virtual void* getPtr()=0;
        virtual void setPtr(void *)=0; // to be called only once. Set managed object
        virtual std::shared_ptr<int> getReferenceCounter()=0; //pass it to alias constructor
                                                         //(this one is aliased too).
        virtual ~IAnyShared(){}
    };

    /** Allows to have containers with different shared_ptr to different types. */
    template <typename T>
    class AnyShared: public virtual IAnyShared{
        int a=49374; // just a number
        std::shared_ptr<T> ist;
    public:
        virtual void setPtr(void * p) override {
            assert(ist==nullptr);
            // FUNZIONA MA NON CHIAMA IL DISTRUTTORE DI T!
            ist.reset(static_cast<T*>(p));
        }
        virtual void* getPtr() override {
            return ist.get();
        }
        virtual std::shared_ptr<int> getReferenceCounter(){
            return std::shared_ptr<int>(ist,&a);
        }
        virtual ~AnyShared(){}
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

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
#include <typeindex>
#include <unordered_map>
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
        virtual void* getPtr(std::type_index idx)=0; //take typeid
        virtual void setPtr(void *)=0; // to be called only once.
        virtual std::shared_ptr<int> getReferenceCounter()=0;
        virtual ~IAnyShared(){}
    };

    /** T is the managed type, but we need pointers to Interfaces,
    *   so there's the need to share reference counter since
    *   smart pointers created in the usual way will not have
    *   the same reference counter if they have different types. */
    template <typename T, typename... Bases> //base classes variadic
    class AnyShared: public virtual IAnyShared{
        int a=49374; // just a number
        std::shared_ptr<T> ist;
        std::unordered_map<std::type_index, void*> multi_solver;

        //omg I need to do that.. again :(
        template<typename Base1>
        void resolve_multiple_cast(T*tp){
            Base1* base = tp;
            multi_solver[std::type_index(typeid(Base1))]
                          = reinterpret_cast<void*>(base);
        }
        template<typename Base1, typename NextBase, typename... Others>
        void resolve_multiple_cast(T*tp){
            Base1* base  = tp;
            multi_solver[std::type_index(typeid(Base1))]
                          = reinterpret_cast<void*>(base);
            resolve_multiple_cast<NextBase,Others...>(tp);
        }

    public:
        virtual void setPtr(void * p) override {
            T* tp = reinterpret_cast<T*>(p);
            try{
                ist.reset(tp);
                resolve_multiple_cast<T,Bases...>(tp);
            }catch(std::exception & ex){
                multi_solver.clear();
                throw ex;
            }
        }
        virtual void* getPtr(std::type_index idx) override {
            return multi_solver[idx];
        }
        virtual std::shared_ptr<int> getReferenceCounter()override{
            return std::shared_ptr<int>(ist,&a);
        }
        virtual ~AnyShared(){}
    };

    class RecursionLimit{
         //You can raise this limit if needed
        static constexpr int max_depth = 20;
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

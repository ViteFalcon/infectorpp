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

#include "InfectorExceptions.hpp"
#include "InfectorExport.hpp"
#include "InfectorHelpers.hpp"
#include <list>

namespace Infector{

/** This class is a exception safe (STRONG GUARANTEE) Inversion of Control
*   Container for Dependency Injection.
*   Multiple injection is not allowed nor contextual binding. Each given
*   interface can have at most 1 implementation. Given a interface a
*   implementation is uniquely determined. It is possible to register
*   concreted classes and for each concrete class/implementation
*   you can do at most 1 wiring to 1 constructor. */
class INFECTOR_EXPORT Container{

public:

    /// POLYMORPHIC BINDING

    /** Declare a type as implementation of an interface(Contract). This type
    *   will be injected using a "std::unique_ptr". Every object depending on
    *   T will have its own istance of T.*/
    template <typename T, typename Contract >
    void bindAs();

    /** Declare a type as implementation of multiple interfaces(Contracts).
    *   This type will be injected using a "std::shared_ptr", only 1 istance of
    *   T will be created and shared. */
    template <typename T, typename... Contracts>
    void bindSingleAs();

    /// CONCRETE BINDING

    /** Declare a type to be used without abstraction. This type will be
    *   injected using a "std::unique_ptr". Every object depending on T will
    *   have its own istance of T.*/
    template <typename T>
    void bindAsNothing();

    /** Declare a type to be used without abstraction. This type will be
    *   injected using a "std::shared_ptr", only 1 istance of T will be created
    *   and shared. */
    template <typename T>
    void bindSingleAsNothing();

    /// WIRING

    /** Wires a type and its constructor to its dependencies. Automatically
    *   deduces if a unique_ptr or a shared_ptr have to be used when injecting
    *   a dependency. If no dependencies, the default constructor is wired.*/
    template <typename T, typename... Dependencies>
    void wire();

    /// FACTORY METHODS

    /** Creates or get the only istance of T. T can be an Interface as long as
    *   it was bound to its concrete type, and that concrete type was wired.
    *   T is not created until is needed.*/
    template <typename T>
    std::shared_ptr<T> buildSingle();

    /** Istantiates a istance of T. T can be an Interface as long as it was
    *   bound to its concrete type, and that concrete type was wired. Every
    *   call will istantiate a different object.*/
    template <typename T>
    std::unique_ptr<T> build();

    inline ~Container();

private:

    template <typename Contract >
    bool resolve_multiple_inheritance_inner(std::type_index T);
    template <typename Contract, typename Next, typename... Others>
    bool resolve_multiple_inheritance_inner(std::type_index T);
    template <typename T, typename Contract, typename... Others>
    bool resolve_multiple_inheritance();

    template <typename Contract >
    void rollback_multiple_inheritance_inner();
    template <typename Contract, typename Next, typename... Others>
    void rollback_multiple_inheritance_inner();
    template <typename T, typename Contract, typename... Others>
    void rollback_multiple_inheritance();

    template <typename T>  void must_have();
    template <typename T>  void must_not_have();
    template <typename T>  void launch_exception();
    template <typename T>  std::shared_ptr<T> buildSingle_delegate();
    template <typename T>  std::unique_ptr<T> build_delegate();

    template<typename Dummy>
    bool tryToGetSize( RecursionLimit *, int *);
    template<typename Actual, typename Next, typename...Others>
    bool tryToGetSize( RecursionLimit *, int *);

    template<typename Dummy=DummyClass> // fix for bug reported by Michael Bischof
    void setKnownSizeForType( std::type_index t, bool known, int size);
    template<typename Dummy=DummyClass> // fix for bug reported by Michael Bischof
    void processRecursionWeb();

    template <typename OBJ> friend class UniqueOrShared;
    struct EmplaceContext;
    /** Too long explain why this is so usefull.*/
    template <typename OBJ>
    class UniqueOrShared{
        Container * ioc=nullptr;
        EmplaceContext * context;
    public:
        UniqueOrShared(Container * ptr, EmplaceContext * c)
        :ioc(ptr)
        ,context(c){}

        operator std::shared_ptr<OBJ>(){
            return std::shared_ptr<OBJ>(ioc->buildSingle_delegate<OBJ>());
        }

        operator std::unique_ptr<OBJ>(){
            return std::unique_ptr<OBJ>((ioc->build_delegate<OBJ>()));
        }
    };

    struct Binding{
        std::type_index type;
        bool            single;
        Binding():type( typeid(Binding)){}
        Binding( std::type_index a, bool b): type(a),single(b){}
        Binding( const Binding & other) = default;
    };

    struct EmplaceContext{
        bool factory;
    };

    std::unordered_map< std::type_index, IAnyShared*>       singleIstances;
    std::unordered_map< std::type_index, Binding>           typeMap;
    std::unordered_map< std::type_index, std::function<void*(EmplaceContext*)>>
                                                            callbacks;

    std::unordered_map< std::type_index, int>               typeInfoMap;
    std::unordered_map< std::type_index,
                        std::function< bool( RecursionLimit *, int *)>>
                                                            recursionMap;
    std::list< std::function< bool( RecursionLimit *, int *)>>
                                                            recursionWaitList;

    //RecursionLimit limit;
};
} // namespace Infector

#include "Infector_details.hpp"
#include "Infector_private.hpp"
#include "Infector_multi.hpp"
#include "Infector_single.hpp"

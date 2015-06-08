# Introduction #

Infector is a Inversion of Control container for doing dependency injection: (see [Wikipedia](http://en.wikipedia.org/wiki/Inversion_of_control)) It takes advantage of the new C++11 language features to provide a reliable and easy to use IoC container. In addition to most common features provided by IoC Containers, there are additional features that are unique to C++ (mostly because other languages don't need that features: C++ is not managed).

## Constructor Injection ##

Infector++ does only Construtor Injection (no setter injection: [see related problems](http://misko.hevery.com/2009/02/19/constructor-injection-vs-setter-injection/))

## Exception safety & leak prevention ##

Infector is also a nice solution for removing memory leaks:
  * Is not possible having circular references (typical problem caused by bad usage of [shared\_ptr](http://en.cppreference.com/w/cpp/memory/shared_ptr))
  * All dependencies are "managed" thanks to smart pointers usage, you don't have to call "new" or "delete".
  * If any method of Infector::Container throws an exception the Container will be left unchanged (strong guarantee)
  * If any of your constructors throws an exception, Infector will not cause memory leaks.

(Memory allocated by you can still cause memory leaks, the point is that you will almost never have to manually allocate something with "new", the same applies to external libraries, Infector cannot prevent in any way third party codes to creates memory leaks.).

## Future releases ##

Some extra features will come with next releases

  * Nested containers ( This is a big "?". This will be added only if there's a way to make it hard to misuse)
  * Custom allocation:
    1. Infector knows how your objects are wired up, and can exploit that to reduce significatively the number of allocations and at same time can improve cache locality.(Infector++ was not originally conceived with performance in mind. But after some tests, it was found that the compiler does a good job at optimizing all templated stuff)
    1. Set custom allocator provided by user: since Infector already batch allocations where needed, you can focus on other optimizations.
  * Report unused dependencies: If some class was not used you'll know that, and you will be able to remove dead code from your project, this will be partially provided by Nested Containers and by extra code in Containers' destructors.



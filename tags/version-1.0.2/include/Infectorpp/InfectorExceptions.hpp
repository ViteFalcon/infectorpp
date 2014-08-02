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
#include <stdexcept>

#ifndef _MSC_VER // Visual studio 2013 still does not support "noexcept"
                 // need a workaround thanks to  wtravisjones for bug report.
    #define NOEXCEPT noexcept(true)
#else
    #define NOEXCEPT
#endif

namespace Infector{

    class ExRecursionLimit: public std::exception{
        char message[69] =
        "Reached recursion limit. Probable cause: circular dependency";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExBuildWhat: public std::exception{
        char message[69] =
        "Cannot build not 'bind'ed types.";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExWireAgain: public std::exception{
        char message[69] =
        "Only 1 constructor allowed for each concrete type. Wiring again?";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExNotWired: public std::exception{
        char message[69] =
        "Cannot build a type if it is not wired";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExSingleMulti: public std::exception{
        char message[69] =
        "shared_ptr<T> cannot be used in place of unique_ptr<T> and viceversa";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExMissingType: public std::exception{
        char message[69] =
        "Requested type is missing, you have to 'bind' it first";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExAnySharedNullPtr: public std::exception{
        char message[69] =
        "AnyShared<...> returned a null pointer";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExExistingType: public std::exception{
        char message[69] =
        "This type was already 'bind'ed.";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExExistingInterface: public std::exception{
        char message[69] =
        "Cannot associate 2 concrete types to the same interface.";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

    class ExInternalError: public std::exception{
        char message[69] =
        "Infector critical error.";
    public:
        virtual const char* what() const NOEXCEPT{
            return message;
        }
    };

} // namespace Infector

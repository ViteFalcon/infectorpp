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

//----------------------------------------------------------------------------
/**
    This file is used as test suite. Compiling and running it takes few
    moments and can istantly detect all possible problems. Usefull for not
    breaking existing functionalities and for prove that everything is
    correct.
*/

#include <InfectorContainer.hpp>
#include <memory>
#include <iostream>
#include <cassert>

/// ////////////////////////////////////////////////
///                  TEST
///                    1
/// ////////////////////////////////////////////////

struct BadClass{
    BadClass(){
        std::cout<<"Bad Ctor"<<std::endl; //DISPLAY CONSTRUCTOR CALL
        throw 1;}
};
typedef std::unique_ptr<BadClass> bad_ptr;

struct GoodClass{
    static int counter;
    GoodClass(){
        std::cout<<"Good Ctor"<<std::endl; // DISPLAY CONSTRUCTOR CALL
        counter++;}
    ~GoodClass(){counter--;}
};
int GoodClass::counter = 0;

typedef std::unique_ptr<GoodClass> good_ptr;

struct GB{
    bad_ptr  bc;
    good_ptr gc;

    GB(good_ptr good, bad_ptr bad):
        bc(std::move(bad)),gc(std::move(good)){}
};


struct BG{
    bad_ptr  bc;
    good_ptr gc;

    BG(bad_ptr bad, good_ptr good):
        bc(std::move(bad)),gc(std::move(good)){ }
};

void LeakTest(){
    GoodClass::counter = 0;
    Infector::Container ioc;
    ioc.bindAsNothing<BadClass>();
    ioc.bindAsNothing<GoodClass>();
    ioc.bindAsNothing<GB>();
    ioc.bindAsNothing<BG>();
    ioc.wire<BadClass>();
    ioc.wire<GoodClass>();
    // C++ make no assumption about evaluation order of parameters
    // so I must test both orders.. and there's no guarantee that
    // evaluation order will be the same between 2 calls, so manual
    // attention must be paid here.
    ioc.wire<BG       ,BadClass,GoodClass>();
    ioc.wire<GB       ,GoodClass,BadClass>();
    std::cout<<"\n3 CONSTRUCTORS CALLS MUST BE DISPLAYED:\n"<<std::endl;
    try{
    auto a = ioc.build<BG>();
    }catch(int a){

    }
    std::cout<<"2nd call"<<std::endl;
    try{
    auto b = ioc.build<GB>();
    }catch(int a){

    }
    std::cout<<"\nend of test 1\n"<<std::endl;
    assert(GoodClass::counter == 0); // TEST MEMORY LEAK
    GoodClass::counter = 0;
}

/// ////////////////////////////////////////////////
///                  TEST
///                    2
/// ////////////////////////////////////////////////


class IBed{
public:
    virtual void sleep() = 0;
};

class IRoom{
public:
    virtual void interact() = 0;
};

class ComfortableBed: public virtual IBed{
public:
    ComfortableBed(){}

    virtual void sleep(){
        std::cout<<"yumm so comfortable!"<<std::endl;
    }
};

class BedRoom:public virtual IRoom{

    std::unique_ptr<IBed> myBed;

public:
    BedRoom(std::unique_ptr<IBed> b):myBed(std::move(b)){
    }

    virtual void interact(){
        std::cout<<"interacting with room:"<<std::endl;
        myBed->sleep();
    }
};


void BedTest(){
    std::cout<<"ROOM AND COMFORTABLE BED\n"<<std::endl;
    Infector::Container ioc;

    ioc.bindAs<ComfortableBed,  IBed>();
    ioc.bindAs<BedRoom,         IRoom>();

    ioc.wire<ComfortableBed>();
    ioc.wire<BedRoom,           IBed>();

    auto room = ioc.build<IRoom>();
    room->interact();

    std::cout<<"\nend of test 2\n"<<std::endl;
}

/// ////////////////////////////////////////////////
///                  TEST
///                    3
/// ////////////////////////////////////////////////
struct Booom;
struct Kaaa{
public:
    Kaaa(std::unique_ptr<Booom> b){ (void) b;}
};


struct Booom{
public:
    Booom(std::unique_ptr<Kaaa> k){ (void) k;}
};

void CircularTest(){
    std::cout<<"CIRCULAR DEPENDENCY TEST\n"<<std::endl;
    Infector::Container ioc;
    ioc.bindAsNothing<Kaaa>();
    ioc.bindAsNothing<Booom>();
    ioc.wire<Kaaa,Booom>();
    ioc.wire<Booom,Kaaa>();

    try{
        auto kaboom = ioc.build<Kaaa>();
    }
    catch(std::exception &ex){
        std::cout<<ex.what()<<std::endl;
    }

    std::cout<<"\nend of test 3\n"<<std::endl;
}

/// ////////////////////////////////////////////////
///                  TEST
///                    4
/// ////////////////////////////////////////////////

class IFoo {
public:
    virtual void letsFoo() = 0;
};

class IBar {
public:
    virtual void letsBar() = 0;
};

class CFooBar: public virtual IFoo, public virtual IBar{
public:
    CFooBar() = default;
    virtual ~CFooBar() = default;

    virtual void letsBar() override{
        std::cout<<"CFooBar : letsBar"<<std::endl;
    }
    virtual void letsFoo() override{
        std::cout<<"CFooBar : letsFoo"<<std::endl;
    }
};

class FooBarUser{
    std::shared_ptr<IFoo> myFoo;
    std::shared_ptr<IBar> myBar;
public:
    FooBarUser(std::shared_ptr<IFoo> foo, std::shared_ptr<IBar> bar)
                : myFoo(foo)
                , myBar(bar)
    {  }

    void doSomething(){
        myFoo->letsFoo();
        myBar->letsBar();
    }
};

void SharedTest(){
    std::cout<<"SHARED OBJECTS TEST\n"<<std::endl;
    Infector::Container ioc;
    ioc.bindSingleAs<CFooBar, IFoo, IBar>();
    ioc.bindSingleAsNothing<FooBarUser>();

    ioc.wire<CFooBar>();
    ioc.wire<FooBarUser, IFoo, IBar>();

    auto user = ioc.build<FooBarUser>();
    user->doSomething();
}

int main(){
    /** Test to prove that Infector does not cause memory leaks due to
    *   unkown evaluation order of constructors' parameters when construcors
    *   throws exceptions. This is a big advantage.*/
    LeakTest();
    /** Basic usage test, no shared objects. Creates a small object graph
    *   with unique ownership semantics.*/
    BedTest();
    /** This test assure that exception is thrown in case there's a circular
    *   dependency. */
    CircularTest();
    /** Basic usage test this time shared objects are used and multiple
    *   inheritance is tested.*/
    SharedTest();

    return 0;
}

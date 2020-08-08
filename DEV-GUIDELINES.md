# Development Guidelines

This page provides information on how to successfully contribute to the Coherence C++ extend client. Coding conventions are stylistic in nature and the Coherence style is different to many open source projects therefore we understand the raising of eyebrows. However, consistency is significantly more important than the adopted subjective style, therefore please conform to the following rules as it has a direct impact on review times.


## Contents
1. [Coding Guidelines](#intro)
    1. [File Layout and Structure](#1)
    2. [Include Rules](#2)
    3. [Comment Rules](#3)
    4. [Static Constantc and Enums](#4)
    5. [Static data](#5)
    6. [Class conventions](#6)
    7. [Interface conventions](#7)
    8. [Inheritance conventions](#8)
    9. [Memory Management](#9)
    10. [General conventions](#10)
    11. [Finally blocks](#11)
    12. [Indentation conventions](#12)
    13. [Variable declarations and assignments](#13)
    14. [Types](#14)
    15. [Variable prefix conventions](#15)
    16. [Const correctness](#16)
    17. [Exceptions](#17)
    18. [Inner Classes](#18)
    19. [End of file conventions](#19)
    20. [Function implementation conventions](#20)
    21. [Object Model](#21)
    22. [Thread Safety](#22)
    23. [Porting TDE Components to C++ Managed objects](#23)
    24. [Porting Java casts and instanceof checks](#24)
    25. [Right Shift operators](#25)
1. [Tools](#tools)


# <a name="intro"></a>Coding Guidelines

The rules below must be adhered to with exceptions being specifically called out to the reviewer. Exceptions to the below rules are exactly that, therefore if an exception is to be presented it must have a compelling justification.

## <a name="1"></a>File Layout and Structure

C++ source code is broken into header and implementation files:

* All header files end in the .hpp suffix.
* All implementation files end in the .cpp suffix.

### Layout structure

* All files end at line 78.
* Each class declaration and defiition must be broken up into logical sections. Each section should have the following delimiter:
  * Two blank lines
  * `// ----- section title --------(out to column 78)`
  * Blank line.
  * visibility modifier for the section:

```c++
// <previous section should have two blank lines before the next section>


  // ----- section title --------------------------------------(out to column 78)

 public:
 /**
 * First method of the new section (notice no blank line after public:)
 */
```

* Classes will generally have the following sections which should be in order as listed (in both .hpp and .cpp files):
 * handle definitions
 * factory methods
 * constructors/destructor
 * this interface (methods added by this class)
 * parent classes interfaces
 * Object interface
 * data member accessors
   * getters and setters are paired, getter before setter
 * inner classes
 * constants
 * data members

## <a name="2"></a>Include rules

* All include files must have standard include header guards.
  * The header guard should be the name of the class prefixed with COH and the CamelCase of the name should be switched to use _ (`CAMEL_CASE`).
  * The `#endif` for the header guard should have a comment with the header guard name in it
  * `coherence/lang.ns` includes all public lang header files. With the exception of the lang package, all header files should include `lang.ns` as the first file in the includes
* All header files must explicitly include the class headers they rely upon, with the exception of `lang.ns`.
  * Note: Header files in the lang package must explicitly include the headers in lang they rely upon. **They cannot include lang.ns**
* All namespace nested under the "coherence" namespace automatically pick up a `using namespace coherence::lang` when they include `lang.ns`. This is done to mimic the automatic using of java.lang in Java. This pattern will not be followed for any other namespaces.
  * No other namespaces, should be brought in via the "`using namespace`" statement. Rather, explicit "`using class`" statements must be used.
  * Two spaces after the last using statement before a class definition.
* Include section rules:
  * All includes and using statements must be in alphabetical order.
    * Note: `lang.ns` or `compatibility.hpp` should be included first regardless of alphabetical order, and be in its own section (i.e. followed by a blank line).
    * Note: In the `.cpp` file, the corresponding header file should be included first regardless of alphabetical order, and be in its own section (i.e. followed by a blank line).
  * public and private includes should be in separate sections.
* All classes should be exported via the `COH_EXPORT` macro

```c++
#ifndef COH_MY_CLASS_NAME_HPP
#define COH_MY_CLASS_NAME_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,foo)


class COH_EXPORT MyClass
    {
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MY_CLASS_NAME_HPP
```

## <a name="3"></a>Comment rules

*   Comments (and source code) should not exceed past the 78 column point within reason.
*   Each class, method and data member must be preceded by a JavaDoc-style comment in the .hpp file.
    *   Class and method docs should describe functionality and not implementation details.
    *   If a class method is synchronized there should be a note specifying so in the method comments.
    *   Data member docs should describe what the data member holds or is used for.
    *   Comment descriptions should be full sentences ending in a period.
    *   Parameter descriptions should be two spaces after the parameter name and all be aligned after the longest parameter name. **All parameters must be documented**
    *   return statements should not start in a capital letter.
    *   The '\*' at the beginning of each comment line should be found in the first spot directly under the '/'
    *   Comment structure should consist of:
        *   Description of the method class or data member
        *   blank line
        *   parameters
        *   return statement
*   Doxygen porting rules:
    *   Don't use html tags that end on their own (`<p/>` for example)
    *   Make sure your `@param` variables match the ones in the method
    *   Any reference to `com.tangosol.<java_class>` must be updated to match a C++ class
    *   don't use `@link`, doxygen will figure out the links.

```c++
/**
* Return a new String comprised of the substring of vsSource
* from iBegin (inclusive) to iEnd (exclusive).
*
* Note: This method is synchronized.
*
* @param vsSource  the string to create a substring from
* @param iBegin    the starting index from which to create the string
* @param iEnd      the index of where the substring should stop
*                  in vsString or -1 for end of string
*
* @return the new substring created from
*/
```

## <a name="4"></a>Static Constants and Enums

Static consts, and enum values must be named with an all lower case name. This deviates from the traditional all caps style, but was chosen to try to avoid name collisions with Microsoft's macro names, which don't follow best practices of adding a macro name prefix. Note that Coherence macros are the only thing expressed in all upper case, and they must be `COH_` prefixed.

## <a name="5"></a>Static data

Due to the static initialization order [fiasco](http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.12) the coherence C++ API will make no use of static variables including:

* non-const static data members
  * const static data members **must** be declared in the header
* file local static variables
* function local statics (see exception below)

For non integral types such as Handles, a function local static can be used to circumvent the fiasco. The solution also requires the use of `COH_STATIC_INIT` to ensure thread safety:

* Bad
  * header
```c++
        static const String::Handle EMPTY_STRING; // UNSAFE
```
  * .cpp
```c++
        String::Handle foo::EMPTY_STRING = ""; // UNSAFE
```
* Good
  * .cpp
```c++
        static String::Handle getEmptyString()
            {
            static String::Handle hEmpty = "";
            return hEmpty;
            }  
        COH_STATIC_INIT(getEmptyString());
```

## <a name="6"></a>Class conventions

* Classes should be implemented as managed objects by virtually inheriting from `coherence::lang::Object`
  * if non-managed objects are created, they should only be stack allocated (no pointers please)
* All methods should be declared virtual. Even if virtual is implied (inherited) it must still be written explicitly.
* Where possible use default parameters to avoid multiple method implementations.
* All classes must have:
  * Protected copy constructor.
    * The parameter name should be that.
  * Protected constructor beyond the copy constructor - make a default constructor if you have no others.
    * In the Coherence Object model the copy constructor is coupled with the clone() implementation (see below), and if implemented must be a deep copy
    * If class will not support clone() then it will not support the copy constructor. In this case the copy constructor should be declared private and have no implementation.
  * Protected virtual destructor.
  * No assignment operators.
  * Static create method(s) that proxy protected constructors
  * Handle/View typedefs (see Object model discussion below)
  * Reasonable toString implementation
  * Real clone implementation (can simply call the copy constructor)
    * As stated in the documentation for clone() on Object, if implemented clone() must return a deep copy such that any changes to the original object will not be visible from the cloned object. Shallow clone() implementations are not allowed, if needed it should be provided via a seperate method, for example an additional create() method.
    * By convention clone() is implemented by simply delegating to the classes copy constructor. This means that the copy constructor must be implemented to perform a deep copy.
    * In the rare occasion that an object should not be cloned it should not even declare the clone() method in its header, and simply use the implementation provided by AbstractObject. This default implementation throws CloneNotSupportedException.
  * For thread safety class data members in general should either be MemberHandle's or MemberViews. In the rare cases where you have a const reference you can use a const Handle or View instead.
  * For thread safety, writing to a data member requires synchronization or a field level write lock.
* Any enum in a header file must have the values explicitly set.

## <a name="7"></a>Interface conventions

* All interfaces must have:
  * virtual inheritance from `coherence::lang::Object`
  * Handle/View typedefs (see Object model discussion below).
  * Protected empty (i.e {}) virtual destructor.
    * Note the protected virtual destructors serve two purposes: To ensure that deleting all objects is restricted, as well as to guarantee that destructors are called in the proper order as classes are extended.
  * Pure virtual methods (i.e. = 0 suffix)
  * Marker interfaces (i.e. interfaces without any methods) must additionally have a protected empty default constructor and protected empty copy constructor.

## <a name="8"></a>Inheritance conventions

The following rules apply to how objects should inherit from base classes:

* Always inherit Object virtually.
* Always inherit interfaces virtually (interfaces are pure virtual and must have an empty constructor).
* Avoid virtual inheritance except for the above two use cases.

## <a name="9"></a>Memory Management

* The Coherence Object model should remove the need for manual memory management in almost all cases.
* pointers and C++ references (including pass by reference) should be avoided
  * even when it would be more efficient, more C++ like, and safe to do use them, they should still be avoided
  * the coherence::lang package does however make use of these features to build up the object model
* WeakHandle/WeakView must be used when objects have a bidirectional relationship
  * by convention the parent holds a strong link to the child, and the child a weak to the parent
* From a language perspective it is generally not safe to store off a pointer to "this" during object construction. Related to that it is unsafe to create Handles/Views to an Object under construction, doing so will result in the Object being destroyed before the constructor returns. The only time this would not occur is if the created Handle/View to "this" outlived the constructor, but even then it is ill advised as the any operations which occurred though the Handle/View would be against a partially construction Object. The need to create a Handle/View to "this" during construction most often arrises due to calling helper methods which happen to use COH\_SYNCHRONIZEd (which creates a new View), or as part of some registration logic, where the new Object gets linked into a larger hierarchy. In any case creating Handles/Views during construction should absolutely be avoided. The Object model borrows from the TDE component model and includes support for a pair of onInit() event methods (one const one not). These methods are called automatically just after the constructor returns, this is done by Object.cpp itself, not the derived classes create methods. When onInit() is called there is already an attachment to the Object, and thus it is safe to create new ones, and also the Object is guaranteed to have been fully constructed. Any logic which relies on references to "this" should be moved into the onInit methods. The only exception to the "this" rule, is that MemberHandles/Views/Holders take \*this in their constructor, this is "safe" as they only rely on the base Object itself to have been initialized, and not the derived class. See the documentation in `Object.hpp` for more details on the usage of onInit().
* Destructors on managed Objects are very rare, as all data members should either be primitive or managed as well, so there is nothing to delete/free/release manually. In cases where destructor are needed, it is not allowable to create a handle to the destructing Object from within the destructor, any attempt to do so will result in a NULL handle.

## <a name="10"></a>General conventions

* Except for template classes, header files must not contain in-lined methods definitions
* The definition of "helper macros" in header files is discouraged, these should be defined in `utility.hpp`
* Method parameters and return values should be primitive or handle/views. In all cases they should be **passed by value**. Pass by reference will not be used, except where required by the language (i.e. copy constructor signature).
* The private visibility modifier **should not** be used, protected should be used instead.
* Data members should always be declared as protected.
* The **NULL** macro is to be used for all "null pointers". 0 should never be used.
* All data members should be prefixed with m\_
* `public const static` members need not be have a prefix and should be in all caps.
* if, while, and catch should have a space between the statement and the parenthesis (ex: `if ()`)

## <a name="11"></a>Finally blocks

C++ does not have finally blocks and as such, code must be duplicated to ensure that cleanup bits are executed properly. In order to simplify the way code looks a cleanup helper class called <methodname>Finally should be implemented.

```c++
namespace
    {
    struct CloseFinally
        {
        CloseFinally()
            : nSomeState(0)
            {
            }

        ~CloseFinally()
            {
            // cleanup code goes here
            if (NULL != vResource)
               {
               // resource specific cleanup
               vResource->free(nSomeState);
               }
            }

        Resource::View vResource;
        int            nSomeState;
        }
    }

void Foo::close()
    {
    // unprotected code ....

       {
       CloseFinally finally; // must be below the curly brace
       // protected code
       finally.nSomeState = 123;
       try // try/catch block not required, but included to show how to mimic Java try/catch/finally, note finally must be declared outside of try block
          {
          // critical point (update finally state)
          Resource::View vResource = acquireFromSomewhere();

          finally.vResource  = vResource;
          finally.nSomeState = 456;
          }
       catch (const SomeException& e)
          {
          // standard exception processing (happens before finally)
          // potentially changes some state of finally
          finally.nSomeState = 789;
          throw;
          }
       } // finally destructor runs

    // unprotected code
    }
```

Note that the CloseFinally helper class breaks most of the rules for Coherence classes. This is because it is not a managed class, not exposed outside the implementation of the Foo class, and most importantly is not accessible to multiple threads. The intention is to write as little code as possible to mimic a Java style finally block. Some notable benefits of the approach are:

* unnamed namespace, ensuring the the class is only visable from within this file
* helper class occurs immediately before the associated method in the .cpp file, rather then residing in the inner class segment
  * this is because it is really just an extension of the method which it helps
* struct used to avoid need to declare items as public
* implemented as a data object, i.e. no accessors methods
  * the data members of the helper class are really just an extension of the methods variables
* Also note that there is no reason we couldn't utilize multiple finally classes for a single method, they could even be nested

## <a name="12"></a>Indentation conventions

*   Each indent consists of 4 spaces (no literal tabs).
*   Curly braces are indented from the parent and on their own line.
*   Statements that may span multiple lines must be enclosed in braces (e.g. if/for/while/try). This implies that there must never be one or two line if blocks.

```c++
if (!vsStr->isASCII())
    {
    COH_THROW(UnsupportedOperationException::create("StringHelper utilities only support ASCII strings."));
    }
```

## <a name="13"></a>Variable declarations and assignment

Variable declarations should be aligned such that:
* The types are aligned
* The names are aligned
* The equals operators are aligned
* The assigned values are aligned

Variable assignments should be aligned such that:
* The names are aligned
* The equals operators are aligned
* The assigned values are aligned

```c++
String::View              vsText        = m_vsFormat;
Array<Object::View>::View vasParameters = m_vasParameters;
```

## <a name="14"></a>Types

* Sized types (e.g. `int32_t`) should be used rather than their un-sized counterparts (e.g. `int`).
* Unsigned types should not be used, except where required by the language.

## <a name="15"></a>Variable prefix conventions

* boolean_t -> f
* octet_t -> b
* char -> ch
* size32_t, int32_t -> i (index), n (number), or c (count)
* int64_t -> l (long)
* float32_t -> fl
* float64_t -> dfl
* Object -> v (Object::View), h (Object::Handle), oh (Object::Holder)
* String -> vs (String::View), hs (String::Handle)
* Array -> va (Array::View), ha (Array::Handle)
* Immutable<T> -> ivT (immutable view of type T)
* WeakReferences -> wh(WeakHandle), wv(WeakView)

## <a name="16"></a>Const correctness

All code should be const correct (i.e. it is not ok to cast away const-ness even when it is "safe").

## <a name="17"></a>Exceptions

Methods must not have a throws clause, but the JavaDoc-style comments should clearly document what exceptions may be thrown explicitly by the method.

## <a name="18"></a>Inner classes

* Inner classes should be fully defined within the scope of the parent class (i.e. `Map::Entry` should be moved within `Map`).
* Inner classes follow the same rules as all other classes (docs, handles, etc.)

## <a name="19"></a>End of file conventions

* End all files with a blank line (g++ requirement).

## <a name="20"></a>Function implementation conventions

Any object that must be retrieved more than once (either through a reference or accessing a member variable) should be first copied to a local variable. There are two major advantages to this approach:

* It's more efficient performance wise.
* It's safer with respect to threading.

```c++
void Logger::writeMessage(Array<Object::View>::View vaoMsg)
    {
    int32_t         nLevel = cast<Integer32::View>(vaoMsg->value[1])->getValue();
    String::View    vsText = formatMessage(vaoMsg);
    Exception::View vEx    = cast<Exception::View>(vaoMsg->value[3]);

    onLog();
    if (NULL == vEx)
        {
        m_hLogOutput->log(nLevel, vsText);
        }
    else if (NULL == vsText)
        {
        m_hLogOutput->log(nLevel, vEx);
        }
    else
        {
        m_hLogOutput->log(nLevel, vEx, vsText);
        }
    }
```

## <a name="21"></a>Object Model

All objects within the coherence API must conform to the C++ object model outlined below:

* All objects are managed via a reference count model. Most objects should inherit from AbstractObject which implements everything related to memory management.
* All objects must have factory methods (`create()`) to return Views and Handles.
  * A View is a const smart pointer to an object. See the `TypedHandle` class.
  * A Handle is a non const smart pointer to an object. See the `Typedhandle` class.
  * Handles and Views must be referred to in documentation as "handles". The names of Handle variables must begin with "h". Likewise, the names of View variables must begin with "v".
  * The use of `BoxedHandle` (autoboxing) is limited to the `String` class and example code.
  * data member Objects are encouraged to be referenced via `MemberHandle`/`MemberViews`
    * MemberViews should make use of the corresponding `AbstractObject::onViewedOnly()` event to switch them to Views when the parent becomes only accessible through Views (see WrapperCollections for examples).
    * Note that MemberHandle and MemberView will be thread safe whereas View and Handle inherently are not (see below).
* **The use of new is restricted to object factory methods only. Direct access to pointers or references are restricted in the C++ object model**

## <a name="22"></a>Thread safety

The Coherence C++ API strives to protect users (and thus our support team) from segfaults and memory corruption as much as possible, without having a large effect on performance. Not all classes within the API need to be thread-safe, but even those which are not thread-safe should still be written in such a way as to help avoid to potential for segfaults/corruption if they are used in a multi-threaded context in an "unsafe" way. For instance our base handle class `TypedHandle` is not thread-safe, that is if one thread assigns it while another thread reads it, there is a possibility that the initially referenced object will be deleted before the accessing thread has a chance to attach to it. Usage of a mutable TypedHandle in a multi-threaded context should therefore be avoided. To do this we've included a few thread-safe implementations of handles. So when should we use the thread-safe handles versus normal TypedHandles? What we've decided is the following:

* mutable `TypedHandle`s must only be allocated on the stack:
  * function/method parameters
  * return values
  * local variables (i.e. within a function)
* global handles (ick) and data-members handles need to be one of our thread-safe handle implementations:
  * MemberHandle
  * MemberView
  * MemberHolder
  * WeakHandle
  * WeakView
  * WeakHolder
  * FinalHandle
  * FinalView
  * FinalHolder

When using one of these thread-safe data members within a method you can simply assign it to a normal TypedHandle local variable just as we would do in Java code.

Since we internally code using our object model the result is that nearly all data structures in the Coherence API are either managed objects or primitives. With these types and the above rules we should end up being fairly well insulated from segfaults/memory corruption even if non thread-safe Objects end up being updated used by multi-threads. This code would certainly be subject to logic type bugs, and could enter and invalid state, but that is no worse then what you would end up with in Java.

On a related note, primitive data-member fields in C++ are a bit more fragile then in Java. Assume you have two primitive data members which are less then the processor word size. If thread 1 always updates field 1 and thread 2 always updates field 2 we may see logic bugs, because the fields could be packed into the same word, and a write to field 1 could also write back a cached value for field 2 (and vice-versa). Since this would introduce a "logic bug" rather then a segfault it is not worth protecting against for code which is not intended to be used in a multi-threaded context. Code which is intended to be used in a multi-threaded context, must protect against this pattern, even though it would be safe in Java. This can be protected either with full-blown synchronization, or with the lighter weight read/write lock provided by AbstractObject.

## <a name="23"></a>Porting TDE Components to C++ managed objects

Almost no one needs to worry about this, but the complex subtleties are easily forgotten:

* All TDE Components must implement a protected virtual instantiate\*Child\* method for any ported TDE child component. This will allow for component specialization throughout the class hierarchy.
* Porting properties requires three steps:
  * Create a private data member for each property
  * Create a getter and or setter for each property (matching the visibility from TDE)
    * All access to the property should be through these methods
  * Set the property's default value in the C++ constructor's initialization list.
    * Note by design this does not use the set method.
* Porting TDE's onInit() method requires two steps:
  * Start with a verbatim copy of TDE's onInit method.
  * At the end of this method you would instantiate each of your children using the virtual instantiate\*Child\* method mentioned above.

## <a name="24"></a>Porting Java casts and instanceof checks

The Coherence C++ Object model supports three basic forms of casting for handles/views/holders

* up casting: i.e. if Bar extends from Foo, casting hBar to hFoo. This will generally happen automatically and no explicit cast operation will be required. In certain rare up-casting cases an explicit cast will be needed. In these cases it is safe to use a Java style cast. i.e.
```c++
    hFoo = (Foo::Handle) hBar;
```

    Of course in this case the explicit cast operator was not even required. When it most often comes up is in ternaries, consider the following contrived example:
```c++
Map::Handle createMap(bool fSynchronized)
    {
    return fSynchronized ? HashMap::create()
                         : SafeHashMap::create();
    }
```

    The above will not compile as the ternary operator requires that both expressions result in the exact same type, simply having them derive some the same type is not acceptable. This example could be fixed as follows:
```c++
Map::Handle createMap(bool fSynchronized)
    {
    return fSynchronized ? (Map::Handle) HashMap::create()
                         : (Map::Handle) SafeHashMap::create();
    }
```

    Note that TypedHandle will ensure that this style cast will only compile if it is safe, so for instance:
```c++
    hBar = (Bar::Handle) hFoo;
```

    Will not compile, since all Foos are not necessarily Bars.

* safe down casting: i.e. casting from Bar to Foo. This is achieved using the `cast<X::Y>(Z)`, where X is the class type, Y is Handle/View, and Z is the handle/view/holder to cast. This cast operator is the functional equivalent of a Java down cast, in that it will do a runtime type check, and will throw ClassCastException if the supplied type (Z) is not an instance of (X::Y).
```c++
    hBar = cast<Bar::Handle>(hFoo); // may throw ClassCastException
```
    Note that if Z is `NULL`, then `NULL` will be returned, just as in Java.

* unsafe down casting: Unlike safe down casting, unsafe down casting will return NULL if the runtime type check fails, rather then throwing ClassCastException. This allows for porting of Java instanceof checks.

```java
// Java
if (oFoo instanceof Bar)
   {
   Bar bar = (Bar) oFoo;
   bar.someBarMethod();
   }
else
   {
   // handle non Bar case (including null)
   }
```
    ports to
```c++
// C++
if (instanceof<Bar::Handle>(hFoo))
    {
    Bar::Handle hBar = cast<Bar::Handle>(hFoo);
    hBar->someBarMethod();
    }
else
    {
    // handle non Bar case (including NULL)
    }
```

    alternatively you can use the super secret, doubly efficient cast as an instance of check, by supplying a second parameter of 'false', which will case a failed cast to return NULL rather then throw a ClassCastException.

```c++
// C++
Bar::Handle hBar = cast<Bar::Handle>(hFoo, false);
if (NULL == hBar)
    {
    // handle non Bar case (including NULL)
    }
else
    {
    hBar->someBarMethod()
    }
```

    Also remember to avoid the down cast operators when up casting, as the run time check is more expensive.

## <a name="25"></a>Right Shift operators

In C, the rule for right shifting unsigned types is:

* The right shift operator will be a logical shift (move to the right and fill with 0).
* For signed types however, it is compiler specific, sometimes it will be arithmetic (preserve the high order bit) and other times it will be logical.

As such, arithmetic right shifts should be avoided, and all logical shifts when ported should be done using an unsigned type.

# <a name="tools"></a>Tools

Any C++ editor is currently acceptable provided that it follows the code formatting guidelines below. Eclipse has been chosen as the IDE of choice.

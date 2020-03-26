/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LANG_SPEC_HPP
#define COH_LANG_SPEC_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

class Object;

/**
* @internal
*
* Definition of terminal class identifier.
*/
template<class T = void> class COH_EXPORT_SPEC Void
    {
    protected:
        virtual ~Void()
            {}

    public:
        /**
        * @internal
        *
        * Perform a cast from this type to the specified
        * type.  This method is for use by the cast<T> operator.
        *
        * @param pInfo  the type to cast to
        *
        * @return NULL on failure, else the cast'd value represented as a
        *         void* which is suitable for direct casting to the
        *         corresponding type.
        */
        virtual void* _cast(coh_class_id /*pInfo*/) const
            {
            return NULL;
            }

        /**
        * @internal
        */
        virtual size64_t sizeOf(bool /*fDeep*/ = false) const
            {
            return 0;
            }
    };

/**
* @internal
*
* Definition of class alias, used in Exceptions.
*/
template<class T> class Alias
    {
    public:
        typedef typename T::alias alias;
    };

/**
* @internal
*
* Terminal Alias.
*/
template<> class Alias<Void<> >
    {
    public:
        typedef Void<> alias;
    };

/**
* @internal
*
* Object Alias.
*/
template<> class Alias<Object>
    {
    public:
        typedef Void<Object> alias;
    };


/**
* @internal
*
* Definition of individual link in chain of implemented interfaces.
*
* @see implements
*
* @author mf 2008.07.14
*/
template<class I, class N> class COH_EXPORT_SPEC interface_link
    : public virtual I, public virtual N
    {
    public:
        /**
        * @internal
        *
        * Perform a cast from this type to the specified type.
        * Unlike the virtual _cast which is part of class_specs,
        * _icast handles interfaces, and is called only by _cast and thus
        * doesn't need to be virtual.
        *
        * @param pInfo  the type to cast to
        *
        * @return NULL on failure, else the cast'd value represented as a
        *         void* which is suitable for direct casting to the
        *         corresponding type.
        */
        inline void* _icast(coh_class_id pInfo) const
            {
            // first check if this interface has the class id in question
            // if not do a breadth first search across directly inherited
            // interfaces, then finally tail recurse up the inheritance
            // hierarchy of each of those interfaces
            void* p = COH_CLASS_ID(I) == pInfo
                    ? (void*) static_cast<const I*>(this)
                    : N::_icast(pInfo);
            return p ? p : I::_icast(pInfo);
            }
    };

/**
* @internal
*
* Terminal interface_link
*/
template<class I> class COH_EXPORT_SPEC interface_link<void, I>
    {
    public:
    inline void* _icast(coh_class_id /*pInfo*/) const
        {
        return NULL;
        }
    };


/**
* The implements template specifies a list of interfaces which a class or
* interface specification derives from.  Each interface will be virtually
* inherited by specified class or interface. Up to sixteen interfaces are
* supported, in the case where more are required, they can be specified using
* an interface_link<I> chain.  i.e.
*
* @code
* implements<I1, ..., I15, interface_link<I16, interface_link<I17> > >
* @endcode
*
* @see abstract_spec
* @see class_spec
* @see cloneable_spec
* @see throwable_spec
* @see interface_spec
*
* @author mf 2008.07.14
*/
template<class I1  = void,
         class I2  = void,
         class I3  = void,
         class I4  = void,
         class I5  = void,
         class I6  = void,
         class I7  = void,
         class I8  = void,
         class I9  = void,
         class I10 = void,
         class I11 = void,
         class I12 = void,
         class I13 = void,
         class I14 = void,
         class I15 = void,
         class I16 = void>
class COH_EXPORT_SPEC implements
    {
    public:
        typedef interface_link<I1,
                interface_link<I2,
                interface_link<I3,
                interface_link<I4,
                interface_link<I5,
                interface_link<I6,
                interface_link<I7,
                interface_link<I8,
                interface_link<I9,
                interface_link<I10,
                interface_link<I11,
                interface_link<I12,
                interface_link<I13,
                interface_link<I14,
                interface_link<I15,
                interface_link<I16,
                void > > > > > > > > > > > > > > > >
        implements_chain;

        typedef I1  interface_1;
        typedef I2  interface_2;
        typedef I3  interface_3;
        typedef I4  interface_4;
        typedef I5  interface_5;
        typedef I6  interface_6;
        typedef I7  interface_7;
        typedef I8  interface_8;
        typedef I9  interface_9;
        typedef I10 interface_10;
        typedef I11 interface_11;
        typedef I12 interface_12;
        typedef I13 interface_13;
        typedef I14 interface_14;
        typedef I15 interface_15;
        typedef I16 interface_16;
    };

/**
* The extends template indicates the parent class in a class specification.
*
* @see abstract_spec
* @see class_spec
* @see cloneable_spec
* @see throwable_spec
*
* @author mf 2008.07.14
*/
template<class P = Void<>, class A = typename Alias<P>::alias >
class COH_EXPORT_SPEC extends
    {
    public:
        /**
        * Marker for the actual inherited parent class.
        */
        typedef P inherited;

        /**
         * The literal inherited class.
         */
        typedef P inherited_literal;

        /**
        * The purpose of this definition is to cause a compilation error if
        * an attempt is made to instantiate the extends template by supplying
        * an interface.
        */
        typedef typename P::super grand;

        /**
        * Alias type used in throwables.
        */
        typedef A alias;
    };

/**
* @internal
*
* Terminal definition in extension hierarchy.
*/
template<> class COH_EXPORT_SPEC extends<Void<>, Void<> >
    {
    public:
        typedef Void<> inherited;
        typedef Void<> inherited_literal;
        typedef Void<> alias;
    };

// ----- helper macros used in the creation of specs ------------------------

/**
* The COH_LIST macros create a comma separated list of names, which each name
* is the same except for a numeric suffix.
*/
#define COH_LIST1(A)                 A##1
#define COH_LIST2(A)  COH_LIST1 (A), A##2
#define COH_LIST3(A)  COH_LIST2 (A), A##3
#define COH_LIST4(A)  COH_LIST3 (A), A##4
#define COH_LIST5(A)  COH_LIST4 (A), A##5
#define COH_LIST6(A)  COH_LIST5 (A), A##6
#define COH_LIST7(A)  COH_LIST6 (A), A##7
#define COH_LIST8(A)  COH_LIST7 (A), A##8
#define COH_LIST9(A)  COH_LIST8 (A), A##9
#define COH_LIST10(A) COH_LIST9 (A), A##10
#define COH_LIST11(A) COH_LIST10(A), A##11
#define COH_LIST12(A) COH_LIST11(A), A##12
#define COH_LIST13(A) COH_LIST12(A), A##13
#define COH_LIST14(A) COH_LIST13(A), A##14
#define COH_LIST15(A) COH_LIST14(A), A##15
#define COH_LIST16(A) COH_LIST15(A), A##16

/**
* The COH_ARG_LIST macros create a comma separated list of argument
* declarations names, which each argument name is the same except for a
* numeric suffix.
*/
#define COH_ARG_LIST1(A)                     A##1&  a1
#define COH_ARG_LIST2(A)  COH_ARG_LIST1 (A), A##2&  a2
#define COH_ARG_LIST3(A)  COH_ARG_LIST2 (A), A##3&  a3
#define COH_ARG_LIST4(A)  COH_ARG_LIST3 (A), A##4&  a4
#define COH_ARG_LIST5(A)  COH_ARG_LIST4 (A), A##5&  a5
#define COH_ARG_LIST6(A)  COH_ARG_LIST5 (A), A##6&  a6
#define COH_ARG_LIST7(A)  COH_ARG_LIST6 (A), A##7&  a7
#define COH_ARG_LIST8(A)  COH_ARG_LIST7 (A), A##8&  a8
#define COH_ARG_LIST9(A)  COH_ARG_LIST8 (A), A##9&  a9
#define COH_ARG_LIST10(A) COH_ARG_LIST9 (A), A##10& a10
#define COH_ARG_LIST11(A) COH_ARG_LIST10(A), A##11& a11
#define COH_ARG_LIST12(A) COH_ARG_LIST11(A), A##12& a12
#define COH_ARG_LIST13(A) COH_ARG_LIST12(A), A##13& a13
#define COH_ARG_LIST14(A) COH_ARG_LIST13(A), A##14& a14
#define COH_ARG_LIST15(A) COH_ARG_LIST14(A), A##15& a15
#define COH_ARG_LIST16(A) COH_ARG_LIST15(A), A##16& a16

/**
* Define a proxy constructor with N arguments.
*/
#define COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, N) \
    template<COH_LIST##N (class A)> SPEC(COH_ARG_LIST##N (A)) \
        : super_spec(COH_LIST##N (a)) {} \
    template<COH_LIST##N (class A)> SPEC(COH_ARG_LIST##N (const A)) \
        : super_spec(COH_LIST##N (a)) {}

/**
* Define a set of templated copy constructors.
*
* @param SPEC the class of which the constructors are declared
*/
#define COH_DEFINE_PROXY_CONSTRUCTORS(SPEC) \
    SPEC() : super_spec() {} \
    SPEC(const SPEC& that) : super_spec(that) {} \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 1)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 2)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 3)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 4)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 5)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 6)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 7)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 8)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 9)  \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 10) \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 11) \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 12) \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 13) \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 14) \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 15) \
    COH_DEFINE_PROXY_CONSTRUCTOR(SPEC, 16)

/**
* Define a factory create method with N arguments.
*/
#define COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, N) \
    template<COH_LIST##N (class A)> static RETURN create(COH_ARG_LIST##N (A)) \
        {return FUNCTION(COH_LIST##N(a));} \
    template<COH_LIST##N (class A)> static RETURN create(COH_ARG_LIST##N (const A)) \
        {return FUNCTION(COH_LIST##N(a));}

/**
* Define factory "create" methods.
*
* @param RETURN    the return type from the factory
* @param FUNCTION  the function to be called to produce the the RETURN
*/
#define COH_DEFINE_CREATE_METHODS(RETURN, FUNCTION) \
    static RETURN create() {return FUNCTION();}  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 1)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 2)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 3)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 4)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 5)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 6)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 7)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 8)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 9)  \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 10) \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 11) \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 12) \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 13) \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 14) \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 15) \
    COH_DEFINE_PROXY_CREATE_METHOD(RETURN, FUNCTION, 16)

/**
* The factory class auto-generates static create methods for a class,
* corresponding to the class's constructors.  The factory must be made a
* friend of the defined class so that it may access the protected constructors.
*
* The factory is used by class specifications to provide the class with static
* create methods.  i.e. the result of using a class_spec when defining class
* Foo will result in Foo having auto-generated static create methods.  Direct
* use of the factory for class instantiation is not supported.
*
* The factory supports constructors from zero to sixteen parameters. To help
* the template system it is best (though not required) that there are not
* multiple constructors with the same number of parameters. Classes wishing to
* provide custom create methods may simply define them in their class, and
* these will override and hide the factory auto-generated versions.
*
* @see class_spec
* @see cloneable_spec
* @see throwable_spec
*
* @author mf 2008.07.14
*/
template<class T> class COH_EXPORT_SPEC factory
    {
    template<class, class, class> friend class class_spec;

    protected:
        /**
        * Generate a set of static "create" methods matching the signatures of
        * class T's constructors.
        *
        * NOTE: Compilation errors referencing this line likely indicate that
        *       the parameters supplied by the caller to the create method did
        *       not match one of the constructors.
        */
        COH_DEFINE_CREATE_METHODS(T*, new T)
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LANG_SPEC_HPP

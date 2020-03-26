/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_METHOD_HPP
#define COH_TYPED_METHOD_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/BoxHandle.hpp"
#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/lang/Method.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/class_spec.hpp"

#include "coherence/lang/Boolean.hpp"
#include "coherence/lang/Octet.hpp"
#include "coherence/lang/Character16.hpp"
#include "coherence/lang/Integer16.hpp"
#include "coherence/lang/Integer32.hpp"
#include "coherence/lang/Integer64.hpp"
#include "coherence/lang/Size32.hpp"
#include "coherence/lang/Size64.hpp"
#include "coherence/lang/Float32.hpp"
#include "coherence/lang/Float64.hpp"
#include "coherence/lang/String.hpp"


COH_OPEN_NAMESPACE2(coherence,lang)

namespace
    {
    // ----- internal helpers -----------------------------------------------

    // the following templates are used by TypedMethod to produce specialized
    // variants of methods in order to account for "void" parameters

    /**
    * @internal
    */
    template<class T>
    class coh_TypedMethodHelper
        {
        public:

            /**
            * @internal
            */
            static Method::ClassView getMethodArgumentType();

            /**
            * @internal
            */
            static int32_t getMethodModifiers();
        };

    template<class T> Method::ClassView coh_TypedMethodHelper<T>::getMethodArgumentType()
        {
        return SystemClassLoader::getInstance()->loadByType(typeid(typename T::ValueType));
        }

    template<> Method::ClassView coh_TypedMethodHelper<void>::getMethodArgumentType()
        {
        return NULL;
        }

    template<class T> int32_t coh_TypedMethodHelper<T>::getMethodModifiers()
        {
        return Method::modifier_instance |
                     (constness<typename T::ValueType>::applied
                     ? Method::modifier_const
                     : Method::modifier_mutable);
        }

    template<> int32_t coh_TypedMethodHelper<void>::getMethodModifiers()
        {
        return Method::modifier_static;
        }

    /**
    * @internal
    */
    template<class H, class P, class R, class A1, class A2, class A3, class A4,
             class A5, class A6, class A7, class A8>
    class coh_invoker
        {
        public:
            static Object::Holder invoke(Object::Holder oh, P p, ObjectArray::View va)
                {
                COH_ENSURE_PARAM_RELATION(va->length, ==, 3);
                return R((*cast<H>(oh).*(p))(cast<A1>(va[0]), cast<A2>(va[1]),
                        cast<A3>(va[2]), cast<A4>(va[3]), cast<A5>(va[4]),
                        cast<A6>(va[5]), cast<A7>(va[6]), cast<A8>(va[7])));
                }
        };
    }

/**
 * TypedMethod provides a template based implementation for building Methods
 * which delegate to member function pointers.
 *
 * Rather then making direct use of this class it is recommended to utilize
 * the following helper macros.  For each of these methods there are nine
 * variants suffixed 0..8 which indicate the number of arguments the method
 * takes.
 *
 * - COH_CONSTRUCTOR     defines a Method representing a constructor
 * - COH_METHOD          defines a Method representing an instance method
 * - COH_STATIC_METHOD   defines a Method representing a static method
 * - COH_MANAGED_METHOD  defines a Method representing an instance on a
 *                       Managed<> class
 *
 * - COH_OVERLOADED_CONSTRUCTOR    defines an overloaded constructor
 * - COH_OVERLOADED_METHOD         defines an overloaded instance method
 * - COH_OVERLOADED_STATIC_METHOD  defines an overloaded static method
 *
 * Additionally the following macros will create setter and getter Methods
 * for a "property".
 *
 * - COH_PROPERTY          defines setter/getter for a object property
 * - COH_BOX_PROPERTY      defines a setter/getter for a value type property
 * - COH_MANAGED_PROPERTY  defines a setter/getter for a value type on a
 *                         Managed<> class
 *
 * These macros output Method objects which can then be registered with
 * their corresponding Class object.  This registration can either be
 * performed as part of the Class registration, or it can be done in multiple
 * COH_REGISTER_METHOD blocks, allowing the Method definition to reside
 * next to the actual method implementation.
 *
 * @code
 * // Person.hpp
 * class Person
 *      : public class_spec<Person>
 *      {
 *      friend class factory<Person>;
 *
 *      protected:
 *          Person(String::View vsName = String::null_string, int32_t nAge = 0);
 *
 *      public:
 *          virtual void setName(String::View vsName);
 *          virtual String::View getName() const;
 *
 *          virtual void setAge(int32_t nAge);
 *          virtual int32_t getAge() const;
 *      ...
 *      };
 * @endcode
 *
 * @code
 * // Person.cpp - using single class registration block
 *
 * COH_REGISTER_CLASS(TypedClass<Person>::create()
 *      ->declare(COH_CONSTRUCTOR2(Person, String::View, BoxHandle<const Integer32>))
 *      ->declare(COH_PROPERTY(Person, Name, String::View))
 *      ->declare(COH_BOX_PROPERTY(Person, Age, Integer32::View));
 * @endcode
 *
 * @code
 * // Person.cpp - using multi method registration blocks
 *
 * COH_REGISTER_TYPED_CLASS(Person);
 *
 * Person::Person(String::View vsName, int32_t nAge)
 *    {
 *    ...
 *    }
 * COH_REGISTER_METHOD(Person, COH_CONSTRUCTOR2(Person, String::View, BoxHandle<const Integer32>));
 *
 * void Person::setName(String::View vsName)
 *     {
 *     ...
 *     }
 * COH_REGISTER_METHOD(Person, COH_METHOD1(void, Person::Handle, setName, String::View));
 *
 * String::View Person::getName() const
 *     {
 *     ...
 *     }
 * COH_REGISTER_METHOD(Person, COH_METHOD0(String::View, Person::View, getName));
 *
 * void Person::setAge(int32_t nAge)
 *     {
 *     ...
 *     }
 * COH_REGISTER_METHOD(Person, COH_METHOD1(void, Person::Handle, setName, BoxHandle<const Integer32>));
 *
 * int32_t Person::getAge() const
 *     {
 *     ...
 *     }
 * COH_REGISTER_METHOD(Person, COH_METHOD0(BoxHandle<const Integer32>, Person::View, getName));
 * @endcode
 *
 * @author mf 2011.02.28
 *
 * @since Coherence 3.7.1
 */
template<class P, class R, class C, class H, P M,
    class A1 = void, class A2 = void, class A3 = void, class A4 = void,
    class A5 = void, class A6 = void, class A7 = void, class A8 = void>
class TypedMethod
    : public class_spec<TypedMethod<P, R, C, H, M, A1, A2, A3, A4, A5, A6, A7, A8>,
          extends<Method> >
    {
    friend class factory<TypedMethod<P, R, C, H, M, A1, A2, A3, A4, A5, A6, A7, A8> >;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Construct a TypedMethod.
         *
         * @param vsName  the method name
         */
        TypedMethod(String::View vsName)
            : class_spec<TypedMethod<P, R, C, H, M, A1, A2, A3, A4, A5, A6, A7, A8>,
              extends<Method> >(vsName)
            {
            }


    // ----- Method interface -----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual int32_t getModifiers() const
            {
            return coh_TypedMethodHelper<H>::getMethodModifiers();
            }

        /**
         * {@inheritDoc}
         */
        virtual ObjectArray::View getParameterTypes() const
            {
            ObjectArray::Handle ha =
                    typeid(A1) == typeid(void) ? ObjectArray::create(0)
                  : typeid(A2) == typeid(void) ? ObjectArray::create(1)
                  : typeid(A3) == typeid(void) ? ObjectArray::create(2)
                  : typeid(A4) == typeid(void) ? ObjectArray::create(3)
                  : typeid(A5) == typeid(void) ? ObjectArray::create(4)
                  : typeid(A6) == typeid(void) ? ObjectArray::create(5)
                  : typeid(A7) == typeid(void) ? ObjectArray::create(6)
                  : typeid(A8) == typeid(void) ? ObjectArray::create(7)
                  :                              ObjectArray::create(8);

            switch (ha->length)
                {
                case 8:
                    ha[7] = coh_TypedMethodHelper<A8>::getMethodArgumentType();
                case 7:
                    ha[6] = coh_TypedMethodHelper<A7>::getMethodArgumentType();
                case 6:
                    ha[5] = coh_TypedMethodHelper<A6>::getMethodArgumentType();
                case 5:
                    ha[4] = coh_TypedMethodHelper<A5>::getMethodArgumentType();
                case 4:
                    ha[3] = coh_TypedMethodHelper<A4>::getMethodArgumentType();
                case 3:
                    ha[2] = coh_TypedMethodHelper<A3>::getMethodArgumentType();
                case 2:
                    ha[1] = coh_TypedMethodHelper<A2>::getMethodArgumentType();
                case 1:
                    ha[0] = coh_TypedMethodHelper<A1>::getMethodArgumentType();
                case 0:
                    break;
                default:
                    coh_throw_illegal_state("unsupported method parameter count");
                }

            return ha;
            }

        /**
         * {@inheritDoc}
         */
        virtual Method::ClassView getReturnType() const
            {
            return coh_TypedMethodHelper<R>::getMethodArgumentType();
            }

        /**
         * {@inheritDoc}
         */
        virtual Method::ClassView getDeclaringClass() const
            {
            return SystemClassLoader::getInstance()->loadByType(typeid(C));
            }

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder invoke(Object::Holder oh, ObjectArray::View vaArgs) const
            {
            if (typeid(H) == typeid(void) && oh != NULL &&
                    !instanceof<typename C::View>(oh))
                {
                coh_throw_illegal_argument("oh must be NULL or of correct type");
                }
            return coh_invoker<H, P, R, A1, A2, A3, A4, A5, A6, A7, A8>::invoke(oh, M, vaArgs);
            }
    };

// ----- coh_invoker specializations ----------------------------------------

namespace
    {
    // the following macros create specialized variants of the coh_invoker in
    // order to support 1-7 parameters
    #define COH_INVOKER(N, HH, RR, AA1, AA2, AA3, AA4, AA5, AA6, AA7, AA8, EXEC) \
        class coh_invoker<HH, P, RR, AA1, AA2, AA3, AA4, AA5, AA6, AA7, AA8> \
            { \
            public: \
                static Object::Holder invoke(Object::Holder oh, P p, ObjectArray::View va) \
                    { \
                    if (!(N == 0 && va == NULL)) \
                        { \
                        COH_ENSURE_PARAM_RELATION(va->length, ==, N); \
                        } \
                    Object::Holder ohSunProWantsItToGetUsed = oh; \
                    Object::Holder r; \
                    EXEC; \
                    return r; \
                    } \
            }

    #define COH_A0
    #define COH_A1 COH_A0, class A1
    #define COH_A2 COH_A1, class A2
    #define COH_A3 COH_A2, class A3
    #define COH_A4 COH_A3, class A4
    #define COH_A5 COH_A4, class A5
    #define COH_A6 COH_A5, class A6
    #define COH_A7 COH_A6, class A7

    #define COH_DEF_METHOD_TEMPLATE(N, RT) template<class H, class P, class RT COH_A##N>
    #define COH_DEF_METHOD_TEMPLATE_VOID(N) template<class H, class P COH_A##N>
    #define COH_DEF_STATIC_METHOD_TEMPLATE(N, RT) template<class P, class RT COH_A##N>
    #define COH_DEF_STATIC_METHOD_TEMPLATE_VOID(N) template<class P COH_A##N>

    #define COH_DEF_ARGS0
    #define COH_DEF_ARGS1 COH_DEF_ARGS0  cast<A1>(va[0])
    #define COH_DEF_ARGS2 COH_DEF_ARGS1, cast<A2>(va[1])
    #define COH_DEF_ARGS3 COH_DEF_ARGS2, cast<A3>(va[2])
    #define COH_DEF_ARGS4 COH_DEF_ARGS3, cast<A4>(va[3])
    #define COH_DEF_ARGS5 COH_DEF_ARGS4, cast<A5>(va[4])
    #define COH_DEF_ARGS6 COH_DEF_ARGS5, cast<A6>(va[5])
    #define COH_DEF_ARGS7 COH_DEF_ARGS6, cast<A7>(va[6])

    #define COH_INSTANCE_METHOD *cast<H>(oh).*(p)
    #define COH_STATIC_METHOD *(p)

    #define COH_DEF_EXEC(N, MS, RS) RS((MS)(COH_DEF_ARGS##N));

    #define COH_DEF_INVOKER0(RT, HH, MS, RS) COH_INVOKER(0, HH, RT, void, void, void, void, void, void, void, void, COH_DEF_EXEC(0, MS, RS))
    #define COH_DEF_INVOKER1(RT, HH, MS, RS) COH_INVOKER(1, HH, RT, A1, void, void, void, void, void, void, void, COH_DEF_EXEC(1, MS, RS))
    #define COH_DEF_INVOKER2(RT, HH, MS, RS) COH_INVOKER(2, HH, RT, A1, A2, void, void, void, void, void, void, COH_DEF_EXEC(2, MS, RS))
    #define COH_DEF_INVOKER3(RT, HH, MS, RS) COH_INVOKER(3, HH, RT, A1, A2, A3, void, void, void, void, void, COH_DEF_EXEC(3, MS, RS))
    #define COH_DEF_INVOKER4(RT, HH, MS, RS) COH_INVOKER(4, HH, RT, A1, A2, A3, A4, void, void, void, void, COH_DEF_EXEC(4, MS, RS))
    #define COH_DEF_INVOKER5(RT, HH, MS, RS) COH_INVOKER(5, HH, RT, A1, A2, A3, A4, A5, void, void, void, COH_DEF_EXEC(5, MS, RS))
    #define COH_DEF_INVOKER6(RT, HH, MS, RS) COH_INVOKER(6, HH, RT, A1, A2, A3, A4, A5, A6, void, void, COH_DEF_EXEC(6, MS, RS))
    #define COH_DEF_INVOKER7(RT, HH, MS, RS) COH_INVOKER(7, HH, RT, A1, A2, A3, A4, A5, A6, A7, void, COH_DEF_EXEC(7, MS, RS))

    #define COH_DEF_METHOD(N) \
        COH_DEF_METHOD_TEMPLATE(N, R) COH_DEF_INVOKER##N(R, H, COH_INSTANCE_METHOD, r = R); \
        COH_DEF_METHOD_TEMPLATE_VOID(N) COH_DEF_INVOKER##N(void, H, COH_INSTANCE_METHOD, ;); \
        COH_DEF_STATIC_METHOD_TEMPLATE(N, R) COH_DEF_INVOKER##N(R, void, COH_STATIC_METHOD, r = R); \
        COH_DEF_STATIC_METHOD_TEMPLATE_VOID(N) COH_DEF_INVOKER##N(void, void, COH_STATIC_METHOD, ;)

    COH_DEF_METHOD(0);
    COH_DEF_METHOD(1);
    COH_DEF_METHOD(2);
    COH_DEF_METHOD(3);
    COH_DEF_METHOD(4);
    COH_DEF_METHOD(5);
    COH_DEF_METHOD(6);
    COH_DEF_METHOD(7);

    #undef COH_DEF_METHOD
    #undef COH_DEF_INVOKER0
    #undef COH_DEF_INVOKER1
    #undef COH_DEF_INVOKER2
    #undef COH_DEF_INVOKER3
    #undef COH_DEF_INVOKER4
    #undef COH_DEF_INVOKER5
    #undef COH_DEF_INVOKER6
    #undef COH_DEF_INVOKER7
    #undef COH_DEF_EXEC
    #undef COH_STATIC_METHOD
    #undef COH_INSTANCE_METHOD

    #undef COH_DEF_ARGS0
    #undef COH_DEF_ARGS1
    #undef COH_DEF_ARGS2
    #undef COH_DEF_ARGS3
    #undef COH_DEF_ARGS4
    #undef COH_DEF_ARGS5
    #undef COH_DEF_ARGS6
    #undef COH_DEF_ARGS7

    #undef COH_DEF_METHOD_TEMPLATE
    #undef COH_DEF_METHOD_TEMPLATE_VOID
    #undef COH_DEF_STATIC_METHOD_TEMPLATE
    #undef COH_DEF_STATIC_METHOD_TEMPLATE_VOID

    #undef COH_A0
    #undef COH_A1
    #undef COH_A2
    #undef COH_A3
    #undef COH_A4
    #undef COH_A5
    #undef COH_A6
    #undef COH_A7

    #undef COH_INVOKER

    /**
    * @internal
    */
    template<class C, class H>
    class coh_type_info
        {
        public: typedef H type;
        };

    /**
    * @internal
    */
    template<class C>
    class coh_type_info<C, BoxHandle<C> >
        {
        public: typedef typename C::BoxedType type;
        };

    /**
    * @internal
    */
    template<class C>
    class coh_type_info<C, BoxHandle<const C> >
        {
        public: typedef typename C::BoxedType type;
        };

    // for compilers (SunPro) which have trouble with the above partial
    // specializations we provide more specific versions
    #define COH_BOX_TYPE_INFO(T) \
        template<> \
        class coh_type_info<T, BoxHandle<T> > \
            { \
            public: typedef T::BoxedType type; \
            }; \
        template<> \
        class coh_type_info<const T, BoxHandle<const T> > \
            { \
            public: typedef T::BoxedType type; \
            }

    COH_BOX_TYPE_INFO(Boolean);
    COH_BOX_TYPE_INFO(Octet);
    COH_BOX_TYPE_INFO(Character16);
    COH_BOX_TYPE_INFO(Integer16);
    COH_BOX_TYPE_INFO(Integer32);
    COH_BOX_TYPE_INFO(Integer64);
    COH_BOX_TYPE_INFO(Size32);
    COH_BOX_TYPE_INFO(Size64);
    COH_BOX_TYPE_INFO(Float32);
    COH_BOX_TYPE_INFO(Float64);
    COH_BOX_TYPE_INFO(String);

    /**
    * @internal
    */
    template<class H>
    class coh_handle_info
        {
        public: typedef typename coh_type_info<typename H::ValueType, H>::type type;
        };

    /**
    * @internal
    */
    template<>
    class coh_handle_info<void>
        {
        public: typedef void type;
        };
    }

// the following templates will compute a function pointer signature, similar
// what the non-standard __typeof__ operator would do

/**
* @internal
*/
template<int N, class R, class C, class H, class A1 = void, class A2 = void,
    class A3 = void, class A4 = void, class A5 = void,
    class A6 = void, class A7 = void, class A8 = void>
class coh_signature_info \
    { \
    public: typedef void signature;
    };

#define COH_SIG_INFON(N, RS, CS, HS, AS, TS) \
template<class R, class C, class A1, class A2, \
    class A3, class A4, class A5, \
    class A6, class A7, class A8> \
class coh_signature_info<N, R, C, HS, A1, A2, A3, A4, A5, A6, A7, A8> \
    { \
    public: typedef typename coh_handle_info<RS>::type (CS signature)AS TS \
    }


#define COH_ARG_TYPE0
#define COH_ARG_TYPE1 COH_ARG_TYPE0  typename coh_handle_info<A1>::type
#define COH_ARG_TYPE2 COH_ARG_TYPE1, typename coh_handle_info<A2>::type
#define COH_ARG_TYPE3 COH_ARG_TYPE2, typename coh_handle_info<A3>::type
#define COH_ARG_TYPE4 COH_ARG_TYPE3, typename coh_handle_info<A4>::type
#define COH_ARG_TYPE5 COH_ARG_TYPE4, typename coh_handle_info<A5>::type
#define COH_ARG_TYPE6 COH_ARG_TYPE5, typename coh_handle_info<A6>::type
#define COH_ARG_TYPE7 COH_ARG_TYPE6, typename coh_handle_info<A7>::type
#define COH_ARG_TYPE8 COH_ARG_TYPE7, typename coh_handle_info<A8>::type

#define COH_SIG_INFO(N) \
COH_SIG_INFON(N, R,    *, void              ,(COH_ARG_TYPE##N), ;); \
COH_SIG_INFON(N, R, C::*, typename C::Handle,(COH_ARG_TYPE##N), ;); \
COH_SIG_INFON(N, R, C::*, typename C::View  ,(COH_ARG_TYPE##N), const;)

COH_SIG_INFO(0);
COH_SIG_INFO(1);
COH_SIG_INFO(2);
COH_SIG_INFO(3);
COH_SIG_INFO(4);
COH_SIG_INFO(5);
COH_SIG_INFO(6);
COH_SIG_INFO(7);
COH_SIG_INFO(8);

#undef COH_SIG_INFO
#undef COH_SIG_INFON
#undef COH_ARG_TYPE0
#undef COH_ARG_TYPE1
#undef COH_ARG_TYPE2
#undef COH_ARG_TYPE3
#undef COH_ARG_TYPE4
#undef COH_ARG_TYPE5
#undef COH_ARG_TYPE6
#undef COH_ARG_TYPE7
#undef COH_ARG_TYPE8

COH_CLOSE_NAMESPACE2


// ----- helper macros ------------------------------------------------------

/**
 * Declare an overloaded static Method based on a description of its signature.
 *
 * This OVERLOADED macro is only necessary when a class or interface declares
 * multiple methods of the same name, otherwise the COH_DECLARE_STATIC_METHOD
 * is a better choice of macros.  An additional use case for this macro variant
 * is to handle the case where defining a static Method for a Managed<T> class
 * where the method is not actually declared on T, but on some super class of T.
 *
 * For both R, and A1..A8 the value should be a handle type, for instance
 * String::View or Map::Handle.  If the underlying method returns a value
 * type instead, then a BoxHandle<> needs to be supplied for the corresponding
 * Object type.  For instance BoxHandle<const Integer32> in place of int32_t.
 *
 * @param S       the method signature
 * @param R       the method return type
 * @param C       the target class to call on
 * @param M       the method name
 * @param A1..A8  the argument types
 */
#define COH_OVERLOADED_STATIC_METHOD8(S, R, C, M, A1, A2, A3, A4, A5, A6, A7, A8) \
        coherence::lang::TypedMethod<S, \
               R, C, void, &C::M, A1, A2, A3, A4, A5, A6, A7, A8>::create(#M)
#define COH_OVERLOADED_STATIC_METHOD7(S, R, C, M, A1, A2, A3, A4, A5, A6, A7) \
        COH_OVERLOADED_STATIC_METHOD8(S, R, C, M, A1, A2, A3, A4, A5, A6, A7, void)
#define COH_OVERLOADED_STATIC_METHOD6(S, R, C, M, A1, A2, A3, A4, A5, A6) \
        COH_OVERLOADED_STATIC_METHOD7(S, R, C, M, A1, A2, A3, A4, A5, A6, void)
#define COH_OVERLOADED_STATIC_METHOD5(S, R, C, M, A1, A2, A3, A4, A5) \
        COH_OVERLOADED_STATIC_METHOD6(S, R, C, M, A1, A2, A3, A4, A5, void)
#define COH_OVERLOADED_STATIC_METHOD4(S, R, C, M, A1, A2, A3, A4) \
        COH_OVERLOADED_STATIC_METHOD5(S, R, C, M, A1, A2, A3, A4, void)
#define COH_OVERLOADED_STATIC_METHOD3(S, R, C, M, A1, A2, A3) \
        COH_OVERLOADED_STATIC_METHOD4(S, R, C, M, A1, A2, A3, void)
#define COH_OVERLOADED_STATIC_METHOD2(S, R, C, M, A1, A2) \
        COH_OVERLOADED_STATIC_METHOD3(S, R, C, M, A1, A2, void)
#define COH_OVERLOADED_STATIC_METHOD1(S, R, C, M, A) \
        COH_OVERLOADED_STATIC_METHOD2(S, R, C, M, A, void)
#define COH_OVERLOADED_STATIC_METHOD0(S, R, C, M) \
        COH_OVERLOADED_STATIC_METHOD1(S, R, C, M, void)

/**
 * Declare a static Method based on a description of its signature.
 *
 * For both R, and A1..A8 the value should be a handle type, for instance
 * String::View or Map::Handle.  If the underlying method returns a value
 * type instead, then a BoxHandle<> needs to be supplied for the corresponding
 * Object type.  For instance BoxHandle<const Integer32> in place of in32_t.
 *
 * @param R       the method return type
 * @param H       the target handle type to call on
 * @param M       the method name
 * @param A1..A8  the argument types
 */
#define COH_STATIC_METHOD(N, R, C, M, A1, A2, A3, A4, A5, A6, A7, A8) \
        coherence::lang::TypedMethod<coh_signature_info<N, R, C, void, A1, A2, \
            A3, A4, A5, A6, A7, A8>::signature, R, C, void, \
            &C::M, A1, A2, A3, A4, A5, A6, A7, A8>::create(#M)
#define COH_STATIC_METHOD8(R, C, M, A1, A2, A3, A4, A5, A6, A7, A8) \
        COH_STATIC_METHOD(8, R, C, M, A1, A2, A3, A4, A5, A6, A7, A8)
#define COH_STATIC_METHOD7(R, C, M, A1, A2, A3, A4, A5, A6, A7) \
        COH_STATIC_METHOD(7, R, C, M, A1, A2, A3, A4, A5, A6, A7, void)
#define COH_STATIC_METHOD6(R, C, M, A1, A2, A3, A4, A5, A6) \
        COH_STATIC_METHOD(6, R, C, M, A1, A2, A3, A4, A5, A6, void, void)
#define COH_STATIC_METHOD5(R, C, M, A1, A2, A3, A4, A5) \
        COH_STATIC_METHOD(5, R, C, M, A1, A2, A3, A4, A5, void, void, void)
#define COH_STATIC_METHOD4(R, C, M, A1, A2, A3, A4) \
        COH_STATIC_METHOD(4, R, C, M, A1, A2, A3, A4, void, void, void, void)
#define COH_STATIC_METHOD3(R, C, M, A1, A2, A3) \
        COH_STATIC_METHOD(3, R, C, M, A1, A2, A3, void, void, void, void, void)
#define COH_STATIC_METHOD2(R, C, M, A1, A2) \
        COH_STATIC_METHOD(2, R, C, M, A1, A2, void, void, void, void, void, void)
#define COH_STATIC_METHOD1(R, C, M, A) \
        COH_STATIC_METHOD(1, R, C, M, A, void, void, void, void, void, void, void)
#define COH_STATIC_METHOD0(R, C, M) \
        COH_STATIC_METHOD(0, R, C, M, void, void, void, void, void, void, void, void)

/**
 * Declare a constructor Method based on a description of its signature.
 *
 * For both R, and A1..A8 the value should be a handle type, for instance
 * String::View or Map::Handle.  If the underlying method returns a value
 * type instead, then a BoxHandle<> needs to be supplied for the corresponding
 * Object type.  For instance BoxHandle<const Integer32> in place of in32_t.
 *
 * @param R       the method return type
 * @param H       the target handle type to call on
 * @param M       the method name
 * @param A1..A8  the argument types
 */
#define COH_CONSTRUCTOR8(C, A1, A2, A3, A4, A5, A6, A7, A8) \
        COH_OVERLOADED_STATIC_METHOD8(C::Handle (*)(const A1&, const A2&, \
            const A3&, const A4&, const A5&, const A6&, const A7&, const A8&), \
            C::Handle, C, create, A1, A2, A3, A4, A5, A6, A7, A8)
#define COH_CONSTRUCTOR7(C, A1, A2, A3, A4, A5, A6, A7) \
        COH_OVERLOADED_STATIC_METHOD7(C::Handle (*)(const A1&, const A2&, \
            const A3&, const A4&, const A5&, const A6&, const A7&), \
            C::Handle, C, create, A1, A2, A3, A4, A5, A6, A7)
#define COH_CONSTRUCTOR6(C, A1, A2, A3, A4, A5, A6) \
        COH_OVERLOADED_STATIC_METHOD6(C::Handle (*)(const A1&, const A2&, \
            const A3&, const A4&, const A5&, const A6&), \
            C::Handle, C, create, A1, A2, A3, A4, A5, A6)
#define COH_CONSTRUCTOR5(C, A1, A2, A3, A4, A5) \
        COH_OVERLOADED_STATIC_METHOD5(C::Handle (*)(const A1&, const A2&, \
            const A3&, const A4&, const A5&), \
            C::Handle, C, create, A1, A2, A3, A4, A5)
#define COH_CONSTRUCTOR4(C, A1, A2, A3, A4) \
        COH_OVERLOADED_STATIC_METHOD4(C::Handle (*)(const A1&, const A2&, \
            const A3&, const A4&), \
            C::Handle, C, create, A1, A2, A3, A4)
#define COH_CONSTRUCTOR3(C, A1, A2, A3) \
        COH_OVERLOADED_STATIC_METHOD3(C::Handle (*)(const A1&, const A2&, \
            const A3&), \
            C::Handle, C, create, A1, A2, A3)
#define COH_CONSTRUCTOR2(C, A1, A2) \
        COH_OVERLOADED_STATIC_METHOD2(C::Handle (*)(const A1&, const A2&), \
            C::Handle, C, create, A1, A2)
#define COH_CONSTRUCTOR1(C, A1) \
        COH_OVERLOADED_STATIC_METHOD1(C::Handle (*)(const A1&), \
            C::Handle, C, create, A1)
#define COH_CONSTRUCTOR0(C) \
        COH_OVERLOADED_STATIC_METHOD1(C::Handle (*)(), C::Handle, C, create)

/**
 * Declare an overloaded Method based on a description of its signature.
 *
 * This OVERLOADED macro is only necessary when a class or interface declares
 * multiple methods of the same name, otherwise the COH_DECLARE_METHOD is
 * a better choice of macros.
 *
 * For both R, and A1..A8 the value should be a handle type, for instance
 * String::View or Map::Handle.  If the underlying method returns a value
 * type instead, then a BoxHandle<> needs to be supplied for the corresponding
 * Object type.  For instance BoxHandle<const Integer32> in place of int32_t.
 *
 * @param S       the method signature
 * @param R       the method return type
 * @param H       the target handle type to call on
 * @param M       the method name
 * @param A1..A8  the argument types
 */
#define COH_OVERLOADED_METHOD8(S, R, H, M, A1, A2, A3, A4, A5, A6, A7, A8) \
        coherence::lang::TypedMethod<S, \
               R, H::ValueType, H, &H::ValueType::M, A1, A2, A3, A4, A5, A6, A7, A8>::create(#M)
#define COH_OVERLOADED_METHOD7(S, R, H, M, A1, A2, A3, A4, A5, A6, A7) \
        COH_OVERLOADED_METHOD8(S, R, H, M, A1, A2, A3, A4, A5, A6, A7, void)
#define COH_OVERLOADED_METHOD6(S, R, H, M, A1, A2, A3, A4, A5, A6) \
        COH_OVERLOADED_METHOD7(S, R, H, M, A1, A2, A3, A4, A5, A6, void)
#define COH_OVERLOADED_METHOD5(S, R, H, M, A1, A2, A3, A4, A5) \
        COH_OVERLOADED_METHOD6(S, R, H, M, A1, A2, A3, A4, A5, void)
#define COH_OVERLOADED_METHOD4(S, R, H, M, A1, A2, A3, A4) \
        COH_OVERLOADED_METHOD5(S, R, H, M, A1, A2, A3, A4, void)
#define COH_OVERLOADED_METHOD3(S, R, H, M, A1, A2, A3) \
        COH_OVERLOADED_METHOD4(S, R, H, M, A1, A2, A3, void)
#define COH_OVERLOADED_METHOD2(S, R, H, M, A1, A2) \
        COH_OVERLOADED_METHOD3(S, R, H, M, A1, A2, void)
#define COH_OVERLOADED_METHOD1(S, R, H, M, A) \
        COH_OVERLOADED_METHOD2(S, R, H, M, A, void)
#define COH_OVERLOADED_METHOD0(S, R, H, M) \
        COH_OVERLOADED_METHOD1(S, R, H, M, void)

/**
 * Declare a Method based on a description of its signature.
 *
 * For both R, and A1..A8 the value should be a handle type, for instance
 * String::View or Map::Handle.  If the underlying method returns a value
 * type instead, then a BoxHandle<> needs to be supplied for the corresponding
 * Object type.  For instance BoxHandle<const Integer32> in place of in32_t.
 *
 * @param R       the method return type
 * @param H       the target handle type to call on
 * @param M       the method name
 * @param A1..A8  the argument types
 */
#define COH_METHOD(N, R, H, M, A1, A2, A3, A4, A5, A6, A7, A8) \
        coherence::lang::TypedMethod<coh_signature_info<N, R, H::ValueType, H, A1, A2, \
            A3, A4, A5, A6, A7, A8>::signature, R, H::ValueType, H, \
            &H::ValueType::M, A1, A2, A3, A4, A5, A6, A7, A8>::create(#M)
#define COH_METHOD8(R, H, M, A1, A2, A3, A4, A5, A6, A7, A8) \
        COH_METHOD(8, R, H, M, A1, A2, A3, A4, A5, A6, A7, A8)
#define COH_METHOD7(R, H, M, A1, A2, A3, A4, A5, A6, A7) \
        COH_METHOD(7, R, H, M, A1, A2, A3, A4, A5, A6, A7, void)
#define COH_METHOD6(R, H, M, A1, A2, A3, A4, A5, A6) \
        COH_METHOD(6, R, H, M, A1, A2, A3, A4, A5, A6, void, void)
#define COH_METHOD5(R, H, M, A1, A2, A3, A4, A5) \
        COH_METHOD(5, R, H, M, A1, A2, A3, A4, A5, void, void, void)
#define COH_METHOD4(R, H, M, A1, A2, A3, A4) \
        COH_METHOD(4, R, H, M, A1, A2, A3, A4, void, void, void, void)
#define COH_METHOD3(R, H, M, A1, A2, A3) \
        COH_METHOD(3, R, H, M, A1, A2, A3, void, void, void, void, void)
#define COH_METHOD2(R, H, M, A1, A2) \
        COH_METHOD(2, R, H, M, A1, A2, void, void, void, void, void, void)
#define COH_METHOD1(R, H, M, A) \
        COH_METHOD(1, R, H, M, A, void, void, void, void, void, void, void)
#define COH_METHOD0(R, H, M) \
        COH_METHOD(0, R, H, M, void, void, void, void, void, void, void, void)

/**
 * Declare a Method for a Managed<> class based on a description of its
 * signature.
 *
 * For both R, and A1..A8 the value should be a handle type, for instance
 * String::View or Map::Handle.  If the underlying method returns a value
 * type instead, then a BoxHandle<> needs to be supplied for the corresponding
 * Object type.  For instance BoxHandle<const Integer32> in place of in32_t.
 *
 * @param S       the method signature
 * @param R       the method return type
 * @param H       the target handle type to call on
 * @param M       the method name
 * @param A1..A8  the argument types
 */
#define COH_MANAGED_METHOD8(S, R, H, M, A1, A2, A3, A4, A5, A6, A7, A8) \
        coherence::lang::TypedMethod<S, R, H::ValueType, H, \
            &H::ValueType::BoxedType::M, A1, A2, A3, A4, A5, A6, A7, A8>::create(#M)
#define COH_MANAGED_METHOD7(S, R, H, M, A1, A2, A3, A4, A5, A6, A7) \
        COH_MANAGED_METHOD8(S, R, H, M, A1, A2, A3, A4, A5, A6, A7, void)
#define COH_MANAGED_METHOD6(S, R, H, M, A1, A2, A3, A4, A5, A6) \
        COH_MANAGED_METHOD7(S, R, H, M, A1, A2, A3, A4, A5, A6, void)
#define COH_MANAGED_METHOD5(S, R, H, M, A1, A2, A3, A4, A5) \
        COH_MANAGED_METHOD6(S, R, H, M, A1, A2, A3, A4, A5, void)
#define COH_MANAGED_METHOD4(S, R, H, M, A1, A2, A3, A4) \
        COH_MANAGED_METHOD5(S, R, H, M, A1, A2, A3, A4, void)
#define COH_MANAGED_METHOD3(S, R, H, M, A1, A2, A3) \
        COH_MANAGED_METHOD4(S, R, H, M, A1, A2, A3, void)
#define COH_MANAGED_METHOD2(S, R, H, M, A1, A2) \
        COH_MANAGED_METHOD3(S, R, H, M, A1, A2, void)
#define COH_MANAGED_METHOD1(S, R, H, M, A) \
        COH_MANAGED_METHOD2(S, R, H, M, A, void)
#define COH_MANAGED_METHOD0(S, R, H, M) \
        COH_MANAGED_METHOD1(S, R, H, M, void)

/**
* Helper macro for creating getter and setter methods for a property.
*
* @param C  the class to call the method on, i.e. Address
* @param P  the property to access, i.e. State
* @param V  the return object type, i.e. String::View
*/
#define COH_PROPERTY(C, P, V) \
        COH_METHOD0(V, C::View, get##P)) \
      ->declare(COH_METHOD1(void, C::Handle, set##P, V)

/**
* Helper macro for creating getter and setter methods for a boxable property.
*
* @param C  the class to call the method on, i.e. Address
* @param P  the property to access, i.e. State
* @param V  the return object type, i.e. String::View
*/
#define COH_BOX_PROPERTY(C, P, V) \
        COH_METHOD0(coherence::lang::BoxHandle<V::ValueType >, C::View, get##P)) \
      ->declare(COH_METHOD1(void, C::Handle, set##P, coherence::lang::BoxHandle<V::ValueType >)

/**
* Helper macro for creating getter and setter methods for a boxable property
* on a Managed<> object.
*
* @param C  the class to call the method on, i.e. Address
* @param P  the property to access, i.e. Zip
* @param V  the return object type, i.e. Integer32::View
*/
#define COH_MANAGED_PROPERTY(C, P, V) \
        COH_MANAGED_METHOD0(V::ValueType::BoxedType (C::*)() const, coherence::lang::BoxHandle<V::ValueType >, coherence::lang::Managed<C >::View, get##P)) \
      ->declare(COH_MANAGED_METHOD1(void (C::*)(V::ValueType::BoxedType), void, coherence::lang::Managed<C >::Handle, set##P, coherence::lang::BoxHandle<V::ValueType >)

#endif // COH_TYPED_METHOD_HPP

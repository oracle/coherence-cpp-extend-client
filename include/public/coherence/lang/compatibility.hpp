/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPATIBILITY_HPP
#define COH_COMPATIBILITY_HPP

/// @cond EXCLUDE

#include <typeinfo>

// ----- identify operating system ------------------------------------------

#if defined(_WIN32)
#   define COH_OS_WINDOWS
#   define COH_LIB_PREFIX ""
#   define COH_LIB_SUFFIX ".dll"
#   if defined(_WIN64)
#      define COH_OS_WIN64
#      define COH_PLATFORM Microsoft Windows x64
#   else
#      define COH_OS_WIN32
#      define COH_PLATFORM Microsoft Windows x86
#   endif
#elif defined(__sun) || defined(sun)
#   define COH_OS_SOLARIS // Oracle Solaris
#   define COH_OS_UNIX
#   define COH_LIB_PREFIX "lib"
#   define COH_LIB_SUFFIX ".so"
#   include <sys/types.h> // for _LP64 definition
#   if defined(__sparc)
#       if defined(_LP64)
#           define COH_OS_SOLARIS64
#           define COH_PLATFORM Oracle Solaris SPARC 64b
#       else
#           define COH_OS_SOLARIS32
#           define COH_PLATFORM Oracle Solaris SPARC 32b
#       endif
#   elif defined(__x86)
#       if defined(_LP64)
#           define COH_OS_SOLARIS64
#           define COH_PLATFORM Oracle Solaris x64
#       else
#           define COH_OS_SOLARIS32
#           define COH_PLATFORM Oracle Solaris x86
#       endif
#   endif
#elif defined(__linux__)
#   define COH_OS_LINUX // Linux
#   define COH_LIB_PREFIX "lib"
#   define COH_LIB_SUFFIX ".so"
#   define COH_OS_UNIX
#   if defined(__x86_64__) || defined(__amd64__)
#       define COH_OS_LINUX64
#       define COH_PLATFORM Linux x64
#   elif defined(__x86_32__) || defined(__i386__)
#       define COH_OS_LINUX32
#       define COH_PLATFORM Linux x86
#   endif
#elif defined(__APPLE__)
#   define COH_OS_DARWIN // OS X
#   define COH_OS_UNIX
#   define COH_LIB_PREFIX "lib"
#   define COH_LIB_SUFFIX ".dylib"
#   if defined(__x86_64__) || defined(__amd64__)
#       define COH_OS_DARWIN64
#       define COH_PLATFORM Apple macOS x64
#   endif
#endif

#ifndef COH_PLATFORM
#   error "Coherence for C++ does not support this platform."
#endif


// ----- identify compiler --------------------------------------------------

#if defined(_MSC_VER) && _MSC_VER >= 1600
#   define COH_CC_MSVC // Microsoft Visual C/C++
#   if _MSC_VER == 1900
#       define COH_CC MSVC 2015 // differentiate VS 2015 build
#   else
#       define COH_CC MSVC
#   endif
#elif defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x590
#   define COH_CC_SUN // Oracle Solaris Studio
#   if defined (STLPORT)
#       define COH_CC SunPro STLport
#   else
#       define COH_CC SunPro
#   endif
#elif defined(__GNUG__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#   define COH_CC_GNU // GNU C++
#   if defined (__clang__)
#       define COH_CC LLVM (clang)
#   else
#       define COH_CC GCC
#   endif
#else
#   error "Coherence for C++ does not support this compiler or compiler version."
#endif


// ----- disable select warnings --------------------------------------------

/**
* Macro for disabling select warnings on MSVC. The warnings are automatically
* disabled upon opening a coherence namespace (using the COH_OPEN_NAMESPACE
* macros), and re-enabled when the namespace is exited via
* COH_CLOSE_NAMESPACE.
*/
#if defined(COH_CC_MSVC)
    #define COH_PRAGMA_PUSH \
    __pragma(warning(push)) \
    /* Allow multiple interface inheritance (inheritance via dominance) */ \
    __pragma(warning(disable : 4250)) \
    /* Allow non-exported DLL templates */ \
    __pragma(warning(disable : 4251)) \
    /* Exported class inheritance from non-exported class, needed for specs */ \
    __pragma(warning(disable : 4275)) \
    /* TypedHandle/Holder: multiple copy constructors */ \
    __pragma(warning(disable : 4521)) \
    /* Member/WeakHandle: assignment operators for const/non-const type */ \
    __pragma(warning(disable : 4522)) \
    /* Lack of return statements are being promoted to errors, when control \
       path results in throw */ \
    __pragma(warning(disable : 4715; disable : 4716))

    #define COH_PRAGMA_POP __pragma(warning(pop))
#else
    #define COH_PRAGMA_PUSH
    #define COH_PRAGMA_POP
#endif

/**
* Macro for temporarily disabling above warnings for the duration of a
* single statement.  This is only needed for code which is not enclosed in a
* COH_OPEN/CLOSE_NAMESPACE block.
*/
#define COH_NO_WARN(STMT) COH_PRAGMA_PUSH STMT COH_PRAGMA_POP

/**
* These macros are used to indicate that a function will not return normally.
*
* Usage example:
* @code
* COH_NO_RETURN_PRE void doSomething() COH_NO_RETURN_POST
*     {
*     COH_NO_RETURN_STMT(doSomething2());
*     }
* @endcode
*/
#if defined(COH_CC_MSVC)
    #define COH_NO_RETURN_PRE __declspec(noreturn)
    #define COH_NO_RETURN_POST
    #define COH_NO_RETURN_STMT(expr) expr
#elif defined(COH_CC_GNU)
    #define COH_NO_RETURN_PRE
    #define COH_NO_RETURN_POST __attribute__((noreturn))
    #define COH_NO_RETURN_STMT(expr) expr
#elif defined(COH_CC_SUN)
    #define COH_NO_RETURN_PRE
    #define COH_NO_RETURN_POST
    #define COH_NO_RETURN_STMT(expr)\
        do { expr; throw std::exception(); } while (0)
#else
    #define COH_NO_RETURN_PRE
    #define COH_NO_RETURN_POST
    #define COH_NO_RETURN_STMT(expr) expr
#endif


// ----- namespace macros ---------------------------------------------------

/**
* Define the existence of the coherence::lang namespace
*/
namespace coherence { namespace lang {}}

#define COH_OPEN_NAMESPACE(ns) namespace ns { \
    COH_PRAGMA_PUSH \
    using namespace coherence::lang;

#define COH_INNER_NAMESPACE(ns) namespace ns {

#define COH_OPEN_NAMESPACE2(ns1, ns2)\
    COH_OPEN_NAMESPACE (ns1) COH_INNER_NAMESPACE (ns2)

#define COH_OPEN_NAMESPACE3(ns1, ns2, ns3)\
    COH_OPEN_NAMESPACE2 (ns1, ns2) COH_INNER_NAMESPACE (ns3)

#define COH_OPEN_NAMESPACE4(ns1, ns2, ns3, ns4)\
    COH_OPEN_NAMESPACE3 (ns1, ns2, ns3) COH_INNER_NAMESPACE (ns4)

#define COH_OPEN_NAMESPACE5(ns1, ns2, ns3, ns4, ns5)\
    COH_OPEN_NAMESPACE4 (ns1, ns2, ns3, ns4) COH_INNER_NAMESPACE (ns5)

#define COH_OPEN_NAMESPACE6(ns1, ns2, ns3, ns4, ns5, ns6)\
    COH_OPEN_NAMESPACE5 (ns1, ns2, ns3, ns4, ns5) COH_INNER_NAMESPACE (ns6)

#define COH_OPEN_NAMESPACE7(ns1, ns2, ns3, ns4, ns5, ns6, ns7)\
    COH_OPEN_NAMESPACE6 (ns1, ns2, ns3, ns4, ns5, ns6) COH_INNER_NAMESPACE (ns7)

#define COH_CLOSE_NAMESPACE COH_PRAGMA_POP }

#define COH_CLOSE_NAMESPACE2 COH_PRAGMA_POP } }

#define COH_CLOSE_NAMESPACE3 COH_PRAGMA_POP } } }

#define COH_CLOSE_NAMESPACE4 COH_PRAGMA_POP } } } }

#define COH_CLOSE_NAMESPACE5 COH_PRAGMA_POP } } } } }

#define COH_CLOSE_NAMESPACE6 COH_PRAGMA_POP } } } } } }

#define COH_CLOSE_NAMESPACE7 COH_PRAGMA_POP } } } } } } }

/**
* Pseudo-anonymous namespaces.  Starting with Visual Studio 2017 it is no
* longer possible to define Coherence managed classes within an anonymous
* namespace.
*
* The parameter NAME should be unique to the parent namespace. Generally
* this can simply be the name of the primary class being defined within the
* file.
*
* For platforms other than Visual Studio 2017 and later, these macros will
* open and close a standard anonymous namespace.
*/
#if defined(COH_CC_MSVC) && _MSC_VER >= 1900
    #define COH_OPEN_NAMESPACE_ANON(NAME)\
        namespace COH_JOIN(_anon_, NAME) {}\
        using namespace COH_JOIN(_anon_, NAME);\
        namespace COH_JOIN(_anon_, NAME) {
#else
    #define COH_OPEN_NAMESPACE_ANON(NAME) namespace {
#endif

#define COH_CLOSE_NAMESPACE_ANON }

// ----- general utility macros ---------------------------------------------

/**
* This macro "mangles" the specified @a Name, producing an identifier which
* is unique in the current file.
*
* Note. This implementation of COH_UNIQUE_IDENTIFIER (as well as macros that
* use it) won't work in MSVC 6.0 when -ZI is on! See Q199057.
*
* @param Name the name to produce a new identifier on its basis
*/
#define COH_JOIN(X, Y) COH_DO_JOIN(X, Y)
#define COH_DO_JOIN(X, Y) COH_DO_JOIN2(X, Y)
#define COH_DO_JOIN2(X, Y) X##Y
#ifdef COH_CC_MSVC_NET // MSVC 2002 and later
#   define COH_UNIQUE_IDENTIFIER(Name) COH_JOIN(Name, __COUNTER__)
#else
#   define COH_UNIQUE_IDENTIFIER(Name) COH_JOIN(Name, __LINE__)
#endif

/**
* This macro will ensure initialization of function local statics at library
* load time. This should be used to force the initialization of statics
* in a thread safe way.  The general form is:
*
* SomeType::Handle staticAccessorFunction()
*     {
*     static FinalHandle<SomeType> hStatic = SomeType::create();
*     return hStatic;
*     }
* COH_STATIC_INIT(staticAccessorFunction());
*
* @param FUNC  The static function and parameters to call that requires
*             initialization.
*/
#ifdef COH_EAGER_INIT
#   define COH_STATIC_INIT_EX(N, FUNC) \
    static const bool COH_UNIQUE_IDENTIFIER(coh_static_init_func##N##_) = (FUNC, true)
#else
// some platforms (Windows) have serious restrictions about what can be safely
// performed during static initialization and so this variant of initializer
// records the initializers in the order they would be loaded to allow for
// execution after the library has been loaded. The execution can be triggered
// by running System::loadLibrary(NULL)
#    define COH_STATIC_INIT_EX(N, FUNC) \
         static void COH_UNIQUE_IDENTIFIER(coh_static_init_func##N##_)() {FUNC;} \
         static coherence::lang::coh_initializer \
                 COH_UNIQUE_IDENTIFIER(coh_static_init_reg##N##_) \
         (&COH_UNIQUE_IDENTIFIER(coh_static_init_func##N##_))
#endif

#define COH_STATIC_INIT(FUNC) COH_STATIC_INIT_EX(0, FUNC)

COH_OPEN_NAMESPACE2(coherence,lang)
    template <bool x> struct STATIC_ASSERTION_FAILURE;
    template<> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
COH_CLOSE_NAMESPACE2

/**
* This macro generates a compile time error message if the integral constant
* expression @a B is not true. In other words, it is the compile time
* equivalent of the @c assert macro. Note that if the condition is true, then
* the macro will generate neither code nor data - and the macro can also be
* used at either namespace, class or function scope. When used in a template,
* the static assertion will be evaluated at the time the template is
* instantiated; this is particularly useful for validating template
* parameters.
*
* #COH_STATIC_ASSERT can be used at any place where a declaration can be
* placed, that is at class, function or namespace scope.
*
* @param B an integral constant expression to check its trueness during
*          translation phase
*/
#define COH_STATIC_ASSERT(B)                               \
    enum { COH_UNIQUE_IDENTIFIER(coh_static_assert_enum_) =\
        sizeof(coherence::lang::STATIC_ASSERTION_FAILURE<(bool)(B)>) }

/**
* DLL import/export macros.
*/
#if defined(COH_CC_MSVC)
    #ifdef COH_BUILD
        #define COH_EXPORT  __declspec(dllexport)
    #else
        #define COH_EXPORT  __declspec(dllimport)
    #endif
    #define COH_EXPORT_SPEC  __declspec(dllexport)
    #if _MSC_VER >= 1900
        #define COH_EXPORT_SPEC_MEMBER(DECL) DECL;
    #else
        #define COH_EXPORT_SPEC_MEMBER(DECL)
    #endif
#else
    #define COH_EXPORT
    #define COH_EXPORT_SPEC
    #define COH_EXPORT_SPEC_MEMBER(DECL) DECL;
#endif

/**
* This macro will strongly encourage/force inlining of a method.
*/
#if defined(COH_CC_MSVC)
    #define COH_INLINE __forceinline
#elif defined(COH_CC_GNU)
    #define COH_INLINE __attribute__((always_inline)) inline
#else
    #define COH_INLINE __attribute__((always_inline)) inline
#endif

/**
* This macro expands to the name and signature of the current function.
*/
#if defined(__GNUC__)
    #define COH_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
    #define COH_CURRENT_FUNCTION __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
    #define COH_CURRENT_FUNCTION __func__
#else
    #define COH_CURRENT_FUNCTION "(unknown function)"
#endif

/**
 * This macro controls alignment
 */
#if defined(COH_CC_GNU)
    #define COH_ALIGN(B, TYPE, NAME) TYPE NAME __attribute__ ((aligned (B)))
#elif defined(COH_CC_SUN)
    #define COH_SUNPRO_PRAGMA(S) _Pragma(S)
    #define COH_ALIGN(B, TYPE, NAME) COH_SUNPRO_PRAGMA(COH_SYMB_STRING(align B(NAME))) TYPE NAME
#elif defined(COH_CC_MSVC)
    #define COH_ALIGN(B, TYPE, NAME) __declspec(align(B)) TYPE NAME
#else
    #error "No Coherence alignment macro for this compiler"
#endif

/**
 * Macro to declare whether a function throws exceptions.
 */
#if __cplusplus > 199711L ||\
    defined(_MSC_VER) && _MSC_VER >= 1900
#       define COH_NOEXCEPT(expr) noexcept(expr)
#else
#       define COH_NOEXCEPT(expr)
#endif

/**
 * Macro to use appropriate C++ language specific smart pointer.
 *
 * std::auto_ptr is deprecated in C++11 and its use may throw compiler warnings.
 */
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#   define COH_AUTO_PTR std::unique_ptr
#else
#   define COH_AUTO_PTR std::auto_ptr
#endif

// ----- fixed size types ---------------------------------------------------

// We need to include an std lib header here in order to detect which library
// is in use (__GLIBC__ may not be defined without this including). Use
// <utility> as it's about the smallest of the std lib headers.
#include <utility>
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901 ||\
    defined(_POSIX_VERSION) && _POSIX_VERSION >= 200100 ||\
    defined(COH_OS_LINUX) &&\
        defined(__GLIBC__) &&\
        (__GLIBC__ > 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ >= 1) &&\
        defined(__GNUC__) ||\
    defined(COH_OS_DARWIN) && __MACH__ && !defined(_MSL_USING_MSL_C)
#       define COH_HAS_STDINT_H
#endif

#if defined(__GCCXML__) ||\
    defined(__GNUC__) ||\
    defined(_MSC_EXTENSIONS)
#       define COH_HAS_LONG_LONG
#endif

#if (defined(__GLIBCPP__) || defined(__GLIBCXX__)) &&\
        !defined(_GLIBCPP_USE_LONG_LONG) &&          \
        !defined(_GLIBCXX_USE_LONG_LONG) &&          \
        defined(COH_HAS_LONG_LONG)
    // Coming here indicates that the GNU compiler supports long long type,
    // but the GNU C++ runtime library does not. Particularly, no global
    // operator<<(std::ostream&, long long) implementation is provided. Let us
    // provide it, at least with minimum incomplete functionality.
#   include <ostream>
    namespace std
        {
        template<class E, class T>
        inline basic_ostream<E, T>& operator<<
                (basic_ostream<E, T>& out, long long l)
            {
            return out << static_cast<long>(l);
            }
        template<class E, class T>
        inline basic_ostream<E, T>& operator<<
                (basic_ostream<E, T>& out, unsigned long long l)
            {
            return out << static_cast<unsigned long>(l);
            }
        }
#   undef COH_HAS_LONG_LONG
#endif

#if !defined(COH_HAS_LONG_LONG) && !defined(COH_CC_MSVC)
#   include <limits.h>
#   if defined(ULLONG_MAX) || defined(ULONG_LONG_MAX) ||\
            defined(ULONGLONG_MAX)
#       define COH_HAS_LONG_LONG
#   endif
#endif

#if defined(_MSC_VER)
#       define COH_HAS_MS_INT64
#endif

/**
* Fixed width primitive types.
*/
#if defined(COH_HAS_STDINT_H)
#   include <stdint.h>
#elif defined(COH_CC_SUN)
#   include <inttypes.h>
#else
//  This platform does not support the C99 stdint.h types. This code block
//  defines them in the global namespace, alternatively you may define
//  COH_NAMESPACED_FIXED_INTS, in which case these definitions will be within
//  the coherence::lang namespace.
#   include <climits>
#   ifdef COH_NAMESPACED_FIXED_INTS
      COH_OPEN_NAMESPACE2(coherence,lang)
#   endif
    COH_STATIC_ASSERT(UCHAR_MAX == 0xFF);
    typedef signed char   int8_t;
    typedef unsigned char uint8_t;
    COH_STATIC_ASSERT(USHRT_MAX == 0xFFFF);
    typedef short          int16_t;
    typedef unsigned short uint16_t;
#   if UINT_MAX == 0xFFFFFFFF
        typedef int          int32_t;
        typedef unsigned int uint32_t;
#   elif ULONG_MAX == 0xFFFFFFFF
        typedef long          int32_t;
        typedef unsigned long uint32_t;
#   else
#       error int size not correct
#   endif
#   if defined(COH_HAS_LONG_LONG) &&\
        !defined(COH_CC_MSVC) &&\
        (!defined(__GLIBCPP__) || defined(_GLIBCPP_USE_LONG_LONG)) && \
        (defined(ULLONG_MAX) ||\
            defined(ULONG_LONG_MAX) ||\
            defined(ULONGLONG_MAX))
#               if defined(ULLONG_MAX)
                    COH_STATIC_ASSERT(ULLONG_MAX == 0xFFFFFFFFFFFFFFFFULL);
#               elif defined(ULONG_LONG_MAX)
                    COH_STATIC_ASSERT
                        (ULONG_LONG_MAX == 0xFFFFFFFFFFFFFFFFULL);
#               elif defined(ULONGLONG_MAX)
                    COH_STATIC_ASSERT
                        (ULONGLONG_MAX == 0xFFFFFFFFFFFFFFFFULL));
#               else
#                   error long long size not correct
#               endif
                typedef long long          int64_t;
                typedef unsigned long long uint64_t;
#   elif ULONG_MAX != 0xFFFFFFFF
        COH_STATIC_ASSERT(ULONG_MAX == 0xFFFFFFFFFFFFFFFFULL);
        typedef long          int64_t;
        typedef unsigned long uint64_t;
#   elif defined(__GNUC__) && defined(COH_HAS_LONG_LONG)
        __extension__ typedef long long          int64_t;
        __extension__ typedef unsigned long long uint64_t;
#   elif defined(COH_HAS_MS_INT64)
        typedef __int64          int64_t;
        typedef unsigned __int64 uint64_t;
#   else
#       error no 64-bit integer support
#   endif
#   ifdef COH_NAMESPACED_FIXED_INTS
        COH_CLOSE_NAMESPACE2
#   endif
#endif

/**
* Non-standard primitive type definitions.
*/
COH_OPEN_NAMESPACE2(coherence,lang)
    typedef unsigned char octet_t;
    typedef uint16_t      wchar16_t;
    typedef uint32_t      size32_t;
    typedef uint64_t      size64_t;
    typedef float         float32_t; COH_STATIC_ASSERT(sizeof(float32_t) >= sizeof(int32_t));
    typedef double        float64_t; COH_STATIC_ASSERT(sizeof(float64_t) >= sizeof(int64_t));
COH_CLOSE_NAMESPACE2

/**
* Produce a 64b value from two 32b parts.
*
* This is a bit-wise construction, and the supplied values should be the
* bitwise unsigned representations.  For example:
* COH_INT64(0x7FFFFFFFU, 0xFFFFFFFFU) == 0x7FFFFFFFFFFFFFFFLL
*/
#define COH_INT64(HIGH, LOW) int64_t(uint64_t(HIGH) << 32 | uint64_t(LOW))

// macros for turning compiler supplied define into a string
#define COH_SYMB_STRING(SYMB) #SYMB
#define COH_SYMB_TO_STRING(SYMB) COH_SYMB_STRING(SYMB)


// ----- helpers ------------------------------------------------------------

#include <typeinfo>
COH_OPEN_NAMESPACE2(coherence,lang)
    class Class;

    typedef void (*coh_static_initializer)();

    /**
    * Register an initialization function with the runtime.
    */
    extern COH_EXPORT void coh_register_initializer(
            coh_static_initializer pInit, bool fAdd);

    /**
     * Helper class for registering and unregistering static initializers.
     */
    class coh_initializer
        {
        public:

        coh_initializer(coh_static_initializer pInit)
            : m_pInit(pInit)
            {
            coh_register_initializer(pInit, true);
            }

        ~coh_initializer()
            {
            coh_register_initializer(m_pInit, false);
            }

        private:
        coh_static_initializer m_pInit;
        };

    /**
    * Helper functions for throwing exceptions.
    */
    extern COH_EXPORT void coh_throw_npe(const std::type_info&);
    extern COH_EXPORT void coh_throw_class_cast(const std::type_info&,
            const std::type_info&);
    extern COH_EXPORT void coh_throw_const_cast(const std::type_info&,
            const std::type_info&);
    extern COH_EXPORT void coh_throw_illegal_state(const char* achMsg);
    extern COH_EXPORT void coh_throw_illegal_argument(const char* achMsg);
    extern COH_EXPORT void coh_throw_unsupported_operation(const char* achMsg);
    extern COH_EXPORT const Class* coh_loadClassByType(const std::type_info& ti);

    /**
    * Helper class used to test for type assignment compatibility at
    * compile time.
    *
    * This implementation is based on the Conversion example from
    * Andrei Alexandrescu's Modern C++ Design.
    */
    template<class A, class B>
    class assignment
        {
        protected:
            typedef char  PathA;
            class   Other {char unused[2];}; // sizeof(Other) != sizeof(PathA)
            static  PathA route(A*); // PathA taken only for compatible types
            static  Other route(...); // incompatible types go this route
            static  B*    test(); // "generate" a test object

        public:
            /**
            * Convert a derived A to an A.
            */
            static A* safe(A* a)
                {
                return a;
                }

            /**
            * Dummy conversion (should never be called)
            */
            static A* safe(...)
                {
                coh_throw_illegal_state("unsafe cast");
                return 0;
                }

        public:
            /**
            * True iff A = B is allowed.
            */
            enum {allowed = (sizeof(route(test())) == sizeof(PathA))};
        };

    /**
    * Helper class used to test if a type has been declared as const.
    */
    template<class A>
    class constness
        {
        public:
            enum {applied = assignment<A, const A>::allowed};
        };
/**
 * Coherence Class and Interface ID helpers.
 */
#if defined(COH_CC_MSVC)
    // MSVC utilizes identical code folding ICF, which makes the alternate
    // very cheap form of id generation unusable and effectively assigns
    // all classes the same id, thus on MSVC we fallback on standard and
    // slower typeid

    /**
     * Coherence class id type.
     */
    typedef const std::type_info& coh_class_id;

    /**
     * Return the class id for a given managed class.
     */
    #define COH_CLASS_ID(CLASS) CLASS::_classId()

    /**
     * @internal
     *
     * Hook for COH_CLASS_ID macro, not for direct use
     */
    #define COH_GENERATE_CLASS_ID(T) \
        static coh_class_id _classId() \
            { \
            return typeid(T); \
            }
#else
    // For most compilers we can use the address of a class specific static
    // function as a unique class identifier. This is cheaper then using
    // the standard typeid operator, which on some platforms adds contention

    /**
     * Coherence class id type.
     */
    typedef void (*coh_class_id)();

    /**
     * Return the class id for a given managed class.
     */
    #define COH_CLASS_ID(CLASS) &CLASS::_classId

    /**
     * @internal
     *
     * Hook for COH_CLASS_ID macro, not for direct use
     */
    #define COH_GENERATE_CLASS_ID(T) static void _classId() {}
#endif

COH_CLOSE_NAMESPACE2

/// @endcond

#endif // COH_COMPATIBILITY_HPP

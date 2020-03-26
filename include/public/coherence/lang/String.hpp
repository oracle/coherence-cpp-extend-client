/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_STRING_HPP
#define COH_STRING_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Array.hpp"
#include "coherence/lang/Comparable.hpp"
#include "coherence/lang/Object.hpp"

#include <memory>

#include <sstream>
#include <string>

COH_OPEN_NAMESPACE2(coherence,lang)

/**
* @internal
*
* Used to protect protected inheritance of Array<octet_t> by String, as
* spec based class definitions don't have a notion of protected
* inheritance.
*/
class COH_EXPORT_SPEC ProtectedOctetArray
    : protected Array<octet_t>
    {
    public:
        typedef Array<octet_t>::super super;
        typedef Array<octet_t>::alias alias;

    protected:
        ProtectedOctetArray(size32_t cb, octet_t* ab)
            : Array<octet_t>(cb, ab)
            {}

        ProtectedOctetArray(ProtectedOctetArray::View vThat,
            size32_t iFrom, size32_t iTo)
            : Array<octet_t>(vThat, iFrom, iTo)
            {}

        virtual ~ProtectedOctetArray()
            {}
    };


/**
* A managed C-style (NUL terminated) string.
*
* In addition to exposing the underlying char array, the String class
* supports transformations to and from Unicode code points within the Basic
* Multilingual Plane (BMP):
*
* <ul>
* <li>UTF-8  BMP char array</li>
* <li>UTF-16 BMP wchar_t array (on platforms where wchar_t is >= 16 bits)</li>
* <li>UTF-8  BMP octet_t array</li>
* <li>UTF-16 BMP wchar16_t array</li>
* </ul>
*
* Note: the ASCII character set is a subset of UTF-8 BMP.
*
* Unlike most managed types in the Coherence class hierarchy, Strings are
* auto-boxable by default. That is a String::Handle or String::View can be
* directly assigned from or to common string representations.  For example
* the following code is legal:
* @code
* String::Handle hs = "hello world";
* @endcode
* as is
* @code
* void someFunction(String::View vs);
*
* someFunction("some value");
* @endcode
*
* @see StringHandle for details
*
* @author mf/jh/djl  2007.07.05
*/
class COH_EXPORT String
    : public cloneable_spec<String,
        extends<ProtectedOctetArray>,
        implements<Comparable> >
    {
    friend class factory<String>;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);


    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * While StringHandle boxes a number of common string types, String is
        * still compatible with BoxHandle, and when used with it can box to
        * only one type. By default Strings are boxable from a number of
        * types, see StringHandle for details.
        */
        typedef std::string BoxedType;


    // ----- nested class: StringHandle -------------------------------------

    public:
        /**
        * StringHandle provides standard TypedHandle features as well as
        * auto-boxing support for standard string types including:
        *
        * <ul>
        * <li>char[]       C-style NUL terminated char array</li>
        * <li>std::string  STL string</li>
        * <li>std::wstring STL wide string</li>
        * </ul>
        *
        * Boxing from wchar_t[] is supported, but requires an explicit
        * constructor call in order to avoid ambiguity when assigning a
        * String handle/view to NULL.
        *
        * Unboxing to char[] and wchar[] is not supported as it is unsafe to
        * maintain a reference to the underlying character array without
        * holding a reference to the String. Unboxing to std::string, and
        * std::wstring is both supported and safe.
        */
        template<class T> class StringHandle
            : public TypedHandle<T>
            {
            // ----- constructors ---------------------------------------

            public:
                /**
                * Create an empty StringHandle.
                */
                StringHandle()
                    : TypedHandle<T>()
                    {
                    }

                /**
                * Create a new StringHandle from a boxable type.
                */
                StringHandle(const char* ach)
                    : TypedHandle<T>()
                    {
                    if (NULL != ach)
                        {
                        TypedHandle<T>::operator=(T::create(ach));
                        }
                    }

                /**
                * Create a new StringHandle from a boxable type.
                */
                explicit StringHandle(const wchar_t* ach)
                    : TypedHandle<T>()
                    {
                    if (NULL != ach)
                        {
                        TypedHandle<T>::operator=(T::create(ach));
                        }
                    }

                /**
                * Create a new StringHandle from a boxable type.
                */
                template<class C, class R, class A> COH_INLINE
                StringHandle(const std::basic_string<C, R, A>& s)
                    : TypedHandle<T>(T::create(s))
                    {
                    }

                /**
                * Create a new StringHandle from the TypedHandle with a type
                * conversion.
                */
                template<class O> StringHandle<T>(const TypedHandle<O>& that)
                    : TypedHandle<T>(that)
                    {
                    }

                /**
                * The copy constructor.
                */
                StringHandle(const StringHandle& that)
                    : TypedHandle<T>(that)
                    {
                    }

                /**
                * Create a new StringHandle from the raw pointer.
                */
                explicit StringHandle(T* o)
                    : TypedHandle<T>(o)
                    {
                    }

            // ----- operators ------------------------------------------

            public:
                /**
                * The assignment operator.
                */
                template<class O>
                StringHandle& operator=(const TypedHandle<O>& that)
                    {
                    TypedHandle<T>::operator=(that);
                    return *this;
                    }

                /**
                * The "boxing" operator.
                */
                StringHandle& operator=(const char* ach)
                    {
                    if (NULL == ach)
                        {
                        TypedHandle<T>::operator=(NULL);
                        }
                    else
                        {
                        TypedHandle<T>::operator=(T::create(ach));
                        }
                    return *this;
                    }

                /**
                * The "boxing" operator.
                */
                template<class C, class R, class A> COH_INLINE
                StringHandle& operator=(const std::basic_string<C, R, A>& s)
                    {
                    TypedHandle<T>::operator=(T::create(s));
                    return *this;
                    }

                /**
                * The "unboxing" operator.
                *
                * @return a copy of the referenced Object
                */
                template<class C, class R, class A> COH_INLINE
                operator std::basic_string<C, R, A>() const
                    {
                    const T* pT = TypedHandle<T>::get();
                    if (NULL == pT)
                        {
                        coh_throw_npe(typeid(T));
                        }
                    return (std::basic_string<C, R, A>) *pT;
                    }
            };

    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Handle definition.
        */
        typedef StringHandle<String> Handle;

        /**
        * View definition.
        */
        typedef StringHandle<const String> View;


    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a String from a C-style NUL terminated char array.
        *
        * @param ach  the NUL terminated string of chars to copy
        * @param cch  the number of chars to copy; if npos, until NUL
        *
        * @throws IllegalArgumentException if any of the elements in the
        *         array are not UTF-8 BMP
        */
        static String::Handle create(const char* achSrc = "", size32_t cch = npos);

        /**
        * Create a String from a C-style NUL terminated wide char array.
        *
        * @param ach  the NUL terminated string of wide chars to copy
        * @param cch  the number of chars to copy; if npos, copy until NUL
        *
        * @throws IllegalArgumentException if any of the elements in the
        *         array are not UTF-16 BMP
        */
        static String::Handle create(const wchar_t* achSrc, size32_t cch = npos);

        /**
        * Create a String from an STL string.
        *
        * @param s  the STL string to copy
        *
        * @throws IllegalArgumentException if any of the elements in the
        *         array are not UTF-8 BMP
        */
        template<class C, class R, class A> static COH_INLINE
            String::Handle create(const std::basic_string<C, R, A>& s)
            {
            size_t cch = s.size();
            if (cch >= npos) // for 64b
                {
                coh_throw_illegal_argument("maximum String length exceeded");
                }
            return String::create(s.data(), size32_t(cch));
            }

        /**
        * Create a String from a char array.
        *
        * @param vach  the array of chars to copy
        * @param of    the offset at which to start copying
        * @param cch   the number of chars to copy; if npos, copy all
        *              subsequent chars in the array
        *
        * @throws IndexOutOfBoundsException if of > vach->length or if
        *         cch < npos and of + cch > vach->length
        * @throws IllegalArgumentException if any of the elements in the
        *         array are not UTF-8 BMP
        */
        static String::Handle create(Array<char>::View vachSrc,
                size32_t of = 0, size32_t cch = npos);

        /**
        * Create a String from a wide char array.
        *
        * @param vach  the array of chars to copy
        * @param of    the offset at which to start copying
        * @param cch   the number of chars to copy; if npos, copy all
        *              subsequent chars in the array
        *
        * @throws IndexOutOfBoundsException if of > vach->length or if
        *         cch < npos and of + cch > vach->length
        * @throws IllegalArgumentException if any of the elements in the
        *         array are not UTF-16 BMP
        * @throws UnsupportedOperationException if sizeof(wchar_t) <
        *         sizeof(wchar16_t)
        */
        static String::Handle create(Array<wchar_t>::View vachSrc,
                size32_t of = 0, size32_t cch = npos);

        /**
        * Create a String from an octet array.
        *
        * @param vab  the array of octets to copy
        * @param of   the offset at which to start copying
        * @param cb   the number of octets to copy; if npos, copy all
        *             subsequent octets in the array
        *
        * @throws IndexOutOfBoundsException if of > vab->length or if
        *         cb < npos and of + cb > vab->length
        * @throws IllegalArgumentException if any of the elements in the
        *         array are not UTF-8 BMP
        */
        static String::Handle create(Array<octet_t>::View vabSrc,
                size32_t of = 0, size32_t cb = npos);

        /**
        * Create a String from a 16-bit char array.
        *
        * @param vach  the array of chars to copy
        * @param of    the offset at which to start copying
        * @param cch   the number of chars to copy; if npos, copy all
        *              subsequent chars in the array
        *
        * @throws IndexOutOfBoundsException if of > vach->length or if
        *         cch < npos and of + cch > vach->length
        * @throws IllegalArgumentException if any of the elements in the
        *         array are not UTF-16 BMP
        */
        static String::Handle create(Array<wchar16_t>::View vachSrc,
                size32_t of = 0, size32_t cch = npos);

        /**
        * Create a String from another String.
        *
        * Needed for clone().
        *
        * @param that  the String to copy
        *
        * @since Coherence 3.7.1.8
        */
        static String::Handle create(const String& that);


    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Constructor.
        *
        * @param ccp  the number of code points in the string
        * @param cb   the number of octets in the string
        * @param ab   the String's octets
        * @param fCs  true if ab represents a c string
        */
        String(size32_t ccp, size32_t cb, octet_t* ab, bool fCs);

        /**
        * Copy constructor.
        */
        String(const String& that);


    // ----- String interface -----------------------------------------------

    public:
        /**
        * Return true iff the String contains only ASCII (ISO-8859-1)
        * characters. In this case each character is represented by a single
        * char, otherwise a character can take between one and three chars.
        *
        * @return true iff the String contains only ASCII characters
        */
        virtual bool isASCII() const;

        /**
         * Return true if the String is ASCII and does not contain embedded nuls.
         *
         * @return true if the String is a C string.
         */
        virtual bool isCString() const;

        /**
        * Return the number of unicode code points (characters) in this String.
        *
        * @return the number of characters in this String
        */
        virtual size32_t length() const;

        /**
         * Returns true if, and only if, length() is 0.
         *
         * @return true if length() is 0, otherwise false
         *
         * @since 12.2.1
         */
        virtual bool isEmpty() const;

        /**
        * Return the String as a C-style NUL terminated char array.
        *
        * If the String is non-ASCII then the String::next() method may be
        * used to expand the char array into a sequence of wchar16_t unicode
        * characters.
        *
        * The returned array's lifetime is bound to the lifetime of the
        * String which it was returned from. Specifically it is unsafe to use
        * the returned char* while not holding a handle to the String.
        *
        * @return the char array representing the String.
        */
        virtual const char* getCString() const;

        /**
        * Compare this String against the supplied C-style string.
        *
        * @param ach  the NUL terminated C-style string to compare to this
        *             String
        * @param cch  the length of the supplied string, or npos to rely on
        *             NUL terminator
        *
        * @return true iff the two strings are identical
        */
        virtual bool equals(const char* ach, size32_t cch = npos) const;

        /**
        * Compare this String against the supplied C-style wide char string.
        *
        * @param ach  the NUL terminated C-style string to compare to this
        *             String
        * @param cch  the length of the supplied string, or npos to rely on
        *             NUL terminator
        *
        * @return true iff the two strings are identical
        *
        * @throws UnsupportedOperationException if sizeof(wchar_t) < sizeof(wchar16_t)
        */
        virtual bool equals(const wchar_t* ach, size32_t cch = npos) const;

        /**
        * Compare this String against the supplied STL string or wstring.
        *
        * @param s  the STL string to compare to this String
        *
        * @return true iff the two strings are identical
        */
        template<class C, class R, class A> COH_INLINE
            bool equalsStd(const std::basic_string<C, R, A>& s) const
            {
            size_t cch = s.size();
            return cch < npos && equals(s.data(), size32_t(cch));
            }

        /**
        * Convert the String to any of the types supported by StringHandle,
        * namely an STL string or wstring.
        *
        * @return the std::string/wstring representation
        */
        template<class C, class R, class A> COH_INLINE
            operator std::basic_string<C, R, A>() const
            {
            if (sizeof(C) == sizeof(octet_t))
                {
                return std::basic_string<C, R, A>((const C*) getCString(),
                        super::length - 1);
                }

            if (sizeof(C) < sizeof(wchar16_t))
                {
                coh_throw_unsupported_operation("unsupported string type");
                }

            typename std::basic_string<C, R, A>::size_type cch =
                typename std::basic_string<C, R, A>::size_type(length());
            const char* iter = getCString();
            std::basic_string<C, R, A> ws;
            ws.reserve(cch);
            for (typename std::basic_string<C, R, A>::size_type
                    i = 0; i < cch; ++i)
                {
                ws.push_back((C) String::next(iter));
                }
            return ws;
            }

        /**
        * Return the index of a substring within this String.
        *
        * @param vsSearch  the substring to search for in vsSource
        * @param iBegin    the location in the string to start searching
        *
        * @return the index of the substring found within this String or npos
        */
        virtual size32_t indexOf(String::View vsSearch,
                size32_t iBegin = 0) const;

        /**
        * Return the index of a character within this String.
        *
        * @param chSearch  the character to search for in this String
        * @param iBegin    the location in this String to start searching
        *
        * @return the index of the character found within this String or npos
        */
        virtual size32_t indexOf(wchar16_t chSearch,
                size32_t iBegin = 0) const;

        /**
        * Return the index of a substring within this String by searching
        * backward from the given beginning index.
        *
        * @param vsSearh  the substring to search for within this String
        * @param iBegin   the location in this String to start searching
        *
        * @return the index of the substring found within this String or npos
        */
        virtual size32_t lastIndexOf(String::View vsSearch,
                size32_t iBegin = npos) const;

        /**
        * Return the index of a substring within this String by searching
        * backward from the given beginning index.
        *
        * @param chSearch  the character to search for in this String
        * @param iBegin    the location in this String to start searching
        *
        * @return the index of the character found within this String or npos
        */
        virtual size32_t lastIndexOf(wchar16_t chSearch,
                size32_t iBegin = npos) const;

        /**
        * Return a new String that is a substring of this string. The substring
        * begins at the specified iBegin and extends to the character at
        * index iEnd - 1.  Thus the length of the substring is iEnd-iBegin.
        *
        * @param iBegin    the starting index from which to create the string
        * @param iEnd      the index of where the substring should stop
        *                  in this String or npos for end of string
        *
        * @throws IndexOutOfBoundsException  if iEnd is larger than the length 
        *                                    of this String object, or if iBegin
        *                                    is larger than iEnd.
        *
        * @return the new substring created from this String
        */
        virtual String::View substring(size32_t iBegin,
                size32_t iEnd = npos) const;

        /**
        * Return true if this String starts with the supplied String.
        *
        * @param vsSearch  the string to search for
        *
        * @return true if this String starts with vsSearch
        */
        virtual bool startsWith(String::View vsSearch) const;

        /**
        * Return true if this String ends with the supplied String.
        *
        * @param vsSearch  the string to search for
        *
        * @return true if this String ends with vsSearch
        */
        virtual bool endsWith(String::View vsSearch) const;

        /**
        * A substring of this String is compared to a substring of a supplied
        * String.
        *
        * @param ofSource  the offset in this String where comparison begins
        * @param vsOther   the String whose substring is compared against
        *                  this String
        * @param ofOther   the offset in vsOther where comparison begins
        * @param cch       the count of characters to compare, or npos for
        *                  (vsOther->length - ofOther)
        *
        * @return the result of the two substrings
        */
        virtual bool regionMatches(size32_t ofSourse,
                String::View vsOther, size32_t ofOther = 0,
                size32_t cch = npos) const;

        /**
        * Return a String that is the result of removing all leading and
        * trailing white space.
        *
        * @return a trimmed copy of this String
        */
        String::View trim() const;

        /**
        * Return the underlying UTF-8 BMP NUL terminated Array<octet_t>.
        *
        * For performance reasons the returned Array may not support cloning.
        * If clone() is called the result will a String, which depending on
        * the compiler's handling of dynamic_cast to a private super class may
        * fail to be castable to an Array<octet_t>.
        *
        * @return the Array<octet_t>
        */
        virtual Array<octet_t>::View getOctets() const;

    // ----- static methods -------------------------------------------------

    public:
        /**
         * Returns the string representation of the {@code v} argument.
         *
         * @param   v  an Object.
         *
         * @return  if the argument is {@code NULL}, then a string equal to
         *          {@code "NULL"}; otherwise, the value of
         *          {@code v->toString()} is returned
         *
         * @see     Object#toString()
         *
         * @since 12.2.1
         */
        static String::View valueOf(Object::View v);

    // ----- Array interface ------------------------------------------------

    protected:
        using Array<octet_t>::regionMatches;


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isImmutable() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size64_t sizeOf(bool fDeep = false) const;

    // ----- static helpers -------------------------------------------------

    public:
        /**
        * Return the Unicode character as UTF-16 from the char array, and
        * increment the pointer such that it references the start of the
        * next Unicode character.
        *
        * @param ach  pointer to the start of the next UTF-8 code point.
        *
        * @return the next Unicode character
        *
        * @throws IllegalArgumentException  if a non UTF-8 BMP sequence is
        *                                   encountered
        */
        static wchar16_t next(const char*& ach);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of unicode code points (characters) in the String.
        */
        size32_t m_ccp;

        /**
         * True iff the String is a C string.
         */
        bool m_fCString;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * String referencing NULL.
        *
        * This constant is generally only needed for defining a default
        * value for a function parameter:
        *
        * @code
        * void function(String::View vs = String::null_string)
        * @endcode
        *
        * Simply passing NULL as a default is not allowable for Strings as due
        * to auto-boxing the compiler is unable to determine if NULL indicates
        * a String* or a char*. For all other uses of NULL with String the
        * literal NULL is preferred.
        */
        static const char* const null_string;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the specified Object to the given
* stream.
*
* @param out  the stream used to output the description
* @param o    the Object to describe
*
* @return the supplied stream
*/
COH_INLINE std::ostream& operator<<(std::ostream& out, const Object& o)
    {
    // legacy support for deprecated toStream method, test if toStream or toString has been overridden
    // this is relatively expensive and will be removed in a future release when toStream is removed
    std::stringstream ss;
    o.toStream(ss); // no-op on Object
    if (ss.tellp() <= 0) // common case - on empty stream STLport returns -1; others return 0
        {
        // toStream has not been overridden, use toString
        String::View vs = o.toString(); // ensure String stays alive while working with the c string
        if (vs->isCString()) // NUL terminated ASCII
            {
            out << vs->getCString();
            }
        else if (vs->isASCII()) // ASCII with embedded NULs
            {
            const char* pch = vs->getCString();
            for (size32_t i = 0, c = vs->length(); i < c; ++i)
                {
                out << pch[i];
                }
            }
        else // unicode, "inherit" however std::string outputs embedded unicode to an ostream
            {
            std::string s = vs;
            out << s;
            }
        }
    else // toStream wrote output (was overridden)
        {
        out << ss.str();
        }
    return out;
    }

/**
* Output a human-readable description of the specified Object to the given
* stream.
*
* @param out  the stream used to output the description
* @param o    the Object to describe
*
* @return the supplied stream
*/
COH_INLINE std::wostream& operator<<(std::wostream& out, const Object& o)
    {
    String::View vs = o.toString(); // ensure String stays alive while working with the c string
    const char* pch = vs->getCString();
    if (vs->isCString())  // NUL terminated ASCII
        {
        out << pch;
        }
    else if (vs->isASCII()) // ASCII with embedded NULs
        {
        for (size32_t i = 0, c = vs->length(); i < c; ++i)
            {
            out << pch[i];
            }
        }
    else // unicode, decode to wchars
        {
        for (size32_t i = 0, c = vs->length(); i < c; ++i)
            {
            out << wchar_t(String::next(pch));
            }
        }
    return out;
    }

// ----- helper macros ------------------------------------------------------

/**
* This macro will take any set of wstreamable contents and turn them into a
* coherence#lang#String instance.
*
* @param CONTENTS  the contents to use in constructing the String.
*
* Usage example:
* @code
* String::Handle hsFoo = COH_TO_WIDE_STRING("This value: " << 5 << " is my value");
* @endcode
*/
#define COH_TO_WIDE_STRING(CONTENTS) \
    coherence::lang::String::create(((std::wstringstream&) \
            (*(COH_AUTO_PTR<std::wstringstream>(new std::wstringstream())) \
                << CONTENTS)).str())

/**
* This macro will take any set of streamable contents and turn them into a
* coherence#lang#String instance.
*
* @param CONTENTS  the contents to use in constructing the String.
*
* Usage example:
* @code
* String::Handle hsFoo = COH_TO_NARROW_STRING("This value: " << 5 << " is my value");
* @endcode
*/
#define COH_TO_NARROW_STRING(CONTENTS) \
    coherence::lang::String::create(((std::stringstream&) \
            (*(COH_AUTO_PTR<std::stringstream>(new std::stringstream())) \
                << CONTENTS)).str())

/**
 * Compile time option to select the default operator<< to use. If COH_DEFAULT_NARROW_TO_STRING
 * is defined then the COH_TO_STRING macro will only require operator<< to be defined
 * for std::ostream, alternatively if COH_DEFAULT_WIDE_TO_STRING is then operator<< must be
 * defined for std::wostream. The preferable choice is to implement them both via a single template
 * based implementation:
 *
 * template <typename Char, typename Traits> std::basic_ostream<Char, Traits>&
 *     operator<<(std::basic_ostream<Char, Traits>& out, const your_class& v)
 *
 * Note all managed classes deriving from Object (including Managed) are already compatible
 * with both stream types, the above operators only need be defined for non-managed
 * classes.
 */
#if defined(COH_DEFAULT_NARROW_TO_STRING)
    #define COH_TO_STRING(CONTENTS) COH_TO_NARROW_STRING(CONTENTS)
#elif defined(COH_DEFAULT_WIDE_TO_STRING)
    #define COH_TO_STRING(CONTENTS) COH_TO_WIDE_STRING(CONTENTS)
#elif defined(COH_BUILD) // default coherence internals build to wide
    #define COH_TO_STRING(CONTENTS) COH_TO_WIDE_STRING(CONTENTS)
#else // default users to narrow not all types (includeing std::string) are compatible with std::wostream
    #define COH_TO_STRING(CONTENTS) COH_TO_NARROW_STRING(CONTENTS)
#endif

COH_CLOSE_NAMESPACE2

#endif // COH_STRING_HPP

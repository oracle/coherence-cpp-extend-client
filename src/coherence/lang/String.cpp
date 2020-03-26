/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/String.hpp"

#include "coherence/lang.ns"

#include <algorithm>
#include <string.h>
#include <wchar.h>


// NOTE: This file must not take or return std::strings.  Any use of
// std::string must be confined to String.hpp in order to protect against
// having a different std::string implementation being passed in from the
// application then what our library was compiled against.  The most notible
// place where this occurs is in MSVC debug versus release libraries, the
// std::string has a different memory layout, and is esentially corrupt once
// it reaches this code.  By handling it entirely within the header, the user
// code is the only thing which interacts with the actuall std::string, and
// it just passes the underlying char* into this file.

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<String>::create());

// compatibility.hpp typedefs octet_t as "unsinged char", so this assertion
// will only fail if someone changes that, which is not allowed, String
// requires that they are the same size
COH_STATIC_ASSERT(sizeof(octet_t) == sizeof(char));

// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Return the Unicode character as UTF-16 from the UTF-8 octet array,
    * and increment the pointer such that it references the start of the
    * next Unicode character.
    *
    * @param ab  pointer to the start of the next UTF-8 code point.
    *
    * @return the next Unicode character
    *
    * @throws IllegalArgumentException  if a non UTF-8 BMP sequence is
    *                                   encountered
    */
    inline wchar16_t _next(const octet_t*& ab)
        {
        wchar16_t ch;
        switch (*ab & 0xF0)
            {
            case 0xE0:
                // 3-octet format:  1110 xxxx, 10xx xxxx, 10xx xxxx
                ch = ((wchar16_t(ab[0]) & 0x00F) << 12) |
                     ((wchar16_t(ab[1]) & 0x03F) << 6)  |
                      (wchar16_t(ab[2]) & 0x03F);
                ab += 3;
                break;

            case 0xD0:
            case 0xC0:
                // 2-octet format:  110x xxxx, 10xx xxxx
                ch = ((wchar16_t(ab[0]) & 0x01F) << 6) |
                      (wchar16_t(ab[1]) & 0x03F);
                ab += 2;
                break;

            case 0x70:
            case 0x60:
            case 0x50:
            case 0x40:
            case 0x30:
            case 0x20:
            case 0x10:
            case 0x00:
                // 1-octet format:  0xxx xxxx
                ch = wchar16_t(*ab++) & 0x07F;
                break;

            default:
                // 4-octet format: non-BMP
                COH_THROW_STREAM (IllegalArgumentException,
                        "encountered non-BMP Unicode character 0x" << std::hex
                        << uint8_t(*ab) << std::dec);
            }

        return ch;
        }

    /**
    * Increment the given pointer until you have skipped the specified number
    * of character positions.  Use pointer arithmetic optimization if ASCII.
    *
    * @param ab        pointer to the start of the next UTF-8 code point.
    * @param cch       the count of characters to advance
    * @param fASCII    flag indicating ASCII
    *
    * @throws IllegalArgumentException  if a non UTF-8 BMP sequence is
    *                                   encountered
    */
    inline void _skip(const octet_t*& ab, size32_t cch, bool fASCII)
        {
        if (fASCII)
            {
            ab += cch;
            return;
            }

        while (cch--)
           {
           octet_t b = *ab++;
           if ((b & 0x80) != 0)
              {
              ++ab;
              if ((b & 0xF0) == 0xE0)
                 {
                 ++ab;
                 }
              }
           }
       }

    /**
    * Decrement the pointer such that it references the start of the previous
    * Unicode character and return that Unicode character.
    *
    * @param ab  pointer to the start of the next UTF-8 code point.
    *
    * @return the previous Unicode character
    *
    * @throws IllegalArgumentException  if a non UTF-8 BMP sequence is
    *                                   encountered
    */
    inline wchar16_t _prev(const octet_t*& ab)
        {
        // 1-octet format:  0xxx xxxx
        // 2-octet format:  110x xxxx, 10xx xxxx
        // 3-octet format:  1110 xxxx, 10xx xxxx, 10xx xxxx
        octet_t b = *(--ab);
        if ((b & 0x80) == 0)
            {
            return (wchar16_t) b;
            }

        if ((b & 0xC0) == 0x80)
            {
            b = *(--ab);
            if ((b & 0xE0) == 0xC0)
                {
                return ((wchar16_t(b)     & 0x01F) << 6) |
                        (wchar16_t(ab[1]) & 0x03F);
                }
            else if ((b & 0xC0) == 0x80)
                {
                b = *(--ab);
                if ((b & 0xF0) == 0xE0)
                    {
                    return ((wchar16_t(b)     & 0x00F) << 12) |
                           ((wchar16_t(ab[1]) & 0x03F) << 6)  |
                            (wchar16_t(ab[2]) & 0x03F);
                    }
                }
            }

        COH_THROW_STREAM (IllegalArgumentException,
                "encountered non-BMP Unicode character 0x" << std::hex
                << uint8_t(b) << std::dec);
        }

    /**
    * Decrement the given pointer until you have skipped backwards the
    * specified number of character positions.  Use pointer arithmetic
    * optimization if ASCII.
    *
    * @param ab      pointer to the start of the next UTF-8 code point.
    * @param cch     the count of characters to retreat
    * @param fASCII  flag indicating ASCII
    *
    * @return the previous Unicode character after retreating cch characters
    *
    * @throws IllegalArgumentException  if a non UTF-8 BMP sequence is
    *                                   encountered
    */
    inline void _back(const octet_t*& ab, size32_t cch, bool fASCII)
        {
        if (fASCII)
            {
            ab -= cch;
            return;
            }

        while (cch--)
            {
            while ((*(--ab) & 0xC0) == 0x80);
            }
        }

    /**
    * Validate the UTF-8 string.
    *
    * @param a    the array to scan
    * @param c    the number of elements in the string, or npos if the size
    *             is unknown but the string is NUL terminated
    * @param ccp  return the number of code points in the string
    * @param fN   return true if the string contains embedded NULs
    *
    * @return the number of octets required to store this string UTF-8 format
    *
    * @throws IllegalArgumentException if the string is not a valid UTF-8 BML string
    */
    template<class T> size32_t validateUTF8String(const T* a, size32_t c, size32_t& ccp, bool& fN)
        {
        if (NULL == a)
            {
            COH_THROW (NullPointerException::create());
            }

        size32_t cb = 0;
        ccp = 0;
        for (T ch; cb < c; ++ccp)
            {
            ch = a[cb];
            switch (ch & 0xF0)
                {
                case 0xF0:
                    // 4-octet format:  1111 0xxx, 10 xx xxxx, 10xx xxxx, 10xx xxxx
                    COH_THROW_STREAM (IllegalArgumentException,
                            "encountered non-BMP Unicode character 0x" <<
                            std::hex << uint32_t(ch) << std::dec <<
                            " at index: " << cb);

                case 0xE0:
                    // 3-octet format:  1110 xxxx, 10xx xxxx, 10xx xxxx
                    // ensure there is at least one more byte
                    ++cb;
                    if (cb + 1 >= c)
                        {
                        // Octet indicates more bytes in character,
                        // but cb will go past the end of the string
                        COH_THROW_STREAM (IllegalArgumentException,
                                "encountered incomplete UTF-8 character 0x" <<
                                std::hex << uint32_t(ch) << std::dec <<
                                " at the end of the string");
                        }

                    // validate that the next byte is a continuation
                    if ( (a[cb] & 0xC0) != 0x80)
                        {
                        // Octet indicates not a continuation byte
                        COH_THROW_STREAM (IllegalArgumentException,
                                "encountered invalid UTF-8 character 0x" <<
                                std::hex << uint32_t(ch) << std::dec <<
                                " at index " << cb);
                        }
                    // fall through

                case 0xD0:
                case 0xC0:
                    // 2-octet format:  110x xxxx, 10xx xxxx
                    if (++cb >= c)
                        {
                        // Octet indicates more bytes in character,
                        // but cb will go past the end of the string
                        COH_THROW_STREAM (IllegalArgumentException,
                                "encountered incomplete UTF-8 character 0x" <<
                                std::hex << uint32_t(ch) << std::dec <<
                                " at the end of the string");
                        }

                    // validate that the next byte is a continuation
                    if ( (a[cb] & 0xC0) != 0x80)
                        {
                        // Octet indicates not a continuation byte,
                        COH_THROW_STREAM (IllegalArgumentException,
                                "encountered invalid UTF-8 character 0x" <<
                                std::hex << uint32_t(ch) << std::dec <<
                                " at index " << cb);
                        }
                    // fall through

                case 0x70:
                case 0x60:
                case 0x50:
                case 0x40:
                case 0x30:
                case 0x20:
                case 0x10:
                    // 1-octet format:  0xxx xxxx
                    ++cb;
                    break;
                    
                case 0x00:
                    // 1-octet format. Possibly NUL
                    if (0 == ch && String::npos == c)
                        {
                        // NUL terminated string
                        return cb;
                        }
                    fN = true;
                    ++cb;
                    break;

                default:
                    COH_THROW_STREAM (IllegalArgumentException,
                            "encountered non UTF-8 sequence 0x" << std::hex <<
                            uint32_t(ch) << std::dec << " at index: " << cb);
                }
            }

        return cb;
        }

    /**
    * Validate the UTF-16 (platform endian order) string.
    *
    * @param a    the array to scan
    * @param c    the number of elements in the array, or npos if the size
    *             is unknown but the array is NUL terminated
    * @param ccp  return the number of code points in the string
    * @param fN   return true if the string contains embedded NULs
    *
    * @return the number of octets required to store this string UTF-8 format
    *
    * @throws IllegalArgumentException if the string is not a valid UTF-8 BML string
    */
    template<class T> size32_t validateUTF16String(const T* a, size32_t c, size32_t& ccp, bool& fN)
        {
        if (NULL == a)
            {
            COH_THROW (NullPointerException::create());
            }

        size32_t cb = 0;
        ccp = 0;
        for (uint32_t ch; ccp < c; ++ccp)
            {
            ch = (uint32_t) a[ccp];

            if (0 == ch && String::npos == c)
                {
                // NUL terminated string
                break;
                }
            else if (ch < 0x80) // U+0000 .. U+007F
                {
                // BMP format
                fN |= (ch == 0);
                cb += 1; // 1 UTF-8 octet
                }
            else if (ch < 0x800) // U+0080 .. U+07FF
                {
                // BMP format
                cb += 2; // 2 UTF-8 octets
                }
            else if (ch < 0xD800) // U+0800 .. U+D7FF
                {
                // BMP format
                cb += 3; // 3 UTF-8 octets
                }
            else if (ch < 0xE000) // U+D800 .. U+DFFF
                {
                // ch is in the U+D800 .. U+DBFF range indicating it is the
                // high unit of a surrogate pair, i.e. non-BMP format
                COH_THROW_STREAM(IllegalArgumentException,
                        "encountered non-BMP Unicode character 0x" << std::hex
                        << uint32_t(ch) << std::dec << " at index: " << ccp);
                }
            else if (ch < 0x10000) // U+E000 .. U+FFFF
                {
                // BMP format
                cb += 3; // 3 UTF-8 octets again
                }
            else // U+10000 ...
                {
                // wchar apparently hold a UINT-32
                COH_THROW_STREAM(IllegalArgumentException,
                        "encountered non UTF-16 sequence 0x" << std::hex <<
                        uint32_t(ch) << std::dec << " at index: " << ccp);
                }
            }

        return cb;
        }

    /**
    * Validate the managed UTF-8 array.
    *
    * @param va   the array to scan
    * @param of   the offset to start validation
    * @param c    the number of elements after the offset to validate, or
    *             npos to validate all subsequent elements
    * @param ccp  return the number of code points in the string
    * @param fN   return true if the string contains embedded NULs
    *
    * @return the number of octets required to store this string UTF-8 format
    *
    * @throws IllegalArgumentException if the string is not a valid UTF-8 BML string
    */
    template<class T> size32_t validateManagedUTF8(T va, size32_t of,
            size32_t c, size32_t& ccp, bool& fN)
        {
        size32_t ce = va->length;
        if (of >= ce)
            {
            COH_THROW (IndexOutOfBoundsException::create());
            }

        if (String::npos == c)
            {
            c = ce - of;
            }
        else if (of + c > ce)
            {
            COH_THROW (IndexOutOfBoundsException::create());
            }
        return validateUTF8String(va->raw + of, c, ccp, fN);
        }

    /**
    * Validate the managed UTF-16 array.
    *
    * @param va   the array to scan
    * @param of   the offset to start validation
    * @param c    the number of elements after the offset to validate, or
    *             npos to validate all subsequent elements
    * @param ccp  return the number of code points in the string
    * @param fN   return true if the string contains embedded NULs
    *
    * @return the number of octets required to store this string UTF-8 format
    *
    * @throws IllegalArgumentException if the string is not a valid UTF-8 BML string
    */
    template<class T> size32_t validateManagedUTF16(T va, size32_t of, size32_t c, size32_t& ccp, bool& fN)
        {
        size32_t ce = va->length;
        if (of >= ce)
            {
            COH_THROW (IndexOutOfBoundsException::create());
            }

        if (String::npos == c)
            {
            c = ce - of;
            }
        else if (of + c > ce)
            {
            COH_THROW (IndexOutOfBoundsException::create());
            }
        return validateUTF16String(va->raw + of, c, ccp, fN);
        }

    /**
    * Convert the given array of type T, which each element in T contains a
    * UTF-16 character to a UTF-8 encoded sequence.
    *
    * @param aSrc    the array to convert
    * @param cSrc    the number of elements in the supplied array to convert
    *                or String::npos to convert until NUL
    * @param abDes  the UTF-8 array to output to, it must be big enough
    * @param fN   return true if the string contains embedded NULs
    *
    * @return the number of chars written into achDes
    */
    template<class T> size32_t UTF16toUTF8(T* aSrc, size32_t cSrc, octet_t* abDes)
        {
        size32_t iDes  = 0;
        for (size32_t iSrc = 0; iSrc < cSrc; ++iSrc)
            {
            uint32_t ch = (uint32_t) aSrc[iSrc];

            if (0 == ch && cSrc == String::npos)
                {
                break;
                }

            if (ch < 0x0080) // U+0000 .. U+007F
                {
                // 1-octet format:  0xxx xxxx
                abDes[iDes++] = (octet_t) ch;
                }
            else if (ch < 0x0800) // U+0080 .. U+07FF
                {
                // 2-octet format:  110x xxxx, 10xx xxxx
                abDes[iDes++] = (octet_t) (0xC0 | ((ch >> 6) & 0x1F));
                abDes[iDes++] = (octet_t) (0x80 | ((ch     ) & 0x3F));
                }
            else if (ch < 0xD800 ||                // U+0800 .. U+D7FF
                    (ch > 0xDFFF && ch < 0x10000)) // U+E000 .. U+FFFF
                {
                // 3-octet format:  1110 xxxx, 10xx xxxx, 10xx xxxx
                abDes[iDes++] = (octet_t) (0xE0 | ((ch >> 12) & 0x0F));
                abDes[iDes++] = (octet_t) (0x80 | ((ch >>  6) & 0x3F));
                abDes[iDes++] = (octet_t) (0x80 | ((ch      ) & 0x3F));
                }
            else
                {
                COH_THROW_STREAM (IllegalArgumentException,
                        "encountered non-BMP Unicode character 0x" << std::hex
                        << ch << std::dec << " at index: " << iSrc);
                }
            }

        // NUL terminate
        abDes[iDes++] = 0;

        return iDes;
        }

    /*
    * Compare two UTF8 encoded Strings represented by octet arrays.
    * If all characters are equal return 0. Otherwise return -1 if the first
    * non-equal char is less than the other else return 1.
    *
    * @param ab1  first utf8 octet array to compare
    * @param ab2  second utf8 octet array to compare
    * @param cch  the maxium number of characters to compare
    * @param fN   return true if the string contains embedded NULs
    *
    * @return  0 if the character arrays are equal otherwise -1 if less
    * or 1 if greater
    */
    int32_t strncmpUTF8(const octet_t* ab1, const octet_t* ab2, size32_t cch)
        {
        for (size32_t i = 0; i < cch; i++)
            {
            wchar16_t ch1 = _next(ab1);
            wchar16_t ch2 = _next(ab2);
            if (ch1 != ch2)
                {
                return ch1 < ch2 ? -1 : 1;
                }
             }
        return 0;
        }

    /**
     * Return the singleton String "NULL".
     *
     * @return String "NULL"
     */
    String::View getNullAsString()
        {
        static FinalView<String> vsSingleton(System::common(),
                String::create("NULL"));
        return vsSingleton;
        }
    COH_STATIC_INIT(getNullAsString());
    }

// ----- constants ----------------------------------------------------------

// NOTE: This is not an error, since statics are initialized to 0 (NULL), this
// occurs before the normal static ordering issues can start to occur. Since
// the desired value is also NULL, we are safe.
const char* const String::null_string = NULL;
COH_EXPORT_SPEC_MEMBER(const size32_t String::npos)


String::Handle String::create(const char* achSrc, size32_t cch)
    {
    bool     fN  = false;
    size32_t ccp = 0;
    size32_t cb  = validateUTF8String(achSrc, cch, ccp, fN) + 1;

    // see Array::create
    void*          pRawObj = Object::operator new(sizeof(String) + cb);
    octet_t*       abDes   = ((octet_t*) pRawObj) + sizeof(String);
    const octet_t* abSrc   = (const octet_t*) achSrc;
    std::copy(abSrc, abSrc + cb, abDes);

    String::Handle h = String::Handle(::new(pRawObj) String(ccp, cb, abDes, ccp == cb - 1 && !fN));

    return h;
    }

String::Handle String::create(const wchar_t* achSrc, size32_t cch)
    {
    bool     fN  = false;
    size32_t ccp = 0;
    size32_t cb  = validateUTF16String(achSrc, cch, ccp, fN) + 1;

    // see Array::create
    void*    pRawObj = Object::operator new(sizeof(String) + cb);
    octet_t* abDes   = ((octet_t*) pRawObj) + sizeof(String);

    // if this ensure fails then we may have corrupted memory, this ensure
    // would only fail if our algorithm (validateString) for computing the
    // UTF-8 array size is flawed.
    COH_ENSURE_EQUALITY(UTF16toUTF8(achSrc, ccp, abDes), cb);

    return String::Handle(::new(pRawObj) String(ccp, cb, abDes, ccp == cb - 1 && !fN));
    }

String::Handle String::create(Array<char>::View vachSrc, size32_t of, size32_t cch)
    {
    bool     fN  = false;
    size32_t ccp = 0;
    size32_t cb  = validateManagedUTF8(vachSrc, of, cch, ccp, fN) + 1;

    // see Array::create
    void*    pRawObj = Object::operator new(sizeof(String) + cb);
    octet_t* abDes   = ((octet_t*) pRawObj) + sizeof(String);

    const octet_t* abSrc = (const octet_t*) (const char*) vachSrc->raw + of;

    std::copy(abSrc, abSrc + cb - 1, abDes);

    abDes[cb - 1] = 0;

    return String::Handle(::new(pRawObj) String(ccp, cb, abDes, ccp == cb - 1 && !fN));
    }

String::Handle String::create(Array<wchar_t>::View vachSrc, size32_t of, size32_t cch)
    {
    bool     fN  = false;
    size32_t ccp = 0;
    size32_t cb  = validateManagedUTF16(vachSrc, of, cch, ccp, fN) + 1;

    // see Array::create
    void*    pRawObj = Object::operator new(sizeof(String) + cb);
    octet_t* abDes   = ((octet_t*) pRawObj) + sizeof(String);

    // if this ensure fails then we may have corrupted memory, this ensure
    // would only fail if our algorithm (validateString) for computing the
    // UTF-8 array size is flawed.
    COH_ENSURE_EQUALITY(UTF16toUTF8(vachSrc->raw + of, ccp, abDes), cb);

    return String::Handle(::new(pRawObj) String(ccp, cb, abDes, ccp == cb - 1 && !fN));
    }

String::Handle String::create(Array<octet_t>::View vabSrc, size32_t of, size32_t cba)
    {
    bool     fN  = false;
    size32_t ccp = 0;
    size32_t cb  = validateManagedUTF8(vabSrc, of, cba, ccp, fN) + 1;

    // see Array::create
    void*          pRawObj = Object::operator new(sizeof(String) + cb);
    octet_t*       abDes   = ((octet_t*) pRawObj) + sizeof(String);
    const octet_t* abSrc   = vabSrc->raw + of;

    std::copy(abSrc, abSrc + cb - 1, abDes);

    abDes[cb - 1] = 0;

    return String::Handle(::new(pRawObj) String(ccp, cb, abDes, ccp == cb - 1 && !fN));
    }

String::Handle String::create(Array<wchar16_t>::View vachSrc, size32_t of, size32_t cch)
    {
    bool     fN  = false;
    size32_t ccp = 0;
    size32_t cb  = validateManagedUTF16(vachSrc, of, cch, ccp, fN) + 1;

    // see Array::create
    void*    pRawObj  = Object::operator new(sizeof(String) + cb);
    octet_t* abDes = ((octet_t*) pRawObj) + sizeof(String);

    // if this ensure fails then we may have corrupted memory, this ensure
    // would only fail if our algorithm (validateString) for computing the
    // UTF-8 array size is flawed.
    COH_ENSURE_EQUALITY(UTF16toUTF8(vachSrc->raw + of, ccp, abDes), cb);

    return String::Handle(::new(pRawObj) String(ccp, cb, abDes, ccp == cb - 1 && !fN));
    }

String::Handle String::create(const String& that)
    {
    return (String::Handle) new String(that);
    }


// ----- constructors -------------------------------------------------------

String::String(size32_t ccp, size32_t cb, octet_t* ab, bool fCString)
    : super(cb, ab), m_ccp(ccp), m_fCString(fCString)
    {
    }

String::String(const String& that)
    : super((const ProtectedOctetArray*) (const String*) &that, 0, npos),
      m_ccp(that.m_ccp), m_fCString(that.m_fCString)
    {
    }


// ----- String interface ---------------------------------------------------

bool String::isASCII() const
    {
    return m_ccp == super::length - 1;
    }

bool String::isCString() const
    {
    return m_fCString;
    }

size32_t String::length() const
    {
    return m_ccp;
    }

bool String::isEmpty() const
    {
    return m_ccp == 0;
    }

const char* String::getCString() const
    {
    return (const char*) (const octet_t*) (raw);
    }

bool String::equals(const char* ach, size32_t cch) const
    {
    if (cch == npos)
        {
        // we can't use strcmp/strncmp as we support embedded NULs
        // strings are allowed to have embedded NULs so we scan based on length
        // scan for equality until we hit an inequality or first NUL
        const char* achAbegin = getCString();
        const char* achA      = achAbegin;
        for (; *ach; ++achA, ++ach)
            {
            if (*achA != *ach)
                {
                return false;
                }
            }

        // we've reached the end of at least one string, the only way the strings
        // are equal is if both are at their end.
        return 0 == *ach && achAbegin + super::length - 1 == achA;
        }
    else
        {
        // length is known, perform fast check
        size32_t cb = super::length - 1;
        return cb == cch && memcmp(getCString(), ach, cb) == 0;
        }
    }

bool String::equals(const wchar_t* ach, size32_t cch) const
    {
    if (sizeof(wchar_t) < sizeof(wchar16_t))
        {
        COH_THROW (UnsupportedOperationException::create());
        }

    if (cch == npos)
        {
        // we can't use strcmp/strncmp as we support embedded NULs
        // strings are allowed to have embedded NULs so we scan based on length
        const octet_t* abAbegin = raw;
        const octet_t* abA      = abAbegin;

        // scan for equality until we hit an inequality or first NUL
        while (*ach)
            {
            if (_next(abA) != *(ach++))
                {
                return false;
                }
            }

        // we've reached the end of at least one string, the only way the strings
        // are equal is if both are at their end.
        return 0 == *ach && abAbegin + super::length - 1 == abA;
        }
    else
        {
        // length is known
        if (cch != length())
            {
            return false;
            }

        const octet_t* iterThis = raw;

        for (size32_t i = 0; i < cch; ++i)
            {
            if (_next(iterThis) != ach[i])
                {
                return false;
                }
            }

        return true;
        }
    }

size32_t String::indexOf(String::View vsSearch, size32_t iBegin) const
    {
    size32_t       cchSource = length();
    size32_t       cchSearch = vsSearch->length();
    size32_t       cbSearch  = vsSearch->super::length - 1;
    const octet_t* abSource  = raw;
    const octet_t* abSearch  = vsSearch->raw;
    bool           fASCII    = isASCII();

    if (iBegin + cchSearch > cchSource)
        {
        return String::npos;
        }

    _skip(abSource, iBegin, fASCII);
    for (size32_t ich = iBegin, iEnd = cchSource - cchSearch; ich <= iEnd;
            ++ich, _skip(abSource, 1, fASCII))
        {
        if (memcmp(abSource, abSearch, cbSearch) == 0)
            {
            return ich;
            }
        }

    return String::npos;
    }

size32_t String::indexOf(wchar16_t chSearch, size32_t iBegin) const
    {
    size32_t       cchSource = length();
    const octet_t* abSource  = raw;

    if (iBegin >= cchSource)
        {
        return String::npos;
        }

    _skip(abSource, iBegin, isASCII());
    for (size32_t ich = iBegin; ich < cchSource; ++ich)
        {
        if (_next(abSource) == chSearch)
            {
            return ich;
            }
        }

    return String::npos;
    }

size32_t String::lastIndexOf(String::View vsSearch, size32_t iBegin) const
    {
    size32_t       cchSource = length();
    size32_t       cchSearch = vsSearch->length();
    size32_t       cbSearch  = vsSearch->super::length - 1;
    const octet_t* abEnd     = raw + super::length - 1;
    const octet_t* abSearch  = vsSearch->raw;
    bool           fASCII    = isASCII();

    if (cchSearch > cchSource)
        {
        return String::npos;
        }
    else if (iBegin == String::npos || iBegin + cchSearch > cchSource)
        {
        iBegin = cchSource - cchSearch;
        }

    _back(abEnd, cchSource - iBegin, fASCII);
    while (true)
        {
        if (memcmp(abEnd, abSearch, cbSearch) == 0)
            {
            return iBegin;
            }
        else if (iBegin == 0)
            {
            return String::npos;
            }

        --iBegin;
        _back(abEnd, 1, fASCII);
        }
    }

size32_t String::lastIndexOf(wchar16_t chSearch, size32_t iBegin) const
    {
    size32_t       cchSource = length();
    const octet_t* abEnd     = raw + super::length - 1;

    if (iBegin >= cchSource)
        {
        iBegin = cchSource - 1;
        }

    iBegin++;
    _back(abEnd, cchSource - iBegin, isASCII());
    for (size32_t ich = iBegin; ich > 0 ; --ich)
        {
        if (_prev(abEnd) == chSearch)
            {
            return ich - 1;
            }
        }

    return String::npos;
    }

String::View String::substring(size32_t iBegin, size32_t iEnd) const
    {
    const octet_t* abSource  = raw;
    const octet_t* abStart   = raw;
    size32_t       cchSource = length();
    bool           fASCII    = isASCII();

    if (iEnd == String::npos)
        {
        iEnd = cchSource;
        }

    if (iEnd > cchSource || iBegin > iEnd)
        {
        COH_THROW_STREAM (IndexOutOfBoundsException,
                "Index out of bounds iBegin ="
                << iBegin << "iEnd =" << iEnd)
        }

    _skip(abStart, iBegin, fASCII);

    const octet_t* abEnd = abStart;
    _skip(abEnd, iEnd - iBegin, fASCII);

    return String::create(getOctets(),
            size32_t(abStart - abSource), size32_t(abEnd - abStart));
    }

bool String::startsWith(String::View vsSearch) const
    {
    size32_t cbSource  = super::length;
    size32_t cbSearch  = vsSearch->super::length;

    return cbSearch <= cbSource &&
        memcmp(raw, vsSearch->raw, cbSearch - 1) == 0;
    }

bool String::endsWith(String::View vsSearch) const
    {
    size32_t cbSource  = super::length;
    size32_t cbSearch  = vsSearch->super::length;

    return cbSearch <= cbSource &&
        memcmp(raw + (cbSource - cbSearch), vsSearch->raw, cbSearch - 1) == 0;
    }

bool String::regionMatches(size32_t ofSource, String::View vsOther,
        size32_t ofOther, size32_t cch) const
    {
    size32_t       cchSource = length();
    size32_t       cchOther  = vsOther->length();
    const octet_t* abSource  = raw;
    const octet_t* abOther   = vsOther->raw;

    if (cch == npos)
        {
        cch = cchOther - ofOther;
        }
    if ((ofSource > cchSource - cch) || (ofOther > cchOther - cch))
        {
        return false;
        }

    _skip(abSource, ofSource, isASCII());
    _skip(abOther, ofOther, vsOther->isASCII());
    return strncmpUTF8(abSource, abOther, cch) == 0;
    }

String::View String::trim() const
    {
    size32_t cbSource = super::length - 1;

    if (cbSource == 0)
        {
        return View(this);
        }

    const octet_t* abSource = raw;
    const octet_t* abStart  = abSource;
    const octet_t* abEnd    = abSource + cbSource;

    for (; abStart < abEnd   && (*abStart <= 0x20); ++abStart);
    for (; abEnd   > abStart && (*abEnd <= 0x20);   --abEnd);

    size32_t cbTrim = size32_t(abEnd - abStart) + 1;
    if (cbTrim == cbSource)
        {
        return View(this);
        }

    return String::create(getOctets(), size32_t(abStart-abSource), cbTrim);
    }

Array<octet_t>::View String::getOctets() const
    {
    return this;
    }

String::View String::valueOf(Object::View v)
    {
    return (v == NULL) ? getNullAsString() : v->toString();
    }

// ----- Comparable interface -----------------------------------------------

int32_t String::compareTo(Object::View v) const
    {
    if (this == v)
        {
        return 0;
        }

    String::View         vThat = cast<String::View>(v);
    const octet_t*       abA   = raw;
    const octet_t*       abB   = vThat->raw;
    size32_t             cbA   = super::length;
    size32_t             cbB   = vThat->super::length;
    const octet_t*       abeA  = abA + cbA;
    const octet_t*       abeB  = abB + cbB;

    while (abA < abeA && abB < abeB)
        {
        int32_t cDiff = int32_t(_next(abA)) - int32_t(_next(abB));
        if (cDiff)
            {
            return cDiff;
            }
        }

    // we've hit the end of at least one string
    return cbA < cbB ? -1 : int32_t(cbA - cbB);
    }


// ----- Object interface ---------------------------------------------------

bool String::equals(Object::View v) const
    {
    if (v == this)
        {
        return true;
        }

    String::View vThat = cast<String::View>(v, false);
    if (NULL == vThat)
        {
        // v is not a String, but it may be an Array<octet_t>, since we extend
        // from Array<octet_t>. Support the following odd case:
        return super::equals(cast<Array<octet_t>::View>(v, false));
        }

    // delegate to Array<octet_t>::equals
    return super::equals(vThat);
    }

size32_t String::hashCode() const
    {
    // Note: Array<>::hashCode is cached, so this isn't expensive here
    return super::hashCode();
    }

bool String::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> String::toString() const
    {
    return this;
    }

size64_t String::sizeOf(bool fDeep) const
    {
    return fDeep
            ? super::sizeOf(true)
            : sizeof(String) + (NULL == Array<octet_t>::m_ohDelegate
                    ? Array<octet_t>::length : 0);
    }

// ----- static helpers -----------------------------------------------------

wchar16_t String::next(const char*& ach)
    {
    const octet_t* ab = (const octet_t*) ach;
    wchar16_t      ch = _next(ab);
    ach = (const char*) ab;
    return ch;
    }

COH_CLOSE_NAMESPACE2


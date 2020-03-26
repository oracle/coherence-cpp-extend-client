/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Array.hpp"

#include "coherence/lang/Float32.hpp"
#include "coherence/lang/Float64.hpp"
#include "coherence/lang/IndexOutOfBoundsException.hpp"
#include "coherence/lang/MemberHolder.hpp"
#include "coherence/lang/NullPointerException.hpp"
#include "coherence/lang/OutOfMemoryError.hpp"
#include "coherence/lang/SynchronizedBlock.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"
#include "coherence/lang/UnsupportedOperationException.hpp"

#include "coherence/util/Set.hpp"

#include <algorithm>
#include <iostream>
#include <string.h>

using coherence::util::Set;

COH_OPEN_NAMESPACE2(coherence,lang)

// ----- non-member functions -----------------------------------------------

// defined in Object.cpp
extern Set::Handle coh_getThreadLocalSizeOfSet();

namespace
    {
    /**
    * Compare two Array elements for equality.
    *
    * @param e1  the first element to compare
    * @param e2  the second element to compare
    *
    * @return true if e1 and e2 are equal
    */
    template<class T> bool coh_element_equals(const T& e1, const T& e2)
        {
        return e1 == e2;
        }

    template<> bool coh_element_equals(const float32_t& e1, const float32_t& e2)
        {
        return Float32::compare(e1, e2) == 0;
        }

    template<> bool coh_element_equals(const float64_t& e1, const float64_t& e2)
        {
        return Float64::compare(e1, e2) == 0;
        }

    template<> bool coh_element_equals(const MemberHolder<Object>& e1,
            const MemberHolder<Object>& e2)
        {
        return Object::equals(e1, e2);
        }

    /**
    * Compare two Array regions for equality.
    *
    * @param aValue  the first Array to compare
    * @param aFrom   the starting index in the first Array
    * @param bValue  the second Array to compare
    * @param bFrom   the starting index in the second Array
    * @param c       the number of elements to compare
    *
    * @return true iff the regions are equal
    */
    template<class T> bool coh_array_equals(const T* aValue, size32_t aFrom,
            const T* bValue, size32_t bFrom, size32_t c)
        {
        for (size32_t i = aFrom, j = bFrom; i < aFrom + c; ++i, ++j)
            {
            if (!coh_element_equals(aValue[i], bValue[j]))
                {
                return false;
                }
            }
        return true;
        }

    template<> bool coh_array_equals(const octet_t* aValue, size32_t aFrom,
            const octet_t* bValue, size32_t bFrom, size32_t c)
        {
        return memcmp(&aValue[aFrom], &bValue[bFrom], c) == 0;
        }

    /**
    * Return the CRC table
    */
    size32_t* getCrcTable()
        {
        static size32_t anCrc[256];
        static bool     fInit = true;
        if (fInit)
            {
            fInit = false;
            for (size32_t i = 0; i < 256; ++i)
                {
                size32_t nCrc = i;
                for (size32_t n = 0; n < 8; ++n)
                    {
                    if ((nCrc & 1) == 1)
                        {
                        nCrc = (nCrc >> 1) ^ 0xEDB88320U;
                        }
                    else
                        {
                        nCrc >>= 1;
                        }
                    }
                anCrc[i] = nCrc;
                }
            }
        return anCrc;
        }
    COH_STATIC_INIT(getCrcTable());

    /**
    * Return the CRC for a given octet.
    */
    size32_t getCrc(octet_t b)
        {
        static const size32_t* anCrc = getCrcTable();
        return anCrc[b];
        }
    COH_STATIC_INIT(getCrc(0));
    }


// ----- factory methods ----------------------------------------------------

template<class T> typename Array<T>::Handle Array<T>::create(size32_t cValue)
    {
    // Perform a single allocation for the Array object and the raw array.
    // The intention is to reduce two dymamic allocations into one. This
    // significantly lowers the runtime cost for String and Binary allocations.
    // We use Object's new operator to gain padding safety checks, and
    // provides memory was zero'd out.

    // start by computing the actual number of bytes needed for the raw array
    // this may be bigger then cValue*sizeof(T), in case the compiler does
    // inter element padding
    T*     aT   = NULL;
    size_t cbaT = (char*) &aT[cValue] - (char*) &aT[0];

    // account for alignment of T. We know that if we started laying out the
    // raw array at the start of the malloc'd block we'd have to be safe, so
    // we compute the alignment pad based on this
    size_t cbObj   = sizeof(Array<T>);
    size_t cbT     = (char*) &aT[1] - (char*) &aT[0];
    size_t cbRem   = cbObj % cbT;
    size_t cbAlign = cbObj > cbT
                   ? cbRem == 0
                       ? 0 : cbT - cbRem
                   : cbT - cbObj;

    // now perform a single allocation
    void* pRawObj = Object::operator new(cbObj + cbAlign + cbaT);

    // compute starting point for raw array
    aT = (T*) ((char*) pRawObj + cbObj + cbAlign);

    // use placement new operator to construct an Array in-place
    return ::new(pRawObj) Array<T>(cValue, aT);
    }

// ----- constructors -------------------------------------------------------

template<> Array<MemberHolder<Object> >::Array(size32_t cValue, MemberHolder<Object>* aValue)
    : m_nHashCode(0), m_ohDelegate(Object::self()), length(cValue), raw(aValue)
    {
    for (size32_t i = 0, c = length; i < c; ++i)
        {
        // placement new create the MemberHolder<Object>
        (::new(&(aValue[i])) MemberHolder<Object>)->setGuardian(self());
        }
    }

template<class T> Array<T>::Array(size32_t cValue, T* aValue)
    : m_nHashCode(0), m_ohDelegate(Object::self()), length(cValue), raw(aValue)
    {
    }

template<class T> Array<T>::Array(typename Array::Holder oha, size32_t iFrom, size32_t iTo)
    : m_nHashCode(0), m_ohDelegate(Object::self(), oha),
      length((npos == iTo ? oha->length : iTo) - iFrom),
      raw(NULL == oha ? NULL : (T*) (const T*) oha->raw + iFrom)
    {
    if (iTo == npos)
        {
        iTo = oha->length;
        }
    else if (iTo > oha->length)
        {
        COH_THROW_STREAM (IndexOutOfBoundsException, "iTo out of bounds: "
                << iTo);
        }

    if (iFrom > iTo)
        {
        COH_THROW (IndexOutOfBoundsException::create("iFrom > iTo"));
        }
    }

template<> Array<MemberHolder<Object> >::~Array()
    {
    if (m_ohDelegate == NULL)
        {
        for (size32_t i = length; i != 0; --i)
            {
            raw[i - 1].~MemberHolder(); // explicit destruction
            }
        }
    }

template<class T> Array<T>::~Array()
    {
    // noop
    }

template<class T> Array<T>::Array(const Array<T>& /*that*/)
    : m_nHashCode(npos), m_ohDelegate(Object::self()), length(npos), raw(NULL)
    {
    COH_THROW (UnsupportedOperationException::create()); // see clone()
    }

// ----- Array interface ----------------------------------------------------

template<class T> typename Array<T>::Handle Array<T>::subArray(size32_t iFrom, size32_t iTo)
    {
    TypedHolder<Array<T> > ohDelegate = m_ohDelegate;
    if (iTo == npos)
        {
        iTo = length;
        }

    if (NULL == ohDelegate)
        {
        return Array<T>::this_spec::create((TypedHandle<Array<T> >) this,
                iFrom, iTo);
        }
    size32_t of = size32_t(raw - ohDelegate->raw);
    return Array<T>::this_spec::create(ohDelegate, of + iFrom, of + iTo);
    }

template<class T> typename Array<T>::View Array<T>::subArray(size32_t iFrom, size32_t iTo) const
    {
    TypedHolder<Array<T> > ohDelegate = m_ohDelegate;
    if (iTo == npos)
        {
        iTo = length;
        }

    if (NULL == ohDelegate)
        {
        return Array<T>::this_spec::create((TypedHandle<Array<T> const>) this,
                iFrom, iTo);
        }
    size32_t of = size32_t(raw - ohDelegate->raw);
    return Array<T>::this_spec::create((TypedHandle<Array<T> const>) ohDelegate,
            of + iFrom, of + iTo);
    }

template<class T> bool Array<T>::regionMatches(size32_t iSource,
        typename Array<T>::View vaOther, size32_t iOther,
        size32_t c) const
    {
    size32_t thatLength = vaOther->length;
    if (iOther >= thatLength)
        {
        COH_THROW (IndexOutOfBoundsException::create("iOther"));
        }

    if (c == npos)
        {
        c = thatLength - iOther;
        }

    size32_t thisTo     = iSource + c;
    size32_t thatTo     = iOther + c;
    size32_t thisLength = length;

    // delegate to equals() if full Arrays
    if (iSource == 0 && thisTo == thisLength && iOther == 0 && thatTo == thatLength)
        {
        return equals(vaOther);
        }

    if (thisTo > thisLength)
        {
        COH_THROW (IndexOutOfBoundsException::create("iSource + c"));
        }

    if (thatTo > thatLength)
        {
        COH_THROW (IndexOutOfBoundsException::create("iOther + c"));
        }

    if (this == vaOther && iSource == iOther)
        {
        return true;
        }

    return coh_array_equals((const T*) raw, iSource, (const T*) vaOther->raw, iOther, c);
    }


// ----- Object interface ---------------------------------------------------

template<class T> TypedHandle<const String> Array<T>::toString() const
    {
    const T*     aValue = raw;
    String::View vs     = COH_TO_STRING('[');
    for (size32_t i = 0, c = length; i < c; ++i)
        {
        if (i == 0)
            {
            vs = COH_TO_STRING(vs << aValue[i]);
            }
        else
            {
            vs = COH_TO_STRING(vs << ", " << aValue[i]);
            }
        }

    return COH_TO_STRING(vs << ']');
    }

template<> TypedHandle<const String> Array<octet_t>::toString() const
    {
    const octet_t* aValue = raw;
    String::View   vs     = COH_TO_STRING('[');
    for (size32_t i = 0, c = length; i < c; ++i)
        {
        // print octets as ints, if the user wants them in hex then
        if (i == 0)
            {
            vs = COH_TO_STRING(vs << ((unsigned int) aValue[i]));
            }
        else
            {
            vs = COH_TO_STRING(vs << ", " << ((unsigned int) aValue[i]));
            }
        }

    return COH_TO_STRING(vs << ']');
    }

template<> TypedHandle<const String> Array<MemberHolder<Object> >::toString() const
    {
    const MemberHolder<Object>* aValue = raw;
    String::View                vs     = COH_TO_STRING('[');
    for (size32_t i = 0, c = length; i < c; ++i)
        {
        if (i == 0)
            {
            vs = COH_TO_STRING(vs << aValue[i]);
            }
        else
            {
            vs = COH_TO_STRING(vs << ",\n" << aValue[i]);
            }
        }

    return COH_TO_STRING(vs << ']');
    }

template<class T> bool Array<T>::equals(Object::View v) const
    {
    View vThat = cast<View>(v, false);

    if (this == vThat)
        {
        return true;
        }

    if (NULL == vThat)
        {
        return false;
        }

    // lengths must be equal
    size32_t c = length;
    if (c != vThat->length)
        {
        return false;
        }

    // hashCodes if computed must be equal, use cached hashes as computing
    // would be to expensive
    size32_t nHashThis = m_nHashCode; // dirty read
    size32_t nHashThat = vThat->m_nHashCode; // dirty read
    if (nHashThis != 0 && nHashThat != 0 && nHashThis != nHashThat)
        {
        // compare using clean reads
        if (hashCode() != vThat->hashCode())
            {
            return false;
            }
        }

    return coh_array_equals((const T*) raw, 0, (const T*) vThat->raw, 0, c);
    }

template<class T> size32_t Array<T>::hashCode() const
    {
    size32_t nHash = m_nHashCode; // dirty read of cached hash
    if (nHash == 0)
        {
        const T*               aValue     = raw;
        TypedHolder<Array<T> > ohDelegate = m_ohDelegate;
        bool                   fImmutable = Object::isImmutable() &&
            (NULL == ohDelegate || ohDelegate->Object::isImmutable());

        nHash = hashCode(aValue, length);

        if (fImmutable)
            {
            // cache the hash
            COH_SYNCHRONIZED_MEMBER_WRITE
                {
                m_nHashCode = nHash;
                }
            }
        }
    else
        {
        // clean read of cached hash
        COH_SYNCHRONIZED_MEMBER_READ
            {
            nHash = m_nHashCode;
            }
        }
    return nHash;
    }

// COH-10734 specialized hashCode() references specialized isImmutable()
template<> bool Array<MemberHolder<Object> >::isImmutable() const;

template<> size32_t Array<MemberHolder<Object> >::hashCode() const
    {
    size32_t nHash = m_nHashCode; // dirty read of cached hash
    if (nHash == 0)
        {
        const MemberHolder<Object>*                aValue     = raw;
        TypedHolder<Array<MemberHolder<Object> > > ohDelegate = m_ohDelegate;
        bool                                       fImmutable =
            length == 0 || (Object::isImmutable() &&
            (NULL == ohDelegate || ohDelegate->Object::isImmutable()));

        for (size32_t i = 0, c = length; i < c; ++i)
            {
            size32_t nHashElm;
            Object::View v = aValue[i];
            if (NULL == v)
                {
                nHashElm = 0;
                }
            else
                {
                nHashElm    = v->hashCode();
                fImmutable &= v->isImmutable();
                }
            nHash = nHashElm ^ ((nHash << 4) | ((nHash >> 28) & 0xF));
            }

        if (nHash == 0)
            {
            // to allow for caching of the hashcode
            nHash = 17;
            }

        if (fImmutable)
            {
            // cache the hash
            COH_SYNCHRONIZED_MEMBER_WRITE
                {
                m_nHashCode = nHash;
                }
            }
        }
    else
        {
        // dirty read gave us non-zero value, though it may not be necessary
        // perform a clean read to ensure we didn't see a partial write
        COH_SYNCHRONIZED_MEMBER_READ
            {
            nHash = m_nHashCode;
            }
        }
    return nHash;
    }

template<class T> bool Array<T>::isImmutable() const
    {
    TypedHolder<Array<T> > ohDelegate = m_ohDelegate;
    return length == 0 ||
            (NULL == ohDelegate
                    ? m_nHashCode != 0 || Object::isImmutable()
                    : ohDelegate->Object::isImmutable());
    }

template<> bool Array<MemberHolder<Object> >::isImmutable() const
    {
    if (length != 0 && m_nHashCode == 0)
        {
        TypedHolder<Array<MemberHolder<Object> > > ohDelegate = m_ohDelegate;
        if (NULL != ohDelegate && !ohDelegate->Object::isImmutable())
            {
            return false;
            }

        bool                        fImmutable = Object::isImmutable();
        const MemberHolder<Object>* aValue     = raw;
        for (size32_t i = 0, c = length; fImmutable && i < c; ++i)
            {
            Object::View v = aValue[i];
            fImmutable = fImmutable && (v == NULL || v->isImmutable());
            }

        if (fImmutable)
            {
            // we can cache this by making use of the hashCode caching
            hashCode(); // since we are immutable this will get cached
            return true;
            }

        return false;
        }
    return true;
    }

template<class T> size64_t Array<T>::sizeOf(bool fDeep) const
    {
    // manually include the size of the actual array
    return fDeep
           ? Object::sizeOf(true)
           : sizeof(*this) + (NULL == m_ohDelegate
                ? (sizeof(T) * length) : 0);
    }

template<> size64_t Array<MemberHolder<Object> >::sizeOf(bool fDeep) const
    {
    // manually include the size of the actual array, as well as the size
    // of what its element reference
    if (fDeep)
        {
        size64_t    cb   = 0;
        Set::Handle hSet = coh_getThreadLocalSizeOfSet();
        if (hSet->add(this))
            {
            bool fRoot = hSet->size() == 1;
            try
                {
                cb = sizeOf(false);
                if (NULL == m_ohDelegate)
                    {
                    for (size32_t i = 0, c = length; i < c; ++i)
                        {
                        Object::View v = raw[i];
                        if (v != NULL)
                            {
                            cb += v->sizeOf(true);
                            }
                        }
                    }
                else
                    {
                    cb += m_ohDelegate->sizeOf(true);
                    }
                }
            catch (...)
                {
                if (fRoot)
                    {
                    hSet->clear();
                    }
                throw;
                }
            if (fRoot)
                {
                hSet->clear();
                }
            }

        return cb;
        }
    else
        {
        return sizeof(*this) + (NULL == m_ohDelegate
                ? (sizeof(MemberHolder<Object>) * length) : 0);
        }
    }

template<> Object::Handle Array<MemberHolder<Object> >::clone() const
    {
    Array<MemberHolder<Object> >::Handle hClone = Array<MemberHolder<Object> >
        ::create(length);
    const MemberHolder<Object>*          aSrc   = raw;
    MemberHolder<Object>*                aDes   = hClone->raw;

    for (size32_t i = 0, c = length; i < c; ++i)
        {
        Object::Holder oh = aSrc[i];
        if (oh == NULL)
            {
            aDes[i] = oh;
            }
        else
            {
            if (instanceof<Object::Handle>(oh))
                {
                // store as handle
                aDes[i] = oh->clone();
                }
            else
                {
                // for storing the clone as a view
                aDes[i] = (Object::View) oh->clone();
                }
            }
        }

    return hClone;
    }

template<class T> Object::Handle Array<T>::clone() const
    {
    typename Array<T>::Handle hClone = Array<T>::create(length);
    const T*                  aSrc   = raw;
    T*                        aDes   = hClone->raw;

    std::copy(aSrc, aSrc + length, aDes);

    return hClone;
    }

template<class T> void Array<T>::onConst()
    {
    m_ohDelegate = (TypedHandle<Array<T> const>) m_ohDelegate; // manual flip
    Array<T>::super::onConst();
    }

template<> void Array<MemberHolder<Object> >::onConst()
    {
    m_ohDelegate = (TypedHandle<Array<MemberHolder<Object> > const>) m_ohDelegate; // manual flip
    Array<MemberHolder<Object> >::super::onConst();

    if (_isEscaped(/*fAttemptCapture*/ false))
        {
        // elements are not part of the smart member stack of an escaped array
        // manually call their onConst methods; note this is a call to the
        // SmartMembers onConst, not the referenced Object's onConst
        MemberHolder<Object>* amoh = raw;
        for (size32_t i = 0, c = length; i < c; ++i)
            {
            if (amoh[i].m_nMutability == SmartMember::inherited)
                {
                amoh[i].onConst();
                }
            }
        }
    }

template<class T> void Array<T>::onEscape(bool fEscaped) const
    {
    Array<T>::super::onEscape(fEscaped);
    }

template<> void Array<MemberHolder<Object> >::onEscape(bool fEscaped) const
    {
    MemberHolder<Object>* amoh = const_cast<MemberHolder<Object>*>(raw.m_aValue);

    if (fEscaped)
        {
        // escaping: switch element guardians to common monitors to minimize
        // inter-element contention

        // run super::onEscape as a pre step, allowing chained calls to
        // element's onEscaped(true); escaping the elements
        Array<MemberHolder<Object> >::super::onEscape(fEscaped);

        size32_t iBase = System::identityHashCode(this);
        for (size32_t i = length; i != 0; )
            {
            --i;
            amoh[i].setGuardian(*System::getCommonMonitor(iBase + i));
            }
        }
    else
        {
        // capturing: switch element guardians back to the Array to avoid
        // memory barrier costs
        for (size32_t i = 0, c = length; i < c; ++i)
            {
            amoh[i].setGuardian(*this);
            }

        // run super::onEscape as a post step, allowing chained calls to
        // element's onEscaped(false)
        Array<MemberHolder<Object> >::super::onEscape(fEscaped);
        }
    }

// ----- static methods -----------------------------------------------------

template<class T> typename Array<T>::Handle Array<T>::copy(
            typename Array<T>::View vaSrc, size32_t iSrc,
            typename Array<T>::Handle haDes, size32_t iDes, size32_t ce)
    {
    size32_t ceSrc = vaSrc->length;
    size32_t ceDes = haDes->length;
    if (npos == ce)
        {
        ce = ceDes - iDes;
        if (iSrc + ce > ceSrc)
            {
            ce = ceSrc - iSrc;
            }
        }

    if (iSrc + ce > ceSrc)
        {
        COH_THROW_STREAM (IndexOutOfBoundsException,
                "iSrc + ce out of bounds: " << iSrc << " + " << ce << " > " <<
                ceSrc);
        }

    if (iDes + ce > ceDes)
        {
        COH_THROW_STREAM (IndexOutOfBoundsException,
                "iDes + ce out of bounds: " << iDes << " + " << ce << " > " <<
                ceDes);
        }

    const T* rawSrc = vaSrc->raw + iSrc;
    T*       rawDes = haDes->raw + iDes;

    if (rawDes >= rawSrc && rawSrc + ce > rawDes)
        {
        const T* sp = rawSrc + ce;
        T*       dp = rawDes + ce;
        for (size_t c = 0; c < ce; ++c)
            {
            *(--dp) = *(--sp);
            }
        }
    else if (rawDes <  rawSrc && rawDes + ce > rawSrc)
        {
        for (size_t c = 0; c < ce; ++c)
            {
            *(rawDes++) = *(rawSrc++);
            }
        }
    else
        {
        // no overlap, perform direct copy
        std::copy(rawSrc, rawSrc + ce, rawDes);
        }

    return haDes;
    }

template<class T>
size32_t Array<T>::hashCode(const T* aValue, size32_t c)
    {
    size32_t nHash = 0xFFFFFFFF;
    // regardless of type perform a binary hash
    const octet_t* ab = (const octet_t*) aValue;
    const size_t   cb = &(aValue[c]) - &(aValue[0]);
    for (size_t i = 0; i < cb; ++i)
        {
        nHash = (nHash >> 8) ^ getCrc((nHash ^ ab[i]) & 0xFF);
        }

    // avoid 0 hash as it would not be cached
    return nHash == 0 ? 17 : nHash;
    }

template<> size32_t Array<MemberHolder<Object> >::hashCode(const MemberHolder<Object>*, size32_t)
    {
    COH_THROW (UnsupportedOperationException::create());
    }


// ----- Array template instantiations --------------------------------------

#define COH_DEFINE_ARRAY(type) template class COH_EXPORT Array<type >

#ifndef COH_SKIP_ARRAY_INSTANTIATION
COH_DEFINE_ARRAY(MemberHolder<Object>);
COH_DEFINE_ARRAY(bool);
COH_DEFINE_ARRAY(octet_t);
COH_DEFINE_ARRAY(char);
COH_DEFINE_ARRAY(wchar_t);
//COH_DEFINE_ARRAY(wchar16_t); // avoid duplicate instantiation
COH_DEFINE_ARRAY(int8_t);
//COH_DEFINE_ARRAY(uint8_t); // avoid duplicate
COH_DEFINE_ARRAY(int16_t);
COH_DEFINE_ARRAY(uint16_t);
COH_DEFINE_ARRAY(int32_t);
COH_DEFINE_ARRAY(uint32_t);
COH_DEFINE_ARRAY(int64_t);
COH_DEFINE_ARRAY(uint64_t);
COH_DEFINE_ARRAY(float32_t);
COH_DEFINE_ARRAY(float64_t);
//COH_DEFINE_ARRAY(size32_t); // avoid duplicate instantiation
//COH_DEFINE_ARRAY(size64_t); // avoid duplicate instantiation

COH_REGISTER_CLASS(TypedBarrenClass<Array<bool> >     ::create());
COH_REGISTER_CLASS(TypedBarrenClass<Array<octet_t> >  ::create());
COH_REGISTER_CLASS(TypedBarrenClass<Array<wchar16_t> >::create());
COH_REGISTER_CLASS(TypedBarrenClass<Array<int16_t> >  ::create());
COH_REGISTER_CLASS(TypedBarrenClass<Array<int32_t> >  ::create());
COH_REGISTER_CLASS(TypedBarrenClass<Array<int64_t> >  ::create());
COH_REGISTER_CLASS(TypedBarrenClass<Array<float32_t> >::create());
COH_REGISTER_CLASS(TypedBarrenClass<Array<float64_t> >::create());
#endif //COH_SKIP_ARRAY_INSTANTIATION

COH_CLOSE_NAMESPACE2

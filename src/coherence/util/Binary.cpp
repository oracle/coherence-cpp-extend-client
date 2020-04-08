/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Binary.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::lang::TypedBarrenClass;

COH_REGISTER_CLASS(TypedBarrenClass<Binary>::create());


// ----- constructors -------------------------------------------------------

Binary::Binary()
    : super(getEmptyOctetArray(), 0, 0)
    {
    }

Binary::Binary(Array<octet_t>::View vab, size32_t of, size32_t cb)
    : super()
    {
    size32_t vabLength = vab->length;
    if (vab->isImmutable())
        {
        if (0 == of && (cb == vabLength || cb == npos))
            {
            // safe and cheap to reference the original array
            initialize(f_vab, vab);
            cb = vabLength;
            }
        else
            {
            // Create a sub array. All Binary objects should have m_of = 0
            // and m_cb = vab->length
            vab = vab->subArray(of, cb == npos ? cb: of + cb);
            initialize(f_vab, vab);
            cb = vab->length; // set to sub array length in case npos was passed in
            }
        }
    else
        {
        // not immutable, so clone
        if (cb == npos ? of > vabLength : of + cb > vabLength)
            {
            COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of << ", cb="
                    << cb << ", vab->length=" << vabLength);
            }
        if (cb == npos)
            {
            cb = vabLength - of;
            }
        initialize(f_vab, cast<Array<octet_t>::View>(Array<octet_t>::copy(vab, of,
                Array<octet_t>::create(cb))));
        }
    m_cb = cb;
    }

Binary::Binary(BinaryWriteBuffer::View vBuf)
    : super()
    {
    Array<octet_t>::View vab     = vBuf->getInternalOctetArray();
    size32_t             cbData  = vBuf->length();
    size32_t             cbTotal = vab->length;
    size32_t             cbWaste = cbTotal - cbData;

    if (cbWaste == 0)
        {
        initialize(f_vab, vab);
        }
    // tolerate up to 12.5% waste
    else if (cbWaste <= std::max((size32_t) 16, (cbTotal >> 3)))
        {
        // Create a sub array.  All Binary objects should have m_of = 0
        // and m_cb = vab->length
        initialize(f_vab, vab->subArray(0, cbData));
        }
    else
        {
        initialize(f_vab, cast<Array<octet_t>::View>(Array<octet_t>::copy(vab, 0,
                Array<octet_t>::create(cbData), 0, cbData)));
        }
    m_cb = cbData;
    }

Binary::Binary(const Binary& that)
    : super(that.f_vab, that.m_of, that.m_cb)
    {
    }


// ----- Binary interface -----------------------------------------------

int32_t Binary::calculateNaturalPartition(int32_t cPartitions) const
    {
    int64_t lHash = ((int64_t) hashCode()) & 0xFFFFFFFFL;
    return cPartitions == 0 ? (int32_t) lHash : (int32_t) (lHash % (int64_t) cPartitions);
    }


// ----- Object interface ---------------------------------------------------

size32_t Binary::hashCode() const
    {
    return f_vab->hashCode();
    }

bool Binary::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    Binary::View vThat = cast<Binary::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return f_vab->equals(vThat->f_vab);
    }

bool Binary::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> Binary::toString() const
    {
    return COH_TO_STRING("Binary(length=" << length() << ')');
    }


// ----- internal methods ---------------------------------------------------

void Binary::updateLength(size32_t /*cb*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

ReadBuffer::View Binary::instantiateReadBuffer(size32_t of,
        size32_t cb) const
    {
    if (m_of == of && cb == m_cb)
        {
        return this;
        }

    return Binary::create(f_vab, of, cb);
    }

COH_CLOSE_NAMESPACE2

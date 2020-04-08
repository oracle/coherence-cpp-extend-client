/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/OctetArrayReadBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


// ----- constructors -------------------------------------------------------

OctetArrayReadBuffer::OctetArrayReadBuffer(Array<octet_t>::View vab, size32_t of,
        size32_t cb, bool fCopy)
    {
    size32_t vabLength = vab->length;
    if (cb == npos ? of > vabLength : of + cb > vabLength)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of << ", cb="
                << cb << ", vab->length=" << vabLength);
        }
    if (cb == npos)
        {
        cb = vabLength - of;
        }
    initialize(f_vab, fCopy ? cast<Array<octet_t>::View>(vab->clone()) : vab);
    m_of = of;
    m_cb = cb;
    }

OctetArrayReadBuffer::OctetArrayReadBuffer(const OctetArrayReadBuffer& that)
    {
    initialize(f_vab, that.f_vab->isImmutable()
        ? that.f_vab // no need to copy an immutable array
        : 0 == that.m_of && that.m_cb == that.f_vab->length
            ? cast<Array<octet_t>::View>(that.f_vab->clone()) // clone the entire array
              // clone just the target portion of the array
            : cast<Array<octet_t>::View>(Array<octet_t>::copy(that.f_vab,
                    that.m_of, Array<octet_t>::create(that.m_cb))));
    m_of = that.f_vab->isImmutable() ? that.m_of : 0;
    m_cb = that.m_cb;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> OctetArrayReadBuffer::toString() const
    {
    return COH_TO_STRING("OctetArrayReadBuffer(length=" << m_cb << ')');
    }


// ----- internal methods ---------------------------------------------------

ReadBuffer::View OctetArrayReadBuffer::instantiateReadBuffer(size32_t of, size32_t cb) const
    {
    return OctetArrayReadBuffer::create(f_vab, m_of + of, cb, false);
    }

void OctetArrayReadBuffer::updateLength(size32_t cb)
    {
    m_cb = cb;
    }

COH_CLOSE_NAMESPACE2

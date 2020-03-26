/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OCTET_ARRAY_READ_BUFFER_HPP
#define COH_OCTET_ARRAY_READ_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/AbstractOctetArrayReadBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

class OctetArrayWriteBuffer;


/**
* A ReadBuffer on top of an octet Array.
*
* @author jh  2008.01.03
*/
class COH_EXPORT OctetArrayReadBuffer
    : public cloneable_spec<OctetArrayReadBuffer,
        extends<AbstractOctetArrayReadBuffer> >
    {
    friend class factory<OctetArrayReadBuffer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an OctetArrayReadBuffer object from a portion of an octet
        * Array.
        *
        * @param vab    an Array of octets
        * @param of     the offset into the octet Array
        * @param cb     the number of octets to extract or npos for
        *               (vab->length - of)
        * @param fCopy  true to make a copy of the passed in Array
        *
        * @throws  IndexOutOfBoundsException if <code>of + cb</code> is
        *          larger than <code>vab->length</code>
        * @throws  NullPointerException if <code>vab</code> is
        *          <code>NULL</code>
        */
        OctetArrayReadBuffer(Array<octet_t>::View vab, size32_t of = 0,
                size32_t cb = npos, bool fCopy = false);

        /**
        * Copy constructor.
        */
        OctetArrayReadBuffer(const OctetArrayReadBuffer& that);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Allow the length to be modified. This method is provided for use by
        * OctetArrayWriteBuffer only, and only for read buffers that it owns.
        *
        * @param cb  the new length
        */
        virtual void updateLength(size32_t cb);

        /**
        * {@inheritDoc}
        */
        virtual ReadBuffer::View instantiateReadBuffer(size32_t of,
                size32_t cb) const;


    // ----- friends --------------------------------------------------------

    friend class OctetArrayWriteBuffer;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OCTET_ARRAY_READ_BUFFER_HPP

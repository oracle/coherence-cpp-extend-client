/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BINARY_HPP
#define COH_BINARY_HPP

#include "coherence/lang.ns"

#include "coherence/io/AbstractOctetArrayReadBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/util/BinaryWriteBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::AbstractOctetArrayReadBuffer;
using coherence::io::ReadBuffer;


/**
* A thread-safe immutable binary object.
*
* @author jh  2008.05.29
*/
class COH_EXPORT Binary
    : public cloneable_spec<Binary,
        extends<AbstractOctetArrayReadBuffer> >
    {
    friend class factory<Binary>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Default constructor for a Binary object. Supports deserialization.
        *
        * @since Coherence 3.7.1
        */
        Binary();

        /**
        * Construct a Binary object from a portion of an octet Array.
        *
        * @param vab  an Array of octets; must not be NULL
        * @param of   the offset into the octet Array
        * @param cb   the number of octets to extract or npos for
        *             (vab->length - of)
        *
        * @throws  IndexOutOfBoundsException if <code>of + cb</code> is
        *          larger than <code>vab->length</code>
        * @throws  NullPointerException if <code>vab</code> is
        *          <code>NULL</code>
        */
        Binary(Array<octet_t>::View vab, size32_t of = 0, size32_t cb = npos);

        /**
        * Construct a Binary object from a BinaryWriteBuffer.
        *
        * @param vBuf  a BinaryWriteBuffer to construct this Binary object from
        *
        * @since Coherence 3.7.1
        */
        Binary(BinaryWriteBuffer::View vBuf);

        /**
        * Copy constructor.
        */
        Binary(const Binary& that);


    // ----- Binary interface -----------------------------------------------

    public:
        /**
        * Calculate the partition ID to which the specified Binary should be
        * naturally assigned. This calculation should not be applied to
        * Binary objects decorated with artificially assigned partitions.
        *
        * The resulting partition ID will be in the range [0..cPartitions).
        *
        * @param cPartitions  the partition count
        *
        * @return the partition that the this Binary is naturally assigned to
        *
        * @since Coherence 3.7.1
        */
        virtual int32_t calculateNaturalPartition(int32_t cPartitions) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual bool isImmutable() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void updateLength(size32_t cb);

        /**
        * {@inheritDoc}
        */
        virtual ReadBuffer::View instantiateReadBuffer(size32_t of,
                size32_t cb) const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_BINARY_HPP

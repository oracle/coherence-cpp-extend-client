/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BINARY_WRITE_BUFFER_HPP
#define COH_BINARY_WRITE_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/OctetArrayWriteBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::OctetArrayWriteBuffer;

class Binary;


/**
* A WriteBuffer implementation whose primary purpose is to be used to create
* Binary objects.
*
* @author phf  2011.03.11
*
* @since Coherence 3.7.1
*/
class COH_EXPORT BinaryWriteBuffer
    : public class_spec<BinaryWriteBuffer,
        extends<OctetArrayWriteBuffer> >
    {
    friend class factory<BinaryWriteBuffer>;

    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Binary View definition.
        */
        typedef TypedHandle<const Binary> BinaryView;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a BinaryWriteBuffer with a certain initial capacity.
        *
        * @param cbCap  initial capacity
        */
        BinaryWriteBuffer(size32_t cbCap);

        /**
        * Construct a BinaryWriteBuffer with a certain initial capacity and a
        * certain maximum capacity.
        *
        * @param cbCap  initial capacity
        * @param cbMax  maximum capacity
        *
        * @throws IllegalArgumentException if <tt>cbCap</tt> is greater
        *         than <tt>cbMax</tt>
        */
        BinaryWriteBuffer(size32_t cbCap, size32_t cbMax);


    // ----- WriteBuffer interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual BinaryView toBinary() const;


    // ----- internal -------------------------------------------------------

    protected:
        /**
        * Obtain the internal octet array that this WriteBuffer uses.
        *
        * Intended for use only by Binary.
        *
        * @return the actual octet array that this WriteBuffer uses
        */
        virtual Array<octet_t>::View getInternalOctetArray() const;

        /**
        * Validate the ranges for the passed bounds and make sure that the
        * underlying array is big enough to handle them.
        *
        * Note: This method prevents all modifications from occurring once the
        * BinaryWriteBuffer has supplied its octet array to a Binary object.
        *
        * @param of  the offset that data is about to be written to
        * @param cb  the length of the data that is about to be written
        */
        virtual void checkBounds(size32_t of, size32_t cb);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Indicator that no more modifications are permitted.
        */
        bool mutable m_fReadOnly;


    // ----- friends --------------------------------------------------------

    friend class Binary;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_BINARY_WRITE_BUFFER_HPP

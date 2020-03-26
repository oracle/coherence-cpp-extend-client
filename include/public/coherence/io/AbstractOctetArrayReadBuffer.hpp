/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_OCTET_ARRAY_READ_BUFFER_HPP
#define COH_ABSTRACT_OCTET_ARRAY_READ_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/AbstractReadBuffer.hpp"

// ----- forward declarations -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,util)

class Binary;

COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,io)


/**
* The AbstractOctetArrayReadBuffer abstract class is intended to serve as
* a base class for the following:
* <ol>
* <li>the pre-existing Binary class</li>
* <li>a new byte[] based class that does not attempt to add the immutability
* aspect provided by the Binary class</li>
* </ol>
*
* This implementation is not intended to be thread safe.
*
* @author phf  2011.03.15
*
* @since Coherence 3.7.1
*/
class COH_EXPORT AbstractOctetArrayReadBuffer
    : public abstract_spec<AbstractOctetArrayReadBuffer,
        extends<AbstractReadBuffer> >
    {
    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);


    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

        /**
        * Binary View definition.
        */
        typedef TypedHandle<const coherence::util::Binary> BinaryView;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor; intended for deserialization use by
        * subclasses.
        *
        * Note that this default constructor leaves the buffer in an invalid
        * state.
        */
        AbstractOctetArrayReadBuffer();

        /**
        * Construct an AbstractOctetArrayReadBuffer on a portion of an octet
        * Array.
        *
        * @param vab  an Array of octets
        * @param of   an offset into the octet array
        * @param cb   the number of octets to utilize or npos for
        *             (vab->length - of)
        *
        * @throws  IndexOutOfBoundsException if <code>of + cb</code> is
        *          larger than <code>vab->length</code>
        * @throws  NullPointerException if <code>vab</code> is
        *          <code>NULL</code>
        */
        AbstractOctetArrayReadBuffer(Array<octet_t>::View vab, size32_t of = 0,
                size32_t cb = npos);


    // ----- ReadBuffer interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t length() const;

        /**
        * {@inheritDoc}
        */
        virtual octet_t read(size32_t of) const;

        /**
        * {@inheritDoc}
        */
        virtual void read(size32_t ofBegin, size32_t ofEnd,
                Array<octet_t>::Handle habDest, size32_t ofDest) const;

        /**
        * {@inheritDoc}
        */
        virtual BinaryView toBinary() const;

        /**
        * {@inheritDoc}
        */
        virtual BinaryView toBinary(size32_t of, size32_t cb) const;

        /**
        * {@inheritDoc}
        */
        virtual Array<octet_t>::View toOctetArray(size32_t of, size32_t cb) const;

        /**
        * {@inheritDoc}
        */
        using AbstractReadBuffer::toOctetArray;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;


    // ----- factory methods ------------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual BufferInput::Handle instantiateBufferInput() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Reset the portion of the octet array the ReadBuffer operates upon.
        *
        * @param of  an offset into the octet array
        * @param cb  the number of octets to utilize
        *
        * @throws  IndexOutOfBoundsException  if <code>of + cb</code> is
        *          larger than the buffer's length
        */
        virtual void resetRange(size32_t of, size32_t cb);


    // ----- OctetArrayBufferInput inner class ------------------------------

    public:
        /**
        * This is an implementation of the BufferInput interface on top of an
        * octet array.
        *
        * @author jh   2008.01.03
        * @author phf  2011.03.22
        */
        class COH_EXPORT OctetArrayBufferInput
            : public class_spec<OctetArrayBufferInput,
                extends<AbstractBufferInput> >
            {
            friend class factory<OctetArrayBufferInput>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new OctetArrayBufferInput instance that
                * delegates to the given AbstractOctetArrayReadBuffer.
                *
                * @param vBuf  the delegate ReadBuffer
                */
                OctetArrayBufferInput(AbstractOctetArrayReadBuffer::View vBuf);

            // ----- BufferInput interface ------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                using AbstractBufferInput::read;

                /**
                * {@inheritDoc}
                */
                virtual octet_t read();

                /**
                * {@inheritDoc}
                */
                virtual void read(Array<octet_t>::Handle hab, size32_t of,
                        size32_t cb);

                /**
                * {@inheritDoc}
                */
                virtual wchar16_t readChar16();

                /**
                * {@inheritDoc}
                */
                virtual int16_t readInt16();

                /**
                * {@inheritDoc}
                */
                virtual int32_t readInt32();

                /**
                * {@inheritDoc}
                */
                virtual int64_t readInt64();

                /**
                * {@inheritDoc}
                */
                virtual float32_t readFloat32();

                /**
                * {@inheritDoc}
                */
                virtual float64_t readFloat64();

            // ----- internal methods -----------------------------------

            protected:
                /**
                * {@inheritDoc}
                */
                virtual String::View convertUTF(size32_t of, size32_t cb) const;

            // ----- data members ---------------------------------------

            protected:
                /**
                 * The delegate read buffer.
                 */
                FinalView<AbstractOctetArrayReadBuffer> f_vBuf;

                /**
                * The delegate's octet array that holds the binary data.
                * This value should not be changed.
                */
                FinalView<Array<octet_t> > f_vab;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The octet array that holds the binary data.
        * This value should not be changed.
        */
        FinalView<Array<octet_t> > f_vab;

        /**
        * Offset into the octet array at which the binary data is located.
        * This value should not be changed.
        */
        size32_t m_of;

        /**
        * Number of octets in the octet array that belong to this ReadBuffer
        * object.
        * This value should not be changed.
        */
        size32_t m_cb;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_OCTET_ARRAY_READ_BUFFER_HPP

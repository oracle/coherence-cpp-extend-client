/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_BUFFER_READER_HPP
#define COH_POF_BUFFER_READER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDateTime.hpp"
#include "coherence/io/pof/RawDayTimeInterval.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/RawTimeInterval.hpp"
#include "coherence/io/pof/RawYearMonthInterval.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::ReadBuffer;
using coherence::util::Binary;
using coherence::util::Collection;
using coherence::util::LongArray;
using coherence::util::Map;


/**
* PofReader implementation that reads POF-encoded data from a
* ReadBuffer::BufferInput.
*
* @author jh  2008.04.09
*/
class COH_EXPORT PofBufferReader
    : public class_spec<PofBufferReader,
        extends<PofHelper>,
        implements<PofReader> >
    {
    friend class factory<PofBufferReader>;
    friend class IdentityHolder;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a new PofBufferReader that will read a POF stream from
        * the passed BufferInput object.
        *
        * @param hIn   a BufferInput object
        * @param vCtx  the PofContext
        *
        * @return the new PofBufferReader
        */
        PofBufferReader(ReadBuffer::BufferInput::Handle hIn,
                PofContext::View vCtx);


    // ----- PofReader interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool readBoolean(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual octet_t readOctet(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual wchar16_t readChar16(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual int16_t readInt16(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual int32_t readInt32(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual int64_t readInt64(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual float32_t readFloat32(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual float64_t readFloat64(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<bool>::Handle readBooleanArray(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<octet_t>::Handle readOctetArray(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<wchar16_t>::Handle readChar16Array(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<int16_t>::Handle readInt16Array(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<int32_t>::Handle readInt32Array(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<int64_t>::Handle readInt64Array(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<float32_t>::Handle readFloat32Array(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Array<float64_t>::Handle readFloat64Array(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Binary::View readBinary(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual String::View readString(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual RawDate::View readRawDate(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        using PofHelper::readRawDate;

        /**
        * {@inheritDoc}
        */
        virtual RawTime::View readRawTime(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        using PofHelper::readRawTime;

        /**
        * {@inheritDoc}
        */
        virtual RawDateTime::View readRawDateTime(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual RawYearMonthInterval::View readRawYearMonthInterval(
                int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual RawTimeInterval::View readRawTimeInterval(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual RawDayTimeInterval::View readRawDayTimeInterval(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder readObject(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle readObjectArray(int32_t iProp,
                ObjectArray::Handle ha = NULL);

        /**
        * {@inheritDoc}
        */
        virtual LongArray::View readLongArray(int32_t iProp,
                LongArray::Handle hla = NULL);

        /**
        * {@inheritDoc}
        */
        virtual Collection::View readCollection(int32_t iProp,
                Collection::Handle hCol = NULL);

        /**
        * {@inheritDoc}
        */
        virtual Map::View readMap(int32_t iProp, Map::Handle hMap = NULL);

        /**
        * {@inheritDoc}
        */
        virtual PofContext::View getPofContext() const;

        /**
        * {@inheritDoc}
        */
        virtual void setPofContext(PofContext::View vCtx);

        /**
        * {@inheritDoc}
        */
        virtual int32_t getUserTypeId() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getVersionId() const;

        /**
        * {@inheritDoc}
        */
        virtual void registerIdentity(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual PofReader::Handle createNestedPofReader(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Binary::View readRemainder();


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Advance through the POF stream until the specified property is
        * found. If the property is found, return true, otherwise return
        * false and advance to the first property that follows the specified
        * property.
        *
        * @param iProp  the index of the property to advance to
        *
        * @return true if the property is found
        *
        * @throws IllegalStateException if the POF stream has already
        *         advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual bool advanceTo(int32_t iProp);

        /**
        * Register the completion of the parsing of a value.
        *
        * @param iProp  the property index
        *
        * @throws IOException if an I/O error occurs
        */
        virtual void complete(int32_t iProp);

        /**
        * If this parser is contextually within a user type, obtain the
        * parser which created this parser in order to parse the user type.
        *
        * @return the parser for the context within which this parser is
        *         operating
        */
        virtual PofBufferReader::Handle getParentParser();

        /**
        * If this parser is contextually within a user type, obtain the
        * parser which created this parser in order to parse the user type.
        *
        * @return the parser for the context within which this parser is
        *         operating
        */
        virtual PofBufferReader::View getParentParser() const;

        /**
        * Obtain the registry for identity-reference pairs, creating it if
        * necessary.
        *
        * @return the identity-reference registry, never NULL
        */
        virtual LongArray::Handle ensureReferenceRegistry();

        /**
        * Register the passed value with the passed identity.
        *
        * @param nId      the identity
        * @param ohValue  the value to register
        *
        * @throws IOException if the specified identity is already
        *         registered with a different object
        */
        virtual void registerIdentity(int32_t nId, Object::Holder ohValue);

        /**
        * Look up the specified identity and return the object to which it
        * refers.
        *
        * @param nId  the identity
        *
        * @return the object registered under that identity
        *
        * @throws IOException if the requested identity is not registered
        */
        virtual Object::Holder lookupIdentity(int32_t nId);

        /**
        * Read a POF value as an Object.
        *
        * @param nType  the type identifier of the value
        *
        * @return an Object value
        *
        * @throws IOException if an I/O error occurs
        */
        virtual Object::Holder readAsObject(int32_t nType);

        /**
        * Read a POF value in a uniform array/map as an Object.
        *
        * @param nType  the type identifier of the value
        *
        * @return an Object value
        *
        * @throws IOException if an I/O error occurs
        *
        * @since Coherence 3.7.1
        */
        virtual Object::Holder readAsUniformObject(int32_t nType);

        /**
        * Read a POF value as an Object array.
        *
        * @param nType  the type identifier of the value
        * @param ha     the optional <tt>ObjectArray</tt> to use to store the values, or to use
        *               as a typed template for creating an array to store the
        *               values
        *
        * @return an Object array
        *
        * @throws IOException if an I/O error occurs
        */
        virtual ObjectArray::Handle readAsObjectArray(int32_t nType,
                ObjectArray::Handle ha = NULL);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The BufferInput containing the POF stream.
        */
        FinalHandle<ReadBuffer::BufferInput> f_hIn;

        /**
        * The PofContext to use to realize user data types as objects.
        */
        FinalView<PofContext> f_vCtx;

        /**
        * Lazily-constructed mapping of identities to references.
        */
        FinalHandle<LongArray> f_hlaRefs;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_BUFFER_READER_HPP

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_BUFFER_WRITER_HPP
#define COH_POF_BUFFER_WRITER_HPP

#include "coherence/lang.ns"

#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDateTime.hpp"
#include "coherence/io/pof/RawDayTimeInterval.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/RawTimeInterval.hpp"
#include "coherence/io/pof/RawYearMonthInterval.hpp"
#include "coherence/io/pof/ReferenceLibrary.hpp"
#include "coherence/io/pof/WritingPofHandler.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::WriteBuffer;
using coherence::util::Binary;
using coherence::util::Collection;
using coherence::util::LongArray;
using coherence::util::Map;


/**
* PofWriter implementation that writes POF-encoded data to a
* WriteBuffer::BufferOutput.
*
* @author jh  2008.04.09
*/
class COH_EXPORT PofBufferWriter
    : public class_spec<PofBufferWriter,
        extends<PofHelper>,
        implements<PofWriter> >
    {
    friend class factory<PofBufferWriter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a new PofBufferWriter that will write a POF stream to the
        * passed BufferOutput object.
        *
        * @param hOut  the BufferOutput object to write to; must not be NULL
        * @param vCtx  the PofContext used by the new PofBufferWriter to
        *              serialize user types; must not be NULL
        */
        PofBufferWriter(WriteBuffer::BufferOutput::Handle hOut,
                PofContext::View vCtx);

        /**
        * Construct a new PofBufferWriter that will write a POF stream using
        * the passed WritingPofHandler.
        *
        * @param hHandler  the WritingPofHandler used for writing; must not
        *                  be NULL
        * @param vCtx      the PofContext used by the new PofBufferWriter to
        *                  serialize user types; must not be NULL
        */
        PofBufferWriter(WritingPofHandler::Handle hHandler,
                PofContext::View vCtx);


    // ----- PofWriter interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void writeBoolean(int32_t iProp, bool f);

        /**
        * {@inheritDoc}
        */
        virtual void writeOctet(int32_t iProp, octet_t b);

        /**
        * {@inheritDoc}
        */
        virtual void writeChar16(int32_t iProp, wchar16_t ch);

        /**
        * {@inheritDoc}
        */
        virtual void writeInt16(int32_t iProp, int16_t n);

        /**
        * {@inheritDoc}
        */
        virtual void writeInt32(int32_t iProp, int32_t n);

        /**
        * {@inheritDoc}
        */
        virtual void writeInt64(int32_t iProp, int64_t l);

        /**
        * {@inheritDoc}
        */
        virtual void writeFloat32(int32_t iProp, float32_t fl);

        /**
        * {@inheritDoc}
        */
        virtual void writeFloat64(int32_t iProp, float64_t dfl);

        /**
        * {@inheritDoc}
        */
        virtual void writeBooleanArray(int32_t iProp, Array<bool>::View vaf);

        /**
        * {@inheritDoc}
        */
        virtual void writeOctetArray(int32_t iProp, Array<octet_t>::View vab);

        /**
        * {@inheritDoc}
        */
        virtual void writeChar16Array(int32_t iProp, Array<wchar16_t>::View vach);

        /**
        * {@inheritDoc}
        */
        virtual void writeInt16Array(int32_t iProp, Array<int16_t>::View van);

        /**
        * {@inheritDoc}
        */
        virtual void writeInt32Array(int32_t iProp, Array<int32_t>::View van);

        /**
        * {@inheritDoc}
        */
        virtual void writeInt64Array(int32_t iProp, Array<int64_t>::View val);

        /**
        * {@inheritDoc}
        */
        virtual void writeFloat32Array(int32_t iProp, Array<float32_t>::View vafl);

        /**
        * {@inheritDoc}
        */
        virtual void writeFloat64Array(int32_t iProp, Array<float64_t>::View vadfl);

        /**
        * {@inheritDoc}
        */
        virtual void writeBinary(int32_t iProp, Binary::View vBin);

        /**
        * {@inheritDoc}
        */
        virtual void writeString(int32_t iProp, String::View vs);

        /**
        * {@inheritDoc}
        */
        virtual void writeRawDate(int32_t iProp, RawDate::View vDate);

        /**
        * {@inheritDoc}
        */
        virtual void writeRawDateTime(int32_t iProp, RawDateTime::View vdt);

        /**
        * {@inheritDoc}
        */
        virtual void writeRawDayTimeInterval(int32_t iProp,
                RawDayTimeInterval::View vInterval);

        /**
        * {@inheritDoc}
        */
        virtual void writeRawTime(int32_t iProp, RawTime::View vTime);

        /**
        * {@inheritDoc}
        */
        virtual void writeRawTimeInterval(int32_t iProp,
                RawTimeInterval::View vInterval);

        /**
        * {@inheritDoc}
        */
        virtual void writeRawYearMonthInterval(int32_t iProp,
                RawYearMonthInterval::View vInterval);

        /**
        * {@inheritDoc}
        */
        virtual void writeObject(int32_t iProp, Object::View v);

        /**
        * {@inheritDoc}
        */
        virtual void writeObjectArray(int32_t iProp, ObjectArray::View va);

        /**
        * {@inheritDoc}
        */
        virtual void writeObjectArray(int32_t iProp, ObjectArray::View va,
                Class::View vClass);

        /**
        * {@inheritDoc}
        */
        virtual void writeLongArray(int32_t iProp, LongArray::View vla);

        /**
        * {@inheritDoc}
        */
        virtual void writeLongArray(int32_t iProp, LongArray::View vla,
                Class::View vClass);

        /**
        * {@inheritDoc}
        */
        virtual void writeCollection(int32_t iProp, Collection::View vCol);

        /**
        * {@inheritDoc}
        */
        virtual void writeCollection(int32_t iProp, Collection::View vCol,
                Class::View vClass);

        /**
        * {@inheritDoc}
        */
        virtual void writeMap(int32_t iProp, Map::View vMap);

        /**
        * {@inheritDoc}
        */
        virtual void writeMap(int32_t iProp, Map::View vMap,
                Class::View vClassKey);

        /**
        * {@inheritDoc}
        */
        virtual void writeMap(int32_t iProp, Map::View vMap,
                Class::View vClassKey, Class::View vClassValue);

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
        virtual void setVersionId(int32_t nVersionId);

        /**
        * {@inheritDoc}
        */
        virtual PofWriter::Handle createNestedPofWriter(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual PofWriter::Handle createNestedPofWriter(int32_t iProp, int32_t nTypeId);

        /**
        * {@inheritDoc}
        */
        virtual void writeRemainder(Binary::View vBinProps);


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Write a property of user defined type to the POF stream.
        *
        * @param iProp  the property index
        * @param v      the <tt>user type</tt> property value to write
        *
        * @since Coherence 3.7.1
        */
        virtual void writeUserType(int32_t iProp, Object::View v);

       /**
        * If this writer is contextually within a user type, obtain the writer
        * which created this writer in order to write the user type.
        *
        * @return the containing writer
        */
        virtual PofBufferWriter::Handle getParentWriter();

        /**
        * If this writer is contextually within a user type, obtain the writer
        * which created this writer in order to write the user type.
        *
        * @return the containing writer
        */
        virtual PofBufferWriter::View getParentWriter() const;

        /**
        * Report that a POF property is about to be written to the POF
        * stream.
        *
        * This method call will be followed by one or more separate calls to
        * a "write" method and the property extent will then be terminated by
        * a call to {@link #endProperty}.
        *
        * @param iProp  the index of the property being written
        *
        * @throws IllegalArgumentException  if the property index is invalid,
        *         or is less than or equal to the index of the previous
        *         property written to the POF stream
        * @throws IOException  if an I/O error occurs
        */
        virtual void beginProperty(int32_t iProp);

        /**
        * Signifies the termination of the current POF property.
        *
        * @param iProp  the index of the current property
        */
        virtual void endProperty(int32_t iProp);

        /**
        * Assert that a class is equal to another class.
        *
        * @param vClass      the expected class; must not be NULL
        * @param vClassTest  the class to test for equality; must not be NULL
        *
        * @throws IllegalStateException if the second class is not equal to
        *         the first
        */
        static void assertEqual(Class::View vClass, Class::View vClassTest);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Ensure that references (support for cyclic dependencies) are enabled.
        */
        virtual void enableReference();

        /**
        * Determine if reference support is enabled.
        *
        * @return true iff reference support is enabled
        *
        * @since Coherence 3.7.1
        */
        virtual bool isReferenceEnabled();

    protected:
        /**
        * Return the BufferOutput that this PofBufferWriter writes to.
        *
        * @return the BufferOutput
        */
        virtual WriteBuffer::BufferOutput::Handle getBufferOutput();

        /**
        * Return the BufferOutput that this PofBufferWriter writes to.
        *
        * @return the BufferOutput
        */
        virtual WriteBuffer::BufferOutput::View getBufferOutput() const;

        /**
        * Return the WritingPofHandler used internally by this
        * PofBufferWriter to write the POF stream.
        *
        * @return the PofHandler
        */
        virtual WritingPofHandler::Handle getPofHandler();

        /**
        * Return the WritingPofHandler used internally by this
        * PofBufferWriter to write the POF stream.
        *
        * @return the PofHandler
        */
        virtual WritingPofHandler::View getPofHandler() const;

        /**
        * Determine if the object to be written is either Evolvable or part of an
        * Evolvable object.
        *
        * @return true iff the object to be written is Evolvable
        *
        * @since Coherence 3.7.1
        */
        virtual bool isEvolvable();

        /**
        * Set the Evolvable flag to indicate if the object to be written is
        * Evolvable or part of an Evolvable object.
        *
        * @param fEvolvable  true iff the object to be written is Evolvable
        *
        * @since Coherence 3.7.1
        */
        virtual void setEvolvable(bool fEvolvable);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The BufferOutput object that the PofBufferWriter writes to.
        */
        FinalHandle<WriteBuffer::BufferOutput> f_hOut;

        /**
        * The PofContext used by this PofBufferWriter to serialize user types.
        */
        FinalView<PofContext> f_vCtx;

        /**
        * Indicate if the object to be written is either Evolvable or part of an
        * Evolvable object.
        */
        bool m_fEvolvable;

        /**
        * The WritingPofHandler used to write a POF stream.
        */
        FinalHandle<WritingPofHandler> f_hHandler;

        /**
        * If references are used, then this is the ReferenceLibrary.
        */
        FinalHandle<ReferenceLibrary> f_hRefs;


    // ----- friends --------------------------------------------------------

    friend class UserTypeWriter;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_BUFFER_WRITER_HPP

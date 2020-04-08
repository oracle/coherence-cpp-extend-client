/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_USER_TYPE_WRITER_HPP
#define COH_USER_TYPE_WRITER_HPP

#include "coherence/lang.ns"

#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/pof/PofBufferWriter.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/WritingPofHandler.hpp"
#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::WriteBuffer;
using coherence::util::Binary;


/**
* The UserTypeWriter implementation is a contextually-aware PofWriter whose
* purpose is to write the properties of a value of a specified user type. The
* "contextual awareness" refers to the fact that the UserTypeWriter maintains
* state about the type identifier, the PofWriter's property index position
* within the user type value, and a PofContext that may differ from the
* PofContext that provided the PofSerializer which is using this
* UserTypeWriter to serialize a user type.
*
* @author jh  2008.04.11
*/
class COH_EXPORT UserTypeWriter
        : public class_spec<UserTypeWriter,
            extends<PofBufferWriter> >
    {
    friend class factory<UserTypeWriter>;
    friend class PofBufferWriter;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a UserTypeWriter for writing the property values of a
        * user type.
        *
        * @param hOut     the BufferOutput object to write to; must not be
        *                 NULL
        * @param vCtx     the PofContext to use for writing the user type
        *                 property values within the user type that this
        *                 writer will be writing
        * @param nTypeId  the type identifier of the user type; must be
        *                 non-negative
        * @param iProp    the index of the user type being written
        */
        UserTypeWriter(WriteBuffer::BufferOutput::Handle hOut,
                PofContext::View vCtx, int32_t nTypeId, int32_t iProp);

        /**
        * Construct a UserTypeWriter for writing the property values of a
        * user type.
        *
        * @param hParent  the containing PofBufferWriter
        * @param hOut     the BufferOutput object to write to; must not be
        *                 null
        * @param vCtx     the PofContext to use for writing the user type
        *                 property values within the user type that this
        *                 writer will be writing
        * @param nTypeId  the type identifier of the user type; must be
        *                 non-negative
        * @param iProp    the index of the user type being written
        */
        UserTypeWriter(PofBufferWriter::Handle hParent,
                WriteBuffer::BufferOutput::Handle hOut,
                PofContext::View vCtx, int32_t nTypeId, int32_t iProp);

        /**
        * Construct a UserTypeWriter for writing the property values of a
        * user type.
        *
        * @param hHandler  the WritingPofHandler used to write user type data
        *                  (except for the user type id itself, which is
        *                  passed as a constructor argument)
        * @param vCtx      the PofContext to use for writing the user type
        *                  property values within the user type that this
        *                  writer will be writing
        * @param nTypeId   the type identifier of the user type; must be
        *                  non-negative
        * @param iProp     the index of the user type being written
        */
        UserTypeWriter(WritingPofHandler::Handle hHandler,
                PofContext::View vCtx, int32_t nTypeId, int32_t iProp);

        /**
        * Construct a UserTypeWriter for writing the property values of a
        * user type.
        *
        * @param hParent  the containing PofBufferWriter
        * @param hHandle  the WritingPofHandler used to write user type data
        *                 (except for the user type id itself, which is
        *                 passed as a constructor argument)
        * @param vCtx     the PofContext to use for writing the user type
        *                 property values within the user type that this
        *                 writer will be writing
        * @param nTypeId  the type identifier of the user type; must be
        *                 non-negative
        * @param iProp    the index of the user type being written
        * @param nId      the identity of the object to encode, or -1 if the
        *                 identity shouldn't be encoded in the POF stream
        */
        UserTypeWriter(PofBufferWriter::Handle hParent,
                WritingPofHandler::Handle hHandler,
                PofContext::View vCtx, int nTypeId, int iProp, int nId=-1);


    // ----- PofWriter interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void writeObject(int32_t iProp, Object::View v);

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
        * {@inheritDoc}
        */
        virtual PofBufferWriter::Handle getParentWriter();

        /**
        * {@inheritDoc}
        */
        virtual PofBufferWriter::View getParentWriter() const;

        /**
        * {@inheritDoc}
        */
        virtual void beginProperty(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual void endProperty(int32_t iProp);

        /**
        * Notify the UserTypeWriter that it is being "closed". This
        * notification allows the UserTypeWriter to write any remaining data
        * that it has pending to write.
        */
        virtual void closeNested();

        /**
        * Write out the type and version identifiers of the user type to the
        * POF stream, if they haven't already been written.
        *
        * @throws IOException on I/O error
        */
        virtual void writeUserTypeInfo();

        /**
        * {@inheritDoc}
        */
        virtual bool isEvolvable();


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void enableReference();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The parent (ie containing) PofBufferWriter.
        */
        FinalHandle<PofBufferWriter> f_hWriterParent;

        /**
        * The type identifier of the user type that is being written.
        */
        const int32_t m_nTypeId;

        /**
        * The version identifier of the user type that is being written.
        */
        int32_t m_nVersionId;

        /**
        * The index of the user type being written.
        */
        const int32_t m_iProp;

        /**
        * The identity of the object to encode, or -1 if the identity
        * shouldn't be encoded in the POF stream
        */
        int32_t m_nId;

        /**
        * The index of the last property written to the POF stream or -1 if
        * the first property has yet to be written.
        */
        int32_t m_iPrevProp;

        /**
        * True iff the type and version identifier of the user type was
        * written to the POF stream.
        */
        bool m_fUserTypeBegin;

        /**
        * True iff the user type was written to the POF stream.
        */
        bool m_fUserTypeEnd;

        /**
        * The Complex value that corresponds to the user type that is being
        * written.
        */
        MemberHandle<WritingPofHandler::Complex> m_hComplex;

        /**
         * The ID of the Complex value that corresponds to the user type that is being
         * written.
         */
        size64_t m_nComplexId;

        /**
        * The currently open nested writer, if any.
        */
        MemberHandle<UserTypeWriter> m_hWriterNested;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_USER_TYPE_WRITER_HPP

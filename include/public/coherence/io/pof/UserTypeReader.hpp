/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_USER_TYPE_READER_HPP
#define COH_USER_TYPE_READER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/pof/PofBufferReader.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::ReadBuffer;
using coherence::util::Binary;


/**
* The UserTypeReader implementation is a contextually-aware PofReader whose
* purpose is to advance through the properties of a value of a specified user
* type. The "contextual awareness" refers to the fact that the UserTypeReader
* maintains state about the type identifier and version of the user type, the
* parser's property index position within the user type value, and a
* PofContext that may differ from the PofContext that provided the
* PofSerializer which is using this UserTypeReader to parse a user type.
*
* @author jh  2008.04.11
*/
class COH_EXPORT UserTypeReader
    : public class_spec<UserTypeReader,
        extends<PofBufferReader> >
    {
    friend class factory<UserTypeReader>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a parser for parsing the property values of a user type.
        *
        * @param hIn         the BufferInput that contains the user type
        *                    data, except for the user type id itself (which
        *                    is passed passed as a constructor argument)
        * @param vCtx        the PofContext to use for parsing the user type
        *                    property values within the user type that this
        *                    parser will be parsing
        * @param nTypeId     the type id of the user type
        * @param nVersionId  the version id of the user type
        *
        * @throws IOException  if an I/O error occurs
        */
        UserTypeReader(ReadBuffer::BufferInput::Handle hIn,
                PofContext::View vCtx, int32_t nTypeId, int32_t nVersionId);

        /**
        * Construct a parser for parsing the property values of a user type.
        *
        * @param hParent     the parent (ie the containing) PofBufferReader
        * @param hIn         the BufferInput that contains the user type
        *                    data, except for the user type id itself (which
        *                    is passed passed as a constructor argument)
        * @param vCtx        the PofContext to use for parsing the user type
        *                    property values within the user type that this
        *                    parser will be parsing
        * @param nTypeId     the type id of the user type
        * @param nVersionId  the version id of the user type
        *
        * @throws IOException  if an I/O error occurs
        */
        UserTypeReader(PofBufferReader::Handle hParent,
                ReadBuffer::BufferInput::Handle hIn, PofContext::View vCtx,
                int32_t nTypeId, int32_t nVersionId);

    private:
        /**
        * Create a nested UserTypeReader, which will be initiated with the
        * information found in the nested buffer.
        *
        * @param hParent the parent (ie the containing) PofBufferReader
        * @param hIn     the BufferInput that contains the user type data
        * @param vCtx    the PofContext to use for parsing the user type property
        *                values within the user type that this parser will be
        *                parsing
        *
        * @throws IOException  if an I/O error occurs
        */
        /**
        * @internal
        */
        UserTypeReader(PofBufferReader::Handle hParent,
                ReadBuffer::BufferInput::Handle hIn, PofContext::View vCtx);

        /**
        * Construct a parser for parsing a nested property that does not exist.
        * In other words, this is a "no-op" user type reader.
        *
        * @param hParent     the parent (ie the containing) PofBufferReader
        * @param hIn         the BufferInput that contains the user type
        *                    data, except for the user type id itself (which
        *                    is passed passed as a constructor argument)
        * @param vCtx        the PofContext to use for parsing the user type
        *                    property values within the user type that this
        *                    parser will be parsing
        * @param nTypeId     the type id of the user type
        *
        * @throws IOException  if an I/O error occurs
        */
        /**
        * @internal
        */
        UserTypeReader(PofBufferReader::Handle hParent,
                ReadBuffer::BufferInput::Handle hIn,
                PofContext::View vCtx, int32_t nTypeId);


    // ----- PofReader interface --------------------------------------------

    public:
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
        using PofBufferReader::registerIdentity;

        /**
        * {@inheritDoc}
        */
        virtual PofReader::Handle createNestedPofReader(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual Binary::View readRemainder();


    // ----- internal methods -----------------------------------------------

    public:
        /**
        * Return the index of the next property in the POF stream.
        *
        * @return  the index of the next property in the POF stream, or -1 if
        *          there are no more properties
        *
        * @throws IOException  if an I/O error occurs
        *
        * @since Coherence 12.1.3
        */
        virtual int32_t getNextPropertyIndex();

        /**
        * Return the index of the most recent property read or (if it were
        * missing) requested.
        *
        * @return  the index of the most recent property read
        *
        * @since Coherence 12.1.3
        */
        virtual int32_t getPreviousPropertyIndex() const;

    protected:
        /**
        * {@inheritDoc}
        */
        virtual bool advanceTo(int32_t iProp);

        /**
        * {@inheritDoc}
        */
        virtual void complete(int32_t iProp);

        /**
        * Notify the UserTypeReader that it is being "closed".
        *
        * @throws IOException  if an I/O error occurs
        */
        virtual void closeNested();

        /**
        * {@inheritDoc}
        */
        virtual PofBufferReader::Handle getParentParser();

        /**
        * {@inheritDoc}
        */
        virtual PofBufferReader::View getParentParser() const;


        // ----- data members ---------------------------------------------------

    protected:
        /**
        * The parent (ie containing) PofBufferReader.
        */
        FinalHandle<PofBufferReader> f_hParent;

        /**
        * The type identifier of the user type that is being parsed.
        */
        int32_t m_nTypeId;

        /**
        * The version identifier of the user type that is being parsed.
        */
        int32_t m_nVersionId;

        /**
        * Most recent property read or (if it were missing) requested. This
        * is used to determine if the client is attempting to read properties
        * in the wrong order.
        */
        int32_t m_iPrevProp;

        /**
        * The index of the next property in the POF stream.
        */
        int32_t m_iNextProp;

        /**
        * The offset of the index of the next property to read.
        */
        size32_t m_ofNextProp;

        /**
        * The currently open nested reader, if any.
        */
        MemberHandle<UserTypeReader> m_hReaderNested;

        /**
        * The property index of the property from which the currently open
        * nested reader is reading from.
        */
        int32_t m_iNestedProp;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_USER_TYPE_READER_HPP

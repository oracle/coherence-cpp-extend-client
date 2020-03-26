/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/PofCodec.hpp"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/SimplePofContext.hpp"
#include "coherence/io/pof/UserTypeReader.hpp"
#include "coherence/io/pof/UserTypeWriter.hpp"
#include "coherence/util/Binary.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::io::Evolvable;
using coherence::io::Serializer;
using coherence::io::pof::PofContext;
using coherence::io::pof::PortableObject;
using coherence::io::pof::SimplePofContext;
using coherence::io::pof::UserTypeReader;
using coherence::io::pof::UserTypeWriter;
using coherence::util::Binary;


// ----- constructors -------------------------------------------------------

PofCodec::PofCodec()
    {
    }


// ----- Codec interface ----------------------------------------------------

void PofCodec::encode(Channel::View vChannel, Message::View vMessage,
        WriteBuffer::BufferOutput::Handle hOut)
    {
    PofContext::View vCtx = cast<PofContext::View>(vChannel, false);
    COH_ENSURE(vCtx != NULL);

    PortableObject::View vpoMessage = cast<PortableObject::View>(vMessage, false);
    COH_ENSURE(vpoMessage != NULL);

    UserTypeWriter::Handle hWriter = UserTypeWriter::create(hOut, vCtx,
            vMessage->getTypeId(), 0);

    Serializer::View vSerializer = vChannel->getSerializer();
    if (instanceof<SimplePofContext::View>(vSerializer))
        {
        if (cast<SimplePofContext::View>(vSerializer)->isReferenceEnabled())
            {
            hWriter->enableReference();
            }
        }

    // set the version identifier
    Evolvable::View vEvolvable = cast<Evolvable::View>(vMessage, false);
    if (NULL != vEvolvable)
        {
        hWriter->setVersionId(std::max(vEvolvable->getDataVersion(),
                vEvolvable->getImplVersion()));
        }

    // write the Message properties
    vpoMessage->writeExternal(hWriter);

    // write the future properties
    hWriter->writeRemainder((vEvolvable == NULL) ? NULL : vEvolvable->getFutureData());
    }

Message::Handle PofCodec::decode(Channel::View vChannel,
        ReadBuffer::BufferInput::Handle hIn)
    {
    PofContext::View vCtx = cast<PofContext::View>(vChannel, false);
    COH_ENSURE(vCtx != NULL);

    int32_t                nTypeId    = hIn->readInt32();
    int32_t                nVersionId = hIn->readInt32();
    UserTypeReader::Handle hReader = UserTypeReader::create(hIn, vCtx,
            nTypeId, nVersionId);

    Message::Handle hMessage = vChannel->getMessageFactory()->createMessage(nTypeId);

    PortableObject::Handle hpoMessage = cast<PortableObject::Handle>(hMessage,
            false);
    COH_ENSURE(NULL != hpoMessage);

    // set the version identifier
    Evolvable::Handle hEvolvable = cast<Evolvable::Handle>(hpoMessage, false);
    if (NULL != hEvolvable)
        {
        hEvolvable->setDataVersion(nVersionId);
        }

    // read the Message properties
    hpoMessage->readExternal(hReader);

    // read the future properties
    Binary::View vBinFuture = hReader->readRemainder();
    if (NULL != hEvolvable)
        {
        hEvolvable->setFutureData(vBinFuture);
        }

    return hMessage;
    }


// ----- Describable interface ----------------------------------------------

String::View PofCodec::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << "Format=POF");
    }

COH_CLOSE_NAMESPACE4

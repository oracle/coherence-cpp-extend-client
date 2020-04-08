/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PofAnnotationSerializer.hpp"

#include "coherence/io/Evolvable.hpp"

#include "coherence/io/pof/annotation/Portable.hpp"

#include "private/coherence/io/pof/reflect/internal/AnnotationVisitor.hpp"
#include "private/coherence/io/pof/reflect/internal/ClassMetadataBuilder.hpp"
#include "private/coherence/io/pof/reflect/internal/TypeMetadata.hpp"
#include "private/coherence/io/pof/reflect/internal/Visitor.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;

using coherence::io::pof::annotation::Portable;

using coherence::io::pof::reflect::internal::AnnotationVisitor;
using coherence::io::pof::reflect::internal::ClassMetadataBuilder;
using coherence::io::pof::reflect::internal::TypeMetadata;

using coherence::util::Iterator;
using coherence::util::logging::Logger;


// ----- constructors -------------------------------------------------------

PofAnnotationSerializer::PofAnnotationSerializer(int32_t nTypeId, Class::View vClz, bool fAutoIndex)
    : f_ohTypeMetadata(self())
    {
    initialize(nTypeId, vClz, fAutoIndex);
    }

// ----- PofSerializer interface --------------------------------------------

void PofAnnotationSerializer::serialize(PofWriter::Handle hOut, Object::View v) const
    {
    // set the version identifier
    Evolvable::View vEvolvable = cast<Evolvable::View>(v, false);
    bool            fEvolvable = NULL != vEvolvable;
    if (fEvolvable)
        {
        hOut->setVersionId(std::max(vEvolvable->getDataVersion(),
                vEvolvable->getImplVersion()));
        }

    for (Iterator::Handle hIter = (cast<TypeMetadata::View>(f_ohTypeMetadata))->getAttributes(); hIter->hasNext(); )
        {
        TypeMetadata::AttributeMetadata::View vAttribute = cast<TypeMetadata::AttributeMetadata::View>(hIter->next());
        vAttribute->getCodec()->encode(hOut, vAttribute->getIndex(), vAttribute->get(v));
        }

    // write out any future properties
    Binary::View vBin;
    if (fEvolvable)
        {
        vBin = vEvolvable->getFutureData();
        }
    hOut->writeRemainder(vBin);
    }

Object::Holder PofAnnotationSerializer::deserialize(PofReader::Handle hIn) const
    {
    TypeMetadata::View vTypeMetadata = cast<TypeMetadata::View>(f_ohTypeMetadata);
    Object::Holder     oh            = vTypeMetadata->newInstance();

    // set the version identifier
    Evolvable::Handle hEvolvable = cast<Evolvable::Handle>(oh, false);
    bool              fEvolvable = NULL != hEvolvable;
    if (fEvolvable)
        {
        hEvolvable->setDataVersion(hIn->getVersionId());
        }

    for (Iterator::Handle hIter = vTypeMetadata->getAttributes(); hIter->hasNext(); )
        {
        TypeMetadata::AttributeMetadata::View vAttribute = cast<TypeMetadata::AttributeMetadata::View>(hIter->next());
        vAttribute->set(oh, vAttribute->getCodec()->decode(hIn, vAttribute->getIndex()));
        }

    // read any future properties
    Binary::View vBin = hIn->readRemainder();
    if (fEvolvable)
        {
        hEvolvable->setFutureData(vBin);
        }

    return oh;
    }

// ----- private functions --------------------------------------------------

void PofAnnotationSerializer::initialize(int32_t nTypeId, Class::View vClz, bool fAutoIndex)
    {
    Annotation::View vPortable = vClz->getAnnotation(Portable::getStaticClass());
    if (NULL == vPortable)
        {
        COH_THROW_STREAM(IllegalArgumentException,
                "Attempting to use PofAnnotationSerializer for a class ("
                << vClz->getName() << ") that has no Portable annotation")
        }

    // via the builder create the type metadata
    ClassMetadataBuilder::Handle hCmb = ClassMetadataBuilder::create();
    hCmb->setTypeId(nTypeId)
        ->accept(AnnotationVisitor::create(fAutoIndex), vClz);
    coherence::lang::initialize(f_ohTypeMetadata, cast<Object::Handle>(hCmb->build()));
    }

COH_CLOSE_NAMESPACE3

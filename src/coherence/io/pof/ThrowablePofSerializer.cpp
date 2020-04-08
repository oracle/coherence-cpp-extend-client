/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/ThrowablePofSerializer.hpp"

#include "coherence/io/pof/PortableException.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::ArrayList;
using coherence::util::Collection;

COH_REGISTER_POF_SERIALIZER(0, TypedClass<Exception>::create(),
        ThrowablePofSerializer::create());


// ----- constructors -------------------------------------------------------

ThrowablePofSerializer::ThrowablePofSerializer()
    {
    }


// ----- PofSerializer interface --------------------------------------------

void ThrowablePofSerializer::serialize(PofWriter::Handle hOut, Object::View v) const
    {
    writeThrowable(hOut, cast<Exception::View>(v));
    hOut->writeRemainder(NULL);
    }

Object::Holder ThrowablePofSerializer::deserialize(PofReader::Handle hIn) const
    {
    PortableException::Handle he = PortableException::create();
    hIn->registerIdentity(he);
    he->readExternal(hIn);
    hIn->readRemainder();
    return he;
    }


// ----- helpers ------------------------------------------------------------

void ThrowablePofSerializer::writeThrowable(PofWriter::Handle hOut,
        Exception::View ve)
    {
    ObjectArray::View vaTrace     = ve->getStackTrace();
    Collection::Handle hListStack = ArrayList::create();

    for (size32_t i = 0, c = vaTrace->length; i < c; ++i)
        {
        hListStack->add(COH_TO_STRING(vaTrace[i]));
        }

    hOut->writeString(0, ve->getName());
    hOut->writeString(1, ve->getMessage());
    hOut->writeCollection(2, hListStack,
            SystemClassLoader::getInstance()->loadByType(typeid(String)));
    hOut->writeObject(3, ve->getCause());
    }

COH_CLOSE_NAMESPACE3

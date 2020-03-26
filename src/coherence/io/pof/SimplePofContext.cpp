/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/SimplePofContext.hpp"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/pof/PofBufferReader.hpp"
#include "coherence/io/pof/PofBufferWriter.hpp"
#include "coherence/util/LongArrayIterator.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"

#include "private/coherence/util/HashArray.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;
using coherence::util::HashArray;
using coherence::util::LongArrayIterator;
using coherence::util::SafeHashMap;
using coherence::util::extractor::ReflectionExtractor;
using coherence::util::logging::Logger;


// ----- constructors -------------------------------------------------------

SimplePofContext::SimplePofContext()
        : f_hlaClass(self(), HashArray::create()),
          f_hlaSerializer(self(), HashArray::create()),
          f_hMapClassName(self(), SafeHashMap::create())
    {
    }

SimplePofContext::SimplePofContext(const SimplePofContext& that)
        : super(that),
          f_hlaClass(self(), HashArray::create()),
          f_hlaSerializer(self(), HashArray::create()),
          f_hMapClassName(self(), cast<Map::Handle>(that.f_hMapClassName->clone()))
    {
    for (LongArrayIterator::Handle hIter = that.f_hlaClass->iterator(); 
         hIter->hasNext(); )
         {
         Object::View v = hIter->next();
         f_hlaClass->set(hIter->getIndex(), v);
         }
    for (LongArrayIterator::Handle hIter = that.f_hlaSerializer->iterator(); 
         hIter->hasNext(); )
         {
         Object::View v = hIter->next();
         f_hlaSerializer->set(hIter->getIndex(), v);
         }
    }


// ----- SimplePofContext interface -----------------------------------------

void SimplePofContext::registerUserType(int32_t nTypeId, Class::View vClass,
        PofSerializer::View vSerializer)
    {
    validateTypeId(nTypeId);
    COH_ENSURE_PARAM(vClass);
    COH_ENSURE_PARAM(vSerializer);

    COH_SYNCHRONIZED(this)
        {
        if (f_hlaClass->exists(nTypeId))
            {
            COH_THROW_STREAM (IllegalArgumentException, "POF type identifier "
                    << nTypeId << " already defined within SimplePofContext");
            }

        // add class-to-type identifier mapping
        f_hMapClassName->put(vClass->getName(), Integer32::valueOf(nTypeId));

        // add type identifier-to-class mapping
        f_hlaClass->set(nTypeId, vClass);

        // add type identifier-to-serializer mapping
        f_hlaSerializer->set(nTypeId, vSerializer);
        }

    COH_LOG("Registered (" << nTypeId << ", " << vClass << ", "
            << vSerializer << ") with SimplePofContext", 7);
    }

void SimplePofContext::unregisterUserType(int32_t nTypeId)
    {
    COH_SYNCHRONIZED(this)
        {
        // remove class-to-type identifier mapping
        f_hMapClassName->remove(getClass(nTypeId)->getName());

        // remove type identifier-to-class mapping
        f_hlaClass->remove(nTypeId);

        // remove type identifier-to-serializer mapping
        f_hlaSerializer->remove(nTypeId);
        }
    }


// ----- PofContext interface -----------------------------------------------

PofSerializer::View SimplePofContext::getPofSerializer(int32_t nTypeId) const
    {
    validateTypeId(nTypeId);

    PofSerializer::View vSerializer = cast<PofSerializer::View>(f_hlaSerializer->get(nTypeId));
    if (NULL == vSerializer)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "unknown user type: " << nTypeId);
        }
    return vSerializer;
    }

int32_t SimplePofContext::getUserTypeIdentifier(Object::View v) const
    {
    COH_ENSURE_PARAM(v);

    String::View    vsClass  = Class::getClassName(v);
    Integer32::View vITypeId = cast<Integer32::View>(f_hMapClassName->
            get(vsClass));

    if (NULL == vITypeId)
        {
        // TODO: add support for walking the class inheritance hierarchy, for
        // now it is only supported for a few well known types; COHCPP-207
        if (instanceof<ReflectionExtractor::View>(v))
            {
            return getUserTypeIdentifier(Class::getTypeName(typeid(ReflectionExtractor)));
            }
        else if (instanceof<Exception::View>(v))
            {
            return getUserTypeIdentifier(Class::getTypeName(typeid(Exception)));
            }

        COH_THROW_STREAM (IllegalArgumentException,
                "unknown user type: " << vsClass);
        }
    return vITypeId->getValue();
    }

int32_t SimplePofContext::getUserTypeIdentifier(Class::View vClass) const
    {
    COH_ENSURE_PARAM(vClass);
    return getUserTypeIdentifier(vClass->getName());
    }

int32_t SimplePofContext::getUserTypeIdentifier(String::View vsClass) const
    {
    COH_ENSURE_PARAM(vsClass);

    Integer32::View vITypeId = cast<Integer32::View>(f_hMapClassName->
            get(vsClass));

    if (NULL == vITypeId)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "unknown user type: " << vsClass);
        }
    return vITypeId->getValue();
    }

String::View SimplePofContext::getClassName(int32_t nTypeId) const
    {
    return getClass(nTypeId)->getName();
    }

Class::View SimplePofContext::getClass(int32_t nTypeId) const
    {
    validateTypeId(nTypeId);

    Class::View vClass = cast<Class::View>(f_hlaClass->get(nTypeId));
    if (NULL == vClass)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "unknown user type: " << nTypeId);
        }
    return vClass;
    }

bool SimplePofContext::isUserType(Object::View v) const
    {
    COH_ENSURE_PARAM(v);

    // TODO: add support for walking the class inheritance hierarchy, for
    // now it is only supported for a few well known types; COHCPP-207
    return isUserType(Class::getClassName(v)) ||
           instanceof<ReflectionExtractor::View>(v) ||
           instanceof<Exception::View>(v);
    }

bool SimplePofContext::isUserType(Class::View vClass) const
    {
    COH_ENSURE_PARAM(vClass);
    return isUserType(vClass->getName());
    }

bool SimplePofContext::isUserType(String::View vsClass) const
    {
    COH_ENSURE_PARAM(vsClass);
    return f_hMapClassName->containsKey(vsClass);
    }


// ----- Serializer interface -----------------------------------------------

void SimplePofContext::serialize(WriteBuffer::BufferOutput::Handle hOut,
        Object::View v) const
    {
    PofBufferWriter::Handle hWriter = PofBufferWriter::create(hOut, this);

    // COH-5065: due to the complexity of maintaining references
    // in future data, we won't support them for Evolvable objects
    if (isReferenceEnabled() && !(instanceof<Evolvable::View>(v)))
        {
        hWriter->enableReference();
        }

    hWriter->writeObject(-1, v);
    }

Object::Holder SimplePofContext::deserialize(
        ReadBuffer::BufferInput::Handle hIn) const
    {
    PofBufferReader::Handle hReader = PofBufferReader::create(hIn, this);
    return hReader->readObject(-1);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> SimplePofContext::toString() const
    {
    return COH_TO_STRING("SimplePofContext: registered classes " << f_hlaClass);
    }


// ----- internal methods ---------------------------------------------------

void SimplePofContext::validateTypeId(int32_t nTypeId) const
    {
    if (nTypeId < 0)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "negative user type identifier: " << nTypeId);
        }
    }


// ----- accessors ----------------------------------------------------------

bool SimplePofContext::isReferenceEnabled() const
    {
    return m_fReferenceEnabled;
    }

void SimplePofContext::setReferenceEnabled(bool fEnabled)
    {
    m_fReferenceEnabled = fEnabled;
    }

void SimplePofContext::setReferenceEnabled(bool fEnabled) const
    {
    m_fReferenceEnabled = fEnabled;
    }

COH_CLOSE_NAMESPACE3

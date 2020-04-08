/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/ReflectionExtractor.hpp"

#include "coherence/io/pof/PofHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofHelper;

COH_REGISTER_PORTABLE_CLASS(55, ReflectionExtractor);


// ----- constructors -------------------------------------------------------

ReflectionExtractor::ReflectionExtractor()
    : f_vsMethod(self()), f_vaParam(self()), m_vMethod(self(), NULL, /*fMutable*/ true)
    {
    m_nTarget = value;
    }
   
ReflectionExtractor::ReflectionExtractor(String::View vsMethod,
        ObjectArray::View vaParam, int32_t nTarget)
    : f_vsMethod(self(), vsMethod), f_vaParam(self(), vaParam),
      m_vMethod(self(), NULL, /*fMutable*/ true)
    {
    COH_ENSURE_PARAM(vsMethod);
    m_nTarget = nTarget;
    }

ReflectionExtractor::ReflectionExtractor(const ReflectionExtractor& that)
    : f_vsMethod(self(), that.f_vsMethod),
      f_vaParam(self(), cast<ObjectArray::View>(Object::clone(that.f_vaParam))),
      m_vMethod(self(), that.m_vMethod, /*fMutable*/ true)
    {
    // ReflectionExtractor is cloneable only because MSVC tries to generate
    // a copy constructor for it, which blows up as MemberHandles do not
    // support automatic copy construction
    }


// ----- ValueExtractor interface -------------------------------------------

Object::Holder ReflectionExtractor::extract(Object::Holder ohTarget) const
    {
    ObjectArray::View vaParams = getParameters();
    Method::View      vMethod = m_vMethod;
    if (vMethod == NULL || !(vMethod->getDeclaringClass()->isInstance(ohTarget)))
        {
        ClassLoader::View vLoader = SystemClassLoader::getInstance();
        m_vMethod = vMethod = vLoader->loadByType(Class::getTypeInfo(ohTarget))->getMethod(
                getMethodName(), vaParams, Method::modifier_const);
        }

    return vMethod->invoke(ohTarget, vaParams);
    }


// ----- PortableObject interface -------------------------------------------

void ReflectionExtractor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vsMethod, hIn->readString(0));

    // slot #1 is taken by pre-Coherence 3.5 versions for the number of
    // arguments in the parameter array
    size32_t cParams = PofHelper::validateIncomingSize(hIn->readInt32(1));
    if (cParams > 0)
        {
        // fully backwards compatible implementation
        ObjectArray::Handle haParam = ObjectArray::create(cParams);

        for (size32_t i = 0; i < cParams; i++)
            {
            haParam[i] = hIn->readObject(
                    PofHelper::validateOutgoingSize(i + 2));
            }
        initialize(f_vaParam, haParam);
        } 
    else
        {
        // slot #2 is used @since Coherence 3.5 to store the entirety
        // of the arguments (as opposed to the first of the arguments)
        initialize(f_vaParam, hIn->readObjectArray(2));
        m_nTarget = hIn->readInt32(3);
        }
    }

void ReflectionExtractor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeString(0, getMethodName());
    // slot #1 is not used @since Coherence 3.5
    hOut->writeObjectArray(2, getParameters());
    hOut->writeInt32(3, m_nTarget);
    }


// ----- Object interface ---------------------------------------------------

bool ReflectionExtractor::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }
    ReflectionExtractor::View that = cast<ReflectionExtractor::View>(v, false);
    if (that != NULL)
        {
        return (this->getMethodName()->equals(that->getMethodName())) &&
             Object::equals(this->getParameters(), that->getParameters());
        }
    return false;
    }

size32_t ReflectionExtractor::hashCode() const
    {
    return getMethodName()->hashCode();
    }

TypedHandle<const String> ReflectionExtractor::toString() const
    {
    String::View vs = "";
    if (m_nTarget == key)
        {
        vs = COH_TO_STRING(vs << ".getKey()");
        }

    return COH_TO_STRING(vs << '.' << getMethodName() << ' ' << getParameters());
    }


// ----- data member accessors ----------------------------------------------

String::View ReflectionExtractor::getMethodName() const
    {
    return f_vsMethod;
    }

ObjectArray::View ReflectionExtractor::getParameters() const
    {
    return f_vaParam;
    }

COH_CLOSE_NAMESPACE3

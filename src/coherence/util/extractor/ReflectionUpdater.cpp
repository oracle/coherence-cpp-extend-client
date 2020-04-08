/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/ReflectionUpdater.hpp"


COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(56, ReflectionUpdater);


// ----- constructors -------------------------------------------------------

ReflectionUpdater::ReflectionUpdater()
    : f_vsMethod(self()), m_vMethod(self(), NULL, /*fMutable*/ true)
    {
    }

ReflectionUpdater::ReflectionUpdater(String::View vsMethod)
    : f_vsMethod(self(), vsMethod), m_vMethod(self(), NULL, /*fMutable*/ true)
    {
    COH_ENSURE_PARAM(vsMethod);
    }

ReflectionUpdater::ReflectionUpdater(const ReflectionUpdater& that)
    : f_vsMethod(self(), that.f_vsMethod),
      m_vMethod(self(), that.m_vMethod, /*fMutable*/ true)
    {
    // ReflectionUpdater is cloneable only because MSVC tries to generate
    // a copy constructor for it, which blows up as MemberHandles do not
    // support automatic copy construction
    }



// ----- ValueUpdater interface ---------------------------------------------

void ReflectionUpdater::update(Object::Handle hTarget,
        Object::Holder ohValue) const
    {
    ObjectArray::Handle haParams = ObjectArray::create(1);
    haParams[0] = ohValue;

    Method::View vMethod = m_vMethod;
    if (vMethod == NULL || !(vMethod->getDeclaringClass()->isInstance(hTarget)))
        {
        ClassLoader::View vLoader = SystemClassLoader::getInstance();
        m_vMethod = vMethod = vLoader->loadByType(Class::getTypeInfo(hTarget))->getMethod(
                getMethodName(), haParams, Method::modifier_mutable);
        }

    vMethod->invoke(hTarget, haParams);
    }


// ----- PortableObject interface -------------------------------------------

void ReflectionUpdater::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vsMethod, hIn->readString(0));
    }

void ReflectionUpdater:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeString(0, getMethodName());
    }


// ----- Object interface ---------------------------------------------------

bool ReflectionUpdater::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }
    ReflectionUpdater::View that = cast<ReflectionUpdater::View>(v, false);
    if (that != NULL)
        {
        return this->getMethodName()->equals(that->getMethodName());
        }
    return false;
    }

size32_t ReflectionUpdater::hashCode() const
    {
    return getMethodName()->hashCode();
    }

TypedHandle<const String> ReflectionUpdater::toString() const
    {
    return COH_TO_STRING('&' << getMethodName());
    }


// ----- data member accessors ----------------------------------------------

String::View ReflectionUpdater::getMethodName() const
    {
    return f_vsMethod;
    }

COH_CLOSE_NAMESPACE3

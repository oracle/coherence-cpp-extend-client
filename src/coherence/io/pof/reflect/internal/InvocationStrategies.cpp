/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/pof/reflect/internal/InvocationStrategies.hpp"

#include "private/coherence/io/pof/reflect/internal/NameManglers.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

// ----- MethodInvocationStrategy: constructors -----------------------------

InvocationStrategies::MethodInvocationStrategy::MethodInvocationStrategy(Method::View vMethod)
    : f_vMethGetter(self()), f_vMethSetter(self())
    {
    Method::View vMethComp = getCompliment(vMethod);
    bool         fSetter   = NULL == vMethod || NULL == vMethod->getReturnType();

    initialize(fSetter ? vMethComp : vMethod,
               fSetter ? vMethod : vMethComp);
    }

InvocationStrategies::MethodInvocationStrategy::MethodInvocationStrategy(Method::View vMethGetter, Method::View vMethSetter)
    : f_vMethGetter(self()), f_vMethSetter(self())
    {
    initialize(vMethGetter, vMethSetter);
    }

// ----- InvocationStrategy interface ---------------------------------------

Object::Holder InvocationStrategies::MethodInvocationStrategy::get(Object::View vhContainer) const
    {
    return f_vMethGetter->invoke(vhContainer);
    }

void InvocationStrategies::MethodInvocationStrategy::set(Object::Handle ohContainer, Object::Holder oValue) const
    {
    if (NULL == oValue)
        {
        return;
        }

    ObjectArray::Handle haAttributes = ObjectArray::create(1);
    haAttributes[0] = oValue;

    f_vMethSetter->invoke(ohContainer, haAttributes);
    }

// ----- MethodInvocationStrategy: helpers ----------------------------------

Method::View InvocationStrategies::MethodInvocationStrategy::getCompliment(Method::View vMethod) const
    {
    if (NULL == vMethod)
        {
        return NULL;
        }

    int32_t nSetter = 0;
    if (NULL == vMethod->getReturnType())
        {
        if (vMethod->getParameterTypes()->length <= 0)
            {
            COH_THROW_STREAM(IllegalArgumentException,
                    "Method (" << vMethod << ") should have a parameter")
            }

        Class::View vClzBoolean = SystemClassLoader::getInstance()->loadByType(typeid(Boolean));
        nSetter = vClzBoolean->equals(cast<Class::View>(vMethod->getParameterTypes()[0]))
                ? 2 : 1;
        }

    String::View vsMethodName = COH_TO_STRING(
            (nSetter == 2 ? "is" : nSetter == 1 ? "get" : "set")
            << vMethod->getName()->substring(vMethod->getName()->startsWith("is") ? 2 : 3));

    Method::View vMethComp;
    try
        {
        if (nSetter == 0)
            {
            ObjectArray::Handle haParams = ObjectArray::create(1);
            haParams[0] = vMethod->getReturnType();

            vMethComp = vMethod->getDeclaringClass()->getDeclaredMethod(vsMethodName, haParams);
            }
        else
            {
            vMethComp = vMethod->getDeclaringClass()->getDeclaredMethod(vsMethodName);
            }
        }
    catch(...)
        {
        COH_THROW_STREAM(NoSuchMethodException,
                "An error occurred in discovering the compliment of method = " << vMethod
               << ", assuming compliment method name is " << vsMethodName)
        }

    if (NULL == vMethComp)
        {
        COH_THROW_STREAM(NoSuchMethodException,
                "Could not derive the compliment method of " << vMethod
               << ", assuming compliment method name is " << vsMethodName)
        }

    return vMethComp;
    }

void InvocationStrategies::MethodInvocationStrategy::initialize(Method::View vMethGetter, Method::View vMethSetter)
    {
    if (NULL == vMethGetter || NULL == vMethSetter)
        {
        COH_THROW_STREAM(IllegalArgumentException,
                "The getter or setter can not be null in constructing a MethodInvocationStrategy")
        }

    coherence::lang::initialize(f_vMethGetter, vMethGetter);
    coherence::lang::initialize(f_vMethSetter, vMethSetter);
    }

COH_CLOSE_NAMESPACE5

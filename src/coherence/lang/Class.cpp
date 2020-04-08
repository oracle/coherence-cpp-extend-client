/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Class.hpp"

#include "coherence/lang/ClassNotFoundException.hpp"
#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/lang/NoSuchMethodException.hpp"

#include "coherence/util/CircularArrayList.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"

#include "private/coherence/lang/ClassInfo.hpp"
#include "private/coherence/native/NativeABI.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeABI;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::CircularArrayList;
using coherence::util::List;

namespace
    {
    String::View ensureClassName(const std::type_info& info)
        {
        ClassInfo* pInfo = ClassInfo::findByType(info);
        COH_ENSURE(pInfo != NULL);
        return pInfo->getClassName();
        }

    /**
     * Return the simple name for a given class name.
     *
     * see getSimpleName()
     *
     * @param  vsClassName the class name to simplify
     *
     * @return the simple name for a given class name
     *
     * @since 12.2.1.3
     */
    String::View calculateSimpleName(String::View vsClassName)
        {
        if (vsClassName == NULL)
            {
            return vsClassName;
            }

        size32_t len = vsClassName->length();
        size32_t of  = vsClassName->lastIndexOf("::");

        if (of == String::npos || of + 2 >= len)
            {
            // no package name delimiter or malformed string
            return vsClassName;
            }
        else
            {
            return vsClassName->substring(of + 2);
            }
        }

    /**
     * Test if vab could be passed into a method taking the params
     */
    bool isSignatureCompatible(ObjectArray::View vaa, ObjectArray::View vab)
        {
        size32_t ca = vaa == NULL ? 0 : vaa->length;
        size32_t cb = vab == NULL ? 0 : vab->length;

        if (ca == cb)
            {
            Class::View vClassObject = SystemClassLoader::getInstance()
                ->loadByType(typeid(Object));

            for (size32_t i = 0; i < ca; ++i)
                {
                Class::View  vClassSig  = cast<Class::View>(vaa[i]);
                Object::View vTest      = vab[i];

                if (!(vTest == NULL || vClassSig == vClassObject ||
                    vClassSig->isInstance(vTest) ||
                    (instanceof<Class::View>(vTest) &&
                     vClassSig->isAssignableFrom(cast<Class::View>(vTest)))))
                    {
                    // no match
                    return false;
                    }
                }
            return true;
            }
        return false;
        }

    /**
     * Test if the defined modifiers match the desired modifiers.
     *
     * @return true if they match
     */
    bool isModifierCompatible(int32_t nModDef, int32_t nModMatch)
        {
        if ((nModMatch & Method::modifier_const) != 0 &&
            (nModDef   & Method::modifier_const) == 0)
            {
            return false;
            }
        else if ((nModMatch & Method::modifier_mutable) != 0 &&
                 (nModDef   & Method::modifier_mutable) == 0)
            {
            return false;
            }
        else if ((nModMatch & Method::modifier_instance) != 0 &&
                 (nModDef   & Method::modifier_instance) == 0)
            {
            return false;
            }
        else if ((nModMatch & Method::modifier_static) != 0 &&
                 (nModDef   & Method::modifier_static) == 0)
            {
            return false;
            }
        return true;
        }
    }

Class::Class(const std::type_info& info)
    : f_vsName(self(), ensureClassName(info)),
      m_hMapMethods(self())
    {
    }

Object::Handle Class::newInstance(ObjectArray::View vaParam) const
    {
    return cast<Object::Handle>(getDeclaredMethod(
            "create", vaParam, Method::modifier_static)
            ->invoke(NULL, vaParam));
    }


String::View Class::getName() const
    {
    return f_vsName;
    }

String::View Class::getSimpleName() const
    {
    return calculateSimpleName(f_vsName);
    }

bool Class::equals(Object::View that) const
    {
    return instanceof<Class::View>(that) &&
        Object::equals(getName(), cast<Class::View>(that)->getName());
    }

size32_t Class::hashCode() const
    {
    return Object::hashCode(getName());
    }

String::View Class::getClassName(Object::View v)
    {
    return getTypeName(getTypeInfo(v));
    }

String::View Class::getSimpleClassName(Object::View v)
    {
    return getSimpleTypeName(getTypeInfo(v));
    }

const std::type_info& Class::getTypeInfo(Object::View v)
    {
    const Object& refV = *v; // assign to local variable to avoid -Wpotentially-evaluated-expression
    return typeid(refV);
    }

String::View Class::getTypeName(const std::type_info& info)
    {
    ClassInfo* pInfo = ClassInfo::findByType(info);
    if (NULL == pInfo)
        {
        // this can happen only during static destruction; since this is part
        // of the user API, we force a non-cached resolution
        return NativeABI::demangle(info.name());
        }
    return pInfo->getClassName();
    }

String::View Class::getSimpleTypeName(const std::type_info& info)
    {
    return calculateSimpleName(getTypeName(info));
    }

bool Class::typeEquals(Object::View v1, Object::View v2)
    {
    return getClassName(v1)->equals(getClassName(v2));
    }

Class::Handle Class::declare(Method::View vMethod)
    {
    COH_SYNCHRONIZED (this)
        {
        Map::Handle hMap = cast<Map::Handle>(m_hMapMethods);
        if (hMap == NULL)
            {
            m_hMapMethods = hMap = HashMap::create();
            }
        String::View   vsMethod = vMethod->getName();
        Object::Holder oh       = hMap->get(vsMethod);

        if (oh == NULL)
            {
            hMap->put(vsMethod, vMethod);
            }
        else if (instanceof<Method::View>(oh))
            {
            List::Handle hList = CircularArrayList::create();
            hMap->put(vsMethod, hList);
            hList->add(oh);
            hList->add(vMethod);
            }
        else
            {
            List::Handle hList = cast<List::Handle>(oh);
            hList->add(vMethod);
            }
        }

    return this;
    }

Method::View Class::getDeclaredMethod(String::View vsName, ObjectArray::View vaParams,
        int32_t nMod, bool fThrow) const
    {
    Exception::View vExLast;
    COH_SYNCHRONIZED (this)
        {
        Map::View vMap = cast<Map::View>(m_hMapMethods);
        if (vMap != NULL)
            {
            Object::View v = vMap->get(vsName);
            if (instanceof<Method::View>(v))
                {
                try
                    {
                    Method::View      vMethod = cast<Method::View>(v);
                    ObjectArray::View vaMatch = vMethod->getParameterTypes();
                    if (isSignatureCompatible(vaMatch, vaParams) &&
                        isModifierCompatible(vMethod->getModifiers(), nMod))
                        {
                        return vMethod;
                        }
                    }
                catch (ClassNotFoundException::View vEx)
                    {
                    // see note below
                    vExLast = vEx;
                    }
                }
            else if (instanceof<List::View>(v))
                {
                List::View  vList = cast<List::View>(v);
                for (Iterator::Handle hIter = vList->iterator(); hIter->hasNext(); )
                    {
                    try
                        {
                        Method::View      vMethod = cast<Method::View>(hIter->next());
                        ObjectArray::View vaMatch = vMethod->getParameterTypes();
                        int32_t           nModDef = vMethod->getModifiers();
                        if (isSignatureCompatible(vaMatch, vaParams) &&
                            isModifierCompatible(nModDef, nMod))
                            {
                            return vMethod;
                            }
                        }
                    catch (ClassNotFoundException::View vEx)
                        {
                        // Note: This can happen if we've registered a Method
                        // with argument types which haven't been registered
                        // with the ClassLoader. Finish the search, but hold
                        // onto the exception so that it can be re-thrown if
                        // no match is found
                        vExLast = vEx;
                        }
                    }
                }
            }
        }

    if (fThrow)
        {
        COH_THROW (NoSuchMethodException::create(COH_TO_STRING(getName() << "::"
                << vsName << " " << vaParams), vExLast));
        }
    return NULL;
    }

ObjectArray::View Class::getDeclaredMethods() const
    {
    COH_SYNCHRONIZED (this)
        {
        Map::View   vMap   = cast<Map::View>(m_hMapMethods);
        List::Handle hList = CircularArrayList::create();

        if (vMap != NULL)
            {
            for (Iterator::Handle hIter = vMap->values()->iterator(); hIter->hasNext(); )
                {
                Object::View v = hIter->next();
                if (instanceof<Method::View>(v))
                    {
                    hList->add(v);
                    }
                else
                    {
                    hList->addAll(cast<List::View>(v));
                    }
                }
            }

        return hList->toArray();
        }
    }

Method::View Class::getMethod(String::View vsName, ObjectArray::View vaParams,
        int32_t nMod, bool fThrow) const
    {
    Method::View vMethod = getDeclaredMethod(vsName, vaParams, nMod, false);

    if (vMethod == NULL)
        {
        // check for inherited methods

        // first test interfaces
        ObjectArray::View vaInterfaces = getInterfaces();
        for (size32_t i = 0, c = vaInterfaces->length; vMethod == NULL && i < c; ++i)
            {
            vMethod = cast<Class::View>(vaInterfaces[i])->getMethod(vsName, vaParams, nMod, false);
            }

        if (vMethod == NULL)
            {
            // next recursively test super classes and their interfaces
            Class::View vClassSuper = getSuperclass();
            if (vClassSuper != NULL)
                {
                vMethod = vClassSuper->getMethod(vsName, vaParams, nMod, false);

                if (vMethod == NULL && fThrow)
                    {
                    COH_THROW_STREAM (NoSuchMethodException, getName() << "::"
                            << vsName << " " << vaParams);
                    }
                }
            }
        }

    return vMethod;
    }

ObjectArray::View Class::getMethods() const
    {
    ObjectArray::View va     = getDeclaredMethods();
    Class::View       vSuper = getSuperclass();
    if (vSuper == NULL)
        {
        return va;
        }
    else
        {
        ObjectArray::View vaSuper = vSuper->getMethods();
        if (va->length == 0)
            {
            return vaSuper;
            }
        else if (vaSuper->length == 0)
            {
            return va;
            }
        ObjectArray::Handle ha = ObjectArray::create(va->length + vaSuper->length);
        ObjectArray::copy(va, 0, ha);
        ObjectArray::copy(vaSuper, 0, ha, va->length);
        return ha;
        }
    }

Class::Handle Class::annotate(Annotation::View vAnnotation)
    {
    addAnnotation(vAnnotation);
    return this;
    }


// ----- AnnotatedElement interface -----------------------------------------

AnnotatedElement::View Class::getSuperelement() const
    {
    return getSuperclass();
    }

COH_CLOSE_NAMESPACE2

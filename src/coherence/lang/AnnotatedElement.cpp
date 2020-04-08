/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/AnnotatedElement.hpp"

#include "coherence/lang.ns"

#include "coherence/lang/annotation/Inherited.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/SafeHashMap.hpp"

COH_OPEN_NAMESPACE2(coherence, lang)

using coherence::lang::annotation::Inherited;

using coherence::util::ArrayList;
using coherence::util::Iterator;
using coherence::util::List;
using coherence::util::SafeHashMap;

// ----- constructors -------------------------------------------------------

AnnotatedElement::AnnotatedElement()
    : f_hMapAnnotations(self(), NULL, /*fMutable*/ true), f_hListAnnotationsDeclared(self())
    {
    }


// ----- AnnotatedElement interface -----------------------------------------

Annotation::View AnnotatedElement::getAnnotation(TypedHandle<const Class> vClassAnnotation) const
    {
    List::View vList = cast<List::View>(f_hListAnnotationsDeclared);
    if (vList == NULL)
        {
        if (vClassAnnotation == this)
            {
            // for the case of meta-annotations such as Inherited, we need
            // to short circuit things when an annotation finds itself
            // specifically this is need for Inherited to avoid endless
            // recusion
            return NULL;
            }

        AnnotatedElement::View vSuper = getSuperelement();
        if (vSuper != NULL && vClassAnnotation->isAnnotationPresent(
            SystemClassLoader::getInstance()->loadByType(typeid(Inherited))))
            {
            return vSuper->getAnnotation(vClassAnnotation);
            }
        return NULL;
        }

    Map::Handle hMap = cast<Map::Handle>(f_hMapAnnotations);
    if (hMap == NULL)
        {
        COH_SYNCHRONIZED (this)
            {
            hMap = SafeHashMap::create();

            for (Iterator::Handle hIter = vList->iterator(); hIter->hasNext(); )
                {
                Annotation::View vAn  = cast<Annotation::View>(hIter->next());
                Class::View      vKey = vAn->annotationType();
                hMap->put(vKey, vAn);
                }

            initialize(f_hMapAnnotations, hMap);
            }
        }

    Annotation::View vAn = cast<Annotation::View>(hMap->get(vClassAnnotation));
    if (vAn == NULL)
        {
        AnnotatedElement::View vSuper = getSuperelement();
        if (vSuper != NULL && vClassAnnotation->isAnnotationPresent(
            SystemClassLoader::getInstance()->loadByType(typeid(Inherited))))
            {
            vAn = vSuper->getAnnotation(vClassAnnotation);
            if (vAn != NULL)
                {
                hMap->put(vClassAnnotation, vAn);
                }
            }
        }

    return vAn;
    }

ObjectArray::View AnnotatedElement::getAnnotations() const
    {
    ObjectArray::View      va     = getDeclaredAnnotations();
    AnnotatedElement::View vSuper = getSuperelement();
    if (vSuper == NULL)
        {
        return va;
        }
    else
        {
        ObjectArray::View vaSuper = vSuper->getAnnotations();
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

ObjectArray::View AnnotatedElement::getDeclaredAnnotations() const
    {
    List::View vList = cast<List::View>(f_hListAnnotationsDeclared);
    if (vList == NULL)
        {
        return ObjectArray::create(0);
        }
    else
        {
        return vList->toArray();
        }
    }

bool AnnotatedElement::isAnnotationPresent(TypedHandle<const Class> vClassAnnotation) const
    {
    return getAnnotation(vClassAnnotation) != NULL;
    }

AnnotatedElement::Handle AnnotatedElement::addAnnotation(Annotation::View vAnnotation)
    {
    COH_SYNCHRONIZED (this)
        {
        List::Handle hList = cast<List::Handle>(f_hListAnnotationsDeclared);
        if (hList == NULL)
            {
            hList = ArrayList::create();
            initialize(f_hListAnnotationsDeclared, hList);
            }
        hList->add(vAnnotation);
        }
    return this;
    }

AnnotatedElement::View AnnotatedElement::getSuperelement() const
    {
    return NULL;
    }

COH_CLOSE_NAMESPACE2

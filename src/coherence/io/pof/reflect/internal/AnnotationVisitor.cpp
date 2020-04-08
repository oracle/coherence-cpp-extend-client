/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/pof/reflect/internal/AnnotationVisitor.hpp"

#include "coherence/io/pof/annotation/Portable.hpp"
#include "coherence/io/pof/annotation/PortableProperty.hpp"
#include "coherence/io/pof/reflect/Codecs.hpp"

#include "coherence/util/CircularArrayList.hpp"

#include "private/coherence/io/pof/reflect/internal/ClassMetadataBuilder.hpp"
#include "private/coherence/io/pof/reflect/internal/InvocationStrategies.hpp"
#include "private/coherence/io/pof/reflect/internal/NameManglers.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

using coherence::io::pof::annotation::Portable;
using coherence::io::pof::annotation::PortableProperty;
using coherence::io::pof::reflect::Codecs;

using coherence::lang::IllegalArgumentException;

using coherence::util::CircularArrayList;

// ----- constructors: AnnotationVisitor ------------------------------------

AnnotationVisitor::AnnotationVisitor(bool fAutoIndex)
    : m_fAutoIndex(fAutoIndex)
    {
    }

// ----- AnnotationVisitor interface ----------------------------------------

void AnnotationVisitor::visit(Object::Holder ohBuilder, Class::View vClz) const
    {
    if (!vClz->isAnnotationPresent(Portable::getStaticClass()))
        {
        return;
        }

    ClassMetadataBuilder::Holder hTcmb      = cast<ClassMetadataBuilder::Holder>(ohBuilder);
    ClassMetadataBuilder::Handle hCmb       = cast<ClassMetadataBuilder::Handle>(hTcmb);
    Portable::View               vPortable  = cast<Portable::View>(vClz->getAnnotation(Portable::getStaticClass()));
    bool                         fAutoIndex = m_fAutoIndex;

    hCmb->setClass(vClz);

    List::Handle hListFieldsExcluded = CircularArrayList::create();
    // get field level information (when available)

    // get method level information
    ObjectArray::View vaMethods = vClz->getDeclaredMethods();

    for (uint32_t i = 0; i < vaMethods->length; ++i)
        {
        Method::View     vMethod = cast<Method::View>(vaMethods[i]);
        Annotation::View vAnno   = vMethod->getAnnotation(PortableProperty::getStaticClass());
        if (NULL == vAnno)
            {
            continue;
            }

        String::View vsMethodName = vMethod->getName();
        if (vsMethodName->startsWith("get") || vsMethodName->startsWith("set")
            || vsMethodName->startsWith("is"))
            {
            PortableProperty::View vPortableProperty = cast<PortableProperty::View>(vAnno);
            String::View           vsName            = NameManglers::getMethodMangler()->mangle(vsMethodName);

            if (hListFieldsExcluded->contains(vsName))
                {
                continue;
                }

            // TODO: when the field level information is available we will need
            //       to modify this code to check against a black-list, i.e.
            //       portable properties derived from field inspection
            if (!fAutoIndex && vPortableProperty->getIndex() < 0)
                {
                COH_THROW_STREAM(IllegalArgumentException,
                        "A POF Index must be specified for the method "
                        << vClz->getName() << "::" << vsMethodName << " by specifying "
                        << "within the annotation or enabling auto indexing")
                }

            InvocationStrategy::View vStrategy = InvocationStrategies::MethodInvocationStrategy::create(vMethod);
            hCmb->addAttribute(hCmb->newAttribute()
                    ->setName(vsName)
                    ->setCodec(Codecs::getCodec(vPortableProperty->getCodec()))
                    ->setInvocationStrategy(vStrategy)
                    ->setIndex(vPortableProperty->getIndex())->build());

            hListFieldsExcluded->add(vsName);
            }
        }
    }

COH_CLOSE_NAMESPACE5

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/ChainedExtractor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"
#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(50, ChainedExtractor);


// ----- constructors -------------------------------------------------------

ChainedExtractor::ChainedExtractor()
    {
    }

ChainedExtractor::ChainedExtractor(ObjectArray::View vaExtractor)
    : super(vaExtractor)
    {
    }

ChainedExtractor::ChainedExtractor(ValueExtractor::View vExtractor1,
        ValueExtractor::View vExtractor2)
    : super(vExtractor1, vExtractor2)
    {
    }


// ----- AbstractExtractor interface ----------------------------------------

Object::Holder ChainedExtractor::extract(Object::Holder ohTarget) const
    {
    ObjectArray::View vaExtractor = getExtractors();
    for (size32_t i = 0, c = vaExtractor->length; i < c && ohTarget != NULL; i++)
        {
        ohTarget = cast<ValueExtractor::View>(vaExtractor[i])->extract(ohTarget);
        }

    return ohTarget;
    }

Object::Holder ChainedExtractor::extractFromEntry(Map::Entry::Holder ohEntry) const
    {
    ObjectArray::View vaExtractor = getExtractors();
    Object::Holder    ohTarget    = InvocableMapHelper::extractFromEntry(
            cast<ValueExtractor::View>(vaExtractor[0]), ohEntry);

    for (size32_t i = 1, c = vaExtractor->length; i < c && ohTarget != NULL; i++)
        {
        ohTarget = cast<ValueExtractor::View>(vaExtractor[i])->extract(ohTarget);
        }

    return ohTarget;
    }


// ----- helpers ------------------------------------------------------------

ObjectArray::View ChainedExtractor::createExtractors(String::View vsName)
    {
    ObjectArray::Handle   haMethod    = StringHelper::split(vsName, ".");
    size32_t              cMethods    = haMethod->length;
    ObjectArray::Handle   haExtractor = ObjectArray::create(cMethods);
    for (size32_t i = 0; i < cMethods; i++)
        {
        haExtractor[i] = ReflectionExtractor::create(
                cast<String::View>(haMethod[i]));
        }
    return haExtractor;
    }

COH_CLOSE_NAMESPACE3

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/MultiExtractor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/ChainedExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"
#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::util::comparator::SafeComparator;

COH_REGISTER_PORTABLE_CLASS(54, MultiExtractor);


// ----- constructors -------------------------------------------------------

MultiExtractor::MultiExtractor()
    {
    }

MultiExtractor::MultiExtractor(ObjectArray::View vaExtractor)
    : super(vaExtractor)
    {
    }


// ----- AbstractExtractor methods ------------------------------------------

Object::Holder MultiExtractor::extract(Object::Holder ohTarget) const
    {
    if (ohTarget == NULL)
        {
        return NULL;
        }

    ObjectArray::View   vaExtractor = getExtractors();
    size32_t            cExtractors = vaExtractor->length;
    ObjectArray::Handle hoaValue    = ObjectArray::create(cExtractors);
    for (size32_t i = 0; i < cExtractors; i++)
        {
        hoaValue[i] =
                cast<ValueExtractor::View>(vaExtractor[i])->extract(ohTarget);
        }

    return ReadOnlyArrayList::create(hoaValue);
    }

Object::Holder MultiExtractor::extractFromEntry(Map::Entry::Holder ohEntry) const
    {
    ObjectArray::View   vaExtractor = getExtractors();
    size32_t            cExtractors = vaExtractor->length;
    ObjectArray::Handle hoaValue    = ObjectArray::create(cExtractors);

    for (size32_t i = 0; i < cExtractors; i++)
        {
        hoaValue[i] = InvocableMapHelper::extractFromEntry(
                cast<ValueExtractor::View>(vaExtractor[i]), ohEntry);
        }

    return ReadOnlyArrayList::create(hoaValue);
    }


// ----- QueryMapComparator interface ---------------------------------------

int32_t MultiExtractor::compareEntries(QueryMap::Entry::View vEntry1,
                QueryMap::Entry::View vEntry2) const
    {

    ObjectArray::View vaExtractor = getExtractors();
    for (size32_t i = 0, c = vaExtractor->length; i < c; i++)
        {
        ValueExtractor::Handle vExtractor = cast<ValueExtractor::Handle>(
                vaExtractor[i]);
        int32_t iResult = SafeComparator::compareSafe(NULL,
                vEntry1->extract(vExtractor), vEntry2->extract(vExtractor));
        if (iResult != 0)
            {
            return iResult;
            }
        }
    return 0;
    }


// ----- helpers ------------------------------------------------------------

ObjectArray::View MultiExtractor::createExtractors(String::View vsName)
    {
    ObjectArray::Handle   haMethod    = StringHelper::split(vsName, ",");
    size32_t              cMethods    = haMethod->length;
    ObjectArray::Handle   haExtract   = ObjectArray::create(cMethods);
    for (size32_t i = 0; i < cMethods; i++)
        {
        String::View vsMethod = cast<String::View>(haMethod[i]);
         if (vsMethod->indexOf('.') == String::npos)
            {
            haExtract[i] = ReflectionExtractor::create(vsMethod);
            }
        else
            {
            haExtract[i] = ChainedExtractor::create(
                        ChainedExtractor::createExtractors(vsMethod));
            }
        }
    return haExtract;
    }

COH_CLOSE_NAMESPACE3

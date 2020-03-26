/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/CompositeAggregator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(26, CompositeAggregator);


// ----- factory methods ----------------------------------------------------

CompositeAggregator::Handle CompositeAggregator::create()
    {
    return this_spec::create();
    }

CompositeAggregator::Handle CompositeAggregator::create(
            ObjectArray::View vaAggregator)
    {
    size32_t cAggregators = NULL == vaAggregator ? 0 : vaAggregator->length;
    if (cAggregators == 0)
        {
        COH_THROW (IllegalArgumentException::create(
                    "Empty aggregator array"));
        }

    return this_spec::create(vaAggregator);
    }


// ----- constructors -------------------------------------------------------

CompositeAggregator::CompositeAggregator()
    : f_vaAggregator(self())
    {
    }

CompositeAggregator::CompositeAggregator(ObjectArray::View vaAggregator)
    : f_vaAggregator(self(), vaAggregator)
    {
    }


// ----- InvocableMap.EntryAggregator interface -----------------------------

Object::Holder CompositeAggregator::aggregate(Set::View vSetEntries)
    {
    ObjectArray::View   vaAggregator = f_vaAggregator;
    size32_t            cAggregators = vaAggregator->length;
    ObjectArray::Handle haResult     = ObjectArray::create(cAggregators);
    for (size32_t i = 0; i < cAggregators; i++)
        {
        haResult[i] = cast<InvocableMap::EntryAggregator::Handle>(vaAggregator[i])->aggregate(vSetEntries);
        }
    return ReadOnlyArrayList::create(haResult);
    }


// ----- PortableObject interface -------------------------------------------

void CompositeAggregator::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vaAggregator, hIn->readObjectArray(0));
    }

void CompositeAggregator:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObjectArray(0, getAggregators());
    }


// ----- Object interface ---------------------------------------------------

bool CompositeAggregator::equals(Object::View v) const
    {
    CompositeAggregator::View that = cast<CompositeAggregator::View>(v, false);
    if (that != NULL)
         {
         return this == that
             || f_vaAggregator->equals(that->f_vaAggregator);
         }
     return false;
    }

size32_t CompositeAggregator::hashCode() const
    {
    return getAggregators()->hashCode();
    }

TypedHandle<const String> CompositeAggregator::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << "("
        << getAggregators()
        << ")");
    }


// ----- data members accessor-----------------------------------------------

ObjectArray::View CompositeAggregator::getAggregators() const
    {
    return f_vaAggregator;
    }

COH_CLOSE_NAMESPACE3

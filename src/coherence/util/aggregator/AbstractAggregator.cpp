/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/AbstractAggregator.hpp"

#include "coherence/util/extractor/ChainedExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::util::extractor::ChainedExtractor;
using coherence::util::extractor::ReflectionExtractor;


// ----- constructors -------------------------------------------------------

AbstractAggregator::AbstractAggregator()
        : m_fParallel(false),
          f_vExtractor(self())
    {
    }

AbstractAggregator::AbstractAggregator(ValueExtractor::View vExtractor)
        : m_fParallel(false),
          f_vExtractor(self(), vExtractor)
    {
    COH_ENSURE_PARAM(vExtractor);
    }

AbstractAggregator::AbstractAggregator(String::View vsMethod)
        : m_fParallel(false),
          f_vExtractor(self())
    {
    COH_ENSURE_PARAM(vsMethod);
    initialize(f_vExtractor, vsMethod->indexOf('.') == String::npos ?
        (ValueExtractor::View) ReflectionExtractor::create(vsMethod) :
        (ValueExtractor::View) ChainedExtractor::create(
                ChainedExtractor::createExtractors(vsMethod)));
    }


// ----- AbstractAggregator Interface  --------------------------------------

void AbstractAggregator::init(bool /* fFinal */)
    {
    COH_THROW (UnsupportedOperationException::create(
            "This aggregator cannot be invoked on the client."));
    }

void AbstractAggregator::process(Object::View /* vO */, bool /* fFinal */)
    {
    COH_THROW (UnsupportedOperationException::create(
            "This aggregator cannot be invoked on the client."));
    }

Object::Holder AbstractAggregator::finalizeResult(bool /* fFinal */)
    {
    COH_THROW (UnsupportedOperationException::create(
            "This aggregator cannot be invoked on the client."));
    }


// ----- InvocableMap.EntryAggregator interface -----------------------------

Object::Holder AbstractAggregator::aggregate(Set::View vSetEntries)
    {
    bool fFinal = !m_fParallel;
    init(fFinal);

    ValueExtractor::View vExtractor = getValueExtractor();
    for (Iterator::Handle iter = vSetEntries->iterator(); iter->hasNext(); )
        {
        InvocableMap::Entry::View vEntry =
                cast<InvocableMap::Entry::View>( iter->next());
        process(vEntry->extract(vExtractor), false);
        }
    return finalizeResult(fFinal);
    }

InvocableMap::EntryAggregator::Handle
        AbstractAggregator::getParallelAggregator()
    {
    m_fParallel = true;
    return this;
    }

Object::Holder AbstractAggregator::aggregateResults(
            Collection::View vCollResults)
    {
    init(true);

    for (Iterator::Handle iter = vCollResults->iterator();
                iter->hasNext(); )
        {
        process(iter->next(), true);
        }
    // try
       {
        struct InternalFinally
            {
            InternalFinally(AbstractAggregator::Handle hAgg)
                    : hA(hAgg)
                {
                }

            ~InternalFinally()
                {
                // reset the state to allow the aggregator re-use
                hA->m_fParallel = false;
                }
          AbstractAggregator::Handle hA;
          } finally(this);
        return finalizeResult(true);
        }
    }


// ----- PortableObject interface -------------------------------------------

void AbstractAggregator::readExternal(PofReader::Handle hIn)
    {
    m_fParallel   = hIn->readBoolean(0);
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObject(1)));
    }

void AbstractAggregator:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeBoolean(0, m_fParallel);
    hOut->writeObject(1, f_vExtractor);
    }


// ----- Object interface ---------------------------------------------------

bool AbstractAggregator::equals(Object::View v) const
    {
    AbstractAggregator::View that = cast<AbstractAggregator::View>(v, false);
    if (that != NULL)
         {
         return this == that ||
             (Class::typeEquals(this, that)
              && m_fParallel == that->m_fParallel
              && f_vExtractor->equals(that->f_vExtractor));
         }
     return false;
    }

size32_t AbstractAggregator::hashCode() const
    {
    return Class::getClassName(this)->hashCode()
        ^  getValueExtractor()->hashCode();
    }

TypedHandle<const String> AbstractAggregator::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << "("
        << getValueExtractor()
        << ")");
    }


// ----- data members accessor-----------------------------------------------

ValueExtractor::View AbstractAggregator::getValueExtractor() const
    {
    return f_vExtractor;
    }

COH_CLOSE_NAMESPACE3

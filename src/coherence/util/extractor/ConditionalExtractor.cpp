/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/ConditionalExtractor.hpp"

#include "coherence/util/ConditionalIndex.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(190, ConditionalExtractor);


// ----- constructors -------------------------------------------------------

ConditionalExtractor::ConditionalExtractor()
        : f_vFilter(self()),
          f_vExtractor(self()),
          m_fForwardIndex(false)
    {
    }

ConditionalExtractor::ConditionalExtractor(Filter::View vFilter,
            ValueExtractor::View vExtractor,
            bool fForwardIndex)
        : f_vFilter(self(), vFilter),
          f_vExtractor(self(), vExtractor),
          m_fForwardIndex(fForwardIndex)
    {
  COH_ENSURE_PARAM(vFilter);
  COH_ENSURE_PARAM(vExtractor);
    }


// ----- IndexAwareExtractor methods -----------------------------------------

MapIndex::Handle ConditionalExtractor::createIndex(bool fOrdered,
        Comparator::View vComparator,
        Map::Handle hMapIndex) const
    {
    ValueExtractor::View vExtractor = f_vExtractor;
    MapIndex::Handle     hIndex     = cast<MapIndex::Handle>(hMapIndex->get(vExtractor));

    if (hIndex != NULL)
        {
        if (instanceof<ConditionalIndex::Handle>(hIndex)
            && cast<ConditionalIndex::Handle>(hIndex)->getFilter()->equals(f_vFilter))
            {
            return NULL;
            }
        COH_THROW_STREAM(IllegalArgumentException, "Repetitive addIndex call for " << *this);
        }

    hIndex = ConditionalIndex::create(f_vFilter, vExtractor, fOrdered,
            vComparator, m_fForwardIndex);
    hMapIndex->put(vExtractor, hIndex);
    return hIndex;
    }

MapIndex::Handle ConditionalExtractor::destroyIndex(Map::Handle hMapIndex) const
    {
    return cast<MapIndex::Handle>(hMapIndex->remove(f_vExtractor));
    }


// ----- ValueExtractor interface ---------------------------------------

Object::Holder ConditionalExtractor::extract(Object::Holder ohTarget) const
    {
    COH_THROW_STREAM(UnsupportedOperationException,
        "ConditionalExtractor may not be used as an extractor for " << ohTarget );
    }


// ----- PortableObject interface ---------------------------------------

void ConditionalExtractor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObject(1)));
    }

    /**
    * {@inheritDoc}
    */
void ConditionalExtractor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeObject(1, f_vExtractor);
    }


// ----- Object interface -----------------------------------------------

bool ConditionalExtractor::equals(Object::View v) const
        {
        if (instanceof<ConditionalExtractor::View>(v))
            {
            ConditionalExtractor::View vThat = cast<ConditionalExtractor::View>(v);
            return this->f_vFilter->equals(vThat->f_vFilter) &&
                this->f_vExtractor->equals(vThat->f_vExtractor) &&
                this->m_fForwardIndex == vThat->m_fForwardIndex;
            }

        return false;
        }

TypedHandle<const String> ConditionalExtractor::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << "(Extractor=" << f_vExtractor
        << ", Filter="   << f_vFilter << ")");
    }

size32_t ConditionalExtractor::hashCode() const
    {
    return f_vFilter->hashCode() ^ f_vExtractor->hashCode();
    }

COH_CLOSE_NAMESPACE3

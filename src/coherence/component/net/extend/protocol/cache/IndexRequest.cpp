/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/IndexRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

IndexRequest::IndexRequest()
    : m_fAdd(false),
      f_vComparator(self()),
      f_vExtractor(self()),
      m_fOrdered(false)
    {
    }


// ----- Message interface --------------------------------------------------

int32_t IndexRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void IndexRequest::readExternal(PofReader::Handle hIn)
    {
    NamedCacheRequest::readExternal(hIn);

    setAdd(hIn->readBoolean(1));
    setExtractor(cast<ValueExtractor::View>(hIn->readObject(2)));
    setOrdered(hIn->readBoolean(3));
    setComparator(cast<Comparator::View>(hIn->readObject(4)));
    }

void IndexRequest::writeExternal(PofWriter::Handle hOut) const
    {
    NamedCacheRequest::writeExternal(hOut);

    hOut->writeBoolean(1, isAdd());
    hOut->writeObject(2, getExtractor());
    hOut->writeBoolean(3, isOrdered());
    hOut->writeObject(4, getComparator());
    }


// ----- Describable interface ----------------------------------------------

String::View IndexRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Add="        << isAdd()
        << ", Comparator=" << getComparator()
        << ", Extractor="  << getExtractor()
        << ", Ordered="    << isOrdered());
    }


// ----- internal methods ---------------------------------------------------

void IndexRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

bool IndexRequest::isAdd() const
    {
    return m_fAdd;
    }

void IndexRequest::setAdd(bool fAdd)
    {
    m_fAdd = fAdd;
    }

Comparator::View IndexRequest::getComparator() const
    {
    return f_vComparator;
    }

void IndexRequest::setComparator(Comparator::View vComparator)
    {
    initialize(f_vComparator, vComparator);
    }

ValueExtractor::View IndexRequest::getExtractor() const
    {
    return f_vExtractor;
    }

void IndexRequest::setExtractor(ValueExtractor::View vExtractor)
    {
    initialize(f_vExtractor, vExtractor);
    }

bool IndexRequest::isOrdered() const
    {
    return m_fOrdered;
    }

void IndexRequest::setOrdered(bool fOrdered)
    {
    m_fOrdered = fOrdered;
    }

COH_CLOSE_NAMESPACE6

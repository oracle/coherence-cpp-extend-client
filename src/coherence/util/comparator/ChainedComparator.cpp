/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/comparator/ChainedComparator.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

using coherence::util::comparator::SafeComparator;

COH_REGISTER_PORTABLE_CLASS(40, ChainedComparator);


// ----- constructors -------------------------------------------------------

ChainedComparator::ChainedComparator()
    : f_vaComparator(self())
    {
    }


ChainedComparator::ChainedComparator(ObjectArray::View vaComparator)
    : f_vaComparator(self(), vaComparator)
    {
    }


// ----- Comparator interface -----------------------------------------------

int32_t ChainedComparator::compare(Object::View vO1, Object::View vO2) const
    {
  ObjectArray::View vaComparator = getComparators();
  for (size32_t i = 0, c = vaComparator->length; i < c; i++)
        {
    if (instanceof<Comparator::View>(vaComparator[i]) == false)
      {
      COH_THROW (IllegalStateException::create("element in collection is not comparator"));
      }

        int32_t result = (cast<Comparator::View>(vaComparator[i]))->compare(vO1, vO2);
    if (result != 0)
      {
      return result;
      }
        }
  return 0;
    }


// ----- QueryMap interface -------------------------------------------------

int32_t ChainedComparator::compareEntries(QueryMap::Entry::View vEntry1,
                    QueryMap::Entry::View vEntry2) const
    {
  ObjectArray::View vaComparator = getComparators();
  for (size32_t i = 0, c = vaComparator->length; i < c; i++)
        {
    Comparator::View vComparator = cast<Comparator::View>(vaComparator[i]);
    int32_t result = instanceof<QueryMapComparator::View>(vComparator)
            ? (cast<QueryMapComparator::View>(vComparator))->compareEntries(vEntry1, vEntry2)
            : compare(vEntry1->getValue(), vEntry2->getValue());
    if (result != 0)
            {
            return result;
            }
    }
    return 0;
  }


// ----- EntryAwareComparator interface -------------------------------------

bool ChainedComparator::isKeyComparator() const
    {
    ObjectArray::View vaComparator = getComparators();
  for (size32_t i = 0, c = vaComparator->length; i < c; i++)
        {
        if (SafeComparator::isKeyComparator(cast<Comparator::View>(
                vaComparator[i])) == false)
            {
            return false;
            }
        }
  return true;
    }


// ----- PortableObject interface -------------------------------------------

void ChainedComparator::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vaComparator, cast<ObjectArray::View>(hIn->readObjectArray(0)));
    }

void ChainedComparator:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObjectArray(0, f_vaComparator);
    }


// ----- Object interface ---------------------------------------------------

bool ChainedComparator::equals(Object::View v) const
    {
    return instanceof<ChainedComparator::View>(v) && Object::equals(getComparators(), cast<ChainedComparator::View>(v)->getComparators());
    }


// ----- data members accessor-----------------------------------------------

ObjectArray::View ChainedComparator::getComparators() const
    {
    return f_vaComparator;
    }


// ---- constants -----------------------------------------------------------

ChainedComparator::Handle ChainedComparator::getInstance()
    {
    static FinalHandle<ChainedComparator> hSC(System::common(),
            ChainedComparator::create());
    return hSC;
    }
COH_STATIC_INIT(ChainedComparator::getInstance());

COH_CLOSE_NAMESPACE3

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/comparator/InverseComparator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

COH_REGISTER_PORTABLE_CLASS(42, InverseComparator);


// ----- constructors -------------------------------------------------------

InverseComparator::InverseComparator()
    {
    }

InverseComparator::InverseComparator(Comparator::View vComparator)
    : super(vComparator)
    {
    }


// ----- Comparator interface -----------------------------------------------

int32_t InverseComparator::compare(Object::View vO1, Object::View vO2) const
    {
  return -super::compare(vO1, vO2);
    }


// ----- QueryMap interface -------------------------------------------------

int32_t InverseComparator::compareEntries(QueryMap::Entry::View vEntry1,
                    QueryMap::Entry::View vEntry2) const
    {
  return -super::compareEntries(vEntry1, vEntry2);
  }


// ----- Object interface ---------------------------------------------------

bool InverseComparator::equals(Object::View v) const
    {
    return instanceof<InverseComparator::View>(v) && super::equals(v);
    }


// ---- constants -----------------------------------------------------------

InverseComparator::Handle InverseComparator::getInstance()
    {
    static FinalHandle<InverseComparator> hSC(System::common(),
            InverseComparator::create());
    return hSC;
    }
COH_STATIC_INIT(InverseComparator::getInstance());

COH_CLOSE_NAMESPACE3

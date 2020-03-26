/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/IntegerComparator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

using namespace std;

COH_OPEN_NAMESPACE2(coherence,tests)

COH_REGISTER_PORTABLE_CLASS(1011, IntegerComparator);


// ----- constructors -------------------------------------------------------

IntegerComparator::IntegerComparator()
    {
    }


// ----- Comparator interface ----------------------------------------------

int32_t IntegerComparator::compare(Object::View v1, Object::View v2) const
    {
    return ((cast<Integer32::View>(v2))->getInt32Value() - ((cast<Integer32::View>(v1)))->getInt32Value());
    }

// ----- PortableObject interface -------------------------------------------

void IntegerComparator::readExternal(PofReader::Handle /*hIn*/)
    {
    }

void IntegerComparator::writeExternal(PofWriter::Handle /*hOut*/) const
    {
    }

COH_CLOSE_NAMESPACE2

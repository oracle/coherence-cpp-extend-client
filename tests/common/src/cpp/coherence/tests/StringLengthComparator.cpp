/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/StringLengthComparator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

COH_REGISTER_PORTABLE_CLASS(1502, StringLengthComparator);


// ----- constructors -------------------------------------------------------

StringLengthComparator::StringLengthComparator()
    {
    }


// ----- Comparator interface ----------------------------------------------

int32_t StringLengthComparator::compare(Object::View v1, Object::View v2) const
    {
    return int32_t(cast<String::View>(v1)->length()) -
           int32_t(cast<String::View>(v2)->length());
    }

// ----- PortableObject interface -------------------------------------------

void StringLengthComparator::readExternal(PofReader::Handle /*hIn*/)
    {
    }

void StringLengthComparator::writeExternal(PofWriter::Handle /*hOut*/) const
    {
    }

COH_CLOSE_NAMESPACE2

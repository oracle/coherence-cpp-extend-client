/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/MBeanInvocable.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

COH_REGISTER_PORTABLE_CLASS(1510, MBeanInvocable);

// ----- constructors -------------------------------------------------------

MBeanInvocable::MBeanInvocable()
    {
    }

// ----- PortableObject interface -------------------------------------------

void MBeanInvocable::readExternal(PofReader::Handle /* hIn */)
    {
    }

void MBeanInvocable::writeExternal(PofWriter::Handle /* hOut */) const
    {
    }

COH_CLOSE_NAMESPACE2

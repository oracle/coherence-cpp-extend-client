/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/AbstractClusterProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

// ----- PortableObject interface -------------------------------------------

void AbstractClusterProcessor::readExternal(PofReader::Handle /* hIn */)
    {
    }

void AbstractClusterProcessor::writeExternal(PofWriter::Handle /* hOut */) const
    {
    }

COH_CLOSE_NAMESPACE3

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_CLUSTER_PROCESSOR_HPP
#define COH_ABSTRACT_CLUSTER_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
* Base class for entry processors that may only be executed
* within the cluster.
*
* As of 12.2.1.3 this class is deprecated in favor of AbstractProcessor
* which now includes a default process method implementation.
*
* @deprecated 12.2.1.3
*
* @author wl  2013.07.02
*/
class COH_EXPORT AbstractClusterProcessor
    : public abstract_spec<AbstractClusterProcessor,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_CLUSTER_PROCESSOR_HPP

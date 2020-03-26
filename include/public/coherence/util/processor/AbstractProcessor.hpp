/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_PROCESSOR_HPP
#define COH_ABSTRACT_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Describable.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::util::InvocableMap;
using coherence::util::Map;
using coherence::util::Set;


/**
* An AbstractProcessor is a partial EntryProcessor implementation that
* provides default implementations of the process and processAll methods.
*
* For EntryProcessors which only run within the Coherence cluster
* (most common case), the C++ process and processAll methods can be left
* unimplemented.
*
* @author tb  2008.04.28
*/
class COH_EXPORT AbstractProcessor
    : public abstract_spec<AbstractProcessor,
        extends<Describable>,
        implements<InvocableMap::EntryProcessor> >
    {
    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        *
        * This implementation throws an UnsupportedOperationException.
        *
        * @since 12.2.1.3
        */
        virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

    public:
        /**
        * {@inheritDoc}
        */
        virtual Map::View processAll(Set::View vSetEntries) const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_PROCESSOR_HPP

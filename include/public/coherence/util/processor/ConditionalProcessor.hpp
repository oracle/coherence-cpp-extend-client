/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONDITIONAL_PROCESSOR_HPP
#define COH_CONDITIONAL_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Filter;
using coherence::util::InvocableMap;


/**
* Conditional entry processor represents a processor that is invoked
* conditionally based on the result of an entry evaluation.
*
* @author tb  2008.04.28
*/
class COH_EXPORT ConditionalProcessor
    : public class_spec<ConditionalProcessor,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<ConditionalProcessor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a ConditionalProcessor.
        */
        ConditionalProcessor();

        /**
        * Construct a ConditionalProcessor for the specified filter and the
        * processor.
        *
        * The specified entry processor gets invoked if and only if the
        * filter applied to the InvocableMap entry evaluates to true;
        * otherwise the result of the process invocation will return
        * NULL.
        *
        * @param vFilter     the filter
        * @param vProcessor  the entry processor
        */
        ConditionalProcessor(Filter::View vFilter,
                InvocableMap::EntryProcessor::View vProcessor);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(
                InvocableMap::Entry::Handle hEntry) const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View processAll(Set::View vsetEntries) const;


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


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying filter.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The underlying entry processor.
        */
        FinalView<InvocableMap::EntryProcessor> f_vProcessor;

    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONDITIONAL_PROCESSOR_HPP

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONDITIONAL_REMOVE_HPP
#define COH_CONDITIONAL_REMOVE_HPP

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
* ConditionalRemove is an EntryProcessor that performs an
* InvocableMap::Entry#remove Entry.remove operation if the specified
* condition is satisfied.
*
* @author tb  2008.04.28
*/
class COH_EXPORT ConditionalRemove
    : public class_spec<ConditionalRemove,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<ConditionalRemove>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a ConditionalRemove.
        */
        ConditionalRemove();

        /**
        * Construct a ConditionalRemove processor that removes an
        * InvocableMap entry if and only if the filter applied to the entry
        * evaluates to true.
        * This processor may optionally return the current value as a result
        * of the invocation if it has not been removed (the filter evaluated
        * to false).
        *
        * @param vFilter   the filter to evaluate an entry
        * @param fReturn  specifies whether or not the processor should
        *                 return the current value if it has not been removed
        */
        ConditionalRemove(Filter::View vFilter, bool fReturn = false);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(
                InvocableMap::Entry::Handle hEntry) const;

        /**
        * {@inheritDoc
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
        * Specifies whether or not a return value is required.
        */
        bool m_fReturn;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONDITIONAL_REMOVE_HPP

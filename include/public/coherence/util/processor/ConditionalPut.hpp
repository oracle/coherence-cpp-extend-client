/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONDITIONAL_PUT_HPP
#define COH_CONDITIONAL_PUT_HPP

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
* ConditionalPut is an EntryProcessor that performs an
* InvocableMap::Entry#setValue operation if the specified condition is
* satisfied.
*
* @author tb  2008.04.28
*/
class COH_EXPORT ConditionalPut
    : public class_spec<ConditionalPut,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<ConditionalPut>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a ConditionalPut.
        */
        ConditionalPut();

        /**
        * Construct a ConditionalPut that updates an entry with a new value
        * if and only if the filter applied to the entry evaluates to true.
        * This processor optionally returns the current value as a result of
        * the invocation if it has not been updated (the filter evaluated to
        * false).
        *
        * @param vFilter  the filter to evaluate an entry
        * @param ohValue  a value to update an entry with
        * @param fReturn  specifies whether or not the processor should
        *                 return the current value in case it has not been
        *                 updated
        */
        ConditionalPut(Filter::View vFilter, Object::Holder ohValue,
                bool fReturn = false);


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
        * Specifies the new value to update an entry with.
        */
        FinalHolder<Object> f_hValue;

        /**
        * Specifies whether or not a return value is required.
        */
        bool m_fReturn;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONDITIONAL_PUT_HPP

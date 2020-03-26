/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONDITIONAL_PUT_ALL_HPP
#define COH_CONDITIONAL_PUT_ALL_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Filter;
using coherence::util::InvocableMap;
using coherence::util::Map;


/**
* ConditionalPutAll is an EntryProcessor that performs a
* InvocableMap::Entry#setValue Entry.setValue} operation for multiple entries
* that satisfy the specified condition.
*
* @author tb  2008.04.28
*/
class COH_EXPORT ConditionalPutAll
    : public class_spec<ConditionalPutAll,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<ConditionalPutAll>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a ConditionalPutAll.
        */
        ConditionalPutAll();

        /**
        * Construct a ConditionalPutAll processor that updates an entry with
        * a new value if and only if the filter applied to the entry
        * evaluates to true. The new value is extracted from the specified
        * map based on the entry's key.
        *
        * @param vFilter  the filter to evaluate all supplied entries
        * @param vMap     a map of values to update entries with
        */
        ConditionalPutAll(Filter::View vFilter, Map::View vMap);


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
        * Specifies the map of new values.
        */
        FinalView<Map> f_vMap;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONDITIONAL_PUT_ALL_HPP

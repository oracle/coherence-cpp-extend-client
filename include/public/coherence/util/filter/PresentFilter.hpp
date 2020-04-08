/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRESENT_FILTER_HPP
#define COH_PRESENT_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Filter which returns true for InvocableMap::Entry objects that currently
* exist in a Map.
*
* This Filter is intended to be used solely in combination with a
* ConditionalProcessor and is unnecessary for standard QueryMap operations.
*
* @author djl  2008.04.08
*
* @see InvocableMap::Entry#isPresent()
*/
class COH_EXPORT PresentFilter
    : public class_spec<PresentFilter,
        extends<Object>,
        implements<IndexAwareFilter, PortableObject> >
    {
    friend class factory<PresentFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a PresentFilter.
        */
        PresentFilter();


    // ----- EntryFilter interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluateEntry(Map::Entry::View vEntry) const;


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluate(Object::View v) const;


    // ----- IndexAwareFilter interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t calculateEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Filter::View applyIndex(Map::View vMapIndexes,
                Set::Handle hSetKeys) const;


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

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * An instance of the PresentFilter.
        */
        static PresentFilter::Handle getInstance();
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PRESENT_FILTER_HPP

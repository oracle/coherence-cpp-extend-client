/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_EXTRACTOR_HPP
#define COH_ABSTRACT_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/QueryMap.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/comparator/QueryMapComparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PortableObject;
using coherence::util::comparator::QueryMapComparator;


/**
* Abstract base for ValueExtractor implementations.  It provides common
* functionality that allows any extending extractor to be used as a value
* Comparator.
*
* Starting with Coherence 3.5, when used to extract information that is
* coming from a Map, subclasses have the additional ability to operate
* against the Map::Entry instead of just the value. In other words, like the
* EntryExtractor class, this allows an extractor implementation to
* extract a desired value using all available information on the
* corresponding Map::Entry object and is intended to be used in advanced
* custom scenarios, when application code needs to look at both key and
* value at the same time or can make some very specific assumptions regarding
* to the implementation details of the underlying Entry object. To maintain
* full backwards compatibility, the default behavior remains to extract from
* the Value property of the Map::Entry.
*
* <b>Note:</b> subclasses are responsible for initialization and POF
* serialization of the {@link #m_nTarget} field.
*
* @author djl  2008.03.10
*/
class COH_EXPORT AbstractExtractor
    : public abstract_spec<AbstractExtractor,
        extends<Object>,
        implements<ValueExtractor, QueryMapComparator, PortableObject> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractExtractor();


    // ----- ValueExtractor interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder extract(Object::Holder ohTarget) const;


    // ----- QueryMapComparator interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareEntries(QueryMap::Entry::View vEntry1,
                QueryMap::Entry::View vEntry2) const;


    // ----- Comparator interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compare(Object::View v1, Object::View v2) const;


    // ----- subclassing support --------------------------------------------

    public:
        /**
        * Extract the value from the passed Entry object. The returned value
        * should follow the conventions outlined in the {@link #extract}
        * method. By overriding this method, an extractor implementation is
        * able to extract a desired value using all available information on
        * the corresponding Map::Entry object and is intended to be used in
        * advanced custom scenarios, when application code needs to look at
        * both key and value at the same time or can make some very specific
        * assumptions regarding to the implementation details of the
        * underlying Entry object.
        *
        * @param ohEntry  an Entry object to extract a desired value from
        *
        * @return the extracted value
        *
        * @since Coherence 3.5
        */
        virtual Object::Holder extractFromEntry(
                Map::Entry::Holder ohEntry) const;

        /**
        * Extract the value from the "original value" of the passed Entry
        * object.  This method's conventions are exactly the same as for the
        * {@link #extractFromEntry} method.
        *
        * @param ohEntry  an Entry object whose original value should be used
        *                 to extract the desired value from
        *
        * @return the extracted value or NULL if the original value is not
        *         present
        *
        * @since Coherence 3.6
        */
        virtual Object::Holder extractOriginalFromEntry(
                MapTrigger::Entry::Holder ohEntry) const;

        /**
        * Return the target of the extractor.
        *
        * @return the target of the extractor
        * 
        * @since Coherence 12.2.1
        */
        virtual int32_t getTarget() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Indicates that the {@link #extractFromEntry} operation should use
        * the Entry's value.
        *
        * @since Coherence 3.5
        */
        static const int32_t value = 0;

        /**
        * Indicates that the {@link #extractFromEntry} operation should use
        * the Entry's value.
        *
        * @since Coherence 3.5
        */
        static const int32_t key = 1;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Specifies which part of the entry should be used by the
        * {@link #extractFromEntry} operation. Legal values are {@link #value}
        * (default) or {@link #key}.
        *
        * <b>Note:</b> subclasses are responsible for initialization and POF
        * serialization of this field.
        *
        * @since Coherence 3.5
        */
        int32_t m_nTarget;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_EXTRACTOR_HPP

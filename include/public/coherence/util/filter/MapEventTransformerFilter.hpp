/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_EVENT_TRANSFORMER_FILTER_HPP
#define COH_MAP_EVENT_TRANSFORMER_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapEventTransformer.hpp"
#include "coherence/util/MapTrigger.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* MapEventTransformerFilter is a generic multiplexing wrapper that combines
* two implementations: a Filter (most commonly a MapEventFilter) and
* a MapEventTransformer and is used to register event listeners that allow to
* change the content of a MapEvent.
*
* @see coherence::util::transformer::SemiLiteEventTransformer
*
* @author djl  2008.05.22
*/
class COH_EXPORT MapEventTransformerFilter
    : public class_spec<MapEventTransformerFilter,
        extends<Object>,
        implements<PortableObject, Filter, MapEventTransformer> >
    {
    friend class factory<MapEventTransformerFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        MapEventTransformerFilter();

        /**
        * Construct a MapEventTransformerFilter based on the specified Filter
        * and MapEventTransformer.
        *
        * @param vFilter      the underlying Filter (e.g. MapEventFilter)
        *                     used to evaluate original MapEvent objects
        *                     (optional)
        * @param vTransformer  the underlying MapEventTransformer used to
        *                     transform original MapEvent objects
        */
        MapEventTransformerFilter(Filter::View vFilter,
                MapEventTransformer::View vTransformer);


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluate(Object::View v) const;


    // ----- MapEventTransformer interface ----------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual MapEvent::Handle transform(MapEvent::Handle hEvent) const;


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


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The underlying Filter  to evaluate MapEvents with.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The underlying transformer
        */
        FinalView<MapEventTransformer> f_vTransformer;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_MAP_EVENT_TRANSFORMER_FILTER_HPP

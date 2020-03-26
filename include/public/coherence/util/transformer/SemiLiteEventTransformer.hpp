/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SEMI_LITE_EVENT_TRANSFORMER_HPP
#define COH_SEMI_LITE_EVENT_TRANSFORMER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapEventTransformer.hpp"

COH_OPEN_NAMESPACE3(coherence,util,transformer)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* SemiLiteEventTransformer is a special purpose MapEventTransformer
* implementation that removes an OldValue from the MapEvent object for the
* purpose of reducing the amount of data that has to be sent over the network
* to event consumers.
*
* Usage example:
* <pre>
*    cache->addMapListener(listener, MapEventTransformerFilter::create(NULL,
*        SemiLiteEventTransformer::getInstance(), false);
* </pre>
*
* @author djl  2008.06.17
*/
class COH_EXPORT SemiLiteEventTransformer
    : public class_spec<SemiLiteEventTransformer,
        extends<Object>,
        implements<PortableObject, MapEventTransformer> >
    {
    friend class factory<SemiLiteEventTransformer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        SemiLiteEventTransformer();


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


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The SemiLiteEventTransformer singleton.
        */
        static SemiLiteEventTransformer::Handle getInstance();
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SEMI_LITE_EVENT_TRANSFORMER_HPP

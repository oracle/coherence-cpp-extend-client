/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_MAP_LISTENER_HPP
#define COH_ABSTRACT_MAP_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A base class that simplifies the implementation of a MapListener,
* particularly inner classes that only implement one or two of the
* three event methods.
*
* @author tb 2008.06.16
*/
class COH_EXPORT AbstractMapListener
    : public abstract_spec<AbstractMapListener,
        extends<Object>,
        implements<MapListener> >
    {
    // ----- MapListener interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void entryInserted(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryUpdated(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryDeleted(MapEvent::View vEvent);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_MAP_LISTENER_HPP


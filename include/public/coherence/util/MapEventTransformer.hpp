/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_EVENT_TRANSFORMER_HPP
#define COH_MAP_EVENT_TRANSFORMER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* MapEventTransformer interface is used to allow an event consumer to change
* the content of a MapEvent destined for the corresponding MapListener.
*
* In general, the #transform method is called after the
* original MapEvent is evaluated by a Filter (such as
* coherence::util::filter::MapEventFilter). The values contained by the
* returned  MapEvent object will be the ones given (sent) to the
* corresponding listener. Returning null will prevent the emission of the
* event altogether.
*
* <b>Note:</b> Currently, the MapEventTransformer interface is supported only
* by partitioned caches.
*
* @see coherence::util::filter::MapEventTransformerFilter
*
* @author djl  2008.05.22
*/
class COH_EXPORT MapEventTransformer
    : public interface_spec<MapEventTransformer>
    {
    // ----- MapEventTransformer interface ----------------------------------

    public:
        /**
        * Transform the specified MapEvent. The values contained by the
        * returned MapEvent object will be the ones given (sent) to the
        * corresponding listener.
        *
        * @param hEvent  the original MapEvent object
        *
        * @return modified MapEvent object or NULL to discard the event
        */
        virtual MapEvent::Handle transform(
                MapEvent::Handle hEvent) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_EVENT_TRANSFORMER_HPP

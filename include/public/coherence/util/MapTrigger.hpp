/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_TRIGGER_HPP
#define COH_MAP_TRIGGER_HPP

#include "coherence/lang.ns"

#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* MapTrigger represents a functional agent that allows to validate, reject or
* modify mutating operations against an underlying map. The trigger operates
* on an Entry object that represents a pending mutation that is about to be
* committed to the underlying map. A MapTrigger could be registered with any
* ObservableMap using the MapTriggerListener class:
* <pre>
*   NamedCache::Handle hCache = CacheFactory->getCache(vsCacheName);
*   MapTrigger::View vTrigger =  MyCustomTrigger::create();
*   hCache->addMapListener(MapTriggerListener::create(vTrigger));
* </pre>
* <b>Note:</b> In a clustered environment, MapTrigger registration process
* requires triggers to be pofable and providing a non-default
* implementation of the hashCode() and equals() methods.
* Failure to do so may result in duplicate registration and a redundant entry
* processing by equivalent, but "not equal" MapTrigger objects.
*
* @see coherence::util::filter::FilterTrigger
*
* @author djl  2008.05.22
*/
class COH_EXPORT MapTrigger
    : public interface_spec<MapTrigger>
    {
    // ----- inner interface: Entry -----------------------------------------

    public:
        /**
        * A MapTrigger Entry represents a pending change to an Entry that is
        * about to committed to the underlying Map. The methods inherited
        * from InvocableMap::Entry provide both the pending state and the
        * ability to alter that state. The original state of the Entry can be
        * obtained usingthe #getOriginalValue() and #isOriginalPresent
        * methods.
        */
        class COH_EXPORT Entry
            : public interface_spec<Entry,
                implements<InvocableMap::Entry> >
            {
            // ----- MapTrigger::Entry interface ----------------------------

            public:
                /**
                * Determine the value that existed before the start of the
                * mutating operation that is being evaluated by the trigger.
                *
                * @return the original value corresponding to this Entry; may
                *         be null if the value is null or if the Entry did
                *         not exist in the Map
                */
                virtual Object::View getOriginalValue() const = 0;

                /**
                * Determine whether or not the Entry existed before the start
                * of thenmutating operation that is being evaluated by the
                * trigger.
                *
                * @return true iff this Entry was existent in the containing
                *         Map
                */
                virtual bool isOriginalPresent() const = 0;
            };

    // ----- MapTrigger interface -------------------------------------------

    public:
        /**
        * This method is called before the result of a mutating operation
        * represented by the specified Entry object is committed into the
        * underlying map.
        *
        * An implementation of this method can evaluate the change by
        * analyzing the original and the new value, and can perform any of
        * the following:
        * <ul>
        *   <li> override the requested change by calling Entry#setValue
        *        with a different value;
        *   <li> undo the pending change by resetting the entry value to the
        *        original value obtained from Entry#getOriginalValue;
        *   <li> remove the entry from the underlying map by calling
        *        Entry#remove;
        *   <li> reject the pending change by throwing a RuntimeException,
        *        which will prevent any changes from being committed, and
        *        will result in the exception being thrown from the operation
        *        that attempted to modify the map; or
        *   <li> do nothing, thus allowing the pending change to be committed
        *        to the underlying map.
        * </ul>
        *
        * @param vEntry  a Entry object that represents the pending change to
        *                be committed to the map, as well as the original
        *                state of the Entry
        */
        virtual void process(MapTrigger::Entry::Handle vEntry) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_TRIGGER_HPP

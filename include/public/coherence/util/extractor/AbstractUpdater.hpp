/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_UPDATER_HPP
#define COH_ABSTRACT_UPDATER_HPP

#include "coherence/lang.ns"

#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


/**
* Abstract base for ValueUpdater implementations.
* <p/>
* Starting with Coherence 3.6, when used to update information stored in a Map,
* subclasses have the additional ability to operate against the Map::Entry
* instead of just the value. This allows an updater implementation to update a
* desired value using all available information on the corresponding Map::Entry
* object and is intended to be used in advanced custom scenarios, when
* application code needs to look at both key and value at the same time or can
* make some very specific assumptions regarding to the implementation details of
* the underlying Entry object (e.g. BinaryEntry).
* To maintain full backwards compatibility, the default behavior remains to
* update the Value property of the Map.Entry.
* <p/>
* <b>Note:</b> subclasses are responsible for POF and/or Lite serialization of
* the updater.
*
* @author gg 2009.09.11
* @since Coherence 3.6
*/
class COH_EXPORT AbstractUpdater
    : public abstract_spec<AbstractUpdater,
        extends<Object>,
        implements<ValueUpdater> >
    {
    // ----- ValueUpdater interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void update(Object::Handle hTarget, Object::Holder ohValue) const;


    // ----- subclassing support --------------------------------------------

    public:
        /**
        * Update the state of the passed entry using the passed value.
        * <p/>
        * By overriding this method, an updater implementation is able to update
        * the entry's value using all available information on the corresponding
        * Map::Entry object and is intended to be used in advanced custom scenarios,
        * when application code needs to look at both key and value at the same time
        * or can make some very specific assumptions regarding to the implementation
        * details of the underlying Entry object.
        *
        * @param hEntry   the Entry object whose value is to be updated
        * @param ohValue  the new value to update the entry with;  for intrinsic
        *                 types, the specified value is expected to be a standard
        *                 wrapper type in the same manner that reflection works
        *                 (e.g. an int value would be passed as a Integer32)
        */
        virtual void updateEntry(Map::Entry::Handle hEntry, Object::Holder ohValue) const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_UPDATER_HPP

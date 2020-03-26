/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAPPED_SET_HPP
#define COH_MAPPED_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapKeySet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Set implementation which is backed by a Map.
*/
class COH_EXPORT MappedSet
    : public cloneable_spec<MappedSet,
        extends<MapKeySet> >
    {
    friend class factory<MappedSet>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Return a new MappedSet backed by the supplied Map.
        *
        * @param hMap  the map to use as storage
        */
        MappedSet(Map::Handle hMap);

        /**
        * Return a new MappedSet backed by the supplied Map.
        *
        * @param vMap  the map to use as storage
        */
        MappedSet(Map::View vMap);

        /**
        * Copy constructor.
        */
        MappedSet(const MappedSet& that);


    // ----- Set interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(Collection::View vCol);

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const;

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator();

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle toArray(
                ObjectArray::Handle hoa = NULL) const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAPPED_SET

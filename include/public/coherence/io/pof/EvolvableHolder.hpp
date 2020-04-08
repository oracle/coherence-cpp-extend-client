/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVOLVABLE_HOLDER_HPP
#define COH_EVOLVABLE_HOLDER_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"

#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;
using coherence::util::Map;
using coherence::util::Set;

/**
 * Storage for Evolvable classes.
 *
 * @author as  2013.07.25
 *
 * @since  12.1.3
 */
class COH_EXPORT EvolvableHolder
    : public class_spec<EvolvableHolder,
        extends<Object> >
    {
    friend class factory<EvolvableHolder>;

    // ---- constructors ----------------------------------------------------

    protected:
        EvolvableHolder();

    // ---- EvolvableHolder methods------------------------------------------

    public:
        /**
         * Return Evolvable for the specified type id.
         *
         * @param nTypeId  type identifier
         *
         * @return  Evolvable instance
         */
        virtual Evolvable::Handle get(int32_t nTypeId) const;

        /**
         * Return type identifiers for all the Evolvables within this holder.
         *
         * @return  type identifiers for all the Evolvables within this holder
         */
        virtual Set::View getTypeIds() const;

        /**
         * Return <c>true</c> if this holder is empty.
         *
         * @return  <c>true</c> if this holder is empty, <c>false</c> otherwise
         */
        virtual bool isEmpty() const;

    // ---- data members ----------------------------------------------------

    private:
        /**
         * Map of Evolvable objects keyed by type id (Integer32)
         */
        mutable FinalHandle<Map> f_hEvolvableMap;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_EVOLVABLE_HOLDER_HPP

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVOLVABLE_OBJECT_HPP
#define COH_EVOLVABLE_OBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"

#include "coherence/io/pof/EvolvableHolder.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;

/**
 * Defines an interface that should be implemented by the classes that want to
 * support evolution.
 *
 * @author as  2013.04.24
 *
 * @since  12.2.1
 */
class COH_EXPORT EvolvableObject
    : public interface_spec<EvolvableObject>
    {
    // ----- EvolvableObject interface --------------------------------------

    public:
        /**
         * Return Evolvable holder object for the specified type id.
         *
         * This method should only return Evolvable instance if the specified type
         * id matches its own type id. Otherwise, it should delegate to the parent:
         * <pre>
         *     // assuming type ID of this class is 1234
         *     FinalView<Evolvable> f_vEvolvable;
         *      ...
         *     // in constructor
         *     : f_vEvolvable(self(), SimpleEvolvable::create(IMPL_VERSION))
         *     ...
         *     Evolvable::View getEvolvable(int32_t nTypeId)
         *         {
         *         if (1234 == nTypeId)
         *             {
         *             return f_vEvolvable;
         *             }
         *
         *         return super::getEvolvable(nTypeId);
         *         }
         * </pre>
         *
         * @param nTypeId  type id to get Evolvable instance for
         *
         * @return  Evolvable instance for the specified type id
         */
        virtual Evolvable::View getEvolvable(int32_t nTypeId) const = 0;

        /**
         * Return Evolvable holder object for the specified type id.
         *
         * This method should only return Evolvable instance if the specified type
         * id matches its own type id. Otherwise, it should delegate to the parent:
         * <pre>
         *     // assuming type ID of this class is 1234
         *     FinalView<Evolvable> f_vEvolvable;
         *      ...
         *     // in constructor
         *     : f_vEvolvable(self(), SimpleEvolvable::create(IMPL_VERSION))
         *     ...
         *     Evolvable::Handle getEvolvable(int32_t nTypeId)
         *         {
         *         if (1234 == nTypeId)
         *             {
         *             return f_vEvolvable;
         *             }
         *
         *         return super::getEvolvable(nTypeId);
         *         }
         * </pre>
         *
         * @param nTypeId  type id to get Evolvable instance for
         *
         * @return  Evolvable instance for the specified type id
         */
        virtual Evolvable::Handle getEvolvable(int32_t nTypeId) = 0;

        /**
         * Return EvolvableHolder that can be used to store information
         * about evolvable objects that are not known during deserialization.
         *
         * For example, it is possible to evolve the class hierarchy by adding new
         * classes at any level in the hierarchy. Normally this would cause a problem
         * during deserialization on older clients that don't have new classes at all,
         * but EvolvableHolder allows us to work around that issue and simply store
         * type id to opaque binary value mapping within it.
         *
         * @return  EvolvableHolder instance
         */
        virtual EvolvableHolder::View getEvolvableHolder() const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PORTABLE_TYPE_HPP

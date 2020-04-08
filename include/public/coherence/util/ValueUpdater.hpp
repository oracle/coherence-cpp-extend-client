/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VALUE_UPDATER_HPP
#define COH_VALUE_UPDATER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* ValueUpdater is used to update an object's state.
*
* @author jh  2008.02.26
*/
class COH_EXPORT ValueUpdater
    : public interface_spec<ValueUpdater>
    {
    // ----- ValueUpdater interface ---------------------------------------

    public:
        /**
        * Update the state of the passed target object using the passed
        * value.
        *
        * @param hTarget  the Object to update the state of
        * @param ohValue  the new value to update the state with
        *
        * @throws ClassCastException if this ValueUpdater is incompatible with
        *         the passed target object or the value and the
        *         implementation <b>requires</b> the passed object or the
        *         value to be of a certain type
        * @throws Exception if this ValueUpdater encounters a checked
        *         exception in the course of updating the target object
        * @throws IllegalArgumentException if this ValueUpdater cannot handle
        *         the passed target object or value for any other reason;
        *         an implementor should include a descriptive message
        */
        virtual void update(Object::Handle hTarget,
                Object::Holder ohValue) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_VALUE_UPDATER_HPP

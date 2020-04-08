/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VERSIONABLE_HPP
#define COH_VERSIONABLE_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An interface for versionable data.
*
* @author tb  2008.05.08
*/
class COH_EXPORT Versionable
    : public interface_spec<Versionable>
    {
    // ----- Versionable interface ------------------------------------------

    public:
        /**
        * Get the version indicator for this object. The version indicator
        * should be an immutable object or one treated as an immutable, which
        * is to say that after the version is incremented, the previous version's
        * indicator reference will never be returned again.
        *
        * @return a non-null version value that implements the Comparable
        *         interface
        */
        virtual Comparable::View getVersionIndicator() const = 0;

        /**
        * Update the version to the next logical version indicator.
        *
        * @throws UnsupportedOperationException  if the object is immutable or
        *         if the object does not know how to increment its own version
        *         indicator
        */
        virtual void incrementVersion() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_VERSIONABLE_HPP


/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SET_HPP
#define COH_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A collection that contains no duplicate elements.  As implied by
* its name, this interface models the mathematical <i>set</i> abstraction.
*
* The <tt>Set</tt> interface places additional stipulations on the contracts
* of all constructors of the <tt>Set</tt> implementations
* and on the contracts of the <tt>add</tt> and <tt>equals</tt> methods.
*
* The additional stipulation on constructors is, not surprisingly,
* that all constructors must create a set that contains no duplicate
* elements.
*
* @see Collection
*/
class COH_EXPORT Set
    : public interface_spec<Set,
        implements<Collection> >
    {
    // ----- Collection interface -------------------------------------------

    public:
        /**
        * Add the specified element to this set if it is not already present.
        * If this set already contains the specified element, the call leaves
        * this set unchanged and returns <tt>false</tt>.
        * In combination with the restriction on constructors, this ensures
        * that sets never contain duplicate elements.
        *
        * @param v element to be added to this set.
        *
        * @return <tt>true</tt> iff this set did not already contain
        *         the specified element.
        *
        * @throws coherence::lang::UnsupportedOperationException
        *         if the #add() operation is not supported by this
        *         collection.
        */
        using Collection::add;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Compare the specified object with this set for equality.  Return
        * <tt>true</tt> if the specified object is also a set, the two sets
        * have the same size, and every member of the specified set is
        * contained in this set (or equivalently, every member of this set is
        * contained in the specified set). This definition ensures that the
        * equals() method works properly across different implementations of
        * the Set interface.
        *
        * @param v {@link coherence::lang::Object Object} to be compared
        *          for equality with this set.
        *
        * @return @c true iff the specified Object is equal to this set.
        */
        using Object::equals;

        /**
        * Return the hash code value for this set.  The hash code of a set
        * is defined to be the sum of the hash codes of the elements in
        * the set, where the hashcode of a <tt>NULL</tt> element is defined
        * to be zero. This ensures that <code>s1->equals(s2)</code> implies
        * that <code>s1->hashCode() == s2->hashCode()</code> for any two sets
        * <tt>s1</tt> and <tt>s2</tt>, as required by the general
        * contract of {@link coherence::lang::Object::hashCode()
        * Object::hashCode()}.
        *
        * @return the hash code value for this set.
        */
        using Object::hashCode;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SET_HPP

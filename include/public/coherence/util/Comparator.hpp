/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPARATOR_HPP
#define COH_COMPARATOR_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* The Comparator defines a partial order on the collection of Objects.
*
* Such ordering is required, for example, for tree-based collections like
* HashMap or TreeSet. As there are no "natural" ordering defined for Objects,
* the tree-based collections require some Comparator at creation time, and
* further can contain only Objects that are comparable with that Comparator.
*
* The implementations of the Comparator shall take care to keep "less-than"
* and "equals" relations impiled by them to satisfy the natural rules for
* the corresponding mathematical relations. See the comments for method
* {@link Comparator::compare compare(Object::View v1, Object::View v2)}
* for more details.
*/
class COH_EXPORT Comparator
    : public interface_spec<Comparator>
    {
    // ----- Comparator interface -------------------------------------------

    public:
        /**
        * Compare two {@link coherence::lang::Object Objects}. If both
        * Objects are comparable with this
        * Comparator, return < 0, 0 or > 0 if the first Object is less than,
        * equal to, or greater than the second Object.
        *
        * The general contract for this method is:
        * <ol>
        * <li> If either of two handles are <tt>NULL</tt>, a
        *      coherence::lang::NullPointerException shall be thrown;
        * </li>
        * <li> If either of two Objects are not comparable with this
        *      Comparator, a coherence::lang::IllegalArgumentException
        *      shall be thrown;
        * </li>
        * <li> It shall be consistent with the {@link
        *      coherence::lang::Object::equals() Object::equals()}
        *      relation, i.e. <code>compare(v1, v2) == 0</code> if and only
        *      if <code>Object::equals(v1, v2) == true</code>
        * </li>
        * <li> The subsequent calls to this method shall return the same
        *      value until either of arguments changed</li>
        * </li>
        * <li> It shall be <i>anti-symmetric</i>, i.e.
        *      <code>compare(v1, v2) == -compare(v2, v1)</code>
        * </li>
        * </ol>
        *
        * A typical implementation of Comparator may look as follows:
        * <pre>
        * int32_t MyTypeComparator::compare(Object::View v1,
        *                                   Object::View v2) const
        *     {
        *     // compares instances of class MyType only
        *     if (!v1 || !v2)
        *         {
        *         COH_THROW(NullPointerException());
        *         }
        *     MyType::View vTypedObj1 = cast<MyType::View>(v1);
        *     MyType::View vTypedObj2 = cast<MyType::View>(v2);
        *     if (!vTypedObj1 || !vTypedObj1)
        *         {
        *         COH_THROW(IllegalArgumentException("Instances of MyType expected"));
        *         }
        *     // here we suppose that the state of the MyType is defined by
        *     // single field of integer type
        *     if (vTypedObj1->m_intField == vTypedObj2->m_intField)
        *         {
        *         return 0;
        *         }
        *     else if (vTypedObj1->m_intField < vTypedObj2->m_intField)
        *         {
        *         return -1;
        *         }
        *     else
        *         {
        *         return 1;
        *         }
        *     }
        * </pre>
        */
        virtual int32_t compare(Object::View v1, Object::View v2) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_COMPARATOR_HPP

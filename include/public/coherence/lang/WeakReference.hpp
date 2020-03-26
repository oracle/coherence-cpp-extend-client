/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WEAK_REFERENCE_HPP
#define COH_WEAK_REFERENCE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/abstract_spec.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/Reference.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* WeakReferences allow one Object to safely reference another without blocking
* it from being destroyed. WeakReferences are necessary when building Object
* graphs to avoid leaking the Object graph due to cyclical references.
*
* WeakReferences provide getter methods to safely obtain a normal Handle to the
* weakly referenced Object if it still exists. Once the Object becomes only
* weakly referenced its associated WeakReference will be automatically NULL'd
* out so that the Object may be reclaimed.
*
* Consider a simple case of an Object tree, where parent's reference their
* children, and children reference their parents. With normal reference
* counting handles, the tree would be leaked even once all external references
* to the tree have been dropped. To resolve this issue the tree could be
* constructed such that parents reference their children via normal Handles,
* but children reference their parents via WeakReferences. This way as long as
* the parent is externally referenced its entire sub-tree will be retained.
* Once all external Handles to the parent are dropped the entire tree will be
* reclaimed as only WeakReferences would hold it together.
*
* Similar to MemberHandles, WeakReferences transfer their constness to their
* referenced Object. That is if a View to a WeakReference is held then only the
* View to the referenced Object will be available.
*
* Do to the need to call the get() which returns an Object::View or Handle
* and then perform a cast, WeakReferences are somewhat cumbersome to make use
* of. WeakHandle and WeakView are TypedHandle like wrappers for WeakReference
* which makes using WeakReferences much simpler. In most cases these should be
* used rather than direct use of WeakReferences.
*
* @see WeakHandle
* @see WeakView
*
* @author mf  2008.04.10
*/
class COH_EXPORT WeakReference
    : public abstract_spec<WeakReference,
        extends<Object>,
        implements<Reference> >
    {
    // ----- static methods -------------------------------------------------

    public:
        /*
        * Return the WeakReference associated with the supplied Object.
        *
        * @param oh  the Object to weakly reference.
        *
        * @return the WeakReference associated with the supplied Object.
        */
        static WeakReference::Holder valueOf(Object::Holder oh);


    // ----- WeakReference interface ----------------------------------------

    public:
        /**
        * Return a Holder, containing a View to the referenced Object
        *
        * @return a view to the referenced Object
        */
        virtual Object::Holder get() const = 0;

        /**
        * Return a Holder to the referenced Object.
        *
        * @return a Handle unless the Object has become solely refereed by
        *         Views.
        */
        virtual Object::Holder get() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_WEAK_REFERENCE_HPP

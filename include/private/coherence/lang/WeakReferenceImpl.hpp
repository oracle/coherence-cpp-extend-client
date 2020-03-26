/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WEAK_REFERENCE_IMPL_HPP
#define COH_WEAK_REFERENCE_IMPL_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/MemberHolder.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/WeakReference.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A WeakReference implementation.
*
* This implementation is not meant for stand-alone use. That is simply creating
* a new WeakReferenceImpl which references an Object will not be a true
* WeakReference and will not allow the Object to be destroyed. WeakReferences
* must be obtained by calling WeakReference::valueOf(), or though the
* _attachWeak(), method on the Object itself.
*/
class COH_EXPORT WeakReferenceImpl
    : public class_spec<WeakReferenceImpl,
        extends<WeakReference> >
    {
    friend class factory<WeakReferenceImpl>;

    // ----- constructors / destructor -------------------------------------

    protected:
        /**
        * Create a WeakReference for the specified Object Handle.
        */
        WeakReferenceImpl(Object::Holder oh);


    // ----- Reference interface --------------------------------------------

    public:
        /**
        * @throws UnsupportedOperationException
        */
        virtual void set(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get();


    // ---- WeakReferenceImpl interface -------------------------------------

    public:
        /**
        * Clear this WeakReference such that it will no longer reference any Object.
        *
        * The use of this method is limit to Object, and is not intended
        * for external use.
        */
        virtual void clear();

        /**
        * Change this WeakReference so that it may only return Views to the
        * referenced Object.
        *
        * The use of this method is limit to Object, and is not intended
        * for external use.
        */
        virtual void flip();


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The referenced Object.
        */
        MemberHolder<Object> m_mhObj;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_WEAK_REFERENCE_IMPL_HPP

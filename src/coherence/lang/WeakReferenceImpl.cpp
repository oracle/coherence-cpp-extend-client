/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/lang/WeakReferenceImpl.hpp"

#include "coherence/lang/UnsupportedOperationException.hpp"

#include <iostream>

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors / destrucutor -----------------------------------------

WeakReferenceImpl::WeakReferenceImpl(Object::Holder oh)
    : m_mhObj(self(), oh)
    {
    }


// ----- Reference interface ------------------------------------------------

void WeakReferenceImpl::set(Object::Holder)
    {
    COH_THROW (UnsupportedOperationException::create("WeakReference::set()"));
    }

Object::Holder WeakReferenceImpl::get() const
    {
    // the returned ref cannot share an attachment count with the WeakRef as
    // Object makes assumptions that if refCount == 1 and there is a WeakRef
    // then the object is "garbage"
    Object::Holder oh = m_mhObj; // oh ensures that the referenced object stays live for this call

    // next we create a view which is intentionally not chained to oh or m_mhObj
    return Object::View(get_pointer(oh));
    }

Object::Holder WeakReferenceImpl::get()
    {
    // the returned ref cannot share an attachment count with the WeakRef as
    // Object makes assumptions that if refCount == 1 and there is a WeakRef
    // then the object is "garbage"
    Object::Holder oh = m_mhObj; // oh ensures that the referenced object stays live for this call
    // next we create a handle/view which is intentionally not chained to oh
    // or m_mhObj
    if (is_handle(oh))
        {
        return Object::Handle((Object*) get_pointer(oh));
        }
    else
        {
        return Object::View(get_pointer(oh));
        }
    }

// ----- WeakReferenceImpl interface -------------------------------------------

void WeakReferenceImpl::clear()
    {
    // As part of clearing the WeakRef both the referent will be deleted and thus potentially this WeakRef as well,
    // if there are no other references to the WeakRef. Because of this we must temporarily hold onto the referent
    // via a stack based handle otherwise m_mhObj.operator= will still be running after its own destructor has
    // completed.  By introducing the on-stack temp handle we ensure the recursive deletion is run during the
    // on-stack handle's destruction rather then during the assignment of m_mhObj to NULL
    Object::Holder oh = m_mhObj;
    m_mhObj = NULL;
    }

void WeakReferenceImpl::flip()
    {
    // flip needs to ensure that we temporarily produce two views as the call
    // to clear is triggered when we go from two views to one view
    Object::View   v  = m_mhObj;
    Object::Holder oh = get_pointer(v);
    m_mhObj = get_pointer(oh);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> WeakReferenceImpl::toString() const
    {
    return COH_TO_STRING(m_mhObj);
    }

COH_CLOSE_NAMESPACE2

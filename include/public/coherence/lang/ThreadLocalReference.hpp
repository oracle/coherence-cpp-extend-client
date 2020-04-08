/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_THREAD_LOCAL_REFERENCE_HPP
#define COH_THREAD_LOCAL_REFERENCE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/Reference.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* ThreadLocalReferences provide a means to store a Handle to an Object in
* thread local storage. A single native thread-local is used to manage
* all ThreadLocalReferences, which means that users are free to allocate
* any number of ThreadLocalReferences.
*
* The memory associated with a thread's ThreadLocals is automatically freed
* when then thread terminates. The memory associated with a non-static
* ThreadLocalReference will be automatically freed at some point after the
* ThreadLocalReference has been reclaimed, and is done in such a way that
* repeated creation and destruction of ThreadLocalReferences will not leak
* memory.
*/
class COH_EXPORT ThreadLocalReference
    : public class_spec<ThreadLocalReference,
        extends<Object>,
        implements<Reference> >
    {
    friend class factory<ThreadLocalReference>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ThreadLocalReference.
        *
        * @return a handle to the new ThreadLocalReference
        */
        ThreadLocalReference();


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- Reference<Object> interface ------------------------------------

    public:
        /**
        * Specify the Object to be referenced by the calling thread.
        *
        * @param ohObject  the referenced object
        */
        virtual void set(Object::Holder hObject);

        /**
        * Return a Holder to the calling threads referenced Object, or
        * NULL if none has been set for the thread.
        *
        * @return a Holder to the referenced Object
        */
        virtual Object::Holder get() const;

        /**
        * Return a Holder to the calling threads referenced Object, or
        * NULL if none has been set for the thread.
        *
        * @return a Holder to the referenced Object
        */
        virtual Object::Holder get();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_THREAD_LOCAL_REFERENCE_HPP

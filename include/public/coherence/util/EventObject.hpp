/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVENT_OBJECT_HPP
#define COH_EVENT_OBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/util/Describable.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* The root class from which all event state objects shall be derived.
* <p>
* All Events are constructed with a reference to the object, the "source",
* that is logically deemed to be the object upon which the Event in question
* initially occurred upon.
*
* @author js  2008.06.04
*/
class COH_EXPORT EventObject
    : public class_spec<EventObject,
        extends<Describable> >
    {
    friend class factory<EventObject>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a prototypical Event.
        *
        * @param hSource  the object on which the Event initially occurred
        *
        * @throws  IllegalArgumentException  if source is NULL.
        */
        EventObject(Object::Handle hSource);


    // ----- EventObject interface ------------------------------------------

    public:
        /**
        * The object on which the Event initially occurred.
        *
        * @return  The object on which the Event initially occurred
        */
        virtual Object::Handle getSource() const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The object on which the Event initially occurred.
        */
        mutable FinalHandle<Object> f_hoSource;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EVENT_OBJECT_HPP


/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVENT_HPP
#define COH_EVENT_HPP

#include "coherence/lang.ns"



COH_OPEN_NAMESPACE2(coherence,util)


/**
* The root class from which all event state classes shall be derived.
* <p>
* All Events are constructed with a reference to the object, the "source",
* that is logically deemed to be the object upon which the Event in question
* initially occurred.
*
* @author jh  2007.12.12
*/
class COH_EXPORT Event
    : public class_spec<Event>
    {
    friend class factory<Event>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new Event instance.
        *
        * @param vSource  the source associated with the new Event
        *
        * @return a new Event
        */
        Event(Object::View vSource);

    private:
        /**
        * Blocked copy constructor.
        */
        Event(const Event&);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the object on which the Event initially occurred.
        *
        * @return the object on which the Event initially occurred
        */
        virtual Object::View getSource() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The object on which the Event initially occurred.
        */
        FinalView<Object> f_vSource;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EVENT_HPP

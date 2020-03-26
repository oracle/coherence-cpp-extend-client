/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LISTENERS_HPP
#define COH_LISTENERS_HPP

#include "coherence/lang.ns"

#include "coherence/util/EventListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Provides a simple, efficient, and thread-safe implementation of a list
* of event listeners.
*
* The implementation is optimized based on the assumption that listeners are
* added and removed relatively rarely, and that the list of listeners is
* requested relatively often.
*
* Thread safety is implemented by synchronizing on all methods that modify
* any data member of the class.  Read-only methods are not synchronized.
*
* @author jh  2007.12.13
*/
class COH_EXPORT Listeners
    : public class_spec<Listeners>
    {
    friend class factory<Listeners>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new Listeners instance.
        */
        Listeners();


    // ----- Listeners interface --------------------------------------------

    public:
        /**
        * Add a listener.
        *
        * @param hListener the EventListener to add
        */
        virtual void add(EventListener::Handle hListener);

        /**
        * Remove a listener.
        *
        * @param vListener the EventListener to remove
        */
        virtual void remove(EventListener::View vListener);

        /**
        * Add all listeners from another Listeners object.
        *
        * @param vListeners  the Listeners to add
        */
        virtual void addAll(Listeners::View vListeners);

        /**
        * Remove all listeners.
        */
        virtual void removeAll();

        /**
        * Check if there are no listeners.
        *
        * @return true if there are no listeners
        */
        virtual bool isEmpty() const;

        /**
        * Check if a listener is in the list of listeners.
        *
        * @param hListener the EventListener to search for
        *
        * @return true if the listener is in the list of listeners
        */
        virtual bool contains(EventListener::Handle hListener) const;

        /**
        * Get the list of listeners.
        *
        * @return a the list of listeners
        */
        virtual ObjectArray::View listeners() const;


    // ----- helper methods -------------------------------------------------

    private:
        /**
        * Return an array containing the union of the the specified lists.
        *
        * @param haListener1 array of listeners
        * @param haListener2 array of listeners
        *
        * @return the union of the two arrays of listeners
        */
        ObjectArray::Handle unionArrays(ObjectArray::Handle haListener1, ObjectArray::Handle haListener2);

        /**
        * Returns the array of listeners (sync or async) that corresponds to
        * the specific listener.
        *
        * @param vListener the EventListener to find a array for
        *
        * @return the array of listeners corresponding to the specified listener
        */
        ObjectArray::Handle getListenerListFor(EventListener::View vListener);

        /**
        * Set the array of listeners (sync or async) that corresponds to the
        * specified listener.  For example, if the specified listener is a
        * SynchronousListener, set the synchronous listener array to the specified
        * listener list.
        *
        * @param vListener the listener to set the array for
        * @param hListenerArray the array of listeners
        */
        void setListenerListFor(EventListener::View vListener, ObjectArray::Handle hListenerArray);

    public:
        /**
        * Set the array of filters associated with this listener object.
        *
        * @since Coherence 3.7.1.8
        *
        * @param hFilters  the array of filters
        */
        void setFilters(ObjectArray::View vFilters);

        /**
        * Return the array of filters associated with this listener object.
        *
        * @since Coherence 3.7.1.8
        *
        * @return  the array of filters
        */
        ObjectArray::View getFilters() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The registered asynchronous listeners.
        */
        MemberHandle<ObjectArray> m_haAsyncListeners;

        /**
        * The registered synchronous listeners.
        */
        MemberHandle<ObjectArray> m_haSyncListeners;

        /**
        * An optional array of filters associated with this Listeners object.
        * @since Coherence 3.7.1.8
        */
        MemberView<ObjectArray> m_vaFilters;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LISTENERS_HPP

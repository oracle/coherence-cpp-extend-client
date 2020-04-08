/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_QUEUE_PROCESSOR_HPP
#define COH_QUEUE_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Queue.hpp"

#include "private/coherence/component/util/Daemon.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::util::Queue;


/**
* This is a Daemon component that waits for items to process from a Queue.
* Whenever the Queue contains an item, the onNotify event occurs. It is
* expected that sub-classes will process onNotify as follows:
* <code>
* Object o;
* while ((o = getQueue().removeNoWait()) != null)
* { // process the item
*   // ..
* }
* </code>
* <p>
* The Queue is used as the synchronization point for the daemon.
*
* @author nsa 01.18.08
*/
class COH_EXPORT QueueProcessor
    : public class_spec<QueueProcessor,
        extends<Daemon> >
    {
    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new QueueProcessor.
        */
        QueueProcessor();

    private:
        /**
        * Blocked copy constructor.
        */
        QueueProcessor(const QueueProcessor&);


    // ----- QueueProcessor interface ---------------------------------------

    public:
        /**
        * Return the queue for this QueueProcessor.
        *
        * @return the queue for this QueueProcessor
        */
        virtual Queue::Handle getQueue();

    protected:
        /**
        * Instantiate a new queue instance.
        *
        * @return a new queue instance
        */
        virtual Queue::Handle instantiateQueue() const;

    private:
        /**
        * Set the Queue.
        */
        virtual void setQueue(Queue::Handle hQueue);


    // ----- Daemon interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isNotification() const;


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- data members ---------------------------------------------------

    protected:

        /**
        * The queue for this QueueProcessor
        */
        FinalHandle<Queue> f_hQueue;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_QUEUE_PROCESSOR_HPP

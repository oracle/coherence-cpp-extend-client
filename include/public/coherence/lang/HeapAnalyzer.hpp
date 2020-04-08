/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_HEAP_ANALYZER_HPP
#define COH_HEAP_ANALYZER_HPP

#include "coherence/lang/interface_spec.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* HeapAnalyzer provides a base diagnostics interface for tracking heap usage.
*
* There is at most one HeapAnalyzer registered with the system for the
* lifetime of the process. The HeapAnalyzer implementation may be specified
* via the "coherence.heap.analyzer" system property. The property
* can be set to one of the following values:
* <ul>
*  <li>none      No heap analysis will be performed.</li>
*  <li>object    The coherence::lang::ObjectCountHeapAnalyzer will be used.</li>
*  <li>class     The coherence::lang::ClassBasedHeapAnalyzer will be used.</li>
*  <li>alloc     The coherence::lang::ClassBasedHeapAnalyzer will be used,
*                in allocation analysis mode.</li>
*  <li>[custom]  The name of a class registered with the SystemClassLoader.</li>
* </ul>
*
* In the case where a custom class is specified, it must implement this
* interface.  The custom analyzer will be initialized as soon as the class
* is registered with the SystemClassLoader. As static initialization order
* cannot be guaranteed, this custom analyzer will not be notified of managed
* objects created earlier in the static initialization order.
*
* The active analyzer may be obtained from the System::getHeapAnalyzer()
* method.
*
* The HeapAnalyzer and Snapshot interfaces are intentionally narrow.
* Implementations are expected to provide useful information via the toString
* method, as well as by possibly augmenting the interfaces. The minimal
* interface is sufficient for detecting memory leaks.
*
* HeapAnalyzer::Snapshot::View vSnap = hAnalyzer->capture();
*   ...
*   ...
* std::cout << "Heap changed by: " << hAnalyzer->delta(vSnap) << std::endl;
*
* @see ObjectCountHeapAnalyzer
* @see ClassBasedHeapAnalyzer
*
* @author mf  2008.04.27
*/
class COH_EXPORT HeapAnalyzer
    : public interface_spec<HeapAnalyzer>
    {
    // ----- nested interface: Snapshot -------------------------------------

    public:
        /**
        * Snapshot provides a abstract mechanism for comparing successive
        * heap analysis points.
        */
        class COH_EXPORT Snapshot
            : public interface_spec<Snapshot>
            {
            // ----- Snapshot interface ---------------------------------

            public:
                /**
                * Return the number of registered objects reflected by this
                * snapshot.
                *
                * @return the number of registered objects
                */
                virtual int64_t getObjectCount() const = 0;

                /**
                * Return the result of "subtracting" the supplied Snapshot
                * from this Snapshot.
                *
                * @param vThat  the snapshot to compare against
                *
                * @return the delta between two snapshots
                */
                virtual Snapshot::View delta(Snapshot::View vThat) const = 0;
            };


    // ----- HeapAnalyzer interface -----------------------------------------

    public:
        /**
        * Capture a Snapshot of the current state of the heap.
        *
        * Note, when performing captures in a loop, and assigning the captured
        * snapshot to a handle referencing a snapshot, it is advisable to
        * NULL out the handle first, so as to avoid the new snapshot including
        * the "cost" of snapshot it is about to replace.
        *
        * @return a Snapshot of the current state of the heap.
        */
        virtual Snapshot::View capture() const = 0;

        /**
        * Compute the delta between the supplied Snapshot and the current heap
        * state.
        *
        * @param vThat  the snapshot to compare against.
        *
        * @return a snapshot containing the delta
        */
        virtual Snapshot::View delta(Snapshot::View vThat) const = 0;

        /**
        * Return the number of registered objects.
        *
        * @return the number of registered objects
        */
        virtual int64_t getObjectCount() const = 0;

        /**
         * Return the number of objects which have been marked as uncollectable.
         *
         * Return the number of objects which have been marked as uncollectable.
         */
        virtual int64_t getImmortalCount() const = 0;

    protected:
        /**
        * Register a newly created Object with the system.
        *
        * This method is called automatically by coherence::lang::Object once
        * the Object has finished construction.
        *
        * @param o  the newly created Object.
        */
        virtual void registerObject(const Object& o) = 0;

        /**
        * Unregister an Object with the system.
        *
        * This method is called automatically by coherence::lang::Object
        * just prior to the deletion of the Object.  No new handles or views
        * may be created to the object.
        *
        * @param o  the Object to unregister
        */
        virtual void unregisterObject(const Object& o) = 0;

        /**
         * Invoked when an object is deemed to immortal and can never be collected.
         *
         * Note the specified object will have already been registered via registerObject.
         */
        virtual void registerImmortal(const Object& o) = 0;


    // ----- static helper methods ------------------------------------------

    public:
        /**
        * Ensure that the delta between the current heap and the supplied
        * snapshot is as expected.
        *
        * This method can be used to perform quick memory leak assertions.
        *
        * @code
        * HeapAnalyzer::Snapshot::View vSnapStart = HeapAnalyzer::ensureHeap();
        *   ...
        *   ...
        * HeapAnalyzer::ensureHeap(vSnapStart);
        * @endcode
        *
        * @param vSnap   the snapshot to ensure; or NULL for return only
        * @param cDelta  the allowable change in the heap's object count
        *
        * @return a new Snapshot
        *
        * @throws IllegalStateException if the delta does not contain the
        *         expected amount. The text of the exception will include the
        *         output of the Snapshots toString() method.
        */
        static Snapshot::View ensureHeap(Snapshot::View vSnap = NULL,
                int64_t cDelta = 0);


    // ----- inner class: Block ---------------------------------------------

    public:
        /**
         * The HeapAnalyzer::Block allows for easily verifying that a block
         * of code does not leak memory.
         *
         * @code
         * COH_ENSURE_HEAP
         *    {
         *    ... // your code here
         *    }
         * @endcode
         */
        class Block
            {
            // ----- constructors ---------------------------------------

            public:
                /**
                * Construct a ZeroBlock object.
                *
                * This will automatically capture an initial snapshot
                */
                Block()
                    : m_vSnap(ensureHeap())
                    {
                    }

                /**
                * Copy constructor for COH_ENSURE_HEAP macro.
                */
                Block(const Block& that)
                    : m_vSnap(that.m_vSnap)
                    {
                    that.m_vSnap = NULL;
                    }

                /**
                * Destroy a Block object.
                *
                * This will test that no memory has been leaked
                */
                ~Block() COH_NOEXCEPT(false)
                    {
                    ensureHeap(m_vSnap);
                    }


            // ----- operators ------------------------------------------

            public:
                /*
                * Boolean conversion for use in COH_ENSURE_HEAP macro.
                *
                * @return false if snapshot is held, true otherwise
                */
                operator bool() const
                    {
                    return m_vSnap == NULL;
                    }

            private:
                /**
                * Blocked assignment operator.
                */
                const Block& operator=(const Block&);

                /**
                * Blocked dynamic allocation.
                */
                static void* operator new(size_t);


            // ----- data members ---------------------------------------

            protected:
                mutable Snapshot::View m_vSnap; // on stack
            };

    // ----- friends --------------------------------------------------------

    friend class Object;
    };

COH_CLOSE_NAMESPACE2


/**
* Macro for making more readable HeapAnalyzer::Block code blocks See the
* documentation of Block for a usage example.
*
* @see coherence::lang::HeapAnalyzer::Block
*/
#define COH_ENSURE_HEAP \
    if (coherence::lang::HeapAnalyzer::Block COH_UNIQUE_IDENTIFIER(_coh_heap_) \
        = coherence::lang::HeapAnalyzer::Block()) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else


#endif // COH_HEAP_ANALYZER_HPP

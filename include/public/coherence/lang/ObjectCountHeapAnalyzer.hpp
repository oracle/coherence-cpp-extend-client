/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OBJECT_COUNT_HEAP_ANALYZER_HPP
#define COH_OBJECT_COUNT_HEAP_ANALYZER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/HeapAnalyzer.hpp"
#include "coherence/native/NativeAtomic64.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeAtomic64;

/**
* ObjectCountHeapAnalyzer provides simple heap analysis based solely on the
* count of the number of live objects in the system.
*
* This heap analyzer has low CPU and memory costs. It is well suited for
* performance sensitive production environments.
*
* @see ClassBasedHeapAnalyzer for more detailed heap analysis
*
* @author mf  2008.04.27
*/
class COH_EXPORT ObjectCountHeapAnalyzer
    : public class_spec<ObjectCountHeapAnalyzer,
        extends<Object>,
        implements<HeapAnalyzer> >
    {
    friend class factory<ObjectCountHeapAnalyzer>;

    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new ObjectCountHeapAnalyzer.
        *
        * @return the analyzer
        */
        ObjectCountHeapAnalyzer();


    // ----- nested class: Snapshot -----------------------------------------

    public:
        /**
        * Snapshot containing the object count.
        */
        class COH_EXPORT Snapshot
            : public class_spec<Snapshot,
                extends<Object>,
                implements<HeapAnalyzer::Snapshot> >
            {
            friend class factory<Snapshot>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new Snapshot.
                *
                * @param cObjects  the object count
                *
                * @return the new snapshot
                */
                Snapshot(int64_t cObjects);

            // ----- Snapshot interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual int64_t getObjectCount() const;

                /**
                * {@inheritDoc}
                */
                virtual HeapAnalyzer::Snapshot::View delta(
                        HeapAnalyzer::Snapshot::View vThat) const;

            // ----- Object interface: ----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The object count.
                */
                int64_t m_cObjects;
            };


    // ----- HeapAnalyzer interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual HeapAnalyzer::Snapshot::View capture() const;

        /**
        * {@inheritDoc}
        */
        virtual HeapAnalyzer::Snapshot::View delta(
                HeapAnalyzer::Snapshot::View vSnap) const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getObjectCount() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getImmortalCount() const;

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void registerObject(const Object& o);

        /**
        * {@inheritDoc}
        */
        virtual void unregisterObject(const Object& o);

        /**
        * {@inheritDoc}
        */
        virtual void registerImmortal(const Object& o);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The size of the array of counters.
        */
        static const size_t s_cCount = 257;

        /**
        * Array of object counts.
        */
        NativeAtomic64 m_acObjects[s_cCount];

        /**
         * The immortal object count.
         */
        NativeAtomic64 m_cImmortals;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OBJECT_COUNT_HEAP_ANALYZER_HPP

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_BASED_HEAP_ANALYZER_HPP
#define COH_CLASS_BASED_HEAP_ANALYZER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/AbstractHeapAnalyzer.hpp"
#include "coherence/lang/Comparable.hpp"
#include "coherence/lang/FinalView.hpp"

#include "coherence/native/NativeAtomic64.hpp"



COH_OPEN_NAMESPACE2(coherence,util)
class Map;
COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::Map;
using coherence::native::NativeAtomic64;

/**
* ClassBasedHeapAnalyzer provides heap analysis at the class level, that is
* it tracks the number of live instances of each class.
*
* The memory consumption of this heap analyzer is relative to the number of
* classes used within the process. The CPU consumption is also very low, each
* registration consists of roughly four compare-and-set operations. It is well
* suited for development as well as many production environments.
*
* @see ObjectCountHeapAnalyzer for a lower overhead analyzer
*
* @author mf  2008.04.27
*/
class COH_EXPORT ClassBasedHeapAnalyzer
    : public class_spec<ClassBasedHeapAnalyzer,
        extends<AbstractHeapAnalyzer> >
    {
    friend class factory<ClassBasedHeapAnalyzer>;

    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new ClassBasedHeapAnalyzer.
        *
        * @param fShowAllocations  true if allocations should also be shown
        */
        ClassBasedHeapAnalyzer(bool fShowAllocations = false);


    // ----- nested class: ClassStats ---------------------------------------

    public:
        /**
        * Statistics relating to a class.
        */
        class COH_EXPORT ClassStats
            : public class_spec<ClassStats,
                extends<Object>,
                implements<Comparable> >
            {
            friend class factory<ClassStats>;

            protected:
                /**
                * Create a new ClassStats.
                *
                * @param cObjs   the instance count
                * @param cBytes  the byte count
                * @param cAlloc  the allocation count
                * @return the new ClassStats
                */
                ClassStats(int64_t cObjs, int64_t cBytes, int64_t cAlloc);

            // ----- ClassStats interface ---------------------------------

            public:
                /**
                * Return the instance count for the class.
                */
                virtual int64_t getObjectCount() const;

                /**
                * Return the byte count for the class.
                */
                virtual int64_t getByteCount() const;

                /**
                * Return the allocation count for the class.
                */
                virtual int64_t getAllocationCount() const;

            // ----- Comparable interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                int32_t compareTo(Object::View v) const;

            // ----- Object interface -------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- data members -----------------------------------------

            protected:
                /**
                * The number of instances of the class.
                */
                int64_t m_cInstanceCount;

                /**
                * The byte size of all the instances.
                */
                int64_t m_cByteCount;

                /**
                * The total number of allocations of the class.
                */
                int64_t m_cAllocationCount;
            };

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
                */
                Snapshot(TypedHandle<const Map> vMap, bool fShowAllocations);

            // ----- Snapshot interface ---------------------------------

            public:
                /**
                * Return the Snapshots map of class names to ClassStats.
                *
                * The keys are class names, the values are ClassStats.
                *
                * @return the snapshots map.
                */
                virtual TypedHandle<const Map> getStatsMap() const;

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
                * The map of class names to ClassStats.
                */
                FinalView<Object> f_vMapStats;

                /**
                * True if allocations are to be shown.
                */
                bool m_fShowAllocations;
            };


    // ----- AbstractHeapAnalyzer interface ---------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void safeRegisterObject(const Object& o);

        /**
        * {@inheritDoc}
        */
        virtual void safeUnregisterObject(const Object& o);


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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * True if allocations should be printed as part of analysis.
        */
        bool m_fShowAllocations;

        /**
         * The number of immortal objects.
         */
        NativeAtomic64 m_cImmortals;


    // ----- friends --------------------------------------------------------

    friend class Snapshot;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CLASS_BASED_HEAP_ANALYZER_HPP

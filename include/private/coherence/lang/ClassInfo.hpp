/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_INFO_HPP
#define COH_CLASS_INFO_HPP

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic64.hpp"


#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,util)
class Map;
COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::Map;


/**
* ClassInfo is a cache of class related information.
*
* This class exists as a consolidation point for components which need to
* cheaply track class specific information. It is not intended to be made
* public, as it will be a bit of a dumping ground, and for performance reasons
* may violate some of our style rules.
*
* @author mf  2008.04.30
*/
class COH_EXPORT ClassInfo
    : public class_spec<ClassInfo>
    {
    friend class factory<ClassInfo>;

    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Find the ClassInfo for a class based on its type.
        *
        * @param info      the type
        * @param fEnsure   if true the ClassInfo object will be created if
        *                  it does not already exist; if false no new objects
        *                  will be created (needed for ClassBasedHeapAnalyzer)
        *
        * @return the ClassInfo for the specified type, or NULL if the library
        *         is in the process of being unloaded
        */
        static ClassInfo* findByType(const std::type_info& info,
                bool fEnsure = true);

        /**
        * Find the ClassInfo for a class based on its name
        *
        * @param vsName  the class name
        *
        * @return the ClassInfo for the specified name, or NULL if the library
        *         is in the process of being unloaded
        */
        static ClassInfo::Handle findByName(String::View vsName);


    protected:
        /**
        * Create a new ClassInfo.
        *
        * @param vsName  the class name
        *
        * @return a new ClassInfo
        */
        using this_spec::create;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        ClassInfo(String::View vsName);


    // ----- ClassInfo interface --------------------------------------------

    public:
        /**
        * Return the class name.
        *
        * @return the class name
        */
        String::View getClassName() const;

        /**
        * Return the registered shallow size of the class, or 0 if it has
        * not been registered.
        *
        * @return the shallow byte size of the class.
        */
        size32_t getSize() const;

        /**
        * Register the shallow size of instances of the associated class.
        *
        * @param cb  the shallow size
        *
        * @throws IllegalStateException if the size has already been set.
        */
        void setSize(size32_t cb);


    // ----- counts ---------------------------------------------------------

    public:
        /**
        * Return the number of instances of this class currently known to
        * exist.
        */
        inline int64_t getObjectCount() const
            {
            return m_cInstances.get();
            }

        /**
        * Update the count of instances of this class.
        *
        * @param fIncrement  true to increment, false to decrement.
        */
        inline void adjustObjectCount(bool fIncrement)
            {
            m_cInstances.postAdjust(fIncrement ? 1 : -1, false);
            }

        /**
        * Return estimated memory overhead for all instances of this class.
        */
        inline int64_t getByteCount() const
            {
            return m_cBytes.get();
            }

        /**
        * Update the byte count for the instances of this class.
        *
        * @param cBytes  the number of bytes to adjust the count by
        */
        inline void adjustByteCount(int64_t cBytes)
            {
            m_cBytes.postAdjust(cBytes, false);
            }

        /**
        * Return the number of allocations of this class.
        */
        inline int64_t getAllocationCount() const
            {
            return m_cAllocations.get();
            }

        /**
        * Increment the count of allocations of this class.
        */
        inline void incrementAllocationCount()
            {
            m_cAllocations.postAdjust(1, false);
            }

    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- static helpers -------------------------------------------------

    public:
        /**
        * Return a view to a map of ClassInfo objects, key'd by their class
        * name.
        *
        * @return a map of ClassInfo objects, or NULL if the library is being
        *         unloaded
        */
        static TypedHandle<const Map> getClassInfoMap();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The class's demangled name.
        */
        FinalView<String> f_vsName;

        /**
        * The number of instances of the class.
        *
        * This value is maintained by certain HeapAnalyzer implementations,
        * and is not guaranteed to be maintained at all.
        */
        coherence::native::NativeAtomic64 m_cInstances;

        /**
        * The estimated memory overhead of all allocated instances
        *
        * This value is maintained by certain HeapAnalyzer implementations,
        * and is not guaranteed to be maintained at all.
        */
        coherence::native::NativeAtomic64 m_cBytes;

        /**
        * The number of allocations of the class.
        *
        * This value is maintained by certain HeapAnalyzer implementations,
        * and is not guaranteed to be maintained at all.
        */
        coherence::native::NativeAtomic64 m_cAllocations;

        /**
        * The registered shallow size of instances of this class, or 0 if
        * the size has not been registered.
        */
        size32_t m_cbShallow;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CLASS_INFO_HPP

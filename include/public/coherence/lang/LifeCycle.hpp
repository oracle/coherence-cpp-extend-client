/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LIFE_CYCLE_HPP
#define COH_LIFE_CYCLE_HPP

#include "coherence/lang/compatibility.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* @internal
*
* LifeCycle is a data structure which maintains the reference counts and
* other key pieces of information related to an Object's life.
*
* This class is used as a snapshot of the multiple NativeAtomic32s representing the
* Object's life-cycle. It is therefore safe to perform multiple
* unsynchronized reads/writes against the individual fields. The snapshot
* can then be written back to the Object's NativeAtomic32 state.
*
* The specific layout of the bit-field is non-public and is subject to
* change.
*
* @author mf 2008.01.28
*/
struct LifeCycle
    {
    // ---- constructors ----------------------------------------------------

    /**
    * Construct a LifeCycle object from a raw int32_t, allowing
    * initialization from a NativeAtomic32.
    *
    * @param nState  the 32-bit representation of the state portion of LifeCycle
    * @param nRefs   the 32-bit representation of the refs portion of LifeCycle
    */
    LifeCycle(const int32_t nState = 0, const int32_t nRefs = 0)
        : state(nState), refs(nRefs)
        {
        }

    // ----- constants ------------------------------------------------------

    /**
    * Field identifiers.
    */
    typedef enum
        {
        life_state,               // Object life state
        escape_state,             // Object thread-escape state
        monitor_state,            // Monitor inflation state
        weak_reference_state,     // WeakReference inflation state
        handle_count,             // Handle reference count
        view_count,               // View reference count
        member_lock_state,        // read/write lock state
        member_lock_reader_count  // read/write lock active reader count
        } FieldId;

    /**
    * Monitor/WeakReference state constants.
    */
    typedef enum
        {
        constructing = 0, // pre onInit
        initializing = 1, // inside onInit()
        initialized  = 2, // post onInit()
        destructing  = 3  // in delete
        } LifeStage;

    /**
    * Monitor/WeakReference state constants.
    */
    typedef enum
        {
        flat      = 0, // lazy element does not exist
        inflating = 1, // lazy initialization in progress
        inflated  = 2  // lazy element is usable
        } InflationState;

    /**
    * Member read/write lock state.
    */
    typedef enum
        {
        gate_open    = 0, // read locks may be acquired
        gate_closing = 1, // no new read locks may be acquired
        gate_closed  = 2  // no read locks held, no new read locks may be acquired
        } MemberLockState;

    /**
    * The per-attachment count for escaped handles.
    */
    enum
        {
        escaped_increment = 2
        };

    /**
    * Limits.
    */
    enum
        {
        max_read_locks = 0xFF,   // maximum number of reader threads, before blocking
        max_handles    = 0x7FFF, // maximum number of handles to an Object
        max_views      = 0x7FFF  // maximum number of views to an Object
        };


    // ----- data members ---------------------------------------------------

    // Note this is broken into two 32 bit atomics rather then one 64 bit
    // atomic as performance testing (at least on Solaris x86) has demonstrated
    // that a 64 bit CAS in a 32 bit build is significantly slower then a
    // 32 bit CAS.  As most operations on LifeCycle don't access all parts of
    // the LifeCycle object breaking it up into two 32 bit structs provides
    // better performance.

    union State
        {
        State(const int32_t n = 0) : raw(n) {}

        /**
        * Bit-field representation of state.
        */
        struct
            {
            /**
            * The stage of life the object is in.
            */
            unsigned int nLifeStage : 2;

            /**
            * The thread-escape state of the object.
            */
            unsigned int fEscaped : 1;

            /**
            * The InflationState of the Object's monitor.
            *
            * @see InflationState
            */
            unsigned int nMonitor : 2;

            /**
            * The MemberLockState for the Object.
            *
            * @see MemberLockState
            */
            unsigned int nMemberWriteLockState: 2;

            /**
            * The number of member read locks held.
            *
            * A value of max_read_locks indicates that there are too many
            * readers and a new reader thread must wait even if the lock is
            * currently gate_open.
            */
            unsigned int cMemberReadLock : 8; // if changed update max_read_locks
            } value;

        // ----- operators ------------------------------------------------------

        /**
        * Compare two LifeCycles for equality.
        *
        * @param that  the LifeCycle to compare against
        *
        * @return true if the LifeCycles are equal
        */
        bool operator==(const LifeCycle::State& that)
            {
            return raw == that.raw;
            }

        /**
        * Compare two LifeCycles for equality.
        *
        * @param that  the LifeCycle to compare against
        *
        * @return true if the LifeCycles are not equal
        */
        bool operator!=(const LifeCycle::State& that)
            {
            return raw != that.raw;
            }

        /**
        * Automatic type conversion to int32_t, allowing a LifeCycle::State instance
        * to be stored within a NativeAtomic32.
        *
        * @return the int32_t representation of the LifeCycle::State
        */
        operator int32_t() const
            {
            return raw;
            }

        /**
        * Raw uninterpreted bit field representation of value
        */
        int32_t raw;
        } state;

    union Refs
        {
        Refs(const int32_t n = 0) : raw(n) {}

        /**
        * Bit-field representation of reference counts.
        */
        struct
            {
            /**
            * The InflationState of the Object's WeakReference.
            *
            * @see InflationState
            */
            unsigned int nWeakReference : 2;

            // Note: The maximum ref count limit is fairly low and if this ever becomes an
            // issue we can likely allievate it by having an overlflow space in another
            // int32_t.  When either of these hit their max value we could push a large
            // portion of their current value into the overflow space which would then only
            // need to be evaluated when these hit escaped_increment at which point we could
            // pull a portion of the overflow count back in to the atomic count.  That being
            // said this doesn't appear to be necessary at this point as the reference chaining
            // which doesn't impact these counters should give us quite a bit of headroom.

            /**
            * The number of Handles to the Object.
            *
            * A value of max_handles is considered invalid and is used to
            * detect roll over.
            */
            unsigned int cHandle : 15; // if changed update max_handles

            /**
            * The number of Views to the Object.
            *
            * A value of max_views is considered invalid and is used to
            * detect roll over.
            */
            unsigned int cView : 15; // if changed update max_views
            } value;


        // ----- operators ------------------------------------------------------

        /**
        * Compare two LifeCycles for equality.
        *
        * @param that  the LifeCycle to compare against
        *
        * @return true if the LifeCycles are equal
        */
        bool operator==(const LifeCycle::Refs& that)
            {
            return raw == that.raw;
            }

        /**
        * Compare two LifeCycles for equality.
        *
        * @param that  the LifeCycle to compare against
        *
        * @return true if the LifeCycles are not equal
        */
        bool operator!=(const LifeCycle::Refs& that)
            {
            return raw != that.raw;
            }

        /**
        * Automatic type conversion to int32_t, allowing a LifeCycle::Refs instance
        * to be stored within a NativeAtomic32.
        *
        * @return the int32_t representation of the LifeCycle::Refs
        */
        operator int32_t() const
            {
            return raw;
            }

        /**
        * Raw uninterpreted bit field representation of value
        */
        int32_t raw;
        } refs;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LIFE_CYCLE_HPP

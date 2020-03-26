/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRIVATE_LIFE_CYCLE_HPP
#define COH_PRIVATE_LIFE_CYCLE_HPP

#include "coherence/lang/compatibility.hpp"
#include "coherence/lang/LifeCycle.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)

// Note: These output operators are located in a private header as they are large enough to not have
// inlining be desirable and are not safe for use by customer code because of possible difference
// in ostream implementations

/**
* Output a human-readable description of the LifeCycle to an output
* stream.
*
* @param out     the stream to write the description to
* @param state  the state to write to the stream
*
* @return a reference to the supplied stream
*/
template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const LifeCycle::State& state)
    {
    out << "Monitor=";
    switch (state.value.nMonitor)
        {
        case LifeCycle::flat:
            out << "flat";
            break;

        case LifeCycle::inflating:
            out << "inflating";
            break;

        case LifeCycle::inflated:
            out << "inflated";
            break;

        default:
            out << "invalid(" << state.value.nMonitor << ")";
            break;
        }

    out << ", WriteLockState=";
    switch (state.value.nMemberWriteLockState)
        {
        case LifeCycle::gate_open:
            out << "gate_open";
            break;

        case LifeCycle::gate_closing:
            out << "gate_closing";
            break;

        case LifeCycle::gate_closed:
            out << "gate_closed";
            break;

        default:
            out << "invalid(" << state.value.nMemberWriteLockState << ")";
            break;
        }
    out << ", ReaderLocks=" << state.value.cMemberReadLock;

    out << ", LifeStage=";
    switch (state.value.nLifeStage)
        {
        case LifeCycle::constructing:
            out << "constructing";
            break;

        case LifeCycle::initializing:
            out << "initializing";
            break;

        case LifeCycle::initialized:
            out << "initialized";
            break;

        case LifeCycle::destructing:
            out << "destructing";
            break;

        default:
            out << "invalid(" << state.value.nLifeStage << ")";
            break;
        }

    out << ", Escaped=" << (state.value.fEscaped ? "true" : "false");

    return out;
    }

/**
* Output a human-readable description of the LifeCycle to an output
* stream.
*
* @param out     the stream to write the description to
* @param refs  the state to write to the stream
*
* @return a reference to the supplied stream
*/
template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const LifeCycle::Refs& refs)
    {
    out << "Handles=" << refs.value.cHandle
        << ", Views=" << refs.value.cView
        << ", WeakReference=";
    switch (refs.value.nWeakReference)
        {
        case LifeCycle::flat:
            out << "flat";
            break;

        case LifeCycle::inflating:
            out << "inflating";
            break;

        case LifeCycle::inflated:
            out << "inflated";
            break;

        default:
            out << "invalid(" << refs.value.nWeakReference << ")";
            break;
        }

    return out;
    }

/**
* Output a human-readable description of the LifeCycle to an output
* stream.
*
* @param out     the stream to write the description to
* @param life  the state to write to the stream
*
* @return a reference to the supplied stream
*/
template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const LifeCycle& life)
    {
    out << "{" << life.state << ", " << life.refs << "}";
    return out;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_PRIVATE_LIFE_CYCLE_HPP

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_HEAP_ANALYZER_HPP
#define COH_ABSTRACT_HEAP_ANALYZER_HPP

#include "coherence/lang/compatibility.hpp"


#include "coherence/lang/abstract_spec.hpp"
#include "coherence/lang/HeapAnalyzer.hpp"



COH_OPEN_NAMESPACE2(coherence,native)
class NativeThreadLocal;
COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeThreadLocal;


/**
* AbstractHeapAnalyzer provides a starting point for writing custom heap
* analyzers. Most notably it includes support for detecting analyzer
* re-entrance so that analyzers can avoid endlessly recursing if they create
* objects as part of the registration process.
*
* @author mf  2008.04.27
*/
class COH_EXPORT AbstractHeapAnalyzer
    : public abstract_spec<AbstractHeapAnalyzer,
        extends<Object>,
        implements<HeapAnalyzer> >
    {
    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractHeapAnalyzer();

        /**
        * @internal
        */
        virtual ~AbstractHeapAnalyzer();


    // ----- AbstractHeapAnalyzer methods -----------------------------------

    protected:
        /**
        * Registers an object with the heap analyzer.
        *
        * @param o  the object to register
        */
        virtual void safeRegisterObject(const Object& o) = 0;

        /**
        * Unregisters an object with the heap analyzer.
        *
        * @param o  the object to unregister
        */
        virtual void safeUnregisterObject(const Object& o) = 0;


    // ----- HeapAnalyzer interface -----------------------------------------

    protected:
        /**
        * {@inheritDoc}
        *
        * @param o  the object to register
        *
        * If the calling thread is reentering this method return immediately
        * otherwise delegates to safeRegisterObject.
        */
        virtual void registerObject(const Object& o);

        /**
        * {@inheritDoc}
        *
        * @param o  the object to unregister
        *
        * If the calling thread is reentering this method return immediately
        * otherwise delegates to safeUnregisterObject.
        */
        virtual void unregisterObject(const Object& o);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Capture a snapshot and output it to the supplied stream.
        *
        * @param out  the stream to output to
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Tracks if the calling thread is within the analyzer.
        *
        * The use of ThreadLocalReference here is not suitable, as it is a
        * managed object, and causes recursion into the HeapAnalyzer.
        */
        NativeThreadLocal* const m_pTLContext;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_HEAP_ANALYZER_HPP

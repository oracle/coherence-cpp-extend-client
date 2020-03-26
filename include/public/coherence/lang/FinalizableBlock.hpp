/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FINALIZABLE_BLOCK_HPP
#define COH_FINALIZABLE_BLOCK_HPP

#include <stddef.h>

#include "coherence/lang/compatibility.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A finalizable block which runs a series of chained Finalizers as part of
* its destruction.
*
* FinalizableBlocks are useful for cases where a series of finalization
* routines need to be run from a nested function calls. For non-nested
* finalization the use of a FinalizableBlock is unnecessary, and a disposable
* or local class should be used.
*/
class COH_EXPORT FinalizableBlock
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a FinalizableBlock object.
        *
        * @param pDelegate  FinalizableBlock to delegate to, or NULL for
        *                   no delegate
        */
        FinalizableBlock(FinalizableBlock* pDelegate = NULL)
            : m_pDelegate(NULL), m_pFinalizerHead(NULL)
            {
            initialize(pDelegate);
            }

        /**
        * Copy constructor.
        *
        * The new block takes over the ownership of the finalization, the
        * supplied block is invalidated, and is not longer usable.
        *
        * @param that  the source block
        *
        * @see COH_FINALIZABLE
        */
        FinalizableBlock(const FinalizableBlock& that)
            : m_pDelegate(that.m_pDelegate), m_pFinalizerHead(that.m_pFinalizerHead)
            {
            // take ownership
            that.m_pDelegate      = const_cast<FinalizableBlock*>(&that); // marker
            that.m_pFinalizerHead = NULL;
            }

        /**
        * Destroy a FinalizableBlock object.
        *
        * In the case of delegating blocks the destructor has no effect,
        * otherwise the finalizer stack is popped, until until all finalizers
        * have been deleted.
        */
        ~FinalizableBlock()
            {
            if (isTerminal())
                {
                // non-delegate lock, perform lock cleanup
                for (Finalizer* pFinalizer = m_pFinalizerHead; NULL != pFinalizer; )
                    {
                    Finalizer* pDelete = pFinalizer;
                    pFinalizer = pFinalizer->m_pNext;
                    delete pDelete;
                    }
                }
            }


    // ----- operators ------------------------------------------------------

    public:
        /*
        * Boolean conversion for use in COH_FINALIZABLE macro.
        *
        * @return false always
        */
        operator bool() const
            {
            return false;
            }

    private:
        /**
        * Blocked assignment operator.
        */
        const FinalizableBlock& operator=(const FinalizableBlock&);

        /**
        * Blocked dynamic allocation.
        */
        static void* operator new(size_t);


    // -------- nested class: Finalizer -------------------------------------

    public:
        /**
        * Interface for custom automatic cleanup operations.
        */
        class COH_EXPORT Finalizer
            {
            // ----- constructor/destructor ----------------------------

            public:
                /**
                * Finalizer constructor.
                */
                Finalizer()
                    : m_pNext(NULL)
                    {
                    }

                /**
                * Finalizer destructor.
                */
                virtual ~Finalizer()
                    {
                    }


            // ----- data members --------------------------------------

            private:
                /**
                * Pointer to the next Finalizer on the stack.
                */
                Finalizer* m_pNext;


            // ----- friends -------------------------------------------

            friend class FinalizableBlock;
            };


    // ----- FinalizableBlock interface -------------------------------------

    public:
        /**
        * Push a custom finalizer onto the FinalizableBlock's
        * finalization stack.
        *
        * If the FinalizableBlock was constructed with a delegate,
        * then the finalizer is pushed on the delegates finalizer stack.
        *
        * The finalizer will then be owned by the FinalizableBlock and will
        * be automatically deleted with the top-most FinalizableBlock is
        * destroyed.
        *
        * @param pFinalizer  finalizer to add to the finalization stack
        */
        void pushFinalizer(Finalizer* pFinalizer)
            {
            if (isTerminal())
                {
                if (pFinalizer == NULL)
                    {
                    coh_throw_illegal_argument("NULL finalizer");
                    }
                pFinalizer->m_pNext = m_pFinalizerHead;
                m_pFinalizerHead    = pFinalizer;
                }
            else
                {
                m_pDelegate->pushFinalizer(pFinalizer);
                }
            }

    protected:
        /**
        * Return true if the FinalizableBlock is not delegating to
        * another FinalizableBlock.
        *
        * @return return true if the FinalizableBlock is not delegating
        *         to another FinalizableBlock
        */
        bool isTerminal() const
            {
            FinalizableBlock* pDelegate = m_pDelegate;
            if (pDelegate == this)
                {
                coh_throw_illegal_state(
                        "attempt to use invalidated FinalizableBlock");
                }
            return NULL == pDelegate;
            }


        /**
        * Initialize a FinalizableBlock object.
        *
        * A block may be re-initialized so long as it has not been assigned a
        * delegate, and has no finalization stack.
        *
        * @param pDelegate  FinalizableBlock to delegate to, or NULL for
        *                   no delegate
        */
        void initialize(FinalizableBlock* pDelegate = NULL)
            {
            if (m_pDelegate != NULL || m_pFinalizerHead != NULL)
                {
                coh_throw_illegal_state("");
                }
            m_pDelegate = pDelegate;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * If non-NULL specified a FinalizableBlock to delegate to.
        */
        mutable FinalizableBlock* m_pDelegate;

        /**
        * The head of the finalizer stack.
        */
        mutable Finalizer* m_pFinalizerHead;
    };

COH_CLOSE_NAMESPACE2

/**
* Macro for making more readable finalizable code blocks See the
* documentation of FinalizableBlock for a usage example.
*
* The macro will create a well known FinalizableBlock instance referenceable
* as "finally".
*
* @see coherence::lang::FinalizableBlock
*/
#define COH_FINALIZABLE() \
    if (coherence::lang::FinalizableBlock finally \
        = coherence::lang::FinalizableBlock()) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else

#endif // COH_FINALIZABLE_BLOCK_HPP

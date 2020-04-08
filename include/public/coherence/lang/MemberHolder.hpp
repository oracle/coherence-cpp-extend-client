/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MEMBER_HOLDER_HPP
#define COH_MEMBER_HOLDER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/SmartMember.hpp"
#include "coherence/lang/SynchronizedMemberReadBlock.hpp"
#include "coherence/lang/SynchronizedMemberWriteBlock.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* MemberHolder is a thread-safe handle implementation which supports
* referencing Objects as either Handles or Views. MemberHolder can safely be
* used in place of View but adds the ability to attempt a safe down cast to a
* Handle. This differs from the C++ const_cast in that the down cast will only
* succeed if the MemberHolder had been assigned from a Handle, an assignment
* from a View results in a MemberHolder whose down cast operation will throw
* a ClassCastException.
*
* MemberHolder is not for general use, instead it is a convenience handle type
* which can be used for container like classes, which need to be able to
* contain both Handles and Views.
*
* Note: In the rare case that a MemberHolder is declared via the mutable
*       keyword, the MemberHolder must be informed of this fact by setting
*       fMutable to true during construction.
*
* @author mf  2008.01.09
*/
template<class T>
class MemberHolder
    : public SmartMember, public ChainedHandleElement
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The type of the values the holder can reference.
        */
        typedef const T ValueType;

        /**
        * The Handle type for the referenced Object.
        */
        typedef typename T::Handle ValueHandle;

        /**
        * The View type for the referenced Object.
        */
        typedef typename T::View ValueView;

        /**
        * The Holder type for the referenced Object.
        */
        typedef typename T::Holder ValueHolder;

        /**
        * Result type for a non-const get operation.
        */
        typedef TypedHolder<T> GetType;


    // -------- constructors ------------------------------------------------

    public:
        /**
        * Construct a new MemberHolder referencing NULL via a handle.
        *
        * @param oGuardian  the object that protects this member
        */
        MemberHolder(const Object& oGuardian)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ false),
              m_po(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            }

        /**
        * Construct a new MemberHolder referencing the specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the Object to reference
        */
        MemberHolder(const Object& oGuardian, const TypedHolder<T>& that)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ false),
              m_po(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            set(that);
            }

        /**
        * Construct a new MemberHolder referencing the specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the Object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        MemberHolder(const Object& oGuardian, const TypedHolder<T>& that,
                bool fMutable)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ false),
              m_po(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            set(that);
            m_nMutability = fMutable ? forever_mutable : safe_immutable;
            }

        /**
        * Destroy the MemberHolder.
        */
        ~MemberHolder()
            {
            try
                {
                m_nMutability = inherited;
                set(NULL);
                }
            catch (const std::exception& e)
                {
                // Exception::View is not a known type within this file
                std::cerr << "Error during ~MemberHolder: " << e.what() << std::endl;
                return; // can't re-throw from within destructor
                }
            }

    protected:
        /**
        * Construct a MemberHolder without any associated guardian.
        *
        * The new MemberHolder is not usable until the guardian is specified.
        */
        MemberHolder()
            : SmartMember(), ChainedHandleElement(/*fView*/ false), m_po(NULL)
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        MemberHolder(const MemberHolder&);


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Assign this MemberHolder to reference the same Object (and in the
        * same manner) as the specified MemberHolder.
        *
        * @param that  the object to reference
        *
        * @return a reference to this MemberHolder
        */
        MemberHolder& operator=(const MemberHolder& that)
            {
            set(that); // assign from snapshot
            return *this;
            }

        /**
        * Assign this MemberHolder to reference the same Object (and in the
        * same manner) as the specified MemberHolder.
        *
        * @param that  the object to reference
        *
        * @return a reference to this MemberHolder
        */
        MemberHolder& operator=(const TypedHolder<T>& that)
            {
            set(that);
            return *this;
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        operator ValueView() const
            {
            if (m_prev == NULL)
                {
                if (m_nMutability == safe_immutable)
                    {
                    // safe_immutable, synchronization not required
                    return ValueView(m_po);
                    }
                else
                    {
                    SynchronizedMemberReadBlock::Guard guard(getGuardian());
                    return ValueView(m_po); // must occur in syncRead scope
                    }
                }
            else
                {
                return TypedHandle<const T>(m_po, *this);
                }
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        template<class PT>
        operator TypedHandle<const PT>() const
            {
            return (ValueView) *this;
            }

        /**
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        */
        template<class PT>
        operator TypedHolder<PT>() const
            {
            return get();
            }

        /**
        * Dereference the MemberHolder.
        *
        * @return a const pointer to the referenced Object
        */
        ValueView operator->() const
            {
            return (ValueView) *this;
            }

        /**
        * Dereference this handle, returning <tt>T&</tt>.
        *
        * @return a raw <tt>T&</tt> reference to the referenced Object
        *
        * @throws NullPointerException if the this handle is @c NULL
        */
        const T& operator*() const
            {
            return *get();
            }

    // ----- SmartMember interface ------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onEscape(bool fEscaped) const
            {
            T* po = m_po;
            if (po)
                {
                if (m_fView)
                    {
                    ((const T*) po)->_attach(fEscaped);
                    }
                else
                    {
                    po->_attach(fEscaped);
                    }
                }

            if (fEscaped)
                {
                performAction(unlink());
                m_prev = m_next = NULL;
                }
            else
                {
                if (po)
                    {
                    if (m_fView)
                        {
                        ((const T*) po)->_detach(/*fEscaped*/ true);
                        }
                    else
                        {
                        po->_detach(/*fEscaped*/ true);
                        }
                    }
                m_prev = m_next = this;
                }

            SmartMember::onEscape(fEscaped);
            }

        /**
        * {@inheritDoc}
        */
        virtual size64_t retained() const
            {
            TypedHolder<T> oh = get();
            return oh == NULL
                    ? 0
                    : oh->sizeOf(/*fDeep*/ true);
            }

    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Set the Holder to reference an Object via a View.
        *
        * @param that   the Object to reference
        * @param pSync  the synch block to delegate to or NULL to create one
        */
        void set(const TypedHolder<T>& that,
                SynchronizedMemberWriteBlock *pSync = NULL)
            {
            if (m_prev == NULL)
                {
                setEscaped(that, pSync);
                }
            else if (m_nMutability >= forever_immutable)
                {
                coh_throw_illegal_state("attempt to set const MemberHolder");
                }
            else
                {
                performAction(link(that));
                m_po    = const_cast<T*>(that.m_cpo);
                m_fView = that.m_fView;
                }
            }

        /**
        * Set the escaped Holder to reference an Object via a View.
        *
        * @param that   the Object to reference
        * @param pSync  the synch block to delegate to or NULL to create one
        */
        void setEscaped(const TypedHolder<T>& that,
                SynchronizedMemberWriteBlock *pSync = NULL)
            {
            const Object& oGuardian = getGuardian();
            T*    pNew              = const_cast<T*>(that.m_cpo);
            bool  fNewView          = that.m_fView;
            T*    pOld;
            bool  fOldView;

            if (pNew != NULL)
                {
                if (fNewView)
                    {
                    ((const T*) pNew)->_attach(/*fEscaped*/ true);
                    }
                else
                    {
                    pNew->_attach(/*fEscaped*/ true);
                    }
                }

            if (pSync == NULL)
                {
                // sync block
                SynchronizedMemberWriteBlock::Guard guard(oGuardian);
                if (m_nMutability >= forever_immutable)
                    {
                    coh_throw_illegal_state("attempt to set const MemberHolder");
                    }
                pOld     = m_po;
                fOldView = m_fView;

                m_po    = pNew;
                m_fView = fNewView;
                }
            else
                {
                // sync block
                SynchronizedMemberWriteBlock syncWrite(oGuardian, pSync);
                if (m_nMutability >= forever_immutable)
                    {
                    coh_throw_illegal_state("attempt to set const MemberHolder");
                    }

                pOld     = m_po;
                fOldView = m_fView;

                m_po    = pNew;
                m_fView = fNewView;
                }

            if (pOld != NULL)
                {
                if (fOldView)
                    {
                    ((const T*) pOld)->_detach(/*fEscaped*/ true);
                    }
                else
                    {
                    pOld->_detach(/*fEscaped*/ true);
                    }
                }
            }

        /**
         * Return a TypedHolder referencing the same Object as this
         * MemberHolder
         *
         * @param pSync  the sync block to delegate to or NULL to create one
         *
         * @return a TypedHolder referencing the same Object as this
         *         MemberHolder
         */
        TypedHolder<T> get(SynchronizedMemberReadBlock* pSync = NULL) const
            {
            if (m_prev == NULL)
                {
                return getEscaped(pSync);
                }
            else if (m_fView)
                {
                return TypedHandle<const T>(m_po, *this);
                }
            else
                {
                return TypedHandle<T>(m_po, *this);
                }
            }

        /**
         * Return a TypedHolder referencing the same Object as this
         * MemberHolder
         *
         * @param pSync  the sync block to delegate to or NULL to create one
         *
         * @return a TypedHolder referencing the same Object as this
         *         MemberHolder
         */
        TypedHolder<T> getEscaped(SynchronizedMemberReadBlock* pSync = NULL) const
            {
            const Object& oGuardian = getGuardian();

            if (pSync != NULL)
                {
                SynchronizedMemberReadBlock syncRead(oGuardian, pSync);
                if (m_fView)
                    {
                    return ValueView(m_po); // must occur in syncRead scope
                    }
                return ValueHandle(m_po); // must occur in syncRead scope
                }
            else if (m_nMutability == safe_immutable)
                {
                // safe_immutable, synchronization not required
                if (m_fView)
                    {
                    return ValueView(m_po);
                    }
                return ValueHandle(m_po);
                }
            else
                {
                SynchronizedMemberReadBlock::Guard guard(oGuardian);
                if (m_fView)
                    {
                    return ValueView(m_po); // must occur in syncRead scope
                    }
                return ValueHandle(m_po); // must occur in syncRead scope
                }
            }

        /**
        * Perform the specified action.
        *
        * @param nAction  the action to perform
        */
        void performAction(Action nAction) const
            {
            T* po = m_po;
            switch (nAction)
                {
                case action_error:
                    coh_throw_illegal_state("corrupted ChainedHandleElement");
                case action_flip:
                    if (NULL != po) ((const T*) po)->_attach(/*fEscaped*/ false);
                    // fall through
                case action_detach:
                    if (NULL != po)
                        {
                        if (m_fView)
                            {
                            ((const T*) po)->_detach(/*fEscaped*/ false);
                            }
                        else
                            {
                            po->_detach(/*fEscaped*/ false);
                            }
                        }
                    // fall through
                case action_none:
                default:
                    break;
                }
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The referenced Object.
        */
        T* m_po;


    // ----- friends --------------------------------------------------------

    /**
    * @internal
    */
    template<class> friend class Array;

    /**
    * @internal
    */
    friend class SynchronizedMemberReadBlock;

    /**
    * @internal
    */
    friend class SynchronizedMemberWriteBlock;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the given MemberHolder to the
* specified stream.
*
* @param out  the stream used to output the description
* @param th   the MemberHolder to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const MemberHolder<T>& th)
    {
    out << (typename T::View) th;
    return out;
    }

/**
* Assign the specified holder to NULL.
*
* @param mh the holder to clear
*/
template<class T> void clear_handle(MemberHolder<T>& mh)
    {
    mh = NULL;
    }

/**
* Return true if the supplied holder equals NULL.
*
* @param mh  the holder to test
*
* @return true iff the supplied holder equals NULL
*/
template<class T>
bool is_null(const MemberHolder<T>& mh)
    {
    return mh == NULL;
    }

/**
* Perform a dynamic cast the pointer associated with the MemberHolder
* to a the specified handle/view type.
*
* @param mh      the MemberHolder from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const MemberHolder<T>& mh, bool fThrow = true)
    {
    return cast<D>((TypedHolder<T>) mh, fThrow);
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param mh  the MemberHolder from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const MemberHolder<T>& mh)
    {
    return instanceof<D>((TypedHolder<T>) mh);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_MEMBER_HOLDER_HPP

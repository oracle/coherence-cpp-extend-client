/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MEMBER_VIEW_HPP
#define COH_MEMBER_VIEW_HPP

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
* MemberView is a thread-safe view intended for use as a data-member within
* Objects.
*
* Note: In the rare case that a MemberView is declared via the mutable
*       keyword, the MemberView must be informed of this fact by setting
*       fMutable to true during construction.
*
* @author mf  2008.01.09
*
* @see MemberHandle
* @see MemberHolder
*/
template<class T>
class MemberView
    : public SmartMember, public ChainedHandleElement
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The type of the values the holder can reference.
        */
        typedef T ValueType;

        /**
        * The View type for the referenced Object.
        */
        typedef typename T::View ValueView;

        /**
        * Result type for a non-const get operation.
        */
        typedef ValueView GetType;


    // -------- constructors ------------------------------------------------

    public:
        /**
        * Construct a new MemberView referencing NULL via a handle.
        *
        * @param oGuardian  the object that protects this member
        */
        MemberView(const Object& oGuardian)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ true),
              m_cpo(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            }

        /**
        * Construct a new MemberView referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        */
        MemberView(const Object& oGuardian, const ValueView& that)
            : SmartMember(oGuardian),  ChainedHandleElement(/*fView*/ true),
              m_cpo(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            set(that);
            }

        /**
        * Construct a new MemberView referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        * @param fMutable   true if the object was declared as mutable
        */
        MemberView(const Object& oGuardian, const ValueView& that, bool fMutable)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ true),
              m_cpo(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            set(that);
            m_nMutability = fMutable ? forever_mutable : safe_immutable;
            }

        /**
        * Destroy the MemberView.
        */
        ~MemberView()
            {
            try
                {
                m_nMutability = inherited;
                set(NULL);
                }
            catch (const std::exception& e)
                {
                // Exception::View is not a known type within this file
                std::cerr << "Error during ~MemberView: " << e.what() << std::endl;
                return; // can't re-throw from within destructor
                }
            }

    private:
        /**
        * Blocked copy constructor.
        */
        MemberView(const MemberView&);


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Assign the MemberView to reference another object.
        *
        * @param that  the object to reference
        *
        * @return a reference to this MemberView
        */
        MemberView& operator=(const ValueView& that)
            {
            set(that);
            return *this;
            }

        /**
        * Assign the MemberView to reference another object.
        *
        * @param that  the object to reference
        *
        * @return a reference to this MemberView
        */
        MemberView& operator=(const MemberView& that)
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
            return get();
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        template<class PT>
        operator TypedHandle<const PT>() const
            {
            return get();
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
        * Dereference the MemberView.
        *
        * @return a const pointer to the referenced Object
        */
        ValueView operator->() const
            {
            return get();
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
            const T* cpo = m_cpo;
            if (cpo)
                {
                cpo->_attach(fEscaped);
                }

            if (fEscaped)
                {
                performAction(unlink());
                m_prev = m_next = NULL;
                }
            else
                {
                if (cpo)
                    {
                    cpo->_detach(/*fEscaped*/ true);
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
            ValueView v = get();
            return v == NULL
                    ? 0
                    : v->sizeOf(/*fDeep*/ true);
            }


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Set the view to reference an Object via a View.
        *
        * @param that   the Object to reference
        * @param pSync  optional external SyncornizedMemberWriteBlock to use
        *               to avoid internal synchronization.
        */
        void set(const TypedHolder<T>& that,
                 SynchronizedMemberWriteBlock* pSync = NULL)
            {
            if (m_prev == NULL)
                {
                setEscaped(that, pSync);
                }
            else if (m_nMutability >= forever_immutable)
                {
                coh_throw_illegal_state("attempt to set const MemberView");
                }
            else
                {
                performAction(link(that));
                m_cpo = get_pointer(that);
                }
            }

        /**
        * Set the escaped view to reference an Object via a View.
        *
        * @param that   the Object to reference
        * @param pSync  optional external SyncornizedMemberWriteBlock to use
        *               to avoid internal synchronization.
        */
        void setEscaped(const TypedHolder<T>& that,
                 SynchronizedMemberWriteBlock* pSync = NULL)
            {
            const Object& oGuardian = getGuardian();
            const T*      cpo       = get_pointer(that);
            const T*      cpDetach  = NULL;
            const Object* cpAttach  = NULL == cpo ? NULL : cpo->_attach(/*fEscaped*/ true);

            if (pSync == NULL)
                {
                // sync block
                SynchronizedMemberWriteBlock::Guard guard(oGuardian);
                if (m_nMutability >= forever_immutable)
                    {
                    coh_throw_illegal_state("attempt to set const MemberView");
                    }

                cpDetach = m_cpo;
                m_cpo    = NULL == cpAttach ? NULL : cpo;
                }
            else
                {
                // sync block
                SynchronizedMemberWriteBlock syncWrite(oGuardian, pSync);
                if (m_nMutability >= forever_immutable)
                    {
                    coh_throw_illegal_state("attempt to set const MemberView");
                    }

                cpDetach = m_cpo;
                m_cpo    = NULL == cpAttach ? NULL : cpo;
                }

            if (cpDetach)
                {
                cpDetach->_detach(/*fEscaped*/ true);
                }

            }

        /**
        * Return a View to the referenced Object.
        *
        * @param pSync  optional external SyncornizedMemberReadBlock to use
        *               to avoid internal synchronization.
        *
        * @return a View to the referenced Object
        */
        ValueView get(SynchronizedMemberReadBlock* pSync = NULL) const
            {
            if (m_prev == NULL)
                {
                return getEscaped(pSync);
                }
            else
                {
                return TypedHandle<const T>(m_cpo, *this);
                }
            }

        /**
        * Return a View to the referenced Object.
        *
        * @param pSync  optional external SyncornizedMemberReadBlock to use
        *               to avoid internal synchronization.
        *
        * @return a View to the referenced Object
        */
        ValueView getEscaped(SynchronizedMemberReadBlock* pSync = NULL) const
            {
            const Object& oGuardian = getGuardian();
            if (pSync != NULL)
                {
                // sync block
                SynchronizedMemberReadBlock syncRead(oGuardian, pSync);
                return ValueView(m_cpo); // must occur within scope of syncRead
                }
            else if (m_nMutability == safe_immutable)
                {
                // safe_immutable, synchronization not required
                return ValueView(m_cpo);
                }
            else
                {
                // sync block
                SynchronizedMemberReadBlock::Guard guard(oGuardian);
                return ValueView(m_cpo); // must occur within scope of guard
                }
            }



        /**
        * Perform the specified action.
        *
        * @param nAction  the action to perform
        */
        void performAction(Action nAction) const
            {
            const T* cpo = m_cpo;
            switch (nAction)
                {
                case action_error:
                case action_flip:
                    coh_throw_illegal_state("corrupted ChainedHandleElement");
                case action_detach:
                    if (cpo) cpo->_detach(/*fEscaped*/ false);
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
        const T* m_cpo;

    // ----- friends --------------------------------------------------------

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
* Output a human-readable description of the given MemberView to the
* specified stream.
*
* @param out  the stream used to output the description
* @param mv   the MemberView to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const MemberView<T>& mv)
    {
    out << (typename T::View) mv;
    return out;
    }

/**
* Assign the specified holder to NULL.
*
* @param mv the MemberView to clear
*/
template<class T> void clear_handle(MemberView<T>& mv)
    {
    mv = NULL;
    }

/**
* Perform a dynamic cast the pointer associated with the MemberView
* to a the specified handle/view type.
*
* @param mv      the MemberView from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const MemberView<T>& mv, bool fThrow = true)
    {
    return cast<D>((typename MemberView<T>::ValueView) mv, fThrow);
    }

/**
* Perform an instanceof check on a MemberView.
*
* @param mv  the MemberView from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const MemberView<T>& mv)
    {
    return NULL != cast<D>(mv, false);
    }

/**
* Return true if the supplied holder equals NULL.
*
* @param mv  the MemberView to test
*
* @return true iff the supplied MemberView equals NULL
*/
template<class T>
bool is_null(const MemberView<T>& mv)
    {
    return mv == NULL;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_MEMBER_VIEW_HPP

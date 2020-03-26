/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FINAL_VIEW_HPP
#define COH_FINAL_VIEW_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/MemberView.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/SynchronizedMemberWriteBlock.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

/**
* FinalView is an immutable thread-safe view intended for use as a data-member
* within Objects.
*
* @author mf  2008.12.01
*
* @see FinalHandle
* @see FinalHolder
*/
template<class T>
class FinalView
    : public MemberView<T>
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


    // -------- constructors ------------------------------------------------

    public:
        /**
        * Construct a new FinalView referencing NULL via a handle.
        *
        * @param oGuardian  the object that protects this member
        */
        FinalView(const Object& oGuardian)
            : MemberView<T>(oGuardian)
            {
            }

        /**
        * Construct a new FinalView referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        */
        FinalView(const Object& oGuardian, const ValueView& that)
            : MemberView<T>(oGuardian, that)
            {
            if (MemberView<T>::m_cpo)
                {
                MemberView<T>::m_nMutability = MemberView<T>::safe_immutable;
                }
            }

        /**
        * Construct a new FinalView referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        FinalView(const Object& oGuardian, const ValueView& that, bool fMutable)
            : MemberView<T>(oGuardian, that, fMutable)
            {
            if (MemberView<T>::m_cpo)
                {
                MemberView<T>::m_nMutability = MemberView<T>::safe_immutable;
                }
            }

    private:
        /**
        * Blocked copy constructor.
        */
        FinalView(const FinalView&);


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        operator ValueView() const
            {
            return getFinal();
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        template<class PT>
        operator TypedHandle<const PT>() const
            {
            return getFinal();
            }

        /**
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        */
        template<class PT>
        operator TypedHolder<PT>() const
            {
            return getFinal();
            }

        /**
        * Dereference the FinalView.
        *
        * @return a const pointer to the referenced Object
        */
        const T* operator->() const
            {
            const T* cpo = MemberView<T>::m_cpo;
            if (MemberView<T>::m_nMutability < MemberView<T>::safe_immutable)
                {
                MemberView<T>::readBarrier();
                if (cpo == NULL)
                    {
                    cpo = MemberView<T>::m_cpo;
                    MemberView<T>::readBarrier();
                    }
                }

            if (NULL == cpo)
                {
                coh_throw_npe(typeid(const T));
                }
            return cpo;
            }

    private:
        /**
        * Blocked assignment operator.
        */
        FinalView& operator=(const ValueView& that);

        /**
        * Blocked assignment operator.
        */
        FinalView& operator=(const FinalView<T>& that);


    // ----- SmartMember interface ------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual size64_t retained() const
            {
            const T* cpo = MemberView<T>::m_cpo;
            if (cpo == NULL &&
                MemberView<T>::m_nMutability < MemberView<T>::safe_immutable)
                {
                MemberView<T>::readBarrier();
                cpo = MemberView<T>::m_cpo;
                }

            return cpo == NULL
                    ? 0
                    : cpo->sizeOf(/*fDeep*/ true);
            }


    // ----- helper methods -------------------------------------------------

    protected:
        /**
         * {@inheritDoc}
         */
        ValueView getFinal() const
            {
            const T* cpo = MemberView<T>::m_cpo;
            if (MemberView<T>::m_prev == NULL)
                {
                if (cpo == NULL &&
                    MemberView<T>::m_nMutability < MemberView<T>::safe_immutable)
                    {
                    MemberView<T>::readBarrier();
                    cpo = MemberView<T>::m_cpo;
                    }

                return ValueView(cpo);
                }
            else
                {
                return TypedHandle<const T>(cpo, *this);
                }
            }

        /**
        * Initialize the value of a FinalView after it is
        * constructed.  A FinalView may be initialized only if the
        * following conditions hold:
        * <ul>
        *   <li> The FinalView has not already been initialized
        *   (either during construction, or via <tt>initialize()</tt>)
        *   <li> The FinalView has not escaped
        * </ul>
        *
        * @param that  the value to initialize this FinalView to
        */
        void initialize(ValueView that)
            {
            if (MemberView<T>::m_nMutability >= MemberView<T>::forever_immutable)
                {
                coh_throw_illegal_state(
                    "attempt to initialize const FinalView");
                }
            else if (MemberView<T>::m_prev == NULL)
                {
                SynchronizedMemberWriteBlock block(MemberView<T>::getGuardian());
                if (MemberView<T>::m_cpo != NULL) // sync'd check
                    {
                    coh_throw_illegal_state(
                        "attempt to multiply initialize FinalView");
                    }
                MemberView<T>::setEscaped(that, &block);
                }
            else if (MemberView<T>::m_cpo != NULL)
                {
                coh_throw_illegal_state(
                    "attempt to multiply initialize FinalView");
                }
            else
                {
                MemberView<T>::m_cpo = get_pointer(that);
                if (MemberView<T>::m_cpo)
                    {
                    MemberView<T>::link(that);
                    MemberView<T>::m_nMutability = MemberView<T>::safe_immutable;
                    }
                }
            }

    // ----- friends --------------------------------------------------------

    /**
    * @internal
    */
    template<class _T, class V> friend void initialize(FinalView<_T>& fh,
                                              V that);
    };

/**
* Perform a dynamic cast the pointer associated with the FinalView
* to a the specified handle/view type.
*
* @param v       the FinalView from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const FinalView<T>& v, bool fThrow = true)
    {
    return cast<D>((typename FinalView<T>::ValueView) v, fThrow);
    }

/**
* Perform an instanceof check on a FinalView.
*
* @param v   the FinalView from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const FinalView<T>& v)
    {
    return NULL != cast<D>(v, false);
    }

/**
* Initialize the value of a FinalView after it is constructed.  A
* FinalView may be initialized only if the following conditions
* hold:
* <ul>
*   <li> The FinalView has not already been initialized to a
*   non-NULL value (either during construction, or via
*   <tt>initialize()</tt>)
* </ul>
*
* @param h     the FinalView to initialize
* @param that  the value to initialize this FinalView to
*/
template<class T, class V>
void initialize(FinalView<T>& h, V that)
    {
    h.initialize(that);
    }


COH_CLOSE_NAMESPACE2

#endif // COH_FINAL_VIEW_HPP

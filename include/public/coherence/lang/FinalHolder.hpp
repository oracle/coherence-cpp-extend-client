/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FINAL_HOLDER_HPP
#define COH_FINAL_HOLDER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/MemberHolder.hpp"
#include "coherence/lang/TypedHolder.hpp"
#include "coherence/lang/SynchronizedMemberWriteBlock.hpp"


COH_OPEN_NAMESPACE2(coherence,lang)

/**
* FinalHolder is an immutable thread-safe holder intended for use as a
* data-member within Objects.
*
* @author mf  2008.12.01
*
* @see FinalHandle
* @see FinalView
*/
template<class T>
class FinalHolder
    : public MemberHolder<T>
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The type of the values the holder can reference.
        */
        typedef T ValueType;

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


    // -------- constructors ------------------------------------------------

    public:
        /**
        * Construct a new FinalHolder referencing NULL via a handle.
        *
        * @param oGuardian  the object that protects this member
        */
        FinalHolder(const Object& oGuardian)
            : MemberHolder<T>(oGuardian)
            {
            }

        /**
        * Construct a new FinalHolder referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        */
        FinalHolder(const Object& oGuardian, const TypedHolder<T>& that)
            : MemberHolder<T>(oGuardian, that)
            {
            if (MemberHolder<T>::m_po)
                {
                MemberHolder<T>::m_nMutability = MemberHolder<T>::safe_immutable;
                }
            }

        /**
        * Construct a new FinalHolder referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        FinalHolder(const Object& oGuardian, const ValueView& that, bool fMutable)
            : MemberHolder<T>(oGuardian, that, fMutable)
            {
            if (MemberHolder<T>::m_cpo)
                {
                MemberHolder<T>::m_nMutability = MemberHolder<T>::safe_immutable;
                }
            }

    private:
        /**
        * Blocked copy constructor.
        */
        FinalHolder(const FinalHolder&);


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Return a Holder to the referenced Object.
        *
        * @return a Holder to the referenced Object
        */
        operator ValueView() const
            {
            return getFinal();
            }

        /**
        * Return a Holder to the referenced Object.
        *
        * @return a Holder to the referenced Object
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
        * Dereference the FinalHolder.
        *
        * @return a const pointer to the referenced Object
        */
        const T* operator->() const
            {
            const T* cpo = MemberHolder<T>::m_po;
            if (MemberHolder<T>::m_nMutability < MemberHolder<T>::safe_immutable)
                {
                MemberHolder<T>::readBarrier();
                if (cpo == NULL)
                    {
                    cpo = MemberHolder<T>::m_po;
                    MemberHolder<T>::readBarrier();
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
        FinalHolder& operator=(const TypedHolder<T>& that);

        /**
        * Blocked assignment operator.
        */
        FinalHolder& operator=(const FinalHolder<T>& that);

    // ----- SmartMember interface ------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual size64_t retained() const
            {
            const T* cpo = MemberHolder<T>::m_po;
            if (MemberHolder<T>::m_nMutability < MemberHolder<T>::safe_immutable)
                {
                MemberHolder<T>::readBarrier();
                cpo = MemberHolder<T>::m_po;
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
        ValueHolder getFinal() const
            {
            T* po = MemberHolder<T>::m_po;
            if (MemberHolder<T>::m_prev == NULL)
                {
                if (po == NULL &&
                    MemberHolder<T>::m_nMutability < MemberHolder<T>::safe_immutable)
                    {
                    MemberHolder<T>::readBarrier();
                    po = MemberHolder<T>::m_po;
                    }

                if (MemberHolder<T>::m_fView)
                    {
                    return ValueView(po);
                    }
                return ValueHandle(po);
                }
            else if (MemberHolder<T>::m_fView)
                {
                return TypedHandle<const T>(po, *this);
                }
            else
                {
                return TypedHandle<T>(po, *this);
                }
            }

        /**
        * Initialize the value of a FinalHolder after it is
        * constructed.  A FinalHolder may be initialized only if the
        * following conditions hold:
        * <ul>
        *   <li> The FinalHolder has not already been initialized
        *   (either during construction, or via <tt>initialize()</tt>)
        *   <li> The FinalHolder has not escaped
        * </ul>
        *
        * @param that  the value to initialize this FinalHolder to
        */
        void initialize(ValueHolder that)
            {
            if (MemberHolder<T>::m_nMutability >= MemberHolder<T>::forever_immutable)
                {
                coh_throw_illegal_state(
                    "attempt to initialize const FinalHolder");
                }
            else if (MemberHolder<T>::m_prev == NULL)
                {
                SynchronizedMemberWriteBlock block(MemberHolder<T>::getGuardian());
                if (MemberHolder<T>::m_po != NULL) // sync'd check
                    {
                    coh_throw_illegal_state(
                        "attempt to multiply initialize FinalHolder");
                    }
                MemberHolder<T>::setEscaped(that, &block);
                }
            else if (MemberHolder<T>::m_po != NULL)
                {
                coh_throw_illegal_state(
                    "attempt to multiply initialize FinalHolder");
                }
            else if (NULL != that)
                {
                TypedHandle<T> hThat = cast<TypedHandle<T> >(that, /*fThrow*/ false);
                if (NULL == hThat) // view
                    {
                    const T* cpo = get_pointer(that);
                    MemberHolder<T>::m_fView = true;
                    MemberHolder<T>::m_po    = const_cast<T*>(
                            NULL == cpo->_attach(/*fEscaped*/ false)
                                ? NULL : cpo);
                    }
                else // handle
                    {
                    T* po = get_pointer(hThat);
                    MemberHolder<T>::m_fView = false;
                    MemberHolder<T>::m_po    =
                        NULL == po->_attach(/*fEscaped*/false) ? NULL : po;
                    }
                MemberHolder<T>::m_nMutability = MemberHolder<T>::safe_immutable;
                }
            }

    // ----- friends --------------------------------------------------------

    /**
    * @internal
    */
    template<class _T, class OH> friend void initialize(FinalHolder<_T>& fh,
                                              OH that);
    };


// ----- non-member operators and functions ---------------------------------

/**
* Perform a dynamic cast the pointer associated with the FinalHolder
* to a the specified handle/view type.
*
* @param h       the FinalHolder from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(FinalHolder<T>& h, bool fThrow = true)
    {
    return cast<D>((TypedHolder<T>) h, fThrow);
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param h  the FinalHolder from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(FinalHolder<T>& h)
    {
    return instanceof<D>((TypedHolder<T>) h);
    }

/**
* Initialize the value of a FinalHolder after it is constructed.  A
* FinalHolder may be initialized only if the following conditions
* hold:
* <ul>
*   <li> The FinalHolder has not already been initialized to a
*   non-NULL value (either during construction, or via
*   <tt>initialize()</tt>)
* </ul>
*
* @param h     the FinalHolder to initialize
* @param that  the value to initialize this FinalHolder to
*/
template<class T, class OH>
void initialize(FinalHolder<T>& h, OH that)
    {
    h.initialize(that);
    }


COH_CLOSE_NAMESPACE2

#endif // COH_FINAL_HOLDER_HPP

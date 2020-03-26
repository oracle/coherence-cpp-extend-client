/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FINAL_HANDLE_HPP
#define COH_FINAL_HANDLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/MemberHandle.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/SynchronizedMemberWriteBlock.hpp"


COH_OPEN_NAMESPACE2(coherence,lang)

/**
* FinalHandle is an immutable thread-safe handle used by an Object to
* reference its non-const child Objects.
*
* FinalHandles may either be initialized with a value at
* construction-time, or thereafter via the initialize() function.  A
* FinalHandle may be initialized only if the following conditions
* hold:
* <ul>
*   <li> The FinalHandle has not already been initialized
*   (either during construction, or via <tt>initialize()</tt>)
* </ul>
* For example:
*
* @code
* class Foo
*   : public class_spec<Foo>
*   {
*   friend class factory<Foo>;
*
*   protected:
*     Foo()
*       : f_hBar(self()) // construction without initialization
*       {}
*
*   public:
*     void setBar(Bar::Handle hBar)
*       {
*       initialize(f_hBar, hBar); // post-construction initialization
*       }
*
*   private:
*     FinalHandle<Bar> f_hBar;
*   };
* @endcode
*
* FinalHandles transfer the constness of their parent Object. When a
* FinalHandle is accessed from within a const method of the enclosing
* "parent" class, it will only provide const access to the Object which it
* references. If the enclosing Object becomes only accessed via views the
* FinalHandle will also automatically, and permanently switch its reference
* type from a handle to a view.
*
* Note: In the rare case that a FinalHandle is declared via the mutable
*       keyword, the FinalHandle must be informed of this fact by setting
*       fMutable to true during construction.  A mutable FinalHandle will
*       will return Handles from const methods of the class it is a data
*       member of.  The FinalHandle cannot be reassigned to reference another
*       object.
*
* @author mf  2008.12.01
*
* @see FinalView
* @see FinalHolder
*/
template<class T>
class FinalHandle
    : public MemberHandle<T>
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


    // -------- constructors ------------------------------------------------

    public:
        /**
        * Construct a new FinalHandle referencing NULL via a handle.
        *
        * @param oGuardian  the object that protects this member
        */
        FinalHandle(const Object& oGuardian)
            : MemberHandle<T>(oGuardian)
            {
            }

        /**
        * Construct a new FinalHandle referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        */
        FinalHandle(const Object& oGuardian, const ValueHandle& that)
            : MemberHandle<T>(oGuardian, that)
            {
            // Note: The underlying MemberHandle is initialized in the
            // inherited mutability state, allowing the FinalHandle to act
            // as a handle in non-const methods.
            if (MemberHandle<T>::m_po)
                {
                MemberHandle<T>::m_nMutability = MemberHandle<T>::safe_immutable;
                }
            }

        /**
        * Construct a new FinalHandle referencing specified Object.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        FinalHandle(const Object& oGuardian, const ValueHandle& that, bool fMutable)
            : MemberHandle<T>(oGuardian, that, fMutable)
            {
            if (MemberHandle<T>::m_po && !fMutable)
                {
                MemberHandle<T>::m_nMutability = MemberHandle<T>::safe_immutable;
                }
            }

    private:
        /**
        * Blocked copy constructor
        */
        FinalHandle(const FinalHandle&);


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
        * Return a Handle to the referenced Object.
        *
        * @return a Handle to the referenced Object
        */
        operator ValueHandle()
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
        * Return a Handle to the referenced Object.
        *
        * @return a Handle to the referenced Object
        */
        template<class PT>
        operator TypedHandle<PT>()
            {
            return getFinal();
            }

        /**
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        *
        */
        template<class PT>
        operator TypedHolder<PT>() const
            {
            return getFinal();
            }

        /**
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        */
        template<class PT>
        operator TypedHolder<PT>()
            {
            return getFinal();
            }

        /**
        * Dereference the FinalHandle.
        *
        * @return a const pointer to the referenced Object
        */
        const T* operator->() const
            {
            const T* cpo = MemberHandle<T>::m_po;
            if (MemberHandle<T>::m_nMutability < MemberHandle<T>::safe_immutable)
                {
                MemberHandle<T>::readBarrier();
                if (cpo == NULL)
                    {
                    cpo = MemberHandle<T>::m_po;
                    MemberHandle<T>::readBarrier();
                    }
                }

            if (NULL == cpo)
                {
                coh_throw_npe(typeid(const T));
                }
            return cpo;
            }

        /**
        * Dereference the FinalHandle.
        *
        * @return a const pointer to the referenced Object
        */
        T* operator->()
            {
            T* po = MemberHandle<T>::m_po;
            if (po == NULL &&
                MemberHandle<T>::m_nMutability < MemberHandle<T>::safe_immutable)
                {
                MemberHandle<T>::readBarrier();
                po = MemberHandle<T>::m_po;
                }
            if (MemberHandle<T>::m_fView)
                {
                coh_throw_illegal_state(
                    "attempt to access handle from const FinalHandle");
                }

            if (NULL == po)
                {
                coh_throw_npe(typeid(T));
                }
            return po;
            }

    private:
        /**
        * Blocked assignment operator.
        */
        FinalHandle& operator=(const ValueHandle& that);

        /**
        * Blocked assignment operator.
        */
        FinalHandle& operator=(const FinalHandle<T>& that);

        /**
        * Blocked assignment operator.
        */
        FinalHandle& operator=(FinalHandle<T>& that);

    // ----- SmartMember interface ------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual size64_t retained() const
            {
            const T* cpo = MemberHandle<T>::m_po;
            if (cpo == NULL &&
                MemberHandle<T>::m_nMutability < MemberHandle<T>::safe_immutable)
                {
                MemberHandle<T>::readBarrier();
                cpo = MemberHandle<T>::m_po;
                }

            return cpo == NULL
                    ? 0
                    : cpo->sizeOf(/*fDeep*/ true);
            }


    // ----- helper methods -------------------------------------------------

    protected:
        ValueView getFinal() const
            {
            const T* cpo = MemberHandle<T>::m_po;
            if (MemberHandle<T>::m_prev == NULL)
                {
                if (cpo == NULL &&
                    MemberHandle<T>::m_nMutability < MemberHandle<T>::safe_immutable)
                    {
                    MemberHandle<T>::readBarrier();
                    cpo = MemberHandle<T>::m_po;
                    }
                return ValueView(cpo);
                }
            else
                {
                return TypedHandle<const T>(cpo, *this);
                }
            }

        ValueHandle getFinal()
            {
            T* po = MemberHandle<T>::m_po;
            if (MemberHandle<T>::m_fView)
                {
                coh_throw_illegal_state(
                    "attempt to access handle from const FinalHandle");
                }

            if (MemberHandle<T>::m_prev == NULL)
                {
                if (po == NULL &&
                    MemberHandle<T>::m_nMutability < MemberHandle<T>::safe_immutable)
                    {
                    MemberHandle<T>::readBarrier();
                    }
                return ValueHandle(po);
                }
            else
                {
                return TypedHandle<T>(po, *this);
                }
            }


        /**
        * Initialize the value of a FinalHandle after it is
        * constructed.  A FinalHandle may be initialized only if the
        * following conditions hold:
        * <ul>
        *   <li> The FinalHandle has not already been initialized
        *   (either during construction, or via <tt>initialize()</tt>)
        *   <li> The FinalHandle has not escaped
        * </ul>
        *
        * @param that  the value to initialize this FinalHandle to
        */
        void initialize(ValueHandle that)
            {
            if (MemberHandle<T>::m_nMutability >= MemberHandle<T>::forever_immutable)
                {
                coh_throw_illegal_state(
                    "attempt to initialize const FinalHandle");
                }
            else if (MemberHandle<T>::m_prev == NULL)
                {
                SynchronizedMemberWriteBlock block(MemberHandle<T>::getGuardian());
                if (MemberHandle<T>::m_po != NULL)
                    {
                    coh_throw_illegal_state(
                        "attempt to multiply initialize FinalHandle");
                    }
                MemberHandle<T>::setEscaped(that, &block);
                }
            else if (MemberHandle<T>::m_po != NULL)
                {
                coh_throw_illegal_state(
                    "attempt to multiply initialize FinalHandle");
                }
            else if (that != NULL)
                {
                T* po = get_pointer(that);
                MemberHandle<T>::m_po =
                    NULL == po || NULL == po->_attach(/*fEscaped*/ false)
                        ? NULL : po;
                if (MemberHandle<T>::m_nMutability != MemberHandle<T>::forever_mutable)
                    {
                    MemberHandle<T>::m_nMutability = MemberHandle<T>::safe_immutable;
                    }
                }
            }

    // ----- friends --------------------------------------------------------

    /**
    * @internal
    */
    template<class _T, class H> friend void initialize(FinalHandle<_T>& fh,
                                              H that);
    };

/**
* Perform a dynamic cast the pointer associated with the FinalHandle
* to a the specified handle/view type.
*
* @param h       the FinalHandle from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(FinalHandle<T>& h, bool fThrow = true)
    {
    return cast<D>((typename FinalHandle<T>::ValueHandle) h, fThrow);
    }

/**
* Perform a dynamic cast the pointer associated with the FinalHandle
* to a the specified handle/view type.
*
* @param h       the FinalHandle from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const FinalHandle<T>& h, bool fThrow = true)
    {
    return cast<D>((typename FinalHandle<T>::ValueView) h, fThrow);
    }

/**
* Perform an instanceof check on a FinalHandle.
*
* @param h   the FinalHandle from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(FinalHandle<T>& h)
    {
    return NULL != cast<D>(h, false);
    }

/**
* Perform an instanceof check on a FinalHandle.
*
* @param h   the FinalHandle from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const FinalHandle<T>& h)
    {
    return NULL != cast<D>(h, false);
    }

/**
* Initialize the value of a FinalHandle after it is constructed.  A
* FinalHandle may be initialized only if the following conditions
* hold:
* <ul>
*   <li> The FinalHandle has not already been initialized to a
*   non-NULL value (either during construction, or via
*   <tt>initialize()</tt>)
* </ul>
*
* @param h     the FinalHandle to initialize
* @param that  the value to initialize this FinalHandle to
*/
template<class T, class H>
void initialize(FinalHandle<T>& h, H that)
    {
    h.initialize(that);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_FINAL_HANDLE_HPP

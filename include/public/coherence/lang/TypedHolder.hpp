/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_HOLDER_HPP
#define COH_TYPED_HOLDER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/ChainedHandleElement.hpp"
#include "coherence/lang/TypedHandle.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)

class Object;
template<class T> class MemberHolder;

/**
* A handle implementation which supports referencing Objects as either
* Handles or Views. TypedHolder can safely be used in place of View
* but adds the ability to attempt a safe down cast to a Handle. This differs
* from a const_cast in that the down cast will only succeed if the
* TypedHolder had been assigned from a Handle, an assignment from a View
* results in a TypedHolder whose down cast operation will fail with a
* ClassCastException.
*
* TypedHolder is not for general use, instead it is a convenience handle type
* which can be used for container like classes, which need to be able to
* contain both Handles and Views.
*
* As with TypedHandles, TypedHolders are not thread-safe, and should not be
* used in a multi-threaded context even if protected by external
* synchronization. They should only be used as local variables. The most
* common place where thread-safe holders are needed is for object data
* members. For these cases the Coherence API includes a thread-safe variant
* MemberHolder.
*
* @author mf  2008.01.09
*
* @see MemberHolder
* @see WeakHolder
* @see FinalHolder
*/
template<class T>
class TypedHolder
    : public ChainedHandleElement
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


    // -------- constructors ------------------------------------------------

    public:
        /**
        * Construct a new TypedHolder referencing NULL via a handle.
        */
        TypedHolder()
            : ChainedHandleElement(/*fView*/ false), m_cpo(NULL)
            {
            }

        /**
        * Construct a new TypedHolder referencing the same Object (and in
        * the same manner) as the specified TypedHolder.
        *
        * @param that  the TypedHolder to initialize from
        */
        TypedHolder(const TypedHolder& that)
            : ChainedHandleElement(that, that.m_fView), m_cpo(that.m_cpo)
            {
            }

        /**
        * Construct a new TypedHolder referencing specified Object via a
        * Handle or View.
        *
        * As a Handle is used, the TypedHolder can be used with the
        * cast<...::Handle>() method to retrieve the Handle from the Holder.
        *
        * @param that  a Handle or View to the Object to reference
        */
        template<class DT> TypedHolder(const TypedHandle<DT>& that)
            : ChainedHandleElement(that, constness<DT>::applied),
              m_cpo(get_pointer(that))
            {
            }

        /**
        * Construct a new TypedHolder referencing the same Object (and in
        * the same manner) as the specified TypedHolder.
        *
        * @param that  the TypedHolder to initialize from
        */
        template<class DT> TypedHolder(const TypedHolder<DT>& that)
            : ChainedHandleElement(that, that.m_fView), m_cpo(that.m_cpo)
            {
            }

        /**
        * Construct a new TypedHolder referencing the supplied object.
        *
        * This method is primarily intended to support assignment from NULL,
        * though it will work with other pointers.
        *
        * @param po  pointer to the Object to reference
        */
        TypedHolder(T* po)
            : ChainedHandleElement(/*fView*/ false),
              m_cpo(po == NULL || po->_attach(/*fEscaped*/ false) == NULL
                      ? NULL : po)
            {
            }

        /**
        * Construct a new TypedHolder referencing the supplied object.
        *
        * @param po  pointer to the Object to reference
        */
        template<class DT> TypedHolder(DT* po)
            : ChainedHandleElement(/*fView*/ constness<DT>::applied),
              m_cpo(po == NULL || po->_attach(/*fEscaped*/ false) == NULL
                      ? NULL : po)
            {
            }

        /**
        * Destroy the TypedHolder.
        */
        ~TypedHolder()
            {
            Action nAction = unlink();
            if (m_cpo && nAction != action_none)
                {
                performAction(nAction);
                }
            }


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Assign this TypedHolder to reference the same Object (and in the
        * same manner) as the specified TypedHolder.
        *
        * @param that  the TypedHolder to assign from
        *
        * @return a reference to this TypedHolder
        */
        TypedHolder& operator=(const TypedHolder& that)
            {
            set(get_pointer(that), that.m_fView, that);
            return *this;
            }

        /**
        * Assign this TypedHolder to reference the same Object (and in the
        * same manner) as the specified TypedHolder.
        *
        * @param that  the TypedHolder to assign from
        *
        * @return a reference to this TypedHolder
        */
        template<class DT> TypedHolder& operator=(const TypedHolder<DT>& that)
            {
            set(get_pointer(that), that.m_fView, that);
            return *this;
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        template<class PT>
        operator TypedHandle<const PT>() const
            {
            return TypedHandle<const PT>(m_cpo, *this);
            }

        /**
        * Dereference this holder, returning <tt>T&</tt>.
        *
        * @return a raw <tt>T&</tt> reference to the referenced Object
        *
        * @throws NullPointerException if the this holder is @c NULL
        */
        const T& operator*() const
            {
            const T* po = m_cpo;
            if (NULL == po)
                {
                coh_throw_npe(typeid(T));
                }
            return *po;
            }

        /**
        * Dereference the TypedHolder.
        *
        * @return a const pointer to the referenced Object
        */
        const T* operator->() const
            {
            const T* po = m_cpo;
            if (NULL == po)
                {
                coh_throw_npe(typeid(T));
                }
            return po;
            }


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Return true if the holder contains a handle
        *
        * @return true iff the holder contains a handle
        */
        bool isHandle() const
            {
            return m_fView == false;
            }

        /**
        * Return a Handle to the referenced Object.
        *
        * @return a Handle to the referenced Object, or NULL if the Object is
        *         referenced via a View
        */
        TypedHandle<T> getHandle() const
            {
            if (m_fView)
                {
                return NULL;
                }
            // chain new handle from this
            return TypedHandle<T>(const_cast<T*>(m_cpo), *this);
            }

        /**
        * Set the Holder to reference an Object via a Handle.
        *
        * @param h  the Handle to the Object to reference
        */
        void set(const T* cp, bool fView, const ChainedHandleElement& that)
            {
            // link to the new
            Action nAction = link(that);
            if (m_cpo && nAction != action_none)
                {
                performAction(nAction);
                }
            m_cpo   = cp;
            m_fView = fView;
            }

        /**
        * Perform the specified action.
        *
        * @param nAction  the action to perform
        */
        void performAction(Action nAction)
            {
            switch (nAction)
                {
                case action_flip:
                    m_cpo->_attach(/*fEscaped*/ false);
                    // fall through
                case action_detach:
                    if (m_fView)
                        {
                        m_cpo->_detach(/*fEscaped*/ false);
                        }
                    else
                        {
                        const_cast<T*>(m_cpo)->_detach(/*fEscaped*/ false);
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
        const T* m_cpo;


    // ----- friends --------------------------------------------------------

    /**
    * @internal
    */
    template<class O> friend const O* get_pointer(const TypedHolder<O>& th);

    /**
    * @internal
    */
    template<class O> friend bool is_handle(const TypedHolder<O>& th);

    /**
    * @internal
    */
    template<class D, class H> friend D cast(const TypedHolder<H>& th,
            bool fTest);

    /**
    * @internal
    */
    template<class> friend class TypedHolder;

    /**
    * @internal
    */
    template<class> friend class MemberHolder;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the given TypedHolder to the
* specified stream.
*
* @param out  the stream used to output the description
* @param th   the TypedHolder to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
        const TypedHolder<T>& th)
    {
    out << (typename T::View) th;
    return out;
    }

/**
* @internal
*
* Return the pointer associated with the TypedHolder.
*
* The lifetime of the Object referenced by the returned pointer
* cannot be guaranteed beyond the lifetime of the Holder from
* which it was obtained. Additionally if the associated handle is reassigned
* the lifetime of the referenced Object is also undefined.
*
* @param th the Holder to used in retrieving the pointer.
*
* @return the pointer associated with the TypedHolder.
*/
template<class T>
const T* get_pointer(const TypedHolder<T>& th)
    {
    return th.m_cpo;
    }

/**
* Assign the specified holder to NULL.
*
* @param th the holder to clear
*/
template<class T> void clear_handle(TypedHolder<T>& th)
    {
    th = NULL;
    }

/**
* Return true if the supplied holder equals NULL.
*
* @param th  the holder to test
*
* @return true iff the supplied holder equals NULL
*/
template<class T>
bool is_null(const TypedHolder<T>& th)
    {
    return th == NULL;
    }

/**
* Return true if the supplied holder contains a handle
*
* @param th  the holder to test
*
* @return true iff the supplied holder contains a handle
*/
template<class T>
bool is_handle(const TypedHolder<T>& th)
    {
    return th.isHandle();
    }

/**
* Perform a dynamic cast the pointer associated with the TypedHolder
* to a the specified handle/view type.
*
* @param th      the TypedHolder from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const TypedHolder<T>& th, bool fThrow = true)
    {
    TypedHandle<T> h = th.getHandle();
    if (NULL == h)
        {
        TypedHandle<const T> v = th;

        if (NULL == v)
            {
            // from NULL to any type is allowed
            return D(); // NULL
            }

        // input is a non-NULL view
        if (constness<typename D::ValueType>::applied)
            {
            // input type is a holder(view), output type is a view or holder
            // return a view and let it be converted if necessary to a holder
            // producing a holder(handle) can't happen from here, but we've
            // already proved that the input type is a view (NULL == h)
            TypedHandle<typename D::ValueType> vD(
                (typename D::ValueType*) get_pointer(
                        cast<typename D::ValueType::View>(v, fThrow)), v);
            return vD;
            }
        else
            {
            // output type is a handle; input type is a view (error)
            if (fThrow)
                {
                coh_throw_const_cast(typeid(typename D::ValueType), typeid(*th));
                }
            return D(); // NULL
            }
        }

    // input type is a holder(handle), output type can be handle/view/holder;
    // cast from on the handle, and allow conversion to produce expected
    // result type
    return cast<typename D::ValueType::Handle>(h, fThrow);
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param th  the TypedHolder from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const TypedHolder<T>& th)
    {
    return NULL !=th && NULL != cast<D>(th, false);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_TYPED_HOLDER_HPP

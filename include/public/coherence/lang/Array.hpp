/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ARRAY_HPP
#define COH_ARRAY_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/MemberHolder.hpp"
#include "coherence/lang/SubscriptHandle.hpp"
#include "coherence/lang/SubscriptHolder.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)

extern COH_EXPORT void coh_throw_ioob(size32_t i);


/**
* A fixed size collection of values, indexed by integer position.
*
* Array is a wrapper around standard C++ arrays, adding thread-safety and
* memory management.  Array provides a "length" field which can be used to
* safely determine the last index of the array.
*
* Array's handle type supports direct subscripting:
*
* @code
* for (size32_t i = 0, c = hArr->length; i < c; ++i)
*   {
*   std::cout << hArr[i] << std::endl;
*   }
* @endcode
*
* Only a limited number of element types are supported, including:
* bool, octect_t, char, wchar_t, wchar16_t, int8_t, uint8_t, int16_t,
* uint16_t, int32_t, uint32_t, int64_t, uint64_t, float32_t, and float64_t.
*
* Arrays of Handles and Views to Objects are supported via ObjectArray.
*
* @see ObjectArray
*
* @author mf  2007.07.05
*/
template<class T>
class COH_EXPORT Array
    : public class_spec<Array<T> >
    {
    friend class factory<Array<T> >;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);


    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Handle definition.
        */
        typedef SubscriptHandle<Array<T>, T> Handle;

        /**
        * View definition.
        */
        typedef SubscriptHandle<const Array<T>, const T> View;

        /**
        * Holder definition.
        */
        typedef SubscriptHolder<Array<T>, const T> Holder;


    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Element type
        */
        typedef T ElementType;


    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a new Array instance.
        *
        * @param cValue  the length of the returned Array
        */
        static Handle create(size32_t cValue = 0);


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        *
        * Create a new Array instance.
        *
        * This constructor is not intended for general use.  It assumes
        * partial ownership of the supplied array. That is upon destruction
        * of the managed Array, the destructor of each raw array element
        * will be executed, but the raw array itself will not be explicitly
        * deleted.
        *
        * @param cValue  the length of the returned Array
        * @param aValue  the raw array to be referenced by this Array
        */
        Array(size32_t cValue, T* aValue);

        /**
        * @internal
        *
        * Create a new Array which is a sub-array of an existing Array.
        *
        * The new Array uses the same storage as the original, and an update
        * to one will be reflected in the other.  If this method is supplied
        * with a View to an Array, the caller must store the resulting created
        * Array as a View.  Storing as a Handle is not const correct.  It is
        * for this reason that this constructor is not made available via a
        * create method, instead it should be safely accessed via the
        * subArray methods.
        *
        * @param oha        the Array to delegate to
        * @param iFrom      the starting index (inclusive)
        * @param iTo        the ending index (exclusive), or npos
        *
        * @see subArray
        */
        Array(typename Array<T>::Holder oha, size32_t iFrom, size32_t iTo);

        /**
        * @internal
        *
        * Destructor
        */
        virtual ~Array();

    private:
        /**
        * @internal
        *
        * Blocked copy constructor.
        */
        Array(const Array<T>& that);


    // ----- Array interface ------------------------------------------------

    public:
        /**
        * Return the element stored at the specified index.
        *
        * @param i  the index into the Array
        *
        * @return the element stored at the specified index
        *
        * @throws IndexOutOfBoundsException if i is not a valid index
        */
        inline ElementType& operator[](size32_t i)
            {
            if (i >= length)
                {
                coh_throw_ioob(i);
                }
            return raw[i];
            }

        /**
        * Return the element stored at the specified index.
        *
        * @param i  the index into the Array
        *
        * @return the element stored at the specified index
        *
        * @throws IndexOutOfBoundsException if i is not a valid index
        */
        inline const ElementType& operator[](size32_t i) const
            {
            if (i >= length)
                {
                coh_throw_ioob(i);
                }
            return raw[i];
            }

        /**
        * A region of this Array is compared to a region of a supplied Array.
        *
        * The Array regions are considered equal if each element of the
        * Array region is equal to the corresponding element in the other
        * Array region.
        *
        * In the case of Arrays of Objects this is a deep equality test,
        * that is each element in the supplied Array must reference an Object
        * which compares equal to the Object referenced from the same element
        * within this Array.
        *
        * @param ofSource  the starting index in this Array where the
        *                  comparison begins
        * @param vaOther   view to the Array to compare for equality
        * @param ofOther   the starting index in the other Array where the
        *                  comparison begins
        * @param c         the count of elements to compare, or npos for
        *                  (vaOther->length - ofOther)
        *
        * @return true iff the two Array regions are identical
        *
        * @throws IndexOutOfBoundsException if ofSource + c or ofOther + c
        *         are out of their respective Array's bounds
        *
        * @since Coherence 3.7.1
        */
        virtual bool regionMatches(size32_t ofSource,
                typename Array<T>::View vaOther, size32_t ofOther = 0,
                size32_t c = npos) const;

        /**
        * Return an Array over a subset of the elements in this Array. The
        * returned Array is backed by this Array, and changes in one will
        * be reflected in the other.
        *
        * @param iFrom  the starting index (inclusive)
        * @param iTo    the ending index (exclusive), or npos
        *
        * @return the sub Array
        *
        * @throws IndexOutOfBoundsException if either index is out of the
        *         Array's bounds
        */
        virtual Handle subArray(size32_t iFrom, size32_t iTo);

        /**
        * Return an Array over a subset of the elements in this Array. The
        * returned Array is backed by this Array, and changes in one will
        * be reflected in the other.
        *
        * @param iFrom  the starting index (inclusive)
        * @param iTo    the ending index (exclusive), or npos
        *
        * @return the sub Array
        *
        * @throws IndexOutOfBoundsException if either index is out of the
        *         Array's bounds
        */
        virtual View subArray(size32_t iFrom, size32_t iTo) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * Return a hash of all the values in the Array.
        *
        * @return a hash of all the values in the Array
        */
        virtual size32_t hashCode() const;

        /**
        * Test two Arrays for equality.
        *
        * The Arrays are considered equal if they are of the same type and
        * length, and each element of the Array is equal to corresponding
        * element in the other Array.
        *
        * In the case of Arrays of Objects this is a deep equality test,
        * that is each element in the supplied Array must reference an Object
        * which compares equal to the Object referenced from the same element
        * within this Array.
        *
        * @param v  view to the Array to compare for equality
        *
        * @return true iff the Arrays are identical
        */
        virtual bool equals(Object::View v) const;

        /**
        * Return true iff the Array and all of its elements are immutable.
        *
        * @return true iff the Array and all of its elements are immutable
        */
        virtual bool isImmutable() const;

        /**
         * Return the size in bytes of the Array.
         *
         * @param fDeep  true if nested objects are to be included
         *
         * @return the size in bytes of the Array
         */
         virtual size64_t sizeOf(bool fDeep = false) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Handle clone() const;

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onConst();

        /**
        * {@inheritDoc}
        */
        virtual void onEscape(bool fEscaped) const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Perform a shallow copy of the elements from one Array to another.
        *
        * If the source and destination Arrays are the same, it is
        * allowable to have the source and destination ranges overlap, though
        * an intermediate copy may be performed internally.
        *
        * @param vaSrc  the source Array
        * @param iSrc   the source offset
        * @param haDes  the destination Array
        * @param iDes   the destination offset
        * @param ce     the number of elements to copy; if npos then copy all
        *               that is available without stepping out of bounds
        *
        * @return haDes
        *
        * @throws IndexOutOfBoundsException if either index is out of the
        *         Array's bounds, or if haDes is too small
        */
        static typename Array<T>::Handle copy(typename Array<T>::View vaSrc,
                    size32_t iSrc, typename Array<T>::Handle haDes,
                    size32_t iDes = 0, size32_t ce = npos);

        /**
        * Compute the hashCode for the supplied raw array.
        *
        * @param aValue  the array to hash
        * @param c       the number of elements to hash
        */
        static size32_t hashCode(const T* aValue, size32_t c);


    // ----- data members ---------------------------------------------------


    protected:
        /**
        * The cached hash of all the Array elements. The hash code is not
        * cached until the Array becomes immutable. A value of zero indicates
        * that a hash has not been cached.
        */
        mutable size32_t m_nHashCode;

        /**
        * The super Array in the case that this Array was produced by a call
        * to subArray. If non-NULL then this Array does not own the memory
        * associated with raw. This data member is here solely for the purpose
        * of ensuring that the Array which "owns" the raw array is kept alive
        * and potentially mutable.
        */
        MemberHolder<Array<T> > m_ohDelegate;

    public:
        /**
        * The number of elements in the Array.
        */
        const size32_t length;

        /**
        * The underlying Array of values.
        *
        * This inner class allows the raw array to inherit the Array Object's
        * constness. Direct usage of this data member is discouraged as it
        * does not provide any bounds checking safety.
        */
        class MemberArray
            {
            // ----- constructors ---------------------------------------

            public:
                /**
                * Construct MemberArray for the given array of values.
                */
                MemberArray(T* aValue)
                    : m_aValue(aValue) {}

            // ---- operators -------------------------------------------

            public:
                /**
                * Provides a conversion of this MemberArray to the type 'T*'
                */
                inline operator T*()
                    {
                    return m_aValue;
                    }

                /**
                * Provides a conversion of this constant MemberArray to the
                * type 'const T*'
                */
                inline operator const T*() const
                    {
                    return m_aValue;
                    }

            // ----- data members ---------------------------------------

            protected:
                /**
                * The underlying array of values as 'T*'
                */
                T* m_aValue;

            // ----- friends --------------------------------------------

            friend class Array<T>;
            } raw;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ARRAY_HPP

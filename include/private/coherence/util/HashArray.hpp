/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_HASH_ARRAY_HPP
#define COH_HASH_ARRAY_HPP

#include "coherence/lang.ns"

#include "coherence/util/LongArray.hpp"

#include "private/coherence/lang/AssociativeArray.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* HashArray is a hash-map based implementation of a LongArray.
*
* HashArray is both fast, and stable in a multi-threaded context.  It makes
* use of Object's internal read-write lock to provide thread safety.  As the
* implementation is hash based it does not provide ordered iteration.
*
* @author mf  2009.01.14
*/
class COH_EXPORT HashArray
    : public class_spec<HashArray,
        extends<Object>,
        implements<LongArray> >
    {
    friend class factory<HashArray>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a HashArray.
        *
        * @param cEstimate        the anticipated number of elements that will
        *                         be stored
        * @param flLoadFactor     the acceptable load factor before resizing
        *                         occurs, 0 &lt; n, such that a load factor
        *                         of 1.0 causes resizing when the number of
        *                         entries exceeds the number of buckets
        * @param flGrowthRate     the rate of bucket growth when a resize
        *                         occurs, 0 &lt; n, such that a growth rate
        *                         of 1.0 will double the number of buckets:
        *                         bucketcount = bucketcount * (1 + growthrate)
        */
        HashArray(size32_t cEstimate = 17, float32_t flLoadFactor = 0.3F,
                  float32_t flGrowthRate = 3.0F);

        /**
        * Destruct a HashArray.
        */
        virtual ~HashArray();


    // ----- LongArray interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(int64_t lIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder set(int64_t lIndex, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual int64_t add(Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual bool exists(int64_t lIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(int64_t lIndex);

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View vElement) const;

        /**
        * {@inheritDoc}
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getSize() const;

        /**
        * {@inheritDoc}
        */
        virtual LongArrayIterator::Handle iterator();

        /**
        * {@inheritDoc}
        */
        virtual LongArrayIterator::Handle iterator() const;

        /**
        * {@inheritDoc}
        *
        * @throws UnsupportedOperationException
        */
        virtual LongArrayIterator::Handle iterator(int64_t lIndex);

        /**
        * {@inheritDoc}
        *
        * @throws UnsupportedOperationException
        */
        virtual LongArrayIterator::Handle iterator(int64_t lIndex) const;

        /**
        * {@inheritDoc}
        *
        * @throws UnsupportedOperationException
        */
        virtual int64_t getFirstIndex() const;

        /**
        * {@inheritDoc}
        *
        * @throws UnsupportedOperationException
        */
        virtual int64_t getLastIndex() const;


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual void onEscape(bool fEscaped) const;

    public:
        /**
        * {@inheritDoc}
        */
        virtual size64_t sizeOf(bool fDeep = false) const;


    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The value type.
        */
        class value
            {
            public:
                value()
                    : m_po(NULL), m_fView(false)
                    {}

                value(Object::Holder oh)
                    : m_po(const_cast<Object*>(get_pointer(oh))),
                      m_fView(!instanceof<Object::Handle>(oh))
                    {
                    }

                value(const value& that)
                    : m_po(that.m_po), m_fView(that.m_fView)
                    {}

                value& operator=(Object::Holder oh)
                    {
                    m_po    = const_cast<Object*>(get_pointer(oh));
                    m_fView = !is_handle(oh);
                    return *this;
                    }

                bool operator==(const value& that) const
                    {
                    // only consider reference, not fView
                    return m_po == that.m_po;
                    }

                void attach(bool fEscaped)
                    {
                    Object* po = m_po;
                    if (po != NULL)
                        {
                        if (m_fView)
                            {
                            ((const Object*) po)->_attach(fEscaped);
                            }
                        else
                            {
                            po->_attach(fEscaped);
                            }
                        }
                    }

                void detach(bool fEscaped)
                    {
                    Object* po = m_po;
                    if (po != NULL)
                        {
                        if (m_fView)
                            {
                            ((const Object*) po)->_detach(fEscaped);
                            }
                        else
                            {
                            po->_detach(fEscaped);
                            }
                        }
                    }

                operator Object::Holder() const
                    {
                    if (m_fView)
                        {
                        return (const Object*) m_po;
                        }
                    return m_po;
                    }

            private:
                Object* m_po;
                bool    m_fView;
            };

        /**
        * The storage type.
        */
        typedef AssociativeArray<int64_t, value> Storage;


    // ----- nested class: HashIterator -------------------------------------

    protected:
        /**
        * HashIterator
        */
        class HashIterator
            : public class_spec<HashIterator,
                extends<Object>,
                implements<LongArrayIterator> >
            {
            friend class factory<HashIterator>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct an iterator for a HashArray
                */
                HashIterator(HashArray::Holder ohHa);

            // ----- LongArrayIterator interface ------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual int64_t getIndex() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder getValue() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder setValue(Object::Holder ohValue);

                /**
                * {@inheritDoc}
                */
                virtual void remove();

            // ----- Iterator interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool hasNext() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder next();

            // ----- helper methods --------------------------------------

            protected:
                /**
                * Return the delegate HashArray
                */
                virtual HashArray::Handle getMutableDelegate();

                /**
                * Return the delegate HashArray
                */
                virtual HashArray::View getDelegate() const;

            // ----- data members ----------------------------------------

            protected:
                /**
                * The associated HashArray
                */
                FinalHolder<HashArray> f_ohHa;

                /**
                * The number of slots at the start of the iteration.
                */
                const size32_t m_cSlots;

                /**
                * The next storage array index
                */
                size32_t m_ofNext;

                /**
                * The current HashArray index
                */
                int64_t m_lCurr;

                /**
                * The current value
                */
                MemberHolder<Object> m_ohCurr;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The max index known to be stored in the array.
        */
        int64_t m_lMax;

        /**
        * The storage for the HashArray.
        */
        mutable FinalHandle<Storage> f_hStorage;

        friend class HashIterator;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_HASH_ARRAY_HPP

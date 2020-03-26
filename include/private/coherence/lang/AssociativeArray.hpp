/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ASSOCIATIVE_ARRAY_HPP
#define COH_ASSOCIATIVE_ARRAY_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- file local helpers -------------------------------------------------

namespace
    {
    #define MAX_MODULO 2147483647

    /**
    * Return the next prime for resize purposes.
    *
    * @return the next prime, or 0 if none found
    */
    static size32_t getNextPrime(size32_t nMin)
        {
        static size32_t nMax      = MAX_MODULO; // std::max<int32_t> is prime!
        static size32_t aModulo[] =
            {
            17,31,47,61,79,103,127,149,173,197,229,277,347,397,457,509,587,641,
            701,761,827,883,953,1019,1129,1279,1427,1543,1733,1951,2143,2371,
            2671,2927,3253,3539,3907,4211,4591,4973,5393,5743,6143,6619,6997,
            7529,8009,8423,8819,9311,9929,10069,11087,12203,13003,14051,15017,
            16007,17027,18061,19013,20063,23011,27011,30011,35023,40009,45007,
            50021,60013,70001,80021,90001,100003,120011,140009,160001,180001,
            200003,233021,266003,300007,350003,400009,450001,500009,550007,
            600011,650011,700001,800011,850009,900001,950009,1000003,1100009,
            1200007,1300021,1400017,1500007,1600033,1700021,1800017,1900009,
            2000003,2500009,3000017,3500017,4000037,4500007,5000011,6000011,
            7000003,8000009,9000011,10000019,12000017,14000029,16000057,
            18000041,20000003,25000009,30000001,35000011,40000003,45000017,
            50000017,60000011,70000027,80000023,90000049,100000007,150000001,
            200000033,300000007,400000009,500000003,600000001,700000001,
            800000011,900000011,1000000007,1100000009,1200000041,1300000003,
            1400000023,1500000001,1600000009,1700000009,1800000011,1900000043,
            nMax
            };

        for (size32_t i = 0, nModulo = aModulo[i]; nModulo != nMax; nModulo = aModulo[++i])
            {
            if (nModulo >= nMin)
                {
                return nModulo;
                }
            }
        return 0;
        }
    COH_STATIC_INIT(getNextPrime(0));
    }


/**
* AssociativeArray is a simple hash-map like storage for associating templated
* keys to values.
*
* The key class must be convertable to a size_t for hashing purposes.
*
* The AssociativeArray is not thread-safe.
*
* @author mf  2007.05
*/
template<class K, class V>
class COH_EXPORT AssociativeArray
    : public class_spec<AssociativeArray<K, V> >
    {
    friend class factory<AssociativeArray>;

    // ---- nested enum: ElementState ---------------------------------------

    public:
        /**
        * The state of an element.
        */
        typedef enum
            {
            terminal,
            mapped,
            recycled
            } ElementState;

    // ----- nested class: Element ------------------------------------------

    public:
        /**
        * The array's element type.
        */
        class Element
            {
            public:
                Element()
                    : state(terminal)
                    {
                    }

                K            key;
                V            value;
                ElementState state;
            };

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new AssociativeArray.
        *
        * @param valDefault    the default value
        * @param cEstimate     the anticipated number of elements that will be
        *                      stored
        * @param flLoadFactor  the acceptable load factor before resizing
        *                      occurs, 0 &lt; n, such that a load factor
        *                      of 1.0 causes resizing when the number of
        *                      entries exceeds the number of buckets
        * @param flGrowth      the growth rate
        *
        * @return a new AssociativeArray
        */
        AssociativeArray(V valDefault, size32_t cEstimate = 17,
                float32_t flLoadFactor = 0.3, float32_t flGrowth = 3.0F)
            : m_valDefault(valDefault),
              m_cSlots(getNextPrime(size32_t(cEstimate / flLoadFactor))),
              m_cMods(0),
              m_aElements(new Element[m_cSlots]),
              m_flLoadFactor(flLoadFactor),
              m_flGrowth(flGrowth),
              m_cMaxAttempts(1),
              m_nGrowthTrigger(cEstimate)
            {
            }

        /**
        * @internal
        */
        virtual ~AssociativeArray()
            {
            Element* aElements = m_aElements;
            if (aElements)
                {
                delete[] aElements;
                }
            }


    // ----- AssociativeArray interface -------------------------------------

    public:
        /**
        * Store a pointer for the specified key.
        *
        * @param key    the key
        * @param value  the value
        */
        template<class T>
        void put(const K& key, T value)
            {
            find(key, /*fConst*/ false, /*fGrow*/ true)->value = value;
            }

        /**
        * Return the value associated with the specified key, or 0 if no
        * associaion exists.
        *
        * @param key  the key to lookup
        *
        * @return the associated value, or default if none exists
        */
        V get(const K& key) const
            {
            Element* pElement = const_cast<AssociativeArray*>(this)->
                    find(key, /*fConst*/ true, /*fGrow*/ false);

            return pElement ? pElement->value : m_valDefault;
            }

        /**
        * Return a direct reference to the pointer associated with the key.
        * This allows for combination get and set operations such as
        * map->at(key)++; to be preformed with only one lookup.
        *
        * A pointer to this storage may also be retained and will remain
        * mapped so long as the array does not resize, and the key is not
        * erased. As the array only grows and never shrinks, resizes can be
        * checked for using the getSlots() methods.
        *
        * @return a reference to the value associated with the key.
        */
        V& at(const K& key)
            {
            return find(key, /*fConst*/ false, /*fGrow*/ true)->value;
            }

        /**
        * Remove the key's association from the array, making the key's slot
        * available for other keys.
        */
        V erase(const K& key)
            {
            Element* pElement = find(key, /*fConst*/ true, /*fGrow*/ false);
            if (pElement)
                {
                V valOld = pElement->value;
                pElement->value = m_valDefault;
                pElement->state = recycled;
                --m_cMappings;
                ++m_cMods;
                return valOld;
                }
            return m_valDefault;
            }

        /**
        * Erase all mappings in the array.
        */
        void clear()
            {
            Element* aElement = m_aElements;
            size_t   cSlots   = getSlots();

            V valDefault = m_valDefault;
            for (size_t i = 0; i < cSlots; ++i)
                {
                aElement[i].state = terminal;
                aElement[i].value = valDefault;
                }

            m_cMaxAttempts = 1;
            m_cMappings    = 0;
            ++m_cMods;
            }

        /**
        * Return true if the specified key exists in the array.
        */
        bool containsKey(const K& key) const
            {
            Element* pElement = const_cast<AssociativeArray*>(this)->
                    find(key, /*fConst*/ true, /*fGrow*/ false);

            return pElement != NULL;
            }

        /**
        * Return true if the specivied value exists in the array.
        */
        bool containsValue(V value) const
            {
            Element* aElement = m_aElements;
            size_t   cSlots   = getSlots();

            for (size_t i = 0; i < cSlots; ++i)
                {
                if (aElement[i].state == mapped && aElement[i].value == value)
                    {
                    return true;
                    }
                }
            return false;
            }

        /**
        * Return the underlying array size.
        *
        * @return the array size.
        */
        size32_t getSlots() const
            {
            return m_cSlots;
            }

        /**
        * Return the mod count for the array.
        */
        size32_t getModCount() const
            {
            return m_cMods;
            }

        /**
        * Return the number of mappings in the array.
        */
        size32_t size() const
            {
            return m_cMappings;
            }

        /**
        * Find the array Element for the specified key.
        *
        * @param key     the search key
        * @param fConst  if true, this method will not change the state of
        *                the array
        * @param fGrow   if true, this method is allowed to grow the array
        *                fConst must be false
        *
        * @return the Element or NULL if it does not exist
        */
        Element* find(const K& key, bool fConst = true, bool fGrow = false)
            {
            while (true)
                {
                // recompute on each resize iteration
                Element* aElements = m_aElements;
                size_t   cSlots    = m_cSlots;
                size_t   nSlot     = size_t(key) % cSlots;
                size_t   cMax      = m_cMaxAttempts;
                Element* pFree     = NULL;
                size_t   nFree     = 0;
                bool     fHeavy    = m_cMappings > m_nGrowthTrigger;
                size_t   cAttempts = fConst || fHeavy ? cMax : cSlots;

                for (size_t i = 0; i < cAttempts;  ++i,
                     nSlot = (nSlot + (i * i - i) / 2) % cSlots) // quadratic step
                    {
                    Element* pElement = aElements + nSlot;
                    switch (pElement->state)
                        {
                        case mapped:
                            if (pElement->key == key)
                                {
                                // found entry
                                return pElement;
                                }
                            // bucket collision
                            break;

                        case terminal:
                            // key does not exist in map
                            i = cAttempts; // no further attempts required
                            // fall through

                        case recycled:
                            if (i > cMax)
                                {
                                i = cAttempts; // no further attempts required
                                }

                            if (!fConst && NULL == pFree)
                                {
                                // this is the first free slot
                                pFree = pElement;
                                nFree = i + 1;
                                }
                            break;

                        default:
                            COH_THROW (IllegalStateException::create());
                        }
                    }

                // add entry to map
                if (pFree != NULL && (!fHeavy || nFree == 1))
                    {
                    // utilize first free slot
                    pFree->key   = key;
                    pFree->value = m_valDefault;
                    pFree->state = mapped;
                    ++m_cMappings;
                    ++m_cMods;
                    if (nFree > cMax)
                        {
                        m_cMaxAttempts = nFree;
                        }
                    return pFree;
                    }
                else if (!fConst && fGrow)
                    {
                    // need more space
                    resize();
                    continue; // retry
                    }

                // failed to add entry
                return NULL;
                }
            }


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Return the next size to use during a resize.
        *
        * @param cCurr  the current size of the bucket array.
        *
        * @return the new size of the bucket array
        */
        size32_t getNextSize(size32_t cCurr)
            {
            size32_t cMin = (size32_t) std::min(
                    (int64_t) (cCurr * (1.0F + m_flGrowth)),
                    (int64_t) MAX_MODULO);
            return std::max(cMin, getNextPrime(std::max(cMin, cCurr + 1)));
            }

        /**
        * Resize the array
        */
        void resize()
            {
            Element*  aOld         = m_aElements;
            size32_t  cOld         = m_cSlots;
            size32_t  cNew         = getNextSize(cOld);
            float32_t flLoadFactor = m_flLoadFactor;

            m_cSlots         = cNew;
            m_aElements      = new Element[cNew];
            m_cMappings      = 0;
            m_cMaxAttempts   = 1;
            m_nGrowthTrigger = size32_t(cNew * flLoadFactor) + 1;

            for (size32_t i = 0; i < cOld;)
                {
                if (mapped == aOld[i].state)
                    {
                    // avoid using put() as it could reenter resize()
                    Element* pNew = find(aOld[i].key, /*fConst*/ false, /*fGrow*/ false);
                    if (NULL == pNew)
                        {
                        // insufficient space in the new array; grow again
                        Element* aTmp    = m_aElements;
                        m_cSlots         = cNew = getNextSize(cNew);
                        m_aElements      = new Element[cNew];
                        m_cMappings      = 0;
                        m_cMaxAttempts   = 1;
                        m_nGrowthTrigger = size32_t(cNew * flLoadFactor) + 1;
                        delete[] aTmp;

                        // restart copy of aOld
                        i = 0;
                        continue;
                        }
                    else
                        {
                        // transfer contents
                        pNew->key   = aOld[i].key;
                        pNew->value = aOld[i].value;
                        pNew->state = aOld[i].state;
                        }
                    }
                ++i;
                }

            delete[] aOld;
            }


    // ----- data members ---------------------------------------------------

    public:
        /**
        * Default value
        */
        V m_valDefault;

        /**
        * The size of the array.
        */
        size32_t m_cSlots;

        /**
        * The number of modifications made to the array.
        */
        size32_t m_cMods;

        /**
        * The array of associative Elements.
        */
        Element* m_aElements;

        /**
        * The number of mappings in the array.
        */
        size32_t m_cMappings;

        /**
        * The configured load factor.
        */
        const float32_t m_flLoadFactor;

        /**
        * The growth rate of the array.
        */
        const float32_t m_flGrowth;

        /**
        * The maximum number of attempts to make to find an open slot.
        */
        size_t m_cMaxAttempts;

        /**
        * The size at which the next growth will be triggered.
        */
        size32_t m_nGrowthTrigger;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ASSOCIATIVE_ARRAY_HPP

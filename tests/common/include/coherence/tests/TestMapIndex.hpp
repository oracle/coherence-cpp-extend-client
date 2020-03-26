/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_MAP_INDEX_HPP
#define COH_TEST_MAP_INDEX_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/ValueExtractor.hpp"


COH_OPEN_NAMESPACE2(coherence,tests)

using coherence::util::Comparator;
using coherence::util::SimpleMapIndex;
using coherence::util::ValueExtractor;


/**
* Test MapIndex implementation.
*
* @author tb  2010.10.28
*/
class TestMapIndex
    : public class_spec<TestMapIndex,
        extends<SimpleMapIndex> >
    {
    friend class factory<TestMapIndex>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an index from the given map.
        *
        * @param vExtractor   the ValueExtractor object that is used to
        *                     extract an indexable Object from a value stored
        *                     in the indexed Map.  Must not be NULL.
        * @param fOrdered     true iff the contents of the indexed information
        *                     should be ordered; false otherwise
        * @param fPartial     true iff this is a partial index; false
        *                     otherwise
        * @param vComparator  the Comparator object which imposes an ordering
        *                     on entries in the indexed map; or <tt>NULL</tt>
        *                     if the entries' values natural ordering should
        *                     be used
        * @param fInit        initialize the index if true, default is true
        */
        TestMapIndex(ValueExtractor::View vExtractor, bool fOrdered,
                bool fPartial, Comparator::View vComparator,
                bool fInit = true):
                super(vExtractor, fOrdered, vComparator, fInit),
                m_fPartial(fPartial)
            {
            }

    // ----- MapIndex interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isPartial() const
            {
            return m_fPartial;
            }

    // ----- data members ---------------------------------------------------
    protected:
        /**
        * Indicates if this is a partial index.
        */
        bool m_fPartial;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_MAP_INDEX_HPP

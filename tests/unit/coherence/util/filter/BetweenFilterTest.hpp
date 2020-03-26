/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/tests/TestMapIndex.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/SimpleMapEntry.hpp"
#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/BetweenFilter.hpp"

using namespace coherence::lang;

using coherence::tests::TestMapIndex;

using coherence::util::Comparator;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Set;
using coherence::util::SimpleMapEntry;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::BetweenFilter;
using coherence::util::SimpleMapIndex;
using coherence::util::MapIndex;


/**
 * Test Suite for the BetweenFilter->
 */
class BetweenFilterSuite : public CxxTest::TestSuite
    {
    public:
        void setUp()
            {
            hSortedIndex        = TestMapIndex::create(IdentityExtractor::create(), true, false, (Comparator::View)NULL);
            hUnsortedIndex      = TestMapIndex::create(IdentityExtractor::create(), false, false, (Comparator::View)NULL);
            hSetKeys            = HashSet::create();
            hSetKeysTenToTwenty = HashSet::create();

            SimpleMapEntry::Handle hEntry;

            for (int i = 50; i < 250; i++)
                {
                Integer32::Handle hKey = Integer32::create(i);
                hSetKeys->add(hKey);

                int value = i % 30;

                if (value >= 10 && value <= 20)
                    {
                    hSetKeysTenToTwenty->add(hKey);
                    }

                hEntry = SimpleMapEntry::create(hKey, Integer32::create(value));

                hSortedIndex->insert(entry);
                hUnsortedIndex->insert(entry);
                }

            hEntry = new SimpleMapEntry(Integer32::create(-1000), NULL);
            hSortedIndex->insert(hEntry);
            hUnsortedIndex->insert(hEntry);

            hSetKeys->add(Integer32::create(-1000));
            }

        void ShouldEvaluateTrueForValueWithinRange()
            {
            BetweenFilter::View       hFilter = BetweenFilter::create(IdentityExtractor::create(),
                                                                  Float32::create(50.0F),
                                                                  Float32::create(100.0F));

            TS_ASSERT( hFilter->evaluate(Float32::create(75.0F)) );
            }

        void ShouldSetLowerBoundIncludedToFalseIfLowerBoundIsNull()
            {
            BetweenFilter::View vFilter1 = BetweenFilter::create(IdentityExtractor::create(),
                                                            NULL, Float32::create(50.0F));

            TS_ASSERT_EQUALS(vFilter1->isLowerBoundInclusive(), false);

            BetweenFilter::View vFilter2 = BetweenFilter::create(IdentityExtractor::create(),
                                                            NULL, Float32::create(50.0F),
                                                            true, true);

            TS_ASSERT_EQUALS(vFilter2->isLowerBoundInclusive(), false);
            }

        /**
         * Test value lower than lower bound evaluates to false with a null comparator.
         */
        void shouldEvaluateValueLowerThanRangeToFalse()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(9)), false);
            }

        /**
         * Test value equal to the lower bound evaluates to true when include lower bound flag is set
         * to true and with a null comparator.
         */
        void shouldEvaluateValueEqualToLowerBoundToTrue()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),                                
                    Integer32::create(10), Integer32::create(20));

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(10)), true);
            }

        /**
         * Test value equal to the lower bound evaluates to false when include lower bound flag is set
         * to false and with a null comparator.
         */
        void shouldEvaluateValueEqualToLowerBoundToFalse()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20), false, true);

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(10)), false);
            }

        /**
         * Test value lower than lower bound evaluates to false with a null comparator.
         */
        void shouldEvaluateValueHigherThanRangeToFalse()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(21)), false);
            }

        /**
         * Test value equal to the upper bound evaluates to true when include upper bound flag is set
         * to true and with a null comparator.
         */
        void shouldEvaluateValueEqualToUpperBoundToTrue()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(20)), true);
            }

        /**
         * Test value equal to the upper bound evaluates to false when include upper bound flag is set
         * to false and with a null comparator.
         */
        void shouldEvaluateValueEqualToUpperBoundToFalseWithoutComparator()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20), true, false);

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(20)), false);
            }

        /**
         * Test value evaluates to false if lower bound is null.
         */
        void shouldEvaluateFalseIfLowerBoundIsNull()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    NULL, Integer32::create(20), true, false);

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(20)), false);
            }

        /**
         * Test value evaluates to false if upper bound is null.
         */
        void shouldEvaluateFalseIfUpperBoundIsNull()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), NULL, true, false);

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(Integer32::create(20)), false);
            }

        /**
         * Test that calculateEffectiveness returns the correct result when there
         * is no matching index.
         */
        void shouldCalculateEffectivenessWithNoInvertedIndex()
            {
            HashMap::Handle   hMapIndexes   = HashMap::create();
            HashSet::Handle   hSetKeys      = HashSet::create();
            BetweenFilter::View vFilter       = BetweenFilter::create(IdentityExtractor::create(),
                                                             Integer32::create(10),
                                                             Integer32::create(20));

            size32_t          effectiveness = vFilter->calculateEffectiveness(hMapIndexes, hSetKeys);

            TS_ASSERT_EQUALS(effectiveness, BetweenFilter::eval_cost * hSetKeys->size());
            }

        /**
         * Test that calculateEffectiveness returns the correct result when
         * the index is a SortedSet.
         */
        void shouldCalculateEffectivenessWhenInvertedIndexIsSortedSet()
            {
            HashMap::Handle hMapIndexes = HashMap::create();

            hMapIndexes->put(IdentityExtractor::create(), hSortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));

            size32_t effectiveness = vFilter->calculateEffectiveness(hMapIndexes, hSetKeys);

            TS_ASSERT_EQUALS(effectiveness, 31);
            }

        /**
         * Test that calculateEffectiveness returns the correct result when
         * the index is a plain Set.
         */
        void shouldCalculateEffectivenessWhenInvertedIndexIsPlainSet()
            {
            HashMap::Handle hMapIndexes = HashMap::create();

            hMapIndexes->put(IdentityExtractor::create(), hUnsortedIndex);

            BetweenFilter vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));

            size32_t effectiveness = vFilter->calculateEffectiveness(hMapIndexes, hSetKeys);

            TS_ASSERT_EQUALS(effectiveness, hUnsortedIndex->getIndexContents()->size());
            }

        /**
         * Test applyIndex returns the null and does not alter
         * key set when no index is present.
         */
        void shouldApplyIndexWhenNoIndexPresent()            {
            HashSet::Handle   hKeys       = HashSet::create(hSetKeys);
            HashMap::Handle   hMapIndexes = HashMap::create();
            BetweenFilter::View vFilter     = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));


            TS_ASSERT_EQUALS(vFilter->applyIndex(hMapIndexes, hKeys), vFilter);
            TS_ASSERT_EQUALS(hKeys, hSetKeys);
            }

        /**
         * Test applyIndex returns the null and clears the key set if the
         * lower bound of the range is null.
         */
        void shouldApplyIndexWhenLowerBoundIsNull()            {
            HashSet::Handle   hKeys       = HashSet::create(hSetKeys);
            HashMap::Handle   hMapIndexes = HashMap::create();
            BetweenFilter::View vFilter     = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));


            TS_ASSERT(vFilter->applyIndex(hMapIndexes, hKeys) == NULL);
            TS_ASSERT_EQUALS(hKeys->size(), 0);
            }

        /**
         * Test applyIndex returns the null and clears the key set if the
         * upper bound of the range is null.
         */
        void shouldApplyIndexWhenUpperBoundIsNull()            {
            HashSet::Handle   hKeys       = HashSet::create(hSetKeys);
            HashMap::Handle   hMapIndexes = HashMap::create();
            BetweenFilter::View vFilter     = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), NULL);


            TS_ASSERT(vFilter->applyIndex(hMapIndexes, hKeys) == NULL);
            TS_ASSERT_EQUALS(hKeys->size(), 0);
            }

        /**
         * Test applyIndex returns the null and correctly alters the key set
         * when an sorted index is present.
         */
        void shouldApplyIndexWhenUnsortedIndexPresent()            {
            HashSet::Handle hKeys       = new HashSet<>(hSetKeys);
            HashMap::Handle hMapIndexes = HashMap::create();

            hMapIndexes->put(IdentityExtractor::create(), hUnsortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));

            TS_ASSERT(vFilter->applyIndex(hMapIndexes, hSetKeys) == NULL);
            TS_ASSERT_EQUALS(hKeys, hSetKeysTenToTwenty);
            }

        /**
         * Test applyIndex returns the null and correctly alters the key set
         * when an sorted index is present and only the upper bound of
         * the range is present in the index.
         */
        void shouldApplyIndexWhenIndexPresentAndOnlyUpperBoundMatches()
            {
            Set::View       vKeys       = HashSet::create(hSetKeys);
            HashMap::Handle hMapIndexes = HashMap::create();
            Map::View       vMap        = hSortedIndex->getIndexContents();
            Set::View       vExpected   = vMap->get(Integer32::create(0));

            hMapIndexes->put(IdentityExtractor::create(), hSortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(-1), Integer32::create(0));

            TS_ASSERT(vFilter->applyIndex(hMapIndexes, hSetKeys) == NULL);
            TS_ASSERT_EQUALS(vKeys, vExpected);
            }

        /**
         * Test applyIndex returns the null and correctly alters the key set
         * when an sorted index is present and only the lower bound and
         * the upper bound of the range is present in the index.
         */
        void shouldApplyIndexWhenIndexPresentAndOnlyLowerAndUpperBoundMatches()
            {
            Set::View       vKeys       = HashSet::create(hSetKeys);
            HashMap::Handle hMapIndexes = HashMap::create();
            Map::View       vMap        = hSortedIndex->getIndexContents();

            hMapIndexes->put(IdentityExtractor::create(), hSortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(0), Integer32::create(1));

            TS_ASSERT(vFilter->applyIndex(hMapIndexes, hSetKeys) == NULL);

            HashSet::Handle expected = HashSet::create();
            expected->addAll((Set::View) vMap->get(Integer32::create(0)));
            expected->addAll((Set::View) vMap->get(Integer32::create(1)));

            TS_ASSERT_EQUALS(vKeys, expected);
            }

        /**
         * Test applyIndex returns the null and correctly alters the key set
         * when a sorted index is present.
         */
        void shouldApplyIndexWhenSortedIndexPresent()
            {
            HashSet::Handle hKeys       = new HashSet<>(hSetKeys);
            HashMap::Handle hMapIndexes = HashMap::create();

            hMapIndexes->put(IdentityExtractor::create(), hSortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(10), Integer32::create(20));

            TS_ASSERT_EQUALS(vFilter->applyIndex(hMapIndexes, hKeys), NULL);
            TS_ASSERT_EQUALS(hKeys, hSetKeysTenToTwenty);
            }

        /**
         * Test applyIndex returns the null and correctly alters the key set
         * when an unsorted index is present and none of the values are within
         * the range.
         */
        void shouldApplyIndexWhenUnsortedIndexPresentAndNoValuesInRange()
            {
            HashSet::Handle hKeys       = new HashSet<>(hSetKeys);
            HashMap::Handle hMapIndexes = HashMap::create();

            hMapIndexes->put(IdentityExtractor::create(), hUnsortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(100), Integer32::create(200));

            TS_ASSERT_EQUALS(vFilter->applyIndex(hMapIndexes, hKeys), NULL);
            TS_ASSERT_EQUALS(hKeys->size(), 0);
            }

        /**
         * Test applyIndex returns the null and correctly alters the key set
         * when a sorted index is present and none of the values are within
         * the range.
         */
        void shouldApplyIndexWhenSortedIndexPresentAndNoValuesInRange()
            {
            HashSet::Handle hKeys       = new HashSet<>(hSetKeys);
            HashMap::Handle hMapIndexes = HashMap::create();

            hMapIndexes->put(IdentityExtractor::create(), hSortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(100), Integer32::create(200));

            TS_ASSERT_EQUALS(vFilter->applyIndex(hMapIndexes, hKeys), NULL);
            TS_ASSERT_EQUALS(hKeys->size(), 0);
            }

        /**
         * Should evaluate a value of null to be false.
         */
        void shouldEvaluateNull()
            {
            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(),
                    Integer32::create(100), Integer32::create(200));

            TS_ASSERT_EQUALS(vFilter->evaluateExtracted(NULL), false);
            }

        /**
         * If the lower bound is null it should not match with any
         * values that are null.
         */
        void shouldEvaluateWithNullLowerBound()
            {
            HashSet::Handle hKeys    = new HashSet<>(hSetKeys);
            HashMap::Handle hMapIndexes = HashMap::create();

            hMapIndexes->put(IdentityExtractor::create(), hSortedIndex);

            BetweenFilter::View vFilter = BetweenFilter::create(IdentityExtractor::create(), NULL, Integer32::create(0));

            TS_ASSERT_EQUALS(vFilter->applyIndex(hMapIndexes, hKeys), NULL);
            TS_ASSERT_EQUALS(hKeys, hSortedIndex->getIndexContents()->get(0));
            }

    private:
        HashSet::Handle      hSetKeys;
        HashSet::Handle      hSetKeysTenToTwenty;
        TestMapIndex::Handle hSortedIndex;
        TestMapIndex::Handle hUnsortedIndex;
    };

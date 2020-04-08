/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_TEST_FILTER_HPP
#define COH_TEST_TEST_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/annotation/Portable.hpp"
#include "coherence/io/pof/annotation/PortableProperty.hpp"

#include "coherence/util/Filter.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::util::Filter;
using coherence::io::pof::annotation::Portable;
using coherence::io::pof::annotation::PortableProperty;

/**
 * Simple filter for validation of custom Filters with view-scheme configurations.
 * Wouldn't recommend its use elsewhere; it doesn't do much.
 *
 * @author rl 2019.05.14
 * @since 12.2.1.4
 */ 
class TestFilter
    : public class_spec<TestFilter,
        extends<Object>,
        implements<Filter> >
    {
    friend class factory<TestFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        TestFilter()
            : f_vsAttr(self(), NULL), m_cCount(-1)
            {
            }

        TestFilter(String::View vsAttr, int32_t cCount)
            : f_vsAttr(self(), vsAttr), m_cCount(cCount)
            {
            }

    // ----- Filter interface -----------------------------------------------

    public:
        bool evaluate(Object::View /* v */) const
            {
            return true;
            }

    // ----- accessors ------------------------------------------------------

    public:
        String::View getAttr() const
            {
            return f_vsAttr;
            }

        void setAttr(String::View vsAttr)
            {
            f_vsAttr = vsAttr;
            }

        int32_t getCount() const
            {
            return m_cCount;
            }
        
        void setCount(int32_t cCount)
            {
            m_cCount = cCount;
            }

    // ----- object methods -------------------------------------------------

    public:
        bool equals(Object::View vOther) const
            {
            if (instanceof<TestFilter::View>(vOther))
                {
                    TestFilter::View vOtherFilter = cast<TestFilter::View>(vOther);
                    return Object::equals(getAttr(), vOtherFilter->getAttr()) && getCount() == vOtherFilter->getCount();
                }
            return false;
            }

        size32_t hashCode() const
            {
            return (getAttr() != NULL ? getAttr()->hashCode() : -1) ^ getCount() * 32;
            }

    // ----- data members ---------------------------------------------------

    protected:
        MemberView<String> f_vsAttr;
        int32_t m_cCount;
    };

COH_REGISTER_CLASS(TypedClass<TestFilter>::create()->annotate(Portable::create())
        ->declare(COH_CONSTRUCTOR2(TestFilter, String::View, BoxHandle<const Integer32>))
        ->declare(COH_PROPERTY    (TestFilter, Attr, String::View))
        ->declare(COH_BOX_PROPERTY(TestFilter, Count,  Integer32::View)));

COH_REGISTER_POF_ANNOTATED_CLASS_AI(3014, TestFilter);

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_TEST_FILTER_HPP

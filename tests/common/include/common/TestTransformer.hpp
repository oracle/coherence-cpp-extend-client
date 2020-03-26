/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_TEST_TRANSFORMER_HPP
#define COH_TEST_TEST_TRANSFORMER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/annotation/Portable.hpp"
#include "coherence/io/pof/annotation/PortableProperty.hpp"

#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::util::ValueExtractor;
using coherence::io::pof::annotation::Portable;
using coherence::io::pof::annotation::PortableProperty;

/**
 * Simple ValueExtractor for validation of custom "Transformers" with view-scheme configurations.
 * Wouldn't recommend its use elsewhere; it doesn't do much.
 *
 * @author rl 2019.05.14
 * @since 12.2.1.4
 */ 
class TestTransformer
    : public class_spec<TestTransformer,
        extends<Object>,
        implements<ValueExtractor> >
    {
    friend class factory<TestTransformer>;

    // ----- constructors ---------------------------------------------------

    protected:
        TestTransformer()
            : m_cCount(-1)
            {
            }

        TestTransformer(int32_t cCount)
            : m_cCount(cCount)
            {
            }

    // ----- ValuExtractor interface ----------------------------------------

    public:
        Object::Holder extract(Object::Holder /* ohTarget */) const
        {
        return NULL;
        }

    // ----- accessors ------------------------------------------------------

    public:
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
            if (instanceof<TestTransformer::View>(vOther))
                {
                    TestTransformer::View vOtherTrans = cast<TestTransformer::View>(vOther);
                    return getCount() == vOtherTrans->getCount();
                }
            return false;
            }

        size32_t hashCode() const
            {
            return getCount();
            }

    // ----- data members ---------------------------------------------------

    protected:
        int32_t m_cCount;
    };

COH_REGISTER_CLASS(TypedClass<TestTransformer>::create()->annotate(Portable::create())
        ->declare(COH_CONSTRUCTOR1(TestTransformer, BoxHandle<const Integer32>))
        ->declare(COH_BOX_PROPERTY(TestTransformer, Count,  Integer32::View)));

COH_REGISTER_POF_ANNOTATED_CLASS_AI(3015, TestTransformer);

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_TEST_TRANSFORMER_HPP

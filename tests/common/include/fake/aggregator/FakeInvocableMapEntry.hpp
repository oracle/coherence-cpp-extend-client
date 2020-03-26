/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FAKE_INVOCABLE_MAP_ENTRY_HPP
#define COH_FAKE_INVOCABLE_MAP_ENTRY_HPP

#include "coherence/lang.ns"

#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE2(fake,aggregator)

using coherence::util::InvocableMap;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
/**
* TODO : Doc this class
*
* @author djl  2008.05.20
*/
class FakeInvocableMapEntry
    : public class_spec<FakeInvocableMapEntry,
        extends<Object>,
        implements<InvocableMap::Entry> >
    {
    friend class factory<FakeInvocableMapEntry>;

    // ----- constructors -------------------------------------------

    protected:
        FakeInvocableMapEntry()
            : m_hKey(self()), m_hValue(self())
            {
            }

        FakeInvocableMapEntry(Object::Holder hKey,
                    Object::Holder hValue)
             : m_hKey(self(),hKey), m_hValue(self(),hValue)
             {
             }

    public:
        virtual Object::Holder setValue(Object::Holder ohValue,
                    bool /* fSynthetic */ )
            {
            Object::Holder ohOld = m_hValue;
            m_hValue = ohValue;
            return ohOld;
            }

        virtual void update(ValueUpdater::View vUpdater,
                Object::Holder ohValue)
            {
            vUpdater->update(cast<Object::Handle>(m_hValue),ohValue);
            }

        virtual bool isPresent() const
            {
            return true;
            }

        virtual void remove(bool /* fSynthetic */)
            {
            }

        virtual Object::View getKey() const
            {
            return m_hKey;
            }

        virtual Object::Holder getValue() const
            {
            return m_hValue;
            }

        virtual Object::Holder getValue()
            {
            return m_hValue;
            }

        virtual Object::Holder setValue(Object::Holder ohValue)
            {
            m_hValue = ohValue;
            return ohValue;
            }

        virtual Object::Holder extract(
                    ValueExtractor::View vExtractor) const
            {
            return vExtractor->extract(m_hValue);
            }

        MemberHolder<Object> m_hKey;
        MemberHolder<Object> m_hValue;
        };

COH_CLOSE_NAMESPACE2

#endif       //COH_FAKE_INVOCABLE_MAP_ENTRY_HPP

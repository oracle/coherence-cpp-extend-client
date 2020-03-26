/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/ObjectCountHeapAnalyzer.hpp"

#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeAtomic64;

COH_REGISTER_TYPED_CLASS(ObjectCountHeapAnalyzer);


// ----- constructor --------------------------------------------------------

ObjectCountHeapAnalyzer::ObjectCountHeapAnalyzer()
    {
    }


// ---- ObjectCountHeapAnalyzer interface -----------------------------------

int64_t ObjectCountHeapAnalyzer::getObjectCount() const
    {
    System::gc();
    int64_t cObj = 0;
    for (size_t i = 0, c = s_cCount; i < c; ++i)
        {
        cObj += m_acObjects[i].get();
        }
    return cObj;
    }

int64_t ObjectCountHeapAnalyzer::getImmortalCount() const
    {
    return m_cImmortals.get();
    }

// ----- HeapAnalyzer interface ---------------------------------------------

HeapAnalyzer::Snapshot::View ObjectCountHeapAnalyzer::capture() const
    {
    // create a snapshot which includes the snapshot (+1)
    return Snapshot::create(getObjectCount() + 1);
    }

HeapAnalyzer::Snapshot::View
ObjectCountHeapAnalyzer::delta(HeapAnalyzer::Snapshot::View vSnap) const
    {
    return Snapshot::create(getObjectCount() - vSnap->getObjectCount());
    }

void ObjectCountHeapAnalyzer::registerObject(const Object& o)
    {
    m_acObjects[((size_t) &o) % s_cCount].postAdjust(1, false);
    }

void ObjectCountHeapAnalyzer::unregisterObject(const Object& o)
    {
    m_acObjects[((size_t) &o) % s_cCount].postAdjust(-1, false);
    }

void ObjectCountHeapAnalyzer::registerImmortal(const Object& /*o*/)
    {
    m_cImmortals.postAdjust(1, false);
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ObjectCountHeapAnalyzer::toString() const
    {
    return COH_TO_STRING("Live Objects: " << getObjectCount() << "; Immortals: " << getImmortalCount());
    }


// ----- nested class: Snapshot ---------------------------------------------

// ----- constructors ---------------------------------------------------

ObjectCountHeapAnalyzer::Snapshot::Snapshot(int64_t cObjects)
    : m_cObjects(cObjects)
    {
    }

int64_t ObjectCountHeapAnalyzer::Snapshot::getObjectCount() const
    {
    return m_cObjects;
    }

HeapAnalyzer::Snapshot::View
ObjectCountHeapAnalyzer::Snapshot::delta(HeapAnalyzer::Snapshot::View vThat) const
    {
    return Snapshot::create(m_cObjects - vThat->getObjectCount());
    }

TypedHandle<const String> ObjectCountHeapAnalyzer::Snapshot::toString() const
    {
    return COH_TO_STRING("Objects: " << getObjectCount());
    }

COH_CLOSE_NAMESPACE2

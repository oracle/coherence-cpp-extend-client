/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/HeapAnalyzer.hpp"

#include "coherence/lang/IllegalStateException.hpp"
#include "coherence/lang/System.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


HeapAnalyzer::Snapshot::View HeapAnalyzer::ensureHeap(
        HeapAnalyzer::Snapshot::View vSnap, int64_t cDelta)
    {
    HeapAnalyzer::View vAnalyzer = System::getHeapAnalyzer();
    if (vSnap != NULL)
        {
        Snapshot::View vDelta  = vAnalyzer->delta(vSnap);
        int64_t        cActual = vDelta->getObjectCount();

        if (cActual != cDelta)
            {
            COH_THROW_STREAM (IllegalStateException, "mismatched object count: "
                    << cActual << " != " << cDelta << '\n' << vDelta);
            }
        }

    return vAnalyzer->capture();
    }

COH_CLOSE_NAMESPACE2

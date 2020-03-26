/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVOLVABLE_PORTABLE_OBJECT_HPP
#define COH_EVOLVABLE_PORTABLE_OBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;


/**
* Extension of the PortableObject interface that supports forwards-
* and backwards-compatibility of its POF serialized state.
*
* Note: For performance reasons in C++ it is generally preferable to
* use implements<PortableObject, Evolvable> over
* implements<EvolvablePortableObject>, as the internal cast<> operator is
* better optimized for the former case. It is for this reason that this class
* is not prefixed by COH_EXPORT as it is not used internally within Coherence.
*
* @author jh  2008.01.14
*/
class EvolvablePortableObject
    : public interface_spec<EvolvablePortableObject,
        implements<PortableObject, Evolvable> >
    {
    };

COH_CLOSE_NAMESPACE3

#endif // COH_EVOLVABLE_PORTABLE_OBJECT_HPP

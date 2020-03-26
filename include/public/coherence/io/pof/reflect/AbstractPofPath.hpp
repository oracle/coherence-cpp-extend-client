/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_POF_PATH_HPP
#define COH_ABSTRACT_POF_PATH_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/reflect/PofNavigator.hpp"
#include "coherence/io/pof/reflect/PofValue.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Abstract base class for static, path-based implementations of the
* PofNavigator interface.
*
* @author as/gm  2009.03.01
* @since Coherence 3.5
*/
class COH_EXPORT AbstractPofPath
    : public abstract_spec<AbstractPofPath,
        extends<Object>,
        implements<PofNavigator, PortableObject> >
    {
    // ----- PofNavigator interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual PofValue::Handle navigate(PofValue::Handle hValueOrigin) const;


    // ----- abstract methods -----------------------------------------------

    protected:
        /**
        * Return a collection of path elements.
        *
        * @return a collection of path elements
        */
        virtual Array<int32_t>::View getPathElements() const = 0;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_POF_PATH_HPP

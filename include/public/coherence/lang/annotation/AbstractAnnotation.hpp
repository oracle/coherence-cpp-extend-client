/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_ANNOTATION_HPP
#define COH_ABSTRACT_ANNOTATION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Annotation.hpp"
#include "coherence/lang/Class.hpp"


COH_OPEN_NAMESPACE3(coherence,lang,annotation)

/**
 * AbstractAnnotation provides a common base class for Annotations.
 *
 * @author mf  2011.03.03
 *
 * @since Coherence 3.7.1
 */
class COH_EXPORT AbstractAnnotation
    : public abstract_spec<AbstractAnnotation,
          extends<Object>,
          implements<Annotation> >
    {
    // ----- Annotation interface -------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual Class::View annotationType() const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_ANNOTATION_HPP

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ANNOTATION_HPP
#define COH_ANNOTATION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/interface_spec.hpp"



COH_OPEN_NAMESPACE2(coherence, lang)

// ----- forward declarations -----------------------------------------------

class Class;

/**
 * Annotation is the common interface extended by all annotation types.
 *
 * @author mf  2011.03.01
 *
 * @Since Coherence 3.7.1
 */
class COH_EXPORT Annotation
    : public interface_spec<Annotation>
    {
    // ----- Annotation interface -------------------------------------------

    public:
        /**
         * Return the annotation type of this annotation.
         */
        virtual TypedHandle<const Class> annotationType() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ANNOTATION_HPP

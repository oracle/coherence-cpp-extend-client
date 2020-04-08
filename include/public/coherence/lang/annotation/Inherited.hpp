/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INHERITED_HPP
#define COH_INHERITED_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/annotation/AbstractAnnotation.hpp"
#include "coherence/lang/class_spec.hpp"

COH_OPEN_NAMESPACE3(coherence,lang,annotation)

/**
 * The Inherited annotation is used to annotate annotations in order to
 * indicate that the annotation is to be automatically inherited by derived
 * AnnotationElements.
 *
 * @author mf  2011.03.03
 *
 * @since Coherence 3.7.1
 */
class COH_EXPORT Inherited
    : public class_spec<Inherited,
          extends<AbstractAnnotation> >
    {
    friend class factory<Inherited>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        using this_spec::create;


    // ----- constants ------------------------------------------------------

    public:
        /**
         * Return a singleton instance of the Inherited Annotation.
         */
        static Inherited::View getInstance();
    };

COH_CLOSE_NAMESPACE3

#endif // COH_INHERITED_HPP

/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ANNOTATED_ELEMENT_HPP
#define COH_ANNOTATED_ELEMENT_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/abstract_spec.hpp"
#include "coherence/lang/Annotation.hpp"
#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)

// ----- forward declarations -----------------------------------------------

class Class;

/**
 * AnnotatedElement represents an annotated element.
 *
 * @author mf 2011.03.01
 *
 * @since Coherence 3.7.1
 */
class COH_EXPORT AnnotatedElement
    : public abstract_spec<AnnotatedElement>
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Construct an AnnotatedElement.
         */
        AnnotatedElement();


    // ----- AnnotatedElement interface -------------------------------------

    public:
        /**
         * Returns this element's annotation for the specified type if such an
         * annotation is present, else NULL.
         *
         * @param vClassAnnotation  the annotation type
         *
         * @return the Annotation or NULL if not present
         */
        virtual Annotation::View getAnnotation(TypedHandle<const Class> vClassAnnotation) const;

        /**
         * Returns all annotations present on this element.
         *
         * @return all annotations present on this element
         */
        virtual ObjectArray::View getAnnotations() const;

        /**
         * Returns all annotations directly present on this element.
         *
         * @return all annotations present on this element
         */
        virtual ObjectArray::View getDeclaredAnnotations() const;

        /**
         * Return true the specified annotation is present on this element.
         *
         * @return true the specified annotation is present on this element.
         */
        virtual bool isAnnotationPresent(TypedHandle<const Class> vClassAnnotation) const;

        /**
         * Add the specified Annotation to this element.
         *
         * @param vAnnotation  the annotation
         */
        virtual AnnotatedElement::Handle addAnnotation(Annotation::View vAnnotation);

        /**
         * Return the parent of this AnnotationElement or NULL if there is none.
         *
         * The default implementation always returns NULL.
         *
         * @return the parent of this AnnotationElement or NULL if there is none
         */
        virtual AnnotatedElement::View getSuperelement() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
         * The map of resolved annotations, key'd by annotation type Class
         */
        mutable FinalHandle<Object> f_hMapAnnotations;

        /**
         * The list of declared annotations.
         */
        FinalHandle<Object> f_hListAnnotationsDeclared;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ANNOTATED_ELEMENT_HPP

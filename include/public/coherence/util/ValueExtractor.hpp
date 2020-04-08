/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VALUE_EXTRACTOR_HPP
#define COH_VALUE_EXTRACTOR_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* ValueExtractor is used to both extract values (for example, for sorting
* or filtering) from an object, and to provide an identity for that
* extraction.
*
* <b>Important Note:</b> all classes that implement the ValueExtractor
* interface must explicitly implement the Object::hashCode() and
* Object::equals methods in a way that is based solely on the object's
* serializable state.
*
* @author jh  2008.02.26
*/
class COH_EXPORT ValueExtractor
    : public interface_spec<ValueExtractor>
    {
    // ----- ValueExtractor interface ---------------------------------------

    public:
        /**
        * Extract the value from the passed object. The returned value may be
        * NULL.
        *
        * @param  ohTarget  an Object to retrieve the value from
        *
        * @return the extracted value; NULL is an acceptable value
        *
        * @throws ClassCastException if this ValueExtractor is incompatible with
        *         the passed object to extract a value from and the
        *         implementation <b>requires</b> the passed object to be of a
        *         certain type
        * @throws Exception if this ValueExtractor encounters an exception in
        *         the course of extracting the value
        * @throws IllegalArgumentException if this ValueExtractor cannot
        *         handle the passed object for any other reason; an
        *         implementor should include a descriptive message
        */
        virtual Object::Holder extract(Object::Holder ohTarget) const = 0;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Compare the ValueExtractor with another object to determine
        * equality. Two ValueExtractor objects, <i>ve1</i> and <i>ve2</i> are
        * considered equal iff <tt>ve1->extract(v)</tt> equals
        * <tt>ve2->extract(v)</tt> for all values of <tt>v</tt>.
        *
        * @param v  the reference object with which to compare
        *
        * @return true iff this ValueExtractor and the passed object are
        *         equivalent ValueExtractors
        */
        using Object::equals;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_VALUE_EXTRACTOR_HPP

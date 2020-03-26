/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_EXTRACTOR_HPP
#define COH_TYPED_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "coherence/util/extractor/ReflectionExtractor.hpp"


COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Template based ValueExtractor implementation.
*
* This extractor functions on const methods which return Objects, for an
* extractor which works with non-Object return types see BoxExtractor.
*
* For ease of use the COH_TYPED_EXTRACTOR macro can be used to easily construct
* an instance of this class. For example the following constructs an extractor
* for calling the "String::View Address::getState() const" method.
*
* @code
* ValueExtractor::View vExt = COH_TYPED_EXTRACTOR(String::View, Address, getState);
* @endcode
*
* @author mf 2009.03.20
*
* @see BoxExtractor
*/
template<class R, class C, R (C::*M)() const, class RH = Object::Holder,
    class OH = typename C::Holder>
class TypedExtractor
    : public class_spec<TypedExtractor<R, C, M, RH, OH>,
        extends<ReflectionExtractor> >
    {
    friend class factory<TypedExtractor<R, C, M, RH, OH> >;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a TypedExtractor
        */
        TypedExtractor()
            {
            }

        /**
        * Construct a TypedExtractor based on a method name and optional
        * parameters.
        *
        * The method name is only used for the purposes of serializing the
        * extractor for execution on remote Java members.
        *
        * @param vsMethod  the name of the method to invoke via reflection
        */
        TypedExtractor(String::View vsMethod)
            : class_spec<TypedExtractor<R, C, M, RH, OH>,
            extends<ReflectionExtractor> >(vsMethod)
            {
            }


   // ----- ValueExtractor interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder extract(Object::Holder ohTarget) const
            {
            // Fix for Bug19826856/COH-12476; strict aliasing failure
            // get_pointer needed for GCC 4.8.2
            const C* pTarget = get_pointer(cast<OH>(ohTarget));
            return RH(((*pTarget).*(M))());
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const
            {
            return instanceof<typename TypedExtractor<R, C, M, RH, OH>::View>(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            return Class::getClassName(this)->hashCode();
            }

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return Class::getClassName(this);
            }
    };

COH_CLOSE_NAMESPACE3

/**
* Helper macro for creating a TypedExtractor.
*
* @param H  the return object type, i.e. String::View
* @param C  the class to call the method on, i.e. Address
* @param M  the method to call, i.e. getState
*/
#define COH_TYPED_EXTRACTOR(H, C, M) \
    coherence::util::extractor::TypedExtractor<H, C, &C::M>::create(#M)

#endif // COH_TYPED_EXTRACTOR_HPP

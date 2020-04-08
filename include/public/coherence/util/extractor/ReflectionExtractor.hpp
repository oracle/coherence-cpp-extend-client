/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REFLECTION_EXTRACTOR_HPP
#define COH_REFLECTION_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Reflection-based ValueExtractor implementation.
*
* Local execution of a ReflectionExtractor is dependent upon target class and
* method having been registered with the SystemClassLoader. For cases where
* the registration has not been performed or is not possible locally executable
* C++ extractors may either be custom built, or auto-generated using the
* TypedExtractor<>.
*
* @author djl/mf 2008.03.06
*
* @see ChainedExtractor
* @see TypedExtractor
* @see Method
*/
class COH_EXPORT ReflectionExtractor
    : public cloneable_spec<ReflectionExtractor,
        extends<AbstractExtractor> >
    {
    friend class factory<ReflectionExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an empty ReflectionExtractor
        * (necessary for the PortableObject interface).
        */
        ReflectionExtractor();

        /**
        * Construct a ReflectionExtractor based on a method name, optional
        * parameters and the entry extraction target.
        *
        * @param vsMethod  the name of the method to invoke via reflection
        * @param vaParam   the array of arguments to be used in the method
        *                  invocation; may be NULL
        * @param nTarget   one of the {@link #value} or {@link #key} values
        *
        * @since Coherence 3.5
        */
        ReflectionExtractor(String::View vsMethod,
                ObjectArray::View vaParam = NULL, int32_t nTarget = value);

        /**
        * Copy constructor.
        */
        ReflectionExtractor(const ReflectionExtractor& that);


   // ----- ValueExtractor interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder extract(Object::Holder ohTarget) const;


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Determine the name of the method that this extractor is configured
        * to invoke.
        *
        * @return the name of the method to invoke using reflection
        */
        virtual String::View getMethodName() const;

        /**
        * Return the array of arguments used to invoke the method.
        *
        * @return the array of arguments used to invoke the method
        */
        virtual ObjectArray::View getParameters() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The name of the method to invoke.
        */
        FinalView<String> f_vsMethod;

        /**
        * The parameter array.
        */
        FinalView<ObjectArray> f_vaParam;

        /**
         * The resolved Method.
         */
        mutable MemberView<Method> m_vMethod;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_REFLECTION_EXTRACTOR_HPP

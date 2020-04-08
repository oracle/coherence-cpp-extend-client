/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REFLECTION_UPDATER_HPP
#define COH_REFLECTION_UPDATER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Reflection-based ValueUpdater implementation.
*
* Local execution of a ReflectionUpdater is dependent upon target class and
* method having been registered with the SystemClassLoader. For cases where
* the registration has not been performed or is not possible locally executable
* C++ updaters may either be custom built, or auto-generated using the
* TypedUpdater<>.
*
* @author djl/mf 2008.04.08
*
* @see CompositeUpdater
* @see TypedUpdater
* @see Method
*/
class COH_EXPORT ReflectionUpdater
    : public cloneable_spec<ReflectionUpdater,
        extends<Object>,
        implements<ValueUpdater, PortableObject> >
    {
    friend class factory<ReflectionUpdater>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an empty ReflectionUpdater
        * (necessary for the PortableObject interface).
        */
        ReflectionUpdater();

        /**
        * Construct a ReflectionUpdater for a given method name. This
        * implementation assumes that the corresponding classes will have one
        * and only one method with a specified name and this method will have
        * exactly one parameter.
        */
        ReflectionUpdater(String::View vsMethod);

        /**
        * Copy constructor.
        */
        ReflectionUpdater(const ReflectionUpdater& that);


    // ----- ValueUpdater interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void update(Object::Handle hTarget,
                Object::Holder ohValue) const;


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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The name of the method to invoke.
        */
        FinalView<String> f_vsMethod;

        /**
         * The resolved Method.
         */
        mutable MemberView<Method> m_vMethod;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_REFLECTION_UPDATER_HPP

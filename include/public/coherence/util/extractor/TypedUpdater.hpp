/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_UPDATER_HPP
#define COH_TYPED_UPDATER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "coherence/util/extractor/ReflectionUpdater.hpp"


COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Template based ValueUpdater implementation.
*
* This updater functions on non-const methods which take Objects, for an
* updater which works with non-Object types see BoxUpdater.
*
* For ease of use the COH_TYPED_UPDATER macro can be used to easily construct
* an instance of this class. For example the following constructs an updater
* for calling the "void Address::setState(String::View)" method.
*
* @code
* ValueUpdater::View vUpd = COH_TYPED_UPDATER(Address, setState, String::View);
* @endcode
*
* @author mf 2009.07.29
*
* @see BoxUpdater
*/
template<class A, class C, void (C::*M)(A), class AH = A, class OH = typename C::Handle>
class TypedUpdater
    : public class_spec<TypedUpdater<A, C, M, AH, OH>,
        extends<ReflectionUpdater> >
    {
    friend class factory<TypedUpdater<A, C, M, AH, OH> >;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a TypedUpdater
        */
        TypedUpdater()
            {
            }

        /**
        * Construct a TypedUpdater based on a method name and optional
        * parameters.
        *
        * The method name is only used for the purposes of serializing the
        * updater for execution on remote Java members.
        *
        * @param vsMethod  the name of the method to invoke via reflection
        */
        TypedUpdater(String::View vsMethod)
            : class_spec<TypedUpdater<A, C, M, AH, OH>,
            extends<ReflectionUpdater> >(vsMethod)
            {
            }


   // ----- ValueUpdater interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void update(Object::Handle hTarget, Object::Holder ohValue) const
            {
            // Bug19826856,COH-12476; strict aliasing errors
            // casting to local variable seems to be necessary, suspect
            // a compiler issue, GCC 4.8.2
            // get_pointer also needed for GCC 4.8.2
            OH hOHTarget = cast<OH>(hTarget);
            C* pTarget = get_pointer(hOHTarget);
            (*pTarget.*(M))(cast<AH>(ohValue));
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const
            {
            return instanceof<typename TypedUpdater<A, C, M, AH, OH>::View>(v);
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
* Helper macro for creating a TypedUpdater.
*
* @param C  the class to call the method on, i.e. Address
* @param M  the method to call, i.e. setState
* @param A  the argument type, i.e. String::View
*/
#define COH_TYPED_UPDATER(C, M, A) \
    coherence::util::extractor::TypedUpdater<A, C, &C::M>::create(#M)

#endif // COH_TYPED_UPDATER_HPP

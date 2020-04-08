/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BOX_UPDATER_HPP
#define COH_BOX_UPDATER_HPP

#include "coherence/lang.ns"

#include "coherence/util/extractor/TypedUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


/**
* Template based auto-boxing ValueUpdater implementation.
*
* This updater functions on non-const methods which take unmanaged types for
* which there is a corresponding the is a managed type which can "box"
* them via a BoxHandle.
*
* For ease of use the COH_BOX_UPDATER macro can be used to easily construct
* an instance of this class.  For example the following constructs an updater
* for calling the "void Address::setZip(int32_t)" method.
*
* @code
* ValueUpdater::View vUpd = COH_BOX_UPDATER(Address, setZip, Integer32::View);
* @endcode
*
* In the case that the supplied class does not extend from Object and instead
* relies on the Managed<> wrapper, the COH_MANAGED_BOX_UPDATER is to be
* used.
*
* @author mf 2009.07.29
*
* @see TypedUpdater
*/
template<class AH, class C, void (C::*M)(typename AH::ValueType::BoxedType),
    class OH = typename C::Handle>
class BoxUpdater
    : public class_spec<BoxUpdater<AH, C, M, OH>,
        extends<TypedUpdater<typename AH::ValueType::BoxedType, C, M,
            BoxHandle<typename AH::ValueType>, OH> > >
    {
    friend class factory<BoxUpdater<AH, C, M, OH> >;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a BoxUpdater
        */
        BoxUpdater()
            {
            }

        /**
        * Construct a BoxUpdater based on a method name and optional
        * parameters.
        *
        * The method name is only used for the purposes of serializing the
        * extractor for execution on remote Java members.
        *
        * @param vsMethod  the name of the method to invoke via reflection
        */
        BoxUpdater(String::View vsMethod)
            : class_spec<BoxUpdater<AH, C, M, OH>,
                extends<TypedUpdater<typename AH::ValueType::BoxedType, C, M,
                    BoxHandle<typename AH::ValueType>, OH> > >(vsMethod)
            {
            }
    };

COH_CLOSE_NAMESPACE3

/**
* Helper macro for creating a BoxUpdater.
*
* @param C  the class to call the method on, i.e. Address
* @param M  the method to call, i.e. setZip
* @param A  the argument handle type to unbox from, i.e. Integer32::View
*/
#define COH_BOX_UPDATER(C, M, A) \
    coherence::util::extractor::BoxUpdater<A, C, &C::M>::create(#M)

/**
* Helper macro for creating a BoxUpdater around a Managed<> wrapped class.
*
* @param C  the Managed<> wrapped class to call the method on, i.e. Address
* @param M  the method to call, i.e. setZip
* @param A  the argument handle type to unbox from, i.e. Integer32::View
*/
#define COH_MANAGED_BOX_UPDATER(C, M, A) \
    coherence::util::extractor::BoxUpdater<A, C, &C::M, \
        coherence::lang::Managed<C>::Handle>::create(#M)

#endif // COH_BOX_UPDATER_HPP

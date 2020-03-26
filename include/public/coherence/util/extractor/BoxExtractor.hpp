/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BOX_EXTRACTOR_HPP
#define COH_BOX_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/extractor/TypedExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


/**
* Template based auto-boxing ValueExtractor implementation.
*
* This extractor functions on const methods which return unmanaged types for
* which there is a corresponding the is a managed type which can "box"
* them via a BoxHandle.
*
* For ease of use the COH_BOX_EXTRACTOR macro can be used to easily construct
* an instance of this class.  For example the following constructs an extractor
* for calling the "int32_t Address::getZip() const" method.
*
* @code
* ValueExtractor::View vExt = COH_BOX_EXTRACTOR(Integer32::View, Address, getZip);
* @endcode
*
* In the case that the supplied class does not extend from Object and instead
* relies on the Managed<> wrapper, the COH_MANAGED_BOX_EXTRACTOR is to be
* used.
*
* @author mf 2009.03.20
*
* @see TypedExtractor
*/
template<class RH, class C, typename RH::ValueType::BoxedType (C::*M)() const,
    class OH = typename C::Holder>
class BoxExtractor
    : public class_spec<BoxExtractor<RH, C, M, OH>,
        extends<TypedExtractor<typename RH::ValueType::BoxedType, C, M,
            BoxHandle<typename RH::ValueType>, OH> > >
    {
    friend class factory<BoxExtractor<RH, C, M, OH> >;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a BoxExtractor
        */
        BoxExtractor()
            {
            }

        /**
        * Construct a BoxExtractor based on a method name and optional
        * parameters.
        *
        * The method name is only used for the purposes of serializing the
        * extractor for execution on remote Java members.
        *
        * @param vsMethod  the name of the method to invoke via reflection
        */
        BoxExtractor(String::View vsMethod)
            : class_spec<BoxExtractor<RH, C, M, OH>,
            extends<TypedExtractor<typename RH::ValueType::BoxedType, C, M,
                BoxHandle<typename RH::ValueType>, OH> > >(vsMethod)
            {
            }
    };

COH_CLOSE_NAMESPACE3

/**
* Helper macro for creating a BoxExtractor.
*
* @param H  the handle type to box to, i.e. Integer32::View
* @param C  the class to call the method on, i.e. Address
* @param M  the method to call, i.e. getZip
*/
#define COH_BOX_EXTRACTOR(H, C, M) \
    coherence::util::extractor::BoxExtractor<H, C, &C::M>::create(#M)

/**
* Helper macro for creating a BoxExtractor around a Managed<> wrapped class.
*
* @param H  the handle type to box to, i.e. Integer32::View
* @param C  the Managed<> wrapped class to call the method on, i.e. Address
* @param M  the method to call, i.e. getZip
*/
#define COH_MANAGED_BOX_EXTRACTOR(H, C, M) \
    coherence::util::extractor::BoxExtractor<H, C, &C::M, \
        coherence::lang::Managed<C>::Holder>::create(#M)

#endif // COH_BOX_EXTRACTOR_HPP

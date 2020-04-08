/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REFERENCE_LIBRARY_HPP
#define COH_REFERENCE_LIBRARY_HPP

#include "coherence/lang.ns"

#include "coherence/util/IdentityHashMap.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::IdentityHashMap;


/**
* A "library" of object references and their corresponding identities in
* the POF stream.
* @author wl  2010.09.21
*/
class COH_EXPORT ReferenceLibrary
      : public class_spec<ReferenceLibrary,
        extends <Object> >
    {
    friend class factory<ReferenceLibrary>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        ReferenceLibrary();


    // ----- member methods -------------------------------------------------

    public:
        /**
        * Look up an identity for an object.
        *
        * @param v  the object
        *
        * @return the identity, or -1 if the object is not registered
        */
        virtual int32_t getIdentity(Object::View v) const;

        /**
        * Register an object.
        *
        * @param v  the object
        *
        * @return the assigned identity for the object
        *
        * @throws IllegalStateException  if the object is already registered
        */
        virtual int32_t registerReference(Object::View v);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The reference counter.
        */
        int32_t m_cRefs;

        /**
        * A map from objects that can be referenced to their Integer
        * identities.
        */
        FinalHandle<IdentityHashMap> f_hMapIdentities;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_REFERENCE_LIBRARY_HPP


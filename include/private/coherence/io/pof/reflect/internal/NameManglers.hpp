/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAME_MANGLERS_HPP
#define COH_NAME_MANGLERS_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/pof/reflect/internal/NameMangler.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

/**
* NameManglers contain singleton access to both a FieldMangler and
* MethodMangler. NameManglers provide the ability to derive the same name of
* a property regardless of their access or inspection methodology.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT NameManglers
    : public extends<Object>
    {
    // ----- inner class: FieldMangler --------------------------------------

    public:
        /**
        * A NameMangler implementation that is aware of field naming
        * conventions and is able to convert from a field name to a generic
        * name.
        * <p>
        * The conventions this mangler is aware of are prefixing variables
        * with \c m_ or \c f. For example \c m_bar and \c fBar
        * would both be converted to a mangled name of \c bar.
        *
        * @author hr 2011.06.29
        *
        * @since 3.7.1
        */
        class COH_EXPORT FieldMangler
            : public class_spec<FieldMangler,
                extends<Object>,
                implements<NameMangler> >
            {
            friend class factory<FieldMangler>;

            // ----- NamedMangler interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual String::View mangle(String::View vsName) const;
            };

    // ----- inner class: MethodMangler -------------------------------------

    public:
        /**
        * A NameMangler implementation that is aware of method naming
        * conventions and is able to convert from a method name to a generic
        * name.
        * <p>
        * The conventions this mangler is aware of are the getter and setter
        * style methods, i.e. \c getBar or \c setBar which are both
        * converted to a mangled name of \c bar.
        *
        * @author hr 2011.06.29
        *
        * @since 3.7.1
        */
        class COH_EXPORT MethodMangler
            : public class_spec<MethodMangler,
                extends<Object>,
                implements<NameMangler> >
            {
            friend class factory<MethodMangler>;

            // ----- NamedMangler interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual String::View mangle(String::View vsName) const;
            };

    // ----- constants-------------------------------------------------------

    public:
        /**
        * Singleton FieldMangler reference.
        */
        static NameMangler::View getFieldMangler();

        /**
        * Singleton MethodMangler reference.
        */
        static NameMangler::View getMethodMangler();
    };

COH_CLOSE_NAMESPACE5

#endif /* COH_NAME_MANGLERS_HPP */

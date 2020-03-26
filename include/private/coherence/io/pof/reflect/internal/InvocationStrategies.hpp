/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_STRATEGIES_HPP
#define COH_INVOCATION_STRATEGIES_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/pof/reflect/internal/InvocationStrategy.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

/**
 * InvocationStrategies contains an InvocationStrategy implementations that
 * abstracts the underlying mechanisms to retrieve and set a property's value.
 *
 * @author hr 2011.06.29
 *
 * @since 3.7.1
 */
class COH_EXPORT InvocationStrategies
    : public extends<Object>
    {
    // ----- inner class: MethodInvocationStrategy --------------------------

    public:
        /**
        * A MethodInvocationStrategy uses Methods to dynamically invoke
        * getter and setter methods to retrieve and set property values.
        *
        * @author hr 2011.06.29
        *
        * @since  3.7.1
        */
        class COH_EXPORT MethodInvocationStrategy
            : public class_spec<MethodInvocationStrategy,
                extends<Object>,
                implements<InvocationStrategy> >
            {
            friend class factory<MethodInvocationStrategy>;

            // ----- constructors -------------------------------------------

            protected:
                /**
                * Based on either the getter or setter derive the missing/
                * complimenting accessor from the class provided.
                *
                * @param vMethod  getter or setter
                */
                MethodInvocationStrategy(Method::View vMethod);

                /**
                * Construct with the get and set methods.
                *
                * @param vMethGetter  Object getX() method
                * @param vMethSetter  void setX(Object a)
                */
                MethodInvocationStrategy(Method::View vMethGetter, Method::View vMethSetter);

            // ----- InvocationStrategy interface ---------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder get(Object::View ohContainer) const;

                /**
                * {@inheritDoc}
                */
                virtual void set(Object::Handle ohContainer, Object::Holder ohValue) const;

            // ----- helpers ------------------------------------------------

            protected:
                /**
                * Determine the complement of the provided method in terms
                * of accessors, i.e. if a set method return the
                * corresponding get or is and vice versa.
                *
                * @param vMethod  the method to determine the compliment of
                *
                * @return the method that compliments the method passed
                */
                virtual Method::View getCompliment(Method::View vMethod) const;

                /**
                * Initialize ensures both accessors are not null.
                *
                * @param vMethGetter  the get accessor
                * @param vMethSetter  the set accessor
                */
                virtual void initialize(Method::View vMethGetter, Method::View vMethSetter);

            // ----- data members -------------------------------------------

            private:
                /**
                * The Method this strategy will use to get the value.
                */
                FinalView<Method> f_vMethGetter;

                /**
                * The Method this strategy will use to set a value.
                */
                FinalView<Method> f_vMethSetter;
            };
    };

COH_CLOSE_NAMESPACE5

#endif // COH_INVOCATION_STRATEGIES_HPP

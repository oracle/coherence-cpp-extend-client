/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VISITOR_HPP
#define COH_VISITOR_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

/**
* Visitor pattern description. This pattern implementation is targeted at
* builders that require Class information.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT Visitor
    : public interface_spec<Visitor>
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

    public:
        /**
        * Visit the given builder and optionally mutate it using
        * information contained within the given Class.
        *
        * @param ohBuilder  the builder being visited
        * @param vClz       the Class used to enrich the builder
        */
        virtual void visit(Object::Holder ohBuilder, Class::View vClz) const = 0;

    // ----- inner interface: Recipient -------------------------------------

    public:
        /**
        * A recipient informs a visitor of it's willingness to be visited.
        * 
        * @author hr 2011.06.29
        *
        * @since 3.7.1
        */
        class COH_EXPORT Recipient
            : public interface_spec<Recipient>
            {

            public:
                /**
                * Accept the given visitor.
                *
                * @param vVisitor  Visitor that is requesting to visit this
                *                  recipient
                * @param vClz      the Class that can be used by the visitor
                */
                virtual void accept(Visitor::View vVisitor, Class::View vClz) = 0;
            };
    };

COH_CLOSE_NAMESPACE5

#endif // COH_VISITOR_HPP

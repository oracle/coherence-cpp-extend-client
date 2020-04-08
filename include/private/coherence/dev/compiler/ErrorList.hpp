/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ERROR_LIST_HPP
#define COH_ERROR_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/LinkedList.hpp"



COH_OPEN_NAMESPACE3(coherence,dev,compiler)

using coherence::util::LinkedList;


/**
* Provides a means to accumulate errors.
* Simply overrides toString().
*
* @author js  2008.01.08
*/
class COH_EXPORT ErrorList
    : public class_spec<ErrorList,
        extends<LinkedList> >
    {
    friend class factory<ErrorList>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new empty ErrorList instance
        *
        * @return the new ErrorList
        */
        ErrorList();


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ERROR_LIST_HPP

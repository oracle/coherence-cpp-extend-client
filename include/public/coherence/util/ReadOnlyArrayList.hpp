/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_READ_ONLY_ARRAY_LIST_HPP
#define COH_READ_ONLY_ARRAY_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/ReadOnlyMultiList.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
* Implementation of the Collection Framework interface "List" in a read-
* only fashion on top of an array data structure.
* <p/>
* This class also implements the Set interface, although the contents are
* not checked to determine whether each element is unique. It is the
* responsibility of the user to ensure that the elements are unique if the
* object is used as a Set.
*
* @author mf  2009.08.20
* @since Coherence 3.6
* @see ReadOnlyMultiList
*/
class COH_EXPORT ReadOnlyArrayList
    : public class_spec<ReadOnlyArrayList,
        extends<ReadOnlyMultiList> >
    {
    friend class factory<ReadOnlyArrayList>;

    // ---- constructors ----------------------------------------------------

    protected:
        /**
        * Construct a List from a specified number of items in an array
        * starting at the specified offset.
        *
        * @param vao  a non-NULL array of Objects
        * @param of   an offset of the first item in the array
        * @param c    the number of items to use
        */
        ReadOnlyArrayList(ObjectArray::View vao, size32_t of = 0,
                size32_t c = npos);
    private:
        /**
        * Blocked copy constructor.
        */
        ReadOnlyArrayList(const ReadOnlyArrayList&);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_READ_ONLY_ARRAY_LIST_HPP

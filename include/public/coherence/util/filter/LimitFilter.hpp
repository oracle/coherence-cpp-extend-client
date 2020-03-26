/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LIMIT_FILTER_HPP
#define COH_LIMIT_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PortableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

/**
* Filter which truncates the results of another filter. This filter is a
* mutable object that is modified by the query processor. Clients are
* supposed to hold a reference to this filter and repetitively pass it to
* query methods after setting a desired page context calling
* #setPage, #nextPage(), or #previousPage.
*
* @author djl  2008.04.14
*/
class COH_EXPORT LimitFilter
    : public cloneable_spec<LimitFilter,
        extends<Object>,
        implements<IndexAwareFilter, PortableObject> >
    {
    friend class factory<LimitFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        LimitFilter();

        /**
        * Construct a LimitFilter filter.
        *
        * @param vFilter    the filter whose results this Filter truncates
        * @param cPageSize  the sie of the page
        */
        LimitFilter(Filter::View vFilter, int32_t cPageSize);

        /**
        * Copy constructor.
        */
        LimitFilter(const LimitFilter& that);


    // ----- Entry Filter interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluateEntry(Map::Entry::View vEntry) const;


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluate(Object::View v) const;


    // ----- IndexAwareFilter interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t calculateEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Filter::View applyIndex(Map::View vMapIndexes,
                Set::Handle hSetKeys) const;


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
        * Output a human-readable description of this Object to the given
        * stream.
        *
        * @param out  the stream used to output the description
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the Filter whose results are truncated by this filter.
        *
        * @return the filter whose results are truncated by this filter
        */
        virtual Filter::View getFilter() const;

        /**
        * Obtain the page size (expressed as a number of entries per page).
        *
        * @return the page size
        */
        virtual int32_t getPageSize() const;

        /**
        * Set the page size (expressed as a number of entries per page).
        *
        * @param cPageSize  the page size
        */
        virtual void setPageSize(int32_t cPageSize);

        /**
        * Obtain a current page number (zero-based).
        *
        * @return the page number
        */
        virtual int32_t getPage() const;

        /**
        * Set the page number (zero-based). Setting the page number to zero
        * will reset the filter's state.
        *
        * @param nPage  the page number
        */
        virtual void setPage(int32_t nPage);

        /**
        * Obtain the Comparator used to partition the entry values into
        * pages.
        * 
        * This method is intended to be used only by query processors.
        * Clients should not modify the content of this property.
        *
        * @return the Comparator object
        */
        virtual Comparator::View getComparator() const;

        /**
        * Set the Comparator used to partition the values into pages.
        * 
        * This method is intended to be used only by query processors.
        * Clients should not modify the content of this property.
        *
        * @param vComparator  Comparator object
        */
        virtual void setComparator(Comparator::View vComparator) const;

        /**
        * Obtain the top anchor object, which is the last value object
        * on a previous page.
        * 
        * This method is intended to be used only by query processors.
        * Clients
        * should not modify the content of this property.
        *
        * @return top anchor object
        */
        virtual Object::View getTopAnchor() const;

        /**
        * Set the top anchor object.
        * 
        * This method is intended to be used only by query processors.
        * Clients should not modify the content of this property.
        *
        * @param vAnchor the top anchor object
        */
        virtual void setTopAnchor(Object::View vAnchor) const;

        /**
        * Obtain the bottom anchor object, which is the last value object
        * on the current page.
        * 
        * This method is intended to be used only by query processors.
        * Clients should not modify the content of this property.
        *
        * @return bottom anchor object
        */
        virtual Object::View getBottomAnchor() const;

        /**
        * Set the bottom anchor object.
        * 
        * This method is intended to be used only by query processors.
        * Clients
        * should not modify the content of this property.
        *
        * @param vAnchor  the bottom anchor object
        */
        virtual void setBottomAnchor(Object::View vAnchor) const;

        /**
        * Obtain the cookie object.
        * 
        * This method is intended to be used only by query processors.
        * Clients should not modify the content of this property.
        *
        * @return cookie object
        */
        virtual Object::View getCookie() const;

        /**
        * Set the cookie object.
        *
        * This method is intended to be used only by query processors.
        * Clients should not modify the content of this property.
        *
        * @param vCookie  the cookie object
        */
        virtual void setCookie(Object::View vCookie) const;


    // ----- helpers --------------------------------------------------------

    public:
        /**
        * Switch to the next page.
        */
        virtual void nextPage();

        /**
        * Switch to the previous page.
        */
        virtual void previousPage();

        /**
        * Extract a subset of the specified array to fit the filter's
        * parameters (i.e. page size and page number). If this filter has a
        * comparator, the specified array is presumed to be sorted
        * accordingly.
        * 
        * The returned array is guaranteed to iterate exactly in the same
        * order as the original array.
        *
        * @param haEntry  an original array of entries
        *
        * @return an array of entries extracted accordingly to the filter
        *         parameters
        */
        virtual ObjectArray::Handle extractPage(ObjectArray::Handle haEntry) const;

        /**
        * Extract a subset of the specified iterator to fit the filter's
        * parameters (i.e. page size and page number). The returned array is
        * guaranteed to iterate exactly in the same order as the original
        * iterator.
        *
        * @param hIter  an original entry iterator
        *
        * @return an array of entries extracted accordingly to the filter
        *         parameters
        */
        virtual ObjectArray::Handle extractPage(Iterator::Handle hIter) const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The Filter whose results are truncated by this filter.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The number of entries per page.
        */
        int32_t m_cPageSize;

        /**
        * The page number.
        */
        int32_t m_nPage;

        /**
        * The comparator used to partition the entry values into pages.
        */
        mutable FinalView<Comparator> f_vComparator;

        /**
        * The top anchor object (the last object on a previous page).
        */
        mutable MemberView<Object> m_vAnchorTop;

        /**
        * The bottom anchor object (the last object on the current page).
        */
        mutable MemberView<Object> m_vAnchorBottom;

        /**
        * The cookie object used by the query processors to store a transient
        * state of the request (on a client side).
        */
        mutable MemberView<Object> m_vCookie;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_LIMIT_FILTER_HPP


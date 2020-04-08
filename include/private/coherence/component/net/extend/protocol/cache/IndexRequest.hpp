/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INDEX_REQUEST_HPP
#define COH_INDEX_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/ValueExtractor.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::Comparator;
using coherence::util::ValueExtractor;


/**
* QueryMap::addIndex(ValueExtractor::View vExtractor, bool fOrdered,
* Comparator::View vComparator) and removeIndex(ValueExtractor::View
* vExtractor) Request message.
*
* @author jh  2008.02.29
*/
class COH_EXPORT IndexRequest
    : public class_spec<IndexRequest,
        extends<NamedCacheRequest> >
    {
    friend class factory<IndexRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new IndexRequest instance.
        */
        IndexRequest();


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


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


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return true if an index should be added, false if an index should
        * be removed
        *
        * @return the value of the add flag
        */
        virtual bool isAdd() const;

        /**
        * Configure the add flag.
        *
        * @param fAdd  true if an index should be added, false if an index
        *              should be removed
        */
        virtual void setAdd(bool fAdd);

        /**
        * Return the Comparator associated with this Request.
        *
        * @return the Comparator associated with this Request
        */
        virtual Comparator::View getComparator() const;

        /**
        * Associate the given Comparator with this Request.
        *
        * @param vComparator  the Comparator to associate with this Request
        */
        virtual void setComparator(Comparator::View vComparator);

        /**
        * Return the ValueExtractor associated with this Request.
        *
        * @return the ValueExtractor associated with this Request
        */
        virtual ValueExtractor::View getExtractor() const;

        /**
        * Associate the given ValueExtractor with this Request.
        *
        * @param vExtractor  the ValueExtractor to associate with this
        *                    Request
        */
        virtual void setExtractor(ValueExtractor::View vExtractor);

        /**
        * Return true if the contents of the indexed information should be
        * ordered; false otherwise.
        *
        * @return the value of the ordered flag
        */
        virtual bool isOrdered() const;

        /**
        * Configure the ordered flag.
        *
        * @param fOrdered  true if the contents of the indexed information
        *                  should be ordered; false otherwise
        */
        virtual void setOrdered(bool fOrdered);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 42;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * If true, add an index; otherwise remove it.
        */
        bool m_fAdd;

        /**
        * The Comparator object which imposes an ordering on entries in the
        * indexed Map; or NULL if the entries' values natural ordering should
        * be used.
        */
        FinalView<Comparator> f_vComparator;

        /**
        * The ValueExtractor object that is used to extract an indexable
        * Object from a value stored in the indexed Map.
        */
        FinalView<ValueExtractor> f_vExtractor;

        /**
        * If true, the contents of the indexed information should be ordered.
        */
        bool m_fOrdered;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_INDEX_REQUEST_HPP

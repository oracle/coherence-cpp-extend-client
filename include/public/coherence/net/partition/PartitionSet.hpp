/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PARTITION_SET_HPP
#define COH_PARTITION_SET_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE3(coherence,net,partition)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* PartitionSet is a light-weight data structure that represents a set of
* partitions that are used in parallel processing. This set quite often
* accompanies a result of partial parallel execution and is used to determine
* whether or not the entire set of partitions was successfully processed.
* 
* Note that all PartitionSet operations that take another set as an argument
* assume that both sets have the same partition count.
*
* @since Coherence 3.7.1 
* @author tb  2011.08.12
*/
class COH_EXPORT PartitionSet
    : public class_spec<PartitionSet,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<PartitionSet>;

    // ----- Format definition ----------------------------------------------

    public:
        /**
        * Serialization format indicator.
        */
        typedef enum
            {
            /**
            * Indicates that no partitions are marked; MARKED_NONE requires 
            * no additional data.
            */
            marked_none = 0,

            /**
            * Indicates that a small number of partitions are marked; 
            * followed by stream of packed integers indicating gaps between 
            * each marked partition, terminated with a -1.
            */
            marked_few = 1,
            
            /**
            * Indicates that a large number of partitions are marked; 
            * followed by a sequence of 64-bit values sufficient to represent 
            * the cardinality of the PartitionSet.
            */
            marked_many = 2,
            
            /**
            * Indicates that all partitions are marked; MARKED_ALL requires 
            * no additional data.
            */
            marked_all = 3
            } Format;


    // ----- Constructors ---------------------------------------------------

    public:
        /**
        * Default constructor.
        */
        PartitionSet();


    // ----- pseudo Set operations ------------------------------------------

    public:
        /**
        * Add the specified partition to the set.
        *
        * @param nPartition  the partition to add
        *
        * @return true if the specified partition was actually added as a 
        *         result of this call; false otherwise
        */
        virtual bool add(int32_t nPartition);

       /**
        * Add the specified PartitionSet to this set.
        *
        * @param vPartitions  the PartitionSet to add
        *
        * @return true if all of the partitions were actually added as a 
        *         result of this call; false otherwise
        */
        virtual bool add(PartitionSet::View vPartitions);

        /**
        * Fill the set to contain all the partitions.
        */
        virtual void fill();
        
        /**
        * Return an index of the first marked partition that is greater than 
        * or equal to the specified partition. If no such partition exists 
        * then -1 is returned.
        * 
        * This method could be used to iterate over all marked partitions:
        * <pre>
        * for (int i = ps.next(0); i >= 0; i = ps->next(i+1))
        *     {
        *     // process partition
        *     }
        * </pre>
        *
        * @param nPartition  the partition to start checking from (inclusive)
        *
        * @return the next marked partition, or -1 if no next marked partition
        *         exists in the set
        *
        * @throws IndexOutOfBoundsException if the specified partition is
        *         invalid
        */
        virtual int32_t next(int32_t nPartition) const;
     
 
    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Determine the number of trailing zero bits in the passed long value.
        *
        * @param l  a long value
        *
        * @return the number of trailing zero bits in the value, from 0
        *         (indicating that the least significant bit is set) to 64
        *         (indicating that no bits are set)
        */
        static int32_t getTrailingZeroCount(int64_t l);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;
        
        
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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Total partition count.
        */
        int32_t m_cPartitions;

        /**
        * A bit array representing the partitions, stored as an array of longs.
        */
        FinalHandle< Array<int64_t> > f_halBits;
    
        /**
        * A mask for the last long that indicates what bits get used.
        */
        int64_t m_lTailMask;
    
        /**
        * A cached count of marked partitions; -1 indicates that the value must
        * be recalculated.
        */
        int32_t m_cMarked;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PARTITION_SET_HPP

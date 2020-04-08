/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_PROCESSORS_HPP
#define COH_CACHE_PROCESSORS_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/Map.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE4(coherence,internal,util,processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Map;
using coherence::util::InvocableMap;
using coherence::util::processor::AbstractProcessor;

/*
 * Contains factory methods and entry processor classes that are used to
 * implement functionality exposed via different variants of {@link NamedCache}
 * API.
 *
 * @author as  2015.01.17
 * @author lh  2015.05.05
 * @since 12.2.1
 */
class COH_EXPORT CacheProcessors
    : public class_spec<CacheProcessors>
    {
    friend class factory<CacheProcessors>;

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        CacheProcessors();

    // ---- Factory methods -------------------------------------------------
    public:
        static coherence::util::InvocableMap::EntryProcessor::Handle nop();

        static InvocableMap::EntryProcessor::Handle get();

        static InvocableMap::EntryProcessor::Handle getOrDefault();

        static InvocableMap::EntryProcessor::Handle put(Object::Holder ohValue, int64_t cMillis);

        static InvocableMap::EntryProcessor::Handle putAll(Map::View vMap);

        static InvocableMap::EntryProcessor::Handle putIfAbsent(Object::Holder ohValue);

        static InvocableMap::EntryProcessor::Handle remove();

        static InvocableMap::EntryProcessor::Handle removeBlind();

        static InvocableMap::EntryProcessor::Handle remove(Object::View v);

        static InvocableMap::EntryProcessor::Handle replace(Object::Holder oh);

        static InvocableMap::EntryProcessor::Handle replace(Object::View vOldValue, Object::View vNewValue);

    // ---- Entry Processors --------------------------------------------

    protected:
        /**
         * Abstract base class for entry processors.
         */
        class COH_EXPORT BaseProcessor
            : public abstract_spec<BaseProcessor,
                extends<AbstractProcessor>,
                implements <InvocableMap::EntryProcessor, PortableObject> >
            {
            // ---- PortableObject methods ----------------------------------
            public:
                virtual void readExternal(PofReader::Handle hIn);

                virtual void writeExternal(PofWriter::Handle hOut) const;
            };

    public:
        /**
         * Null entry processor.
         */
        class COH_EXPORT Null
            : public class_spec<Null,
                extends<BaseProcessor> >
            {
            friend class factory<Null>;

            public:
                Object::Holder process(InvocableMap::Entry::Handle hEntry) const;
            };

        /**
         * Get entry processor.
         */
        class COH_EXPORT Get
            : public class_spec<Get,
                extends<BaseProcessor> >
            {
            friend class factory<Get>;

            public:
                Object::Holder process(InvocableMap::Entry::Handle hEntry) const;
            };

        /**
         * GetOrDefault entry processor.
         */
        class COH_EXPORT GetOrDefault
            : public class_spec<GetOrDefault,
                extends<BaseProcessor> >
            {
            friend class factory<GetOrDefault>;

            protected:
                GetOrDefault();

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;
            };

        /**
         * Put entry processor.
         */
        class COH_EXPORT Put
            : public class_spec<Put,
                extends<BaseProcessor> >
            {
            friend class factory<Put>;

            protected:
                Put();

                Put(Object::Holder ohValue, int64_t cMillis);

            // ---- EntryProcessor methods --------------------------

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

                // ---- PortableObject methods --------------------------

                virtual void readExternal(PofReader::Handle hIn);

                virtual void writeExternal(PofWriter::Handle hOut) const;

            // ---- data members ----------------------------------------

            protected:
                FinalHolder<Object> f_ohValue;
                int64_t m_cMillis;
            };

        /**
         * PutAll entry processor.
         */
        class COH_EXPORT PutAll
            : public class_spec<PutAll,
                extends<BaseProcessor> >
            {
            friend class factory<PutAll>;

            protected:
                PutAll();

                PutAll(Map::View vMap);

                // ---- EntryProcessor methods --------------------------

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

                // ---- PortableObject methods --------------------------

                virtual void readExternal(PofReader::Handle hIn);

                virtual void writeExternal(PofWriter::Handle hOut) const;

            // ---- data members ----------------------------------------

            protected:
                FinalView<Map> f_vMap;
            };

        /**
         * PutIfAbsent entry processor.
         */
        class COH_EXPORT PutIfAbsent
            : public class_spec<PutIfAbsent,
                extends<BaseProcessor> >
            {
            friend class factory<PutIfAbsent>;

            protected:
                PutIfAbsent();

                PutIfAbsent(Object::Holder ohValue);

                // ---- EntryProcessor methods --------------------------

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

                // ---- PortableObject methods --------------------------

                virtual void readExternal(PofReader::Handle hIn);

                virtual void writeExternal(PofWriter::Handle hOut) const;

            // ---- data members ----------------------------------------

            protected:
                FinalHolder<Object> f_ohValue;
            };

        /**
         * Remove entry processor.
         */
        class COH_EXPORT Remove
            : public class_spec<Remove,
                extends<BaseProcessor> >
            {
            friend class factory<Remove>;

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;
            };

        /**
         * Remove entry processor.
         */
        class COH_EXPORT RemoveBlind
            : public class_spec<RemoveBlind,
                extends<BaseProcessor> >
            {
            friend class factory<RemoveBlind>;

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;
            };

        /**
         * RemoveValue entry processor.
         */
        class COH_EXPORT RemoveValue
            : public class_spec<RemoveValue,
                extends<BaseProcessor> >
            {
            friend class factory<RemoveValue>;

            protected:
                RemoveValue();

                RemoveValue(Object::View v);

                // ---- EntryProcessor methods --------------------------

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

                // ---- PortableObject methods --------------------------

                virtual void readExternal(PofReader::Handle hIn);

                virtual void writeExternal(PofWriter::Handle hOut) const;

            // ---- data members --------------------------------------------

            protected:
                FinalView<Object> f_ov;
            };

        /**
         * Replace entry processor.
         */
        class COH_EXPORT Replace
            : public class_spec<Replace,
                extends<BaseProcessor> >
            {
            friend class factory<Replace>;

            protected:
                Replace();

                Replace(Object::Holder oh);

                // ---- EntryProcessor methods --------------------------

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

            // ---- PortableObject methods ------------------------------

                virtual void readExternal(PofReader::Handle hIn);

                virtual void writeExternal(PofWriter::Handle hOut) const;

            // ---- data members ----------------------------------------

            protected:
                FinalHolder<Object> f_ohValue;
            };

        /**
         * ReplaceValue entry processor.
         */
        class COH_EXPORT ReplaceValue
            : public class_spec<ReplaceValue,
                extends<BaseProcessor> >
            {
            friend class factory<ReplaceValue>;

            protected:
                ReplaceValue();

                ReplaceValue(Object::View vOldValue, Object::View vNewValue);

                // ---- EntryProcessor methods --------------------------

            public:
                virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

                // ---- PortableObject methods --------------------------

                virtual void readExternal(PofReader::Handle hIn);

                virtual void writeExternal(PofWriter::Handle hOut) const;

            // ---- data members ----------------------------------------

            protected:
                FinalView<Object> f_vOldValue;
                FinalView<Object> f_vNewValue;
            };
    };

COH_CLOSE_NAMESPACE4

#endif // COH_CACHE_PROCESSORS_HPP

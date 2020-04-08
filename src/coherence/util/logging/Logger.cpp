/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/logging/Logger.hpp"

#include "private/coherence/util/Date.hpp"
#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/util/logging/LogOutput.hpp"
#include "private/coherence/util/logging/Standard.hpp"
#include "private/coherence/util/StringHelper.hpp"

#include <sstream>
#include <ctime>

COH_OPEN_NAMESPACE3(coherence,util,logging)

using coherence::run::xml::SimpleElement;
using coherence::util::Date;
using coherence::util::StringHelper;


// ----- local file helpers -------------------------------------------------

namespace
    {
    /**
    * Return the log level text for a given log level.
    *
    * @param the log level
    *
    * @param the log level text
    */
    String::View getLevelText(int32_t nLevel)
        {
        // force initialization of all levels
        static FinalView<String> vsInternal(System::common(),
                String::create("Internal"));
        static FinalView<String> vsError(System::common(),
                String::create("Error"));
        static FinalView<String> vsWarning(System::common(),
                String::create("Warning"));
        static FinalView<String> vsInfo(System::common(),
                String::create("Info"));
        static FinalView<String> vsDebug4(System::common(),
                String::create("D4"));
        static FinalView<String> vsDebug5(System::common(),
                String::create("D5"));
        static FinalView<String> vsDebug6(System::common(),
                String::create("D6"));
        static FinalView<String> vsDebug7(System::common(),
                String::create("D7"));
        static FinalView<String> vsDebug8(System::common(),
                String::create("D8"));
        static FinalView<String> vsDebug9(System::common(),
                String::create("D9"));

        // return appropriate level
        switch (nLevel)
            {
            case Logger::level_internal:
                return vsInternal;

            case Logger::level_error:
                return vsError;

            case Logger::level_warning:
                return vsWarning;

            case Logger::level_info:
                return vsInfo;

            case Logger::level_d4:
                return vsDebug4;

            case Logger::level_d5:
                return vsDebug5;

            case Logger::level_d6:
                return vsDebug6;

            case Logger::level_d7:
                return vsDebug7;

            case Logger::level_d8:
                return vsDebug8;

            case Logger::level_d9:
            default:
                return vsDebug9;
            }
        }
    COH_STATIC_INIT(getLevelText(0));

    /**
    * Return the default time pattern.
    *
    * @return the default time pattern
    */
    String::View getDefaultTimePattern()
        {
        static FinalView<String> vsPattern(System::common(),
                String::create("yyyy'-'MM'-'dd HH':'mm':'ss'.'fff"));
        return vsPattern;
        }
    COH_STATIC_INIT(getDefaultTimePattern());

    Object::Handle getInstantiationLock()
        {
        static FinalHandle<Object> hLock(System::common(),
                Object::create());
        return hLock;
        }
    COH_STATIC_INIT(getInstantiationLock());

    Integer32::View boxLogLevel(int32_t nLevel)
        {
        static FinalView<Integer32> vLEVEL_NONE(System::common(),
                Integer32::valueOf(Logger::level_none));
        static FinalView<Integer32> vLEVEL_INTERNAL(System::common(),
                Integer32::valueOf(Logger::level_internal));
        static FinalView<Integer32> vLEVEL_ERROR(System::common(),
                Integer32::valueOf(Logger::level_error));
        static FinalView<Integer32> vLEVEL_WARNING(System::common(),
                Integer32::valueOf(Logger::level_warning));
        static FinalView<Integer32> vLEVEL_INFO(System::common(),
                Integer32::valueOf(Logger::level_info));
        static FinalView<Integer32> vLEVEL_D4(System::common(),
                Integer32::valueOf(Logger::level_d4));
        static FinalView<Integer32> vLEVEL_D5(System::common(),
                Integer32::valueOf(Logger::level_d5));
        static FinalView<Integer32> vLEVEL_D6(System::common(),
                Integer32::valueOf(Logger::level_d6));
        static FinalView<Integer32> vLEVEL_D7(System::common(),
                Integer32::valueOf(Logger::level_d7));
        static FinalView<Integer32> vLEVEL_D8(System::common(),
                Integer32::valueOf(Logger::level_d8));
        static FinalView<Integer32> vLEVEL_D9(System::common(),
                Integer32::valueOf(Logger::level_d9));
        static FinalView<Integer32> vLEVEL_ALL(System::common(),
                Integer32::valueOf(Logger::level_all));

        switch(nLevel)
            {
            case Logger::level_none:
                return vLEVEL_NONE;
            case Logger::level_internal:
                return vLEVEL_INTERNAL;
            case Logger::level_error:
                return vLEVEL_ERROR;
            case Logger::level_warning:
                return vLEVEL_WARNING;
            case Logger::level_info:
                return vLEVEL_INFO;
            case Logger::level_d4:
                return vLEVEL_D4;
            case Logger::level_d5:
                return vLEVEL_D5;
            case Logger::level_d6:
                return vLEVEL_D6;
            case Logger::level_d7:
                return vLEVEL_D7;
            case Logger::level_d8:
                return vLEVEL_D8;
            case Logger::level_d9:
                return vLEVEL_D9;
            case Logger::level_all:
                return vLEVEL_ALL;
            default:
                return nLevel < Logger::level_none ? vLEVEL_NONE : vLEVEL_ALL;
            }
        }
    COH_STATIC_INIT(boxLogLevel(0));
    } // end of anonymous namespace


// ----- static methods -----------------------------------------------------

static int32_t coh_singleton_logger_level = Logger::level_all;
bool Logger::isEnabledSingleton(int32_t nLevel)
    {
    return nLevel <= coh_singleton_logger_level;
    }

Logger::Handle Logger::getLogger()
    {
    // logger must be protected by a MemberHandle (thread-safe handle) as
    // it is possible that the reference will change over time, so readers
    // must be protected from writers, for this the InstantiationLock is used
    // as the "parent" guardian object.
    static MemberHandle<Logger> hLogger(*getInstantiationLock(),
            Logger::create());
    static bool                 fFirstRun = true;

    if (fFirstRun)
        {
        COH_SYNCHRONIZED(getInstantiationLock())
            {
            if (fFirstRun)
                {
                // start the initial logger. CacheFactory will stop it and a new configured
                // one will be created in its place when CacheFactory's anonymous
                // initLogger() is called.
                hLogger->start();

                coh_singleton_logger_level = hLogger->getLevel();
                fFirstRun = false;
                }
            }
        }

    if (NULL == hLogger)
        {
        // this can only happen during shutdown or unload of our library
        // static destruction has already run
        return hLogger;
        }
    else if (hLogger->getDaemonState() == daemon_exiting ||
             hLogger->getDaemonState() == daemon_exited)
        {
        COH_SYNCHRONIZED(getInstantiationLock())
            {
            COH_SYNCHRONIZED(hLogger)
                {
                Daemon::DaemonStatus status = hLogger->getDaemonState();
                if (status == daemon_exiting || status == daemon_exited)
                    {
                    hLogger = Logger::create();
                    coh_singleton_logger_level = hLogger->getLevel();
                    }
                }
            }
        }
    return hLogger;
    }
COH_STATIC_INIT(Logger::getLogger());


// ----- constructors -------------------------------------------------------

Logger::Logger()
    : super(),
      m_vsDestination(self(), getDefaultDestination()),
      m_vsFormat(self(), getDefaultFormat()),
      m_nLogLevel(getDefaultLevel()),
      m_vsTimePattern(self(), getDefaultTimePattern()),
      m_cCharLimit(getDefaultLimit()),
      m_hLogOutput(self(), Standard::create()),
      m_vasParameters(self(), getDefaultParameters()),
      m_vsProduct(self(), String::create(COH_SYMB_TO_STRING(COH_PRODUCT))),
      m_vsVersion(self(), String::create(COH_SYMB_TO_STRING(COH_VERSION))),
      m_vsEdition(self(), "n/a")
    {
    }


// ----- Logger interface ---------------------------------------------------

void Logger::init()
    {
    }

String::View Logger::getDefaultDestination()
    {
    // Note this is only called once so the cost of a new string is minimal
    return "Error";
    }

String::View Logger::getDefaultFormat()
    {
    // Note this is only called once
    return "{date}/{uptime} {product} {version} <{level}> (thread={thread}): {text}";
    }

int32_t Logger::getDefaultLevel()
    {
    // pulling from a system property here allows customization of the log
    // level before configuration via CacheFactory::configure, we re-use the
    // standard system property name
    // Could possibly remove the system property reference if Logger is
    // changed to always be accessed through CacheFactory
    return Integer32::parse(
            System::getProperty("coherence.log.level", "5"));
    }

int32_t Logger::getDefaultLimit()
    {
    // pulling from a system property here allows customization of the log
    // limit before configuration via CacheFactory::configure, we re-use the
    // standard system property name
    // Could possibly remove the system property reference if Logger is
    // changed to always be accessed through CacheFactory
    return Integer32::parse(
            System::getProperty("coherence.log.limit", "1048576"));
    }

namespace
    {
    ObjectArray::View _createDefaultParameters()
        {
        ObjectArray::Handle hasParams = ObjectArray::create(8);
        hasParams[0] = String::create("{date}");
        hasParams[1] = String::create("{uptime}");
        hasParams[2] = String::create("{level}");
        hasParams[3] = String::create("{thread}");
        hasParams[4] = String::create("{e}");
        hasParams[5] = String::create("{text}");
        hasParams[6] = String::create("{product}");
        hasParams[7] = String::create("{version}");

        return hasParams;
        }
    }

ObjectArray::View Logger::getDefaultParameters()
    {
    static FinalView<ObjectArray> vasParams(System::common(),
            _createDefaultParameters());
    return vasParams;
    }
COH_STATIC_INIT(Logger::getDefaultParameters());

String::View Logger::getFormat() const
    {
    String::View vs = m_vsFormat;
    return vs == NULL
            ? getDefaultFormat() // pre-config
            : vs;
    }

void Logger::setFormat(String::View sFormat)
    {
    m_vsFormat = sFormat;
    }

int32_t Logger::getLevel() const
    {
    return m_nLogLevel;
    }

void Logger::setLevel(int32_t nLevel)
    {
    m_nLogLevel = nLevel;
    if (this == getLogger())
        {
        coh_singleton_logger_level = nLevel;
        }
    }

int32_t Logger::getLimit() const
    {
    return m_cCharLimit;
    }

void Logger::setLimit(int32_t iLimit)
    {
    m_cCharLimit = iLimit;
    }

ObjectArray::View Logger::getParameters() const
    {
    return m_vasParameters;
    }

void Logger::setParameters(ObjectArray::View vasParameters)
    {
    m_vasParameters = vasParameters;
    }

String::View Logger::getDestination() const
    {
    return m_vsDestination;
    }

void Logger::setDestination(String::View vsDestination)
    {
    m_vsDestination = vsDestination;
    }

String::View Logger::getProduct() const
    {
    return m_vsProduct;
    }

void Logger::setProduct(String::View vsProduct)
    {
    m_vsProduct = vsProduct;
    }

String::View Logger::getVersion() const
    {
    return m_vsVersion;
    }

void Logger::setVersion(String::View vsVersion)
    {
    m_vsVersion = vsVersion;
    }

String::View Logger::getEdition() const
    {
    String::View vs = m_vsEdition;
    return vs == NULL
            ? (String::View) String::create("n/a") // pre-config
            : vs;
    }

void Logger::setEdition(String::View vsEdition)
    {
    m_vsEdition = vsEdition;
    }

bool Logger::isEnabled(int32_t nLevel) const
    {
    return nLevel <= m_nLogLevel;
    }

void Logger::configure(OperationalContext::View vContext)
    {
    setEdition(vContext->getEditionName());
    int32_t nLevel = vContext->getLogLevel();

    // validate the log level
    if (nLevel < level_none)
        {
        nLevel = level_none;
        }
    if (nLevel > level_all)
        {
        nLevel = level_all;
        }
    setLevel(nLevel);

    int32_t nLimit = vContext->getLogCharacterLimit();
    if (nLimit == 0)
        {
        nLimit = getDefaultLimit();
        }

    setLimit(nLimit <= 0 ? 0x7FFFFFFF : nLimit);

    String::View vsFormat = vContext->getLogMessageFormat();
    if (vsFormat == NULL || vsFormat->length() == 0)
        {
        vsFormat = getDefaultFormat();
        }

    setFormat(vsFormat);

    LogOutput::Handle hOldLogOut = m_hLogOutput;
    LogOutput::Handle hNewLogOut = Standard::create();
    try
        {
        hNewLogOut->configure(vContext);
        }
    catch (Exception::View tEx)
        {
        hNewLogOut->log(level_error, tEx,
                "Error configuring logger; using default settings.");
        }
    m_hLogOutput = hNewLogOut;
    hOldLogOut->close();
    }

String::View Logger::formatMessage(ObjectArray::View message) const
    {
    if (message->length < 6)
        {
        COH_THROW (IllegalArgumentException::create("Invalid message format"));
        }

    int32_t nLevel = cast<Integer32::View>(message[2])->getValue();
    if (nLevel == level_internal)
        {
        return cast<String::View>(message[5]);
        }

    String::View      vsText        = m_vsFormat;
    ObjectArray::View vasParameters = m_vasParameters;
    for (size32_t i = 0; i < vasParameters->length; ++i)
        {
        String::View vsParamName = cast<String::View>(vasParameters[i]);
        Object::View voValue     = i < message->length ? message[i]
                                                       : (Object::View) NULL;

        vsText = StringHelper::replace(vsText, vsParamName,
                                       formatParameter(vsParamName, voValue));
        }

    return vsText;
    }

void Logger::log(int32_t nLevel, String::View vsMessage,
                 ObjectArray::View vaoParams)
    {
    log(nLevel, (Exception::View) NULL, vsMessage, vaoParams);
    }

void Logger::log(int32_t nLevel, Exception::View vEx,
                 ObjectArray::View vaoParams)
    {
    String::View vsNull;
    log(nLevel, vEx, vsNull, vaoParams);
    }

void Logger::log(int32_t nLevel, Exception::View vEx, String::View vsMessage,
                 ObjectArray::View vaoParams)
    {
    nLevel = nLevel < level_none ? level_none : nLevel;
    nLevel = nLevel > level_all  ? level_all  : nLevel;

    if (nLevel <= m_nLogLevel)
        {
        ObjectArray::Handle hoaMsg =
                createMessage(nLevel, vEx, vsMessage, vaoParams);

        getQueue()->add(hoaMsg);
        }
    }

void Logger::shutdown()
    {
    Thread::Handle hThread;
    COH_SYNCHRONIZED(this)
        {
        Daemon::DaemonStatus status = getDaemonState();
        hThread = getThread();
        if (status != daemon_exiting && status != daemon_exited)
            {
            // No new messages will be placed on queue after setting state
            // to daemon_exiting and when queue is emptied, deamon thread
            // will terminate.
            setExiting();
            // notify onWait()
            setNotification(true);
            }
        }
    if (hThread != NULL && hThread != Thread::currentThread())
        {
        hThread->join();
        }
    }

void Logger::onExit()
    {
    if (m_hLogOutput != NULL)
        {
        m_hLogOutput->close();
        m_hLogOutput = NULL;
        }
    }

LogOutput::Handle Logger::getLogOutput()
    {
    return m_hLogOutput;
    }

void Logger::setLogOutput(LogOutput::Handle log)
    {
    m_hLogOutput = log;
    }

ObjectArray::Handle Logger::createMessage(int32_t nLevel,
        Exception::View vEx, String::View vsMessage,
        ObjectArray::View vaoParams) const
    {
    size32_t countParam = (NULL == vaoParams ? 0 : vaoParams->length);

    ObjectArray::Handle haovMessage = ObjectArray::create(6 + countParam);

    haovMessage[0] = Date::create();
    haovMessage[1] = Integer64::create(System::upTimeMillis());
    haovMessage[2] = boxLogLevel(nLevel);
    haovMessage[3] = Thread::currentThread();
    haovMessage[4] = vEx;
    haovMessage[5] = vsMessage;

    if (countParam > 0)
        {
        for (size32_t i = 0; i < countParam; ++i)
            {
            size32_t messageIdx = i + 5; // +5 because we've already added the 5 defaults above
            haovMessage[messageIdx] = vaoParams[i];
            }
        }

    return haovMessage;
    }

String::View Logger::formatParameter(String::View name, Object::View value) const
    {
    String::View vsParam;

    if (NULL != name && name->length() > 2)
        {
        const char ch = name->getCString()[1]; //Get the 2nd char in the array
        switch(ch)
            {
            case 'd':
                // {date}
                if (name->equals("{date}"))
                    {
                    vsParam = COH_TO_STRING(value);
                    }
                break;

            case 'u':
                // {uptime}
                if (name->equals("{uptime}"))
                    {
                    int64_t cMillisUp = cast<Integer64::View>(value)->getValue();
                    int64_t cSec      = cMillisUp / 1000;
                    int64_t cMillis   = cMillisUp % 1000;

                    if (cMillis < 10)
                        {
                        vsParam = COH_TO_STRING(cSec << ".00" << cMillis);
                        }
                    else if (cMillis < 100)
                        {
                        vsParam = COH_TO_STRING(cSec << ".0" << cMillis);
                        }
                    else
                        {
                        vsParam = COH_TO_STRING(cSec << "." << cMillis);
                        }
                    }
                break;

            case 'l':
                // {level}
                if (name->equals("{level}"))
                    {
                    Integer32::View vIval = cast<Integer32::View>(value);
                    if (NULL != vIval)
                        {
                        vsParam = getLevelText(vIval->getValue());
                        }
                    }
                break;

            case 'p':
                // {product}
                if (name->equals("{product}"))
                    {
                    String::Handle vsText = COH_TO_STRING(m_vsProduct << " "
                            << m_vsEdition);
                    if (NULL != vsText && vsText->length() > 0)
                        {
                        vsParam = vsText;
                        }
                    }
                break;

            case 't':
                // {thread}
                if (name->equals("{thread}"))
                    {
                    vsParam = cast<Thread::View>(value)->getName();
                    }
                // {text}
                else if (name->equals("{text}"))
                    {
                    String::View vsText = cast<String::View>(value);
                    vsParam = NULL == vsText
                            ? (String::View) StringHelper::getEmptyString()
                            : vsText;
                    }
                break;

            case 'v':
                // {version}
                if (name->equals("{version}"))
                    {
                    String::View vsVersion = m_vsVersion;
                    if (NULL != vsVersion)
                        {
                        vsParam = vsVersion;
                        }
                    }
                break;
            }
        }

    return vsParam;
    }

void Logger::onLog()
    {
    }

void Logger::onNotify()
    {
    const int32_t MAX_TOTAL      = getLimit();
    int32_t       cchTotal       = 0;
    bool          fTruncate      = false;
    int32_t       cTruncate      = 0;
    int32_t       cTruncateLines = 0;
    int32_t       cchTruncate    = 0;
    bool          fDone          = false;

    do
        {
        ObjectArray::Handle haoMessage = cast<ObjectArray::Handle>
                (getQueue()->removeNoWait());

        // check for end of queue; if any messages have been discarded,
        // report the number and size
        if (NULL == haoMessage)
            {
            if (fTruncate && cTruncate > 0)
                {
                haoMessage = ObjectArray::create(6);
                haoMessage[0] = Date::create();
                haoMessage[1] = Integer64::create(System::upTimeMillis());
                haoMessage[2] = boxLogLevel(level_warning);
                haoMessage[3] = Thread::currentThread();
                haoMessage[4] = NULL;
                haoMessage[5] = COH_TO_STRING(
                        "Asynchronous logging character limit exceeded; " <<
                        "discarding " << cTruncate << " log messages " <<
                        "(lines=" << cTruncateLines << ", chars=" <<
                        cchTruncate << ")");
                fDone = true;
                }
            else
                {
                // Queue is empty, exit loop.
                // If state == daemon_exiting, thread will terminate
                break;
                }
            }

        int32_t logLevel = cast<Integer32::View>
                (haoMessage[2])->getInt32Value();
        if (logLevel > getLevel())
            {
            // log level must have been changed after start
            continue;
            }

        String::View vsText = formatMessage(haoMessage);
        String::View vsTextSafe = NULL == vsText ?
                (String::View) StringHelper::getEmptyString() : vsText;
        Exception::View vException = cast<Exception::View>
                (haoMessage[4]);
        String::View vsException = NULL == vException ?
                (String::View) StringHelper::getEmptyString() :
                vException->getDescription();

        if (NULL == vsTextSafe)
            {
            vsTextSafe = StringHelper::getEmptyString();
            }

        cchTotal += vsTextSafe->length() + vsException->length();
        if (fTruncate && !fDone)
            {
            cTruncate      += 1;
            cTruncateLines += StringHelper::split(vsTextSafe, "\n")->length;
            cTruncateLines += StringHelper::split(vsException, "\n")->length;
            cchTruncate    += vsTextSafe->length();
            cchTruncate    += vsException->length();
            }
        else
            {
            if (cchTotal > MAX_TOTAL)
                {
                fTruncate = true;
                }

            onLog();
            LogOutput::Handle hOutput = getLogOutput();
            if (NULL == hOutput)
                {
                std::cerr << vsTextSafe << std::endl;
                if (vException != NULL)
                    {
                    std::cerr << vException << std::endl;
                    }
                }
            else if (NULL == vException)
                {
                hOutput->log(logLevel, vsTextSafe);
                }
            else if (NULL == vsText)
                {
                hOutput->log(logLevel, vException);
                }
            else
                {
                hOutput->log(logLevel, vException, vsTextSafe);
                }
            }
        }
    while (!fDone);
    }

COH_CLOSE_NAMESPACE3 //COH_OPEN_NAMESPACE3(coherence,util,logging)

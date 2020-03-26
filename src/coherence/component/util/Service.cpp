/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/Service.hpp"

#include "coherence/io/ConfigurableSerializerFactory.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ServiceEvent.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::util::QueueProcessor;
using coherence::io::ConfigurableSerializerFactory;
using coherence::io::pof::SystemPofContext;
using coherence::run::xml::XmlElement;
using coherence::run::xml::XmlHelper;
using coherence::util::Event;
using coherence::util::EventListener;
using coherence::util::Listeners;
using coherence::util::ServiceEvent;
using coherence::util::StringHelper;


// ----- constructor --------------------------------------------------------

Service::Service()
    : m_fAcceptingClients(false), f_hEventDispatcher(self()),
      f_hEventListeners(self(), Listeners::create()),
      f_vSerializer(self()), f_vSerializerFactory(self()),
      f_vServiceConfig(self()),
      f_vsServiceName(self()), m_nServiceState(service_initial),
      m_lStatsCpu(0), m_lStatsReceived(0), m_lStatsReset(0),
      f_vContext(self())
    {
    }


// ----- Nested class: DispatchEvent ----------------------------------------

// ----- constructor ---------------------------------------------------

Service::DispatchEvent::DispatchEvent()
    : f_vEvent(self()), f_hListeners(self())
    {
    }

// ----- DispatchEvent interface ---------------------------------------

Event::View Service::DispatchEvent::getEvent() const
    {
    return f_vEvent;
    }

void Service::DispatchEvent::setEvent(Event::View vEvent)
    {
    initialize(f_vEvent, vEvent);
    }

Listeners::View Service::DispatchEvent::getListeners() const
    {
    return f_hListeners;
    }

Listeners::Handle Service::DispatchEvent::getListeners()
    {
    return f_hListeners;
    }

String::View Service::DispatchEvent::getName() const
    {
    return Class::getSimpleClassName(this);
    }

void Service::DispatchEvent::setListeners(Listeners::Handle hListeners)
    {
    initialize(f_hListeners, hListeners);
    }

// ----- DispatchEvent: Runnable interface ------------------------------

void Service::DispatchEvent::run()
    {
    cast<ServiceEvent::View>(getEvent())->dispatch(getListeners());
    }

// ----- Describable interface ------------------------------------------

String::View Service::DispatchEvent::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << getName() << ": " << getEvent());
    }


// ----- Nested class: EventDispatcher --------------------------------------

// ----- constructor ----------------------------------------------------

Service::EventDispatcher::EventDispatcher(Service::View vService)
    : m_cCloggedCount(1024), m_cCloggedDelay(32),
      m_fDispatching(self(), false), m_wvService(self(), vService)
    {
    }

// ----- EventDispatcher interface --------------------------------------

size32_t Service::EventDispatcher::getCloggedCount() const
    {
    return m_cCloggedCount;
    }

int32_t Service::EventDispatcher::getCloggedDelay() const
    {
    return m_cCloggedDelay;
    }

bool Service::EventDispatcher::isDispatching() const
    {
    return m_fDispatching;
    }

void Service::EventDispatcher::setCloggedCount(size32_t cMaxEvents)
    {
    m_cCloggedCount = cMaxEvents;
    }

void Service::EventDispatcher::setCloggedDelay(int32_t cMillis)
    {
    m_cCloggedDelay = cMillis;
    }

void Service::EventDispatcher::setDispatching(bool fDispatching)
    {
    m_fDispatching = fDispatching;
    }

void Service::EventDispatcher::drainOverflow()
    {
    Service::View vService = m_wvService;
    if (!(vService->isServiceThread()))
        {
        // slow down too agressive clients to prevent memory overflow
        size32_t  cMaxEvents   = getCloggedCount();
        int32_t cPauseMillis = getCloggedDelay();
        while (isStarted())
            {
            size32_t cEvents = getQueue()->size();
            if (cEvents < cMaxEvents || cMaxEvents <= 0 || !sleep(cPauseMillis))
                {
                break;
                }
            }
        }
    }

void Service::EventDispatcher::drainQueue()
    {
    // the queue cannot be drained by service threads
    if (!m_wvService->isServiceThread())
        {
        coherence::util::Queue::View hQueue = getQueue();

        // wait for all outstanding tasks to complete
        while (hQueue->size() > 0 || isDispatching())
            {
            Thread::currentThread()->sleep(1);
            }
        }
    }

Queue::Handle Service::EventDispatcher::instantiateQueue() const
    {
    return EventQueue::create(m_wvService);
    }

// ----- Daemon interface -----------------------------------------------

String::View Service::EventDispatcher::getThreadName() const
    {
    return COH_TO_STRING(m_wvService->getThreadName() << ":"
            << QueueProcessor::getThreadName());
    }

int64_t Service::getWaitMillis() const
    {
    /*
    DaemonPool pool = getDaemonPool();
    long cWait1 = super.getWaitMillis();
    long cWait2 = pool.isStarted() ? Math.max(1L,
        pool.getNextCheckMillis() - Base.getSafeTimeMillis()) : 0L;
    return cWait1 <= 0L ? cWait2 : Math.min(cWait1, cWait2);
    */
    return Daemon::getWaitMillis();
    }

void Service::EventDispatcher::onException(Exception::Holder ohe)
    {
    if (!isExiting())
        {
        COH_LOG("The following exception was caught by the event dispatcher:", 1);
        COH_LOGEX(ohe, 1);
        COH_LOG("(The service event thread has logged the exception and is continuing.)", 1);
        }
    }

void Service::EventDispatcher::onExit()
    {
    // drain the queue
    onNotify();
    QueueProcessor::onExit();
    }

void Service::EventDispatcher::onNotify()
    {
    QueueProcessor::onNotify();

    coherence::util::Queue::Handle hQueue = getQueue();
    Runnable::Handle hTask;

    setDispatching(true);

    struct onNotifyFinally
        {
        onNotifyFinally(EventDispatcher::Handle hDispatcher)
        : hThis(hDispatcher)
            {
            }

        ~onNotifyFinally()
            {
            hThis->setDispatching(false);
            }

        EventDispatcher::Handle hThis;
        } finally(this);
    try
        {
        while (true)
            {
            hTask = cast<Runnable::Handle>(hQueue->removeNoWait());
            if (NULL == hTask)
                {
                break;
                }

            hTask->run();
            }
        }
    catch (Exception::View e)
        {
        if (m_wvService->isRunning())
            {
            COH_LOG("An exception occurred while dispatching the following event:\n"
                << hTask, 1);
            onException(e);
            }
        }
    }

// ----- Object interface -----------------------------------------------

void Service::EventDispatcher::onInit()
    {
    try
        {
        String::View vsMaxEvents = System::getProperty("coherence.events.limit");
        String::View vsDelay     = System::getProperty("coherence.events.delay");

        if (NULL != vsMaxEvents)
            {
            setCloggedCount(Integer32::parse(vsMaxEvents));
            }
        if (NULL != vsDelay)
            {
            setCloggedDelay(Integer32::parse(vsDelay));
            }
        }
    catch (Exception::View) {}

    super::onInit();
    }


// ----- nested class EventQueue  -------------------------------------------
// ----- constructor ----------------------------------------------------

Service::EventDispatcher::EventQueue::EventQueue(Service::View vService)
    : m_wvService(self(), vService)
    {
    }

// ----- Queue interface ------------------------------------------------

bool Service::EventDispatcher::EventQueue::add(Object::Holder oh)
    {
    switch (m_wvService->getServiceState())
        {
        case service_starting:
        case service_started:
        case service_stopping:
        case service_stopped:
            return coherence::util::DualQueue::add(cast<Runnable::Handle>(oh));

        default:
            COH_THROW (IllegalStateException::create());
        }
    }


// ----- Service interface --------------------------------------------------

void Service::addServiceListener(ServiceListener::Handle hListener)
    {
    addEventListener(hListener);
    }

int64_t Service::adjustTimeout(int64_t lDefaultTimeout, int64_t lContextTimeout)
    {
    static int64_t TIMEOUT_DEFAULT = 0;
    static int64_t TIMEOUT_NONE    = int64_t(-1);

    return lContextTimeout == TIMEOUT_DEFAULT ? lDefaultTimeout :
            lContextTimeout == TIMEOUT_NONE ? 0 : lContextTimeout;
    }

void Service::configure(XmlElement::View vXml)
    {
    COH_SYNCHRONIZED(this)
        {
        if (getServiceState() > service_initial)
            {
            COH_THROW (IllegalStateException::create());
            }

        if (NULL != vXml)
            {
            XmlElement::View vXmlSerializer = vXml->getElement("serializer");
            if (vXmlSerializer != NULL && !XmlHelper::isEmpty(vXmlSerializer))
                {
                if (vXmlSerializer->isEmpty())
                    {
                    ConfigurableSerializerFactory::Handle hFactory = ConfigurableSerializerFactory::create();
                    hFactory->setConfig(vXmlSerializer);
                    setSerializerFactory(hFactory);
                    }
                else
                    {
                    SerializerFactory::Handle hFactory;
                    String::View vsName         = vXmlSerializer->getString();
                    Map::View    vMapSerializer = getOperationalContext()->getSerializerMap();

                    // The <serializer> element is _not_ empty; it contains
                    // a string value (e.g. <serializer>pof</serializer>).
                    // The serializer map should contain a corresponding serializer.
                    hFactory = cast<SerializerFactory::Handle> (vMapSerializer->get(vsName));
                    if (hFactory == NULL)
                        {
                        COH_THROW_STREAM(IllegalArgumentException, "Serializer name \""
                                << vsName << "\" is undefined:\n"
                                << vXmlSerializer);
                        }
                    setSerializerFactory(hFactory);
                    }
                }
            }
        }
    }

void Service::dispatchServiceEvent(ServiceEvent::Id nEvent)
    {
    Listeners::Handle hListeners = getEventListeners();
    if (!hListeners->isEmpty())
        {
        dispatchEvent(ServiceEvent::create(this, nEvent), hListeners);
        }
    }

void Service::drainEvents()
    {
    EventDispatcher::Handle hDaemon = getEventDispatcher();
    if (NULL != hDaemon)
        {
        hDaemon->drainQueue();
        }
    }

Service::EventDispatcher::Handle Service::ensureEventDispatcher()
    {
    EventDispatcher::Handle hDispatcher = getEventDispatcher();

    if (!isExiting())
        {
        hDispatcher->start();
        }

    return hDispatcher;
    }

Serializer::View Service::ensureSerializer()
    {
    Serializer::View vSerializer = getSerializer();
    if (vSerializer == NULL)
        {
        vSerializer = instantiateSerializer(SystemClassLoader::getInstance());
        setSerializer(vSerializer);
        }
    return vSerializer;
    }

Serializer::View Service::instantiateSerializer(ClassLoader::View vLoader) const
    {
    SerializerFactory::View vFactory = getSerializerFactory();
    return vFactory == NULL
            ? cast<Serializer::View>(SystemPofContext::getInstance())
            : vFactory->createSerializer(vLoader);
    }

String::View Service::ensureStringValue(XmlElement::View vXmlConfig,
                String::View vsName, String::View vsDefault)
    {
    String::View vsValue = vXmlConfig->getSafeElement(vsName)->getString(vsDefault);
    if (NULL == vsValue || vsValue->length() == 0)
        {
        COH_THROW_STREAM(IllegalArgumentException, "the required \"" << vsName
                << "\" configuration element is missing or empty");
        }

    return vsValue;
    }

String::View Service::formatServiceStateName(State nState) const
    {
    switch (nState)
        {
        case service_initial:
            return "service_initial";
        case service_starting:
            return "service_starting";
        case service_started:
            return "service_started";
        case service_stopping:
            return "service_stopping";
        case service_stopped:
            return "service_stopped";
        default:
            return "<unknown>";
        }
    }

String::View Service::formatStats() const
    {
    int64_t   cCpu   = getStatsCpu();
    int64_t   cTotal = System::currentTimeMillis() - getStatsReset();
    int64_t   cMsgs  = getStatsReceived();
    float64_t dCpu   = cTotal == 0L ? 0.0 : ((float64_t) cCpu)/((float64_t) cTotal);
    float64_t dThru  = cCpu == 0L ? 0.0 : ((float64_t) cMsgs*1000)/((float64_t) cCpu);

    // round rates
    dCpu = ((int32_t) (dCpu * 1000)) / 10; // percentage


    return COH_TO_STRING("Cpu=" << cCpu << "ms (" << dCpu << "%), Messages=" << cMsgs <<
        ", Throughput=" << (float64_t)dThru << "msg/sec");
    }

bool Service::isRunning() const
    {
    return getServiceState() == service_started && !isExiting();
    }

bool Service::isServiceThread() const
    {
    Thread::Handle          hThread     = Thread::currentThread();
    EventDispatcher::View hDispatcher = getEventDispatcher();

    if (hThread == getThread() ||
        (NULL != hDispatcher && hThread == hDispatcher->getThread()))
        {
        return true;
        }

    return false;
    }

void Service::onServiceStarted()
    {
    setAcceptingClients(true);
    }

void Service::removeServiceListener(ServiceListener::Handle hListener)
    {
    removeEventListener(hListener);
    }

void Service::resetStats()
    {
    setStatsCpu(0);
    setStatsReceived(0);
    setStatsReset(System::currentTimeMillis());
    }

void Service::shutdown()
    {
    COH_SYNCHRONIZED(this)
        {
        stop();

        // as of Coherence 3.5, this method will not return until the service
        // has actually stopped
        if (getThread() != Thread::currentThread())
            {
            while (getDaemonState() != daemon_exited)
                {
                try
                    {
                    wait(1000L);
                    }
                catch (InterruptedException::View e)
                    {
                    Thread::currentThread()->interrupted();
                    COH_THROW (e);
                    }
                }
            }
        }
    }

void Service::start()
    {
    COH_ENSURE(getServiceState() <= service_started);

    COH_SYNCHRONIZED (this)
        {
        QueueProcessor::start();

        while (isStarted() && getServiceState() <= service_started
                && !isAcceptingClients())
            {
            wait(1000L);
            }

        if (getServiceState() != service_started)
            {
            Exception::Holder ohe = getStartException();
            String::View vS = COH_TO_STRING("Failed to start Service \""
                    << getServiceName() << "\" (ServiceState="
                    << getServiceStateName() << ')');

            COH_THROW (Exception::create(vS, ohe));
            }
        }
    }

void Service::stop()
    {
    if (!isStarted())
        {
        COH_SYNCHRONIZED (this)
            {
            if (!isStarted())
                {
                // service is not running ... don't worry about whether
                // it has never been running vs. whether it started and
                // stopped ... just set the state to register that stop
                // was called so no one can later start it
                setServiceState(service_stopped);
                return;
                }
            }
        }

    QueueProcessor::stop();

    if (getThread() == Thread::currentThread())
        {
        setServiceState(service_stopped);
        }
    }

void Service::waitAcceptingClients()
    {
    while (!isAcceptingClients())
        {
        COH_SYNCHRONIZED (this)
            {
            if (getServiceState() != service_started)
                {
                Exception::Holder ohe = getStartException();

                String::Handle vS = COH_TO_STRING("Failed to start Service \""
                    << getServiceName() << "\" (ServiceState="
                    << getServiceStateName() << ')');

                COH_THROW (Exception::create(vS, ohe));
                }

            if (!isAcceptingClients())
                {
                wait();
                }
            }
        }
    }

void Service::setSerializer(Serializer::View vSerializer)
    {
    COH_ENSURE_PARAM(vSerializer);
    initialize(f_vSerializer, vSerializer);
    }

Serializer::View Service::getSerializer() const
    {
    return f_vSerializer;
    }

void Service::onServiceStarting()
    {
    }

void Service::onServiceState(State nState)
    {
    switch (nState)
        {
        case service_starting:
            onServiceStarting();
            break;

        case service_started:
            onServiceStarted();
            break;

        case service_stopping:
            onServiceStopping();
            break;

        case service_stopped:
            onServiceStopped();
            break;

        default:
            COH_ENSURE(false);
        }
    }

void Service::onServiceStopped()
    {
    setAcceptingClients(false);
    }

void Service::onServiceStopping()
    {
    setAcceptingClients(false);
    }

int64_t Service::parseTime(XmlElement::View vXml, String::View vsName,
        int64_t cDefault)
    {
    if (NULL == vXml)
        {
        return cDefault;
        }

    String::View vsTime = vXml->getSafeElement(vsName)->getString();
    if (vsTime->length() == 0)
        {
        return cDefault;
        }

    return StringHelper::parseTime(vsTime);
    }


// ----- accessor methods ---------------------------------------------------

Service::EventDispatcher::Handle Service::getEventDispatcher()
    {
    return f_hEventDispatcher;
    }

Service::EventDispatcher::View Service::getEventDispatcher() const
    {
    return f_hEventDispatcher;
    }

Listeners::Handle Service::getEventListeners()
    {
    return f_hEventListeners;
    }

SerializerFactory::View Service::getSerializerFactory() const
    {
    return f_vSerializerFactory;
    }

String::View Service::getServiceName() const
    {
    String::View vsServiceName = f_vsServiceName;
    return NULL == vsServiceName ? Class::getSimpleClassName(this) : vsServiceName;
    }

String::View Service::getServiceStateName() const
    {
    return formatServiceStateName(getServiceState());
    }

Service::State Service::getServiceState() const
    {
    return m_nServiceState;
    }

int64_t Service::getStatsCpu() const
    {
    return m_lStatsCpu;
    }

int64_t Service::getStatsReset() const
    {
    return m_lStatsReset;
    }

int64_t Service::getStatsReceived() const
    {
    return m_lStatsReceived;
    }

bool Service::isAcceptingClients() const
    {
    return m_fAcceptingClients;
    }

void Service::setAcceptingClients(bool fAccepting)
    {
    COH_SYNCHRONIZED (this)
        {
        m_fAcceptingClients = fAccepting;

        // free any blocked client threads
        notifyAll();
        }
    }

void Service::setServiceConfig(XmlElement::View vXml)
    {
    initialize(f_vServiceConfig, vXml);
    }

void Service::setServiceName(String::View vsName)
    {
    COH_ENSURE(!isStarted());
    initialize(f_vsServiceName, vsName);
    }

void Service::setServiceState(State nState)
    {
    COH_SYNCHRONIZED (this)
        {
        State nPrevState = getServiceState();
        if (nState > nPrevState)
            {
            m_nServiceState = nState;

            onServiceState(nState);

            switch (nState)
                {
                case service_starting:
                    dispatchServiceEvent(ServiceEvent::service_starting);
                    break;

                case service_started:
                    dispatchServiceEvent(ServiceEvent::service_started);
                    break;

                case service_stopping:
                    dispatchServiceEvent(ServiceEvent::service_stopping);
                    break;

                case service_stopped:
                    dispatchServiceEvent(ServiceEvent::service_stopped);
                    break;

                default:
                    COH_ENSURE(false);
                }
            }
        else
            {
            COH_ENSURE(nState == nPrevState);
            }

        notifyAll();
        }
    }

void Service::setStatsCpu(int64_t cCpu)
    {
    m_lStatsCpu = cCpu;
    }

void Service::setStatsReceived(int64_t cMessage)
    {
    m_lStatsReceived = cMessage;
    }

void Service::setStatsReset(int64_t lStatsReset)
    {
    m_lStatsReset = lStatsReset;
    }

void Service::setEventListeners(Listeners::Handle hListeners)
    {
    initialize(f_hEventListeners, hListeners);
    }

void Service::setSerializerFactory(SerializerFactory::View vFactory)
    {
    initialize(f_vSerializerFactory, vFactory);
    }

void Service::setEventDispatcher(EventDispatcher::Handle hDispatcher)
    {
    initialize(f_hEventDispatcher, hDispatcher);
    }

OperationalContext::View Service::getOperationalContext() const
    {
    return f_vContext;
    }

void Service::setOperationalContext(OperationalContext::View vContext)
    {
    if (f_vContext != NULL)
        {
        COH_THROW (IllegalStateException::
                create("OperationalContext has already been set"));
        }
    initialize(f_vContext, vContext);
    }


// ----- Daemon interface ---------------------------------------------------

String::View Service::getThreadName() const
    {
    return getServiceName();
    }

void Service::onEnter()
    {
    QueueProcessor::onEnter();

    resetStats();

    setServiceState(service_started);
    }

void Service::onException(Exception::Holder ohe)
    {
    if (getServiceState() < service_started ||
       (getServiceState() == service_started && !isAcceptingClients()))
        {
        setStartException(ohe);
        }
    QueueProcessor::onException(ohe);
    }

void Service::onExit()
    {
    QueueProcessor::onExit();

    // set the service-state to STOPPED
    try
        {
        setServiceState(service_stopped);
        }
    catch (Exception::View ve)
        {
        COH_LOG("Exception occurred during exiting:\n " <<  ve->getStackTrace(), 4);
        }

    // stop the EventDispatcher
    // note: this must be done after changing the service state, which could
    //       cause events to be emitted
    EventDispatcher::Handle hDaemon = getEventDispatcher();
    if (NULL != hDaemon)
        {
        hDaemon->stop();

        // give the daemon a chance to drain its queue
        if (!hDaemon->join(1000L))
            {
            COH_LOG("failed to stop " << hDaemon << "; abandoning", 2);
            }
        }
    getEventListeners()->removeAll();
    }

void Service::onNotify()
    {
    QueueProcessor::onNotify();

    /*
    DaemonPool pool = getDaemonPool();
    if (pool.isStarted() && Base.getSafeTimeMillis() >= pool.getNextCheckMillis())
        {
        pool.checkTimeouts();
        }
    */
    }


// ----- Describable interface ----------------------------------------------

String::View Service::getDescription() const
    {
    // skip super::getDescription();
    return COH_TO_STRING("Name=" << getServiceName() << ", State=("
        << getServiceStateName() << ")");
    }


// ----- Object interface ---------------------------------------------------

void Service::onInit()
    {
    ensureInitializedEventDispatcher();

    super::onInit();
    }

TypedHandle<const String> Service::toString() const
    {
    return COH_TO_STRING(Class::getSimpleClassName(this) << '{' << getDescription() << '}');
    }


// ----- internal methods ---------------------------------------------------

void Service::addEventListener(EventListener::Handle hEventListener)
    {
    ensureEventDispatcher();
    getEventListeners()->add(hEventListener);
    }

void Service::dispatchEvent(Event::View vEvent, Listeners::Handle hListeners)
    {
    DispatchEvent::Handle hTask = instantiateDispatchEvent();
    hTask->setEvent(vEvent);
    hTask->setListeners(hListeners);
    ensureEventDispatcher()->getQueue()->add(hTask);
    }

Service::DispatchEvent::Handle Service::instantiateDispatchEvent() const
    {
    return DispatchEvent::create();
    }

void Service::removeEventListener(EventListener::Handle hListener)
    {
    getEventListeners()->remove(hListener);
    }


// ----- Service child component factory ------------------------------------

Service::EventDispatcher::Handle Service::instantiateEventDispatcher() const
    {
    return coherence::component::util::Service::EventDispatcher::create(this);
    }

Service::EventDispatcher::Handle Service::ensureInitializedEventDispatcher()
    {
    EventDispatcher::Handle hResult = getEventDispatcher();

    if (NULL == hResult)
        {
        hResult = instantiateEventDispatcher();
        setEventDispatcher(hResult);
        }

    return hResult;
    }

COH_CLOSE_NAMESPACE3

#include <IOKit/IOLib.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include "InsomniaT.h"

/**
 * Defines the super class.
 */
#define super IOService

#define DEBUG
#ifndef DEBUG
#define IOLog(x, ...)
#endif

#ifndef kIOPMMessageSleepWakeUUIDChange
/**
 * This should should be in IOPM.h but does not seem to be there.
 */
#define kIOPMMessageSleepWakeUUIDChange iokit_family_msg(sub_iokit_powermanagement, 0x140)
/**
 * This should should be in IOPM.h but does not seem to be there.
 */
#define kIOPMMessageSleepWakeUUIDSet      ((void *)1)
/**
 * This should should be in IOPM.h but does not seem to be there.
 */
#define kIOPMMessageSleepWakeUUIDCleared  ((void *)0)
#endif
#ifndef kIOPMMessageDriverAssertionsChanged
/**
 * This should should be in IOPM.h but does not seem to be there.
 */
#define kIOPMMessageDriverAssertionsChanged iokit_family_msg(sub_iokit_powermanagement, 0x150)
#endif
#ifndef kIOPMPowerStateMax
/**
 * This is the maximum number of power states allowed by IOKit.
 */
#define kIOPMPowerStateMax          0xFFFFFFFF
#endif

/**
 * Returns a single instance of an IOService for a given class name and a base provider.
 * This ensures that there is a single instance before returning, otherwise it will
 * return NULL.
 * 
 * @param className class name.
 * @param provider provider to use as the starting point for the search.
 * @return the IOService that matches the criteria.
 */
static inline IOService* getIOService(const char* className, const IOService* provider) {
    OSIterator* i = provider->getMatchingServices(IOService::nameMatching(className));
    if (i == NULL) {
        return NULL;
    }
    unsigned int deviceCount = 0;
    OSObject* obj;
    IOService* service;
    
    while ((obj = i->getNextObject()) != NULL && deviceCount < 2) {
        service = OSDynamicCast(IOService, obj);
        ++deviceCount;
    }
    i->release();
    if (deviceCount > 1) {
        IOLog("InsomniaT: %s count = %d, returning NULL\n", className, deviceCount);
        return NULL;
    }
    IOLog("InsomniaT: %s found %p\n", className, service);
    return service;
}

/**
 * This logs other sleep wake interests if it is not specifically handled.
 * Primarily used for logging.
 * 
 * @param target target object, should be an IOService otherwise this will return kIOReturnError.
 * @return kIOReturnSuccess when logged correctly.
 */
static inline IOReturn handleOtherSleepWakeInterest(void *target, void *refCon,
                                                    UInt32 messageType, IOService *provider,
                                                    void *messageArgument, vm_size_t argSize )
{
    IOService *obj = OSDynamicCast(IOService, (OSObject *)target);
    if (obj == NULL) {
        IOLog("InsomniaT: target object was not an IOService or NULL\n");
        return kIOReturnError;
    }
    
    if (messageType == kIOPMMessageSleepWakeUUIDChange) {
        if (messageArgument == kIOPMMessageSleepWakeUUIDSet) {
            IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOPMMessageSleepWakeUUIDChange, kIOPMMessageSleepWakeUUIDSet\n", obj->getName());
        } else if (messageArgument == kIOPMMessageSleepWakeUUIDCleared) {
            IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOPMMessageSleepWakeUUIDChange, kIOPMMessageSleepWakeUUIDCleared\n", obj->getName());
        }
    } else if (messageType == kIOMessageServicePropertyChange) {
        IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOMessageServicePropertyChange, %p\n", obj->getName(), messageArgument);
    } else if (messageType == kIOMessageServiceWasClosed) {
        IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOMessageServiceWasClosed, %p\n", obj->getName(), messageArgument);
    } else if (messageType == kIOPMMessageDriverAssertionsChanged) {
        IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOPMMessageDriverAssertionsChanged, %p\n", obj->getName(), messageArgument);
    } else if (messageType == kIOMessageSystemWillPowerOn) {
        IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOMessageSystemWillPowerOn, %p\n", obj->getName(), messageArgument);
    } else if (messageType == kIOMessageSystemHasPoweredOn) {
        IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOMessageSystemHasPoweredOn, %p\n", obj->getName(), messageArgument);
    } else if (messageType == kIOMessageSystemWillPowerOff) {
        IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOMessageSystemWillPowerOff, %p\n", obj->getName(), messageArgument);
    } else {
        IOLog("InsomniaT: handleIOServiceSleepWakeInterest on %s invoked with %lx\n", obj->getName(), (unsigned long)messageType);
        IOLog("InsomniaT: target = %p\n", target);
        IOLog("InsomniaT: refCon = %p\n", refCon);
        IOLog("InsomniaT: messageArgument = %p\n", messageArgument);
        IOLog("InsomniaT: argSize = %d\n", (int)argSize);
        if (provider) {
            IOLog("InsomniaT: provider = %s\n", provider->getName());
        } else {
            IOLog("InsomniaT: provider = NULL\n");
        }
    }
    return kIOReturnSuccess;
}
IOReturn handleIOServiceSleepWakeInterest(void *target, void *refCon,
                                          UInt32 messageType, IOService *provider,
                                          void *messageArgument, vm_size_t argSize )
{
    IOService *obj = (IOService *)target;
    
    if (messageType == kIOPMMessageClamshellStateChange) {
        IOLog("InsomniaT: handleIOServiceSleepWakeInterest invoked with kIOPMMessageClamshellStateChange\n");
        const long message = (long)messageArgument;
        const bool clamshellClosed = (message & kClamshellStateBit);
        IOLog("InsomniaT: clamshellClosed = %d\n", clamshellClosed);
        if (clamshellClosed && obj->getPowerState() != 0x0) {
            IOLog("InsomniaT: clamshell closed and %s is on, turning %s off\n", obj->getName(), obj->getName());
            obj->changePowerStateTo(0x0);
        } else if (!clamshellClosed && obj->getPowerState() == 0x0) {
            IOLog("InsomniaT: clamshell open and %s is off, turning %s on\n", obj->getName(), obj->getName());
            obj->changePowerStateTo(kIOPMPowerStateMax);
        }
    } else if (messageType == kIOMessageSystemHasPoweredOn) {
        IOLog("InsomniaT: handleIOServiceSleepWakeInterest invoked with kIOMessageSystemHasPoweredOn, %p\n", messageArgument);
        const bool clamshellClosed = provider->getProperty(kAppleClamshellStateKey) == kOSBooleanTrue;
        if (clamshellClosed) {
            IOLog("InsomniaT: clamshell closed, turning %s off\n", obj->getName());
            obj->changePowerStateTo(0x0);
        }
    } else {
        return handleOtherSleepWakeInterest(target, refCon, messageType, provider, messageArgument, argSize);
    }
    return kIOReturnSuccess;
}

IOReturn handleSleepWakeInterest(void *target, void *refCon,
								 UInt32 messageType, IOService *provider,
								 void *messageArgument, vm_size_t argSize )
{
    net_trajano_driver_InsomniaT *obj = (net_trajano_driver_InsomniaT *)target;
    
    if (messageType == kIOPMMessageClamshellStateChange) {
        IOLog("InsomniaT: handleSleepWakeInterest invoked with kIOPMMessageClamshellStateChange\n");
        long message = (long)messageArgument;
        bool clamshellShouldSleep = (message & kClamshellSleepBit);
        
        IOPMrootDomain *root = (IOPMrootDomain *)provider;
        if (!obj->sleepOnClamshellClose && clamshellShouldSleep) {
            IOLog("InsomniaT: want sleep is disabled and clamshellCausesSleep is enabled\n");
            root->receivePowerNotification(kIOPMDisableClamshell);
            provider->setProperty(kAppleClamshellCausesSleepKey, kOSBooleanFalse);
        } else if (obj->sleepOnClamshellClose && !clamshellShouldSleep) {
            IOLog("InsomniaT: want sleep is enabled and clamshellCausesSleep is disabled\n");
            root->receivePowerNotification(kIOPMEnableClamshell);
            provider->setProperty(kAppleClamshellCausesSleepKey, kOSBooleanTrue);
        } else {
            IOLog("InsomniaT: sleepOnClamshellClose = %d and clamshellShouldSleep = %d, doing nothing\n", obj->sleepOnClamshellClose, clamshellShouldSleep);
        }        
    } else if (messageType == kIOMessageSystemWillSleep) {
        IOLog("InsomniaT: handleSleepWakeInterest invoked with kIOMessageSystemWillSleep\n");
        IOPowerStateChangeNotification *notification = (IOPowerStateChangeNotification *)messageArgument;
        if (obj->sleepOnClamshellClose) {
            IOLog("InsomniaT: acknowledgeSleepWakeNotification, notification->powerRef = %p\n", notification->powerRef);
            acknowledgeSleepWakeNotification(notification->powerRef);
        } else {
            IOLog("InsomniaT: vetoSleepWakeNotification, notification->powerRef = %p\n", notification->powerRef);
            vetoSleepWakeNotification(notification->powerRef);
        }
    } else {
        return handleOtherSleepWakeInterest(target, refCon, messageType, provider, messageArgument, argSize);
    }
    
	return kIOReturnSuccess;
}

/**
 * Sets up the common members used in OSObject instances.
 */
OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaT, IOService)

bool net_trajano_driver_InsomniaT::init(OSDictionary* dictionary)
{
    IOLog("InsomniaT: init\n");
    if (!super::init(dictionary)) {
        return false;
    }
    sleepOnClamshellClose = true;
    return true;
}

bool net_trajano_driver_InsomniaT::start(IOService *provider)
{
    IOLog("InsomniaT: start\n");
    
    IOWorkLoop* workLoop = getWorkLoop();
    if (workLoop == NULL) {
        IOLog("InsomniaT: no workloop on start()\n");
        return false;
    }
    IOLog("InsomniaT: workloop = %p\n", workLoop);
    
    
    IOPMrootDomain *root = getPMRootDomain();
    if (root == NULL) {
        return false;
    }
    
    mach_timespec_t t;
    t.tv_sec = 20;
    t.tv_nsec = 0;
    
    IOService *appleBacklightDisplay = waitForService(nameMatching("AppleBacklightDisplay"), &t);
    if (appleBacklightDisplay == NULL) {
        IOLog("InsomniaT: AppleBacklightDisplay not found\n");
        return false;
    }
    appleBacklightDisplayNotifier = registerSleepWakeInterest(handleIOServiceSleepWakeInterest, appleBacklightDisplay);
    IOLog("InsomniaT: appleBacklightDisplayNotifier = %p\n", appleBacklightDisplayNotifier);
    if (appleBacklightDisplayNotifier == NULL) {
        return false;
    }
    
    IOService *appleLMUController = waitForService(nameMatching("AppleLMUController"), &t);
    if (appleLMUController == NULL) {
        IOLog("InsomniaT: AppleLMUController not found\n");
        return false;
    }
    appleLMUControllerNotifier = registerSleepWakeInterest(handleIOServiceSleepWakeInterest, appleLMUController);
    IOLog("InsomniaT: appleLMUControllerNotifier = %p\n", appleLMUControllerNotifier);
    if (appleLMUControllerNotifier == NULL) {
        return false;
    }
    
    clamshellNotifier = registerSleepWakeInterest(handleSleepWakeInterest, this);
    IOLog("InsomniaT: clamshellNotifier = %p\n", clamshellNotifier);
    if (clamshellNotifier == NULL) {
        return false;
    }
    
    bool res = super::start(provider);
	if (res) {
		registerService();
	}
	return res;
}

bool net_trajano_driver_InsomniaT::open(IOService *forClient, IOOptionBits options, void *arg) {
    IOLog("InsomniaT: open %p\n", forClient);
    return super::open(forClient, options, arg);
}

void net_trajano_driver_InsomniaT::close(IOService *forClient, IOOptionBits options) {
    IOLog("InsomniaT: close %p\n", forClient);
    super::close(forClient, options);
}

void net_trajano_driver_InsomniaT::stop(IOService *provider)
{
    IOLog("InsomniaT: stop\n");
    appleBacklightDisplayNotifier->remove();
    appleBacklightDisplayNotifier = NULL;
    appleLMUControllerNotifier->remove();
    appleLMUControllerNotifier = NULL;
    clamshellNotifier->remove();
    clamshellNotifier = NULL;
    sleepOnClamshellClose = true;
    IOPMrootDomain *root = getPMRootDomain();
    getIOService("AppleLMUController",root)->changePowerStateTo(kIOPMPowerStateMax);
    root->receivePowerNotification(kIOPMEnableClamshell);
	super::stop(provider);
}

void net_trajano_driver_InsomniaT::enableSleepOnClamshellClose() {
    IOLog("InsomniaT: enableSleepOnClamshellClose\n");
    sleepOnClamshellClose = true;
    //systemPowerEventOccured(NULL, NULL);
    getPMRootDomain()->receivePowerNotification(kIOPMEnableClamshell);
}

void net_trajano_driver_InsomniaT::disableSleepOnClamshellClose() {
    IOLog("InsomniaT: disableSleepOnClamshellClose\n");
    sleepOnClamshellClose = false;
    getPMRootDomain()->receivePowerNotification(kIOPMDisableClamshell);
}

bool net_trajano_driver_InsomniaT::isClamshellCloseCausesSleep() const {
    IOLog("InsomniaT: isClamshellCloseCausesSleep clamshellNotifier = %p, %d\n", clamshellNotifier, sleepOnClamshellClose);
    return sleepOnClamshellClose;
}

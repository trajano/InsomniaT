#include <IOKit/IOLib.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include "InsomniaT.h"

/**
 * Defines the super class.
 */
#define super IOService

//#define NOPOWER
#define DEBUG
#ifndef DEBUG
#define IOLog(x, ...)
#endif

#ifndef kIOPMMessageSystemPowerEventOccurred
/**
 * This should should be in IOPM.h but does not seem to be there.
 */
#define kIOPMMessageSystemPowerEventOccurred  iokit_family_msg(sub_iokit_powermanagement, 0x130)
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
 * @param provider provider to use as the starting point for the search.  Usually this is the PMrootDomain.
 * @return the IOService that matches the criteria.
 */
static inline IOService* getIOService(const char * const className, const IOService * const provider) {
    OSIterator* i = provider->getMatchingServices(IOService::serviceMatching(className));
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
 * This returns the number of IOServices for a given class name and a base provider.
 * @param className class name.
 * @param provider provider to use as the starting point for the search.  Usually this is the PMrootDomain.
 * @return the number IOService that matches the criteria.
 */
static inline unsigned int getNumberOfIOService(const char * const className, const IOService * const provider) {
    OSIterator* i = provider->getMatchingServices(IOService::serviceMatching(className));
    if (i == NULL) {
        return NULL;
    }
    unsigned int deviceCount = 0;
    OSObject* obj;
    while ((obj = i->getNextObject()) != NULL) {
        ++deviceCount;
        IOLog("InsomniaT: %s count = %d\n", className, deviceCount);
    }
    i->release();
    return deviceCount;
}

/**
 * This returns true if the AppleBacklightDisplay is connected and no other
 * displays are connected.  This is determined by checking that there is
 * an AppleBacklightDisplay service and there is only one IODisplayConnect
 * service.
 *
 * @param provider provider to use as the starting point for the search.  Usually this is the PMrootDomain.
 * @return the number IOService that matches the criteria.
 */
static inline bool isOnlyAppleBacklightDisplayConnected(const IOService * const provider) {
    return getIOService("AppleBacklightDisplay", provider) != NULL &&
           getNumberOfIOService("IODisplayConnect", provider) == 1;
}

/**
 * This logs other sleep wake interests if it is not specifically handled.
 * Primarily used for logging.
 *
 * @param target target object, should be an IOService otherwise this will return kIOReturnError.
 * @param refCon reference constant that is passed when the notification was created.
 * @param messageType message type.
 * @param provider the IOService object who is delivering the notification. It is retained for the duration of the handler's invocation and doesn't need to be released by the handler.
 * @param messageArgument extra information for the message.
 * @param argSize size of the message argument
 * @return kIOReturnSuccess when logged correctly.
 */
static inline IOReturn handleOtherSleepWakeInterest(void * const target, void * const refCon,
        UInt32 const messageType, IOService * const provider,
        void * const messageArgument, vm_size_t const argSize ) {
    IOService *obj = (IOService *)target;
    if (obj == NULL) {
        IOLog("InsomniaT: target object was NULL\n");
        return kIOReturnError;
    }

    if (messageType == kIOPMMessageSleepWakeUUIDChange) {
        if (messageArgument == kIOPMMessageSleepWakeUUIDSet) {
            IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOPMMessageSleepWakeUUIDChange, kIOPMMessageSleepWakeUUIDSet\n", obj->getName());
        } else if (messageArgument == kIOPMMessageSleepWakeUUIDCleared) {
            IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOPMMessageSleepWakeUUIDChange, kIOPMMessageSleepWakeUUIDCleared\n", obj->getName());
        }
    } else if (messageType == kIOPMMessageSystemPowerEventOccurred) {
        IOLog("InsomniaT: handleSleepWakeInterest %s, invoked with kIOPMMessageSystemPowerEventOccurred, %p\n", obj->getName(), messageArgument);
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

/**
 * Turns a IOService referenced by a className in a provider off.  It only does
 * this if the IOService is unique base on the class name.
 *
 * @param className class name
 * @param provider provider to use as the starting point for the search.  Usually this is the PMrootDomain.
 */
static inline void powerOff(const char * const className, IOService * const provider) {
    IOService *obj = getIOService(className, provider);
    if (obj == NULL) {
        return;
    }
    if (obj->getPowerState() != 0x0) {
        obj->changePowerStateTo(0x0);
    }
}

/**
 * Turns a IOService referenced by a className in a provider on.  It only does
 * this if the IOService is unique base on the class name.
 *
 * @param className class name
 * @param provider provider to use as the starting point for the search.  Usually this is the PMrootDomain.
 */
static inline void powerOn(const char * const className, IOService * const provider) {
    IOService *obj = getIOService(className, provider);
    if (obj == NULL) {
        return;
    }
    if (obj->getPowerState() == 0x0) {
        obj->changePowerStateTo(kIOPMPowerStateMax);
    }
}

IOReturn handleSleepWakeInterest(void * const target, void * const refCon,
                                 UInt32 messageType, IOService * const provider,
                                 void * const messageArgument, vm_size_t const argSize ) {
    net_trajano_driver_InsomniaT *obj = (net_trajano_driver_InsomniaT *)target;

    if (messageType == kIOPMMessageClamshellStateChange) {
        IOLog("InsomniaT: handleSleepWakeInterest invoked with kIOPMMessageClamshellStateChange\n");
        IOPMrootDomain *root = (IOPMrootDomain *)provider;
        long message = (long)messageArgument;
        const bool clamshellClosed = (message & kClamshellStateBit);
        const bool clamshellShouldSleep = (message & kClamshellSleepBit);
        const bool onlyAppleBacklightDisplayConnected = isOnlyAppleBacklightDisplayConnected(root);

        if (clamshellClosed) {
            powerOff("AppleBacklightDisplay", provider);
            powerOff("AppleLMUController", provider);
        } else {
            powerOn("AppleBacklightDisplay", provider);
            powerOn("AppleLMUController", provider);
        }
        if (!obj->sleepOnClamshellClose && clamshellShouldSleep) {
            IOLog("InsomniaT: want sleep is disabled and clamshellCausesSleep is enabled, disabling clamshell\n");
#ifndef NOPOWER
            root->receivePowerNotification(kIOPMDisableClamshell);
            powerOff("AppleBacklightDisplay", provider);
#endif
            provider->setProperty(kAppleClamshellCausesSleepKey, kOSBooleanFalse);
        } else if (obj->sleepOnClamshellClose && !clamshellShouldSleep && onlyAppleBacklightDisplayConnected) {
            IOLog("InsomniaT: want sleep is enabled, clamshellCausesSleep is disabled and only single monitor, enabling clamshell\n");
#ifndef NOPOWER
            root->receivePowerNotification(kIOPMEnableClamshell);
#endif
            provider->setProperty(kAppleClamshellCausesSleepKey, kOSBooleanTrue);
        } else {
            IOLog("InsomniaT: sleepOnClamshellClose = %d, clamshellShouldSleep = %d, and onlyAppleBacklightDisplayConnected = %d, doing nothing\n",
                  obj->sleepOnClamshellClose, clamshellShouldSleep, onlyAppleBacklightDisplayConnected);
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

bool net_trajano_driver_InsomniaT::init(OSDictionary * const dictionary) {
    IOLog("InsomniaT: init\n");
    if (!super::init(dictionary)) {
        return false;
    }
    sleepOnClamshellClose = true;
    return true;
}

bool net_trajano_driver_InsomniaT::start(IOService * const provider) {
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

bool net_trajano_driver_InsomniaT::open(IOService * const forClient, IOOptionBits const options, void * const arg) {
    IOLog("InsomniaT: open %p\n", forClient);
    return super::open(forClient, options, arg);
}

void net_trajano_driver_InsomniaT::close(IOService * const forClient, IOOptionBits const options) {
    IOLog("InsomniaT: close %p\n", forClient);
    super::close(forClient, options);
}

void net_trajano_driver_InsomniaT::stop(IOService * const provider) {
    IOLog("InsomniaT: stop\n");
    clamshellNotifier->remove();
    clamshellNotifier = NULL;
    sleepOnClamshellClose = true;
    IOPMrootDomain *root = getPMRootDomain();
    powerOn("AppleLMUController", provider);
    root->receivePowerNotification(kIOPMEnableClamshell);
    super::stop(provider);
}

void net_trajano_driver_InsomniaT::enableSleepOnClamshellClose() {
    IOLog("InsomniaT: enableSleepOnClamshellClose\n");
    sleepOnClamshellClose = true;
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

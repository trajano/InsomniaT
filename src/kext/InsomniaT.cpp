#include <IOKit/IOLib.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include "InsomniaT.h"
#include "InsomniaTUserClient.h"

#define super IOService

OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaT, IOService)

IOReturn handleSleepWakeInterest( void * target, void * refCon,
								 UInt32 messageType, IOService * provider,
								 void * messageArgument, vm_size_t argSize )
{
	net_trajano_driver_InsomniaT *obj = (net_trajano_driver_InsomniaT*)target;
	if (obj->isLoggingEnabled()) {
		IOLog("InsomniaT: handleSleepWakeInterest invoked with message type %x.\n", (unsigned int)messageType);
	} 
	if (messageType == kIOPMMessageClamshellStateChange) {
		if (obj->isLoggingEnabled()) {
			IOLog("InsomniaT: message type is kIOPMMessageClamshellStateChange.\n");
		}
		obj->updateSystemSleep();
	}
	return 0;
}

IOService* net_trajano_driver_InsomniaT::getBuiltInISight() {
    if (builtInISight != NULL) {
        return builtInISight;
    }
    IOLog("lookup Built-in iSight\n");
    OSDictionary* dict = OSDictionary::withCapacity(1);
    dict->setObject(kIOProviderClassKey, OSString::withCStringNoCopy("AppleLMUController"));
//    dict->setObject(kIONameMatchKey, OSString::withCStringNoCopy("Built-in iSight"));
    OSIterator* ioDisplayConnectIterator = getMatchingServices(dict);
    if (ioDisplayConnectIterator == NULL) {
        dict->release();
        return NULL;
    }
    unsigned int deviceCount = 0;
    OSObject* obj;
    while ((obj = ioDisplayConnectIterator->getNextObject()) != NULL && deviceCount < 2) {
        builtInISight = OSDynamicCast(IOService, obj);
        ++deviceCount;
    }
    ioDisplayConnectIterator->release();
    IOLog("iSight count = %d\n", deviceCount);
    if (deviceCount > 1) {
        dict->release();
        builtInISight = NULL;
        return NULL;
    }
    IOLog("iSight found");
    dict->release();
    return builtInISight;
}

IOService* net_trajano_driver_InsomniaT::getAppleBacklightDisplay() {
    if (appleBacklightDisplay != NULL) {
        return appleBacklightDisplay;
    }
    IOLog("lookup backlight display\n");
    OSDictionary* dict = OSDictionary::withCapacity(1);
    dict->setObject(kIOProviderClassKey, OSString::withCStringNoCopy("AppleBacklightDisplay"));
    OSIterator* ioDisplayConnectIterator = getMatchingServices(dict);
    if (ioDisplayConnectIterator == NULL) {
        dict->release();
        return NULL;
    }
    unsigned int displayCount = 0;
    OSObject* obj;
    while ((obj = ioDisplayConnectIterator->getNextObject()) != NULL && displayCount < 2) {
        appleBacklightDisplay = OSDynamicCast(IOService, obj);
        ++displayCount;
    }
    ioDisplayConnectIterator->release();
    IOLog("display count = %d\n", displayCount);
    if (displayCount > 1) {
        dict->release();
        appleBacklightDisplay = NULL;
        return NULL;
    }
    dict->release();
    return appleBacklightDisplay;
}

void net_trajano_driver_InsomniaT::updateSystemSleep() {
	if (isLoggingEnabled()) {
		IOLog("InsomniaT: sleep status is being updated.\n");
	} 
	if (isLoggingEnabled() && isSleepEnabled()) {
		IOLog("InsomniaT: sleep is enabled\n");
	} 
	if (isLoggingEnabled() && !isSleepEnabled()) {
		IOLog("InsomniaT: sleep is disabled\n");
	} 
	if (isLoggingEnabled() && isSleepEnabledBySystem()) {
		IOLog("InsomniaT: sleep is enabled by system\n");
	} 
	if (isLoggingEnabled() && !isSleepEnabledBySystem()) {
		IOLog("InsomniaT: sleep is disabled by system\n");
	} 
    
    
    if (getBuiltInISight() != NULL) {
        IOPMrootDomain *root = getPMRootDomain();
        if (root->getProperty(kAppleClamshellStateKey) == kOSBooleanTrue) {
            // Lid is closed
            IOLog("InsomniaT: lid is closed, disabling camera to prevent screen activation\n");
            // The USB device itself has no power state, it has to be in the controller.
            //            ((IOService*)(getBuiltInISight()->getParentEntry(gIOServicePlane)))->changePowerStateTo(0x0);
            // getAppleBacklightDisplay()->changePowerStateTo(0x0);
            getBuiltInISight()->changePowerStateTo(0x0);
        } else if (root->getProperty(kAppleClamshellStateKey) == kOSBooleanFalse) {
            // Lid is open
            IOLog("InsomniaT: lid is open, enabling camera\n");
//            ((IOService*)(getBuiltInISight()->getParentEntry(gIOServicePlane)))->changePowerStateTo(0x1);
            // getAppleBacklightDisplay()->changePowerStateTo(0x3);
            getBuiltInISight()->changePowerStateTo(0x1);
        } 
    } else {
        IOLog("InsomniaT: missing Built-in ISight\n");
    }
    
    // If lid is open disable check on the brightness
    net_trajano_driver_InsomniaTUserClient* client = (net_trajano_driver_InsomniaTUserClient*)getClient();
    if (client != NULL) {
        // In theory this will call the user land client 
        // to disable the automatic brightness adjustment
    }

    if (isSleepEnabled() && !isSleepEnabledBySystem()) {
		if (isLoggingEnabled()) {
			IOLog("InsomniaT: enabling sleep.\n");
		} 
		enableSleep();
	} else if (!isSleepEnabled() && isSleepEnabledBySystem()) {
		if (isLoggingEnabled()) {
			IOLog("InsomniaT: disabling sleep.\n");
		}
		disableSleep();
	}
}
bool net_trajano_driver_InsomniaT::isMultipleDisplays() {
    OSDictionary* dict = OSDictionary::withCapacity(1);
    dict->setObject(kIOProviderClassKey, OSString::withCStringNoCopy("IODisplayConnect"));
    OSIterator* ioDisplayConnectIterator = getMatchingServices(dict);
    if (ioDisplayConnectIterator == NULL) {
        dict->release();
        return false;
    }
    unsigned int displayCount = 0;
    OSObject* obj;
    while ((obj = ioDisplayConnectIterator->getNextObject()) != NULL && displayCount < 2) {
        ++displayCount;
    }
    ioDisplayConnectIterator->release();
    dict->release();
    return displayCount > 1;
}
bool net_trajano_driver_InsomniaT::start(IOService *provider)
{
	IOPMrootDomain *root = getPMRootDomain();
    
	IOWorkLoop *workloop = getWorkLoop();
	if (!workloop) {
		return false;
	}
	
	fAppleClamshellCausesSleep = root->getProperty(kAppleClamshellCausesSleepKey);
    fNotifier = registerSleepWakeInterest(handleSleepWakeInterest, this);
	disableSleep();
    appleBacklightDisplay = NULL;
    builtInISight = NULL;
    
    bool res = super::start(provider);
	if (res) {
		registerService();
	}
	return res;
}

void net_trajano_driver_InsomniaT::disableSleep() {
    if (isMultipleDisplays()) {
		if (isLoggingEnabled()) {
			IOLog("InsomniaT: multiple displays detected doing nothing.\n");
		} 
        return;
    }
	IOPMrootDomain *root = getPMRootDomain();
	root->setProperty(kAppleClamshellCausesSleepKey,kOSBooleanFalse);
    // Calling this method will set the ignoringClamShell to true for the PM root domain.
	root->receivePowerNotification(kIOPMDisableClamshell);
	if (isLoggingEnabled()) {
		IOLog("InsomniaT: disabling sleep complete\n");
	}
}

/**
 * SleepEnabled key.
 */
const char* net_trajano_driver_InsomniaT::gKeySleepEnabled = "SleepEnabled";

/**
 * LoggingEnabled key.
 */
const char* net_trajano_driver_InsomniaT::gKeyLoggingEnabled = "LoggingEnabled";

bool net_trajano_driver_InsomniaT::isSleepEnabled() {
	return ((OSBoolean*)getProperty(gKeySleepEnabled))->getValue();
}

bool net_trajano_driver_InsomniaT::isLoggingEnabled() {
	return ((OSBoolean*)getProperty(gKeyLoggingEnabled))->getValue();
}


bool net_trajano_driver_InsomniaT::isSleepEnabledBySystem() {
	IOPMrootDomain *root = getPMRootDomain();
	return root->getProperty(kAppleClamshellCausesSleepKey) == kOSBooleanTrue;
}

void net_trajano_driver_InsomniaT::enableSleep() {
    if (isMultipleDisplays()) {
		if (isLoggingEnabled()) {
			IOLog("InsomniaT: multiple displays detected doing nothing.\n");
		} 
        return;
    }
	IOPMrootDomain *root = getPMRootDomain();
	root->setProperty(kAppleClamshellCausesSleepKey,fAppleClamshellCausesSleep);
    // Calling this method will set the ignoringClamShell to false for the PM root domain.
	root->receivePowerNotification(kIOPMEnableClamshell);
	if (isLoggingEnabled()) {
		IOLog("InsomniaT: enabling sleep complete\n");
	}
}

IOReturn net_trajano_driver_InsomniaT::setSleepEnabled(bool sleepEnabled) {
	setProperty(gKeySleepEnabled, sleepEnabled);
	if (sleepEnabled) {
		if (isLoggingEnabled()) {
			IOLog("InsomniaT: sleep enabled requested, enabling sleep.\n");
		} 
		enableSleep();
	} else {
		if (isLoggingEnabled()) {
			IOLog("InsomniaT: sleep disabled requested, disabling sleep.\n");
		} 
		disableSleep();
	}
	return true;
}

IOReturn net_trajano_driver_InsomniaT::setLoggingEnabled(bool loggingEnabled) {
	setProperty(gKeyLoggingEnabled, loggingEnabled);
	if (isLoggingEnabled()) {
		IOLog("InsomniaT: logging is enabled\n");
	} else {
		IOLog("InsomniaT: logging is disabled\n");
	}
	return true;
}

/**
 * This is called when the kext is being unloaded.  It will remove the notifier handler
 * and enable sleep.
 */
void net_trajano_driver_InsomniaT::stop(IOService *provider)
{
	if (isLoggingEnabled()) {
		IOLog("InsomniaT: service is stopping.\n");
	} 
	fNotifier->remove();
    appleBacklightDisplay = NULL;
    builtInISight = NULL;
	enableSleep();
	super::stop(provider);
}

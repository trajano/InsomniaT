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

IOService* net_trajano_driver_InsomniaT::getAppleLMUController() {
    if (appleLMUController != NULL) {
        return appleLMUController;
    }
    if (isLoggingEnabled()) {
        IOLog("lookup AppleLMUController\n");
    }
    OSDictionary* dict = OSDictionary::withCapacity(1);
    dict->setObject(kIOProviderClassKey, OSString::withCStringNoCopy("AppleLMUController"));
    OSIterator* ioDisplayConnectIterator = getMatchingServices(dict);
    if (ioDisplayConnectIterator == NULL) {
        dict->release();
        return NULL;
    }
    unsigned int deviceCount = 0;
    OSObject* obj;
    while ((obj = ioDisplayConnectIterator->getNextObject()) != NULL && deviceCount < 2) {
        appleLMUController = OSDynamicCast(IOService, obj);
        ++deviceCount;
    }
    ioDisplayConnectIterator->release();
    if (isLoggingEnabled()) {
        IOLog("AppleLMUController count = %d\n", deviceCount);
    }
    if (deviceCount > 1) {
        dict->release();
        appleLMUController = NULL;
        return NULL;
    }
    if (isLoggingEnabled()) {
        IOLog("AppleLMUController found");
    }
    dict->release();
    return appleLMUController;
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
    
    
    IOPMrootDomain *root = getPMRootDomain();
    if (root->getProperty(kAppleClamshellStateKey) == kOSBooleanTrue) {
        // Lid is closed
        if (isLoggingEnabled()) {
            IOLog("InsomniaT: lid is closed, disabling ambient light sensor prevent screen activation\n");
        }
        appleLMUController->changePowerStateTo(0x0);
    } else if (root->getProperty(kAppleClamshellStateKey) == kOSBooleanFalse) {
        // Lid is open
        if (isLoggingEnabled()) {
            IOLog("InsomniaT: lid is open, enabling ambient light sensor\n");
        }
        appleLMUController->changePowerStateTo(0x1);
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
    appleLMUController = getAppleLMUController();
    
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
	enableSleep();
    appleLMUController = NULL;
	super::stop(provider);
}

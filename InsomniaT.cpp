#include <IOKit/IOLib.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include "InsomniaT.h"

#define super IOService

OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaT, IOService)

IOReturn handleSleepWakeInterest( void * target, void * refCon,
								 UInt32 messageType, IOService * provider,
								 void * messageArgument, vm_size_t argSize )
{
	net_trajano_driver_InsomniaT *obj = (net_trajano_driver_InsomniaT*)target;
	if (messageType == kIOPMMessageClamshellStateChange) {
		obj->updateSystemSleep();
	}
	return 0;
}

void net_trajano_driver_InsomniaT::updateSystemSleep() {
	if (isSleepEnabled() && !isSleepEnabledBySystem()) {
		enableSleep();
	} else if (!isSleepEnabled() && isSleepEnabledBySystem()) {
		disableSleep();
	}
}
bool net_trajano_driver_InsomniaT::start(IOService *provider)
{
	IOPMrootDomain *root = getPMRootDomain();
	
		//setSleepEnabled(false);
	
	IOWorkLoop *workloop = getWorkLoop();
	if (!workloop) {
		return false;
	}
	
	fAppleClamshellCausesSleep = root->getProperty(kAppleClamshellCausesSleepKey);
	fNotifier = registerSleepWakeInterest(handleSleepWakeInterest, this);
	disableSleep();

    bool res = super::start(provider);
	if (res) {
		registerService();
	}
	return res;
}
void net_trajano_driver_InsomniaT::disableSleep() {
	IOPMrootDomain *root = getPMRootDomain();
	root->setProperty(kAppleClamshellCausesSleepKey,kOSBooleanFalse);
		// Calling this method will set the ignoringClamShell to true for the PM root domain.
	root->receivePowerNotification(kIOPMDisableClamshell);
	
}

const char* net_trajano_driver_InsomniaT::gKeySleepEnabled = "SleepEnabled";

bool net_trajano_driver_InsomniaT::isSleepEnabled() {
	return ((OSBoolean*)getProperty(gKeySleepEnabled))->getValue();
}

bool net_trajano_driver_InsomniaT::isSleepEnabledBySystem() {
	IOPMrootDomain *root = getPMRootDomain();
	return 	root->getProperty(kAppleClamshellCausesSleepKey) == kOSBooleanTrue;
}
void net_trajano_driver_InsomniaT::enableSleep() {
	IOPMrootDomain *root = getPMRootDomain();
	
	root->setProperty(kAppleClamshellCausesSleepKey,fAppleClamshellCausesSleep);
		// Calling this method will set the ignoringClamShell to false for the PM root domain.
	root->receivePowerNotification(kIOPMEnableClamshell);
}

IOReturn net_trajano_driver_InsomniaT::setSleepEnabled(bool sleepEnabled) {
	setProperty(gKeySleepEnabled, sleepEnabled);
	if (sleepEnabled) {
		enableSleep();
	} else {
		disableSleep();
	}
	return true;
}

/**
 * This is called when the kext is being unloaded.  It will remove the notifier handler
 * and enable sleep.
 */
void net_trajano_driver_InsomniaT::stop(IOService *provider)
{
	fNotifier->remove();
	enableSleep();
	super::stop(provider);
}
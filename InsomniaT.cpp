#include <string.h>
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
		if (obj->isSleepEnabledBySystem()){
			obj->disableSleep();
		} 
	}
	return 0;
}


bool net_trajano_driver_InsomniaT::init(OSDictionary *dict)
{
    bool res = super::init(dict);	
    return res;
}

void net_trajano_driver_InsomniaT::free(void)
{
    super::free();
}

IOService *net_trajano_driver_InsomniaT::probe(IOService *provider, SInt32
											   *score)
{
    IOService *res = super::probe(provider, score);
    return res;
}
bool net_trajano_driver_InsomniaT::start(IOService *provider)
{
    bool res = super::start(provider);
	IOPMrootDomain *root = getPMRootDomain();
	
	setProperty("Active", true);
	
	
	fAppleClamshellCausesSleep = root->getProperty(kAppleClamshellCausesSleepKey);
	fNotifier = registerSleepWakeInterest(handleSleepWakeInterest, this);
	disableSleep();
	return res;
}
void net_trajano_driver_InsomniaT::disableSleep() {
	IOPMrootDomain *root = getPMRootDomain();
	root->setProperty(kAppleClamshellCausesSleepKey,kOSBooleanFalse);
		// Calling this method will set the ignoringClamShell to true for the PM root domain.
	root->receivePowerNotification(kIOPMDisableClamshell);
	
}

const OSString* net_trajano_driver_InsomniaT::gKeySleepEnabled = OSString::withCStringNoCopy("SleepEnabled");

bool net_trajano_driver_InsomniaT::isSleepEnabled() {
	return ((OSBoolean*)getProperty("Active"))->getValue();
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
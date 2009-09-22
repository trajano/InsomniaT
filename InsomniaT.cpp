#include <string.h>
#include <IOKit/IOLib.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include "InsomniaT.h"

#define super IOService

static const char *kLogFormat = "[net.trajano.driver.InsomniaT] %s\n";
OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaT, IOService)


const OSSymbol *sleepdisabled_string =
OSSymbol::withCString("SleepDisabled");


extern "C" {
	
	IOReturn mySleepHandler( void * target, void * refCon,
							UInt32 messageType, IOService * provider,
							void * messageArgument, vm_size_t argSize )
	{
		net_trajano_driver_InsomniaT *obj = (net_trajano_driver_InsomniaT*)target;
		if (messageType == kIOPMMessageClamshellStateChange) {
			IOLog(kLogFormat,"kIOPMMessageClamshellStateChange received");
			if (	obj->isSleepEnabled()){
				IOLog(kLogFormat,"sleep was enabled. disabling sleep");
				obj->disableSleep();
			} else {
				IOLog(kLogFormat,"sleep was disabled doing nothing");
				
			}}
		return 0;
	}
}


bool net_trajano_driver_InsomniaT::init(OSDictionary *dict)
{
    bool res = super::init(dict);
    IOLog(kLogFormat, "Initializing");
	
    return res;
}

void net_trajano_driver_InsomniaT::free(void)
{
    IOLog(kLogFormat, "Freeing");
    super::free();
}

IOService *net_trajano_driver_InsomniaT::probe(IOService *provider, SInt32
											   *score)
{
    IOService *res = super::probe(provider, score);
    IOLog(kLogFormat,"Probing");
    return res;
}

bool net_trajano_driver_InsomniaT::start(IOService *provider)
{
    bool res = super::start(provider);
	IOLog(kLogFormat, "Starting");
	/*
	 fWorkloop = IOWorkLoop::workLoop();
	 fGate = IOCommandGate::commandGate(this);
	 
	 if (fGate && fWorkloop) {
	 fWorkloop->addEventSource(fGate);
	 }
	 */
	
	PMinit();
		// provider->joinPMtree(this);
	
	
		// registerPowerDriver (this, myPowerStates, kMyNumberOfStates);
	
	IOPMrootDomain *root = getPMRootDomain();
	
	fDefaultSleepSupportedFlags = root->getSleepSupported();
	disableSleep();
	return res;
}
void net_trajano_driver_InsomniaT::disableSleep() {
	IOLog(kLogFormat, "Disabling sleep");
	
	notifier = registerSleepWakeInterest(
										 &mySleepHandler, this, NULL);
	
	
	IOPMrootDomain *root = getPMRootDomain();
	root->setSleepSupported(fDefaultSleepSupportedFlags | kPCICantSleep);
	
	root->setProperty(kAppleClamshellCausesSleepKey,kOSBooleanFalse);
		// Calling this method will set the ignoringClamShell to true for the PM root domain.
	root->receivePowerNotification(kIOPMDisableClamshell);
	
}
/**
 * Used the positive form to prevent negatives in the method name for clarity.  This method was created to prevent resending the disable message to the root PM context when it is not needed.
 */
bool net_trajano_driver_InsomniaT::isSleepEnabled() {
	IOPMrootDomain *root = getPMRootDomain();
	return 	root->getProperty(kAppleClamshellCausesSleepKey) == kOSBooleanTrue;
}
void net_trajano_driver_InsomniaT::enableSleep() {
	IOLog(kLogFormat, "Enabling sleep");
	notifier->remove();
	
	IOPMrootDomain *root = getPMRootDomain();
		// This may be all that is needed. Should store the original value during init.
	root->setProperty(kAppleClamshellCausesSleepKey,kOSBooleanTrue);
	
	root->setSleepSupported(fDefaultSleepSupportedFlags);
		// Calling this method will set the ignoringClamShell to false for the PM root domain.
	root->receivePowerNotification(kIOPMEnableClamshell);
}
void net_trajano_driver_InsomniaT::stop(IOService *provider)
{
	IOLog(kLogFormat, "Stopping");
	enableSleep();
	PMstop();
	super::stop(provider);
}
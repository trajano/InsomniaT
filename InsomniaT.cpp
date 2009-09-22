#include <string.h>
#include <IOKit/IOLib.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include "InsomniaT.h"

#define super IOService

static const char *kLogFormat = "[net.trajano.driver.InsomniaT] %s\n";
OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaT, IOService)

static const int kMyNumberOfStates = 1;
static IOPMPowerState myPowerStates[kMyNumberOfStates] = {
	{1, kIOPMPreventIdleSleep, kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0}
};

const OSSymbol *sleepdisabled_string =
OSSymbol::withCString("SleepDisabled");

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
	provider->joinPMtree(this);
	
	
	registerPowerDriver (this, myPowerStates, kMyNumberOfStates);
	
	IOPMrootDomain *root = getPMRootDomain();
	
	fDefaultSleepSupportedFlags = root->getSleepSupported();
	disableSleep();
	return res;
}
IOReturn net_trajano_driver_InsomniaT::message( UInt32 type, IOService * provider,
											   void * argument  ) {
	if (type == kIOPMMessageClamshellStateChange) {
		IOLog(kLogFormat,"kIOPMMessageClamshellStateChange received");
		if (isSleepEnabled()) {
			IOLog(kLogFormat,"sleep was enabled. disabling sleep");
			disableSleep();
		} else {
			IOLog(kLogFormat,"sleep was disabled doing nothing");
			
		}}
	return super::message(type,provider,argument);
}
void net_trajano_driver_InsomniaT::disableSleep() {
	IOPMrootDomain *root = getPMRootDomain();
	root->setSleepSupported(fDefaultSleepSupportedFlags | kPCICantSleep);
	
	root->setProperty(sleepdisabled_string,kOSBooleanTrue);
		// Calling this method will set the ignoringClamShell to true for the PM root domain.
	root->receivePowerNotification(kIOPMDisableClamshell);
	
}
/**
 * Used the positive form to prevent negatives in the method name for clarity.  This method was created to prevent resending the disable message to the root PM context when it is not needed.
 */
bool net_trajano_driver_InsomniaT::isSleepEnabled() {
	IOPMrootDomain *root = getPMRootDomain();
	return 	root->getProperty(sleepdisabled_string) != kOSBooleanTrue;
}
void net_trajano_driver_InsomniaT::enableSleep() {
	IOPMrootDomain *root = getPMRootDomain();
		// This may be all that is needed. Should store the original value during init.
	root->setProperty(sleepdisabled_string,kOSBooleanFalse);
	
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
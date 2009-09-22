#include <IOKit/IOLib.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include "InsomniaT.h"

#define super IOService

OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaT, IOService)

bool net_trajano_driver_InsomniaT::init(OSDictionary *dict)
{
    bool res = super::init(dict);
    IOLog("Initializing\n");
    IOPMrootDomain *root = getPMRootDomain();
	defaultSleepSupportedFlags = root->getSleepSupported();


    return res;
}

void net_trajano_driver_InsomniaT::free(void)
{
    IOLog("Freeing\n");
    super::free();
}

IOService *net_trajano_driver_InsomniaT::probe(IOService *provider, SInt32
											   *score)
{
    IOService *res = super::probe(provider, score);
    IOLog("Probing\n");
    return res;
}

bool net_trajano_driver_InsomniaT::start(IOService *provider)
{
    bool res = super::start(provider);
    IOLog("Starting\n");
   PMinit();

	IOPMrootDomain *root = getPMRootDomain();
	root->setSleepSupported(defaultSleepSupportedFlags | kPCICantSleep);
	
	// Calling this method will set the ignoringClamShell to true for the PM root domain.
    root->receivePowerNotification(kIOPMDisableClamshell);

	return res;
}

void net_trajano_driver_InsomniaT::stop(IOService *provider)
{
    IOLog("Stopping\n");
    IOPMrootDomain *root = getPMRootDomain();
	root->setSleepSupported(defaultSleepSupportedFlags);
	// Calling this method will set the ignoringClamShell to false for the PM root domain.
    root->receivePowerNotification(kIOPMEnableClamshell);

	PMstop();
    super::stop(provider);
}
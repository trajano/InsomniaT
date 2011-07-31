/*
 *  InsomniaTUserClient.cpp
 *  InsomniaT
 *
 *  Created by Archimedes Trajano on 2009-09-24.
 *  Copyright 2009 trajano.net. All rights reserved.
 *
 */

#include <IOKit/IOLib.h>

#include "InsomniaTUserClient.h"

#ifndef DEBUG
#define IOLog(x, ...)
#endif

#define super IOUserClient

OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaTUserClient, IOUserClient)

// start is called after initWithTask as a result of the user process calling IOServiceOpen.
bool net_trajano_driver_InsomniaTUserClient::start(IOService* provider)
{
    IOLog("InsomniaTClient: start\n");
    // Verify that this user client is being started with a provider that it knows
    // how to communicate with.
    fProvider = OSDynamicCast(net_trajano_driver_InsomniaT, provider);
    bool success = (fProvider != NULL && fProvider->open(this));
    
    if (success) {
        // It's important not to call super::start if some previous condition
        // (like an invalid provider) would cause this function to return false. 
        // I/O Kit won't call stop on an object if its start function returned false.
        success = super::start(provider);
    }
    return success;
}

IOReturn net_trajano_driver_InsomniaTUserClient::externalMethod( uint32_t selector, IOExternalMethodArguments * arguments,
																IOExternalMethodDispatch * dispatch , OSObject * target , void * reference ) {
    IOLog("InsomniaTClient: externalMethod selector = %d\n", selector);
    IOLog("InsomniaTClient: provider = %p\n", fProvider);
    IOLog("InsomniaTClient: target = %p\n", target);
    IOLog("InsomniaTClient: reference = %p\n", reference);
	if (selector == 1 ) {
		fProvider->enableSleepOnClamshellClose();
		return kIOReturnSuccess;
	} 
	
	if (selector == 2 ) {
		fProvider->disableSleepOnClamshellClose();
		return kIOReturnSuccess;
	}
	
	if (selector == 3 && arguments->scalarOutputCount == 1 && arguments->scalarOutput != NULL) {
		if (fProvider->isClamshellCloseCausesSleep()) {
			arguments->scalarOutput[0] = 1;
		} else {
			arguments->scalarOutput[0] = 0;
		}
		return kIOReturnSuccess;
	} 
    return kIOReturnSuccess;
}

bool net_trajano_driver_InsomniaTUserClient::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
    IOLog("InsomniaTClient: didTerminate\n");    
    return super::didTerminate(provider, options, defer);
}

void net_trajano_driver_InsomniaTUserClient::stop(IOService* provider)
{    
    IOLog("InsomniaTClient: stop %p\n", provider);
    
    if (provider->isOpen(this)) {
        provider->close(this);
    }
    else {
        IOLog("InsomniaT: client did not open provider.\n");
    }
    super::stop(provider);
}

IOReturn net_trajano_driver_InsomniaTUserClient::clientClose()
{
    IOLog("InsomniaTClient: clientClose\n");
    if( !isInactive()) {
        terminate();
    }
    
    return kIOReturnSuccess;
}

IOReturn net_trajano_driver_InsomniaTUserClient::registerNotificationPort(
                                                                          mach_port_t port, UInt32 type, io_user_reference_t refCon) {
    IOLog("InsomniaTClient: registerNotificationPort, %p, %p, %p\n", port, type, refCon);
    return kIOReturnSuccess;
}

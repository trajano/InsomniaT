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

#define super IOUserClient

OSDefineMetaClassAndStructors(net_trajano_driver_InsomniaTUserClient, IOUserClient)

	// start is called after initWithTask as a result of the user process calling IOServiceOpen.
bool net_trajano_driver_InsomniaTUserClient::start(IOService* provider)
{
    bool    success;
    
		// Verify that this user client is being started with a provider that it knows
		// how to communicate with.
    fProvider = OSDynamicCast(net_trajano_driver_InsomniaT, provider);
    success = (fProvider != NULL);
    
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
	if (fProvider->isLoggingEnabled()) {
		IOLog("InsomniaT: external method selector is %d\n", selector);
	}
	if (selector == 1 ) {
		fProvider->setSleepEnabled(true);
		return kIOReturnSuccess;
	} 
	
	if (selector == 2 ) {
		fProvider->setSleepEnabled(false);
		return kIOReturnSuccess;
	} 
	
	if (selector == 3 && arguments->scalarOutputCount == 1 && arguments->scalarOutput != NULL) {
		if (fProvider->isSleepEnabled()) {
			arguments->scalarOutput[0] = 1;
		} else {
			arguments->scalarOutput[0] = 0;
		}
		return kIOReturnSuccess;
	} 
	
	
	if (selector == 4 ) {
		fProvider->setLoggingEnabled(true);
		return kIOReturnSuccess;
	} 
	
	if (selector == 5 ) {
		fProvider->setLoggingEnabled(false);
		return kIOReturnSuccess;
	} 
	
	if (selector == 6 && arguments->scalarOutputCount == 1 && arguments->scalarOutput != NULL) {
		if (fProvider->isLoggingEnabled()) {
			arguments->scalarOutput[0] = 1;
		} else {
			arguments->scalarOutput[0] = 0;
		}
		return kIOReturnSuccess;
	} 
	
    IOLog("InsomniaT: unknown method selector %d -- ignoring\n", selector);
    return kIOReturnBadArgument;	
}
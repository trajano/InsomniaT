/*
 *  InsomniaTUserClient.h
 *  InsomniaT
 *
 *  Created by Archimedes Trajano on 2009-09-24.
 *  Copyright 2009 trajano.net. All rights reserved.
 *
 */

#include <IOKit/IOUserClient.h>

#include "InsomniaT.h"

class net_trajano_driver_InsomniaTUserClient : public IOUserClient
{
	OSDeclareDefaultStructors(net_trajano_driver_InsomniaTUserClient)
	
private:
	net_trajano_driver_InsomniaT* fProvider;
public:
    virtual bool start(IOService* provider);
	
	/**
	 * If selector is 1, it invokes the setSleepEnabled method.
	 * If selector is 2, it invokes the isSleepEnabled method.
	 */
	virtual IOReturn externalMethod( uint32_t selector, IOExternalMethodArguments * arguments,
									IOExternalMethodDispatch * dispatch = 0, OSObject * target = 0, void * reference = 0 );
};


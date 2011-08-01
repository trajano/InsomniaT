#ifndef IOUserClient_H
#define IOUserClient_H
#include <IOKit/IOUserClient.h>

#include "InsomniaT.h"

/**
 * This is the IOUserClient that communicates with the InsomniaT service.
 */
class net_trajano_driver_InsomniaTUserClient : public IOUserClient
{
	OSDeclareDefaultStructors(net_trajano_driver_InsomniaTUserClient)

private:
	net_trajano_driver_InsomniaT* fProvider;
public:
    virtual bool start(IOService* provider);

    /**
     * Invoked when the IOUserClient has stopped.
     */
    virtual void stop(IOService *provider);	
    
    virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer);
    virtual IOReturn clientClose();
	/**
	 * If selector is 1, it invokes the setSleepEnabled(true)
	 * If selector is 2, it invokes the setSleepEnabled(false)
	 * If selector is 3, it invokes the isSleepEnabled method requires one scalar output argument 
	 * If selector is 4, it invokes the setLoggingEnabled(true)
	 * If selector is 5, it invokes the setLoggingEnabled(false)
	 * If selector is 6, it invokes the isLoggingEnabled method requires one scalar output argument 
	 */
	virtual IOReturn externalMethod( uint32_t selector, IOExternalMethodArguments * arguments,
									IOExternalMethodDispatch * dispatch = 0, OSObject * target = 0, void * reference = 0 );
};

#endif

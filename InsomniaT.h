#include <IOKit/IOService.h>

class net_trajano_driver_InsomniaT : public IOService
{
	OSDeclareDefaultStructors(net_trajano_driver_InsomniaT)
private:
	IOOptionBits fDefaultSleepSupportedFlags;
		//	IOWorkloop *fWorkLoop;
	virtual void disableSleep();
	virtual void enableSleep();
	virtual bool isSleepEnabled();
	
public:
    virtual bool init(OSDictionary *dictionary = NULL);
    virtual void free(void);
    virtual IOReturn message( UInt32 type, IOService * provider,
							 void * argument = 0 );
   virtual IOService *probe(IOService *provider, SInt32 *score);
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
};
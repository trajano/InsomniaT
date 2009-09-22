#include <IOKit/IOService.h>

class net_trajano_driver_InsomniaT : public IOService
{
	OSDeclareDefaultStructors(net_trajano_driver_InsomniaT)
private:
	IOOptionBits fDefaultSleepSupportedFlags;
	IONotifier* notifier;
		//	IOWorkloop *fWorkLoop;
	
public:
    virtual bool init(OSDictionary *dictionary = NULL);
    virtual void free(void);
	virtual IOService *probe(IOService *provider, SInt32 *score);
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
	
		// TODO convert externed method to a friend so we do not expose these methods
	virtual bool isSleepEnabled();
	virtual void disableSleep();
	virtual void enableSleep();
	
};
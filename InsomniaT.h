#include <IOKit/IOService.h>

class net_trajano_driver_InsomniaT : public IOService
{
	OSDeclareDefaultStructors(net_trajano_driver_InsomniaT)
	friend IOReturn handleSleepWakeInterest( void * target, void * refCon,
											UInt32 messageType, IOService * provider,
											void * messageArgument, vm_size_t argSize );
private:
	OSObject* fAppleClamshellCausesSleep;
	IONotifier* fNotifier;
	
	/**
	 * Checks if the system itself allows for sleep.
	 * Used the positive form to prevent negatives in the method name for clarity.  This method was created to prevent resending the disable message to the root PM context when it is not needed.
	 */
	virtual bool isSleepEnabledBySystem();
	virtual void disableSleep();
	virtual void enableSleep();
	
public:
		// virtual bool init(OSDictionary *dictionary = NULL);
		//virtual void free(void);
		//virtual IOService *probe(IOService *provider, SInt32 *score);
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
	
	/**
	 * Checks the IORegistry value "SleepEnabled" to see if sleep is enabled.
	 */
	virtual bool isSleepEnabled();
	virtual IOReturn setSleepEnabled(bool sleepEnabled);
	
	static const char* gKeySleepEnabled;
};
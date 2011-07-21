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
     * The AppleBacklightDisplay.  There should be only one or NULL.  Once it
     * has been set, it should be kept.
     */
	IOService* appleBacklightDisplay;

    /**
     * The Built-in ISight device.  There should be only one or NULL.  Once it
     * has been set, it should be kept.
     */
    IOService* builtInISight;
	/**
	 * Checks if the system itself allows for sleep.
	 * Used the positive form to prevent negatives in the method name for
     * clarity.  This method was created to prevent resending the disable
     * message to the root PM context when it is not needed.
	 */
	virtual bool isSleepEnabledBySystem();

	/**
	 * Checks if multiple displays are connected to the system.  If so then
     * sleep needs to be disabled.
	 */
	virtual bool isMultipleDisplays();
    
    /**
     * This gets the AppleBacklightDisplay.  There should be only one.
     */
	virtual IOService* getAppleBacklightDisplay();

    /**
     * This gets the Built-in ISight.  There should be only one.
     */
	virtual IOService* getBuiltInISight();
	virtual void disableSleep();
	virtual void enableSleep();
	
	/**
	 * This updates the system to force sleep enable or disable based on the registry value.
	 */
	virtual void updateSystemSleep();
	
public:
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
	
	/**
	 * Checks the IORegistry value "SleepEnabled" to see if sleep is enabled.
	 */
	virtual bool isSleepEnabled();
	virtual IOReturn setSleepEnabled(bool sleepEnabled);

	/**
	 * Checks the IORegistry value "LoggingEnabled" to see if sleep is enabled.
	 */
	virtual bool isLoggingEnabled();
	virtual IOReturn setLoggingEnabled(bool loggingEnabled);
	
	static const char* gKeySleepEnabled;
	static const char* gKeyLoggingEnabled;
};
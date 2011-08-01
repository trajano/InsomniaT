#ifndef INSOMNIAT_H
#define INSOMNIAT_H
#include <IOKit/IOService.h>

/**
 * This is the IOService that is used to prevent a MacBook from sleeping when
 * the lid is closed.
 */
class net_trajano_driver_InsomniaT : public IOService {
    OSDeclareDefaultStructors(net_trajano_driver_InsomniaT);
    
    /**
     * This handles the event when there is a sleep wake event including the
     * notification where the clamshell is open and closed.  This is meant
     * to specifically handle events where the the device should be turned
     * off when the clamshell is closed.
     *
     * It turns off the IOService by setting the power state to 0x0.  It 
     * turns it on by setting the power state to 0xFFFFFFFF.
     *
     * @param target this is an IOService.
     * @param provider this should be IOPMrootDomain.
     */
	friend IOReturn handleIOServiceSleepWakeInterest(void *target, void *refCon,
                                                     UInt32 messageType, IOService *provider,
                                                     void *messageArgument, vm_size_t argSize );
    /**
     * This handles the event when there is a sleep wake event including the
     * notification where the clamshell is open and closed.
     *
     * If the lid is closed, it will locate and disable the light sensor.
     * If the lid is open, it will locate and enable the light sensor.
     *
     * If sleepOnClamshellClose, the system clamshell sleep state is false,
     * and only the AppleBacklightDisplay is connected, then this will enable
     * sleep.
     *
     * If not sleepOnClamshellClose and the system clamshell sleep state is true,
     * this will disable sleep.
     *
     * @param target this is the InsomniaT service.
     * @param provider this should be IOPMrootDomain.
     */
	friend IOReturn handleSleepWakeInterest(void *target, void *refCon,
											UInt32 messageType, IOService *provider,
											void *messageArgument, vm_size_t argSize );
private:
    /**
     * This points to the notifier that will be triggered when a clamshell
     * event has occurred.  It will be NULL when sleep on clamshel is enabled
     * but defined when sleep on clamshell is disabled.
     */
    IONotifier *clamshellNotifier;
    
    /**
     * This points to the notifier that will be triggered when an event happens
     * on the Apple Backlight display.  Primarily this is to handle the case when the 
     * backlight display turns on when the lid is closed.
     */
    IONotifier *appleBacklightDisplayNotifier;
    
    /**
     * This points to the notifier that will be triggered when an event happens
     * on the Apple LMU controller.  Primarily this is to handle the case when the 
     * backlight display turns on when the lid is closed.
     */
    IONotifier *appleLMUControllerNotifier;
    
    /**
     * Sleep on clamshell close.
     */
    bool sleepOnClamshellClose;
public:
    /**
     * Invoked when the IOService has initialized.
     * 
     * This will set the class variables to its initial state.  Specifically
     * sleepOnClamshellClose = true.
     */
    virtual bool init(OSDictionary* dictionary = 0);
    
    /**
     * Invoked when the IOService has started.  Only occurs once per kext
     * load.
     * 
     * It waits for 20 seconds for the AppleBacklightDisplay and the
     * AppleLMUController to exist before registering any notifiers.  If either
     * the AppleBacklightDisplay or AppleLMUController services do not exist
     * within the timeout period, this will return false.
     *
     * This will register the sleep wake handlers and determine the power state
     * for the AppleLMUController and the AppleBacklightDisplay.  If any handler
     * was not registered successfully, this will return false.
     *
     * @return true if the service wast started successfuly, false otherwise.
     */
    virtual bool start(IOService *provider);
    
    /**
     * Invoked when a client connects to the service.
     */
    virtual bool open(IOService *forClient, IOOptionBits options = 0, void *arg = 0);
    
    /**
     * Invoked when a client releases from the service.
     */
    virtual void close(IOService *forClient, IOOptionBits options = 0);
    
    /**
     * Invoked when the IOService has stopped.  Only occurs once per kext
     * load.
     *
     * This will deregister the sleep wake handlers and reset the
     * AppleLMUController power state back to the value that was obtained
     * on start().
     */
    virtual void stop(IOService *provider);	
    
    /**
     * This will enable sleep on clamshell close.
     *
     * It will remove the clamshellNotifier and set it to NULL if it is defined.
     */
    virtual void enableSleepOnClamshellClose();
    
    /**
     * This will disable sleep on clamshell close.
     *
     * It will remove the clamshellNotifier and set it to NULL if it is defined.
     * Afterewards, it will register a sleep/wake interest using the
     * handleSleepWakeInterest function and assign it to clamshellNotifier.
     */
    virtual void disableSleepOnClamshellClose();
    
    /**
     * This checks whether clamshell close causes sleep to be triggered.
     *
     * It determines this by checking whether sleepOnClamshellClose is true
     * or not.  It only checks sleepOnClamshellClose, not whether there
     * are multiple displays or whether the system deterimines that clamshell
     * state would be ignored or not.
     *
     * @return true if the sleepOnClamshellClose is tr, false otheruewise.
     */
    virtual bool isClamshellCloseCausesSleep() const;
};
#endif

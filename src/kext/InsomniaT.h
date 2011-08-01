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
     * notification where the clamshell is open and closed.
     *
     * This only handles kIOPMMessageClamshellStateChange.  Any other
     * messageType will just log the event for debugging purposes.
     *
     * If the lid is closed, it will locate and disable the light sensor and the
     * display.
     *
     * If the lid is open, it will locate and enable the light sensor and the
     * display.
     *
     * If sleepOnClamshellClose, the system clamshell sleep state is false,
     * and only the AppleBacklightDisplay is connected, then this will enable
     * sleep.
     *
     * If not sleepOnClamshellClose and the system clamshell sleep state is true,
     * this will disable sleep.
     *
     * This function was made a friend in order for it to get the value of
     * sleepOnClamshellClose.
     *
     * @param target this is the InsomniaT service.
     * @param refCon reference constant that is passed when the notification was created.  Should be NULL.
     * @param messageType message type.
     * @param provider this should be IOPMrootDomain.
     * @param messageArgument extra information for the message.
     * @param argSize size of the message argument
     * @return kIOReturnSuccess if everything went well.
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
     * Sleep on clamshell close flag.
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
     * This will register the sleep wake handler.  If the handler
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
     * This will deregister the sleep wake handlers and turn on the
     * AppleLMUController.
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

//
//  InsomniaTPrefController.m
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-09.
//  Copyright 2010 trajano.net. All rights reserved.
//

#import "InsomniaTPrefController.h"
#include "unistd.h"


@implementation InsomniaTPrefController

/**
 * Initializes the controller.  It should create an instance of the
 * model (ideally the model should be passed into here as a parameter
 * but I don't know the life cycle well enough yet to do that).
 *
 * It should also register itself as an observer to the model using the
 * KVO pattern.
 */
- (id) init {
	if ( self = [super init] ) {
        // By design automatic garbage collection is
        // not used.  Not necessarily for efficiency,
        // but for learning purposes.
		insomniaTstatus = [[InsomniaTStatus alloc]init];
		[insomniaTstatus addObserver: self
						  forKeyPath: @"insomniaTEnabled"
							 options:(NSKeyValueObservingOptionNew |
									  NSKeyValueObservingOptionOld)
							 context:NULL];
	}
	return self;
} 

- (void) finalize {
	[insomniaTstatus removeObserver: self
						 forKeyPath: @"insomniaTEnabled"];
	[super finalize];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
    if ([keyPath isEqual:@"insomniaTEnabled"]) {
        //	bool insomniaTEnabled = [change objectForKey: NSKeyValueChangeNewKey];
		[self updateStatus: nil];
		
    }
	
    // be sure to call the super implementation	
    // if the superclass implements it
	
    [super observeValueForKeyPath:keyPath
						 ofObject:object
						   change:change
						  context:context];
	
	
}
- (IBAction) updateStatus:(id) sender {
	if ([[insomniaTstatus insomniaTEnabled] unsignedIntValue] == 0) {
		[statusLevel setIntValue: 1];
		[statusLevelText setTitleWithMnemonic: @"InsomniaT: On"];
		[statusLevelBlurb setTitleWithMnemonic: @"The MacBook will not suspend when the lid closed."];
		[startStopButton setTitle: @"Stop"];
		[startStopButtonBlurb setTitleWithMnemonic: @"Click Stop to turn InsominaT off"];
		[startStopButton setEnabled: true];
    } else if ([[insomniaTstatus insomniaTEnabled] unsignedIntValue] == 1) {
		[statusLevel setIntValue: 0];
		[statusLevelText setTitleWithMnemonic: @"InsomniaT: Off"];
		[statusLevelBlurb setTitleWithMnemonic: @"The MacBook will suspend when the lid closed."];
		[startStopButton setTitle: @"Start"];
		[startStopButtonBlurb setTitleWithMnemonic: @"Click Start to turn InsominaT on"];
		[startStopButton setEnabled: true];
    } else {
		[statusLevel setIntValue: 2];
		[statusLevelText setTitleWithMnemonic: @"InsomniaT: Unknown"];
		[statusLevelBlurb setTitleWithMnemonic: @"InsomniaT kernel module is not installed"];
		[startStopButton setTitle: @"Start"];
		[startStopButtonBlurb setTitleWithMnemonic: @"Unable to start as InsomniaT kernel extension is not installed."];
		[startStopButton setEnabled: false];
		[uninstallButtonBlurb setTitleWithMnemonic: @"InsomniaT is already uninstalled, please remove the preference pane"];
    }
}
- (IBAction)startStop:(id)sender {
	if ([[insomniaTstatus insomniaTEnabled] unsignedIntValue] == 1) {
		[startStopButton setTitle: @"Stopping..."];
		[startStopButtonBlurb setTitleWithMnemonic: @"Please wait, trying to stop InsomniaT ..."];
		[startStopButton setEnabled: false];
		[insomniaTstatus disableInsomniaT];
	} else {
		[startStopButton setTitle: @"Starting..."];
		[startStopButtonBlurb setTitleWithMnemonic: @"Please wait, trying to start InsomniaT ..."];
		[startStopButton setEnabled: false];
		[insomniaTstatus enableInsomniaT];
	}
}
- (IBAction)uninstall:(id)sender {
    AuthorizationRef myAuthorizationRef;
    OSStatus osStatus = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment,kAuthorizationFlagDefaults,&myAuthorizationRef);
    if (osStatus != errAuthorizationSuccess) {
        return;
    }
    
    AuthorizationItem myItems = {kAuthorizationRightExecute, 0,   
        NULL, 0};
    
    AuthorizationRights myRights = {1, &myItems};                 
    
    
    AuthorizationFlags myFlags = kAuthorizationFlagDefaults |                        
    kAuthorizationFlagInteractionAllowed |    
    kAuthorizationFlagPreAuthorize |    
    kAuthorizationFlagExtendRights;
    
    OSStatus copyStatus = AuthorizationCopyRights (myAuthorizationRef, &myRights, NULL, myFlags, NULL );
    if (copyStatus != errAuthorizationSuccess) {
        return;
    }
    
	if ([[insomniaTstatus insomniaTEnabled] unsignedIntValue] == 1) {
		[startStopButton setTitle: @"Stopping..."];
		[startStopButtonBlurb setTitleWithMnemonic: @"Please wait, trying to stop InsomniaT ..."];
		[startStopButton setEnabled: false];
		[insomniaTstatus disableInsomniaT];
	} 
    if (access("/System/Library/Extensions/InsomniaT.kext", R_OK)) {
        char *args[] = { "/System/Library/Extensions/InsomniaT.kext", NULL };
        const OSStatus status = AuthorizationExecuteWithPrivileges(myAuthorizationRef, "/sbin/kextunload", myFlags, args, NULL);
        if (status != errAuthorizationSuccess) {
            NSBeep();
        }
    }
    if (access("/System/Library/Extensions/InsomniaT (10.5).kext", R_OK)) {
        char *args[] = { "/System/Library/Extensions/InsomniaT (10.5).kext", NULL };
        const OSStatus status = AuthorizationExecuteWithPrivileges(myAuthorizationRef, "/sbin/kextunload", myFlags, args, NULL);
        if (status != errAuthorizationSuccess) {
            NSBeep();
        }
    }
    if (access("/System/Library/Extensions/InsomniaT.kext", R_OK)) {
        char *args[] = { "-rf", "/System/Library/Extensions/InsomniaT.kext", NULL };
        AuthorizationExecuteWithPrivileges(myAuthorizationRef, "/bin/rm", myFlags, args, NULL);
    }
    if (access("/System/Library/Extensions/InsomniaT (10.5).kext", R_OK)) {
        char *args[] = { "-rf", "/System/Library/Extensions/InsomniaT (10.5).kext", NULL };
        AuthorizationExecuteWithPrivileges(myAuthorizationRef, "/bin/rm", myFlags, args, NULL);
    }
    if (access("/usr/local/bin", R_OK)) {
        char *args[] = { "-rf", "/usr/local/bin/insomniat", NULL };
        AuthorizationExecuteWithPrivileges(myAuthorizationRef, "/bin/rm", myFlags, args, NULL);
    }
    AuthorizationFree(myAuthorizationRef, kAuthorizationFlagDestroyRights);

    // Do a disable to trigger a change in the status.
    [insomniaTstatus disableInsomniaT];
}
@end

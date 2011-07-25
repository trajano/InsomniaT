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
		
    } else {
        
        // be sure to call the super implementation	
        // if the superclass implements it
        
        [super observeValueForKeyPath:keyPath
                             ofObject:object
                               change:change
                              context:context];
	}
	
}
- (IBAction) updateStatus:(id) sender {
    
    SInt32 sysv;
    Gestalt(gestaltSystemVersion, &sysv);

    if (sysv >= 0x1070) {
        [automaticBrightnessBlurb setTitleWithMnemonic: @"Please note that if \"Automatically adjust brightness\" is enabled in the Displays preference pane then the backlight will turn on temporarily for a few seconds before switching off again."];
    } else {
        [automaticBrightnessBlurb setTitleWithMnemonic: @"Please note that if \"Automatically adjust brightness as ambient light changes\" is enabled in the Displays preference pane then the backlight will turn on temporarily for a few seconds before switching off again."];
    }
    
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
@end

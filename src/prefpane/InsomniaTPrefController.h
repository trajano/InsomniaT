//
//  InsomniaTPrefController.h
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-09.
//  Copyright 2010 trajano.net. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "InsomniaTStatus.h"

@interface InsomniaTPrefController : NSObject {
	IBOutlet NSLevelIndicator *statusLevel;
	IBOutlet NSTextField *statusLevelText;
	IBOutlet NSTextField *statusLevelBlurb;
	IBOutlet NSButton *startStopButton;
	IBOutlet NSTextField *startStopButtonBlurb;
	IBOutlet NSButton *uninstallButton;
	IBOutlet NSTextField *uninstallButtonBlurb;
	InsomniaTStatus *insomniaTstatus;
}
/*
 - (id) init;
- (void) dealloc;
- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context;
 */
- (IBAction) updateStatus:(id) sender;
- (IBAction) startStop:(id)sender;
- (IBAction) uninstall:(id)sender;
@end

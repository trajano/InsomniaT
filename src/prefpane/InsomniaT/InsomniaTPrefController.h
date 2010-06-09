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
- (void) updateStatus;
- (IBAction) startStop:(id)sender;
@end

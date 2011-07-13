//
//  InsomniaTPref.h
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-07.
//  Copyright (c) 2010 trajano.net. All rights reserved.
//

#import <PreferencePanes/PreferencePanes.h>


@interface InsomniaTPref : NSPreferencePane 
{
	IBOutlet NSLevelIndicator *statusLevel;
	IBOutlet NSTextField *statusLevelText;
	IBOutlet NSTextField *statusLevelBlurb;
	IBOutlet NSButton *startStopButton;
	IBOutlet NSTextField *startStopButtonBlurb;
	IBOutlet NSButton *uninstallButton;
	IBOutlet NSTextField *uninstallButtonBlurb;
}

- (void) mainViewDidLoad;

@end

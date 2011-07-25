//
//  InsomniaTPref.h
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-07.
//  Copyright (c) 2010 trajano.net. All rights reserved.
//

#import <PreferencePanes/PreferencePanes.h>
#import "InsomniaTPrefController.h"

@interface InsomniaTPref : NSPreferencePane 
{
    IBOutlet InsomniaTPrefController *controller;
}
- (void) willSelect;
- (void) didUnselect;

@end

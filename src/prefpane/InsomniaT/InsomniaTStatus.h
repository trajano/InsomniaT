//
//  InsomniaTStatus.h
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-08.
//  Copyright 2010 trajano.net. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface InsomniaTStatus : NSObject {
	// This is empty because there are no actual properties
	// that are stored.  Instead they are retrieved from IOKit.
}

/**
 * Send a message to InsomniaT IOKit driver to determine if it is
 * enabled or not.  True if insomniaT is enabled
 */
- (bool) insomniaTEnabled;

/**
 * Send a message to InsomniaT IOKit driver to enable itself.
 */
- (void) enableInsomniaT;

/**
 * Send a message to InsomniaT IOKit driver to disable itself.
 */
- (void) disableInsomniaT;

- (void)setNilValueForKey:(NSString *)theKey;
@end

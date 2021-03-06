//
//  InsomniaTStatus.h
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-08.
//  Copyright 2010 trajano.net. All rights reserved.
//

#import <Cocoa/Cocoa.h>

/**
 * InsomniaT status model.  This interfaces with the InsomniaT kernel extension.
 */
@interface InsomniaTStatus : NSObject {
    /**
     * This is zero if disabled. 
     * This is one if enabled.
     * This is two if there's a problem.
     */
	NSNumber* insomniaTEnabled;
}

/**
 * Send a message to InsomniaT IOKit driver to determine if it is
 * enabled or not.  True if insomniaT is enabled (meaning sleep is disabled)
 */
-(uint64_t) getInsomniaTStatusFromDriver;

@property(readwrite, assign) NSNumber* insomniaTEnabled;

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

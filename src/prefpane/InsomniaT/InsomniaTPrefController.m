//
//  InsomniaTPrefController.m
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-09.
//  Copyright 2010 trajano.net. All rights reserved.
//

#import "InsomniaTPrefController.h"


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

- (void) dealloc {
	[insomniaTstatus removeObserver: self
						 forKeyPath: @"insomniaTEnabled"];
	[insomniaTstatus release];
	[super dealloc];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
		// By design this is checked again to make sure
		// we get the latest status of InsomniaT IOKit
		// driver.
	if ([insomniaTstatus insomniaTEnabled]) {
		[statusLevel setIntValue: 1];
		[startStopButton setTitle: @"Stop"];
	} else {
		[statusLevel setIntValue: 0];
		[startStopButton setTitle: @"Start"];
	}
	
}
- (IBAction)startStop:(id)sender {
	if ([insomniaTstatus insomniaTEnabled]) {
		[insomniaTstatus disableInsomniaT];
	} else {
		[insomniaTstatus enableInsomniaT];
	}
}
@end

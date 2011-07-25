//
//  InsomniaTPref.m
//  InsomniaT
//
//  Created by Archimedes Trajano on 2010-06-07.
//  Copyright (c) 2010 trajano.net. All rights reserved.
//

#include <CoreServices/CoreServices.h>
#import "InsomniaTPref.h"

@implementation InsomniaTPref


- (void) willSelect {
    [controller updateStatus: self];
}

- (void) didUnselect {
}

@end

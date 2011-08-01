	//
	//  InsomniaTStatus.m
	//  InsomniaT
	//
	//  Created by Archimedes Trajano on 2010-06-08.
	//  Copyright 2010 trajano.net. All rights reserved.
	//

#import "InsomniaTStatus.h"


@implementation InsomniaTStatus

@synthesize insomniaTEnabled;

- (id) init {
	if (self = [super init]) {
		io_service_t    service;
		service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
		if (service == IO_OBJECT_NULL) {
			[self setValue: [NSNumber numberWithUnsignedInt: 2]
					forKey: @"insomniaTEnabled"];
		}
		
		io_connect_t connect;
		kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
		if (kernResult == KERN_SUCCESS) {
			
			uint64_t output[1];
			uint32_t count = 1;
			IOConnectCallScalarMethod(connect, 3, NULL, 0, output, &count);
			IOServiceClose(connect);

            if (output[0] == 1) {
                [self setValue: [NSNumber numberWithUnsignedInt: 1]
                        forKey: @"insomniaTEnabled"];
            } else {
                [self setValue: [NSNumber numberWithUnsignedInt: 0]
                        forKey: @"insomniaTEnabled"];
            }
		} else {
			[self setValue: [NSNumber numberWithUnsignedInt: 2]
					forKey: @"insomniaTEnabled"];
		}
	}
	return self;
}

-(uint64_t) getInsomniaTStatusFromDriver{
	NSLog(@"Retrieving status from driver");
		// By design this is replicated because we want to make sure we
		// always get the currently running driver rather than somethin
		// that may have been gone from a KExt restart or reinstall.
	io_service_t    service;
	service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
        return 2;
    }
	
	io_connect_t connect;
	kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
	if (kernResult == KERN_SUCCESS) {
		
		uint64_t output[1];
		uint32_t count = 1;
		IOConnectCallScalarMethod(connect, 3, NULL, 0, output, &count);
		IOServiceClose(connect);
        
        if (output[0] == 1) {
            return 1;
        } else {
            return 0;
        }
	} else {
		return 2;
	}
}

- (void) enableInsomniaT {
	const io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
		[self setValue: [NSNumber numberWithUnsignedInt: 2]
				forKey: @"insomniaTEnabled"];
        return;
    }
	
	io_connect_t connect;
	kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
	if (kernResult == KERN_SUCCESS) {
		
		uint64_t output[1];
		uint32_t count = 0;
		IOConnectCallScalarMethod(connect, 1, NULL, 0, output, &count);
		IOServiceClose(connect);
	}
    [self setValue: [NSNumber numberWithUnsignedInt:[self getInsomniaTStatusFromDriver]]
            forKey: @"insomniaTEnabled"];
}
- (void) disableInsomniaT {
	const io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
		[self setValue: [NSNumber numberWithUnsignedInt:[self getInsomniaTStatusFromDriver]]
				forKey: @"insomniaTEnabled"];
        return;
    }
	
	io_connect_t connect;
	kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
	if (kernResult == KERN_SUCCESS) {
		
		uint64_t output[1];
		uint32_t count = 0;
		IOConnectCallScalarMethod(connect, 2, NULL, 0, output, &count);
		IOServiceClose(connect);
	}
    [self setValue: [NSNumber numberWithUnsignedInt:[self getInsomniaTStatusFromDriver]]
            forKey: @"insomniaTEnabled"];
}

- (void)setNilValueForKey:(NSString *)theKey {
	if ([theKey isEqualToString:@"insomniaTEnabled"]) {
		[self setValue: [NSNumber numberWithUnsignedInt:[self getInsomniaTStatusFromDriver]]
				forKey: @"insomniaTEnabled"];
    } else {
        [super setNilValueForKey:theKey];
	}
}
@end

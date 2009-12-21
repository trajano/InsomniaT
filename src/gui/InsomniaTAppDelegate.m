//
//  CafeBabeAppDelegate.m
//  CafeBabe
//
//  Created by Archimedes Trajano on 2009-10-15.
//  Copyright 2009 trajano.net. All rights reserved.
//

#import "InsomniaTAppDelegate.h"
#import <IOKit/IOKitLib.h>

@implementation InsomniaTAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	io_service_t    service;
	service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
        return;
    }
	
	io_connect_t connect;
	kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
	if (kernResult == KERN_SUCCESS) {
		
		uint64_t output[1];
		uint32_t count = 1;
		IOConnectCallScalarMethod(connect, 2, NULL, 0, output, &count);
		IOServiceClose(connect);
	} else {
		return;
	}
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	io_service_t    service;
    service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
        return;
    }
	
	io_connect_t connect;
	kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
	if (kernResult == KERN_SUCCESS) {
		
		uint64_t output[1];
		uint32_t count = 1;
		IOConnectCallScalarMethod(connect, 1, NULL, 0, output, &count);
		IOServiceClose(connect);
	} else {
		return;
	}
}

@end

	//
	//  InsomniaTTest.m
	//  InsomniaT
	//
	//  Created by Archimedes Trajano on 2009-09-24.
	//  Copyright 2009 trajano.net. All rights reserved.
	//

#import "InsomniaTTest.h"


@implementation InsomniaTTest

/**
 * Do nothing sanity test to make sure that the test framework works.
 */
- (void) testSanity {
	STAssertEquals(1,1, @"Expected numbers to be equal");
 	
}

- (void) testCommand1and2 {
	io_service_t    service;
	
    service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	STAssertTrue(service != IO_OBJECT_NULL, @"Service was not found");

	
	io_connect_t connect;
	{
		kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
	}

	{
		kern_return_t kernResult = IOConnectCallScalarMethod(connect, 1, NULL, 0, NULL, 0);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
	}
	
	{
		kern_return_t kernResult = IOConnectCallScalarMethod(connect, 2, NULL, 0, NULL, 0);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
	}
	
	IOServiceClose(connect);
}

- (void) testCommand1and2and3 {
	io_service_t  service;
	
    service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	STAssertTrue(service != IO_OBJECT_NULL, @"Service was not found");
	
	
	io_connect_t connect;
	{
		kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
	}
	
	{
		kern_return_t kernResult = IOConnectCallScalarMethod(connect, 1, NULL, 0, NULL, 0);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
	}

	{
		uint64_t output[1];
		uint32_t count = 1;
		kern_return_t kernResult = IOConnectCallScalarMethod(connect, 3, NULL, 0, output, &count);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
		STAssertTrue(output[0] == 1, @"Expected value to be 1 (for true).");
	}
	
	{
		kern_return_t kernResult = IOConnectCallScalarMethod(connect, 2, NULL, 0, NULL, 0);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
	}
	
	{
		uint64_t output[1];
		uint32_t count = 1;
		kern_return_t kernResult = IOConnectCallScalarMethod(connect, 3, NULL, 0, output, &count);
		STAssertEquals(kernResult,KERN_SUCCESS, @"Expected kernel to return successfully.");
		STAssertTrue(output[0] == 0, @"Expected value to be 0 (for false).");
	}
	
	IOServiceClose(connect);
}
@end

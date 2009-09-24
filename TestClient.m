//
//  TestClient.m
//  InsomniaT
//
//  Created by Archimedes Trajano on 2009-09-24.
//  Copyright 2009 trajano.net. All rights reserved.
//

#import "TestClient.h"
#import <stdio.h>
#import <IOKit/IOKitLib.h>


@implementation TestClient

@end


int main( int argc, const char *argv[] ) {
    printf( "hello world\n" );
	
	kern_return_t    kernResult; 
    io_service_t    service;
    io_iterator_t     iterator;
    bool            driverFound = false;

	kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceNameMatching("net_trajano_driver_InsomniaT"), &iterator);
    
    printf( "kernresult %d\n", kernResult );

	if (kernResult != KERN_SUCCESS) {
        fprintf(stderr, "IOServiceGetMatchingServices returned 0x%08x\n\n", kernResult);
        return -1;
    }
	printf( "scanning %d\n", kernResult );

	while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
        driverFound = true;
     
		printf("Found a device of class net_trajano_driver_InsomniaT.\n\n");
			//TestUserClient(service);
    }
    
		// Release the io_iterator_t now that we're done with it.
    IOObjectRelease(iterator);
	
	
    return 0;
}
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

		//	kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching("IOBluetoothSerialManager"), &iterator);
    service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
        fprintf(stderr, "null\n");
        return -1;
    }
	printf( "scanning %d\n", 0 );

	
    return 0;
}
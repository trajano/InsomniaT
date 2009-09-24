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
    io_service_t    service;
	
    service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
        fprintf(stderr, "service was not found\n");
        return -1;
    }
	printf( "service found\n");
	
	io_connect_t connect;
	IOServiceOpen(service, mach_task_self(), 0, &connect);
	printf( "open\n");
	IOServiceClose(connect);

    return 0;
}
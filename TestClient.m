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
#import <getopt.h>

@implementation TestClient

@end

int main( int argc, char * const argv[] ) {
    io_service_t    service;
	
	extern char *optarg;
	extern int optind;
	
	static struct option longopts[] = {
		{ "enable",      no_argument,            0,             'y' },
		{ "disable",   no_argument,      0,              'n' },
		{ "status",  no_argument,            0, 'c'},
		{ NULL, 0, NULL,           0 }
	};
	
	int ch;
	int action = 3;
	while ((ch = getopt_long(argc, argv, "yns", longopts, NULL)) != -1) {
		switch (ch) {
			case 'y':
				action = 1;
				break;
			case 'n':
				action = 2;
				break;
			case 'c':
				action = 3;
				break;
			case '?':
			default:
				fprintf(stderr, "Usage: %s [-y|--enable|-n|--disable|-c|--status]\n", argv[0]); 
				exit(-2);
		}
	}
		//argc -= optind;
		//argv += optind;
	
    service = IOServiceGetMatchingService(kIOMasterPortDefault,IOServiceMatching("net_trajano_driver_InsomniaT"));
	if (service == IO_OBJECT_NULL) {
        fprintf(stderr, "service was not found\n");
        return -1;
    }
	
	io_connect_t connect;
	kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &connect);
	if (kernResult == KERN_SUCCESS) {
		
		uint64_t output[1];
		uint32_t count = 1;
		IOConnectCallScalarMethod(connect, action, NULL, 0, output, &count);
		
		if (action == 3) {
			if (output[0] == 1) {
				printf("sleep is enabled\n");
			} else {
				printf("sleep is disabled\n");
			}
		}
		
		
		IOServiceClose(connect);
	} else {
		fprintf(stderr, "open failed\n");
		exit(-1);
	}
	
    return 0;
}
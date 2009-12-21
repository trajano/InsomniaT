	//
	//  TestClient.m
	//  InsomniaT
	//
	//  Created by Archimedes Trajano on 2009-09-24.
	//  Copyright 2009 trajano.net. All rights reserved.
	//

#import "insomniat.h"
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
		{ "enable", no_argument, 0, 'y' },
		{ "disable", no_argument, 0, 'n' },
		{ "status", no_argument, 0, 0 },
		{ "log", required_argument, 0, 0 },
		{ "help", no_argument, 0, 'h'},
		{ NULL, 0, NULL, 0 }
	};
	int longIndex;
	int ch;
	int action = 3;
	while ((ch = getopt_long(argc, argv, "ynclL", longopts, &longIndex)) != -1) {
		switch (ch) {
			case 0:
				if (strcmp("status", longopts[longIndex].name) == 0) {
					action = 3;
				} else if (strcmp("log", longopts[longIndex].name) == 0) {
					long logLevel = strtol(optarg, NULL, 0);
					if (logLevel > 0) {
						action = 5;
					} else if (logLevel == 0) {
						action = 4;
					} else {
						fprintf(stderr, "LEVEL must be a number that is 0 or higher\n");
					}
				}
				break;
			case 'y':
				action = 1;
				break;
			case 'n':
				action = 2;
				break;
			case 'h':
				printf("Usage: %s [OPTION]\n"
						"Enable or disable sleep when the laptop lid is closed.\n"
						"\n"
						"Options:\n"
						"  -y, --enable    Enables sleep when the laptop lid is closed.\n"
						"  -n, --disable   Disables sleep when the laptop lid is closed.\n"
						"      --status    Displays the status of the kernel driver.\n"
						"                  This is what gets displayed if no option is given.\n"
						"\n"
						"Logging options:\n"
						"      --log=LEVEL Sets the log level, should be 0 or higher.\n"
						"                  A value of 0 will disable logging.\n"
						"                  Logs are put in /var/system/messages prefixed\n"
					   "                   with 'InsomniaT:'.\n"
						"\n"
						"Report bugs to https://bugs.launchpad.net/insomniat/+filebug\n", argv[0]);
				exit(0);
			case '?':
			default:
				fprintf(stderr, "Usage: %s [OPTION]\n"
						"Try `%s --help' for more information.\n", argv[0], argv[0]); 
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

		if (action == 3) {
			IOConnectCallScalarMethod(connect, 3, NULL, 0, output, &count);
			if (output[0] == 1) {
				printf("sleep is enabled\n");
			} else {
				printf("sleep is disabled\n");
			}
			
			IOConnectCallScalarMethod(connect, 6, NULL, 0, output, &count);
			if (output[0] == 1) {
				printf("logging is enabled\n");
			} else {
				printf("logging is disabled\n");
			}
		} else {
			IOConnectCallScalarMethod(connect, action, NULL, 0, output, &count);
		}
		
		
		IOServiceClose(connect);
	} else {
		fprintf(stderr, "open failed\n");
		exit(-1);
	}
	
    return 0;
}

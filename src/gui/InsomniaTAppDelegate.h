//
//  CafeBabeAppDelegate.h
//  CafeBabe
//
//  Created by Archimedes Trajano on 2009-10-15.
//  Copyright 2009 trajano.net. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if (MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_5)
@interface InsomniaTAppDelegate : NSObject {
#else
@interface InsomniaTAppDelegate : NSObject <NSApplicationDelegate> {
#endif

    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end

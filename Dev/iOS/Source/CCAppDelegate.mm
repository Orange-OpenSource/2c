/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCAppDelegate.mm
 *-----------------------------------------------------------
 */

#import "CCDefines.h"
#import "CCAppDelegate.h"
#import "CCWindowController.h"


@implementation CCAppDelegate


-(id)init
{
	self = [super init];
	if( self != NULL )
	{
        self->windowController = new CCWindowController();
	}
	
	return self;
}


-(void)applicationDidFinishLaunching:(UIApplication*)application
{
    windowController->startup();
}


-(void)dealloc 
{	
    delete windowController;
	[super dealloc];
}


-(void)applicationWillTerminate:(UIApplication*)application
{
    windowController->shutdown();
}


-(void)applicationWillResignActive:(UIApplication*)application
{
    windowController->pause();
}


-(void)applicationDidBecomeActive:(UIApplication*)application
{
    windowController->resume();
}


-(void)applicationDidReceiveMemoryWarning:(UIApplication*)application
{
	NSLog( @"applicationDidReceiveMemoryWarning" );
}  

@end


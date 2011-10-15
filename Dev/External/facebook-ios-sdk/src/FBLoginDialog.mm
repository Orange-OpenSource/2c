/*
 * Copyright 2010 Facebook
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "FBDialog.h"
#import "FBLoginDialog.h"
#include "CCFBApi.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

@implementation FBLoginDialog

///////////////////////////////////////////////////////////////////////////////////////////////////
// public 

/*
 * initialize the FBLoginDialog with url and parameters
 */
- (id)initWithURL:(NSString*) loginURL 
      loginParams:(NSMutableDictionary*) params 
{  
    self = [super init];
    _serverURL = [loginURL retain];
    _params = [params retain];
    return self;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FBDialog

/**
 * Override FBDialog : to call when the webView Dialog did succeed
 */
- (void) dialogDidSucceed:(NSURL*)url 
{
    NSString *q = [url absoluteString];
    NSString *token = [self getStringFromUrl:q needle:@"access_token="];
    NSString *expTime = [self getStringFromUrl:q needle:@"expires_in="];
    NSDate *expirationDate =nil;

    if (expTime != nil) 
    {
        int expVal = [expTime intValue];
        if (expVal == 0) 
        {
            expirationDate = [NSDate distantFuture];
        } 
        else 
        {
            expirationDate = [NSDate dateWithTimeIntervalSinceNow:expVal];
        } 
        } 

    if ((token == (NSString *) [NSNull null]) || (token.length == 0)) 
    {
        [self dialogDidCancel:url];
    }
    else 
    {
        CCFBApi::SetUserAccessToken( [token UTF8String] );
        [self dismissWithSuccess:YES animated:YES];
        CCFBApi::onBrowserClosed();
    }

}

/**
 * Override FBDialog : to call with the login dialog get canceled 
 */
- (void)dialogDidCancel:(NSURL *)url
{
    [self dismissWithSuccess:NO animated:YES];
    CCFBApi::onBrowserClosed();
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error 
{
//    if (!(([error.domain isEqualToString:@"NSURLErrorDomain"] && error.code == -999) ||
//        ([error.domain isEqualToString:@"WebKitErrorDomain"] && error.code == 102))) 
//    {
//    }
    [super webView:webView didFailLoadWithError:error];
    CCFBApi::onBrowserClosed();
}

@end

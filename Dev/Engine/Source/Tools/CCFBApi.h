/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCFBApi.h
 * Description : Interfaces with Facebook Api.
 *
 * Created     : 01/07/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCFBAPI_H__
#define __CCFBAPI_H__


#include "CCDefines.h"

class CCFBApiInterface : public virtual CCLazyCallback
{
public:
    // Returns the state of the request, the url it got the photo from (if any), the photoID requested and the filename saved as
    virtual void FBApiPhoto(const CCURLRequest::RequestState state, const char *url, const char *photoID, const char *filename) {}
};

class CCFBApi
{
public:
	static void Initialize();

    // Make an initial api call
    static void Request(CCURLCallback *inCallback, 
                        const char *apiCall,
                        const int priority=0, 
                        const bool refresh=false, 
                        const float timeout=0.0f, 
                        const int limit=-1, 
                        const int offset=-1);
    //static void RequestIDs(URLCallback *inCallback, CCList<const char> &userIDs, const int priority=0);

    // Make a subsequent api call with an entire url parsed from a FB json response
    static void RequestFBURL(CCURLCallback *inCallback, const char *url, const int priority, const bool refresh, const float timeout=0.0f);

    // Handle downloading a photo
    static void RequestPhotoID(CCFBApiInterface *fbInterface, const char *photoID, const int priority);

    // If we know the URL
    static void RequestPhotoURL(CCFBApiInterface *fbInterface, const char *photoID, const char *photoURL, 
                                const bool isThumbnail, const int priority);

    static void LogOut();
    static void LaunchBrowser(CCLambdaCallback *onSuccess=NULL);
    static void LaunchBrowserOnNativeThread();
    static void onBrowserClosed();
    static const CCText GetAuthorizationURL();
    static const bool ProcessLogin(const char *url);

    static void SetUserAccessToken(const char *token);
    static const char* GetUserAccessToken()
    {
        return m_userAccessToken.buffer;
    }

    static const char *APPLICATION_ID;
    static const char *APPLICATION_SECRET;
    static const char *API_URL;
    static const char *API_PERMISSIONS;

private:
    static CCText m_userAccessToken;
    static CCLambdaCallback *m_onSuccess;
};


#endif // __CCFBAPI_H__

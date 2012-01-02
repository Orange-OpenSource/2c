/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCFBApi.cpp
 *-----------------------------------------------------------
 */

#include "CCFBApi.h"
#include "CCFileManager.h"

#ifdef IOS
    #import "FBLoginDialog.h"
#elif defined ANDROID
	#include "CCJNI.h"
#elif defined QT
    #include "CCFBWebView.h"
#endif


const char *CCFBApi::APPLICATION_ID = "138263572915502";
const char *CCFBApi::APPLICATION_SECRET = "c3ccc56ab0a6035818351c2a7016a0fe";
const char *CCFBApi::API_URL = "https://graph.facebook.com/";
const char *CCFBApi::API_PERMISSIONS = "user_photos," \
"friends_photos,";// \
//"user_photo_video_tags," \
//"friends_photo_video_tags,";// \
//"user_videos," \
//"user_activities," \
//"user_groups," \
//"user_interests," \
//"user_likes," \
//"user_notes," \
//"user_online_presence," \
//"user_status," \
//"read_friendlists," \
//"read_requests," \
//"offline_access," \
//"friends_activities," \
//"friends_events," \
//"friends_groups," \
//"friends_likes," \
//"friends_notes," \
//"friends_online_presence," \
//"publish_stream," \
//"read_mailbox," \
//"read_stream" \
;
CCText CCFBApi::m_userAccessToken;
CCLambdaCallback *CCFBApi::m_onSuccess = NULL;



void CCFBApi::Initialize()
{
	m_onSuccess = NULL;
}


class CCFBCallback : public CCURLCallback
{
public:
    CCFBCallback(CCURLCallback *callback)
    {
        this->callback = callback;
    }

    void run()
    {
        if( CCText::Contains( reply->data.buffer, "OAuthException" ) )
        {
            reply->state = CCURLRequest::data_error;
            //ASSERT( false );
        }
        else if( CCText::Equals( reply->data.buffer, "false" ) )
        {
            reply->state = CCURLRequest::data_error;
            //ASSERT( false );
        }
        callback->reply = reply;
        callback->run();
        DELETE_POINTER( callback );
    }

private:
    CCURLCallback *callback;
};


static void appendToken(CCText &url, const char *key, const char *value)
{
    url += key;
    url += value;
}


static void SetCacheFilePath(CCText &cacheFilePath, const char *apiCall)
{
    cacheFilePath.set( "cache/api/" );
    {
        CCText cacheFilename( apiCall );
        cacheFilename.replaceChar( '/', '.' );
        cacheFilename += ".txt";
        cacheFilePath += cacheFilename.buffer;
    }
}

void CCFBApi::Request(CCURLCallback *inCallback, 
                      const char *apiCall, 
                      const int priority, 
                      const bool refresh,
                      const float timeout, 
                      const int limit, 
                      const int offset)
{
    CCText url = API_URL;
    url += apiCall;

    CCText cacheFilePath;
    SetCacheFilePath( cacheFilePath, apiCall );
    
    appendToken( url, "?access_token=", m_userAccessToken.buffer );

    if( limit != -1 )
    {
        url += "&limit=";
        url += limit;
    }

    if( offset != -1 )
    {
        url += "&offset=";
        url += offset;
    }
    
#ifdef DEBUGON
    if( priority > 0 )
    {
        DEBUGLOG( "CCFBApi::Request %s \n", apiCall );
    }
#endif

    gEngine->urlManager->requestURL( url.buffer,
                                     new CCFBCallback( inCallback ),
                                     priority,
                                     cacheFilePath.buffer,
                                     refresh==false,
                                     timeout );
}



//void CCFBApi::RequestIDs(URLCallback *fbCallback, CCList<const char> &userIDs, const int priority)
//{
//    CCText url = API_URL;
//    url += "?";
    
//    CCText userIDsRequest = "ids=";
//    for( int i=0; i<userIDs.length; ++i )
//    {
//        if( i != 0 )
//        {
//            userIDsRequest += ",";
//        }
//        userIDsRequest += userIDs.list[i];
//    }
//    url += userIDsRequest.buffer;
    
//    CCText cacheFilePath( "cache/api/" );
//    {
//        CCText cacheFilename( userIDsRequest );
//        cacheFilename.replaceChar( '/', '.' );
//        cacheFilename.replaceChar( ',', '.' );
//        cacheFilename.replaceChar( '=', '.' );
//        cacheFilename += ".txt";
//        cacheFilePath += cacheFilename.buffer;
//    }
    
//    appendToken( url, "&access_token=", m_userAccessToken.buffer );
    
//#ifdef DEBUGON
//    if( priority > 0 )
//    {
//        DEBUGLOG( "CCFBApi::Request %s \n", userIDsRequest.buffer );
//    }
//#endif
    
//    gEngine->urlManager->requestURL( url.buffer,
//                                    new FBCallback( fbCallback ),
//                                    cacheFilePath.buffer, priority );
//}


void CCFBApi::RequestFBURL(CCURLCallback *inCallback, const char *url, const int priority, const bool refresh, const float timeout)
{
    CCText cacheFilePath( "cache/api/" );
    {
        CCText splitURL( url );
        splitURL.splitAfter( splitURL, "facebook.com/" );
        splitURL.removeBetweenIncluding( "access_token=", "&" );

        CCText cacheFilename( splitURL.buffer );
        cacheFilename.replaceChar( '/', '.' );
        cacheFilename.replaceChar( '?', '.' );
        cacheFilename.replaceChar( '&', '.' );
        cacheFilename.replaceChar( '=', '.' );
        cacheFilename += ".txt";
        cacheFilePath += cacheFilename.buffer;
    }
    
    // Replace access token
    CCText escapedURL( url );
    escapedURL.removeBetweenIncluding( "access_token=", "&" );
    appendToken( escapedURL, "&access_token=", m_userAccessToken.buffer );

    gEngine->urlManager->requestURL( escapedURL.buffer,
                                     new CCFBCallback( inCallback ),
                                     priority,
                                     cacheFilePath.buffer, 
                                     refresh==false,
                                     timeout );
}


class CCFBPhotoCallback : public CCURLCallback
{
public:
    CCFBPhotoCallback(CCFBApiInterface *fbInterface, const char *token, const char *photoID, const char *filename)
    {
        this->fbInterface = fbInterface;
        this->lazyPointer = fbInterface->lazyPointer;
        this->lazyID = fbInterface->lazyID;
        this->token = token;
        this->photoID = photoID;
        this->filename = filename;
    }

    void run()
    {
        if( CCLazyCallback::isCallbackActive( lazyPointer, lazyID ) == false )
        {
            DEBUGLOG( "CCFBApiInterface::Rejected callback \n" );
            return;
        }

        // Android version automatically follows redirects
#ifndef ANDROID
        // Downloaded header?
        if( CCText::Contains( token.buffer, "photoHeader" ) )
        {
            if( reply->state == CCURLRequest::succeeded )
            {
                // Parse the headers
                for( int i=0; i<reply->header.length(); ++i )
                {
                    CCText *headerName = reply->header.names.list[i];
                    CCText *headerData = reply->header.values.list[i];
                    if( CCText::Contains( headerName->buffer, "Location" ) )
                    {
                        // https is slow
                        CCText httpURL = *headerData;
                        httpURL.removeBetween( "http", "://" );
                        gEngine->urlManager->requestURL( httpURL.buffer,
                                                         new CCFBPhotoCallback( fbInterface,
                                                                                "photoDownload", 
                                                                                photoID.buffer, 
                                                                                filename.buffer ),
                                                         2 );
                        return;
                    }
                }
            }
        }

        // Downloaded photo data
        else if( CCText::Contains( token.buffer, "photoDownload" ) )
#endif
        {
            if( reply->state == CCURLRequest::succeeded )
            {
                bool saved = false;
                // Parse the headers
                for( int i=0; i<reply->header.length(); ++i )
                {
                    CCText *headerName = reply->header.names.list[i];
                    CCText *headerData = reply->header.values.list[i];
                    if( CCText::Contains( headerName->buffer, "Content-Type" ) )
                    {
                    	// Android version handles gifs
#ifndef ANDROID
                        if( CCText::Contains( headerData->buffer, "image/gif" ) )
                        {
                            reply->state = CCURLRequest::data_error;
                        }
                        else
#endif
                        {
                            CCEngineThreadLock();
                            if( CCFileManager::saveCachedFile( filename.buffer, reply->data.buffer, reply->data.length ) )
                            {
                                saved = true;
                            }
                            CCEngineThreadUnlock();
                        }
                        break;
                    }
                }
           
                // Error, let's debug why?
                if( saved == false )
                {     
                    reply->state = CCURLRequest::data_error;
#ifdef DEBUGON
                    for( int i=0; i<reply->header.length(); ++i )
                    {
                        CCText *headerName = reply->header.names.list[i];
                        CCText *headerData = reply->header.values.list[i];
                        if( CCText::Contains( headerName->buffer, "Content-Type" ) )
                        {
                            if( CCText::Contains( headerData->buffer, "image/gif" ) )
                            {
                                reply->state = CCURLRequest::data_error;
                            }
                            else
                            {
                                CCEngineThreadLock();
                                if( CCFileManager::saveCachedFile( filename.buffer, reply->data.buffer, reply->data.length ) )
                                {
                                    saved = true;
                                }
                                CCEngineThreadUnlock();
                            }
                            break;
                        }
                    }
#endif
                }
            } 
            
            fbInterface->FBApiPhoto( reply->state, reply->url.buffer, photoID.buffer, filename.buffer );
        }
    }

private:
    CCFBApiInterface *fbInterface;
    void *lazyPointer;
    long lazyID;
    CCText token;
    CCText photoID;
    CCText filename;
};


void CCFBApi::RequestPhotoID(CCFBApiInterface *fbInterface, const char *photoID, const int priority)
{
    CCText photoFilePath( "cache/photos/" );
    photoFilePath += photoID;
    photoFilePath += ".jpg";
    
    class FileCallback : public CCIOCallback
    {
    public:
        FileCallback(const char *photoFilePath, CCFBApiInterface *fbInterface, const char *photoID, const int priority) :
            CCIOCallback( priority )
        {
            this->photoFilePath = photoFilePath;
            this->fbInterface = fbInterface;
            this->lazyPointer = fbInterface->lazyPointer;
            this->lazyID = fbInterface->lazyID;
            this->photoID = photoID;
        }
        
        const bool isCallbackActive()
        {
            if( CCLazyCallback::isCallbackActive( lazyPointer, lazyID ) )
            {
                return true;
            }
            return false;
        }
        
        void run()
        {
            if( exists )
            {
                fbInterface->FBApiPhoto( CCURLRequest::cached, NULL, photoID.buffer, photoFilePath.buffer );
                return;
            }

            CCText url = API_URL;
            url += photoID.buffer;
            url += "/picture";
            
            appendToken( url, "?access_token=", m_userAccessToken.buffer );

            gEngine->urlManager->requestURL( url.buffer,
                                             new CCFBPhotoCallback( fbInterface,
                                                                    "photoHeader", 
                                                                    photoID.buffer, 
                                                                    photoFilePath.buffer ),
                                                                    priority );
        }
        
    private:
        CCText photoFilePath;
        CCFBApiInterface *fbInterface;
        void *lazyPointer;
        long lazyID;
        CCText photoID;
    };
//    CCFileManager::doesCachedFileExistAsync( photoFilePath.buffer, new FileCallback( photoFilePath.buffer, fbInterface,
//                                                                                  photoID, priority ) );
    
    const bool exists = CCFileManager::doesCachedFileExist( photoFilePath.buffer );
    if( exists )
    {
        fbInterface->FBApiPhoto( CCURLRequest::cached, NULL, photoID, photoFilePath.buffer );
    }
    else
    {
        CCText url = API_URL;
        url += photoID;
        url += "/picture";
        
        appendToken( url, "?access_token=", m_userAccessToken.buffer );
        
        gEngine->urlManager->requestURL( url.buffer,
                                         new CCFBPhotoCallback( fbInterface,
                                                                "photoHeader", 
                                                                photoID, 
                                                                photoFilePath.buffer ),
                                         priority );
    }
}


void CCFBApi::RequestPhotoURL(CCFBApiInterface *fbInterface, const char *photoID, const char *photoURL, 
                              const bool isThumbnail, const int priority)
{
    ASSERT( photoURL != NULL );
    
    CCText photoFilePath( "cache/photos/" );
    photoFilePath += photoID;
    if( isThumbnail )
    {
        photoFilePath += "_thumb";
    }
    photoFilePath += ".jpg";

    class FileCallback : public CCIOCallback
    {
    public:
        FileCallback(const char *photoFilePath, CCFBApiInterface *fbInterface, const char *photoID, const char *photoURL, const int priority) :
            CCIOCallback( priority )
        {
            this->photoFilePath = photoFilePath;
            this->fbInterface = fbInterface;
            this->lazyPointer = fbInterface->lazyPointer;
            this->lazyID = fbInterface->lazyID;
            this->photoID = photoID;
            this->photoURL = photoURL;
        }
        
        const bool isCallbackActive()
        {
            if( CCLazyCallback::isCallbackActive( lazyPointer, lazyID ) )
            {
                return true;
            }
            return false;
        }
        
        void run()
        {
            if( exists )
            {
                fbInterface->FBApiPhoto( CCURLRequest::cached, photoURL.buffer, photoID.buffer, photoFilePath.buffer );
                return;
            }
            
            CCText url = API_URL;
            url += photoID.buffer;
            url += "/picture";
            
            appendToken( url, "?access_token=", m_userAccessToken.buffer );
            
            gEngine->urlManager->requestURL( photoURL.buffer,
                                             new CCFBPhotoCallback( fbInterface,
                                                                    "photoDownload", 
                                                                    photoID.buffer,
                                                                    photoFilePath.buffer ),
                                             priority );
        }
        
    private:
        CCText photoFilePath;
        CCFBApiInterface *fbInterface;
        void *lazyPointer;
        long lazyID;
        CCText photoID;
        CCText photoURL;
    };

    CCFileManager::doesCachedFileExistAsync( photoFilePath.buffer, new FileCallback( photoFilePath.buffer, 
                                                                                     fbInterface,
                                                                                     photoID, photoURL, priority ) );
}


void CCFBApi::LogOut()
{
#ifdef IOS
    
    gEngine->urlManager->flushPendingRequests();
    
    // Clear cookies
    NSHTTPCookieStorage *cookies = [NSHTTPCookieStorage sharedHTTPCookieStorage];
    NSArray *facebookCookies = [cookies cookiesForURL:[NSURL URLWithString:@"http://login.facebook.com"]];
    
    for( NSHTTPCookie* cookie in facebookCookies ) 
    {
        [cookies deleteCookie:cookie];
    }
    
    m_userAccessToken.clear();
    
#elif defined ANDROID

    CCJNI::WebViewClearData();

#elif defined QT

    CCFBWebView::ClearData();

#endif

    CCText cacheFilePath;
    SetCacheFilePath( cacheFilePath, "me" );
    if( CCFileManager::doesCachedFileExist( cacheFilePath.buffer ) )
    {
        CCFileManager::deleteCachedFile( cacheFilePath.buffer );
    }
}


void CCFBApi::LaunchBrowser(CCLambdaCallback *onSuccess)
{
    // Ignore calls until launch finished
    if( m_onSuccess != NULL )
    {
        return;
    }
    
    m_onSuccess = onSuccess;

    LAMBDA_RUN_NATIVETHREAD(
        CCFBApi::LaunchBrowserOnNativeThread();
    )
}


void CCFBApi::LaunchBrowserOnNativeThread()
{
#ifdef IOS
    
    NSString *kRedirectURL = @"fbconnect://success";
    NSString *kLogin = @"oauth";
    NSString *kSDKVersion = @"2";
    
    NSString *appId = [[NSString alloc] initWithFormat:@"%s", APPLICATION_ID];
    NSMutableDictionary *params = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                   appId, @"client_id",
                                   @"user_agent", @"type",
                                   kRedirectURL, @"redirect_uri",
                                   @"touch", @"display",
                                   kSDKVersion, @"sdk",
                                   nil];
    
    NSString *kDialogBaseURL = @"https://www.facebook.com/dialog/";
    NSString *loginDialogURL = [kDialogBaseURL stringByAppendingString:kLogin];
    
    NSString *permissions = [[NSString alloc] initWithFormat:@"%s", API_PERMISSIONS];
    [params setValue:permissions forKey:@"scope"];
    
    FBLoginDialog *loginDialog = [[FBLoginDialog alloc] initWithURL:loginDialogURL
                                                        loginParams:params];
    [loginDialog show];
    
    [appId release];
    [permissions release];
    
#elif defined QT
    
    // OpenGL widget's always render on top, so hide it until finished with this view
    gView->hide();
    CCMainWindow::instance->addChild( new CCFBWebView( CCMainWindow::instance ) );
    
#elif defined ANDROID

    CCJNI::WebViewOpen();

#endif
}


void CCFBApi::onBrowserClosed()
{
#ifdef QT
    gView->show();
#endif

    if( m_onSuccess != NULL )
    {
        if( m_userAccessToken.length > 0 )
        {
            LAMBDA_RUN_ENGINETHREAD(
                gEngine->urlManager->setDomainTimeOut( "facebook.com", 1.0f );
                m_onSuccess->run();
                m_onSuccess = NULL;
            )
        }
        else
        {
            m_onSuccess = NULL;
        }
    }
}


const CCText CCFBApi::GetAuthorizationURL()
{
    CCText authorizationURL = "https://www.facebook.com/dialog/oauth";
    authorizationURL += "?client_id=";
    authorizationURL += CCFBApi::APPLICATION_ID;
    authorizationURL += "&redirect_uri=https://www.facebook.com/connect/login_success.html&" \
                        "type=user_agent&" \
                        "display=popup&" \
                        "scope=";
    authorizationURL += API_PERMISSIONS;
    return authorizationURL;
}


const bool CCFBApi::ProcessLogin(const char *url)
{
    CCText textURL( url );
    if( CCText::StartsWith( textURL.buffer, "https://www.facebook.com/connect/login_success.html" ) )
    {
        if( CCText::Contains( textURL.buffer, "login_success.html#access_token" ) )
        {
            // the login was succesful now we going to start the application
            CCText token;
            token.splitBetween( textURL, "#access_token=", "&" );
            SetUserAccessToken( token.buffer );
        }
        return true;
    }

    return false;
}


void CCFBApi::SetUserAccessToken(const char *token)
{
    m_userAccessToken.set( token );
    
#ifdef IOS
    
    NSString *nsValue = [[NSString alloc] initWithFormat:@"%s", m_userAccessToken.buffer];
    NSString *escapedValue = (NSString*)CFURLCreateStringByAddingPercentEscapes( NULL, /* allocator */
                                                                                (CFStringRef)nsValue,
                                                                                NULL, /* charactersToLeaveUnescaped */
                                                                                (CFStringRef)@"!*'();:@&=+$,/?%#[]",
                                                                                kCFStringEncodingUTF8 );
    m_userAccessToken.set( [escapedValue UTF8String] );
    [escapedValue release];
    [nsValue release];
    
#endif
}

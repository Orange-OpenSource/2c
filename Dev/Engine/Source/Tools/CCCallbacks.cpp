/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCCallbacks.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"


// CCLazyCallback
CCList<void> CCLazyCallback::activeCallbacks;
long CCLazyCallback::activeID = 0;


CCLazyCallback::CCLazyCallback()
{
    CCNativeThreadLock();
    
#ifdef DEBUGON
    for( int i=0; i<activeCallbacks.length; ++i )
    {
        if( activeCallbacks.list[i] == this )
        {
            ASSERT( false );
            break;
        }
    }
#endif
    
    lazyPointer = this;
    lazyID = activeID++;
    activeCallbacks.add( lazyPointer );
    CCNativeThreadUnlock();
}


CCLazyCallback::~CCLazyCallback()
{
    CCNativeThreadLock();
    
    activeCallbacks.remove( this );
    
#ifdef DEBUGON
    for( int i=0; i<activeCallbacks.length; ++i )
    {
        if( activeCallbacks.list[i] == this )
        {
            ASSERT( false );
            break;
        }
    }
#endif
    
    CCNativeThreadUnlock();
}


const bool CCLazyCallback::isCallbackActive(void *pendingCallback, const long pendingID)
{
    CCNativeThreadLock();
    for( int i=0; i<activeCallbacks.length; ++i )
    {
        void *activeCallback = activeCallbacks.list[i];
        if( activeCallback == pendingCallback )
        {
            // Perhaps the same pointer will be added later on
            // which will lead to an incorrect call?
            // Let's cross check with the callback's timestamp
            if( ((CCLazyCallback*)activeCallback)->lazyID == pendingID )
            {
                CCNativeThreadUnlock();
                return true;
            }
            
#ifdef DEBUGON
            
            else
            {
                //DEBUGLOG( "Detected that the callback has been deleted and the memory has been replaced by another callback\n" );
            }
            
#else
            
            break;
            
#endif
        }
    }
    CCNativeThreadUnlock();
    return false;
}


/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCInterpolators.cpp
 * Description : Interpolators for various curves.
 *
 * Created     : 30/04/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#include "CCDefines.h"


void CCInterpolator::setDuration(const float duration)
{
    speed = 1.0f/duration;
}



const bool CCInterpolatorSin2Curve::equals(float *inCurrent, const float inTarget)
{ 
    // Ignore if we're already doing this
    if( current == inCurrent && target == inTarget )
    {
        return true;
    }
    return false;
}


void CCInterpolatorSin2Curve::setup(float *inCurrent, const float inTarget, const bool force)
{
    if( force || current != inCurrent || target != inTarget )
    {
        setCurrent( inCurrent );
        setTarget( inTarget );
    }
}


void CCInterpolatorSin2Curve::setCurrent(float *inCurrent)
{
    current = inCurrent;
}


void CCInterpolatorSin2Curve::setTarget(float inTarget)
{
    target = inTarget;
    ready();
}


void CCInterpolatorSin2Curve::ready()
{
    if( current != NULL )
    {
        start = *current;
        length = target - start;
        amount = 0.0f;
    }
}


const bool CCInterpolatorSin2Curve::incrementAmount(const float delta)
{
    if( CCToTarget( amount, 1.0f, delta * speed ) )
    {
        return true;
    }
    return false;
}


const float CCInterpolatorSin2Curve::calculatePercentage()
{
    static const float interpolationCurveMultiple = 1.0f / sinf( 2.0f );
    const float percentage = sinf( amount * 2.0f ) * interpolationCurveMultiple;
    return percentage;
}


void CCInterpolatorSin2Curve::updateInterpolation(const float percentage)
{
    const float movement = length * percentage;
    *current = movement + start;
}


const bool CCInterpolatorSin2Curve::update(const float delta)
{
    if( current != NULL )
    {
        if( incrementAmount( delta ) )
        {
            const float percentage = calculatePercentage();
            updateInterpolation( percentage );
            return true;
        }
        else if( *current != target )
        {
            *current = target;
            current = NULL;
            return true;
        }
    }
    else if( onInterpolated.length > 0 )
    {
        LAMBDA_SIGNAL pendingCallbacks;
        for( int i=0; i<onInterpolated.length; ++i )
        {
            pendingCallbacks.add( onInterpolated.list[i] );
        }
        onInterpolated.length = 0;
        LAMBDA_EMIT_ONCE( pendingCallbacks );
    }

    return false;
}



const float CCInterpolatorX2Curve::calculatePercentage()
{
    const float percentage = amount * amount;
    return percentage;
}



const float CCInterpolatorX3Curve::calculatePercentage()
{
    const float sudoAmount = amount - 1.0f;
    const float percentage = 1.0f + ( sudoAmount * sudoAmount * sudoAmount );
    return percentage;
}



const float CCInterpolatorSinCurve::calculatePercentage()
{
    static const float interpolationCurveMultiple = 1.0f / sinf( CC_HPI );
    const float percentage = sinf( amount * CC_HPI ) * interpolationCurveMultiple;
    return percentage;
}



const float CCInterpolatorLinear::calculatePercentage()
{
    return amount;
}



template class CCInterpolatorListV3<CCInterpolatorSin2Curve>;
template class CCInterpolatorListV3<CCInterpolatorLinear>;


template <typename T>
const bool CCInterpolatorListV3<T>::update(const float delta)
{
    if( interpolators.length > 0 )
    {
        CCInterpolatorV3<T> *interpolator = interpolators.list[0];
        if( interpolator->update( delta * speed ) == false )
        {
            interpolators.remove( interpolator );
            DELETE_OBJECT( interpolator );
            
            // If there's another interpolation planned, tell it to ready itself to interpolate
            if( interpolators.length > 0 )
            {
                interpolators.list[0]->ready();
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    return false;
}



const bool CCInterpolatorLinearColour::update(const float delta)
{
    if( current != NULL )
    {
        if( updating )
        {
            if( current->toTarget( target, delta * speed ) )
            {
                return true;
            }
            else
            {
                updating = false;
            }
        }
        else 
        {
            ASSERT( current->equals( target ) );
            if( onInterpolated.length > 0 )
            {
                LAMBDA_SIGNAL pendingCallbacks;
                for( int i=0; i<onInterpolated.length; ++i )
                {
                    pendingCallbacks.add( onInterpolated.list[i] );
                }
                onInterpolated.length = 0;
                LAMBDA_EMIT_ONCE( pendingCallbacks );
            }
        }
    }
    return false;
}



// CCTimer
const bool CCTimer::update(const float delta)
{
    if( updating )
    {
        const float real = gEngine->time.real;
        time -= real;
        if( time <= 0.0f )
        {
            updating = false;
            LAMBDA_EMIT( onTime );
        }
        return true;
    }
    return false;
}


void CCTimer::start(const float timeout)
{
    interval = timeout * 0.001f; // Milliseconds to seconds
    restart();
}


void CCTimer::stop()
{
    updating = false;
}


void CCTimer::restart()
{
    time = interval;
    updating = true;
}
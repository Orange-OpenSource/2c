/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCInterpolators.h
 * Description : Interpolators for various curves.
 *
 * Created     : 30/04/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCINTERPOLATORS_H__
#define __CCINTERPOLATORS_H__


class CCInterpolator : public CCUpdater
{
public:
    CCInterpolator()
    {
        speed = 1.0f;
    }

    virtual ~CCInterpolator()
    {
#ifdef DEBUGON
        destructCalled = true;
#endif

        onInterpolated.deleteObjectsAndList();
    }
    
    void setDuration(const float duration);

public:
    LAMBDA_SIGNAL onInterpolated;
    
protected:
    float speed;
};



class CCInterpolatorSin2Curve : public CCInterpolator
{
public:
    CCInterpolatorSin2Curve()
    {  
        current = NULL;
        amount = 1.0f;
    }
    
    CCInterpolatorSin2Curve(float *inCurrent, const float inTarget)
    {
        setup( inCurrent, inTarget, true );
    }
    
    const bool equals(float *inCurrent, const float inTarget);
    void setup(float *inCurrent, const float inTarget, const bool force=true);

    void setCurrent(float *inCurrent);
    void setTarget(float inTarget);
    
    // Call when ready to start the interpolation
    void ready();
    
    const bool incrementAmount(const float delta);
    void setAmount(const float amount)
    {
        this->amount = amount;
    }
    virtual const float calculatePercentage();
    void updateInterpolation(const float percentage);
    virtual const bool update(const float delta);
    
    inline const float* getCurrent() const { return current; }
    inline const float getAmount() const { return amount; }
    inline const float getTarget() const { return target; }
    
protected:
    float *current;
    float target;
    float start;
    float length;
    float amount;
};



class CCInterpolatorX2Curve : public CCInterpolatorSin2Curve
{
public:
    const float calculatePercentage();
};


class CCInterpolatorX3Curve : public CCInterpolatorSin2Curve
{
public:
    const float calculatePercentage();
};


class CCInterpolatorSinCurve : public CCInterpolatorSin2Curve
{
public:
    const float calculatePercentage();
};


class CCInterpolatorLinear : public CCInterpolatorSin2Curve
{
public:
    const float calculatePercentage();
};



template <typename T> 
class CCInterpolatorXY : public CCInterpolator
{
public:
    void setup(float *inX, float *inY, const float target)
    {
        x.setup( inX, target );
        y.setup( inY, target );
    }
    
    void setup(float *inX, float *inY, const float targetX, const float targetY)
    {
        
        if( x.getCurrent() != inX || x.getTarget() != targetX )
        {
            x.setup( inX, targetX );
        }
        
        
        if( y.getCurrent() != inY || y.getTarget() != targetY )
        {
            y.setup( inY, targetY );
        }
    }
    
    const bool update(const float delta)
    {
        float deltaSpeed = delta * speed;
        bool updating = x.update( deltaSpeed );
        updating |= y.update( deltaSpeed );
        return updating;
    }
    
protected:
    T x,y;
};



template <typename T> 
class CCInterpolatorV3 : public CCInterpolator
{
public:
    CCInterpolatorV3() {}
    
    CCInterpolatorV3(CCVector3 *inCurrent, const CCVector3 target)
    {
        setup( inCurrent, target );
    }
    
    const bool equals(CCVector3 *inCurrent, const CCVector3 target)
    {
        if( x.equals( &inCurrent->x, target.x ) &&
            y.equals( &inCurrent->y, target.y ) &&
            z.equals( &inCurrent->z, target.z ) )
        {
            return true;
        }
        return false;
    }
    
    void clear()
    {
        x.setup( NULL );
        y.setup( NULL );
        z.setup( NULL );
    }

    
    void setup(CCVector3 *inCurrent, const CCVector3 target, CCLambdaCallback *inCallback=NULL)
    {
        x.setup( &inCurrent->x, target.x );
        y.setup( &inCurrent->y, target.y );
        z.setup( &inCurrent->z, target.z );
        
        onInterpolated.deleteObjects();
        if( inCallback != NULL )
        {
            onInterpolated.add( inCallback );
        }
    }
    
    void setup(CCVector3 *inCurrent, const float target)
    {
        x.setup( &inCurrent->x, target );
        y.setup( &inCurrent->y, target );
        z.setup( &inCurrent->z, target );
    }
    
    void ready()
    {
        x.ready();
        y.ready();
        z.ready();
    }
    
    const bool update(const float delta)
    {
        float deltaSpeed = delta * speed;
        bool updating = x.update( deltaSpeed );
        updating |= y.update( deltaSpeed );
        updating |= z.update( deltaSpeed );
        
        if( onInterpolated.length > 0 )
        {
            if( updating == false )
            {
                LAMBDA_EMIT_ONCE( onInterpolated );
            }
        }
        
        return updating;
    }
    
    const CCVector3 getAmount() const { return CCVector3( x.getAmount(), y.getAmount(), z.getAmount() ); }
    const CCVector3 getTarget() const { return CCVector3( x.getTarget(), y.getTarget(), z.getTarget() ); }
    
protected:
    T x,y,z;
};



template <typename T> 
class CCInterpolatorListV3 : public CCInterpolator
{
public:
    CCInterpolatorListV3()
    {
        interpolators.deleteObjectsAndList();
    }
    
    const bool pushV3(CCVector3 *inCurrent, const CCVector3 target, const bool replace=false)   
    {
        if( interpolators.length > 0 )
        {
            if( replace )
            {
                bool found = false;
                for( int i=0; i<interpolators.length; ++i )
                {
                    CCInterpolatorV3<T> *interpolator = interpolators.list[i];
                    if( interpolator->equals( inCurrent, target ) )
                    {
                        found = true;
                        if( i != 0 )
                        {
                            interpolator->ready();
                        }
                    }
                    else
                    {
                        interpolators.remove( interpolator );
                        delete interpolator;
                    }
                }
                
                if( found )
                {
                    return false;
                }
            }
            else
            {
                for( int i=0; i<interpolators.length; ++i )
                {
                    CCInterpolatorV3<T> *interpolator = interpolators.list[i];
                    if( interpolator->equals( inCurrent, target ) )
                    {
                        return false;
                    }
                }
            }
        }
        
        if( *inCurrent != target )
        {
            interpolators.add( new CCInterpolatorV3<T>( inCurrent, target ) );
        }
        return true;
    }
    
    const bool update(const float delta);
    
    const bool finished() { return interpolators.length == 0; }
    
public:
    CCList< CCInterpolatorV3< T > > interpolators;
};


class CCInterpolatorLinearColour : public CCInterpolator
{
public:
    CCInterpolatorLinearColour()
    {
        current = NULL;
        updating = false;
    }
    
    CCInterpolatorLinearColour(CCColour *inCurrent, const CCColour inTarget)
    {
        setup( inCurrent, inTarget );
    }
    
    const bool equals(CCColour *inCurrent, const CCColour inTarget)
    { 
        // Ignore if we're already doing this
        if( current != inCurrent || target != inTarget )
        {
            return false;
        }
        return true;
    }
    
    void setup(CCColour *inCurrent, const CCColour inTarget)
    {
        current = inCurrent;
        target = inTarget;
        updating = true;
    }
    
    const bool update(const float delta);
    
    void setTarget(const CCColour inTarget, CCLambdaCallback *inCallback=NULL)
    {
        setup( current, inTarget );
        if( onInterpolated.length > 0 )
        {
            onInterpolated.deleteObjects();
        }
        if( inCallback != NULL )
        {
            onInterpolated.add( inCallback );
        }
    }

    void setTarget(const CCColour *inTarget, CCLambdaCallback *inCallback=NULL)
    {
        setTarget( *inTarget, inCallback );
    }
    
    void setTargetColour(const float grey)
    {
        target.red = grey;
        target.green = grey;
        target.blue = grey;
        updating = true;
    }

    void setTargetAlpha(const float inTargetAlpha, CCLambdaCallback *inCallback=NULL)
    {
        if( target.alpha != inTargetAlpha )
        {
            target.alpha = inTargetAlpha;
            setTarget( target, inCallback );
        }
        else
        {
            if( inCallback != NULL )
            {
                onInterpolated.add( inCallback );
            }
        }
    }

    inline const CCColour& getTarget() const { return target; }
    
protected:
    bool updating;
    CCColour *current;
    CCColour target;
};



class CCTimer : public CCUpdater
{
public:
    CCTimer()
    {
        time = 0.0f;
        updating = false;
        interval = 0.0f;
    }
    const bool update(const float delta);
    void start(const float timeout);
    void stop();
    void restart();
    
    bool updating;
    float time;
    float interval;
    LAMBDA_SIGNAL onTime;
};


#endif // __CCInterpolators_H__

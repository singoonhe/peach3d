//
//  Peach3DScheduler.h
//  Peach3DLib
//
//  Created by singoon.he on 12/13/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SCHEDULER_H
#define PEACH3D_SCHEDULER_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    class PEACH3D_DLL Scheduler
    {
    public:
        void start() {mIsPaused = false;}
        void pause() {mIsPaused = true;}
        void reset() {mCurrentTime = 0.0f;}
        bool isPaused() {return mIsPaused;}
        void trigger(float interval) {
            if (mFixInterval > 0.0f) {
                mCurrentTime += interval;
                if (mCurrentTime > mFixInterval) {
                    mCurrentTime -= mFixInterval;
                    mCallBack(mFixInterval);
                    
                    // reduce count
                    if(mRepeatCount > 0) {
                        mRepeatCount--;
                    }
                }
            }
            else {
                mCallBack(interval);
                
                // reduce count
                if(mRepeatCount > 0) {
                    mRepeatCount--;
                }
            }
        }
        
    protected:
        Scheduler(std::function<void(float)> callback, float interval=0.0f, int repeat=-1):mIsPaused(true),
            mCallBack(callback), mFixInterval(interval), mCurrentTime(0.0f), mRepeatCount(repeat) {}
        
        int getRepeatCount() {return mRepeatCount;}
        /** If set repeat count to 0, scheduler will auto deleted.*/
        void setRepeatCount(int repeat) {mRepeatCount = repeat;}
        
    private:
        float                       mFixInterval;
        float                       mCurrentTime;   // every scheduler must have unique current time.
        bool                        mIsPaused;
        int                         mRepeatCount;   // call back function repeat count, -1 for repeat forever
        std::function<void(float)>  mCallBack;
        friend class IPlatform;     //! IPlatform can call destructor function.
    };
}

#endif /* defined(PEACH3D_SCHEDULER_H) */

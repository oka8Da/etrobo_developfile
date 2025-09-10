#ifndef ETTR_APP_BOTTLEAVOIDANCE_H_
#define ETTR_APP_BOTTLEAVOIDANCE_H_

#include "LineMonitor.h"
#include "Walker.h"
#include "DistanceMeter.h"
#include "LineTracer.h"

class BottleAvoidance {
public:
    BottleAvoidance(LineMonitor* lineMonitor,
                   Walker* walker,
                   DistanceMeter* distanceMeter,
                   LineTracer* lineTracer);//追加(岡田)

    bool ScenarioTransition();
    void Avoid();
    float turnControl(float startDistance, float endDistance, float tCtrl);
    void DistanceReset();

private:
    LineMonitor* mLineMonitor;
    Walker* mWalker;
    DistanceMeter* mDistanceMeter;
    LineTracer* mLineTracer;

    bool mIsInitialized;
    bool mIsOnBlueLine;
    bool mNeverCalled;
    bool mToLineTracer_8;
    bool mScenarioTransition;
    bool mTurnSign;
    

    float mTurnControl;
    float preDistance;
    float currentDistance;
    float diffDistance;
};

#endif // ETTR_APP_BOTTLEAVOIDANCE_H_

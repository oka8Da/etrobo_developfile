#ifndef ETTR_APP_LINETRACER_8_H_
#define ETTR_APP_LINETRACER_8_H_

#include"BottleAvoidance.h"
#include"LineTracer.h"
#include"LineMonitor.h"
#include"DistanceMeter.h"


//定義
class LineTracer_8{
public:
        LineTracer_8(LineMonitor* lineMonitor,
                      LineTracer* lineTracer,DistanceMeter* distanceMeter,
                      BottleAvoidance* bottleAvoidance);//追加(岡田)

        void run_8();
        float turnControl();
        bool ScenarioTransition();

private:

        LineTracer* mLineTracer;
        LineMonitor* mLineMonitor;
        DistanceMeter* mDistanceMeter;
        BottleAvoidance* mBottleAvoidance;


        int8_t  mBlueLineTime;//0でいいのか、何回目の青ラインか,青ラインを超えてから判断、つまり何回青ラインを超えたか
        bool    mWhetherCalled;//呼び出されたことがあるか
        bool    mScenarioTransition;//senario遷移させるか
        bool    mTurnSign;//turnの符号切り替えるかどうか
        bool    mIsOnBlueLine; 
        bool    mIsOnLine;
        bool    mNotIsBlueLine;
        bool    mTurnInitialized;
        float   preDistance;
        float   currentDistance;
        float   diffDistance;
        float   mTurnControl;
        int8_t  BlueLineTime();
        float   turnControl(float startDistance,float endDistance,float tCtrl);
      

};

#endif //ETTR_APP_LINETRACER_8_H_

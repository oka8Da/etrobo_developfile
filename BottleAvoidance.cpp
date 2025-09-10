//okada
#include "BottleAvoidance.h"
#include "LineMonitor.h"
#include "LineTracer.h"
//追加
#include "Walker.h"
#include "DistanceMeter.h"
#include "stdio.h"

/**
 * コンストラクタ
 * @param lineMonitor     ライン判定
 * @param walker 走行
 */

BottleAvoidance::BottleAvoidance(LineMonitor* lineMonitor,
                                Walker* walker,DistanceMeter* distanceMeter,LineTracer* lineTracer)//追加
                       
    : mLineMonitor(lineMonitor),
      mWalker(walker),
      mDistanceMeter(distanceMeter),
      mLineTracer(lineTracer),
      //tuika
      mIsInitialized(false),
      //追加
      mIsOnBlueLine(false),
      mNeverCalled(false),
      mToLineTracer_8(false),
      //追加（岡田）
      mTurnControl(0),
      preDistance(0),
      currentDistance(0),
      diffDistance(0),
      mTurnSign(false),
      mScenarioTransition(false) {
} 

bool BottleAvoidance::ScenarioTransition(){
    if(mScenarioTransition == true){
        printf("Test2に移行します\n");
        return true;
    }
    else 
        return false;
}

//追加（岡田）
/**
 * ボトルを避ける
 */
void BottleAvoidance::Avoid(){
    // 最初の呼び出し時だけstop()を実行
    if (!mIsInitialized) {
        mWalker->stop();
        mWalker->turn(10,30);
        mIsInitialized = true;
    }

    //その場で旋回
    mTurnControl = BottleAvoidance::turnControl(0.0,6.0,50.0);
    printf("%.2f",mTurnControl);
    if(mTurnControl > 0.0){
        mWalker->setCommand(mTurnControl);
        mWalker->run_fast();//Lは10,Rは50
        printf("\nsenkaityuu");
    }

    if(mTurnControl == 0.0){//turnControlの返り値はdifdistanceがfinishdistanceを超えてから0を返し続ける       
        if(6.0 < diffDistance && diffDistance <= 27.0){//前進させたい距離
            mWalker->turn(50,50);
            printf("\ntyokusinntyuu");
        }

        else if(27.0 < diffDistance && diffDistance <=52.0){//後進させたい距離
            mWalker->turn(-50,-50);//関数名がややこしいが、後進させている
            printf("\nkousinntyuu");

        } 

        else if(52.0 < diffDistance && diffDistance <= 56.0){//ライントレースできる位置まで持っていってあげる
            mWalker->turn(60,-60);
         
            
        } else {    
            mScenarioTransition = true;//senarioをLINETRACER2に遷移
            printf("\nTo camerastage.");

        }
            
    }

}

float BottleAvoidance::turnControl(float startDistance,float endDistance,float tCtrl){//引数に距離と力
        currentDistance = mDistanceMeter->calDistance();
        if(preDistance == 0){//呼び出した時リセットが必要
            preDistance = currentDistance;//計算を開始したときの今までの距離
        }
        diffDistance = diffDistance + (currentDistance - preDistance);//計算開始からの距離合計
        preDistance  = currentDistance;
        //printf("currentDisntance:%.2f",currentDistance);
        //printf("preDistance:%.2f",preDistance);
        printf("\ndiffDistane:%.2f\n",diffDistance);
        if(startDistance <= diffDistance && diffDistance <= endDistance){//どのくらいの距離制御するかorどのくらい走ったら制御するか
            mTurnControl = tCtrl;
        }
        else{
            mTurnControl = 0.0;
        }
    
        return mTurnControl;
}

void BottleAvoidance::DistanceReset(){
    preDistance = 0;
    diffDistance = 0;
}

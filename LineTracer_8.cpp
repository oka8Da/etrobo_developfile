#include "LineTracer_8.h"

//コンストラクタ
LineTracer_8::LineTracer_8(LineMonitor* lineMonitor,
                            LineTracer* lineTracer,DistanceMeter *distanceMeter,
                            BottleAvoidance* bottleAvoidance)//追加(岡田)
    : mLineMonitor(lineMonitor),
      mLineTracer(lineTracer),
      mDistanceMeter(distanceMeter),
      mBottleAvoidance(bottleAvoidance),
      mBlueLineTime(0),//0でいいのか、何回目の青ラインか,青ラインを超えてから判断、つまり何回青ラインを超えたか
      mWhetherCalled(true),//呼び出されたことがあるか
      mScenarioTransition(false),//senario遷移させるか
      mTurnSign(false),//turnの符号切り替え
      mTurnInitialized(false),//いらない
      mIsOnBlueLine(false),
      mNotIsBlueLine(false),//青ラインを越したかどうか
      preDistance(0),
      currentDistance(0),
      diffDistance(0),
      mTurnControl(0),//青ライン上で直接モーターを制御する
      mIsOnLine(false){
}

bool LineTracer_8::ScenarioTransition(){
    if(mScenarioTransition == true){
        printf("TEST2に移行します\n");
        return true;
    }
    else 
        return false;
} 

void LineTracer_8::run_8(){ 
    //一つ目の青ラインに乗っているとき左のモーターを強くする
    mBlueLineTime = LineTracer_8::BlueLineTime();
    //ここでDistanceMeter呼び出し続ける
    switch(mBlueLineTime){//何回青ラインを超えたかどうか

    case 0:       
                mTurnControl = 0;
                mTurnSign =false;
                mLineTracer->run(mTurnSign,mTurnControl);
                printf("一つ目の青ラインにいます\n");
    case 1:        
                mTurnControl = LineTracer_8::turnControl(2.0,4.0,5.0);
                mTurnSign = false;//mTurnSignもここで宣言してもよい
                mLineTracer->run_fast(mTurnSign,mTurnControl);
                printf("一つ目の青ラインを超えました\n");
                printf("mTurnControl:%.2f",mTurnControl);
                //切り替えなし
                break;
    case 2:     
                
                mTurnControl = LineTracer_8::turnControl(0.1,2.0,6.0);  
                
                mTurnSign = true;
                mLineTracer->run_fast(mTurnSign,mTurnControl);
                
                printf("2つ目の青ラインを超えました\n");
                printf("mTurnControl:%.2f",mTurnControl);
                //切り替えあり、左に切り替え,mTurnの符号を変える
                break;
    case 3:         
                mTurnControl = LineTracer_8::turnControl(0.1,2.0,-11.0);  
                mTurnSign = false;
                mLineTracer->run_fast(mTurnSign,mTurnControl);
                
                printf("3つ目の青ラインを超えました\n");
                //切り替えあり、右に切り替え
                break;
    
    case 4:         
                //mTurnControl = 0; 
                mBottleAvoidance->Avoid();
                //mTurnControl = LineTracer_8::turnControl(0.0,1.0,-3.0);  //左の車輪を強くする    
                //mTurnSign = true;
                //mLineTracer->run(mTurnSign,mTurnControl);//切り替えあり、左に切り替え
                //mScenarioTransition = true;//senarioをLINETRACER2に遷移
                
                if(mBottleAvoidance->ScenarioTransition()){
                    mScenarioTransition = true;//senarioをTEST2に遷移
                    
                }

                printf("4つ目の青ラインを超えました\n");
                break;   
    }  
}

int8_t LineTracer_8::BlueLineTime(){

    //mDistanceMeter->calWheelAngle();


    mIsOnBlueLine    = mLineMonitor->isOnBlueLine();//IsOnBlueLineはここで宣言してもよい
    mIsOnLine        = mLineMonitor->isOnLine();
    mNotIsBlueLine   = mLineMonitor->notIsBlueLine();

    if(mIsOnBlueLine){
        if(mWhetherCalled == true){
                        //青ラインに乗った時一度turnを初期化する
            mWhetherCalled = false; 
            printf("mBLueLineTime:%d\n",mBlueLineTime);
        }
        
    }
    if(mNotIsBlueLine){//青ライン上にいない＆黒ライン上にいる
        if(mWhetherCalled == false){
            if(diffDistance >= 20){
                printf("超えたのでライン加算します\n");
                mBlueLineTime++;//mBlueLineTimeとdiffDistanceはセット
                diffDistance = 0;//距離合計をリセット  
                mWhetherCalled = true;//また、青ラインを踏んだ時に更新できるようにする 
            }
        }           
        mTurnInitialized = false; //この変数いらない 
    }
    return mBlueLineTime;
}
    
float LineTracer_8::turnControl(float startDistance,float endDistance,float tCtrl){//引数に距離と力
        currentDistance = mDistanceMeter->calDistance();
        if(preDistance == 0){
            preDistance =currentDistance;//計算を開始したときの今までの距離
        }
        diffDistance = diffDistance + (currentDistance - preDistance);//計算開始からの距離合計
        preDistance  = currentDistance;
        //printf("currentDisntance:%.2f",currentDistance);
        //printf("preDistance:%.2f",preDistance);
        printf("\ndiffDistane:%.2f\n",diffDistance);
        if(startDistance < diffDistance && diffDistance < endDistance){//どのくらいの距離制御するかorどのくらい走ったら制御するか
            mTurnControl = tCtrl;
        }
        else{
            mTurnControl = 0.0;
        }

        return mTurnControl;
}
  

//bool LineTracer_8::blueAcutalizar()   

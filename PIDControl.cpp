#include "PIDControl.h"
#include "LineTracer.h"
#include "stdio.h"

// 定数宣言
const float PIDControl::Kp = 0.3;
const int 	PIDControl::bias = 0;//走る位置の制御



//追加
const float   PIDControl::INITIAL_DERIVATIVE_ = 0.0;//変化率の初期化
const float   PIDControl::INITIAL_PREVOUSDIFF_=0.0;
const float   PIDControl::INITIAL_SUMDIFF_=0.0;

/**
 * コンストラクタ
 * 
 * 
 * @param lineMonitor     ライン判定
 * @param walker 走行
 */
//追加
PIDControl::PIDControl()
        :previousDiff(PIDControl::INITIAL_PREVOUSDIFF_),
         derivative(PIDControl::INITIAL_DERIVATIVE_),
         sumDiff(PIDControl::INITIAL_SUMDIFF_){
       }



/**
 * 走行体の操作量を計算する
 * @param diffBrightness ラインから外れた度合い（ライン閾値との差）
 */
 //pid seigyo no p
float PIDControl::calcPropValue(float diffBrightness) {//diffが入る

    //追加：微分計算:D制御
    float currentDiff = diffBrightness;
    derivative = currentDiff-previousDiff;
    previousDiff = currentDiff;
    printf("derivative:%.2f\n",derivative);

    //追加:積分計算:I制御:diffかderivativeどっちがいいか
    if(derivative > 5){//変化率が大きい場合だけ加算
      sumDiff = sumDiff - derivative * 0.1;
    }
    else if(derivative < -5){
      sumDiff = sumDiff + derivative * 0.1;
    }
    
    printf("sumDiff:%.2f\n",sumDiff);


    //PID制御計算
    float turn =((PIDControl::Kp * diffBrightness) * 1.75) + ((PIDControl::Kp * derivative) * 3.0) + ((PIDControl::Kp * sumDiff) * 0.003) + PIDControl::bias;
	
    return turn;
}



float PIDControl::calcPropValue_fast(float diffBrightness) {//run_fast用のPID制御値計算

    //追加：微分計算:D制御
    float currentDiff = diffBrightness;
    derivative = currentDiff-previousDiff;
    previousDiff = currentDiff;
    printf("derivative:%.2f\n",derivative);

    //追加:積分計算:I制御:diffかderivativeどっちがいいか
    if(derivative > 5){//変化率が大きい場合だけ加算
      sumDiff = sumDiff - derivative * 0.1;
    }
    else if(derivative < -5){
      sumDiff = sumDiff + derivative * 0.1;
    }
    
    printf("sumDiff:%.2f\n",sumDiff);


    //PID制御計算
    float turn =((PIDControl::Kp * diffBrightness) * 1.3) + ((PIDControl::Kp * derivative) * 3.0) + ((PIDControl::Kp * sumDiff) * 0.003) + PIDControl::bias;
	//pwm:60                                         1.3                                     3.0                                  0.003
    return turn;
}


//D(微分)制御関数:前回のずれと今回のズレの変化量、閾値で大きいか判断。pとdの比重の設定
//drivative=currentDiffBrightness-prevousDiffBrightness
//calcPropoにderivative引数として


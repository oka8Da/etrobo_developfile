/******************************************************************************
 *  app.cpp (for SPIKE)
 *  Created on: 2025/01/05
 *  Implementation of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Modifier: Yuki Tsuchitoi
 *  Copyright (c) 2025 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "app.h"
#include "ScenarioTracer.h"
#include "PIDControl.h"

// デストラクタ問題の回避
// https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping
//void *__dso_handle=0;

#include "Light.h"
#include "Button.h"
#include "Display.h"


#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <signal.h>
#include <iostream>

// using宣言
using namespace spikeapi;
//追加
using namespace cv;


// Device objects
// オブジェクトを静的に確保する
ColorSensor gColorSensor(EPort::PORT_E);
ForceSensor gForceSensor(EPort::PORT_D);
Motor       gLeftWheel(EPort::PORT_B,Motor::EDirection::COUNTERCLOCKWISE,true);
Motor       gRightWheel(EPort::PORT_A,Motor::EDirection::CLOCKWISE,true);
Clock       gClock;

//OpenCV用オブジェクト
//cv::VideoCapture gCap(0);//0は接続しているカメラの番号
cv::Mat          gFrame;//フレーム（一枚保存みたいな感じ)
cv::Mat          gFrame_HSV;
cv::Mat          gFrame_Blue;
cv::Mat          gFrame_Gray;
static sigset_t  signal_mask;//シグナルマスクの変数
static pthread_t CameraThread;//カメラスレッド　並列させるため
static  pthread_mutex_t gMutex;
// オブジェクトの定義
static Walker          *gWalker;
static LineMonitor     *gLineMonitor;
static Starter         *gStarter;
static LineTracer      *gLineTracer;
static ScenarioTracer  *gScenarioTracer;
static Test            *gTest;

//追加（岡田）
static BottleAvoidance *gBottleAvoidance;

//追加
static LineTracer_8    *gLineTracer_8;
static PIDControl      *gPIDControl;
static DistanceMeter   *gDistanceMeter;



void Denoising_Blue(cv::Mat& gFrame);
void Denoising_Gray(cv::Mat& gFrame);


/**
 * システム生成
 */
static void user_system_create() {
    // [TODO] タッチセンサの初期化に2msのdelayがあるため、ここで待つ
    tslp_tsk(2U * 1000U);

    // オブジェクトの作成
    gWalker          = new Walker(gLeftWheel,
                                  gRightWheel);
    gStarter         = new Starter(gForceSensor);
    gLineMonitor     = new LineMonitor(gColorSensor);
    gPIDControl      = new PIDControl();
    gDistanceMeter   = new DistanceMeter(gLeftWheel,gRightWheel);
    gLineTracer      = new LineTracer(gLineMonitor, gWalker,gDistanceMeter,gPIDControl);
    gTest            = new Test(gLineMonitor, gWalker);
    //追加（岡田）
    gBottleAvoidance = new BottleAvoidance(gLineMonitor,gWalker,gDistanceMeter,gLineTracer);

    //追加;
    gLineTracer_8    = new LineTracer_8(gLineMonitor,gLineTracer,gDistanceMeter,gBottleAvoidance);
    gScenarioTracer  = new ScenarioTracer(gStarter, gWalker, gLineTracer,gLineTracer_8,gDistanceMeter);
    

    Light light;
    light.turnOnColor(Light::EColor::ORANGE);



}

/**
* システム破棄
 */
static void user_system_destroy() {
    gLeftWheel.stop();
    gRightWheel.stop();
    gLeftWheel.resetCount();
    gRightWheel.resetCount();
    //追加
    gLeftWheel.getCount();
    gRightWheel.getCount();

    delete gScenarioTracer;
    delete gLineTracer;
    delete gLineMonitor;
    delete gStarter;
    delete gWalker;
    delete gPIDControl;
    delete gLineTracer_8;
    delete gDistanceMeter;
}

/**
 * メインタスク
 */
void main_task(intptr_t unused) {//unusedは引数として必要だが、このタスクでは使わない
    user_system_create();  // センサやモータの初期化処理　↑のコードがsystem_create

    // 周期ハンドラ開始
    /*
    周期ハンドラ：一定時間で自動で呼び出される
    周期ハンドラはひとつしか呼び出せないため、ライントレースタスクしか原則呼べない
    なので、スレッドを作成して、並列処理を行いたい、ひとつしか動いていないように見せるたためにマスクで隠す   
    マスク処理はシグナル制御で行う
    */
    //ライントレースと画面表示を並列処理で行う
    
    
    //マスク処理開始
    LockSignalMask();

    pthread_mutex_init(&gMutex,nullptr);
    //thread作成:threadとは並列処理をするための関数・カメラ画面を表示
    pthread_create(&CameraThread,nullptr,CameraShow,nullptr);//std::thread CameraThread(CameraShow);

    //マスク処理終了
    UnlockSignalMask();
    
    
    
    

    //ライントレースタスク
    sta_cyc(CYC_TRACER);//一定周期でtracer_taskを呼び出す
    
    slp_tsk();  // バックボタンが押されるまで待つ:バックボタンは本体のボタン(おそらく、正面のETとか表示しているボタン)

    // 周期ハンドラ停止
    stp_cyc(CYC_TRACER);

    //マスク処理開始
    LockSignalMask();

    pthread_join(CameraThread,nullptr);//CameraShowが終わるのを待つ:detachは独立、joinは終わるまで待つ

    //マスク処理終了
    UnlockSignalMask();

    user_system_destroy();  // 終了処理

    ext_tsk();
}

/**
 * ライントレースタスク
 */
void tracer_task(intptr_t exinf) {
  Button button;
  
	if (button.isLeftPressed()) {
	    wup_tsk(MAIN_TASK);  // レフトボタン押下
    } else {
        //ここでモーターが動き出す
        gScenarioTracer->run();  // 走行
    }

    ext_tsk();
}

//青だけ白で表示する処理　ノイズ処理
void Denoising_Blue(cv::Mat& gFrame){//gFrameは参照型なので戻り値で返さなくてもよい
        // HSVに変換
        cv::cvtColor(gFrame,gFrame_HSV, cv::COLOR_BGR2HSV);
        // 青色範囲を設定 (HSV):H:色相 S:彩度 V:明度　　H:120ぐらいが青 S:彩度は0-255だが青なので大きめに　V:0-255 青は色価で暗くなりがち 0が黒
        cv::Scalar Lower_blue(100, 150, 50);  // 下限
        cv::Scalar Upper_blue(140, 255, 255); // 上限
        //inRange関数：値の範囲内の画素を白に、それ以外を黒に変換する->青を白に変換
        cv::inRange(gFrame_HSV,Lower_blue,Upper_blue,gFrame_Blue);
        //5*5の長方形カーネルを作成
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        //ノイズ処理　カーネルは5*5以上の穴を埋める　Morph_openで　　精度を上げるための処理
        cv::morphologyEx(gFrame_Blue,gFrame_Blue, cv::MORPH_OPEN, kernel);
    }

void Denoising_Gray(cv::Mat& gFrame){
        // HSVに変換
        cv::cvtColor(gFrame,gFrame_HSV, cv::COLOR_BGR2HSV);
        // 青色範囲を設定 (HSV)
        cv::Scalar Lower_gray(0, 0, 30);  // 下限
        cv::Scalar Upper_gray(180,70,220); // 上限
        //inRange関数：値の範囲内の画素を白に、それ以外を黒に変換する->青を白に変換
        cv::inRange(gFrame_HSV,Lower_gray,Upper_gray,gFrame_Gray);
        //5*5の長方形カーネルを作成
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        //ノイズ処理　カーネルは5*5以上の穴を埋める　Morph_openで　　精度を上げるための処理
        cv::morphologyEx(gFrame_Gray,gFrame_Gray, cv::MORPH_OPEN, kernel);
    }


void* CameraShow(void* arg){
    // LockSignalMask();

    
    cv::VideoCapture gCap(0);//0は接続しているカメラの番号
    if(!gCap.isOpened()){
        std::cout << "カメラが開けません" << std::endl;
    }
    else{
        std::cout << "カメラを開けました" << std::endl;
    }
    while(true){

        pthread_mutex_lock(&gMutex);
        pthread_testcancel();
        
        gCap >> gFrame;//カメラから1枚保存
        //青だけを白で表示する
        //Denoising_Blue(gFrame);
        Denoising_Gray(gFrame);
        if(!gFrame_Gray.empty()){
        cv::imshow("Camera",gFrame_Gray);//保存したものを表示
        cv::waitKey(1);//表示するうえで必須:GUIの役割
        }
        
        
        //cv::Mat gray_frame;
        //グレースケールに変換
        //cv::cvtColor(gFrame,gray_frame,cv::COLOR_BGR2GRAY);
        /*
        if(!gFrame.empty()){
            cv::imshow("Camera",gFrame);//保存したものを表示
            cv::waitKey(1);//表示するうえで必須:GUIの役割
            
        }
        */
        pthread_mutex_unlock(&gMutex);
        usleep(1000);//cpu使用率を下げる
            
        //UnlockSignalMask();
    }
    

    
    pthread_exit(nullptr);//returnみたいなの
}

//マスク開始処理
void LockSignalMask(){
    sigemptyset(&signal_mask);//シグナル集合の初期化
    sigaddset(&signal_mask,SIGUSR2);//シグナル集合にSIGUSR1を追加
    sigaddset(&signal_mask, SIGPOLL);
    sigaddset(&signal_mask, SIGALRM);
    //引数1：シグナル集合　　2：ブロックor許可したいシグナル番号
    pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);//特定のシグナルをブロックしたり許可する関数
    //引数:1-sigaddsetにあるシグナルをブロック（無視）
    //引数2:シグナルを解除　３－現在のマスクを置き換え
    //引数3:古いマスクを指定する場合は指定する
    
}

//マスク解除処理
void UnlockSignalMask(){
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask,SIGUSR2);   
    sigaddset(&signal_mask, SIGPOLL);
    sigaddset(&signal_mask, SIGALRM);
    pthread_sigmask(SIG_UNBLOCK, &signal_mask, NULL);
}

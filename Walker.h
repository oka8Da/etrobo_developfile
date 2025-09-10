/******************************************************************************
 *  Walker.cpp (for SPIKE)
 *  Created on: 2025/01/05
 *  Implementation of the Class Walker
 *  Author: Kazuhiro.Kawachi
 *  Modifier: Yuki Tsuchitoi
 *  Copyright (c) 2025 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef ETTR_UNIT_WALKER_H_
#define ETTR_UNIT_WALKER_H_

#include "Motor.h"

class Walker {
public:
    static const int RIGHT_TURN = -30;  
    static const int LEFT_TURN = 30; 

    static const int8_t RIGHT;
    static const int8_t LEFT;
    static const int8_t DECIDE;   
  
    Walker(spikeapi::Motor& leftWheel,
                    spikeapi::Motor& rightWheel);

    void init();
    void run();
    void run_fast(); //追加（岡田）
    void run_so_fast(); //tuika(okada)
    void stop();
    void turn(int LeftTurn,int RightTurn);
    void setCommand(float turn);
    void OnOff(uint8_t turn);

private:
    spikeapi::Motor& mLeftWheel;
    spikeapi::Motor& mRightWheel;
    int mTurn;
    int mForward;
    int mForwardRight;
    int mForwardLeft;

  #ifndef MAKE_RASPIKE
    const int8_t pwm = 70;
  #else
    const int8_t pwm = 40;
  #endif
};

#endif  // ETTR_UNIT_WALKER_H_


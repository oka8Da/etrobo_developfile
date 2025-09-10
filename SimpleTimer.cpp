/******************************************************************************
 *  SimpleTimer.cpp  (for SPIKE )
 *  Created on: 2025/01/05
 *  Definition of the Class SimpleTimer
 *  Author: Kenya Yabe
 *  Modifier : Yuki Tsuchitoi
 *  Copyright (c) 2025 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "SimpleTimer.h"

/**
 * コンストラクタ
 * @param clock クロック
 */
SimpleTimer::SimpleTimer(const spikeapi::Clock& clock)
    : mClock(clock),
      mDelayMs(0),
      mStartTime(0),
      mTargetTime(0) {
}

/**
 * タイマ満期時間設定
 * @param delay 満期時間[ms]
 */
void SimpleTimer::setTime(uint32_t delay) {
    mDelayMs = delay;
}

/**
 * タイマ開始
 */
void SimpleTimer::start() {
    mStartTime  = mClock.now();
    mTargetTime = mStartTime + mDelayMs;
}

/**
 * タイマ停止
 */
void SimpleTimer::stop() {
    mStartTime  = 0;
    mTargetTime = 0;
}

/**
 * タイマ満期したか否か
 * @retval true  タイマ満期した
 * @retval false タイマ満期していない
 */
bool SimpleTimer::isTimedOut() const {
    return (mClock.now() >= mTargetTime) ? true : false;
}

/**
 * タイマ開始したか否か
 * @retval true  タイマ開始した
 * @retval false タイマ開始していない
 */
bool SimpleTimer::isStarted() const {
    return (mStartTime == 0) ? false : true;
}

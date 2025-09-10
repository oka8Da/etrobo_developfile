/******************************************************************************
 *  app.h (for SPIKE)
 *  Created on: 2025/01/05
 *  Definition of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Modifier: Yuki Tsuchitoi
 *  Copyright (c) 2025 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#include "spikeapi.h"




/*
 *  各タスクの優先度の定義
 */
#define MAIN_PRIORITY    TMIN_APP_TPRI + 1  /* メインタスクの優先度 */
#define TRACER_PRIORITY  TMIN_APP_TPRI + 2

/*
 *  ターゲットに依存する可能性のある定数の定義
 */
#ifndef STACK_SIZE
#define STACK_SIZE      4096        /* タスクのスタックサイズ */
#endif /* STACK_SIZE */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void tracer_task(intptr_t exinf);
extern void ev3_cyc_tracer(intptr_t exinf);

//extern void Denoising_Blue();
//extern void Denosing_Gray();
extern void* CameraShow(void* arg);
extern void LockSignalMask();
extern void UnlockSignalMask();

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif


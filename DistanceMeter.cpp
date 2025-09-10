#include "DistanceMeter.h"
#include <stdio.h>



DistanceMeter::DistanceMeter(spikeapi::Motor& leftWheel,
                                spikeapi::Motor& rightWheel)
            :mLeftWheel(leftWheel),
             mRightWheel(rightWheel),
             preLeftAngle(0),
             preRightAngle(0),
             WheelDistance_currentState(0),
             preDistance(0),
             currentDistance(0),
             DiffLeftAngle(0),
             DiffRightAngle(0),
             WheelDistance(0){
             }


float DistanceMeter::calDistance(){
    //�����炭LeftDistance,currentLeftAngle�͐�Βl�I�Ȃ��� 
    float currentLeftAngle  = mLeftWheel.getCount();//���݂̍��̎ԗւ̊p�x�𒲂ׂ�
    float currentRightAngle = mRightWheel.getCount();

    //diffLeftAngel�ɍŌ㍇�v��˂�����
    if(DiffLeftAngle == 0 || preLeftAngle == 0){
        DiffLeftAngle = currentLeftAngle;
        preLeftAngle  = currentLeftAngle;
    }
    if(DiffRightAngle == 0 || preRightAngle == 0){
        DiffRightAngle = currentRightAngle;
        preRightAngle  = currentRightAngle;
    }
    //�o�b�N����
    if(currentLeftAngle < preLeftAngle)//�O��̂ق����傫��->�o�b�N���Ă���->�O��̒l�ɍ���̍����Βl�Ń^�X
        DiffLeftAngle = DiffLeftAngle + (preLeftAngle - currentLeftAngle);
    else  
        DiffLeftAngle  = DiffLeftAngle + (currentLeftAngle -  preLeftAngle);
    

    if(currentRightAngle < preRightAngle)
        DiffRightAngle = DiffRightAngle + (preRightAngle - currentRightAngle);
    
    else
        DiffRightAngle = DiffLeftAngle + (currentRightAngle - preRightAngle);
    

    
    //preLeftAngle�͂�����0�ŌŒ� DiffLeftAngle�͂����Ƒ傫���Ȃ�Ð}����o�b�N���Ƃ��������Ȃ������
    //�O�񂩂�ԗւ��������p�x�𒲂ׂ�

    preLeftAngle  = currentLeftAngle;//�X�V
    preRightAngle = currentRightAngle;
    

    //���a*3.14 =�ʂ̒���   ���a:2
    float LeftDistance  = (5.5 * 3.14) * (DiffLeftAngle / 360);//�ʂ̒����ɑ΂��āA�ԗւ��������p�x���̒����𒲂ׂ�
    float RightDistance = (5.5 * 3.14) * (DiffRightAngle / 360);

    
    //�����܂�


    WheelDistance = (LeftDistance + RightDistance)/2;
    
    printf("WheelDisntance:%.2f\n",WheelDistance);

    return WheelDistance;
}
float DistanceMeter::calDistance_currentState(){
    currentDistance = DistanceMeter::calDistance();
        if(preDistance == 0){//�Ăяo���������Z�b�g���K�v
            preDistance = currentDistance;//�v�Z���J�n�����Ƃ��̍��܂ł̋���
        }
        WheelDistance_currentState = WheelDistance_currentState + (currentDistance - preDistance);//�v�Z�J�n����̋������v
        preDistance  = currentDistance;
        //printf("currentDisntance:%.2f",currentDistance);
        //printf("preDistance:%.2f",preDistance);
        
    printf("WheelDisntance_currentState:%.2f\n",WheelDistance_currentState);

    return WheelDistance_currentState;
}
void  DistanceMeter::calDistance_currentStateReset(){
    WheelDistance_currentState = 0.0;
    preDistance = 0.0;
}

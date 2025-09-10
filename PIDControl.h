#ifndef ETTR_UNIT_PIDCONTROL_H_
#define ETTR_UNIT_PIDCONTROL_H_//二重定義を防止するため

//#include "LineTracer.h"



class PIDControl{
public:
		static const float Kp;
		static const int   bias;
		float calcPropValue(float diffReflection);
		float calcPropValue_fast(float diffReflection);//追加(岡田)

		//追加
		static const float  INITIAL_PREVOUSDIFF_;
		static const float  INITIAL_DERIVATIVE_;
		static const float  INITIAL_SUMDIFF_;
		PIDControl();
		
	
private:
		//LineTracer* lineTracer;
		float previousDiff;
		float derivative;
		float sumDiff;
		

};
	








#endif //ETTR_UNIT_PIDCONTROL_H_

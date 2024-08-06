// ---------------------------------------------------------------------------
// xsin.c
// ---------------------------------------------------------------------------
//
// Fixed Point Sine
//

#include <stdint.h>
#include "FixedMath.h"

fix16_t __fastcall__ xsin(fix16_t inAngle){
	fix16_t tempAngle = inAngle % FX_PI2;
	fix16_t tempAngleSq;
	fix16_t tempOut;
	
	if(tempAngle > FX_PI){
		tempAngle = tempAngle - FX_PI2;
	}
	else if(tempAngle < -FX_PI){
		tempAngle = tempAngle + FX_PI2;
	}
	
	tempAngleSq = xmul(tempAngle, tempAngle);
	
	// Fast implementation, accuracy of ~2.3%
	tempOut = xmul(-13, tempAngleSq) + 546;
	tempOut = xmul(tempOut, tempAngleSq) - 10923;
	tempOut = xmul(tempOut, tempAngleSq) + 65536;
	return xmul(tempOut, tempAngle);
}


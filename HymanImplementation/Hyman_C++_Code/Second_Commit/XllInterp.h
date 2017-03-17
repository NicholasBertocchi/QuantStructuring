#ifndef INC_XLLINTERP_H
#define INC_XLLINTERP_H

#include "excel32.h"

extern "C"{

	LPXLOPER EXCEL_EXPORT xlInterp1D(XlfOper xlInX, XlfOper xlInY, XlfOper xlOutX, XlfOper xlInterpMethod, XlfOper xlBoundaryType, XlfOper xlExtrapolator, XlfOper xlHymanFilter);
	LPXLOPER EXCEL_EXPORT xlInterpLinear(XlfOper xlInX, XlfOper xlInY, XlfOper xlOutX);
	LPXLOPER EXCEL_EXPORT xlInterpNaturalBSpline(XlfOper xlInX, XlfOper xlInY, XlfOper xlOutX);
   LPXLOPER EXCEL_EXPORT xlInterpCubicSpline(XlfOper xlInX, XlfOper xlInY, XlfOper xlOutX, XlfOper xlBoundaryType, XlfOper xlExtrapolator, XlfOper xlHymanFilter);
	LPXLOPER EXCEL_EXPORT xlInterp2D(XlfOper xlInX, XlfOper xlInY, XlfOper xlInZ, XlfOper xlOutX, XlfOper xlOutY, XlfOper xlInterpMethod);
	LPXLOPER EXCEL_EXPORT xlInterpLinearSurf(XlfOper xlInX, XlfOper xlInY, XlfOper xlInZ, XlfOper xlOutX, XlfOper xlOutY);
	LPXLOPER EXCEL_EXPORT xlInterpNaturalBSplineSurf(XlfOper xlInX, XlfOper xlInY, XlfOper xlInZ, XlfOper xlOutX, XlfOper xlOutY);
	LPXLOPER EXCEL_EXPORT xlInterpKruger(XlfOper xlInX, XlfOper xlInY, XlfOper xlOutX);
}

#endif  // INC_XLLINTERP_H


//ciInterp.cpp
#include "ciInterp.h"
#include "cError.h"
#include "cMatrix.h"
#include "eInterp.h"
#include "eBSpline.h"
#include "eCubicSpline.h"
#include "eContInterp.hpp"
#include "eBSplineUtility.h"
#include <vector>
#include <iostream>
#include <math.h>
#include <string>
#include <list>
#include <algorithm>

PDGLIB_API pdgerr_t pdg_interpNaturalBSpline(double *InX, double *InY, long nIn,
                                             double *OutX, double *OutY, long nOut)

{
   try {

      std::vector<double> inX(InX,InX+nIn);
      std::vector<double> inY(InY,InY+nIn);
      std::vector<double> outX(OutX,OutX+nOut);

      std::vector<double> tmpOut = bspline::interpNaturalBSpline<double>(inX, inY, outX);

      for (int i = 0; i < nOut; ++i) {
         OutY[i] = tmpOut[i];
      }

   }
   catch(pdg::Error &e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

PDGLIB_API pdgerr_t pdg_interpCubicSpline(double *InX, double *InY, long nIn, 
                                          double *OutX, double *OutY, long nOut,
                                          long boundaryType, long Extrapolator, bool HymanFilter)

{
   try {

      std::vector<double> inX(InX,InX+nIn);
      std::vector<double> inY(InY,InY+nIn);
      std::vector<double> outX(OutX,OutX+nOut);


      std::vector<double> tmpOut = cubicspline::interpCubicSpline<double>(inX, inY, outX,
                                                                          boundaryType, Extrapolator, HymanFilter);

      for (int i = 0; i < nOut; ++i) {
         OutY[i] = tmpOut[i];
      }

   }
   catch(pdg::Error &e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

PDGLIB_API pdgerr_t pdg_interpNaturalBSplineSurf(long szInX, double *InX, long szInY, double *InY,
                                                 double **InZ, long szOutX, double *OutX,
                                                 long szOutY, double *OutY, double **OutZ)
{
   try {
      Matrix MInX(szInX, 1, InX);
      Matrix MInY(szInY, 1, InY);
      Matrix MInZ(szInX, szInY, InZ);
      Matrix MOutX(szOutX, 1, OutX);
      Matrix MOutY(szOutY, 1, OutY);

      Matrix MOutZ = interp::interpNaturalBSplineSurf(MInX, MInY, MInZ, MOutX, MOutY);

      long r, c;
      for (r = 0; r < szOutX; ++r)
         for (c = 0; c < szOutY; ++c)
            OutZ[r][c] = MOutZ(r, c);

   }
   catch(pdg::Error &e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

PDGLIB_API pdgerr_t pdg_interpLinearSurf(long szInX, double *InX, long szInY, double *InY,
                                         double **InZ, double OutX,
                                         double OutY, double *pVal)
{
	try {
      std::vector<double> vecXi(szInX);
      std::copy(InX, InX + szInX, vecXi.begin());

      std::vector<double> vecYi(szInY);
      std::copy(InY, InY + szInY, vecYi.begin());

      std::matrix<double> matZi(szInX, szInY, InZ);

		*pVal = interp::interpLinearSurf(vecXi, vecYi, matZi, OutX, OutY);
	}
	catch(pdg::Error &e) {
		return e.getInfo();
	}
	catch(...) {
		return RES_FAIL;
	}
	return RES_OK;
}

PDGLIB_API pdgerr_t pdg_regressBSplineSurf(long szBaseX, double *BaseX, long szBaseY, double *BaseY,
                                           long szInX, double *InX, long szInY, double *InY,
                                           long szInZ, double *InZ, long szOutX, double *OutX,
                                           long szOutY, double *OutY, double **OutZ)
{
   try {
      Matrix MBaseX(szBaseX, 1, BaseX);
      Matrix MBaseY(szBaseY, 1, BaseY);
      Matrix MInX(szInX, 1, InX);
      Matrix MInY(szInY, 1, InY);
      Matrix MInZ(szInZ, 1, InZ);
      Matrix MOutX(szOutX, 1, OutX);
      Matrix MOutY(szOutY, 1, OutY);

      Matrix MOutZ = interp::regressBSplineSurf(MBaseX, MBaseY, MInX, MInY, MInZ, MOutX, MOutY);

      long r, c;
      for (r = 0; r < szOutX; ++r)
         for (c = 0; c < szOutY; ++c)
            OutZ[r][c] = MOutZ(r, c);

   }
   catch(pdg::Error &e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

PDGLIB_API pdgerr_t pdg_regressBSpline(long szBaseX, double *BaseX,
                                       long szInX, double *InX, long szInY, double *InY,
                                       long szOutX, double *OutX, double *OutY)
{
   try {
      Matrix MBaseX(szBaseX, 1, BaseX);
      Matrix MInX(szInX, 1, InX);
      Matrix MInY(szInY, 1, InY);
      Matrix MOutX(szOutX, 1, OutX);

      Matrix MOutY = interp::regressBSpline(MBaseX, MInX, MInY, MOutX);

      long r;
      for (r = 0; r < szOutX; ++r) OutY[r] = MOutY(r, 0);
   }
   catch(pdg::Error &e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

PDGLIB_API pdgerr_t pdg_interpKruger(double *InX, double *InY, long nIn, 
                                     double *OutX, double *OutY, long nOut)
{
   try {
      cont_interp::kruger(InX, InX + nIn,
                          InY, InY + nIn,
                          OutX, OutX + nOut,
                          OutY);
   }
   catch(pdg::Error &e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}


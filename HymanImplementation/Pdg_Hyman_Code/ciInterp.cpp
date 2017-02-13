//ciInterp.cpp
#include <vector>
#include "ciInterp.h"
#include "cError.h"
#include "cMatrix.h"
#include "eBSplineUtility.h"
#include "eInterp.h"
#include "eContInterp.hpp"

PDGLIB_API pdgerr_t pdg_interpNaturalBSpline(double *InX, double *InY, long nIn,
                                             double *OutX, double *OutY, long nOut)
{
   try {
      if (nIn<=3) throw pdg::Error(2, "Too few input points");

      // knot array
      long allknot_sz = nIn+6;
      std::vector<double> allknot(allknot_sz);
      allknot[2] = allknot[1] = allknot[0] = InX[0];
      long i;
      for(i=0; i<nIn; i++) allknot[i+3] = InX[i];
      allknot[allknot_sz-1] = allknot[allknot_sz-2] = allknot[allknot_sz-3] = InX[nIn-1];

      // evaluate splines on the input points
      long nbasi = nIn + 2;
      long j;
      Matrix b(nbasi, nbasi);
      for(i=0;i<nIn; i++) {
         for(j=0; j<nbasi; j++) {
            b(i+1, j) = bsplineutility::bspline3(InX[i], allknot[j], allknot[j+1], allknot[j+2], allknot[j+3], allknot[j+4]);
         }
      }

      // second derivatives on the boundary
      long peg1, peg2;
      if(nbasi>4){
         peg1=6;
         peg2=nbasi-6;
      }else{
         peg1=nbasi;
         peg2=0;
      }
      for(j=0; j<peg1; j++) {
         b(0, j) = bsplineutility::bspline3D2(InX[0], allknot[j], allknot[j+1], allknot[j+2], allknot[j+3], allknot[j+4]);
      }

      for(j=peg2; j<nbasi; j++) {
         b(nbasi - 1, j) = bsplineutility::bspline3D2(InX[nIn-1], allknot[j], allknot[j+1], allknot[j+2], allknot[j+3], allknot[j+4]);
      }

      // evaluate splines on the output points
      Matrix outb(nOut, nbasi);
      for(i=0;i<nOut; i++) {
         for(j=0; j<nbasi; j++) {
            outb(i, j) = bsplineutility::bspline3(OutX[i], allknot[j], allknot[j+1], allknot[j+2], allknot[j+3], allknot[j+4]);
         }
      }

      Matrix invb = b.inverse();
      Matrix beta(nbasi,1);
      for(i=0;i<nbasi; i++) {
         for(j=0; j<nIn; j++) {
            beta(i,0) += invb(i,j+1) * InY[j];
         }
      }

      Matrix y = outb * beta;

      // first derivatives on the boundary
      double q1=0;
      double q2=0;
      for(j=0; j<peg1; j++) {
         q1 += beta(j,0) * bsplineutility::bspline3D1(InX[0], allknot[j], allknot[j+1], allknot[j+2], allknot[j+3], allknot[j+4]);
      }

      for(j=peg2; j<nbasi; j++) {
         q2 += beta(j,0) * bsplineutility::bspline3D1(InX[nIn-1], allknot[j], allknot[j+1], allknot[j+2], allknot[j+3], allknot[j+4]);
      }

      // assign the out put (if the OutX is outside the range [ InX[0] ; InX[nIn-1] the interpolate linearly)
      for(i=0; i<nOut; i++){
         if(OutX[i]>=InX[0] && OutX[i]<=InX[nIn-1]){
            OutY[i] = y(i,0);
         }else if(OutX[i]<InX[0]){
            OutY[i] = InY[0] + q1*(OutX[i] - InX[0]);
         }else{
            OutY[i] = InY[nIn-1] + q2*(OutX[i] - InX[nIn-1]);
         }
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

PDGLIB_API pdgerr_t pdg_interpMonotonicNaturalBSpline(double *InX, double *InY, long nIn,
   double *OutX, double *OutY, long nOut)
{
   try {
      if (nIn <= 3) throw pdg::Error(2, "Too few input points");

      long nminusone = nIn - 1;
      std::vector<double> dx(nminusone);
      std::vector<double> s(nminusone);

      for (unsigned int i = 0; i < nminusone; ++i) {
         dx[i] = InX[i + 1] - InX[i];
         s[i] = (InY[i + 1] - InY[i]) / dx[i];
      }

      // knot array
      long allknot_sz = nIn + 6;
      std::vector<double> allknot(allknot_sz);
      allknot[2] = allknot[1] = allknot[0] = InX[0];
      long i;
      for (i = 0; i<nIn; i++) allknot[i + 3] = InX[i];
      allknot[allknot_sz - 1] = allknot[allknot_sz - 2] = allknot[allknot_sz - 3] = InX[nIn - 1];

      // evaluate splines on the input points
      long nbasi = nIn + 2;
      long j;
      Matrix b(nbasi, nbasi);
      for (i = 0; i<nIn; i++) {
         for (j = 0; j<nbasi; j++) {
            b(i + 1, j) = bsplineutility::bspline3(InX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
         }
      }

      // second derivatives on the boundary
      long peg1, peg2;
      if (nbasi>4){
         peg1 = 6;
         peg2 = nbasi - 6;
      }
      else{
         peg1 = nbasi;
         peg2 = 0;
      }
      for (j = 0; j<peg1; j++) {
         b(0, j) = bsplineutility::bspline3D2(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
      }

      for (j = peg2; j<nbasi; j++) {
         b(nbasi - 1, j) = bsplineutility::bspline3D2(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
      }

      // evaluate splines on the output points
      Matrix outb(nOut, nbasi);
      for (i = 0; i<nOut; i++) {
         for (j = 0; j<nbasi; j++) {
            outb(i, j) = bsplineutility::bspline3(OutX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
         }
      }

      Matrix invb = b.inverse();
      Matrix beta(nbasi, 1);
      for (i = 0; i<nbasi; i++) {
         for (j = 0; j<nIn; j++) {
            beta(i, 0) += invb(i, j + 1) * InY[j];
         }
      }

      Matrix y = outb * beta;

      // first derivatives on the boundary
      double q1 = 0;
      double q2 = 0;
      for (j = 0; j<peg1; j++) {
         q1 += beta(j, 0) * bsplineutility::bspline3D1(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
      }

      for (j = peg2; j<nbasi; j++) {
         q2 += beta(j, 0) * bsplineutility::bspline3D1(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
      }

      // assign the out put (if the OutX is outside the range [ InX[0] ; InX[nIn-1] the interpolate linearly)
      for (i = 0; i<nOut; i++){
         if (OutX[i] >= InX[0] && OutX[i] <= InX[nIn - 1]){
            OutY[i] = y(i, 0);
         }
         else if (OutX[i]<InX[0]){
            OutY[i] = InY[0] + q1*(OutX[i] - InX[0]);
         }
         else{
            OutY[i] = InY[nIn - 1] + q2*(OutX[i] - InX[nIn - 1]);
         }
      }
      // Hyman monotonicity filter
      double filter;
      double pm, pu, pd, M;
      for (unsigned int i = 0; i<nIn; ++i) {
         if (i == 0) {
            if (OutY[i] * s[0]>0.0) {
               filter = OutY[i] / std::fabs(OutY[i]) *
                  std::min<double>(std::fabs(OutY[i]),
                  std::fabs(3.0*s[0]));
            }
            else {
               filter = 0.0;
            }
            if (filter != OutY[i]) {
               OutY[i] = filter;
            }
         }
         else if (i == nIn - 1) {
            if (OutY[i] * s[nIn - 2]>0.0) {
               filter = OutY[i] / std::fabs(OutY[i]) *
                  std::min<double>(std::fabs(OutY[i]),
                  std::fabs(3.0*s[nIn - 2]));
            }
            else {
               filter = 0.0;
            }
            if (filter != OutY[i]) {
               OutY[i] = filter;
            }
         }
         else {
            pm = (s[i - 1] * dx[i] + s[i] * dx[i - 1]) /
               (dx[i - 1] + dx[i]);
            M = 3.0 * std::min(std::min(std::fabs(s[i - 1]),
               std::fabs(s[i])),
               std::fabs(pm));
            if (i>1) {
               if ((s[i - 1] - s[i - 2])*(s[i] - s[i - 1])>0.0) {
                  pd = (s[i - 1] * (2.0*dx[i - 1] + dx[i - 2])
                     - s[i - 2] * dx[i - 1]) /
                     (dx[i - 2] + dx[i - 1]);
                  if (pm*pd>0.0 && pm*(s[i - 1] - s[i - 2])>0.0) {
                     M = std::max<double>(M, 1.5*std::min(
                        std::fabs(pm), std::fabs(pd)));
                  }
               }
            }
            if (i<nIn - 2) {
               if ((s[i] - s[i - 1])*(s[i + 1] - s[i])>0.0) {
                  pu = (s[i] * (2.0*dx[i] + dx[i + 1]) - s[i + 1] * dx[i]) /
                     (dx[i] + dx[i + 1]);
                  if (pm*pu>0.0 && -pm*(s[i] - s[i - 1])>0.0) {
                     M = std::max<double>(M, 1.5*std::min(
                        std::fabs(pm), std::fabs(pu)));
                  }
               }
            }
            if (OutY[i] * pm>0.0) {
               filter = OutY[i] / std::fabs(OutY[i]) *
                  std::min(std::fabs(OutY[i]), M);
            }
            else {
               filter = 0.0;
            }
            if (filter != OutY[i]) {
               OutY[i] = filter;
            }
         }
      }
   }
   catch (pdg::Error &e) {
      return e.getInfo();
   }
   catch (...) {
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



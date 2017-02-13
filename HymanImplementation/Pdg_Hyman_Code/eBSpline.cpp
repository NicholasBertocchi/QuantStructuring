//eBSpline.cpp
#include "eBSpline.h"
#include "cError.h"
#include "cMatrix.h"
#include "eBSplineUtility.h"
#include "auto_diff.h"

namespace bspline {

using bsplineutility::bspline3;   
using bsplineutility::bspline3D2;   
using bsplineutility::bspline3D1;   
using AD_NAMESPACE::adouble;
   
template<class Real>
std::vector<Real> interpNaturalBSpline(const std::vector<double> &InX, 
                                         const std::vector<Real> &InY,
                                         const std::vector<double> &OutX)
{
   const long nIn = InX.size();
   if (nIn <= 3) throw pdg::Error(2, "Too few points.");
   
   // knot array
   const long allknot_sz = nIn + 6;
   std::vector<double> allknot(allknot_sz);
   allknot[2] = allknot[1] = allknot[0] = InX[0];
   
   long i;
   for(i = 0; i < nIn; ++i) allknot[i + 3] = InX[i];
   allknot[allknot_sz - 1] = allknot[allknot_sz - 2] = allknot[allknot_sz - 3] = InX[nIn - 1];
   
   // evaluate splines on the input points
   const long nbasi = nIn + 2;
   long j;
   Matrix b(nbasi, nbasi);
   for(i = 0; i < nIn; ++i) 
      for(j = 0; j < nbasi; ++j) 
         b(i + 1, j) = bspline3(InX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
   
   // second derivatives on the boundary
   long peg1, peg2;
   if(nbasi > 4) {
      peg1 = 6;
      peg2 = nbasi - 6;
   }
   else {
      peg1 = nbasi;
      peg2 = 0;
   }

   for(j = 0; j < peg1; ++j) 
      b(0, j) = bspline3D2(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
   
   for(j = peg2; j < nbasi; ++j) 
      b(nbasi - 1, j) = bspline3D2(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
   
   // evaluate splines on the output points
   const long nOut = OutX.size();
   Matrix outb(nOut, nbasi);
   for(i = 0; i < nOut; ++i) 
      for(j = 0; j < nbasi; ++j) 
         outb(i, j) = bspline3(OutX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
   
   Matrix invb = b.inverse();
   std::vector<Real> beta(nbasi, 0.); //Matrix beta(nbasi, 1);
   for(i = 0; i < nbasi; ++i) 
      for(j = 0; j < nIn; ++j) 
         beta[i] += invb(i, j + 1) * InY[j]; //beta(i, 0) += invb(i, j + 1) * InY[j];
   
   //Matrix y = outb * beta;
   std::vector<Real> y(nOut, 0.);
   for(i = 0; i < nOut; ++i) 
      for(j = 0; j < nbasi; ++j) 
         y[i] += outb(i, j) * beta[j];
   
   // first derivatives on the boundary
   Real q1 = 0;
   Real q2 = 0;	
   for(j = 0; j < peg1; ++j) 
      q1 += /*beta(j, 0)*/beta[j] * bspline3D1(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
   
   for(j = peg2; j < nbasi; ++j) 
      q2 += /*beta(j, 0)*/beta[j] * bspline3D1(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
   
   // assign the out put (if the OutX is outside the range [ InX[0] ; InX[nIn-1] the interpolate linearly)
   std::vector<Real> OutY(nOut);
   for(i = 0; i < nOut; ++i) {
      if((OutX[i] >= InX[0]) && (OutX[i] <= InX[nIn - 1])) OutY[i] = y[i]; //y(i, 0);
      else if(OutX[i] < InX[0]) OutY[i] = InY[0] + q1 * (OutX[i] - InX[0]);
      else OutY[i] = InY[nIn - 1] + q2 * (OutX[i] - InX[nIn - 1]);
   }
   
   return OutY;
}

//explicit instantiation
template std::vector<double> interpNaturalBSpline<double>(const std::vector<double> &InX, 
                                         const std::vector<double> &InY,
                                         const std::vector<double> &OutX);
template std::vector<adouble> interpNaturalBSpline<adouble>(const std::vector<double> &InX, 
                                         const std::vector<adouble> &InY,
                                         const std::vector<double> &OutX);

template<class Real>
std::vector<Real> interpMonotonicNaturalBSpline(const std::vector<double> &InX,
                                                const std::vector<double> &InY,
                                                const std::vector<double> &OutX)
{
   const long nOut = OutX.size();
   const long nIn = InX.size();
   long nminusone = nIn - 1;
   std::vector<double> OutY(nOut);
   std::vector<double> dx(nminusone);
   std::vector<double> s(nminusone);
   if (nIn <= 3) throw pdg::Error(2, "Too few points.");

   // knot array
   const long allknot_sz = nIn + 6;
   std::vector<double> allknot(allknot_sz);
   allknot[2] = allknot[1] = allknot[0] = InX[0];

   long i;
   for (i = 0; i < nIn; ++i) allknot[i + 3] = InX[i];
   allknot[allknot_sz - 1] = allknot[allknot_sz - 2] = allknot[allknot_sz - 3] = InX[nIn - 1];

   // evaluate splines on the input points
   const long nbasi = nIn + 2;
   long j;
   Matrix b(nbasi, nbasi);
   for (i = 0; i < nIn; ++i)
      for (j = 0; j < nbasi; ++j)
         b(i + 1, j) = bspline3(InX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   // second derivatives on the boundary
   long peg1, peg2;
   if (nbasi > 4) {
      peg1 = 6;
      peg2 = nbasi - 6;
   }
   else {
      peg1 = nbasi;
      peg2 = 0;
   }

   for (j = 0; j < peg1; ++j)
      b(0, j) = bspline3D2(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   for (j = peg2; j < nbasi; ++j)
      b(nbasi - 1, j) = bspline3D2(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   // evaluate splines on the output points
   Matrix outb(nOut, nbasi);
   for (i = 0; i < nOut; ++i)
      for (j = 0; j < nbasi; ++j)
         outb(i, j) = bspline3(OutX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   Matrix invb = b.inverse();
   std::vector<Real> beta(nbasi, 0.); //Matrix beta(nbasi, 1);
   for (i = 0; i < nbasi; ++i)
      for (j = 0; j < nIn; ++j)
         beta[i] += invb(i, j + 1) * InY[j]; //beta(i, 0) += invb(i, j + 1) * InY[j];

   //Matrix y = outb * beta;
   std::vector<Real> y(nOut, 0.);
   for (i = 0; i < nOut; ++i)
      for (j = 0; j < nbasi; ++j)
         y[i] += outb(i, j) * beta[j];

   // first derivatives on the boundary
   Real q1 = 0;
   Real q2 = 0;
   for (j = 0; j < peg1; ++j)
      q1 += /*beta(j, 0)*/beta[j] * bspline3D1(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   for (j = peg2; j < nbasi; ++j)
      q2 += /*beta(j, 0)*/beta[j] * bspline3D1(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   // assign the out put (if the OutX is outside the range [ InX[0] ; InX[nIn-1] the interpolate linearly)
   for (i = 0; i < nOut; ++i) {
      if ((OutX[i] >= InX[0]) && (OutX[i] <= InX[nIn - 1])) OutY[i] = y[i]; //y(i, 0);
      else if (OutX[i] < InX[0]) OutY[i] = InY[0] + q1 * (OutX[i] - InX[0]);
      else OutY[i] = InY[nIn - 1] + q2 * (OutX[i] - InX[nIn - 1]);
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

   return OutY;
}

//explicit instantiation
template std::vector<double> interpMonotonicNaturalBSpline<double>(const std::vector<double> &InX,
                                                                   const std::vector<double> &InY,
                                                                   const std::vector<double> &OutX);
template std::vector<adouble> interpMonotonicNaturalBSpline<adouble>(const std::vector<double> &InX,
                                                                     const std::vector<adouble> &InY,
                                                                     const std::vector<double> &OutX);

}



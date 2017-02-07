#ifndef eInterpolator_h_ALREADY_INCLUDED_AUTO_GUARD
#define eInterpolator_h_ALREADY_INCLUDED_AUTO_GUARD
//eInterpolator.h
#pragma once

#include <vector>
#include "cError.h"
#include "eStatistics.h"
#include "eBSplineUtility.h"
#include "nearEqual.h"
#include "xtos.h"

#include "auto_diff.h"

namespace interp {

//A linear interpolator function
template<class T, class S>
S linearInterp(const T &prevX, const T &x, const T &succX, const S &prevY, const S &succY)
{
   if (pdg::nearEqual(succX,prevX))
   {  //if  prevX = succX assumes a constant structure
      pdg::Assertion(pdg::nearEqual(succY,prevY),"#Error in interp::linearInterp, succX = prevX but succY!=prevY");
      return prevY;
   }
   return prevY + (succY - prevY) * (x - prevX) / (succX - prevX);
}

//A linear interpolator function
template<class T, class S>
std::vector<S> linearMultipleInterp(const std::vector<T> &inX, const std::vector<S> &inY, const std::vector<T> &outX)
{
   pdg::Assertion((inX.size()>0) && (inX.size()==inY.size()),"#Error in interp::linearMultipleInterp, empty input array"); 
   
   std::vector<S> output;
   output.reserve(outX.size());
   typename std::vector<T>::const_iterator pos;
   typename std::vector<T>::const_iterator pred = inX.begin();
   typename std::vector<T>::const_iterator succ = inX.begin();
   
   for (pos = outX.begin(); pos != outX.end(); ++pos) {
      if (inX.size()==1) output.push_back(inY.front());
      else{
         bool gr = false;
         if ((*pos < *pred) || (*pos > *succ)) {
            pred = std::lower_bound(inX.begin(), inX.end(), *pos);
            if (pred == inX.begin()) {
               ++pred;
            }
            else if (pred == inX.end()) {
               --pred;
            }
            succ = pred--;
         }
         if (gr || *pos == *pred) output.push_back(inY[std::distance(inX.begin(), pred)]);
         else if (*pos == *succ) output.push_back(inY[std::distance(inX.begin(), succ)]);
         else output.push_back(linearInterp(*pred, *pos, *succ, inY[std::distance(inX.begin(), pred)], inY[std::distance(inX.begin(), succ)]));
      }
   }

   return output;
}

namespace {
   // quadratic interpolation utility function
   template<class T, class S>
   S poliquadro(const T x, const T x0, const T x1, const T x2, const S y0, const S y1, const S y2)
   {
      return y0 * (x - x1) * (x - x2) / (x0 - x1) / (x0 - x2)
           + y1 * (x - x0) * (x - x2) / (x1 - x0) / (x1 - x2)
           + y2 * (x - x0) * (x - x1) / (x2 - x0) / (x2 - x1);
   }
   
}

//A quadratic interpolator function
template<class T, class S>
S quadraticInterp(const T &x, const T &pX0, const T &pX1, const T &sX0, const T &sX1, const S &pY0, const S &pY1, const S &sY0, const S &sY1)
{
   S left;
   S right;
   double weight;

   if(pX0 == pX1) {
      left = 0;
      right = interp::poliquadro(x, pX1, sX0, sX1, pY1, sY0, sY1);
      weight = 1;
   }
   else if(sX0 == sX1) {
      left = interp::poliquadro(x, pX0, pX1, sX0, pY0, pY1, sY0);
      right = 0;
      weight = 0;
   }
   else {
      left = interp::poliquadro(x, pX0, pX1, sX0, pY0, pY1, sY0);
      right = interp::poliquadro(x, pX1, sX0, sX1, pY1, sY0, sY1);

      double frac = (x - pX1) / (sX0 - pX1);
      weight = sin(frac * statistics::GreekPI / 2);
       weight *= weight;
   }

   return ((1.0 - weight) * left) + (weight * right);
}

//An (approximate) quadratic interpolator function
template<class T>
T fastQuadraticInterpolator(const T &prevX, const T &x, const T &succX, const T &prevY, const T &succ1Y, const T &succ2Y)
{
   T theta = (x - prevX) / (succX - prevX);
   T diff1 = succ1Y - prevY;
   T diff2 = succ2Y - succ1Y;

   T quadDiff = diff2 - diff1;

   return prevY + (theta * diff1) + (0.5 * theta * (theta - 1) * quadDiff);
}

template<class T, class S>
std::vector<S> interpNaturalBSpline(const std::vector<T> &InX,
                                    const std::vector<S> &InY,
                                    const std::vector<T> &OutX)
{
   if(OutX.empty()) return std::vector<S>();  
   //to avoid problems with nonsensical linear systems... 

   if(InX.size() <= 3) 
	{
		return linearMultipleInterp(InX, InY, OutX);
	}

   long nIn = InX.size();
   long nOut = OutX.size();
	std::vector<S> OutY(nOut);

   // knot array
   long allknot_sz = nIn + 6;
   std::vector<double> allknot(allknot_sz);
   allknot[2] = allknot[1] = allknot[0] = InX[0];
   long i;
   for(i = 0; i < nIn; ++i)
      allknot[i + 3] = InX[i];

   allknot[allknot_sz - 1] = allknot[allknot_sz - 2] = allknot[allknot_sz - 3] = InX[nIn - 1];

   // evaluate splines on the input points
   long nbasi = nIn + 2;
   long j;
   Matrix b(nbasi, nbasi);
   for(i = 0; i < nIn; ++i)
      for(j=0; j<nbasi; ++j)
         b(i + 1, j) = bsplineutility::bspline3(InX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

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

   for(j = 0; j < peg1; ++j)
      b(0, j) = bsplineutility::bspline3D2(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   for(j = peg2; j < nbasi; ++j)
      b(nbasi - 1, j) = bsplineutility::bspline3D2(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   // evaluate splines on the output points
   Matrix outb(nOut, nbasi);
   for(i = 0; i < nOut; ++i)
      for(j = 0; j < nbasi; ++j)
         outb(i, j) = bsplineutility::bspline3(OutX[i], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   Matrix invb = b.inverse();
   
   //(Daluiso) do not want to put adoubles into Matrix yet: rewritten below
   //Matrix beta(nbasi, 1);
   //for(i = 0;i < nbasi; ++i)
   //   for(j = 0; j < nIn; ++j)
   //      beta(i, 0) += invb(i, j + 1) * InY[j];

   std::vector<S> beta(nbasi);
   for(i = 0; i < nbasi; ++i)
     for(j = 0; j < nIn; ++j)
       beta[i] += invb(i, j + 1) * InY[j];

   //(Daluiso) Idem
   //Matrix y = outb * beta;
   std::vector<S> y(nOut);
   for(i = 0; i < nOut; ++i)
     for(j = 0; j < nbasi; ++j)
       y[i] += outb(i, j) * beta[j];

   // first derivatives on the boundary
   S q1 = 0;
   S q2 = 0;
   for(j = 0; j < peg1; ++j)
      //q1 += beta(j, 0) * bsplineutility::bspline3D1(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
      q1 += beta[j] * bsplineutility::bspline3D1(InX[0], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   for(j = peg2; j < nbasi; ++j)
      //q2 += beta(j, 0) * bsplineutility::bspline3D1(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);
      q2 += beta[j] * bsplineutility::bspline3D1(InX[nIn - 1], allknot[j], allknot[j + 1], allknot[j + 2], allknot[j + 3], allknot[j + 4]);

   // assign the output (if the OutX is outside the range [ InX[0] ; InX[nIn-1] the interpolate linearly)
   for(i = 0; i < nOut; ++i)
      if(OutX[i]>=InX[0] && OutX[i]<=InX[nIn-1])
         OutY[i] = y[i];   //OutY[i] = y(i,0);
      else if(OutX[i]<InX[0])
         OutY[i] = InY[0] + q1*(OutX[i] - InX[0]);
      else
         OutY[i] = InY[nIn-1] + q2*(OutX[i] - InX[nIn-1]);

   return OutY;
}

template<class T, class S>
std::vector<S> cubicKrugerInterp(const std::vector<T> &InX,
                                 const std::vector<S> &InY,
                                 const std::vector<T> &OutX)
{
   if(InX.size() <= 3) 
	{
		return linearMultipleInterp(InX, InY, OutX);
	}
		
   unsigned int n = InX.size();
	unsigned int nminusone = n-1;
   std::vector<S> tmp(n);
   std::vector<T> dx(nminusone);
   std::vector<S> s(nminusone);
   
   for(unsigned int i = 0; i < nminusone; ++i) {
      dx[i] = InX[i + 1] - InX[i];
      s[i] = (InY[i + 1] - InY[i]) / dx[i];
   }

   // intermediate points
   for (unsigned int i = 1; i <nminusone; ++i) {
       if (AD_NAMESPACE::cast<double>(s[i - 1]) * AD_NAMESPACE::cast<double>(s[i]) <= 0.0)
         // slope changes sign at point
         tmp[i] = 0.0;
      else
         // slope will be between the slopes of the adjacent
         // straight lines and should approach zero if the
         // slope of either line approaches zero
         tmp[i] = 2.0 / (1.0 / s[i - 1] + 1.0 / s[i]);
   }
   // end points
   tmp[0] = (3.0 * s[0] - tmp[1]) / 2.0;
   tmp[nminusone] = (3.0 * s[n - 2] - tmp[n - 2]) / 2.0;

   // cubic coefficients
   std::vector<S> a(nminusone), b(nminusone), c(nminusone);
   for(unsigned int i = 0; i < nminusone; ++i) {
      a[i] = tmp[i];
      b[i] = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      c[i] = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }

   std::vector<S> OutY(OutX.size());
   for(unsigned int i = 0; i < OutX.size(); ++i) {
      unsigned int j;
      if(OutX[i] < InX.front())
         j = 0;
      else if(OutX[i] > InX.back())
         j = InX.size() - 2;
      else
         j = std::upper_bound(InX.begin(), InX.end()-1, OutX[i]) - InX.begin() - 1;

      
      double dx = OutX[i] - InX[j];
      OutY[i] = InY[j] + dx * (a[j] + dx * (b[j] + dx * c[j]));
   }
   
   return OutY;
}

void cubicKrugerInterpPre(
   const std::vector<double> &InX,
   const std::vector<double> &InY,
   std::vector<double> &a,
   std::vector<double> &b,
   std::vector<double> &c
);

std::vector<double> cubicInterpPost(
   const std::vector<double> &a,
   const std::vector<double> &b,
   const std::vector<double> &c,
   const std::vector<double> &inX,
   const std::vector<double> &inY,
   const std::vector<double> &outX
);

template<class T, class S>
std::vector<S> MonotoneCubicSpline(const std::vector<T> &InX,
                                   const std::vector<S> &InY,
                                   const std::vector<T> &OutX)
{
   if(InX.size() <= 3) 
	{
		return linearMultipleInterp(InX, InY, OutX);
	}
	
   unsigned int n = InX.size();
   unsigned int nminusone = n-1;
   std::vector<S> tmp(n);
   std::vector<T> dx(nminusone);
   std::vector<S> s(nminusone);
   
   for(unsigned int i = 0; i < nminusone; ++i) {
      dx[i] = InX[i + 1] - InX[i];
      s[i] = (InY[i + 1] - InY[i]) / dx[i];
   }

   // intermediate points
   for (unsigned int i = 1; i <nminusone; ++i) {
       if (AD_NAMESPACE::cast<double>(s[i - 1]) * AD_NAMESPACE::cast<double>(s[i]) <= 0.0)
         // slope changes sign at point
         tmp[i] = 0.0;
      else
         // slope will be between the slopes of the adjacent
         // straight lines and should approach zero if the
         // slope of either line approaches zero
         tmp[i] = 2.0 / (1.0 / s[i - 1] + 1.0 / s[i]);
   }
   // end points
   tmp[0] = (3.0 * s[0] - tmp[1]) / 2.0;
   tmp[nminusone] = (3.0 * s[n - 2] - tmp[n - 2]) / 2.0;

   // Hyman monotonicity filter
      double filter;
      double pm, pu, pd, M;
      for (unsigned int i = 0; i<n; ++i) {
         if (i == 0) {
            if (tmp[i] * s[0]>0.0) {
               filter = tmp[i] / std::fabs(tmp[i]) *
                  std::min<double>(std::fabs(tmp[i]),
                  std::fabs(3.0*s[0]));
            }
            else {
               filter = 0.0;
            }
            if (filter != tmp[i]) {
               tmp[i] = filter;
            }
         }
         else if (i == n - 1) {
            if (tmp[i] * s[n - 2]>0.0) {
               filter = tmp[i] / std::fabs(tmp[i]) *
                  std::min<double>(std::fabs(tmp[i]),
                  std::fabs(3.0*s[n - 2]));
            }
            else {
               filter = 0.0;
            }
            if (filter != tmp[i]) {
               tmp[i] = filter;
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
            if (i<n - 2) {
               if ((s[i] - s[i - 1])*(s[i + 1] - s[i])>0.0) {
                  pu = (s[i] * (2.0*dx[i] + dx[i + 1]) - s[i + 1] * dx[i]) /
                     (dx[i] + dx[i + 1]);
                  if (pm*pu>0.0 && -pm*(s[i] - s[i - 1])>0.0) {
                     M = std::max<double>(M, 1.5*std::min(
                        std::fabs(pm), std::fabs(pu)));
                  }
               }
            }
            if (tmp[i] * pm>0.0) {
               filter = tmp[i] / std::fabs(tmp[i]) *
                  std::min(std::fabs(tmp[i]), M);
            }
            else {
               filter = 0.0;
            }
            if (filter != tmp[i]) {
               tmp[i] = filter;
            }
         }
      }
   

   // cubic coefficients
   std::vector<S> a(nminusone), b(nminusone), c(nminusone);
   for(unsigned int i = 0; i < nminusone; ++i) {
      a[i] = tmp[i];
      b[i] = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      c[i] = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }

   std::vector<S> OutY(OutX.size());
   for(unsigned int i = 0; i < OutX.size(); ++i) {
      unsigned int j;
      if(OutX[i] < InX.front())
         j = 0;
      else if(OutX[i] > InX.back())
         j = InX.size() - 2;
      else
         j = std::upper_bound(InX.begin(), InX.end()-1, OutX[i]) - InX.begin() - 1;

      
      double dx = OutX[i] - InX[j];
      OutY[i] = InY[j] + dx * (a[j] + dx * (b[j] + dx * c[j]));
   }
 
      return OutY;

}

void MonotoneCubicSplineInterpPre(
   const std::vector<double> &InX,
   const std::vector<double> &InY,
   std::vector<double> &a,
   std::vector<double> &b,
   std::vector<double> &c
);

std::vector<double> MonotoneCubicSplineInterpPost(
   const std::vector<double> &a,
   const std::vector<double> &b,
   const std::vector<double> &c,
   const std::vector<double> &inX,
   const std::vector<double> &inY,
   const std::vector<double> &outX
);
   
}


#endif // eInterpolator_h_ALREADY_INCLUDED_AUTO_GUARD

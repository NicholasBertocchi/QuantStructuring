//eInterpolator.cpp
#include <algorithm>
#include "eInterpolator.h"

void interp::cubicKrugerInterpPre(const std::vector<double> &inX,
                                  const std::vector<double> &inY,
                                  std::vector<double> &a,
                                  std::vector<double> &b,
                                  std::vector<double> &c)
{
   if(inX.size() <= 3) throw pdg::Error(2, "Too few input points");
   
   unsigned int n = inX.size();
   unsigned int nminusone = n-1;
   std::vector<double> tmp(n);
   std::vector<double> dx(nminusone), s(nminusone);
   
   for(unsigned int i = 0; i < nminusone; ++i) {
      dx[i] = inX[i + 1] - inX[i];
      s[i] = (inY[i + 1] - inY[i]) / dx[i];
   }

   // intermediate points
   for (unsigned int i = 1; i < nminusone; ++i) {
      if (s[i - 1] * s[i] < 0.0)
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
   a.resize(nminusone);
   b.resize(nminusone);
   c.resize(nminusone);
   for(unsigned int i = 0; i <nminusone; ++i) {
      a[i] = tmp[i];
      b[i] = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      c[i] = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }

}

void interp::MonotoneCubicSplineInterpPre(const std::vector<double> &inX,
                                          const std::vector<double> &inY,
                                          std::vector<double> &a,
                                          std::vector<double> &b,
                                          std::vector<double> &c)
{
   if(inX.size() <= 3) throw pdg::Error(2, "Too few input points");
   
   unsigned int n = inX.size();
   unsigned int nminusone = n-1;
   std::vector<double> tmp(n);
   std::vector<double> dx(nminusone), s(nminusone);
   
   for(unsigned int i = 0; i < nminusone; ++i) {
      dx[i] = inX[i + 1] - inX[i];
      s[i] = (inY[i + 1] - inY[i]) / dx[i];
   }

   // intermediate points
   for (unsigned int i = 1; i < nminusone; ++i) {
      if (s[i - 1] * s[i] < 0.0)
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
   a.resize(nminusone);
   b.resize(nminusone);
   c.resize(nminusone);
   for(unsigned int i = 0; i <nminusone; ++i) {
      a[i] = tmp[i];
      b[i] = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      c[i] = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }

}

std::vector<double> interp::cubicInterpPost(const std::vector<double> &a,
                                            const std::vector<double> &b,
                                            const std::vector<double> &c,
                                            const std::vector<double> &inX,
                                            const std::vector<double> &inY,
                                            const std::vector<double> &outX)
{
   std::vector<double> outY(outX.size());
   for(unsigned int i = 0; i < outX.size(); ++i) {
      unsigned int j;
      if(outX[i] < inX.front())
         j = 0;
      else if(outX[i] > inX.back())
         j = inX.size() - 2;
      else
         j = std::upper_bound(inX.begin(), inX.end()-1, outX[i]) - inX.begin() - 1;

      
      double dx = outX[i] - inX[j];
      outY[i] = inY[j] + dx * (a[j] + dx * (b[j] + dx * c[j]));
   }
   
   return outY;
}


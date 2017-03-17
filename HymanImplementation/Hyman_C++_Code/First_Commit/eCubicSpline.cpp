//eCubicSpline.cpp
#include "eCubicSpline.h"
#include "cError.h"
#include "auto_diff.h"
#include <vector>
#include <math.h>
#include <list>
#include <algorithm>

enum BoundaryCondition {

      notaknot, // fixes f'''(x_0) = f'''(x_1) and f'''(x_N) = f'''(x_N-1)
      firstderivative, // natural 1: match value of end-slope
      secondderivative, // natural 2: match value of second derivative at end
      clamped // fixes f'(x_0) = a and f'(x_N) = b where a and b are constant values

      };

enum Extrapolator {
      
      // if OutX is not in range (InX[0] ; InX[nIn-1]):
      error, // throw error
      flat, // extrapolate flat
      linear, // extrapolate linear
      extrap // extrapolate cubic

      };

namespace cubicspline {
  
using AD_NAMESPACE::adouble;
   
template<class Real>
std::vector<Real> interpCubicSpline(const std::vector<double>& InX, 
                                    const std::vector<Real>& InY,
                                    const std::vector<double>& OutX,
                                    long boundaryType_, 
                                    long Extrapolator_,
                                    bool HymanFilter)

{
      if(OutX.empty()) return std::vector<Real>(); 
      const size_t nIn = InX.size();

      if (nIn <= 3) {
         throw pdg::Error(2, "Too few input points");
      }

      for (size_t i = 0; i + 1 < nIn; ++i) {
          if (InX[i+1] < InX[i]) throw pdg::Error(2, "Unordered sequence");
      }

      BoundaryCondition boundaryType = static_cast<BoundaryCondition>(boundaryType_);
      Extrapolator extrapType = static_cast<Extrapolator>(Extrapolator_);
      std::vector<Real> diagonal(nIn);
      std::vector<Real> lowerdiagonal(nIn-1);
      std::vector<Real> upperdiagonal(nIn-1);
      std::vector<Real> tmp(nIn);
      std::vector<double> dx(nIn-1);
      std::vector<Real> s(nIn-1);

      // Building tridiagonal system
      for (size_t i = 0; i + 1 < nIn; ++i) {
         dx[i] = InX[i+1] - InX[i];
         s[i] = (InY[i+1] - InY[i]) / dx[i];
      }

      for (size_t i = 0; i < nIn; ++i) {
         if (i == 0) {
            switch (boundaryType) {
               case notaknot:
                  diagonal[i] = dx[1]*(dx[1] + dx[0]);
                  upperdiagonal[i] = (dx[0] + dx[1])*(dx[0] + dx[1]);
                  tmp[0] = s[0]*dx[1]*(2.0*dx[1] + 3.0*dx[0]) + s[1]*dx[0]*dx[0];
                  break;
               case firstderivative:
                  diagonal[i] = 1;
                  upperdiagonal[i] = 0;
                  tmp[0] = 0;
                  break;
               case secondderivative:
                  diagonal[i] = 2;
                  upperdiagonal[i] = 1;
                  tmp[0] = 3.0 * s[0];
                  break;
               case clamped:
                  diagonal[i] = 2 * dx[0];
                  upperdiagonal[i] = dx[0];
                  tmp[0] = 0;
                  break;
               default:
                  throw pdg::Error(2, "unknown boundary condition");
            }
         } else if (i == nIn-1) {
            switch (boundaryType) {
               case notaknot:
                  diagonal[i] = -dx[nIn-3]*(dx[nIn-3] + dx[nIn-2]);
                  lowerdiagonal[i-1] = -(dx[nIn-2] + dx[nIn-3])*(dx[nIn-2] + dx[nIn-3]);
                  tmp[nIn-1] = -s[nIn-3]*dx[nIn-2]*dx[nIn-2] - s[nIn-2]*dx[nIn-3]*(3.0*dx[nIn-2] + 2.0*dx[nIn-3]);
                  break;
               case firstderivative:
                  diagonal[i] = 1;
                  lowerdiagonal[i-1] = 0;
                  tmp[nIn-1] = 0;
                  break;
               case secondderivative:
                  diagonal[i] = 2;
                  lowerdiagonal[i-1] = 1;
                  tmp[nIn-1] = 3.0*s[nIn-2];
                  break;
               case clamped:
                  diagonal[i] = 2*dx[nIn-2];
                  lowerdiagonal[i-1] = dx[nIn-2];
                  tmp[nIn-1] = 0;
                  break;
               default:
                  throw pdg::Error(2, "unknown boundary condition");
            }
         } else {
         diagonal[i] = 2*(dx[i] + dx[i-1]);
         lowerdiagonal[i-1] = dx[i];
         upperdiagonal[i] = dx[i-1];
         tmp[i] = 3.0*(dx[i]*s[i-1] + dx[i-1]*s[i]);
         }
      }

      // Solving tridiagonal system
      std::vector<Real> solv(nIn);
      Real start = diagonal[0];
      tmp[0] = tmp[0] / start;
      for (size_t j = 1; j + 1 <= nIn; ++j) {
         solv[j] = upperdiagonal[j-1] / start;
         start = diagonal[j] - lowerdiagonal[j-1]*solv[j];
         tmp[j] = (tmp[j] - lowerdiagonal[j-1]*tmp[j-1]) / start;
      }
      for (size_t j = nIn-2; j > 0; --j)
            tmp[j] -= solv[j+1]*tmp[j+1];

      tmp[0] -= solv[1]*tmp[1];

      // Hyman monotonicity constrained filter
      Real filter;
      Real pm, pu, pd, M;
      if (HymanFilter) {
         for (size_t i = 0; i < nIn; ++i) {
            if (i == 0) {
                  if (tmp[i]*s[0] > 0.0) {
                     filter = tmp[i] / fabs(tmp[i])*std::min<Real>(fabs(tmp[i]),fabs(3.0*s[0]));
                  } else {
                     filter = 0.0;
                  }
                  if (filter != tmp[i]) {
                     tmp[i] = filter;
                  }
            } else if (i == nIn-1) {
                  if (tmp[i] * s[nIn-2] > 0.0) {
                     filter = tmp[i] / fabs(tmp[i])*std::min<Real>(fabs(tmp[i]), fabs(3.0*s[nIn-2]));
                  } else {
                     filter = 0.0;
                  }
                  if (filter != tmp[i]) {
                     tmp[i] = filter;
                  }
            } else {
                  pm = (s[i-1]*dx[i] + s[i]*dx[i-1]) / (dx[i-1] + dx[i]);
                  M = 3.0*std::min(std::min(fabs(s[i-1]),fabs(s[i])),fabs(pm));
                  if (i > 1) {
                     if ((s[i-1] - s[i-2])*(s[i] - s[i-1]) > 0.0) {
                        pd = (s[i-1]*(2.0*dx[i-1] + dx[i-2]) - s[i-2]*dx[i-1]) / (dx[i-2] + dx[i-1]);
                        if (pm*pd > 0.0 && pm*(s[i-1] - s[i-2]) > 0.0) {
                              M = std::max<Real>(M, 1.5*std::min(fabs(pm),fabs(pd)));
                        }
                     }
                  }
                  if (i < nIn-2) {
                     if ((s[i] - s[i-1])*(s[i+1] - s[i]) > 0.0) {
                        pu = (s[i]*(2.0*dx[i] + dx[i+1]) - s[i+1]*dx[i]) / (dx[i] + dx[i+1]);
                        if (pm*pu > 0.0 && -pm*(s[i] - s[i-1]) > 0.0) {
                              M = std::max<Real>(M, 1.5*std::min(fabs(pm),fabs(pu)));
                        }
                     }
                  }
                  if (tmp[i]*pm > 0.0) {
                     filter = tmp[i] / fabs(tmp[i])*std::min<Real>(fabs(tmp[i]), M);
                  } else {
                     filter = 0.0;
                  }
                  if (filter != tmp[i]) {
                     tmp[i] = filter;
                  }
            }
         }
      }

      // Cubic coefficients
      unsigned int nOut = OutX.size();
      unsigned int locator;
      std::vector<Real> a(nIn-1);
      std::vector<Real> b(nIn-1);
      std::vector<Real> c(nIn-1);

      for (size_t i = 0; i + 1 < nIn; ++i) {
         a[i] = tmp[i];
         b[i] = (3.0*s[i] - tmp[i+1] - 2.0*tmp[i]) / dx[i];
         c[i] = (tmp[i+1] + tmp[i] - 2.0*s[i]) / (dx[i]*dx[i]);
      }

      // Find output point location
      if (OutX[0] < InX[0]) {
         locator = 0;
      } else if (OutX[0] > InX[nIn-1]) {
         locator = nIn-1;
      } else {
           for (size_t i = 1; i < nIn; ++i) {
              if (OutX[0] < InX[i]) {
                 locator = i-1;
                 break;
              }
           }
      }

      // Assign output
      std::vector<Real> OutY(nOut);
      double dX = OutX[0] - InX[locator];    
      for (size_t i = 0; i < nOut; ++i) {
         if (OutX[i] < InX[0]) {
            switch (extrapType) {
               case error:
                  throw pdg::Error(2, "Output point before first pillar");
                  break;
               case flat:
                  OutY[i] = InY[0];
                  break;
               case linear:
                  OutY[i] = InY[0] + s[0]*(OutX[i] - InX[0]);
                  break;
               case extrap:
                  throw pdg::Error(2, "Not implemented yet");
                  break;
               default:
                  throw pdg::Error(2, "Unknown Extrapolation");
            }
         } else if (OutX[i] >= InX[0] && OutX[i] <= InX[nIn-1]) {
            OutY[i] = InY[locator] + dX*
                           (a[locator] + dX* 
                              (b[locator] + dX* 
                                 c[locator]));
         } else {
            switch (extrapType) {
               case error:
                  throw pdg::Error(2, "Output point before last pillar");
                  break;
               case flat:
                  OutY[i] = InY[nIn-1];
                  break;
               case linear:
                  OutY[i] = InY[nIn-1] + s[nIn-2]*(OutX[i] - InX[nIn-1]);
                  break;
               case extrap:
                  throw pdg::Error(2, "Not implemented yet");
                  break;
               default:
                  throw pdg::Error(2, "Unknown Extrapolation");
            }
         }
      }

      return OutY;

}

//explicit instantiation
template std::vector<double> interpCubicSpline<double>(const std::vector<double> &InX, 
                                                       const std::vector<double> &InY,
                                                       const std::vector<double> &OutX,
                                                       long boundaryType_, 
                                                       long Extrapolator_,
                                                       bool HymanFilter);

template std::vector<adouble> interpCubicSpline<adouble>(const std::vector<double> &InX, 
                                                         const std::vector<adouble> &InY,
                                                         const std::vector<double> &OutX,
                                                         long boundaryType_, 
                                                         long Extrapolator_,
                                                         bool HymanFilter);

}
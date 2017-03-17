//eBSpline.cpp
#include "eBSpline.h"
#include "cError.h"
#include "cMatrix.h"
#include "eInterpolator.h"
#include "auto_diff.h"

namespace bspline {
  
using AD_NAMESPACE::adouble;
   
template<class Real>
std::vector<Real> interpNaturalBSpline(const std::vector<double> &InX, 
                                       const std::vector<Real> &InY,
                                       const std::vector<double> &OutX)
{
   return interp::interpNaturalBSpline<double,Real>(InX, InY, OutX);
}

//explicit instantiation
template std::vector<double> interpNaturalBSpline<double>(const std::vector<double> &InX, 
                                                          const std::vector<double> &InY,
                                                          const std::vector<double> &OutX);

template std::vector<adouble> interpNaturalBSpline<adouble>(const std::vector<double> &InX, 
                                                            const std::vector<adouble> &InY,
                                                            const std::vector<double> &OutX);
}


//eCubicSpline.h
#ifndef _ECUBICSPLINE_H__
#define _ECUBICSPLINE_H__

#include <vector>

namespace cubicspline {


/**
* @defgroup Cubic Spline interpolation functions. 
*/

//@{
template<class Real>
std::vector<Real> interpCubicSpline(const std::vector<double> &InX, 
                                    const std::vector<Real> &InY,  
                                    const std::vector<double> &OutX,
                                    long boundaryType,
                                    long Extrapolator,
                                    bool HymanFilter);

};

#endif // _ECUBICSPLINE_H__
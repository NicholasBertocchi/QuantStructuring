//eBSpline.h
#ifndef _EBSPLINE_H__
#define _EBSPLINE_H__

#include <vector>

namespace bspline {

/**
* @defgroup bsplineinterpfun BSpline interpolation functions. 
*/

//@{
template<class Real>
std::vector<Real> interpNaturalBSpline(const std::vector<double> &InX, 
                                       const std::vector<Real> &InY,  
                                       const std::vector<double> &OutX);
										 
template<class Real>
std::vector<Real> interpMonotonicNaturalBSpline(const std::vector<double> &InX, 
                                                const std::vector<Real> &InY,  
                                                const std::vector<double> &OutX);

/*double interpLinearSurf(const std::vector<double> &xi, 
                        const std::vector<double> &yi, 
                        double **zi, double xo, double yo, double *pVal);*/                                                                  
//@}

};

#endif // _EBSPLINE_H__


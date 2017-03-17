//cTSCubicSplineInterpolator.cpp
#include "cTSCubicSplineInterpolator.h"
#include "eCubicSpline.h"
#include "eBSpline.h"
#include "eInterpolator.h"
#include "eUtility.h"

/*natural-basis-spline*/
template<class Real>
void aTSCubicBSplineInterpolator<Real>::completeInterp(const termstructure_type &ts, 
                                                       termstructure_type &ets) const
{
   long size = ts.size();
   std::vector<double> InX(size);
   std::vector<Real> InY(size);

   typename termstructure_type::const_iterator pos;
   long i;
   for (pos = ts.begin(), i = 0; pos != ts.end(); ++pos, ++i) {
      InX[i] = pos->second->getTime();
      InY[i] = pos->second->getValue();
   }

   size = ets.size();
   std::vector<double> OutX(size);
   for (pos = ets.begin(), i = 0; pos != ets.end(); ++pos, ++i)
      OutX[i] = pos->first.getExcelDate();

   std::vector<Real> OutY = bspline::interpNaturalBSpline(InX, InY, OutX);

   for (pos = ets.begin(), i = 0; pos != ets.end(); ++pos, ++i)
      pos->second->putValue(OutY[i]);
}

template<class Real>
Real aTSCubicBSplineInterpolator<Real>::interpValue(const termstructure_type &ts, 
                                                    aValue<Real> *value) const
{
   long size = ts.size();
   std::vector<double> InX(size);
   std::vector<Real> InY(size);
   std::vector<double> OutX(1);

   typename termstructure_type::const_iterator pos;
   long i;
   for (pos = ts.begin(), i = 0; pos != ts.end(); ++pos, ++i) {
      InX[i] = pos->second->getTime();
      InY[i] = pos->second->getValue();
   }
   OutX[0] = value->getTime();

   return bspline::interpNaturalBSpline(InX, InY, OutX)[0];
}

/*natural-cubic-spline*/
template<class Real>
void aTSCubicNSplineInterpolator<Real>::completeInterp(const termstructure_type &ts, 
                                                       termstructure_type &ets) const
{
   long size = ts.size();
   std::vector<double> InX(size);
   std::vector<Real> InY(size);

   typename termstructure_type::const_iterator pos;
   long i;
   for (pos = ts.begin(), i = 0; pos != ts.end(); ++pos, ++i) {
      InX[i] = pos->second->getTime();
      InY[i] = pos->second->getValue();
   }

   size = ets.size();
   std::vector<double> OutX(size);
   for (pos = ets.begin(), i = 0; pos != ets.end(); ++pos, ++i)
      OutX[i] = pos->first.getExcelDate();

   const long BoundaryCondition = 2; // second derivative natural spline
   const long Extrapolator = 0; // throw error if OutX is out of range: (InX[0] ; InX[nIn-1])
   bool ApplyHyman = false;

   std::vector<Real> OutY = cubicspline::interpCubicSpline(InX, InY, OutX, 
                                                           BoundaryCondition, Extrapolator, ApplyHyman);

   for (pos = ets.begin(), i = 0; pos != ets.end(); ++pos, ++i)
      pos->second->putValue(OutY[i]);
}

template<class Real>
Real aTSCubicNSplineInterpolator<Real>::interpValue(const termstructure_type &ts, 
                                                    aValue<Real> *value) const
{
   long size = ts.size();
   std::vector<double> InX(size);
   std::vector<Real> InY(size);
   std::vector<double> OutX(1);

   typename termstructure_type::const_iterator pos;
   long i;
   for (pos = ts.begin(), i = 0; pos != ts.end(); ++pos, ++i) {
      InX[i] = pos->second->getTime();
      InY[i] = pos->second->getValue();
   }
   OutX[0] = value->getTime();

   const long BoundaryCondition = 2; // second derivative natural spline
   const long Extrapolator = 0; // throw error if OutX is out of range: (InX[0] ; InX[nIn-1])
   bool ApplyHyman = false;

   return cubicspline::interpCubicSpline(InX, InY, OutX,
                                         BoundaryCondition, Extrapolator, ApplyHyman)[0];
}

/*natural-hyman-cubic-spline*/
template<class Real>
void aTSCubicNHSplineInterpolator<Real>::completeInterp(const termstructure_type &ts, 
                                                        termstructure_type &ets) const
{
   long size = ts.size();
   std::vector<double> InX(size);
   std::vector<Real> InY(size);

   typename termstructure_type::const_iterator pos;
   long i;
   for (pos = ts.begin(), i = 0; pos != ts.end(); ++pos, ++i) {
      InX[i] = pos->second->getTime();
      InY[i] = pos->second->getValue();
   }

   size = ets.size();
   std::vector<double> OutX(size);
   for (pos = ets.begin(), i = 0; pos != ets.end(); ++pos, ++i)
      OutX[i] = pos->first.getExcelDate();

   const long BoundaryCondition = 2; // second derivative natural spline
   const long Extrapolator = 0; // throw error if OutX is out of range: (InX[0] ; InX[nIn-1])
   bool ApplyHyman = true;

   std::vector<Real> OutY = cubicspline::interpCubicSpline(InX, InY, OutX, 
                                                           BoundaryCondition, Extrapolator, ApplyHyman);

   for (pos = ets.begin(), i = 0; pos != ets.end(); ++pos, ++i)
      pos->second->putValue(OutY[i]);
}

template<class Real>
Real aTSCubicNHSplineInterpolator<Real>::interpValue(const termstructure_type &ts, 
                                                     aValue<Real> *value) const
{
   long size = ts.size();
   std::vector<double> InX(size);
   std::vector<Real> InY(size);
   std::vector<double> OutX(1);

   typename termstructure_type::const_iterator pos;
   long i;
   for (pos = ts.begin(), i = 0; pos != ts.end(); ++pos, ++i) {
      InX[i] = pos->second->getTime();
      InY[i] = pos->second->getValue();
   }
   OutX[0] = value->getTime();

   const long BoundaryCondition = 2; // second derivative natural spline
   const long Extrapolator = 0; // throw error if OutX is out of range: (InX[0] ; InX[nIn-1])
   bool ApplyHyman = true;

   return cubicspline::interpCubicSpline(InX, InY, OutX,
                                         BoundaryCondition, Extrapolator, ApplyHyman)[0];
}

//explicit instantiation
template class aTSCubicBSplineInterpolator<double>;
template class aTSCubicBSplineInterpolator<AD_NAMESPACE::adouble>;

template class aTSCubicNSplineInterpolator<double>;
template class aTSCubicNSplineInterpolator<AD_NAMESPACE::adouble>;

template class aTSCubicNHSplineInterpolator<double>;
template class aTSCubicNHSplineInterpolator<AD_NAMESPACE::adouble>;
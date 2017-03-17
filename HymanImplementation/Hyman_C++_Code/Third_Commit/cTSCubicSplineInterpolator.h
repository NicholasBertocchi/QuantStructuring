//cTSCubicSplineInterpolator.h
#ifndef __CTSCUBICSPLINEINTERPOLATOR_H
#define __CTSCUBICSPLINEINTERPOLATOR_H

#include "cTSInterpolator.h"
#include "cValue.h"
#include "hTypes.h"
#include "auto_diff.h"


/*natural-basis-spline*/
template<class Real>
class aTSCubicBSplineInterpolator : public FinancialObject,
                                    public aTSInterpolator<Real>
{
public:
   typedef typename aTSInterpolator<Real>::termstructure_type termstructure_type;
   void completeInterp(const termstructure_type &ts, termstructure_type &ets) const;
   Real interpValue(const termstructure_type &ts, aValue<Real> *value) const;
};

typedef aTSCubicBSplineInterpolator<double> TSCubicBSplineInterpolator;


/*natural-cubic-spline*/
template<class Real>
class aTSCubicNSplineInterpolator : public FinancialObject,
                                    public aTSInterpolator<Real>
{
public:
   typedef typename aTSInterpolator<Real>::termstructure_type termstructure_type;
   void completeInterp(const termstructure_type &ts, termstructure_type &ets) const;
   Real interpValue(const termstructure_type &ts, aValue<Real> *value) const;
};

typedef aTSCubicNSplineInterpolator<double> TSCubicNSplineInterpolator;


/*natural-hyman-cubic-spline*/
template<class Real>
class aTSCubicNHSplineInterpolator : public FinancialObject,
                                     public aTSInterpolator<Real>
{
public:
   typedef typename aTSInterpolator<Real>::termstructure_type termstructure_type;
   void completeInterp(const termstructure_type &ts, termstructure_type &ets) const;
   Real interpValue(const termstructure_type &ts, aValue<Real> *value) const;
};

typedef aTSCubicNHSplineInterpolator<double> TSCubicNHSplineInterpolator;


//Register this subclass into the TSInterpolator Factory.
namespace {

   /*natural-basis-spline*/
   TSInterpolator *CreateTSCubicBSplineInterpolator()
   {
      return new TSCubicBSplineInterpolator;
   }
   aTSInterpolator<AD_NAMESPACE::adouble> *aCreateTSCubicBSplineInterpolator()
   {
      return new aTSCubicBSplineInterpolator<AD_NAMESPACE::adouble>;
   }

   const int itTSCubicBSplineInterpolator = itSpline;

   const bool regTSCubicBSplineInterpolator = TSInterpolatorFactory::Instance().Register(itTSCubicBSplineInterpolator, CreateTSCubicBSplineInterpolator);
   const bool regaTSCubicBSplineInterpolator = aTSInterpolator<AD_NAMESPACE::adouble>
     ::Factory::Instance().Register(itTSCubicBSplineInterpolator, aCreateTSCubicBSplineInterpolator);


   /*natural-cubic-spline*/
      TSInterpolator *CreateTSCubicNSplineInterpolator()
   {
      return new TSCubicNSplineInterpolator;
   }
   aTSInterpolator<AD_NAMESPACE::adouble> *aCreateTSCubicNSplineInterpolator()
   {
      return new aTSCubicNSplineInterpolator<AD_NAMESPACE::adouble>;
   }

   const int itTSCubicNSplineInterpolator = itCNSpline;

   const bool regTSCubicNSplineInterpolator = TSInterpolatorFactory::Instance().Register(itTSCubicNSplineInterpolator, CreateTSCubicNSplineInterpolator);
   const bool regaTSCubicNSplineInterpolator = aTSInterpolator<AD_NAMESPACE::adouble>
     ::Factory::Instance().Register(itTSCubicNSplineInterpolator, aCreateTSCubicNSplineInterpolator);
   

   /*natural-hyman-cubic-spline*/
      TSInterpolator *CreateTSCubicNHSplineInterpolator()
   {
      return new TSCubicNHSplineInterpolator;
   }
   aTSInterpolator<AD_NAMESPACE::adouble> *aCreateTSCubicNHSplineInterpolator()
   {
      return new aTSCubicNHSplineInterpolator<AD_NAMESPACE::adouble>;
   }

   const int itTSCubicNHSplineInterpolator = itCNHSpline;

   const bool regTSCubicNHSplineInterpolator = TSInterpolatorFactory::Instance().Register(itTSCubicNHSplineInterpolator, CreateTSCubicNHSplineInterpolator);
   const bool regaTSCubicNHSplineInterpolator = aTSInterpolator<AD_NAMESPACE::adouble>
     ::Factory::Instance().Register(itTSCubicNHSplineInterpolator, aCreateTSCubicNHSplineInterpolator);

}

#endif // __CTSCUBICSPLINEINTERPOLATOR_H

//cTSLinearInterpolator.cpp
#include "cTSLinearInterpolator.h"
#include "eInterpolator.h"
#include "eUtility.h"
#include "cDiscountInterface.h"

template<class Real>
void aTSLinearInterpolator<Real>::completeInterp(const termstructure_type &ts, termstructure_type &ets) const
{
   typename termstructure_type::iterator pos;
   typename termstructure_type::const_iterator greaterTermIter = ts.begin();
   typename termstructure_type::const_iterator lessTermIter = ts.begin();
   
   //cycle on ets, which is an increasingly ordered (by date) map
   for(pos = ets.begin(); pos != ets.end(); ++pos) {
      //if necessary, re-bracket 
      if(((pos->second)->getTime() < (lessTermIter->second)->getTime()) || ((pos->second)->getTime() > (greaterTermIter->second)->getTime())) {
         //finds first ts entry with date >= date(pos)
         lessTermIter = ts.lower_bound((pos->second)->getEndDate());
         if(lessTermIter == ts.begin() && (pos->second)->getEndDate() != lessTermIter->first) 
            throwException(2, "Date before first TermStructure's date.");
         else if(lessTermIter == ts.end()){
            --lessTermIter; //to continue linearly from two last values
            aDiscountInterface<Real>* di = dynamic_cast<aDiscountInterface<Real>* >(pos->second.get());
            if(di) {//if the concept of zero rate is known, extrapolate constantly it (as Mx does)
               extrapolateZeroRate(ts.end(), di);
               continue;
            }
         }
         greaterTermIter = lessTermIter--;
      }
      //once bracketing is done check:
      // if time(pos)=time(lessTermIter)
      if((pos->second)->getTime() == (lessTermIter->second)->getTime())
         (pos->second)->putValue((lessTermIter->second)->getValue());
      // if time(pos)=time(greaterTermIter)
	  else if((pos->second)->getTime() == (greaterTermIter->second)->getTime())
         (pos->second)->putValue((greaterTermIter->second)->getValue());
      // else interpolate 
	  else {
         double x0 = (lessTermIter->second)->getTime();
         double x  = (pos->second)->getTime();
         double x1 = (greaterTermIter->second)->getTime();
         Real y0 = (lessTermIter->second)->getValue();
         Real y1 = (greaterTermIter->second)->getValue();
         (pos->second)->putValue(interp::linearInterp(x0, x, x1, y0, y1));
      }
   }
}

template<class Real>
Real aTSLinearInterpolator<Real>::extrapolateZeroRate(
	typename termstructure_type::const_iterator tsEnd, 
    aDiscountInterface<Real> *di) const
{
   aDiscountInterface<Real>* prec = 
   dynamic_cast<aDiscountInterface<Real>* >((--tsEnd)->second.get());
   Real zeroRate = -log(prec->getDiscount()) / prec->getTime();
   di->putDiscount(exp(-zeroRate * di->getTime()));
   return di->getValue();
}

template<class Real>
Real aTSLinearInterpolator<Real>::interpValue(const termstructure_type &ts, 
                                                aValue<Real> *value) const
{
   typename termstructure_type::const_iterator greaterTermIter;
   greaterTermIter = ts.lower_bound(value->getEndDate());

   if(greaterTermIter == ts.begin()) {
      if(value->getEndDate() == greaterTermIter->first) 
          return greaterTermIter->second->getValue();
      else
        throwException(2, "Date before first TermStructure's date.");
   }
   /*   else if(greaterTermIter == ts.end())
      greaterTermIter--;*/ //to continue linearly from two last values
   if(greaterTermIter == ts.end()) {
      aDiscountInterface<Real>* di = dynamic_cast<aDiscountInterface<Real>* >(value);
      if(!di) --greaterTermIter; //to continue linearly from two last values
	  else return extrapolateZeroRate(greaterTermIter, di);
   }
 
  typename termstructure_type::const_iterator lessTermIter = utility::prior(greaterTermIter);

   double x0 = (lessTermIter->second)->getTime();
   double x  = value->getTime();
   double x1 = (greaterTermIter->second)->getTime();
   Real y0 = (lessTermIter->second)->getValue();
   Real y1 = (greaterTermIter->second)->getValue();
   return interp::linearInterp(x0, x, x1, y0, y1);
}

template class aTSLinearInterpolator<double>;
template class aTSLinearInterpolator<AD_NAMESPACE::adouble>;

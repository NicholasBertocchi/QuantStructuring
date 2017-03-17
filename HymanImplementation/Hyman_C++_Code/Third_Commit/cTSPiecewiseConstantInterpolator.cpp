//cTSLinearInterpolator.cpp
#include "cTSPiecewiseConstantInterpolator.h"
#include "eInterpolator.h"

template<class Real>
void aTSPiecewiseConstantInterpolator<Real>::completeInterp(
  const termstructure_type &ts, termstructure_type &ets) const
{
   typename termstructure_type::iterator pos;
   typename termstructure_type::const_iterator termIter = ts.begin();

   for(pos=ets.begin(); pos!=ets.end(); ++pos) {
      termIter = ts.lower_bound((pos->second)->getEndDate());
      if(termIter==ts.begin()) throwException(2, "Date before first TermStructure's date.");
      else if(termIter==ts.end()) termIter--;

      (pos->second)->putValue((termIter->second)->getValue());
   }
}

template<class Real>
Real aTSPiecewiseConstantInterpolator<Real>::interpValue(const termstructure_type &ts,
                                                         aValue<Real> *value) const
{
   typename termstructure_type::const_iterator termIter;
   termIter = ts.lower_bound(value->getEndDate());

   if(termIter == ts.begin()) 
      throwException(2, "Date before first TermStructure's date.");
   else if(termIter == ts.end())  // if date's greater than last main term structure date, put value equal to last value
      termIter--;

   return (termIter->second)->getValue();
}

template class aTSPiecewiseConstantInterpolator<double>;
template class aTSPiecewiseConstantInterpolator<AD_NAMESPACE::adouble>;

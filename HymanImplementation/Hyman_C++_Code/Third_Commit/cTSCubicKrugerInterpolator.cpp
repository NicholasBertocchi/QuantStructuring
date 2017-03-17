//cTSCubicKrugerInterpolator.cpp
#include "cTSCubicKrugerInterpolator.h"
#include "eInterpolator.h"
#include "eUtility.h"

template<class Real>
void aTSCubicKrugerInterpolator<Real>::completeInterp(const termstructure_type &ts, 
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
      OutX[i] = pos->second->getTime();

   std::vector<Real> OutY = interp::cubicKrugerInterp(InX, InY, OutX);
   for (pos = ets.begin(), i = 0; pos != ets.end(); ++pos, ++i)
      pos->second->putValue(OutY[i]);
}

template<class Real>
Real aTSCubicKrugerInterpolator<Real>::interpValue(const termstructure_type &ts, 
                                                   aValue<Real> *value) const
{
#define NEBASTANO4 //dimezza i tempi
#ifdef NEBASTANO4

   std::vector<double> InX(4);
   std::vector<Real> InY(4);
   std::vector<double> OutX(1, value->getTime());
   long i;
   typename termstructure_type::const_iterator start, end; 
   start = ts.lower_bound(value->getEndDate()); //first element >= outX[0]
   end = start;
   //determine the 4 "surrounding" points in ts:
   for(i = 0; i < 2 && start != ts.begin(); ++i) --start;
   for(; i < 4; ++i) {
       if(end == ts.end()) {
           if(start == ts.begin()) 
               throw pdg::Error(2, "TSKrugerInterpolator::interpValue: ts is too short");
           else --start;
       }
       else ++end;
   }
   for(i = 0; i < 4 && start != end; ++start, ++i) { 
       InX[i] = start->second->getTime();
       InY[i] = start->second->getValue();
   }

#else //di #ifdef NEBASTANO4

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

#endif //#else di #ifdef NEBASTANO4

   return interp::cubicKrugerInterp(InX, InY, OutX)[0];
   
}

template class aTSCubicKrugerInterpolator<double>;
template class aTSCubicKrugerInterpolator<AD_NAMESPACE::adouble>;

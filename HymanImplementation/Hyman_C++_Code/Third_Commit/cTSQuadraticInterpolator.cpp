//cTSQuadraticInterpolator.cpp
#include "cTSQuadraticInterpolator.h"
#include "eInterpolator.h"
#include "eUtility.h"

template<class Real>
void aTSQuadraticInterpolator<Real>::completeInterp(const termstructure_type &ts, 
                                              termstructure_type &ets) const
{
   typename termstructure_type::const_iterator llTermIter;
   typename termstructure_type::const_iterator lTermIter;
   typename termstructure_type::const_iterator gTermIter;
   typename termstructure_type::const_iterator ggTermIter;

   typename termstructure_type::iterator pos;

   gTermIter = ts.begin();
   lTermIter = ts.begin();

   double x;
   double pX0; double pX1; double sX0; double sX1;
   Real pY0; Real pY1; Real sY0; Real sY1;

   for(pos = ets.begin(); pos != ets.end(); ++pos) {
      if(((pos->second)->getTime() < (lTermIter->second)->getTime()) || ((pos->second)->getTime() > (gTermIter->second)->getTime())) {
         gTermIter = ts.lower_bound((pos->second)->getEndDate());
         if(gTermIter == ts.begin()) //|| (gTermIter == ts.end())) 
            throwException(2, "Bad date in discount function interpolation process.");

         lTermIter = utility::prior(gTermIter);

         llTermIter = lTermIter;
         if(gTermIter == ts.end())  
            gTermIter = lTermIter--; // right out of sample
         if((lTermIter != ts.begin()) | (gTermIter == ts.end())) llTermIter = utility::prior(lTermIter);
         ggTermIter = utility::next(gTermIter);
         if(ggTermIter == ts.end()) ggTermIter = gTermIter;
      
         if ((lTermIter == llTermIter) && (gTermIter == ggTermIter)) throwException(3, "Not enough points to make an interpolation process.");
      }

      x  = (pos->second)->getTime();

      pX0 = (llTermIter->second)->getTime();
      pX1 = (lTermIter->second)->getTime();
      sX0 = (gTermIter->second)->getTime();
      sX1 = (ggTermIter->second)->getTime();
      
      pY0 = (llTermIter->second)->getValue();
      pY1 = (lTermIter->second)->getValue();
      sY0 = (gTermIter->second)->getValue();
      sY1 = (ggTermIter->second)->getValue();

      (pos->second)->putValue(interp::quadraticInterp(x, pX0, pX1, sX0, sX1, pY0, pY1, sY0, sY1));
   }
}

template<class Real>
Real aTSQuadraticInterpolator<Real>::interpValue(const termstructure_type &ts, 
                                                 aValue<Real> *value) const
{
   typename termstructure_type::const_iterator llTermIter;
   typename termstructure_type::const_iterator lTermIter;
   typename termstructure_type::const_iterator gTermIter;
   typename termstructure_type::const_iterator ggTermIter;

   gTermIter = ts.lower_bound(value->getEndDate());

   if(gTermIter == ts.begin())// || (gTermIter == ts.end())) right out of sample interpolation
      throwException(2, "Bad date in discount function interpolation process.");
   lTermIter = utility::prior(gTermIter);

   double x  = value->getTime();
   double pX0; double pX1; double sX0; double sX1;
   Real pY0; Real pY1; Real sY0; Real sY1;

   llTermIter = lTermIter;
   if(gTermIter == ts.end())  
      gTermIter = lTermIter--; // right out of sample
   if(lTermIter != ts.begin())   
      llTermIter = utility::prior(lTermIter);
   ggTermIter = utility::next(gTermIter);
   if(ggTermIter == ts.end()) 
      ggTermIter = gTermIter;

//   double prova = ((ts.end())->second)->getTime();
   double prova = (gTermIter->second)->getTime();  // ?
   if ((lTermIter == llTermIter) && (gTermIter == ggTermIter)) throwException(3, "Not enough points to make an interpolation process.");

   pX0 = (llTermIter->second)->getTime();
   pX1 = (lTermIter->second)->getTime();
   sX0 = (gTermIter->second)->getTime();
   sX1 = (ggTermIter->second)->getTime();
   
   pY0 = (llTermIter->second)->getValue();
   pY1 = (lTermIter->second)->getValue();
   sY0 = (gTermIter->second)->getValue();
   sY1 = (ggTermIter->second)->getValue();

   return interp::quadraticInterp(x, pX0, pX1, sX0, sX1, pY0, pY1, sY0, sY1);
}

template class aTSQuadraticInterpolator<double>;
template class aTSQuadraticInterpolator<AD_NAMESPACE::adouble>;

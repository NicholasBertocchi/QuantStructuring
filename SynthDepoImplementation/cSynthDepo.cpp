//cSynthDepo.cpp

#include "cSynthDepo.h"
#include "cZeroCurveData.h"
#include "cError.h"
#include "auto_diff.h"
#include <vector>
#include <math.h>
#include <list>
#include <algorithm>

enum TenorBasis {

      oneM = 1,
      threeM = 3,
      sixM = 6,
      twelveM = 12

      };

enum InterpBasis {
      
      lin,
      krug,  
      cspline,
      hspline,
      abcd

      };

std::vector<double> SynthCalculator(pdg::ZCData &Euron, 
                                    const std::vector<double> &MktQuote,  
                                    const std::vector<long> &SynthDepoMat,
                                    long Tenor,
                                    long InterpBasisType)

{
   std::vector<double> a(1);
   a[0] = 0;
   return a;

}
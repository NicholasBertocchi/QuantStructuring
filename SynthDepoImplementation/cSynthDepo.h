//cSyntDepo.h

#ifndef _CSYNTHDEPO_H_
#define _CSYNTHDEPO_H_

#include <vector>
#include "cZeroCurveData.h"

namespace {

   class SynthDepo
   {
   public: // SynthDepoCalculator
           std::vector<double> SynthCalculator(pdg::ZCData &Euron, 
                                               const std::vector<double> &MktQuote,  
                                               const std::vector<long> &SynthDepoMat,
                                               long Tenor,
                                               long InterpBasisType);

   };
}

#endif // _SYNTHDEPOCALCULATOR_H_
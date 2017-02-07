//ciInterp.h
#ifndef _CIINTERP_H__
#define _CIINTERP_H__

#include "pdgapi.h"
#include "ciError.h"

#ifdef __cplusplus
extern "C" {     /* Begin C Interface wrapping */
#endif

PDGLIB_API pdgerr_t pdg_interpNaturalBSpline(double *InX, double *InY, long nIn, 
                                             double *OutX, double *OutY, long nOut);

PDGLIB_API pdgerr_t pdg_interpNaturalBSplineSurf(long szInX, double *InX, long szInY, double *InY,
                                                 double **InZ, long szOutX, double *OutX,
                                                 long szOutY, double *OutY, double **OutZ);

PDGLIB_API pdgerr_t pdg_interpLinearSurf(long szInX, double *InX, long szInY, double *InY,
                                         double **InZ, double OutX,
                                         double OutY, double *pVal);

PDGLIB_API pdgerr_t pdg_regressBSplineSurf(long szBaseX, double *BaseX, long szBaseY, double *BaseY,
                                           long szInX, double *InX, long szInY, double *InY,
                                           long szInZ, double *InZ, long szOutX, double *OutX,
                                           long szOutY, double *OutY, double **OutZ);

PDGLIB_API pdgerr_t pdg_regressBSpline(long szBaseX, double *BaseX,
                                       long szInX, double *InX, long szInY, double *InY,
                                       long szOutX, double *OutX, double *OutY);

PDGLIB_API pdgerr_t pdg_interpKruger(double *InX, double *InY, long nIn, 
                                     double *OutX, double *OutY, long nOut);
									 
PDGLIB_API pdgerr_t pdg_interpMonotoneCubicSpline(double *InX, double *InY, long nIn, 
                                                  double *OutX, double *OutY, long nOut);

#ifdef __cplusplus
}     /* End C Interface wrapping */
#endif

#endif //_CIINTERP_H__


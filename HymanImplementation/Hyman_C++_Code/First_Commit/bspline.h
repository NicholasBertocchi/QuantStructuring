// bspline.h
#ifndef __BSPLINE_H
#define __BSPLINE_H

long stimabeta(long today, double alpha, long knot_sz, double *knot, long n_bond, long n_flow, 
               double **flow, long *time, double *beta, double *sqerr);

long stimabetaott(long today, double *alpha, long knot_sz, double *knot, long n_bond, long n_flow, 
                  double **flow, long *time, double *beta, double *sqerr);

long stimabetagss(long today, double *alpha, long knot_sz, double *knot, long n_bond, long n_flow, 
                  double **flow, long *time, double *beta, double *sqerr);

long spleval(long today, double alpha, long knot_sz, double *knot, double *beta, long n_date, 
             long *date, double **disc);

#endif   // __BSPLINE_H

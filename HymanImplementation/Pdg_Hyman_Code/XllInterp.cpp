#include "XllInterp.h"

#include <algorithm>
#include <vector>
#include <string>

#include "XlfOperCast.h"

#include "ciError.h"
#include "ciInterp.h"
#include "ciLibor.h"

/** \page xlInterp1D_page xlInterp1D
 *
 *  Returns an array containing the interpolation points relative
 *  to the outX coordinates. The interpolation methodology is specified
 *  the user. (Example: "linear", bspline", "kruger")
 *
 * \section Syntax
 *   xlInterp1D(InX, InY, OutX)
 *
 * \section Parameters
 *   \arg \a InX vertical array of input X coordinates.
 *   \arg \a InY vertical array of input Y coordinates.
 *   \arg \a OutX vertical array of output X coordinates.
 *
 * \section Returns
 *   an vertical array of output Y coordinates
 *
 * \section Example
 *
 * \sa
 *   \ref xlInterpLinear_page, 
 *   \ref xlInterpNaturalBSpline_page
 */
LPXLOPER EXCEL_EXPORT xlInterp1D(
  XlfOper xlInX,
  XlfOper xlInY,
  XlfOper xlOutX,
  XlfOper xlInterpMethod
){
  EXCEL_BEGIN;

  std::string method(xlInterpMethod.IsMissing() ? "linear" : xlInterpMethod.AsString());
  std::transform(method.begin(), method.end(), method.begin(), tolower);

  std::vector<double> inX;
  xloper_cast< std::vector<double> >(xlInX, inX);
  if (inX.empty()) return XlfOper("Empty X input array");

  std::vector<double> inY;
  xloper_cast< std::vector<double> >(xlInY, inY);
  if (inY.empty()) return XlfOper("Empty Y input array");

  if (inX.size() != inY.size()) return XlfOper("Number of X's differs from number of Y's");

  std::vector<double> outX;
  xloper_cast< std::vector<double> >(xlOutX, outX);
  if (outX.empty()) return XlfOper("Empty X output array");

  std::vector<double> outY;
  outY.resize(outX.size());

  pdgerr_t res;
  if ("linear" == method) {
    res = pdg_liborInterpLinear(
      inX.size(),
      &inX[0],
      &inY[0],
      outX.size(),
      &outX[0],
      &outY[0]
    );
  }
  else if ("bspline" == method) {
    res = pdg_interpNaturalBSpline(
      &inX[0],
      &inY[0],
      inX.size(),
      &outX[0],
      &outY[0],
      outX.size()
    );
  }
  else if ("kruger" == method) {
    res = pdg_interpKruger(
      &inX[0],
      &inY[0],
      inX.size(),
      &outX[0],
      &outY[0],
      outX.size()
    );
  }
  else if ("monotonicspline" == method) {
    res = pdg_interpMonotonicNaturalBSpline(
      &inX[0],
      &inY[0],
      inX.size(),
      &outX[0],
      &outY[0],
      outX.size()
    );
  }
  else {
    return XlfOper("Invalid Interpolation Method");
  }

  return ( (res.code) ? XlfOper(res.des) : XlfOper(outX.size(),1,&outY[0]) );

  EXCEL_END;
}

/** \page xlInterpLinear_page xlInterpLinear
 *
 *  Returns an array containing the linear interpolation points
 *  relative to the outX coordinates. If a point outside of the
 *  input domine is requested, the interpolation function provide
 *  the value of the nearest boundary.
 *
 * \section Syntax
 *   xlInterpLinear(InX, InY, OutX)
 *
 * \section Parameters
 *   \arg \a InX vertical array of input X coordinates.
 *   \arg \a InY vertical array of input Y coordinates.
 *   \arg \a OutX vertical array of output X coordinates.
 *
 * \section Returns
 *   an vertical array of output Y coordinates
 */
LPXLOPER EXCEL_EXPORT xlInterpLinear(
  XlfOper xlInX,
  XlfOper xlInY,
  XlfOper xlOutX
){
  EXCEL_BEGIN;

  char method[] = "\6linear";

  XLOPER xlMethod;
  xlMethod.xltype = xltypeStr;
  xlMethod.val.str = method;

  return xlInterp1D(xlInX, xlInY, xlOutX, &xlMethod);

  EXCEL_END;
}

/** \page xlInterpNaturalBSpline_page xlInterpNaturalBSpline
 *
 *  Returns an array containing the interpolation points relative
 *  to the outX coordinates. The Natural Cubic BSpline methodology
 *  is applied (see technical note), in particular the term "natural"
 *  concerns the boundary conditions: in this case we impose second
 *  derivative equal to zero to the boundary. Furthermore if a point
 *  outside of the input domine is requested the interpolation function
 *  provide the linear continuation from the nearest boundary (with
 *  slope given by the first derivative at the same boundary).
 *
 * \section Syntax
 *   xlInterpNaturalBSpline(InX, InY, OutX)
 *
 * \section Parameters
 *   \arg \a InX vertical array of input X coordinates.
 *   \arg \a InY vertical array of input Y coordinates.
 *   \arg \a OutX vertical array of output X coordinates.
 *
 * \section Returns
 *   an vertical array of output Y coordinates
 *
 */
LPXLOPER EXCEL_EXPORT xlInterpNaturalBSpline(
  XlfOper xlInX,
  XlfOper xlInY,
  XlfOper xlOutX
){
  EXCEL_BEGIN;

  char method[] = "\7bspline";

  XLOPER xlMethod;
  xlMethod.xltype = xltypeStr;
  xlMethod.val.str = method;

  return xlInterp1D(xlInX, xlInY, xlOutX, &xlMethod);

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlInterpMonotonicNaturalBSpline(
  XlfOper xlInX,
  XlfOper xlInY,
  XlfOper xlOutX
){
  EXCEL_BEGIN;

  char method[] = "\7bmonotonicspline";

  XLOPER xlMethod;
  xlMethod.xltype = xltypeStr;
  xlMethod.val.str = method;

  return xlInterp1D(xlInX, xlInY, xlOutX, &xlMethod);

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlInterpKruger(
  XlfOper xlInX, 
  XlfOper xlInY, 
  XlfOper xlOutX
){
  EXCEL_BEGIN;

  char method[] = "\6kruger";

  XLOPER xlMethod;
  xlMethod.xltype = xltypeStr;
  xlMethod.val.str = method;

  return xlInterp1D(xlInX, xlInY, xlOutX, &xlMethod);

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlInterp2D(
  XlfOper xlInX,
  XlfOper xlInY,
  XlfOper xlInZ,
  XlfOper xlOutX,
  XlfOper xlOutY,
  XlfOper xlInterpMethod
){
  EXCEL_BEGIN;

  std::string method(xlInterpMethod.IsMissing() ? "linear" : xlInterpMethod.AsString());
  std::transform(method.begin(), method.end(), method.begin(), tolower);

  std::vector<double> inX;
  xloper_cast< std::vector<double> >(xlInX, inX);
  if (inX.empty()) return XlfOper("Empty X input array");
  long inX_sz = inX.size();

  std::vector<double> inY;
  xloper_cast< std::vector<double> >(xlInY, inY);
  if (inY.empty()) return XlfOper("Empty Y input array");
  long inY_sz = inY.size();

  std::matrix<double> inZ;
  xloper_cast<std::matrix<double> >(xlInZ, inZ);
  if (inZ.size() == 0) return XlfOper("Empty Z input matrix");
  long inZ_r = inZ.rows();
  long inZ_c = inZ.cols();

  if (inX_sz != inZ_r) return XlfOper("Number of X's differs from number of Z rows");
  if (inY_sz != inZ_c) return XlfOper("Number of Y's differs from number of Z columns");

  std::vector<double> outX;
  xloper_cast< std::vector<double> >(xlOutX, outX);
  if (outX.empty()) return XlfOper("Empty X output array");
  long outX_sz = outX.size();

  std::vector<double> outY;
  xloper_cast< std::vector<double> >(xlOutY, outY);
  if (outY.empty()) return XlfOper("Empty Y output array");
  long outY_sz = outY.size();

  pdgerr_t res;
  if ("linear" == method) {
    std::vector<double> outZ;
    outZ.resize(outX_sz*outY_sz);

    for (long i = 0; i < outX_sz; ++i) {
      for (long j = 0; j < outY_sz; ++j) {
        res = pdg_interpLinearSurf(
                inX_sz,
                &inX[0],
                inY_sz,
                &inY[0],
                &inZ[0],
                outX[i],
                outY[j],
                &outZ[i*outY_sz + j]
              );
        if(res.code) return XlfOper(res.des);
      }
    }

    return ( (res.code) ? XlfOper(res.des) : XlfOper(outX.size(),outY.size(),&outZ[0]) );
  }
  else if ("bspline" == method) {
    std::matrix<double> outZ(outX_sz, outY_sz);

    res = pdg_interpNaturalBSplineSurf(
            inX_sz,
            &inX[0],
            inY_sz,
            &inY[0],
            &inZ[0],
            outX_sz,
            &outX[0],
            outY_sz,
            &outY[0],
            &outZ[0]
          );

    return ( (res.code) ? XlfOper(res.des) : XlfOper(outX.size(),outY.size(),&(outZ[0][0])) );
  }
  
  else {
    return XlfOper("Invalid Interpolation Method");
  }


  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlInterpLinearSurf(
  XlfOper xlInX,
  XlfOper xlInY,
  XlfOper xlInZ,
  XlfOper xlOutX,
  XlfOper xlOutY
){
  EXCEL_BEGIN;

  char method[] = "\6linear";

  XLOPER xlMethod;
  xlMethod.xltype = xltypeStr;
  xlMethod.val.str = method;

  return xlInterp2D(xlInX, xlInY, xlInZ, xlOutX, xlOutY, &xlMethod);

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlInterpNaturalBSplineSurf(
  XlfOper xlInX,
  XlfOper xlInY,
  XlfOper xlInZ,
  XlfOper xlOutX,
  XlfOper xlOutY
){
  EXCEL_BEGIN;

  char method[] = "\7bspline";

  XLOPER xlMethod;
  xlMethod.xltype = xltypeStr;
  xlMethod.val.str = method;

  return xlInterp2D(xlInX, xlInY, xlInZ, xlOutX, xlOutY, &xlMethod);

  EXCEL_END;
}

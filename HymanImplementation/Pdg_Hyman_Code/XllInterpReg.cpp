#include "excel32.h"
#include "RegMacros.h"

int PDGInterpReg(void)
{
   EXCEL_BEGIN;

   XlfExcel::Instance().SendMessage("Registering PDGInterp...");

  { // xlInterp1D(xlInX, xlInY, xlOutX, xlInterpMethod)
    XlfFuncDesc xlInterp1D("xlInterp1D",XLFUNCNAME(Interp1D),"Return the interpolation using the method specified","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    XlfArgDesc InterpMethod("InterpMethod","Interpolation Method ('linear', 'bspline', 'kruger')");
    xlInterp1D.SetArguments(InX+InY+OutX+InterpMethod);
    xlInterp1D.Register();
  } // END xlInterp1D

  { // xlInterpLinear(xlInX, xlInY, xlOutX)
    XlfFuncDesc InterpLinear("xlInterpLinear",XLFUNCNAME(InterpLinear),"Returns an array containing the linear interpolation points relative to the outX coordinates","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    InterpLinear.SetArguments(InX+InY+OutX);
    InterpLinear.Register();
  } // END xlInterpLinear

  { // xlInterpNaturalBSpline(xlInX, xlInY, xlOutX)
    XlfFuncDesc InterpNaturalBSpline("xlInterpNaturalBSpline",XLFUNCNAME(InterpNaturalBSpline),"Return the Cubic B-Spline interpolation","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    InterpNaturalBSpline.SetArguments(InX+InY+OutX);
    InterpNaturalBSpline.Register();
  } // END xlInterpNaturalBSpline
  
  { // xlInterpMonotonicNaturalBSpline(xlInX, xlInY, xlOutX)
    XlfFuncDesc InterpMonotonicNaturalBSpline("xlInterpMonotonicNaturalBSpline",XLFUNCNAME(InterpMonotonicNaturalBSpline),"Return the Hyman filtered Cubic B-Spline interpolation","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    InterpMonotonicNaturalBSpline.SetArguments(InX+InY+OutX);
    InterpMonotonicNaturalBSpline.Register();
  } // END xlInterpMonotonicNaturalBSpline

  { // xlInterp2D(xlInX, xlInY, xlInZ, xlOutX, xlOutY, xlInterpMethod)
    XlfFuncDesc xlInterp2D("xlInterp2D",XLFUNCNAME(Interp2D),"Returns a surface using the specified interpolation method","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc InZ("InZ","Matrix of input Z coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    XlfArgDesc OutY("OutY","Array of output Y coordinates");
    XlfArgDesc InterpMethod("InterpMethod","Interpolation Method ('linear', 'bspline')");
    xlInterp2D.SetArguments(InX+InY+InZ+OutX+OutY+InterpMethod);
    xlInterp2D.Register();
  } // END xlInterp1D

  { // xlInterpLinearSurf(xlInX, xlInY, xlInZ, xlOutX, xlOutY)
    XlfFuncDesc xlInterpLinearSurf("xlInterpLinearSurf",XLFUNCNAME(InterpLinearSurf),"Returns a surface using the linear interpolation method","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc InZ("InZ","Matrix of input Z coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    XlfArgDesc OutY("OutY","Array of output Y coordinates");
    xlInterpLinearSurf.SetArguments(InX+InY+InZ+OutX+OutY);
    xlInterpLinearSurf.Register();
  } // END xlInterp1D

  { // xlInterpNaturalBSplineSurf(xlInX, xlInY, xlInZ, xlOutX, xlOutY)
    XlfFuncDesc xlInterpNaturalBSplineSurf("xlInterpNaturalBSplineSurf",XLFUNCNAME(InterpNaturalBSplineSurf),"Returns a surface using the BSpline interpolation method","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc InZ("InZ","Matrix of input Z coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    XlfArgDesc OutY("OutY","Array of output Y coordinates");
    xlInterpNaturalBSplineSurf.SetArguments(InX+InY+InZ+OutX+OutY);
    xlInterpNaturalBSplineSurf.Register();
  } // END xlInterp1D

  { // xlInterpKruger(xlInX, xlInY, xlOutX)
    XlfFuncDesc InterpKruger("xlInterpKruger",XLFUNCNAME(InterpKruger),"Returns an array containing the kruger interpolation points relative to the outX coordinates","PDGInterp");
    XlfArgDesc InX("InX","Array of input X coordinates");
    XlfArgDesc InY("InY","Array of input Y coordinates");
    XlfArgDesc OutX("OutX","Array of output X coordinates");
    InterpKruger.SetArguments(InX+InY+OutX);
    InterpKruger.Register();
  } // END xlInterpKruger
  
  
   return 0;

   EXCEL_END;

}  // END PDGInterpReg

///////////////////////////////////////////////////////////////////////////////

#include "XllLibor.h"
#include <string>
#include <list>
#include <algorithm>
#include "cError.h"
#include "cCurrency.h"
#include "ciError.h"
#include "eLibor.h"
#include "ciLibor.h"
#include "XlfOperCast.h"
#include "XlfNamedPars.h"
#include "cShmLiborClientManager.h"
#include "rateBootstrapUtils.h"
#include "rateBootstrapDiff.h"
#include "cIROptions.h"

/** \page xllibornametocurrencyhandle_page xlLiborNameToCurrencyHandle
 *
 *  Returns the handle of the currency associated with the specified libor curve name.
 *
 * \section Syntax
 *   xlLiborNameToCurrencyHandle(LiborName)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of the libor curve.
 *
 * \section Returns
 *   An Excel number.
 *
 */
LPXLOPER EXCEL_EXPORT xlLiborNameToCurrencyHandle(XlfOper xlCurveName)
{
   EXCEL_BEGIN;

   std::string liborName(xlCurveName.AsString());
   //uppercase all characters
   std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

   long handle;
   pdgerr_t res = pdg_liborNameToCurHandle(liborName.c_str(), &handle);

   return (res.code) ? XlfOper(res.des) : XlfOper(static_cast<long>(handle));

   EXCEL_END;
}

/** \page xllibornametocurrency_page xlLiborNameToCurrency
 *
 *  Returns the name of the currency associated with the specified libor curve name.
 *
 * \section Syntax
 *   xlLiborNameToCurrency(LiborName)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of the libor curve.
 *
 * \section Returns
 *   An Excel string.
 *
 */
LPXLOPER EXCEL_EXPORT xlLiborNameToCurrency(XlfOper xlCurveName)
{
   EXCEL_BEGIN;

   std::string liborName(xlCurveName.AsString());
   //uppercase all characters
   std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

   std::auto_ptr<char> curName(new char[16]); // Magic Number! Pay attention!
   pdgerr_t res = pdg_liborNameToCurString(liborName.c_str(), curName.get());

   return (res.code) ? XlfOper(res.des) : XlfOper(curName.release());

   EXCEL_END;
}

/** \page xlcurrencytohandle_page xlCurrencyToHandle
 *
 *  Returns the handle of the requested currency.
 *
 * \section Syntax
 *   xlCurrencyToHandle(Currency)
 *
 * \section Parameters
 *   \arg \a Currency is the name of currency.
 *
 * \section Returns
 *   An Excel number.
 *
 */
LPXLOPER EXCEL_EXPORT xlCurrencyToHandle(XlfOper xlCurrency)
{
   EXCEL_BEGIN;

   long handle;
   pdgerr_t res = pdg_curStringToHandle(xlCurrency.AsString(), &handle);

   return (res.code) ? XlfOper(res.des) : XlfOper(static_cast<long>(handle));

   EXCEL_END;
}

/** \page xllibornametohandle_page xlLiborNameToHandle
 *
 *  Returns the handle associated with the requested libor.
 *
 * \section Syntax
 *   xlLiborNameToHandle(LiborName)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of the libor curve.
 *
 * \section Returns
 *   An Excel number.
 *
 */
LPXLOPER EXCEL_EXPORT xlLiborNameToHandle(XlfOper xlCurveName)
{
   EXCEL_BEGIN;

   long handle;
   pdgerr_t res = pdg_liborStringToHandle(xlCurveName.AsString(), &handle);

   return (res.code) ? XlfOper(res.des) : XlfOper(static_cast<long>(handle));

   EXCEL_END;
}

/** \page xlratetodisc_page xlRateToDisc
 *
 *  Returns the discount price relative to the rate using the calcMethod
 *  between the dateStart and dateEnd dates.
 *
 * \section Syntax
 *   xlRateToDisc(Rate, DateStart, DateEnd, CalcMethod)
 *
 * \section Parameters
 *   \arg \a Rate interest rate in absolute terms (i.e. 5% = 0.05).
 *   \arg \a DateStart is the start date.
 *   \arg \a DateEnd is the end date.
 *   \arg \a CalcMethod day count and compounding conventions.
 *           1 - 30E / 360 - Annual compounding.
 *           2 - 360 / 360 - Annual compounding.
 *           3 - 30E + 360 - Annual compounding.
 *           4 - ACT / 360 - Annual compounding.
 *           5 - ACT / 365 - Annual compounding.
 *           6 - 365 / 365 - Annual compounding.
 *           7 - ACT / ACT - Annual compounding.
 *           8 - ACT / 360 - Linear.
 *           9 - ACT / 365 - Linear.
 *
 * \section Returns
 *   An Excel number.
 *
 * \section Example
 *   Rate = 0.05
 *   DateStart = 5 apr 2000
 *   DateEnd = 7 apr 2000
 *   CalcMethod = 9
 *
 *   disc = xlRateToDisc(Rate, DateStart, DateEnd, CalcMethod)
 *   yields disc = 0.999726
 *
 * \sa
 *   \ref xlratetodiscext_page , 
 *   \ref xldisctorate_page , 
 *   \ref xldisctorateext_page 
 *
 */
LPXLOPER EXCEL_EXPORT xlRateToDisc(XlfOper xlRate, XlfOper xlDateStart, XlfOper xlDateEnd, XlfOper xlCalcMethod)
{
  EXCEL_BEGIN;

  double rate = xlRate.AsDouble();
  long dateStart = static_cast<long>(xlDateStart.AsDouble());
  long dateEnd = static_cast<long>(xlDateEnd.AsDouble());
  long calcMethod = static_cast<long>(xlCalcMethod.AsDouble());

  double disc = 0;
  pdgerr_t res = pdg_rateToDisc(rate, &disc, dateStart, dateEnd, calcMethod);

  return (res.code) ? XlfOper(res.des) : XlfOper(static_cast<double>(disc));

  EXCEL_END;
}

/** \page xlratetodiscext_page xlRateToDiscExt
 *
 *  Returns the discount price relative to the rate using the composition and
 *  the day count between the dateStart and dateEnd dates.
 *
 * \section Syntax
 *   xlRateToDiscExt(Rate, DateStart, DateEnd, Comp, DayCount)
 *
 * \section Parameters
 *   \arg \a Rate interest rate in absolute terms (i.e. 5% = 0.05).
 *   \arg \a DateStart is the start date.
 *   \arg \a DateEnd is the end date.
 *   \arg \a Comp is the compounding convention.
 *           0 - Linear.
 *           1 - Annual.
 *           2 - Continous.
 *   \arg \a DayCount is the day count convention.
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *
 * \section Returns
 *   An Excel number.
 *
 * \section Example
 *   Rate = 0.05
 *   DateStart = 5 apr 2000
 *   DateEnd = 7 apr 2000
 *   Comp = 0
 *   DayCount = 5
 *
 *   disc = xlRateToDiscExt(Rate, DateStart, DateEnd, Comp, DayCount)
 *   yields disc = 0.999726
 *
 * \sa
 *   \ref xlratetodisc_page , 
 *   \ref xldisctorate_page , 
 *   \ref xldisctorateext_page
 *
 */
LPXLOPER EXCEL_EXPORT xlRateToDiscExt(XlfOper xlRate, XlfOper xlDateStart, XlfOper xlDateEnd, XlfOper xlComp, XlfOper xlDayCount)
{
   EXCEL_BEGIN;

   double rate = xlRate.AsDouble();
   long dateStart = xlDateStart.AsLong();
   long dateEnd = xlDateEnd.AsLong();
   long comp = xlComp.AsLong();
   long dayCount = xlDayCount.AsLong();

   double disc = 0;

   pdgerr_t res = pdg_rateToDiscExt(rate, &disc, dateStart, dateEnd, comp, dayCount);

   return (res.code) ? XlfOper(res.des) : XlfOper(disc);

   EXCEL_END;
}

/** \page xldisctorate_page xlDiscToRate
 *
 *  Returns the interest rate relative to the discount using the calcMethod
 *  between the dateStart and dateEnd dates.
 *
 * \section Syntax
 *   xlDiscToRate(Disc, DateStart, DateEnd, CalcMethod)
 *
 * \section Parameters
 *   \arg \a Disc discount price.
 *   \arg \a DateStart is the start date.
 *   \arg \a DateEnd is the end date.
 *   \arg \a CalcMethod day count and compounding conventions.
 *           1 - 30E / 360 - Annual compounding.
 *           2 - 360 / 360 - Annual compounding.
 *           3 - 30E + 360 - Annual compounding.
 *           4 - ACT / 360 - Annual compounding.
 *           5 - ACT / 365 - Annual compounding.
 *           6 - 365 / 365 - Annual compounding.
 *           7 - ACT / ACT - Annual compounding.
 *           8 - ACT / 360 - Linear.
 *           9 - ACT / 365 - Linear.
 *
 * \section Returns
 *   An Excel number.
 *
 * \section Example
 *   Disc = 0.999726
 *   DateStart = 5 apr 2000
 *   DateEnd = 7 apr 2000
 *   CalcMethod = 9
 *
 *   rate = xlDiscToRate(Rate, DateStart, DateEnd, CalcMethod)
 *   yields rate = 0.05
 *
 * \sa
 *   \ref xlratetodisc_page , 
 *   \ref xlratetodiscext_page , 
 *   \ref xldisctorateext_page
 *
 */
LPXLOPER EXCEL_EXPORT xlDiscToRate(XlfOper xlDisc, XlfOper xlDateStart, XlfOper xlDateEnd, XlfOper xlCalcMethod)
{
  EXCEL_BEGIN;

   double disc = xlDisc.AsDouble();
   long dateStart = static_cast<long>(xlDateStart.AsDouble());
   long dateEnd = static_cast<long>(xlDateEnd.AsDouble());
   long calcMethod = static_cast<long>(xlCalcMethod.AsDouble());

   double rate = 0;

   pdgerr_t res = pdg_discToRate(&rate, disc, dateStart, dateEnd, calcMethod);

   return (res.code) ? XlfOper(res.des) : XlfOper(rate);

  EXCEL_END;
}

/** \page xldisctorateext_page xlDiscToRateExt
 *
 *  Returns the interest rate relative to the discount using the composition and
 *  the day count between the DateStart and DateEnd dates.
 *
 * \section Syntax
 *   xlDiscToRateExt(Disc, DateStart, DateEnd, Comp, DayCount)
 *
 * \section Parameters
 *   \arg \a Disc discount price.
 *   \arg \a DateStart is the start date.
 *   \arg \a DateEnd is the end date.
 *   \arg \a Comp is the compounding convention.
 *           0 - Linear.
 *           1 - Annual.
 *           2 - Continous.
 *   \arg \a DayCount is the day count convention.
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *
 * \section Returns
 *   An Excel number.
 *
 * \section Example
 *   Disc = 0.999726
 *   DateStart = 5 apr 2000
 *   DateEnd = 7 apr 2000
 *   Comp = 0
 *   DayCount = 5
 *
 *   rate = xlDiscToRateExt(Rate, DateStart, DateEnd, Comp, DayCount)
 *   yields rate = 0.05
 *
 * \sa
 *   \ref xlratetodisc_page , 
 *   \ref xlratetodiscext_page , 
 *   \ref xldisctorate_page
 *
 */
LPXLOPER EXCEL_EXPORT xlDiscToRateExt(XlfOper xlRate, XlfOper xlDateStart, XlfOper xlDateEnd, XlfOper xlComp, XlfOper xlDayCount)
{
  EXCEL_BEGIN;

   double disc = xlRate.AsDouble();
   long dateStart = static_cast<long>(xlDateStart.AsDouble());
   long dateEnd = static_cast<long>(xlDateEnd.AsDouble());
   long comp = xlComp.AsLong();
   long dayCount = xlDayCount.AsLong();

   double rate = 0;

   pdgerr_t res = pdg_discToRateExt(&rate, disc, dateStart, dateEnd, comp, dayCount);

   return (res.code) ? XlfOper(res.des) : XlfOper(rate);

  EXCEL_END;
}

/** \page xlcurvebootstrapper_page xlCurveBootstrapper
 *
 *  Bootstrap (yield) curve from deposits, FRA/Futures and Swaps, eventually using an
 *  exogenous (previously bootstrapped) discounting curve. 
 *  The output is a two-column matrix containing a vertical array of
 *  dates and a vertical array of discounts/zero rates;
 *  additional columns are added to the output if the jacobian of the bootstrap is required. 
 *  The bootstrapped curve is stored in shared memory (by the curve name).
 *  An array of custom fields allows to play with a variety of bootstrapping options/settings, as listed below.
 * \section Syntax
 *   xlCurveBootstrapper(Today, Currency, CurveName, 
                         NDepo, Deposits,
                         NFra, Fra,                          
                         NFutu, Futures, 
                         NSwap, Swaps,
                         InterpType, InterpOn, Comp, DayCount,
                         CustomFields,CustomValues)
 *
 * \section parameters Parameters
 *   \arg \a Today today date.
 *   \arg \a Currency is the currency of the curve.
 *   \arg \a CurveName is the name of the curve to be built.
 *   \arg \a NDepo number of deposits.
 *   \arg \a Deposits three-column array containing the deposit data:
 *           1st column and  2nd column contain information about the start and
 *           the end date of the contract; the 3rd column contains the rate in
 *           percentage terms (i.e. 3% = 3).
 *           1st column:
 *             [integer number] - number of calendar days/weeks/months or a date,
 *             according to deposit type as specified in the first column of
 *             corresponding element.
 *             [empty] - for spot-next, tom-next, overnight  deposit contract.
 *           2nd column:
 *             o – "overnight", ([t,t+1] deposit).
 *             t – "tom-next"  ([t+1,t+2] deposit).
 *             s – "spot-next" ([t+2,t+3] deposit).
 *             m - month deposit.
 *             w - week deposit.
 *             d - calendar days deposit.
 *             c - custom deposit date.
 *             e - EONIA deposit (covers depos with maturities > 1Y).
 *   \arg \a NFra number of FRA.
 *   \arg \a Fra two-column array containing the FRA market data:
 *           1st column contains the maturity date of the FRA, 2nd column contains the
 *           FRA rate (as a percentage). 
 *   \arg \a NFutu number of FRA/Futures.
 *   \arg \a FRA/Futures two-column array containing the future market data:
 *           1st column contains the maturity date of the future/FRA 2nd column contains the
 *           future price. (don't worry
 *           about the exact day of maturity month, the actual delivery date is
 *           calculated using xlDateNextFutLIFFE.) FRA rates can be passed as futures, mind the conversion FuturesRate = 1-FRARate
 *   \arg \a NSwap number of single-currency swaps.
 *   \arg \a Swaps two-column matrix containing the swap market data:
 *           1st column contains the Maturity of the swap (years to maturity, month, or date, according to optional field SwapMatType), 
 *           2nd column contains the swap rate in percentage terms (3% inserted as 3).
 *   \arg \a InterpType interpolation method:
 *           "linear" - linear interpolation
 *           "quadra" - quadratic interpolation
 *           "kruger" - monotonic cubic spline 
 *           "const"  - piece-wise constant
 *           "bspline" - natural basis spline
 *           "cspline" - natural cubic spline
 *           "hspline" - natural cubic spline with Hyman Filter
 *           [Optional field, default = "linear"]
 *   \arg \a InterpOn interpolation seed (i.e. the underlying of the
 *           interpolation):
 *           "r"  - interest rate.
 *           "rt" - interpolated rate by time to maturity.
 *           "d"  - discount.
 *           [Optional field, default= "r"]
 *   \arg \a Comp interpolated rate compounding method:
 *           "cont"   - continuously compounded.
 *           "annual" - annual compounded.
 *           "simple" - linear.
 *           [Optional field, default= "cont"]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           9 - 1/1
 *           [Optional field, default= ACT/365]
 *   \arg \a CustomFields an array of names referring to optional customizable fields, see below
 *   \arg \a CustomValues an array of names referring to optional customizable fields, see below
 *
 *   \section customdetails CustomFields and CustomValues
 *
 *   \arg \a DiscCurveName is the name of the discounting curve to be used for bootstrap purposes.
 *           If empty, the curve is bootstrapped with endogenous discounting (single curve)
 *
 *   \arg \a FutEndDate futures end date flag:
 *           "adjusted" - the maturity of the deposit implicit in the future is
 *                        adjusted for the calendar holiday.
 *           "standard" - the maturity of the deposit implicit in the future is
 *                        the maturity in the following future in the strip.
 *           [Optional field, default = "standard"]
 *
 *   \arg \a MatBlockCons maturity block consistency flag. The block
 *           consistency refers to the fact that the maturities of deposit
 *           futures and swap often overlap; in this case we have to choose
 *           which instrument to consider and which one to put aside:
 *           "on"  - consistency on.
 *           "off" - consistency off.
 *           [Optional field, default = "on"]
 *
 *   \arg \a FutBlockCons future block consistency flag:
 *           "end"   - consistency starts from the maturity of the deposit
 *                     implicit on the first future.
 *           "start" - consistency starts from the maturity of the first future.
 *           "stub"  - the same as "end" but an additional stub deposit is
 *                     considered with maturity equal to the maturity of the
 *                     first future and rate obtained by interpolation of two
 *                     nearest market deposits.
 *           [Optional field, default = "start"]
 *
 *   \arg \a UseExtStubRate If>0, search for an exogenously given stub rate, otherwise interpolate it. (Effective only if FutBlockCons="stub", default = 0) 
 *
 *   \arg \a FutBlockStubRate stub rate in % units, mandatory if UseExtStubRate>0
 *
 *   \arg \a BlockOrder string specifying the instruments block order for block
 *           consistency selection (i.e. "213" means precedence given to
 *           fra/futures (# 2) then to the deposits (# 1) then to the swaps (# 3))
 *           [Optional field, default = "231"]
 *
 *   \arg \a StubPriority flag specifying the priority rule between deposit block and
 *           stub deposit ("1" means precedence given to stub, "0" to deposit)
 *           [Optional field, default = "0"]
 *
 *   \arg \a DepoValDays is the number of value days of deposits 
 *				 [optional, if missing currency default is used, see cMarketConvention.cpp] 
 *
 *   \arg \a FraFutDCCode the day count code to be used for FRA/Futures.
 *           [Optional field, default is the depo dayccode, see cDepositConvention.h]
 * 
 *   \arg \a FraFutTenor the tenor of FRA/Futures in months
 *           [Optional field, default = 3]
 * 
 *   \arg \a FraFutAdjType the adjustment type for FRA/Futures
 *           0 NoAdjust
 *           1 Backward
 *           2 Forward
 *           3 FwModFollowing
 *           [Optional field, default = 2]
 *
 *   \arg \a SwapFltFreq the frequency of swap floating leg payments. If missing or zero, a
 *           currency-based default frequency is used (e.g. semiannually for EUR)    
 *           [Optional field, if missing currency default is used, see cSwapConvention.cpp]
 *
 *   \arg \a SwapFixFreq the frequency of swap fixed leg payments. If missing or zero, a
 *           currency-based default frequency is used (e.g. annually for EUR)    
 *           [Optional field, if missing currency default is used, see cSwapConvention.cpp]
 *
 *   \arg \a SwapFltYrf the day count convention of swap floating leg payments. If missing or zero, a
 *           currency-based default dcc is used (e.g. ACT/360 for EUR)    
 *           [Optional field, if missing currency default is used, see cSwapConvention.cpp]
 *
 *   \arg \a SwapFixYrf the day count convention of swap fixed leg payments. If missing or zero, a
 *           currency-based default dcc is used (e.g. 30/360 for EUR)    
 *           [Optional field, if missing currency default is used, see cSwapConvention.cpp]
 *
 *   \arg \a SwapMatType the measure unit of swap maturities:
 *           0 if expressed in years to maturity
 *           1 if expressed in months
 *           2 if expressed as (XL) date 
 *           [Optional field, default 0]
 *
 *   \arg \a MSwapFixFreq overrides the frequency of the fixed leg payments of swaps with maturity less or equal to 1 year. 
 *           If missing or zero, no override take place.
 *
 *   \arg \a MSwapFixYrf ovverrides the day count convention for fixed leg payments of swaps with maturity less or equal to 1 year. 
 *           If missing or zero, no override take place.
 *
 *   \arg \a SwapFirstCpnFixed If positive, the floater rate for the 1st coupon is assumed to be known and is read from the field "SwapFirstCpn", otherwise it is estimated on the forwarding curve. [optional, default=0]
 *
 *   \arg \a SwapFirstCpn  is the fixing of the floating rate for first coupon. Effective only if the field "SwapFirstCpnFixed" is positive; in this case it is mandatory to value this field.
 *
 *   \arg \a OutputType triggers between discounts (0) or zero rates (1) [default is (0)]
 *
 *   \arg \a CalculateJacobian choose to compute the jacobian ("YES") or not ("NO") [default is "NO"]
 *
 * \section Returns
 *   A two columns Excel range with maturities and discounts/zero rates (according to OutputType option).
 *   If custom option CalculateJacobian is set to "YES", additional columns are added to the range,
 *   each column representing the derivative of the discounts vector with respect to an input value;
 *   the first columns represent derivatives with respect to deposit rates, 
 *   followed by derivatives with respect to future prices, then to swap rates, 
 *   and finally to the discounting curve's discounts in case of exogenous bootstrap.
 */

LPXLOPER EXCEL_EXPORT xlCurveBootstrapper(XlfOper xlToday, XlfOper xlCurrency,XlfOper xlCurveName, 
                                         XlfOper xlNDepo, XlfOper xlDeposits, 
                                         XlfOper xlNFra, XlfOper xlFra,
                                         XlfOper xlNFutu, XlfOper xlFutures, 
                                         XlfOper xlNSwap, XlfOper xlSwaps,
                                         XlfOper xlInterpType, XlfOper xlInterpOn, XlfOper xlComp, XlfOper xlDayCount, 
                                         XlfOper xlCustomFields, XlfOper xlCustomValues)
{
   EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");

   //currency and its handle
   std::string curName(xlCurrency.AsString());
   long hCurr;   
   pdgerr_t res = pdg_curStringToHandle(curName.c_str(), &hCurr);
   if(res.code) return XlfOper(res.des);
   
   //curve name
   std::string curveName(xlCurveName.AsString());

  // Depos
   long NDepo = xloper_cast<long>(xlNDepo);
   if(NDepo < 0) return XlfOper("Invalid number of depos");
   
   XlfRef Deposits = xlDeposits.AsRef();
   if((Deposits.GetNbCols() != 3) || (Deposits.GetNbRows() < NDepo))
      return XlfOper("Deposits data range of inconsistent size");
   
   std::vector<long>   depoLen(NDepo);
   std::vector<char>   depoType(NDepo);
   std::vector<double> depoRate(NDepo); 
   for (long i = 0; i < NDepo; ++i) {
         depoLen.at(i)  = static_cast<long>(1.e-9 + Deposits(i, 0).AsDouble());
         depoType.at(i) = Deposits(i, 1).AsString()[0];
         depoRate.at(i) = 0.01 * Deposits(i, 2).AsDouble();
   }

   // FRA
   long NFra = xloper_cast<long>(xlNFra);
   if (NFra < 0) return XlfOper("Invalid number of FRA");
   
   XlfRef Fra = xlFra.AsRef();
   if ((Fra.GetNbCols() != 2) || (Fra.GetNbRows() < NFra))
      return XlfOper("FRA data range of inconsistent size");
   
   std::vector<long>   fraDate(NFra);
   std::vector<double> fraRate(NFra);
   for (long i = 0; i < NFra; ++i) {
         fraDate.at(i) = static_cast<long>(1.e-9 + Fra(i,0).AsDouble());
         fraRate.at(i) = 0.01 * Fra(i,1).AsDouble();
   }
   
   // Futures
   long NFutu = xloper_cast<long>(xlNFutu);
   if (NFutu < 0) return XlfOper("Invalid number of futures");
   
   XlfRef Futures = xlFutures.AsRef();
   if ((Futures.GetNbCols() != 2) || (Futures.GetNbRows() < NFutu))
      return XlfOper("Futures data range of inconsistent size");
   
   std::vector<long>   futDate(NFutu);
   std::vector<double> futPrice(NFutu);
   for (long i = 0; i < NFutu; ++i) {
         futDate.at(i) = static_cast<long>(1.e-9 + Futures(i,0).AsDouble());
         futPrice.at(i) = 0.01 * Futures(i,1).AsDouble();
   }
   
   // Swaps
   long NSwap = xloper_cast<long>(xlNSwap);
   if (NSwap < 0 ) return XlfOper("Invalid number of swaps");
   
   XlfRef Swaps = xlSwaps.AsRef();
   if ((Swaps.GetNbCols() != 2) || (Swaps.GetNbRows() < NSwap))
      return XlfOper("Swaps data range of inconsistent size");
   
   std::vector<long> swapLen(NSwap);
   std::vector<double> swapRate(NSwap);
   for (long i = 0; i < NSwap; ++i) {
         swapLen.at(i) = static_cast<long>(1.e-9 + Swaps(i,0).AsDouble());
         swapRate.at(i) = 0.01 * Swaps(i,1).AsDouble();
   }
  
   // Interpolation parameters
   long interpType;
   if (xlInterpType.IsMissing() || xlInterpType.AsString()[0] == 0) interpType = 1;
   else {
      res = pdg_getInterpMethod(xlInterpType.AsString(), &interpType);
      if(res.code) return XlfOper(res.des);
   } 
   
   long interpOn;
   if (xlInterpOn.IsMissing() || xlInterpOn.AsString()[0] == 0) interpOn = 1;
   else {
      res = pdg_getInterpSeed(xlInterpOn.AsString(), &interpOn);
      if(res.code) return XlfOper(res.des);
   }
   
   long comp;
   if (xlComp.IsMissing() || xlComp.AsString()[0] == 0) comp = 3;
   else {
      res = pdg_getCompoundingMethod(xlComp.AsString(), &comp);
      if(res.code) return XlfOper(res.des);
   }
   
   // Day Count convention
   long dayCount = (xlDayCount.IsMissing() || xlDayCount.AsString()[0] == 0) ? 5 : (long)xlDayCount.AsDouble();

   //Customizable Fields
   std::vector<std::string> optNames;
   xloper_cast<std::vector<std::string> >(xlCustomFields, optNames);
   std::vector<double> optVals;
   xloper_cast<std::vector<double> >(xlCustomValues, optVals);
   if(optNames.size()!=optVals.size())
      return XlfOper("#Error, custom name and value vectors have different sizes");

   //all rates are passed through in natural units
   for(size_t i=0, imax = optNames.size(); i<imax ; ++i){
      const std::string& name = optNames.at(i);
      if (name == "FutBlockStubRate" || name == "SwapFirstCpn")
         optVals.at(i)*=0.01;
   }
   //convert optnames to char**
   std::vector<const char *> rnv_c;
   for(size_t i=0, imax = optNames.size(); i<imax; ++i){
      rnv_c.push_back(optNames.at(i).c_str());
   }

   // Discount function arrays
   long discSize = 1 /* Today */ + NDepo + NFra + NFutu + NSwap + 1 /*place for (optional) stub date*/;
   std::vector<long>  discDates(discSize);
   std::vector<double> discVals(discSize);
   
   res = pdg_liborCurveCustom((long)xlToday.AsDouble(),
                              hCurr,
                              curveName.c_str(),
                              NDepo,
                              safe_begin(depoType),
                              safe_begin(depoLen),
                              safe_begin(depoRate),
                              NFra,
                              safe_begin(fraDate),
                              safe_begin(fraRate),
                              NFutu,
                              safe_begin(futDate),
                              safe_begin(futPrice),
                              NSwap,
                              safe_begin(swapLen),
                              safe_begin(swapRate),
                              interpType,
                              interpOn,
                              comp,
                              dayCount,
                              optNames.size(),
                              safe_begin(rnv_c),
                              safe_begin(optVals),
                              &discSize,
                              safe_begin(discDates),
                              safe_begin(discVals)
                              );
   
    if(res.code) return XlfOper(res.des);

    long nColsJac = 0;
	std::vector<std::vector<double> > jacobian;

    try {
		const std::string errMsg("#Error in xlCurveBootstrapper, ");

   		//Read customizable fields
		IROptions customOpt(safe_begin(rnv_c), safe_begin(optVals), optNames.size());

		const bool calculateJacobian = customOpt.get_string("calculateJacobian", "NO") == "YES"; //"YES", "NO"

		if(calculateJacobian){

			discDates.resize(discSize);
            discVals.resize(discSize);

			BootstrapData bootstrapData((long)xlToday.AsDouble(), curName, curveName, depoType,
										depoLen, depoRate, fraDate, fraRate, futDate, futPrice, swapLen, swapRate, 
										xlInterpType.AsString(), xlInterpOn.AsString(), xlComp.AsString(), dayCount,
										optNames, optVals);

			//if needed, get handle to exogenous discounting curve and retrieve its data
			pdg::ZCData exo_disc_curve_data;
			pdg::ZCDataPtr exogenousZcData;
			std::string exoDiscName = customOpt.get_string("DiscCurveName","");
			std::transform(exoDiscName.begin(), exoDiscName.end(), exoDiscName.begin(), toupper);
			bool exogenousBootstrap = exoDiscName.size()>0;
			if(exogenousBootstrap){
				bool curveOK = libor::getShmZCData(exoDiscName, exo_disc_curve_data);
				if(!curveOK) throw pdg::Error(2,errMsg + "curve " + exoDiscName + " not found");
				exogenousZcData = pdg::ZCDataPtr(new pdg::ZCData(exo_disc_curve_data));
			}

			std::vector<std::vector<double> > sensToDepo;
            std::vector<std::vector<double> > sensToFra;
			std::vector<std::vector<double> > sensToFutu;
			std::vector<std::vector<double> > sensToSwap;
			std::vector<std::vector<double> > sensToExo;

			rateBootstrapJacobian_ad(bootstrapData, discDates, discVals,
									&sensToDepo, &sensToFra, &sensToFutu, &sensToSwap, &sensToExo,
									exogenousZcData);

			size_t nRows = sensToDepo.size() + 1;
			jacobian.resize(nRows);			
			nColsJac = sensToDepo.at(0).size() + sensToFra.at(0).size() 
               + sensToFutu.at(0).size() + sensToSwap.at(0).size();
			if(exogenousBootstrap) nColsJac += sensToExo.at(0).size();
	        
			//manca la sensitivity del nodo today (0 per i discount)
			jacobian[0].resize(nColsJac, 0.0);
			for(size_t i = 1; i < nRows; ++i) {
				jacobian[i].reserve(nColsJac);
				jacobian[i].insert(jacobian[i].end(), sensToDepo[i-1].begin(), sensToDepo[i-1].end());
                jacobian[i].insert(jacobian[i].end(), sensToFra[i-1].begin(), sensToFra[i-1].end());
				jacobian[i].insert(jacobian[i].end(), sensToFutu[i-1].begin(), sensToFutu[i-1].end());
				jacobian[i].insert(jacobian[i].end(), sensToSwap[i-1].begin(), sensToSwap[i-1].end());
				if(exogenousBootstrap)
					jacobian[i].insert(jacobian[i].end(), sensToExo[i-1].begin(), sensToExo[i-1].end());
			}
		}
	}
	catch(pdg::Error e) {
		return XlfOper(e.getDes());
	}
	catch(...) {
		return XlfOper(RES_FAIL.des);
	}
   
	const long outCols = 2 + nColsJac;
	std::vector<double> OutVals(discSize * outCols);
	for (size_t i = 0; i < discSize; ++i) {
		OutVals.at(i*outCols) = discDates.at(i);
		OutVals.at(i*outCols+1) = discVals.at(i);
		for (size_t k = 0; k < nColsJac; ++k) {
			OutVals.at(i*outCols + k + 2) = jacobian.at(i).at(k);
		}
	}

	return XlfOper(discSize,outCols,safe_begin(OutVals));

	EXCEL_END;
}


/** \page xlliborcurvecustom_page xlLiborCurveCustom
 *
 *  ******************************************************************************************************************************\n
 *  THIS FUNCTION IS SCHEDULED FOR DEPRECATION AND HAS BEEN PRESERVED ONLY FOR BACKWARD COMPATIBILITY WITH EXISTING XL WORKBOOKS.  \n  
 *  PLEASE, PREFER NEW FUNCTION \ref xlcurvebootstrapper_page INSTEAD. \n
 *  ******************************************************************************************************************************\n
 *  Bootstrap (yield) curve from deposits, FRA/Futures and Swaps, eventually using an
 *  exogenous (previously bootstrapped) discounting curve. 
 *  The output is a two-column matrix containing a vertical array of
 *  dates and a vertical array of discounts/zero rates. The bootstrapped curve is 
 *  stored in shared memory (by the curve name).
 *  An array of custom fields allows to play with a variety of bootstrapping options/settings, as listed below.\n
 * \section Syntax
 *   xlLiborCurveCustom(xlToday, xlCurrency, xlCurveName, 
 *                      xlNDepo, xlDeposits, xlNFutu, xlFutures, xlNSwap, xlSwaps, 
 *                      xlFutEndDate, xlMatBlockConsFlag, xlFutBlockConsFlag, xlBlockOrder, 
 *                      xlInterpType,xlInterpOn, xlComp, xlDayCount, 
 *                      xlCustomFields, xlCustomValues)
 *
 * \section parameters Parameters
 *   \arg \a Today today date.
 *   \arg \a Currency is the currency of the curve.
 *   \arg \a CurveName is the name of the curve to be built.
 *   \arg \a NDepo number of deposits.
 *   \arg \a Deposits three-column array containing the deposit data:
 *           1st column and  2nd column contain information about the start and
 *           the end date of the contract; the 3rd column contains the rate in
 *           percentage terms (i.e. 3% = 3).
 *           1st column:
 *             [integer number] - number of calendar days/weeks/months or a date,
 *             according to deposit type as specified in the first column of
 *             corresponding element.
 *             [empty] - for spot-next, tom-next, overnight  deposit contract.
 *           2nd column:
 *             m - month deposit.
 *             w - week deposit.
 *             s - spot (for spot-next contract).
 *             t - tom (for tom-next contract).
 *             o - overnight (for over-night contract).
 *             d - calendar days deposit.
 *             c - custom deposit date.
 *             e - EONIA deposit (covers depos with maturities > 1Y).
 *   \arg \a NFutu number of FRA/Futures.
 *   \arg \a FRA/Futures two-column array containing the future market data:
 *           1st column contains the maturity date of the future/FRA 2nd column contains the
 *           future price. (don't worry
 *           about the exact day of maturity month, the actual delivery date is
 *           calculated using xlDateNextFutLIFFE), FRA rates are passed as futures, mind the conversion FuturesRate = 1-FRARate
 *   \arg \a NSwap number of euro-currency swaps.
 *   \arg \a Swaps two-column matrix containing the swap market data:
 *           1st column contains the Maturity of the swap (years to maturity, month, or date, according to optional field SwapMatType), 
 *           2nd column contains the swap rate in percentage terms (3% inserted as 3).
 *   \arg \a FutEndDate futures end date flag:
 *           "adjusted" - the maturity of the deposit implicit in the future is
 *                        adjusted for the calendar holiday.
 *           "standard" - the maturity of the deposit implicit in the future is
 *                        the maturity in the following future in the strip.
 *           [Optional field, default = "standard"]
 *   \arg \a MatBlockConsFlag maturity block consistency flag. The block
 *           consistency refers to the fact that the maturities of deposit
 *           futures and swap often overlap; in this case we have to choose
 *           which instrument to consider and which one to put aside:
 *           "on"  - consistency on.
 *           "off" - consistency off.
 *           [Optional field, default = "on"]
 *   \arg \a FutBlockConsFlag future block consistency flag:
 *           "end"   - consistency starts from the maturity of the deposit
 *                     implicit on the first future.
 *           "start" - consistency starts from the maturity of the first future.
 *           "stub"  - the same as "end" but an additional stub deposit is
 *                     considered with maturity equal to the maturity of the
 *                     first future and rate obtained by interpolation of two
 *                     nearest market deposits.
 *   \arg \a BlockOrder string specifying the instruments block order for block
 *           consistency selection (i.e. "213" means precedence given to
 *           futures (# 2) then to the deposits (# 1) then to the swaps (# 3))
 *           [Optional field, default = "231"]
 *
 *   \arg \a InterpType interpolation method:
 *           "linear" - linear interpolation
 *           "quadra" - quadratic interpolation
 *           "kruger" - monotonic cubic spline 
 *           "const"  - piece-wise constant
 *          "bspline" - natural basis spline
 *           "cspline" - natural cubic spline
 *           "hspline" - natural cubic spline with Hyman Filter
 *           [Optional field, default = "linear"]
 *   \arg \a InterpOn interpolation seed (i.e. the underlying of the
 *           interpolation):
 *           "r"  - interest rate.
 *           "rt" - interpolated rate by time to maturity.
 *           "d"  - discount.
 *           [Optional field, default= "r"]
 *   \arg \a Comp interpolated rate compounding method:
 *           "cont"   - continuously compounded.
 *           "annual" - annual compounded.
 *           "simple" - linear.
 *           [Optional field, default= "cont"]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           9 - 1/1
 *           [Optional field, default= ACT/365]
 *   \arg \a CustomFields an array of names referring to optional customizable fields, see below
 *   \arg \a CustomValues an array of names referring to optional customizable fields, see below
 *
 *   \section customdetails CustomFields and CustomValues
 *   \arg \a stub_rate stub rate in % units, used only if FutBlockConsFlag="stub" (default = 0)   
 *   \arg \a swap_fl_freq the frequency of swap floating leg payments. If missing or zero, a
 *           currency-based default frequency is used (e.g. semiannually for EUR)    
 *           [Optional field, if missing currency default is used, see cSwapConvention.cpp]
 *   \arg \a swap_first_cpn the rate of the first fixing of the swap floating leg. If zero/missing is neglected (% units)
 *
 * \section Returns
 *   A two columns Excel range with maturities and discounts.
 *
 */

LPXLOPER EXCEL_EXPORT xlLiborCurveCustom(XlfOper xlToday, XlfOper xlCurrency, XlfOper xlCurveName, 
                                         XlfOper xlNDepo, XlfOper xlDeposits,
                                         XlfOper xlNFutu, XlfOper xlFutures, 
                                         XlfOper xlNSwap, XlfOper xlSwaps, 
                                         XlfOper xlFutEndDate, XlfOper xlMatBlockConsFlag, XlfOper xlFutBlockConsFlag, XlfOper xlBlockOrder, 
                                         XlfOper xlInterpType, XlfOper xlInterpOn, XlfOper xlComp, XlfOper xlDayCount, 
                                         XlfOper xlCustomFields, XlfOper xlCustomValues)
{
   EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");

   //currency and its handle
   std::string curName(xlCurrency.AsString());
   long hCurr;   
   pdgerr_t res = pdg_curStringToHandle(curName.c_str(), &hCurr);
   if(res.code) return XlfOper(res.des);
   
   //curve name
   std::string curveName(xlCurveName.AsString());

   // Depos
   long NDepo = xloper_cast<long>(xlNDepo);
   if(NDepo < 0) return XlfOper("Invalid number of depos");
   
   XlfRef Deposits = xlDeposits.AsRef();
   if((Deposits.GetNbCols() != 3) || (Deposits.GetNbRows() < NDepo))
      return XlfOper("Deposits data range of inconsistent size");
   
   std::vector<long>   depoLen(NDepo);
   std::vector<char>   depoType(NDepo);
   std::vector<double> depoRate(NDepo); 
   for (long i = 0; i < NDepo; ++i) {
         depoLen.at(i)  = static_cast<long>(1.e-9 + Deposits(i, 0).AsDouble());
         depoType.at(i) = Deposits(i, 1).AsString()[0];
         depoRate.at(i) = 0.01 * Deposits(i, 2).AsDouble();
   }

   
   // FRA/Futures
   long NFutu = xloper_cast<long>(xlNFutu);
   if (NFutu < 0) return XlfOper("Invalid number of futures");
   
   XlfRef Futures = xlFutures.AsRef();
   if ((Futures.GetNbCols() != 2) || (Futures.GetNbRows() < NFutu))
      return XlfOper("Futures data range of inconsistent size");
   
   std::vector<long>   futDate(NFutu);
   std::vector<double> futPrice(NFutu);
   for (long i = 0; i < NFutu; ++i) {
         futDate.at(i) = static_cast<long>(1.e-9 + Futures(i,0).AsDouble());
         futPrice.at(i) = 0.01 * Futures(i,1).AsDouble();
   }
   
   // Swaps
   long NSwap = xloper_cast<long>(xlNSwap);
   if (NSwap < 0 ) return XlfOper("Invalid number of swaps");
   
   XlfRef Swaps = xlSwaps.AsRef();
   if ((Swaps.GetNbCols() != 2) || (Swaps.GetNbRows() < NSwap))
      return XlfOper("Swaps data range of inconsistent size");
   
   std::vector<long> swapLen(NSwap);
   std::vector<double> swapRate(NSwap);
   for (long i = 0; i < NSwap; ++i) {
         swapLen.at(i) = static_cast<long>(1.e-9 + Swaps(i,0).AsDouble());
         swapRate.at(i) = 0.01 * Swaps(i,1).AsDouble();
   }
  
   // Interpolation parameters
   long interpType;
   if (xlInterpType.IsMissing() || xlInterpType.AsString()[0] == 0) interpType = 1;
   else {
      res = pdg_getInterpMethod(xlInterpType.AsString(), &interpType);
      if(res.code) return XlfOper(res.des);
   } 
   
   long interpOn;
   if (xlInterpOn.IsMissing() || xlInterpOn.AsString()[0] == 0) interpOn = 1;
   else {
      res = pdg_getInterpSeed(xlInterpOn.AsString(), &interpOn);
      if(res.code) return XlfOper(res.des);
   }
   
   long comp;
   if (xlComp.IsMissing() || xlComp.AsString()[0] == 0) comp = 3;
   else {
      res = pdg_getCompoundingMethod(xlComp.AsString(), &comp);
      if(res.code) return XlfOper(res.des);
   }
   
   // Day Count convention
   long dayCount = (xlDayCount.IsMissing() || xlDayCount.AsString()[0] == 0) ? 5 : (long)xlDayCount.AsDouble();

   //Customizable Fields
   std::vector<std::string> optNames;
   xloper_cast<std::vector<std::string> >(xlCustomFields, optNames);
   std::vector<double> optVals;
   xloper_cast<std::vector<double> >(xlCustomValues, optVals);
   if(optNames.size()!=optVals.size())
      return XlfOper("#Error, custom name and value vectors have different sizes");

   // Backward compatibility of XL interface: 
   //   -convert some explicit parameters into custom fields/vals
   //   -change names of some custom fields
   std::string futED = (xlFutEndDate.IsMissing() || xlFutEndDate.AsString()[0] == 0)? "standard" :  xlFutEndDate.AsString(); // Default value is "standard"
   optNames.push_back("FutEndDate#"+futED);
   optVals.push_back(0);

   std::string matBC = (xlMatBlockConsFlag.IsMissing() || xlMatBlockConsFlag.AsString() == 0)? "on" :  xlMatBlockConsFlag.AsString(); // Default value is "on"
   optNames.push_back("MatBlockCons#"+matBC);
   optVals.push_back(0);

   std::string futBC = (xlFutBlockConsFlag.IsMissing() || xlFutBlockConsFlag.AsString()[0] == 0)? "start" : xlFutBlockConsFlag.AsString(); // Default value is "start"
   optNames.push_back("FutBlockCons#"+futBC);
   optVals.push_back(0);
   
   std::string blockOrd;
   if(xlBlockOrder.IsMissing() || xlBlockOrder.AsString()[0] == 0) {
       blockOrd = "2341"; 
   }
   else {
       //convert old-style blockOrd (3 args) to new version 
       //(where 2="FRA", here unsupported) 
       blockOrd = xlBlockOrder.AsString();
       for(size_t i = 0; i < blockOrd.size(); ++i) {
           if(blockOrd[i] == '2') blockOrd[i] = '3';
           else if(blockOrd[i] == '3') blockOrd[i] = '4';
       }
   }
   optNames.push_back("BlockOrder#"+blockOrd);
   optVals.push_back(0);

   for(size_t i=0, imax = optNames.size(); i<imax ; ++i){
      std::string& name = optNames.at(i);
      if (name == "stub_rate") name = "FutBlockStubRate";
      if (name == "swap_fl_freq") name = "SwapFltFreq";
      if (name == "swap_first_cpn") name = "SwapFirstCpn";
   }

      //all rates are passed through in natural units
   for(size_t i=0, imax = optNames.size(); i<imax ; ++i){
      const std::string& name = optNames.at(i);
      if (name == "FutBlockStubRate" || name == "SwapFirstCpn")
         optVals.at(i)*=0.01;
   }

   //convert optnames to char**
   std::vector<const char *> rnv_c;
   for(size_t i=0, imax = optNames.size(); i<imax; ++i){
      rnv_c.push_back(optNames.at(i).c_str());
   }

   // Discount function arrays
   long discSize = 1 /* Today */ + NDepo + NFutu + NSwap + 1 /*place for (optional) stub date*/;
   std::vector<long>  discDates(discSize);
   std::vector<double> discVals(discSize);
   
   res = pdg_liborCurveCustom((long)xlToday.AsDouble(),
                              hCurr,
                              curveName.c_str(),
                              NDepo,
                              safe_begin(depoType),
                              safe_begin(depoLen),
                              safe_begin(depoRate),
                              0, //does not support FRA block
                              NULL, //does not support FRA block
                              NULL, //does not support FRA block
                              NFutu,
                              safe_begin(futDate),
                              safe_begin(futPrice),
                              NSwap,
                              safe_begin(swapLen),
                              safe_begin(swapRate),
                              interpType,
                              interpOn,
                              comp,
                              dayCount,
                              optNames.size(),
                              safe_begin(rnv_c),
                              safe_begin(optVals),
                              &discSize,
                              safe_begin(discDates),
                              safe_begin(discVals)
                              );
   
   if(res.code) return XlfOper(res.des);
   
   const long outCols = 2;
   std::vector<double> OutVals(discSize*outCols);
   for (size_t i = 0; i < discSize; ++i) {
      OutVals.at(i*outCols) = discDates.at(i);
      OutVals.at(i*outCols+1) = discVals.at(i);
   }
   
   return XlfOper(discSize,outCols,safe_begin(OutVals));
                     
   EXCEL_END;
}

/** \page xlccbasiscurvebootstrapper_page xlCCBasisCurveBootstrapper
 *
 *  Bootstrap an effective cross-currency (yield) curve from deposits, FRA/Futures and cross-currency swaps.
 *  Deposits and FRA/futures quotes are fictitious and are to be built from FX forwards and other market data in an effective-discount approach.
 *  The bootstrapper needs as inputs the domestic discounting and forwarding curve as well as the foreign forwarding curve,
 *  which have to be exogenously bootstrapped. The result of the calibration is the effective foreign curve.
 *  The output is a two-column matrix containing a vertical array of
 *  dates and a vertical array of discounts/zero rates. The bootstrapped curve is 
 *  stored in shared memory (by the curve name).
 *  An array of custom fields allows to play with a variety of bootstrapping options/settings, as listed below.
 * \section Syntax
 *   xlCCBasisCurveBootstrapper(CurveName, Today, DomCurrency, ForCurrency, SpotFXDom2For 
 *                       NDepo, Deposits, 
 *                       NFxSwap, FxSwaps, 
 *                       NCCSwap, CCSwaps,
 *                       InterpType, InterpOn, Comp, DayCount,
 *                       CustomFields, CustomValues,
 * 							 DummyReCalc)
 *
 * \section parameters Parameters
 *   \arg \a CurveName is the name of the foreign effective-discount curve to be built.
 *   \arg \a Today today date.
 *   \arg \a DomCurrency is the domestic economy currency.
 *   \arg \a ForCurrency is the foreign economy currency.
 *   \arg \a SpotFXDom2For converts domestic amounts to foreign currency.
 *           [optional, default=1]
 *   \arg \a NDepo number of deposits.
 *   \arg \a Deposits three-column array containing the deposit data:
 *           1st column and  2nd column contain information about the start and
 *           the end date of the contract; the 3rd column contains the rate in
 *           percentage terms (i.e. 3% = 3).
 *           1st column:
 *             [integer number] - number of calendar days/weeks/months or a date,
 *             according to deposit type as specified in the first column of
 *             corresponding element.
 *             [empty] - for spot-next, tom-next, overnight  deposit contract.
 *           2nd column:
 *             m - month deposit.
 *             w - week deposit.
 *             s - spot (for spot-next contract).
 *             t - tom (for tom-next contract).
 *             o - overnight (for over-night contract).
 *             d - calendar days deposit.
 *             c - custom deposit date.
 *             e - EONIA deposit (covers depos with maturities > 1Y).
 *   \arg \a NFxSwap number of fx swaps.
 *   \arg \a FxSwaps three-column array containing the fx swap data:
 *           1st column and  2nd column contain information about the start and
 *           the end date of the contract; the 3rd column contains the fx points
 *           in figures (cents of spot unit of measure).
 *           1st column:
 *             [integer number] - number of calendar days/weeks/months or a date,
 *             according to deposit type as specified in the first column of
 *             corresponding element.
 *           2nd column:
 *             m - months.
 *             w - weeks.
 *             y - years.
 *             d - calendar days.
 *             b - business days.
 *             c - custom delivery date.
 *   \arg \a NSwap number of euro-currency swaps.
 *   \arg \a Swaps two-column matrix containing the swap market data:
 *           1st column contains the Maturity of the swap (years to maturity, month, or date, according to optional field SwapMatType), 
 *           2nd column contains the swap rate in percentage terms (3% inserted as 3).
 *   \arg \a InterpType interpolation method:
 *           "linear" - linear interpolation
 *           "quadra" - quadratic interpolation
 *           "kruger" - monotonic cubic spline 
 *           "const"  - piece-wise constant
 *           "bspline" - natural basis spline
 *           "cspline" - natural cubic spline
 *           "hspline" - natural cubic spline with Hyman Filter
 *           [Optional field, default = "linear"]
 *   \arg \a InterpOn interpolation seed (i.e. the underlying of the
 *           interpolation):
 *           "r"  - interest rate.
 *           "rt" - interpolated rate by time to maturity.
 *           "d"  - discount.
 *           [Optional field, default= "r"]
 *   \arg \a Comp interpolated rate compounding method:
 *           "cont"   - continuously compounded.
 *           "annual" - annual compounded.
 *           "simple" - linear.
 *           [Optional field, default= "cont"]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           9 - 1/1
 *           [Optional field, default= ACT/365]
 *   \arg \a CustomFields an array of names referring to optional customizable fields, see below
 *   \arg \a CustomValues an array of names referring to optional customizable fields, see below
 *
 *   \arg \a DummyReCalc is a dummy optional parameter to trigger recalculation
 *
 *   \section customdetails CustomFields and CustomValues
 *
 *   \arg \a DomDiscCurveName is the name of the domestic discounting curve. The presence of such a curve is mandatory.
 *   \arg \a DomFwdCurveName is the name of the domestic forwarding curve. The presence of such a curve is mandatory.
 *   \arg \a ForFwdCurveName is the name of the foreign forwarding curve. The presence of such a curve is mandatory.

 *
 *   \arg \a MatBlockCons maturity block consistency flag. The block
 *           consistency refers to the fact that the maturities of deposit
 *           futures and swap often overlap; in this case we have to choose
 *           which instrument to consider and which one to put aside:
 *           "on"  - consistency on.
 *           "off" - consistency off.
 *           [Optional field, default = "on"]
 *
 *   \arg \a BlockOrder string specifying the instruments block order for block
 *           consistency selection (i.e. "213" means precedence given to
 *           fx swaps (# 2) then to the deposits (# 1) then to the swaps (# 3))
 *           [Optional field, default = "231"]
 *
 *   \arg \a FxSwapDom2For string ("yes" of "no") specifies if fx swap points
 *           are expressed as spreads on the domestic to forward exchange rate
 *           [Optional field, default = "yes"]
 *
 *   \arg \a CCDepoValDays is the number of value days of synthetic deposits (based on forward fx)[optional, if missing currency default is used, see cMarketConvention.cpp] 
 *
 *   \arg \a CCSValDays is the number of value days of CCS [optional, if missing currency default is used, see cMarketConvention.cpp] 
 *
 *   \arg \a CCSSpreadCurr is the name of the currency whose floating rates are to be charged of the spread [mandatory, if missing error is thrown]
 *
 *   \arg \a CCSM2MCurr is the name of the currency whose notional has to be reset at par every payment date [optional, default is empty string, which means no M2M] 
 *
 *   \arg \a CCSDomNotional is the notional of the domestic leg of CCS [optional, default = 100]
 *
 *   \arg \a CCSForNotional is the notional of the foreign leg of CCS  [optional, default =  (spotFX * DomNotional)]
 *
 *   \arg \a CCSDomMult is the multiplier of domestic currency floating rate payments [optional, default=1] 
 *
 *   \arg \a CCSForMult is the multiplier of foreign currency floating rate payments [optional, default=1] 
 *
 *   \arg \a CCSDomFreq the frequency of domestic currency floating leg payments. 
 *           [optional, if missing currency default is used, see cCCSwapConvention.cpp]
 *
 *   \arg \a CCSForFreq the frequency of foreign currency floating leg payments.  
 *           [optional, if missing currency default is used, see cCCSwapConvention.cpp]
 *
 *   \arg \a CCSDomYrf the day count convention of domestic  floating leg payments.  
 *           [optional, if missing currency default is used, see cCCSwapConvention.cpp]
 *
 *   \arg \a CCSForYrf the day count convention of foreign  floating leg payments.  
 *           [optional, if missing currency default is used, see cCCSwapConvention.cpp]
 *
 *   \arg \a CCSMatType the measure unit of swap maturities:
 *           0 if expressed in years to maturity
 *           1 if expressed in months
 *           2 if expressed as (XL) date 
 *           [optional, default 0]
 *
 *   \arg \a CCSDom1stRateFixed If positive, the floater rate for the 1st coupon is assumed to be known and read from the field
 *    "CCSDom1stRate", otherwise it is estimated on the forwarding curve. [optional, default=0]
 *   \arg \a CCSDom1stRate is the fixing of the floating rate for domestic leg first coupon. Effective only if the field 
 *    "CCSDom1stRateFixed" is positive; in this case it is mandatory to value this field.
 * 
 *   \arg \a CCSFor1stRateFixed If positive, the floater rate for the 1st coupon is assumed to be known and read from the field
 *    "CCSFor1stRate", otherwise it is estimated on the forwarding curve. [optional, default=0]
 *   \arg \a CCSFor1stRate is the fixing of the floating rate for foreign leg first coupon. Effective only if the field 
 *    "CCSFor1stRateFixed" is positive; in this case it is mandatory to value this field.
 *
 *   \arg \a OutputType triggers between discounts (0) or zero rates (1) [default is (0)]
 *
 * \section Returns
 *   A two columns Excel range with maturities and discounts/zero rates (according to OutputType option).
 *
 */

LPXLOPER EXCEL_EXPORT xlCCBasisCurveBootstrapper(XlfOper xlCurveName, XlfOper xlToday, 
																 XlfOper xlDomCurrency, XlfOper xlForCurrency, XlfOper xlSpotFXDom2For,
																 XlfOper xlNDepo, XlfOper xlDeposits, 
																 XlfOper xlNFxSwap, XlfOper xlFxSwaps, 
																 XlfOper xlNCCSwap, XlfOper xlCCSwaps,
																 XlfOper xlInterpType, XlfOper xlInterpOn, XlfOper xlComp, XlfOper xlDayCount, 
																 XlfOper xlCustomFields, XlfOper xlCustomValues,
																 XlfOper xlDummyReCalc)
{
   EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");
	
	pdgerr_t res;
   
	//currency (uppercased) and its handle
   std::string domCurName(xlDomCurrency.AsString());
   std::transform(domCurName.begin(), domCurName.end(), domCurName.begin(), toupper);
   long domCurr;   
   res = pdg_curStringToHandle(domCurName.c_str(), &domCurr);
   if(res.code) return XlfOper(res.des);
 
	std::string forCurName(xlForCurrency.AsString());
   std::transform(forCurName.begin(), forCurName.end(), forCurName.begin(), toupper);
   long forCurr;   
   res = pdg_curStringToHandle(forCurName.c_str(), &forCurr);
   if(res.code) return XlfOper(res.des);

   //name of the curve to be bootstrapped (uppercased)
   std::string curveName(xlCurveName.AsString());
   std::transform(curveName.begin(), curveName.end(), curveName.begin(), toupper);

	//spot fx
	double spotFXDom2For = xlSpotFXDom2For.IsMissing()? 1.0 : xlSpotFXDom2For.AsDouble();
  // Depos
   long NDepo = xloper_cast<long>(xlNDepo);
   if(NDepo < 0) return XlfOper("Invalid number of depos");
   
   XlfRef Deposits = xlDeposits.AsRef();
   if((Deposits.GetNbCols() != 3) || (Deposits.GetNbRows() < NDepo))
      return XlfOper("Deposits data range of inconsistent size");
   
   std::vector<long>   depoLen(NDepo);
   std::vector<char>   depoType(NDepo);
   std::vector<double> depoRate(NDepo); 
   for (long i = 0; i < NDepo; ++i) {
         depoLen.at(i)  = static_cast<long>(1.e-9 + Deposits(i, 0).AsDouble());
         depoType.at(i) = Deposits(i, 1).AsString()[0];
         depoRate.at(i) = 0.01 * Deposits(i, 2).AsDouble();
   }

   
   // FxSwaps
   long NFxSwap = xloper_cast<long>(xlNFxSwap);
   if (NFxSwap < 0) return XlfOper("Invalid number of fx swaps");
   
   XlfRef FxSwaps = xlFxSwaps.AsRef();
   if ((NFxSwap > 0) && (FxSwaps.GetNbCols() != 3 || FxSwaps.GetNbRows() < NFxSwap))
      return XlfOper("Fx swap data range of inconsistent size");
   
   std::vector<long>   fxSwapLen(NFxSwap);
   std::vector<char>   fxSwapType(NFxSwap);
   std::vector<double> fxPoints(NFxSwap); 
   for (long i = 0; i < NFxSwap; ++i) {
         fxSwapLen.at(i)  = static_cast<long>(1.e-9 + FxSwaps(i, 0).AsDouble());
         fxSwapType.at(i) = FxSwaps(i, 1).AsString()[0];
         fxPoints.at(i) = 0.01 * FxSwaps(i, 2).AsDouble();
   }

   // CCSwaps
   long NCCSwap = xloper_cast<long>(xlNCCSwap);
   if (NCCSwap < 0 ) return XlfOper("Invalid number of swaps");
   
   XlfRef CCSwaps = xlCCSwaps.AsRef();
   if ((CCSwaps.GetNbCols() != 2) || (CCSwaps.GetNbRows() < NCCSwap))
      return XlfOper("CCSwaps data range of inconsistent size");
   
   std::vector<long> swapLen(NCCSwap);
   std::vector<double> swapRate(NCCSwap);
   for (long i = 0; i < NCCSwap; ++i) {
         swapLen.at(i) = static_cast<long>(1.e-9 + CCSwaps(i,0).AsDouble());
         swapRate.at(i) = 0.01 * CCSwaps(i,1).AsDouble();
   }
  
   // Interpolation parameters
   long interpType;
   if (xlInterpType.IsMissing() || xlInterpType.AsString()[0] == 0) interpType = 1;
   else {
      res = pdg_getInterpMethod(xlInterpType.AsString(), &interpType);
      if(res.code) return XlfOper(res.des);
   } 
   
   long interpOn;
   if (xlInterpOn.IsMissing() || xlInterpOn.AsString()[0] == 0) interpOn = 1;
   else {
      res = pdg_getInterpSeed(xlInterpOn.AsString(), &interpOn);
      if(res.code) return XlfOper(res.des);
   }
   
   long comp;
   if (xlComp.IsMissing() || xlComp.AsString()[0] == 0) comp = 3;
   else {
      res = pdg_getCompoundingMethod(xlComp.AsString(), &comp);
      if(res.code) return XlfOper(res.des);
   }
   
   // Day Count convention
   long dayCount = (xlDayCount.IsMissing() || xlDayCount.AsString()[0] == 0) ? 5 : (long)xlDayCount.AsDouble();

   //Customizable Fields
   std::vector<std::string> optNames;
   xloper_cast<std::vector<std::string> >(xlCustomFields, optNames);
   std::vector<double> optVals;
   xloper_cast<std::vector<double> >(xlCustomValues, optVals);
   if(optNames.size()!=optVals.size())
      return XlfOper("#Error, custom name and value vectors have different sizes");

   //all rates are passed through in natural units
   for(size_t i=0, imax = optNames.size(); i<imax ; ++i){
      const std::string& name = optNames.at(i);
      if (name == "CCSDom1stRate" || name == "CCSFor1stRate")
         optVals.at(i)*=0.01;
   }
   //convert optnames to char**
   std::vector<const char *> rnv_c;
   for(size_t i=0, imax = optNames.size(); i<imax; ++i){
      rnv_c.push_back(optNames.at(i).c_str());
   }

   // Discount function arrays
   long discSize = 1 /* Today */ + NDepo + NFxSwap + NCCSwap + 1 /*place for (optional) stub date*/;
   std::vector<long>  discDates(discSize);
   std::vector<double> discVals(discSize);
   
   res = pdg_crossCurrencyBasisCurve(
                              curveName.c_str(),
                              (long)xlToday.AsDouble(),
                              domCurr,
                              forCurr,
                              spotFXDom2For,
                              NDepo,
                              safe_begin(depoType),
                              safe_begin(depoLen),
                              safe_begin(depoRate),
                              NFxSwap,
                              safe_begin(fxSwapType),
                              safe_begin(fxSwapLen),
                              safe_begin(fxPoints),
                              NCCSwap,
                              safe_begin(swapLen),
                              safe_begin(swapRate),
                              interpType,
                              interpOn,
                              comp,
                              dayCount,
                              optNames.size(),
                              safe_begin(rnv_c),
                              safe_begin(optVals),
                              &discSize,
                              safe_begin(discDates),
                              safe_begin(discVals)
                              );
   
   if(res.code) return XlfOper(res.des);
   
   const long outCols = 2;
   std::vector<double> OutVals(discSize*outCols);
   for (size_t i = 0; i < discSize; ++i) {
      OutVals.at(i*outCols) = discDates.at(i);
      OutVals.at(i*outCols+1) = discVals.at(i);
   }
   
   return XlfOper(discSize,outCols,safe_begin(OutVals));
                     
   EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlClearCurve(XlfOper xlCurveName)
{
  EXCEL_BEGIN;

  std::string liborName(xlCurveName.AsString());
  //uppercase all characters
  std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

  pdgerr_t res = pdg_clearShmCurveName(liborName.c_str());
  if(res.code) return XlfOper(res.des);

  return XlfOper(true);

  EXCEL_END;
}
  LPXLOPER EXCEL_EXPORT xlMergeFRAFut(XlfOper xlFutDates,XlfOper xlFutVals,
                                          XlfOper xlFRADates,XlfOper xlFRAVals, 
                                          XlfOper xlTenor,XlfOper xlCalCode,
                                          XlfOper xlFutFraCons,XlfOper xlESCons,XlfOper xlModFollow)
{
   EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");
 
   std::vector<long> futdates,FRAdates;
   std::vector<double> futvals,FRAvals;
   xloper_cast<std::vector<long> >(xlFutDates,futdates);
   xloper_cast<std::vector<long> >(xlFRADates,FRAdates);
   xloper_cast<std::vector<double> >(xlFutVals,futvals);
   xloper_cast<std::vector<double> >(xlFRAVals,FRAvals);
   
   long nfut =  futvals.size();
   long nFRA =  FRAvals.size();
   if(nfut!=futdates.size()) return XlfOper("Futures dates/values mismatch");
   if(nFRA!=FRAdates.size()) return XlfOper("FRA dates/values mismatch");
   
   long tenor = xloper_cast<long>(xlTenor);
   long futfracons = xlFutFraCons.IsMissing() ? 1 : xloper_cast<long>(xlFutFraCons);
   long escons = xlESCons.IsMissing() ? 1 : xloper_cast<long>(xlESCons);
   long calcode = xlCalCode.IsMissing() ? 0 : xloper_cast<long>(xlCalCode);
   long modfollow = xlModFollow.IsMissing() ? 1 : xloper_cast<long>(xlModFollow);

   long out_sz=0;
   std::vector<long> outdates(nfut+nFRA);
   std::vector<double> outvals(nfut+nFRA);
   
   pdgerr_t res = pdg_mergeFRAFut(safe_begin(futdates),safe_begin(futvals),nfut,
                                 safe_begin(FRAdates),safe_begin(FRAvals),nFRA,
                                  tenor,futfracons,escons,calcode,modfollow,
                                  &out_sz,safe_begin(outdates),safe_begin(outvals));
   
   if(out_sz < 1) return XlfOper("#Error: void output array");
   std::vector<double> out(2*out_sz); 
   for (size_t i = 0; i < out_sz; i++) {
    out[i*2] = outdates.at(i);
    out[i*2+1] = outvals.at(i);
  }

  return (res.code) ? XlfOper(res.des) : XlfOper(out_sz,2,&out[0]);
 EXCEL_END;
 }//xlMergeFRAFut


/** \page xlshmcurvelist_page xlShmCurveList
*
* Returns the list of curves registered in a shared memory singleton
*
* \section Syntax
*   xlShmCurveList(DummyReCalc)
*
* \section Parameters
*   \arg \a DummyReCalc is an optional dummy field useful to ensure safer recalculation behaviors. By forcing the function to depend
*   on other Cells, the user helps Excel to build a consistent recalculation graph.  
*
* \section Returns
*   the list of registered curves (upper case names)
*
*/
LPXLOPER EXCEL_EXPORT
xlShmCurveList(XlfOper xlDummyReCalc)
{
   EXCEL_BEGIN;
   if (XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Disabled while wizard is active");

   pdgerr_t res(RES_OK);
   std::vector<std::string> lst;
   std::vector<const char *> vec;
   try
   {
      lst  = libor_client::Instance().getCurveList();
      pdg::Assertion(lst.size()>0,"No registered curves.");

      for(std::vector<std::string>::const_iterator p = lst.begin(),e = lst.end(); p != e; ++p)
         vec.push_back(p->c_str());
   }
   catch(const pdg::Error & e) {res=e.getInfo();}
   catch(const std::exception & e) {res=pdg::Error(1,e.what()).getInfo();}
   catch(const XlfExceptionCoerce &) {res=pdg::Error(1,"invalid cell value (wrong type)").getInfo();}
   catch(...) {res=RES_FAIL;}

   return (res.code) ? XlfOper(res.des) : XlfOper(vec.size(),1,&vec[0]);

   EXCEL_END;
}

/** \page xlshminterpdisc_page xlShmInterpDisc
 *
 *   Returns the discount price relative to the libor curve LiborName at the
 *   OutDate. The discount curve is not provided in input because it is read
 *   from the "shared memory" (this is the reason for the function suffix
 *   "shm"). If the function provide the message "Requested Libor curve in
 *   shared memory not available" it means that the shared relative to that
 *   particular libor name is empty. Note that the all the Excel sections
 *   share the same memory.
 *
 * \section Syntax
 *   xlShmInterpDisc(LiborName, OutDate)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of libor curve.
 *   \arg \a OutDate is the date at which the discount price will be computed.
 *
 * \section Returns
 *   An Excel number.
 *
 * \section Example
 *   LiborName = "eur"
 *   OutDate = 30 jun 2005
 *
 *   disc = xlShmInterpDisc(LiborName, OutDate)
 *   yields disc = 0.98562
 *
 * \sa
 *   \ref xlliborcurvecustom_page ,
 *   \ref xlcurvebootstrapper_page ,
 *   \ref xlpushshmliborcurve_page , 
 *   \ref xlpushshmliborcurvename_page , 
 *   \ref xlshmimplswap_page
 *
 */
LPXLOPER EXCEL_EXPORT xlShmInterpDisc(XlfOper xlCurveName, XlfOper xlOutDate)
{
   EXCEL_BEGIN;

   if(xlOutDate.IsMissing()) return XlfOper("outDate missing.");
   long r(xlOutDate.GetArrayRows()), c(xlOutDate.GetArrayCols());
   long outSz = r*c;
   std::vector<long> outDate(outSz);
   long i, j;
   for(i = 0; i < r; ++i)
      for(j = 0; j < c; ++j)
         outDate[i * c + j] = xlOutDate(i, j).AsLong();

   std::vector<double> outDisc(outSz);

   // Libor name to handle
   long hLibor;

   std::string liborName(xlCurveName.AsString());
   //uppercase all characters
   std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

   pdgerr_t res = pdg_liborStringToHandle(liborName.c_str(), &hLibor);
   if(res.code) return XlfOper(res.des);

   res = pdg_shmInterpDisc(hLibor, outSz, &(outDate[0]), &(outDisc[0]));
   if(res.code) return XlfOper(res.des);

   return XlfOper(outSz, 1, static_cast<double *>(&(outDisc[0])));

   EXCEL_END;
}

/** \page xlshminterprate_page xlShmInterpRate
 *
 *   Returns the rate relative to the libor curve LiborName at the
 *   OutDate. The discount curve is not provided in input because it is read
 *   from the "shared memory" (this is the reason for the function suffix
 *   "shm"). If the function provide the message "Requested Libor curve in
 *   shared memory not available" it means that the shared relative to that
 *   particular libor name is empty. Note that the all the Excel sections
 *   share the same memory.
 *
 * \section Syntax
 *   xlShmInterpRate(LiborName, Comp, DayCount, OutDate)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of libor curve.
 *   \arg \a Comp is the compounding convention.
 *           0 - Linear.
 *           1 - Annual.
 *           2 - Continous.
 *   \arg \a DayCount is the day count convention.
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *   \arg \a OutDate is the date at which the discount price will be computed.
 *
 * \section Returns
 *   An Excel range.
 *
 * \sa
 *   \ref xlliborcurvecustom_page ,
 *   \ref xlcurvebootstrapper_page ,
 *   \ref xlpushshmliborcurve_page , 
 *   \ref xlpushshmliborcurvename_page , 
 *   \ref xlshmimplswap_page
 *
 */
LPXLOPER EXCEL_EXPORT xlShmInterpRate(XlfOper xlCurveName, XlfOper xlComp, XlfOper xlDayCount, XlfOper xlOutDate)
{
   EXCEL_BEGIN;

   if(xlOutDate.IsMissing()) return XlfOper("outDate missing.");
   long r(xlOutDate.GetArrayRows()), c(xlOutDate.GetArrayCols());
   long outSz = r*c;
   std::vector<long> outDate(outSz);
   long i, j;
   for(i = 0; i < r; ++i)
      for(j = 0; j < c; ++j)
         outDate[i * c + j] = xlOutDate(i, j).AsLong();

   std::vector<double> outRate(outSz);

   // Libor name to handle
   long hLibor;

   std::string liborName(xlCurveName.AsString());
   //uppercase all characters
   std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

   pdgerr_t res = pdg_liborStringToHandle(liborName.c_str(), &hLibor);
   if(res.code) return XlfOper(res.des);

   res = pdg_shmInterpRate(hLibor, xlComp.AsLong(), xlDayCount.AsLong(), outSz, &(outDate[0]), &(outRate[0]));
   if(res.code) return XlfOper(res.des);

   return XlfOper(outSz, 1, static_cast<double *>(&(outRate[0])));

   EXCEL_END;
}

/** \page xlshminterprate_page xlShmInterpRate
 *
 *   Returns the forward rate relative to the libor curve LiborName at the
 *   FwdDate and OutDate. The discount curve is not provided in input because
 *   it is read from the "shared memory" (this is the reason for the function
 *   suffix "shm"). If the function provide the message "Requested Libor curve
 *   in shared memory not available" it means that the shared relative to that
 *   particular libor name is empty. Note that the all the Excel sections
 *   share the same memory.
 *
 * \section Syntax
 *   xlShmInterpRate(LiborName, Comp, DayCount, OutDate)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of libor curve.
 *   \arg \a Comp is the compounding convention.
 *           0 - Linear.
 *           1 - Annual.
 *           2 - Continous.
 *   \arg \a DayCount is the day count convention.
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *   \arg \a OutDate is the date at which the discount price will be computed.
 *
 * \section Returns
 *   An Excel range.
 *
 */
LPXLOPER EXCEL_EXPORT xlShmInterpRateFwd(XlfOper xlCurveName, XlfOper xlFwdDate, XlfOper xlComp, XlfOper xlDayCount, XlfOper xlOutDate)
{
   EXCEL_BEGIN;

   if(xlOutDate.IsMissing()) return XlfOper("outDate missing.");
   long r(xlOutDate.GetArrayRows()), c(xlOutDate.GetArrayCols());
   long outSz = r*c;
   std::vector<long> outDate(outSz);
   long i, j;
   for(i = 0; i < r; ++i)
      for(j = 0; j < c; ++j)
         outDate[i * c + j] = xlOutDate(i, j).AsLong();

   std::vector<double> outRate(outSz);

   // Libor name to handle
   long hLibor;

   std::string liborName(xlCurveName.AsString());
   //uppercase all characters
   std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

   pdgerr_t res = pdg_liborStringToHandle(liborName.c_str(), &hLibor);
   if(res.code) return XlfOper(res.des);

   res = pdg_shmInterpRateFwd(hLibor, xlFwdDate.AsLong(), xlComp.AsLong(), xlDayCount.AsLong(), outSz, &(outDate[0]), &(outRate[0]));
   if(res.code) return XlfOper(res.des);

   return XlfOper(outSz, 1, static_cast<double *>(&(outRate[0])));

   EXCEL_END;
}

/** \page xlshmimplswap_page xlShmImplSwap
 *
 *   Returns the swap rate implied in a single curve used both for discounting and forwarding. 
 *   Being a single curve function, assumes that the npv of a floating leg with natural payments
 *   does not depend on the funding source used (EUR3M, EUR6M,...) .
 *   The stub period, if any, is considered at the beginning. If the StartDate is after the first date
 *   in the libor curve the function calculate the forward swap. The maturity
 *   may be specified in two ways: 1) speciying a date in the Maturity field or
 *   2) specifying the number of months in the MatMonths field (Maturity is
 *   considered only if MatMonths is zero). The latter has precedence if
 *   both the field are specified. The last three fields ModFollow, AdjFlag
 *   and DayCount can be used to set the fix leg conventions.
 *
 * \section Syntax
 *   xlShmImplSwap(LiborName, StartDate, Maturity, MatMonths, Freq, ModFollow,
 *                 AdjFlag, DayCount, FirstCouponRate)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of libor curve.
 *   \arg \a StartDate is the swap start date.
 *   \arg \a Maturity is the swap maturity date.
 *   \arg \a MatMonths is the maturity of the swap or bond expressed in months,
 *           if nonzero has precedence over maturity
 *   \arg \a Freq is the payment frequency; if zero, instrument is assumed to
 *           be zero coupon
 *   \arg \a ModFollow is the modified-following convention for theoretical
 *           date adjustment:
 *           0            - no correction
 *           Other values - returns first working day before calculation date
 *                          if next working day is beyond end-of-month
 *           [Optional field, default = 1]
 *   \arg \a AdjFlag is the date adjustment for calculation of year fractions:
 *           0 - Unadjusted.
 *           1 - Adjusted.
 *           [Optional field, defaul t = 1]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           [Optional field, default = 4]
 *
 *   \arg \a FirstCoupon If not empty, is the rate of the first coupon in % units.
 *           [Optional field, default = leave empty]
 *
 * \section Returns
 *   An Excel number.
 *
 * \section Example
 *   LiborName = "eur"
 *   StartDate = 24 may 2000
 *   Maturity = 24 may 2010
 *   MatMonth = 0
 *   Freq = 2
 *   ModFollow = 0
 *   AdjFlag = 1
 *   DayCount = 5
 *
 *   rate = shmImplSwap(liborName, out_Date, maturity, mat_month, freq, mod_follow, flag_adj, day_count)
 *   yields rate = 0.056023.
 *
 * \sa
 *   \ref xlliborcurvecustom_page ,
 *   \ref xlcurvebootstrapper_page ,
 *   \ref xlpushshmliborcurve_page , 
 *   \ref xlpushshmliborcurvename_page , 
 *   \ref xlshminterpdisc_page,\n
 *   \ref xlshmimplswap2_page is a version supporting different curves for discounting and forwarding
 */
LPXLOPER EXCEL_EXPORT xlShmImplSwap(XlfOper xlCurveName, XlfOper xlStartDate, XlfOper xlMaturity, XlfOper xlMatMonths,
                                    XlfOper xlFreq, XlfOper xlModFollow, XlfOper xlAdjFlag, XlfOper xlDayCount, XlfOper xlFirstCoupon)
{
   EXCEL_BEGIN;

   // Libor name to handle
   long hLibor;
   pdgerr_t res = pdg_liborStringToHandle(xlCurveName.AsString(), &hLibor);
   if(res.code) return XlfOper(res.des);

   bool firstCouponFixed = !xlFirstCoupon.IsMissing();
	double firstCoupon =  firstCouponFixed ? xlFirstCoupon.AsDouble() / 100. : 0.;

   double implRate;
   res = pdg_shmImplSwap(hLibor,
                         xlStartDate.AsLong(),
                         xlMaturity.AsLong(),
                         xlMatMonths.AsLong(),
                         xlFreq.AsLong(),
                         xlModFollow.AsLong(),
                         xlAdjFlag.AsLong(),
                         xlDayCount.AsLong(),
                         firstCouponFixed, 
								 firstCoupon,
                         &implRate);

   return (res.code) ? XlfOper(res.des) : XlfOper(implRate);

   EXCEL_END;
}
/** \page xlshmimplswap2_page xlShmImplSwap2
 *
 *   Returns the swap rate implied in a multi curve setting (discounting different from forwarding).
 *   Assumes that the forwarding curve has been bootstrapped by means of the discounting curve.
 *   The stub period, if any, is considered at the beginning. If the StartDate is after the first date
 *   in the libor curve the function calculate the forward swap. The maturity
 *   can be specified in two ways: 1) specifying a date in the Maturity field or
 *   2) specifying the number of months in the MatMonths field (Maturity is
 *   considered only if MatMonths is zero). The latter has precedence if
 *   both the field are specified. The last three fields ModFollow, AdjFlag
 *   and DayCount can be used to set the fix leg conventions.
 *
 * \section Syntax
 *   xlShmImplSwap2(HomeCurve, FwdCurve, StartDate, Maturity, MatMonths, 
 *						 FixFreq, FixDayCC,FLoatFreq,FloatDayCC, ModFollow, AdjFlag, FirstCoupon)
 *
 * \section Parameters
 *   \arg \a HomeCurve is the name of home discounting curve.
 *   \arg \a FwdCurve is the name of forwarding curve.
 *   \arg \a StartDate is the swap start date.
 *   \arg \a Maturity is the swap maturity date.
 *   \arg \a MatMonths is the maturity of the swap or bond expressed in months,
 *           if nonzero has precedence over maturity
 *   \arg \a FixFreq is the fixed leg payment frequency; if zero, fixed leg is assumed to be zero coupon
 *   \arg \a FixDayCC fixed leg day count convention
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30/360
 *   \arg \a FloatFreq is the floating leg payment frequency; 
 *   \arg \a FloatDayCC floating leg leg day count convention
 *   \arg \a ModFollow is the modified-following convention for theoretical
 *           date adjustment:
 *           0            - no correction
 *           Other values - returns first working day before calculation date
 *                          if next working day is beyond end-of-month
 *   \arg \a AdjFlag is the date adjustment for calculation of year fractions:
 *           0 - Unadjusted.
 *           1 - Adjusted.
 *           [Optional field, default = 1]
 *
 * 
 *   \arg \a FirstCoupon If not empty, is the rate of the first coupon in % units.
 *           [Optional field, default = leave empty]
 *
 * \section Returns
 *   The implied swap rate.
 *
 *
 * \sa
 *   \ref xlliborcurvecustom_page ,
 *   \ref xlcurvebootstrapper_page ,
 *   \ref xlpushshmliborcurve_page , 
 *   \ref xlpushshmliborcurvename_page , 
 *   \ref xlshminterpdisc_page ,
 *   \ref xlshmimplswap_page
 *
 */
LPXLOPER EXCEL_EXPORT xlShmImplSwap2(XlfOper xlHomeCurve, XlfOper xlFwdCurve, 
												XlfOper xlStartDate, XlfOper xlMaturity, XlfOper xlMatMonths,
                                    XlfOper xlFixFreq, XlfOper xlFixDayCC,
                                    XlfOper xlFloatFreq,  XlfOper xlFloatDayCC,
												XlfOper xlModFollow, XlfOper xlAdjFlag, 
                                    XlfOper xlFirstCoupon)
{
   EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");

   // get handles to fwd and disc curves
   long hdisc,hfwd;
   pdgerr_t res = pdg_liborStringToHandle(xlHomeCurve.AsString(), &hdisc);
	if(res.code) return XlfOper(res.des);
   res = pdg_liborStringToHandle(xlFwdCurve.AsString(), &hfwd);
	if(res.code) return XlfOper(res.des);

   bool firstCouponFixed = !xlFirstCoupon.IsMissing();
	double firstCoupon =  firstCouponFixed ? xlFirstCoupon.AsDouble() / 100. : 0.;

   double implRate;
   res = pdg_shmImplSwap2(hdisc,hfwd,
                         xlStartDate.AsLong(),
                         xlMaturity.AsLong(),
                         xlMatMonths.AsLong(),
                         xlFixFreq.AsLong(),
                         xlFixDayCC.AsLong(),
                         xlFloatFreq.AsLong(),
                         xlFloatDayCC.AsLong(),
                         xlModFollow.AsLong(),
                         xlAdjFlag.AsLong(),
                         firstCouponFixed, 
								 firstCoupon,
                         &implRate);

   return (res.code) ? XlfOper(res.des) : XlfOper(implRate);

   EXCEL_END;
}

/** \page xlshmimplannuity_page xlShmImplAnnuity
 *
 *   Returns the (double-curve) swap-related annuity, with possibility to choose among cash or physical settlment style.
 *   \n Given a swap with start at \f$T_0,\f$ par rate \f$S\f$ and fixed leg payments at \f$T_1,\ldots,T_n,\f$
 *    physical and cash-settled annuities are defined as
 *   \f[
 *   Ap(t) := \sum_{i=1}^{n}\tau_iP(t,T_i) \qquad Ac(t):= P(t,T_0)\sum_{i=1}^{n}\tau(1+\tau S)^{-i}
 *   \f] 
 * \section Syntax
 *   xlShmImplAnnuity(HomeCurve, FwdCurve, StartDate, Maturity, MatMonths, 
 *						 FixFreq, FixDayCC,FLoatFreq,FloatDayCC, ModFollow, AdjFlag, FirstCoupon, CSFlag)
 *
  * \section Parameters
 *   \arg \a HomeCurve is the name of home discounting curve.
 *   \arg \a FwdCurve is the name of forwarding curve.
 *   \arg \a StartDate is the swap start date.
 *   \arg \a Maturity is the swap maturity date.
 *   \arg \a MatMonths is the maturity of the swap or bond expressed in months,
 *           if nonzero has precedence over maturity
 *   \arg \a FixFreq is the fixed leg payment frequency; if zero, fixed leg is assumed to be zero coupon
 *   \arg \a FixDayCC fixed leg day count convention
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30/360
 *   \arg \a FloatFreq is the floating leg payment frequency; 
 *   \arg \a FloatDayCC floating leg leg day count convention
 *   \arg \a ModFollow is the modified-following convention for theoretical
 *           date adjustment:
 *           0            - no correction
 *           Other values - returns first working day before calculation date
 *                          if next working day is beyond end-of-month
 *   \arg \a AdjFlag is the date adjustment for calculation of year fractions:
 *           0 - Unadjusted.
 *           1 - Adjusted.
 *           [Optional field, default = 1]
 *   \arg \a FirstCoupon If not empty, is the rate of the first coupon in % units.
 *           [Optional field, default = leave empty]
 *   \arg \a CSFlag triggers among Cash (1) or Physical (0) settlment annuity 
 * \section Returns
 *   The cash/physically settled annuity.
 */
   LPXLOPER EXCEL_EXPORT xlShmImplAnnuity(XlfOper xlHomeCurve, XlfOper xlFwdCurve, XlfOper xlStartDate, XlfOper xlMaturity, XlfOper xlMatMonths, 
      XlfOper xlFixFreq, XlfOper xlFixDayCC, XlfOper xlFloatFreq, XlfOper xlFloatDayCC, 
      XlfOper xlModFollow, XlfOper xlAdjFlag, XlfOper xlFirstCoupon, XlfOper xlCSFlag)
{
   EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");

  // get handles to fwd and disc curves
   long hdisc,hfwd;
   pdgerr_t res = pdg_liborStringToHandle(xlHomeCurve.AsString(), &hdisc);
   res = pdg_liborStringToHandle(xlFwdCurve.IsMissing() ? xlHomeCurve.AsString() : xlFwdCurve.AsString(), &hfwd);
	if(res.code) return XlfOper(res.des);

   bool firstCouponFixed = !xlFirstCoupon.IsMissing();
	double firstCoupon =  firstCouponFixed ? xlFirstCoupon.AsDouble() / 100. : 0.;

   double annuity;
   res = pdg_shmImplAnnuity(hdisc,hfwd,
                         xlStartDate.AsLong(),
                         xlMaturity.AsLong(),
                         xlMatMonths.AsLong(),
                         xlFixFreq.AsLong(),
                         xlFixDayCC.AsLong(),
                         xlFloatFreq.AsLong(),
                         xlFloatDayCC.AsLong(),
                         xlModFollow.AsLong(),
                         xlAdjFlag.AsLong(),
                         firstCouponFixed, 
								 firstCoupon,
                         xlCSFlag.AsLong(),
                         &annuity);


   return (res.code) ? XlfOper(res.des) : XlfOper(annuity);

   EXCEL_END;
}

/** \page xlshmswapnpv_page xlShmSwapNPV
  *
 *   Returns the NPV of a swap in a multi curve setting (discounting different from forwarding).
 *   Assumes that the forwarding curve has been bootstrapped by means of the discounting curve.
 *   The stub period, if any, is considered at the beginning. If the StartDate is after the first date
 *   in the libor curve the function calculate the forward swap rate. 
 *   The maturity can be specified in two ways: 1) specifying a date in the Maturity field or
 *   2) specifying the number of months in the MatMonths field (Maturity is
 *   considered only if MatMonths is zero). The latter has precedence if
 *   both the field are specified. 
 *
 * \section Syntax
 *   xlShmImplSwapNPV(HomeCurve, FwdCurve, 
 *                   StartDate, Maturity, MatMonths, 
 *						   FixRate,FixFreq, FixDayCC,
 *                   FLoatFreq,FloatDayCC, ModFollow, AdjFlag, 
 *                   FirstCoupon, Nominal)
 *
 * \section Parameters
 *   \arg \a HomeCurve is the name of home discounting curve.
 *   \arg \a FwdCurve is the name of forwarding curve.
 *   \arg \a StartDate is the swap start date.
 *   \arg \a Maturity is the swap maturity date.
 *   \arg \a MatMonths is the maturity of the swap or bond expressed in months,
 *           if nonzero has precedence over maturity
 *   \arg \a FixRate is the fixed leg rate in % units.
 *   \arg \a FixFreq is the fixed leg payment frequency; if zero, fixed leg is assumed to be zero coupon
 *   \arg \a FixDayCC fixed leg day count convention
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30/360
 *   \arg \a FloatFreq is the floating leg payment frequency; 
 *   \arg \a FloatDayCC floating leg leg day count convention
 *   \arg \a ModFollow is the modified-following convention for theoretical
 *           date adjustment:
 *           0            - no correction
 *           Other values - returns first working day before calculation date
 *                          if next working day is beyond end-of-month
 *   \arg \a AdjFlag is the date adjustment for calculation of year fractions:
 *           0 - Unadjusted.
 *           1 - Adjusted.
 *           [Optional field, default = 1]
  *   \arg \a FirstCoupon If not empty, is the rate of the first coupon in % units.
 *           [Optional field, default = leave empty]
 *   \arg \a Nominal is the amount of the swap.
 *
 * \section Returns
 *   An Excel number.
 *
 */
LPXLOPER EXCEL_EXPORT xlShmSwapNPV(XlfOper xlHomeCurve, XlfOper xlFwdCurve, 
                                   XlfOper xlStartDate, XlfOper xlMaturity, XlfOper xlMatMonths, 
                                   XlfOper xlFixRate, XlfOper xlFixFreq, XlfOper xlFixDayCC, 
                                   XlfOper xlFloatFreq, XlfOper xlFloatDayCC, 
                                   XlfOper xlModFollow, XlfOper xlAdjFlag, 
                                   XlfOper xlFirstCoupon, 
                                   XlfOper xlNominal)
{
  EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");
   
   // get handles to fwd and disc curves
   long hdisc,hfwd;
   pdgerr_t res = pdg_liborStringToHandle(xlHomeCurve.AsString(), &hdisc);
	if(res.code) return XlfOper(res.des);
   res = pdg_liborStringToHandle(xlFwdCurve.AsString(), &hfwd);
	if(res.code) return XlfOper(res.des);

   bool firstCouponFixed = !xlFirstCoupon.IsMissing();
	double firstCoupon =  firstCouponFixed ? xlFirstCoupon.AsDouble() / 100. : 0.;
   
	double rate = xlFixRate.AsDouble() / 100.;
	double nominal = xlNominal.IsMissing()? 100. : xlNominal.AsDouble(); 
   double npv;
   
	res = pdg_shmSwapNPV(hdisc,hfwd,
                         xlStartDate.AsLong(),
                         xlMaturity.AsLong(),
                         xlMatMonths.AsLong(),
                         rate,
                         xlFixFreq.AsLong(),
                         xlFixDayCC.AsLong(),
                         xlFloatFreq.AsLong(),
                         xlFloatDayCC.AsLong(),
                         xlModFollow.AsLong(),
                         xlAdjFlag.AsLong(),
                         firstCouponFixed, 
								 firstCoupon,
                         nominal,
                         &npv);

   if(res.code) return XlfOper(res.des);

   return XlfOper(1, 1, &npv);

  EXCEL_END;
}


/** \page xlshmccswapnpv_page xlShmCCSwapNPV
  *
 *   Returns the NPV of a Cross-Currency swap in a multi curve setting (discounting different from forwarding).
 *   The stub period, if any, is considered at the beginning. If the StartDate is after the first date
 *   in the libor curve the function calculate the forward starting swap npv. 
 *   The maturity can be specified in two ways: 1) speciying a date in the Maturity field or
 *   2) specifying the number of months in the MatMonths field (Maturity is
 *   considered only if MatMonths is zero). The latter has precedence if
 *   both the field are specified. 
 *
 * \section Syntax
 *   xlShmCCSwapNPV(StartDate, Maturity, MatMonths,
 *							DomDiscCurve, DomFwdCurve, ForDiscCurve, ForFwdCurve,
 *                   CCfreq, CCYrf, CCNotional, CCMult, CCSPread, CCAdjFlag, CCModFollow, CC1stCpnFixed, CC1stCpnRate, SpotFXDom2For, M2MCurr)
 *
 * \section Parameters
 *   \arg \a StartDate is the swap start date.
 *   \arg \a Maturity is the swap maturity date.
 *   \arg \a MatMonths is the maturity of the swap or bond expressed in months, if nonzero has precedence over maturity *   \arg \a DomDiscCurve is the name of domestic discounting curve.
 *   \arg \a DomDiscCurve is the name of domestic discounting curve.
 *   \arg \a DomFwdCurve is the name of domestic forwarding curve. 
 *   \arg \a ForDiscCurve is the name of foreign discounting curve.
 *   \arg \a ForFwdCurve is the name of foreign forwarding curve. 
 *   \arg \a CCFreq is a 1x2 array with domestic and foreign payment frequencies
 *   \arg \a CCYrf is a 1x2 array with domestic and foreign day count convention codes
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30/360
 *   \arg \a CCNotional is a 1x2 array with domestic and foreign leg notionals
 *   \arg \a CCMult is a 1x2 array with domestic and foreign rate multipliers
 *   \arg \a CCSpread is a 1x2 array with domestic and foreign rate spreads (0.5 stands for 0.5%)
 *   \arg \a CCAdjFlag is a 1x2 array with domestic and foreign adjustment flag:
 *           0 - Unadjusted.
 *           1 - Adjusted.
 *   \arg \a CCModFollow is a 1x2 array with domestic and foreign modified-following settings:
 *           0            - no correction
 *           Other values - returns first working day before calculation date
 *                          if next working day is beyond end-of-month
 *
 *   \arg \a CC1stCpnFixed is a 1x2 array. If entry is positive, the corresponding rate for first coupon is assumed to be known and read by the "CC1stCpnRate" field (if<=0 fixings are estimated)
 *   [optional, default=0]
 *
 *   \arg \a CC1stCpnRate is a 1x2 array with domestic and foreign fixings for the first coupons (5 stands for 5%).mandatory if the corresponding "CC1stCpnFixed" is positive.
 *
 *   \arg \a SpotFXDom2For is the fx rate converting domestic amounts into foreign currency
 *   [optional, default=1.0]
 *
 *   \arg \a M2MCurr is the name of the currency of the leg which is marked-to-market 
 *   [optional, default="" i.e. no m2m]
 *
 * \section Returns
 *   An Excel number.
 *
 */
LPXLOPER EXCEL_EXPORT xlShmCCSwapNPV(XlfOper xlStartDate, XlfOper xlMaturity, XlfOper xlMatMonths, XlfOper xlDomDiscCurve, XlfOper xlDomFwdCurve, XlfOper xlForDiscCurve, XlfOper xlForFwdCurve, 
							 XlfOper xlCCFreq, XlfOper xlCCYrf, XlfOper xlCCNotional, XlfOper xlCCMult, XlfOper xlCCSpread, XlfOper xlCCAdjFlag, XlfOper xlCCModFollow, XlfOper xlCC1stCpnFixed, XlfOper xlCC1stCpnRate, 
							 XlfOper xlSpotFXDom2For, XlfOper xlM2MCurr)
{
   EXCEL_BEGIN;
   if(XlfExcel::Instance().IsCalledByFuncWiz()) return XlfOper("Wizard!");
   pdgerr_t res;

	//get handles to shm curves
	long hDomDisc,hDomFwd,hForDisc,hForFwd;
   res = pdg_liborStringToHandle(xlDomDiscCurve.AsString(), &hDomDisc);
	if(res.code) return XlfOper(res.des);
   res = pdg_liborStringToHandle(xlDomFwdCurve.AsString(), &hDomFwd);
	if(res.code) return XlfOper(res.des);
   res = pdg_liborStringToHandle(xlForDiscCurve.AsString(), &hForDisc);
	if(res.code) return XlfOper(res.des);
   res = pdg_liborStringToHandle(xlForFwdCurve.AsString(), &hForFwd);
	if(res.code) return XlfOper(res.des);	
	
	//get today
   long today;
   res = pdg_shmToday(hDomDisc, &today);
   if(res.code) return XlfOper(res.des);

	std::string m2mCurrName = xlM2MCurr.IsMissing()? "" : xlM2MCurr.AsString();
	long m2mCurrCode = static_cast<long>(ccNAN);
	if(m2mCurrName.size()>0){
		res = pdg_curStringToHandle(m2mCurrName.c_str(), &m2mCurrCode);
		if(res.code) return XlfOper(res.des);
	}

	std::vector<long> ccfreq;
	xloper_cast<std::vector<long> >(xlCCFreq, ccfreq);
	if( ccfreq.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong freq array size");

	std::vector<long> ccyrf;
	xloper_cast<std::vector<long> >(xlCCYrf, ccyrf);
	if( ccyrf.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong yrfarray size");

	std::vector<long> ccadj;
	xloper_cast<std::vector<long> >(xlCCAdjFlag, ccadj);
   if( ccadj.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong AdjFlag array size");

	std::vector<long> ccmodfol;
	xloper_cast<std::vector<long> >(xlCCModFollow, ccmodfol);
   if( ccmodfol.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong ModFollow array size");

	std::vector<double> ccnot;
	xloper_cast<std::vector<double> >(xlCCNotional, ccnot);
	if( ccnot.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong notional array size");

	std::vector<double> ccmult;
	xloper_cast<std::vector<double> >(xlCCMult, ccmult);
	if( ccmult.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong multiplier array size");

	std::vector<double> ccspr;
	xloper_cast<std::vector<double> >(xlCCSpread, ccspr);
   if( ccspr.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong spread array size");


	std::vector<bool> cc1stratefixed;
	if(xlCC1stCpnFixed.IsMissing()){
		cc1stratefixed = std::vector<bool>(2,false);
	}else{
		xloper_cast<std::vector<bool> >(xlCC1stCpnFixed, cc1stratefixed);
	}
	if(cc1stratefixed.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong 1stcpnratefixed array size");

	std::vector<double> cc1strate;
	if(xlCC1stCpnRate.IsMissing()){
		cc1strate = std::vector<double>(2,0.);
	}else{
		xloper_cast<std::vector<double> >(xlCC1stCpnRate, cc1strate);
	}
	if( cc1strate.size()!=2) return XlfOper("#Error in xlShmCCSwapNPV, wrong 1stcouponrate array size");

	double npv;
	res = pdg_shmCCSwapNPV(
								today,
								xlStartDate.AsLong(),
								xlMaturity.AsLong(),
                        xlMatMonths.AsLong(),
								hDomDisc,
								hDomFwd,
								hForDisc,
								hForFwd,
                        ccfreq.at(0),
                        ccyrf.at(0),
								ccnot.at(0),
								ccmult.at(0),
								ccspr.at(0)/100.,
                        ccadj.at(0),
								ccmodfol.at(0),
								cc1stratefixed.at(0),
								cc1strate.at(0)/100.,
                        ccfreq.at(1),
                        ccyrf.at(1),
								ccnot.at(1),
								ccmult.at(1),
								ccspr.at(1)/100.,
                        ccadj.at(1),
								ccmodfol.at(1),
								cc1stratefixed.at(1),
								cc1strate.at(1)/100.,
								xlSpotFXDom2For.IsMissing() ? 1. : xlSpotFXDom2For.AsDouble(),
                        m2mCurrCode,
								&npv
								);

	return (res.code) ? XlfOper(res.des) : XlfOper(npv);

   EXCEL_END;
}

#include "ciDates.h"
#include "VectorManipulations.h"

LPXLOPER EXCEL_EXPORT xlSwapSensitivity2(XlfOper xlRefDay, XlfOper xlStartFixDate, XlfOper xlEndFixDate, XlfOper xlFixDayCount, XlfOper xlStartFloatDate, XlfOper xlEndFloatDate, XlfOper xlFloatDayCount, XlfOper xlStartLiborDate, XlfOper xlEndLiborDate, XlfOper xlLiborDayCount, XlfOper xlZeroRateDayCount, XlfOper xlFixPayZCB, XlfOper xlFloatPayZCB, XlfOper xlLiborStartZCB, XlfOper xlLiborEndZCB/**/)
{
    EXCEL_BEGIN;
    
/*    xlRefDay
    xlFixDayCount
    xlFloatDayCount
    xlLiborDayCount
*/
    
    try {


    double refDay      = xlRefDay.AsDouble();
    long FixDayCount   = xlFixDayCount.AsLong();
    long FloatDayCount = xlFloatDayCount.AsLong();
    long LiborDayCount = xlLiborDayCount.AsLong();
    long zeroRateDayCount = xlZeroRateDayCount.AsLong();


    std::vector< double > vecStartFixDate;
    xloper_cast< std::vector< double > >( xlStartFixDate, vecStartFixDate );

    std::vector< double > vecEndFixDate;
    xloper_cast< std::vector< double > >( xlEndFixDate, vecEndFixDate );


    std::vector< double > vecStartFloatDate;
    xloper_cast< std::vector< double > >( xlStartFloatDate, vecStartFloatDate );

    std::vector< double > vecEndFloatDate;
    xloper_cast< std::vector< double > >( xlEndFloatDate, vecEndFloatDate );

    std::vector< double > vecStartLiborDate;
    xloper_cast< std::vector< double > >( xlStartLiborDate, vecStartLiborDate );

    
    std::vector< double > vecEndLiborDate;
    xloper_cast< std::vector< double > >( xlEndLiborDate, vecEndLiborDate );

    std::vector< double > fix_pay_zcbond;
    xloper_cast< std::vector< double > >( xlFixPayZCB, fix_pay_zcbond );

    std::vector< double > float_pay_zcbond;
    xloper_cast< std::vector< double > >( xlFloatPayZCB, float_pay_zcbond );

    std::vector< double > libor_start_zcbond;
    xloper_cast< std::vector< double > >( xlLiborStartZCB, libor_start_zcbond );

    std::vector< double > libor_end_zcbond;
    xloper_cast< std::vector< double > >( xlLiborEndZCB, libor_end_zcbond );

    std::vector< double> fix_yfrac( vecStartFixDate.size() );
    std::vector< double> fix_pay_ttm( vecStartFixDate.size() );
    for( unsigned i = 0; i < fix_yfrac.size(); ++i ){
        
        double local_ret = -1.0;
        pdgerr_t res     = pdg_yrfDayCount((long)vecStartFixDate.at( i ),
                                           (long)vecEndFixDate.at( i ),
                                           FixDayCount,
                                           &local_ret);
        if(res.code) return XlfOper(res.des);
        fix_yfrac.at( i ) = local_ret;

        res               = pdg_yrfDayCount((long)refDay,
                                           (long)vecEndFixDate.at( i ),
                                           zeroRateDayCount,
                                           &local_ret);
        if(res.code) return XlfOper(res.des);
        fix_pay_ttm.at( i ) = local_ret;
        
    }

    std::vector< double> float_yfrac( vecStartFloatDate.size() );
    std::vector< double> float_pay_ttm( vecStartFloatDate.size() );
    for( unsigned i = 0; i < float_yfrac.size(); ++i ){
        
        double local_ret = -1.0;
        pdgerr_t res     = pdg_yrfDayCount((long)vecStartFloatDate.at( i ),
                                           (long)vecEndFloatDate.at( i ),
                                           FloatDayCount,
                                           &local_ret);
        if(res.code) return XlfOper(res.des);
        float_yfrac.at( i ) = local_ret;
        res                 = pdg_yrfDayCount((long)refDay,
                                           (long)vecEndFloatDate.at( i ),
                                           zeroRateDayCount,
                                           &local_ret);
        if(res.code) return XlfOper(res.des);
        float_pay_ttm.at( i ) = local_ret;
    }

    std::vector< double> libor_yfrac( vecStartLiborDate.size() );
    std::vector< double> libor_start_ttm( vecStartLiborDate.size() );
    std::vector< double> libor_end_ttm( vecStartLiborDate.size() );
    for( unsigned i = 0; i < libor_yfrac.size(); ++i ){
        
        double local_ret = -1.0;
        pdgerr_t res     = pdg_yrfDayCount((long)vecStartLiborDate.at( i ),
                                           (long)vecEndLiborDate.at( i ),
                                           LiborDayCount,
                                           &local_ret);
        if(res.code) return XlfOper(res.des);
        libor_yfrac.at( i ) = local_ret;
        res                 = pdg_yrfDayCount((long)refDay,
                                           (long)vecStartLiborDate.at( i ),
                                           zeroRateDayCount,
                                           &local_ret);
        if(res.code) return XlfOper(res.des);
        libor_start_ttm.at( i ) = local_ret;
        res                 = pdg_yrfDayCount((long)refDay,
                                           (long)vecEndLiborDate.at( i ),
                                           zeroRateDayCount,
                                           &local_ret);
        if(res.code) return XlfOper(res.des);
        libor_end_ttm.at( i ) = local_ret;
    }

    /// out: pillar belonging to the discount curve : a vector of dates obtained by merging fix_pay_ttm and float_pay_ttm vectors
    std::vector<double> discount_pillar;
    /// out: pillar belonging to the forwarding curve : a vector of dates obtained by merging libor_start_ttm and libor_end_ttm vectors
    std::vector<double> forwarding_pillar;
    //vector_manipulations::merge_increasing<double>(libor_start_ttm, libor_end_ttm, forwarding_pillar, pos12, pos22);
    
    /// out: sensitivity relative to the discount curve : a vector of values with the same size of discount_pillar vector
    std::vector<double> discount_rho/*(discount_pillar.size())*/;
    /// out: sensitivity relative to the forwarding curve : a vector of values with the same size of forwarding_pillar vector
    std::vector<double> forwarding_rho/*(forwarding_pillar.size())*/;


    libor::calculateSwapRateSensitivity( fix_yfrac, 
                                         float_yfrac, 
                                         libor_yfrac, 
                                         fix_pay_ttm, 
                                         float_pay_ttm, 
                                         libor_start_ttm, 
                                         libor_end_ttm, 
                                         fix_pay_zcbond, 
                                         float_pay_zcbond, 
                                         libor_start_zcbond, 
                                         libor_end_zcbond, 
                                         discount_pillar, 
                                         forwarding_pillar, 
                                         discount_rho, 
                                         forwarding_rho);

    int rowMatrixOutput = std::max(discount_pillar.size(), forwarding_pillar.size());
    std::matrix< double > output( rowMatrixOutput , 4 );

    for (int i = 0; i<discount_pillar.size(); ++i){

        output.at(i,0) = discount_pillar.at(i);
        output.at(i,1) = discount_rho.at(i);
    }

    for (int i = 0; i<forwarding_pillar.size(); ++i){

        output.at(i,2) = forwarding_pillar.at(i);
        output.at(i,3) = forwarding_rho.at(i);
    }

   
    return XlfOper( rowMatrixOutput, 4, output.begin() );

    }

   catch(pdg::Error e) {
      return XlfOper(e.getInfo().des);
   }
   catch(...) {
      return XlfOper(RES_FAIL.des);
   }

    EXCEL_END;
}

/** \page xlpushshmliborcurve_page xlPushShmLiborCurve
 *
 *  Fills the shared memory with the dates and discs and the other conventions
 *  passed by the function parameters. The first date is the today date and the
 *  first discount must be 1.
 *
 * \section Syntax
 *   xlPushShmLiborCurve(xlMktName, xlInDates, xlInDisc, xlInterpType,
 *                       xlInterpOn, xlComp, xlDayCount)
 *
 * \section Parameters
 *   \arg \a MktName is the currency name.
 *   \arg \a InDates is the vector of pillar dates.
 *   \arg \a InterpType interpolation method:
 *           "linear" - linear interpolation.
 *           "quadra" - quadratic interpolation.
 *           [Optional field, default = "linear"]
 *   \arg \a InterpOn interpolation seed (i.e. the underlying of the
 *           interpolation):
 *           "r"  - interest rate.
 *           "rt" - interpolated rate by time to maturity.
 *           "d"  - discount.
 *           [Optional field, default= "r"]
 *   \arg \a Comp interpolated rate compounding method:
 *           "cont"   - continuously compounded.
 *           "annual" - annual compounded.
 *           "simple" - linear.
 *           [Optional field, default= "cont"]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           [Optional field, default= 7]
 *
 * \section Returns
 *   True if is all right.
 *
 * \section Example
 *    MktName = "eur"
 *    InDates = 19-Aug-01
 *              21-Aug-01
 *              21-Sep-01
 *              21-Nov-01
 *              21-May-02
 *              22-Aug-02
 *              23-Aug-04
 *              21-Sep-05
 *              22-Aug-06
 *              22-Aug-08
 *              23-Aug-10
 *              22-Aug-13
 *              23-Aug-21
 *              22-Aug-31
 *              22-Aug-41
 *    InDisc =  1
 *              0.999884458
 *              0.998111727
 *              0.994562856
 *              0.984026901
 *              0.978189308
 *              0.928902348
 *              0.999884458
 *              0.872391251
 *              0.811398445
 *              0.74931744
 *              0.661023735
 *              0.468778157
 *              0.312090814
 *              0.210919852
 *    InterpType = "linear"
 *    InterpOn = "r"
 *    Comp = "annual"
 *    DayCount = 2
 *
 *    res  = xlPushShmLiborCurve(MktName, InDates, InDisc, InterpType,
 *                               InterpOn, Comp, DayCount)
 *
 *    yields res = true.
 *\sa
 *   \ref curvesetting_page
 *
 */
LPXLOPER EXCEL_EXPORT xlPushShmLiborCurve(XlfOper xlMktName, XlfOper xlInDates, XlfOper xlInDisc, XlfOper xlInterpType,
                                          XlfOper xlInterpOn, XlfOper xlComp, XlfOper xlDayCount)
{
   EXCEL_BEGIN;

   return xlPushShmLiborCurveName(xlMktName, xlMktName, xlInDates, xlInDisc,
                                  xlInterpType, xlInterpOn, xlComp, xlDayCount);

   EXCEL_END;
}

/** \page xlpushshmliborcurvename_page xlPushShmLiborCurveName
 *
 *  Fills the shared memory (by the libor name) with the dates and discs and the
 *  other conventions passed by the function parameters. The first date is the
 *  today date and the first discount must be 1.
 *
 * \section Syntax
 *   xlPushShmLiborCurveName(Currency, LiborName, InDates, InDisc,
                             InterpType, InterpOn, Comp, DayCount)
 *
 * \section Parameters
 *   \arg \a Currency is the currency name.
 *   \arg \a LiborName is the name of the libor curve.
 *   \arg \a InDates is the vector of pillar dates.
 *   \arg \a InterpType interpolation method:
 *           "linear" - linear interpolation.
 *           "quadra" - quadratic interpolation.
 *           [Optional field, default = "linear"]
 *   \arg \a InterpOn interpolation seed (i.e. the underlying of the
 *           interpolation):
 *           "r"  - interest rate.
 *           "rt" - interpolated rate by time to maturity.
 *           "d"  - discount.
 *           [Optional field, default= "r"]
 *   \arg \a Comp interpolated rate compounding method:
 *           "cont"   - continuously compounded.
 *           "annual" - annual compounded.
 *           "simple" - linear.
 *           [Optional field, default= "cont"]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           [Optional field, default= 7]
 *
 * \section Returns
 *   True if is all right.
 *
 * \section Example
 *    Currency = "eur"
 *    LiborName = "prova"
 *    InDates = 19-Aug-01
 *              21-Aug-01
 *              21-Sep-01
 *              21-Nov-01
 *              21-May-02
 *              22-Aug-02
 *              23-Aug-04
 *              21-Sep-05
 *              22-Aug-06
 *              22-Aug-08
 *              23-Aug-10
 *              22-Aug-13
 *              23-Aug-21
 *              22-Aug-31
 *              22-Aug-41
 *    InDisc =  1
 *              0.999884458
 *              0.998111727
 *              0.994562856
 *              0.984026901
 *              0.978189308
 *              0.928902348
 *              0.999884458
 *              0.872391251
 *              0.811398445
 *              0.74931744
 *              0.661023735
 *              0.468778157
 *              0.312090814
 *              0.210919852
 *    InterpType = "linear"
 *    InterpOn = "r"
 *    Comp = "annual"
 *    DayCount = 2
 *
 *    res  = xlPushShmLiborCurve(Currency, LiborName, InDates, InDisc,
 *                               InterpType, InterpOn, Comp, DayCount)
 *
 *    yields res = true.
 *\sa
 *   \ref curvesetting_page
 *
 */
LPXLOPER EXCEL_EXPORT xlPushShmLiborCurveName(XlfOper xlCurrency, XlfOper xlCurveName, XlfOper xlInDates, XlfOper xlInDisc, XlfOper xlInterpType,
                                              XlfOper xlInterpOn, XlfOper xlComp, XlfOper xlDayCount)
{
   EXCEL_BEGIN;

   std::string currency(xlCurrency.AsString());
   //uppercase all characters
   std::transform(currency.begin(), currency.end(), currency.begin(), toupper);

   std::string liborName(xlCurveName.AsString());
   //uppercase all characters
   std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

   pdgerr_t res;
   // Interpolation parameters
   long interpType;
   if (xlInterpType.IsMissing() || xlInterpType.AsString()[0] == 0) interpType = 1;
   else {
      res = pdg_getInterpMethod(xlInterpType.AsString(), &interpType);
      if(res.code) return XlfOper(res.des);
   }

   long interpOn;
   if (xlInterpOn.IsMissing() || xlInterpOn.AsString()[0] == 0) interpOn = 1;
   else {
      res = pdg_getInterpSeed(xlInterpOn.AsString(), &interpOn);
      if(res.code) return XlfOper(res.des);
   }

   long comp;
   if (xlComp.IsMissing() || xlComp.AsString()[0] == 0) comp = 3;
   else {
      res = pdg_getCompoundingMethod(xlComp.AsString(), &comp);
      if(res.code) return XlfOper(res.des);
   }

   // Day Count convention
   long dayCount = (xlDayCount.IsMissing() || xlDayCount.AsString()[0] == 0) ? 7 : static_cast<long>(xlDayCount.AsDouble());

   // In dates
   XlfRef InDates = xlInDates.AsRef();

   // In discs
   XlfRef InDiscs = xlInDisc.AsRef();

   if (InDates.GetNbRows() != InDiscs.GetNbRows()) return XlfOper("Number of dates differs from number of discounts.");

   long nRows = InDates.GetNbRows();

   std::vector<long> inDates(nRows);
   std::vector<double> inDiscs(nRows);

   long prev = -1;
   for(long i = 0; i < nRows; ++i) {
      inDates[i] = static_cast<long>(InDates(i,0).AsDouble());
      if (inDates[i] < prev)
         return XlfOper("Unordered term structure (" + liborName + ")") ;
      prev = inDates[i];
      inDiscs[i] = InDiscs(i,0).AsDouble();
   }

   res = pdg_pushShmLiborCurveName(currency.c_str(), liborName.c_str(), &inDates[0], &inDiscs[0], interpType, interpOn, comp, dayCount, nRows);
   if(res.code) return XlfOper(res.des);

   bool push = true;
   return XlfOper(1, 1, &push);

   EXCEL_END;
}

 /** \page xlpushshmliborspreadcurve_page xlPushShmLiborSpreadCurve
 *
 *  Fills the shared memory (by the libor name) with the dates and discs and the
 *  other conventions of a curve at spread on a backbone. The first date is the
 *  today date and the first discount must be 1.
 *
 * \section Syntax
 *   xlPushShmLiborSpreadCurve(Currency, LiborName, BackboneName, InDates, InDisc,
                             InterpType, InterpOn, Comp, DayCount)
 *
 * \section Parameters
 *   \arg \a Currency is the currency name.
 *   \arg \a LiborName is the name of the libor curve.
 *   \arg \a BackboneName is the name of the backbone curve.
 *   \arg \a InDates is the vector of pillar dates.
 *   \arg \a InterpType interpolation method:
 *           "linear" - linear interpolation.
 *           "quadra" - quadratic interpolation.
 *           [Optional field, default = "linear"]
 *   \arg \a InterpOn interpolation seed (i.e. the underlying of the
 *           interpolation):
 *           "rs"  - interest rate spread.
 *           "rts" - interpolated rate by time to maturity.
 *           "ds"  - discount spread.
 *           [Optional field, default= "rs"]
 *   \arg \a Comp interpolated rate compounding method:
 *           "cont"   - continuously compounded.
 *           "annual" - annual compounded.
 *           "simple" - linear.
 *           [Optional field, default= "cont"]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           [Optional field, default= 7]
 *
 * \section Returns
 *   True if is all right.
 *\sa
 *   \ref xlpushshmliborcurvename_page
 *
 */
LPXLOPER EXCEL_EXPORT xlPushShmLiborSpreadCurve(XlfOper xlCurrency, XlfOper xlCurveName, XlfOper xlBackboneName, XlfOper xlInDates, XlfOper xlInDisc, XlfOper xlInterpType,
                                                 XlfOper xlInterpOn, XlfOper xlComp, XlfOper xlDayCount)
{
   EXCEL_BEGIN;

   std::string currency(xlCurrency.AsString());
   //uppercase all characters
   std::transform(currency.begin(), currency.end(), currency.begin(), toupper);

   std::string liborName(xlCurveName.AsString());
   //uppercase all characters
   std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

   std::string backboneName(xlBackboneName.AsString());
   //uppercase all characters
   std::transform(backboneName.begin(), backboneName.end(), backboneName.begin(), toupper);

   pdgerr_t res;
   // Interpolation parameters
   long interpType;
   if (xlInterpType.IsMissing() || xlInterpType.AsString()[0] == 0) interpType = 1;
   else {
      res = pdg_getInterpMethod(xlInterpType.AsString(), &interpType);
      if(res.code) return XlfOper(res.des);
   }

   long interpOn;
   if (xlInterpOn.IsMissing() || xlInterpOn.AsString()[0] == 0) interpOn = 1;
   else {
      res = pdg_getInterpSeed(xlInterpOn.AsString(), &interpOn);
      if(res.code) return XlfOper(res.des);
   }

   long comp;
   if (xlComp.IsMissing() || xlComp.AsString()[0] == 0) comp = 3;
   else {
      res = pdg_getCompoundingMethod(xlComp.AsString(), &comp);
      if(res.code) return XlfOper(res.des);
   }

   // Day Count convention
   long dayCount = (xlDayCount.IsMissing() || xlDayCount.AsString()[0] == 0) ? 7 : static_cast<long>(xlDayCount.AsDouble());

   // In dates
   XlfRef InDates = xlInDates.AsRef();

   // In discs
   XlfRef InDiscs = xlInDisc.AsRef();

   if (InDates.GetNbRows() != InDiscs.GetNbRows()) return XlfOper("Number of dates differs from number of discounts.");

   long nRows = InDates.GetNbRows();

   std::vector<long> inDates(nRows);
   std::vector<double> inDiscs(nRows);

   long prev = -1;
   for(long i = 0; i < nRows; ++i) {
      inDates[i] = static_cast<long>(InDates(i,0).AsDouble());
      if (inDates[i] < prev)
         return XlfOper("Unordered term structure (" + liborName + ")") ;
      prev = inDates[i];
      inDiscs[i] = InDiscs(i,0).AsDouble();
   }

   res = pdg_pushShmLiborSpreadCurve(currency.c_str(), liborName.c_str(), backboneName.c_str(), nRows == 0 ? 0 : inDates[0], &inDates[0], &inDiscs[0], interpType, interpOn, comp, dayCount, nRows);
   if(res.code) return XlfOper(res.des);

   bool push = true;
   return XlfOper(1, 1, &push);

   EXCEL_END;
}

/** \page xlinterpdisc_page xlInterpDisc
 *
 *  Returns the discount price relative to the libor curve provided in input by
 *  the fields InDates and InDisc. he out_date. The interpolation parameters
 *  are set by default (see Arguments) but they can be changed by the user. The
 *  function make the same job as shmImplDisc but in this case the input
 *  discount curve must be provided in input; shmImplDisc instead find the
 *  discount reading the discount curve available in the shared memory.
 *
 * \section Syntax
 *   xlInterpDisc(InDates, InDisc, OutDates, InterpType, InterpOn, Comp,
 *                DayCount)
 *
 * \section Parameters
 *   \arg \a InDates is the input array of discount function dates; the first
 *           element is interpreted as today’s date.
 *   \arg \a InDisc is input array of discount function values; the first
 *           element must have a value of 1.0000
 *   \arg \a OutDates are the dates at which the discount prices will be computed.
 *   \arg \a InterpType interpolation method:
 *           "linear" - linear interpolation.
 *           "quadra" - quadratic interpolation.
 *           [Optional field, default = "linear"]
 *   \arg \a InterpOn interpolation seed (i.e. the underlying of the
 *           interpolation):
 *           "r"  - interest rate.
 *           "rt" - interpolated rate by time to maturity.
 *           "d"  - discount.
 *           [Optional field, default= "r"]
 *   \arg \a Comp interpolated rate compounding method:
 *           "cont"   - continuously compounded.
 *           "annual" - annual compounded.
 *           "simple" - linear.
 *           [Optional field, default= "cont"]
 *   \arg \a DayCount interpolated rate day count convention flag:
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360
 *           [Optional field, default= 7]
 *
 * \section Returns
 *   An Excel range.
 *\sa
 *   \ref curvesetting_page
 *
 */
LPXLOPER EXCEL_EXPORT xlInterpDisc(XlfOper xlInDates, XlfOper xlInDisc, XlfOper xlOutDates, XlfOper xlInterpType,
                                   XlfOper xlInterpOn, XlfOper xlComp, XlfOper xlDayCount)
{
  EXCEL_BEGIN;

  long i;

  XlfRef InDates = xlInDates.AsRef();
  XlfRef InDiscs = xlInDisc.AsRef();
  long in_sz = InDates.GetNbRows();

  std::vector<long> in_date(in_sz);
  std::vector<double> in_disc(in_sz);
  for(i = 0; i < in_sz; ++i) {
    in_date[i] = InDates(i,0).AsLong();
    in_disc[i] = InDiscs(i,0).AsDouble();
  }

  XlfRef OutDates = xlOutDates.AsRef();
  long out_sz = OutDates.GetNbRows();
  std::vector<long> out_date(out_sz);
  std::vector<double> out_disc(out_sz);
  for(i = 0; i < out_sz; ++i) out_date[i] = OutDates(i,0).AsLong();

  // Interpolation parameters
  pdgerr_t res;
  long interpType;
  if (xlInterpType.IsMissing() || xlInterpType.AsString()[0] == 0) interpType = 1;
  else {
    res = pdg_getInterpMethod(xlInterpType.AsString(), &interpType);
    if(res.code) return XlfOper(res.des);
  }

  long interpOn;
  if (xlInterpOn.IsMissing() || xlInterpOn.AsString()[0] == 0) interpOn = 1;
  else {
    res = pdg_getInterpSeed(xlInterpOn.AsString(), &interpOn);
    if(res.code) return XlfOper(res.des);
  }

  long comp;
  if (xlComp.IsMissing() || xlComp.AsString()[0] == 0) comp = 2;
  else {
    res = pdg_getCompoundingMethod(xlComp.AsString(), &comp);
    if(res.code) return XlfOper(res.des);
  }

  // Day Count convention
  long dayCount = (xlDayCount.IsMissing() || xlDayCount.AsString()[0] == 0) ? 4 : (long)xlDayCount.AsDouble();

  res = pdg_interpDisc(in_sz, &(in_date[0]), &(in_disc[0]), out_sz, &(out_date[0]), &(out_disc[0]),
                       interpType, interpOn, comp, dayCount);
  if(res.code) return XlfOper(res.des);

  return XlfOper(out_sz, 1, &(out_disc[0]));

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlMktValueDate(XlfOper xlMktName, XlfOper xlToday)
{
  EXCEL_BEGIN;

   long dv;
   pdgerr_t res = pdg_marketValueDate(xlMktName.AsString(), xlToday.AsLong(), &dv);
   if(res.code) return XlfOper(res.des);

   return XlfOper(dv);

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlRateInterpSolver(XlfOper xlRates, XlfOper xlTimes, XlfOper xlRateSpread, XlfOper xlStartValues, XlfOper xlParsToCalibrate, XlfOper xlNMin)
{
  EXCEL_BEGIN;

   if(xlRates.IsMissing()) return XlfOper("Input Rates are missing.");
   if(xlTimes.IsMissing()) return XlfOper("Input times are missing.");

   long nMin = xlNMin.IsMissing()? 1 : xlNMin.AsDouble();
   if(nMin>10) return XlfOper("Number of minimization can not be greater than 10.");
   long r(xlRates.GetArrayRows());
   long c(xlRates.GetArrayCols());
   if(r!=xlTimes.GetArrayRows() || c!=xlTimes.GetArrayCols())
      return XlfOper("rates and times vector must have the same size.");
  
   std::vector<double> rates(r*c);
   std::vector<double> times(r*c);
   for(long i=0; i<r; i++)
      for(long j=0; j<c; j++){
         rates[i*c+j] = xlRates(i,j).AsDouble();
         times[i*c+j] = xlTimes(i,j).AsDouble();
      }
   std::vector<double> spread(r*c);
   if(xlRateSpread.IsMissing())
      for(long i=0; i<r*c; i++) spread[i]=0;
   else {
      if(xlRateSpread.GetArrayRows()!=r || xlRateSpread.GetArrayCols()!=c)
         return XlfOper("Error in rate/spread value dimention.");
      else
         for(long i=0; i<r; i++)
            for(long j=0; j<c; j++)
               spread[i*c+j] = xlRateSpread(i,j).AsDouble();
   }
   const long sz = 4;
   std::vector<double> startVal(sz);
   bool parToCal[sz];//cannot use std::vector<bool> as it is converted by STDLibrary into an array of bits

   if(xlStartValues.IsMissing()){
      for(long i=0; i<sz; i++) startVal[i]=0;
      if(!xlParsToCalibrate.IsMissing())
         return XlfOper("Start values must be specified.");
      else
         for(long i=0; i<sz; i++) parToCal[i]=true;
   } else {
      if(xlStartValues.GetArrayCols()*xlStartValues.GetArrayRows()!=sz)
         return XlfOper("Error in start value dimention.");
      else
         for(long i=0; i<xlStartValues.GetArrayRows(); i++)
            for(long j=0; j<xlStartValues.GetArrayCols(); j++)
               startVal[i*xlStartValues.GetArrayCols()+j] = xlStartValues(i,j).AsDouble();
      if(xlParsToCalibrate.IsMissing())
         for(long i=0; i<sz; i++) parToCal[i]=true;
      else {
         if(xlParsToCalibrate.GetArrayCols()*xlParsToCalibrate.GetArrayRows()!=sz)
            return XlfOper("Error in parameters to calibrate dimention.");
         else
            for(long i=0; i<xlParsToCalibrate.GetArrayRows(); i++)
               for(long j=0; j<xlParsToCalibrate.GetArrayCols(); j++)
                  parToCal[i*xlParsToCalibrate.GetArrayCols()+j] = xlParsToCalibrate(i,j).AsBool();
      }
   }
   bool notCal(true);
   for(long i=0; i<sz; i++) if(parToCal[i]) notCal=false;
   if(notCal) return XlfOper("At least a parameter must be calibrated.");
   
   std::vector<double> output(sz);
   pdgerr_t res = pdg_rateInterpSolver(safe_begin(rates), 
                                       safe_begin(times),
                                       safe_begin(spread),
                                       safe_begin(startVal), 
                                       parToCal,
                                       r*c, nMin, 
                                       safe_begin(output));
   
   if(res.code) return XlfOper(res.des);

   return XlfOper(sz, 1, safe_begin(output));

  EXCEL_END;
}

/** \page xlgetlibortermstruct_page xlGetLiborTermStruct
 *
 *  Returns the term structure of the specified libor curve name.
 *
 * \section Syntax
 *   xlGetLiborTermStruct(MktName)
 *
 * \section Parameters
 *   \arg \a MktName is the name of the libor curve.
 *
 * \section Returns
 *   An Excel two columns range. The first is the date of the pillar, the second
 *   column is the discount associated.
 *
 */
LPXLOPER EXCEL_EXPORT xlGetLiborTermStruct(XlfOper xlMktName)
{
   EXCEL_BEGIN;

   std::string marketName(xlMktName.AsString());
   //uppercase all characters
   std::transform(marketName.begin(), marketName.end(), marketName.begin(), toupper);

   long nValues;
   pdgerr_t res = pdg_getLiborTermStructSize(marketName.c_str(), &nValues);
   if(res.code) return XlfOper(res.des);

   std::vector<long> dates(nValues);
   std::vector<double> discounts(nValues);
   long outSize;
   res = pdg_getLiborTermStruct(marketName.c_str(), nValues, &(dates[0]), &(discounts[0]), &outSize);
   if(res.code) return XlfOper(res.des);

   std::vector<double> output(2 * nValues);
   long i;
   for(i = 0; i < nValues; ++i) output[i * 2] = dates[i];
   for(i = 0; i < nValues; ++i) output[(i * 2) + 1] = discounts[i];

   return XlfOper(nValues, 2, &(output[0]));

   EXCEL_END;
}

/** \page xlgetlibortermstructsize_page xlGetLiborTermStructSize
 *
 *  Returns the number of pillars used by the specified libor curve name.
 *
 * \section Syntax
 *   xlGetLiborTermStructSize(MktName)
 *
 * \section Parameters
 *   \arg \a MktName is the name of the libor curve.
 *
 * \section Returns
 *   An Excel number.
 *
 */
LPXLOPER EXCEL_EXPORT xlGetLiborTermStructSize(XlfOper xlMktName)
{
  EXCEL_BEGIN;

   long nValues;
   pdgerr_t res = pdg_getLiborTermStructSize(xlMktName.AsString(), &nValues);
   if(res.code) return XlfOper(res.des);

   return XlfOper(nValues);

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlGetLiborCrossTermStruct(XlfOper xlMktName1,XlfOper xlMktName2)
{
  EXCEL_BEGIN;
  pdgerr_t res;
  long i;

  std::string liborName1(xlMktName1.AsString());
  //uppercase all characters
  transform(liborName1.begin(),liborName1.end(),liborName1.begin(),toupper);
  std::string liborName2(xlMktName2.AsString());
  //uppercase all characters
  transform(liborName2.begin(),liborName2.end(),liborName2.begin(),toupper);

  long nValues1,nValues2;
  res = pdg_getLiborTermStructSize(liborName1.c_str(), &nValues1);
  if(res.code) return XlfOper(res.des);
  res = pdg_getLiborTermStructSize(liborName2.c_str(), &nValues2);
  if(res.code) return XlfOper(res.des);

  std::vector<long> dates1(nValues1);
  std::vector<double> discounts1(nValues1);
  long outSize1,outSize2;
  res = pdg_getLiborTermStruct(liborName1.c_str(), nValues1, &(dates1[0]), &(discounts1[0]), &outSize1);
  if(res.code) return XlfOper(res.des);

  std::vector<long> dates2(nValues2);
  std::vector<double> discounts2(nValues2);
  res = pdg_getLiborTermStruct(liborName2.c_str(), nValues2, &(dates2[0]), &(discounts2[0]), &outSize2);
  if(res.code) return XlfOper(res.des);

  long nCrossValues;
  std::list<long> datesList; //the list that contains the dates

  for(i=0; i<nValues1; i++) datesList.push_back(dates1[i]);
  for(i=0; i<nValues2; i++) datesList.push_back(dates2[i]);

  if(datesList.empty())
    return XlfOper("Error in xlGetLiborCrossTermStruct,no dates.");

  datesList.sort();
  datesList.unique();
  nCrossValues=datesList.size();

  std::vector<long> datesCross(nCrossValues);

  std::list<long>::const_iterator posDatesList;
  for(posDatesList = datesList.begin(), i=0; posDatesList != datesList.end(); ++posDatesList, ++i)
    datesCross[i]=*posDatesList;

  long hLibor1,hLibor2;
  res = pdg_liborStringToHandle(liborName1.c_str(), &hLibor1);
  if(res.code) return XlfOper(res.des);
  res = pdg_liborStringToHandle(liborName2.c_str(), &hLibor2);
  if(res.code) return XlfOper(res.des);

  std::vector<double> discountsCross1(nCrossValues);
  res = pdg_shmInterpDisc(hLibor1, nCrossValues, &datesCross[0], &discountsCross1[0]);
  if(res.code) return XlfOper(res.des);
  std::vector<double> discountsCross2(nCrossValues);
  res = pdg_shmInterpDisc(hLibor2, nCrossValues, &datesCross[0], &discountsCross2[0]);
  if(res.code) return XlfOper(res.des);

  std::vector<double> output(3 * nCrossValues);
  for(i = 0; i < nCrossValues; ++i) output[i * 3] = datesCross[i];
  for(i = 0; i < nCrossValues; ++i) output[(i * 3) + 1] = discountsCross1[i];
  for(i = 0; i < nCrossValues; ++i) output[(i * 3) + 2] = discountsCross2[i];

  return XlfOper(nCrossValues, 3, &(output[0]));

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlGetLiborCrossTermStructSize(XlfOper xlMktName1,XlfOper xlMktName2)
{
  EXCEL_BEGIN;
  pdgerr_t res;
  long i;

  std::string liborName1(xlMktName1.AsString());
  //uppercase all characters
  transform(liborName1.begin(),liborName1.end(),liborName1.begin(),toupper);
  std::string liborName2(xlMktName2.AsString());
  //uppercase all characters
  transform(liborName2.begin(),liborName2.end(),liborName2.begin(),toupper);

  long nValues1,nValues2;
  res = pdg_getLiborTermStructSize(liborName1.c_str(), &nValues1);
  if(res.code) return XlfOper(res.des);
  res = pdg_getLiborTermStructSize(liborName2.c_str(), &nValues2);
  if(res.code) return XlfOper(res.des);

  std::vector<long> dates1(nValues1);
  std::vector<double> discounts1(nValues1);
  long outSize1,outSize2;
  res = pdg_getLiborTermStruct(liborName1.c_str(), nValues1, &(dates1[0]), &(discounts1[0]), &outSize1);
  if(res.code) return XlfOper(res.des);

  std::vector<long> dates2(nValues2);
  std::vector<double> discounts2(nValues2);
  res = pdg_getLiborTermStruct(liborName2.c_str(), nValues2, &(dates2[0]), &(discounts2[0]), &outSize2);
  if(res.code) return XlfOper(res.des);

  long nCrossValues;
  std::list<long> datesList; //the list that contains the dates

  for(i=0; i<nValues1; i++) datesList.push_back(dates1[i]);
  for(i=0; i<nValues2; i++) datesList.push_back(dates2[i]);

  if(datesList.empty())
    return XlfOper("Error in xlGetLiborCrossTermStructSize,no dates.");

  datesList.sort();
  datesList.unique();
  nCrossValues=datesList.size();

  return XlfOper(nCrossValues);

  EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlDeliveryDate(XlfOper xlExpiryDate,
  XlfOper xlCur1Name,
  XlfOper xlCur2Name)
{
  EXCEL_BEGIN;

  std::string cur1Name = xlCur1Name.AsString();
  std::string cur2Name = xlCur2Name.AsString();

  long DeliveryDate;
  pdgerr_t res = pdg_deliveryDateCurrency((long)xlExpiryDate.AsDouble(),
                  cur1Name.c_str(),
                  cur2Name.c_str(),
                  &DeliveryDate);
  return (res.code) ? XlfOper(res.des) : XlfOper((double)DeliveryDate);

  EXCEL_END;
}


/** \page xladjustcmsrate_page xlAdjustCMSRate
 *
 * Returns the convexity adjusted CMS rate according to Hagan's "CMS Convexity Correction" Working Paper, Nomura.\n
 * The paper is a review of Doust's formula: the change of measure from swap to terminal measure is
 * linearized and the smile is flat.\n 
 * The underlying is assumed to follow a shifted lognormal dynamics \f$ dS(t)=(S(t)+\Delta)\eta dW(t)\,.\f$
 * A 'Normal' version can be obtained by approximating the normal variance as \f$(\eta (S(0)+\Delta))^2T\f$, \f$\eta\f$ being the
 * (ATM) lognormal implied volatility and \f$ S(0) \f$ is the forward swap rate.\n
 * It also supports anticipated payments (e.g. a 6M Libor paid in advance.)  
 * \section Syntax
 *   xlAdjustCMSRate(CalcDate, FixingDate, StartDate, PaymentDate, SwapRate, SwapSLNVol, SwapTenor,
 *      FixTenor, CouponTenor, Model,  Multiplier)
 *
 * \section Parameters
 *   \arg \a Trade is the trade date (long format, like in XL)
 *   \arg \a Fixing is the swap rate fixing date.
 *   \arg \a StartDate is the swap start date
 *   \arg \a PaymentDate is the swap rate payment date.
 *   \arg \a SwapRate is the forward value of the swap rate
 *   \arg \a SwapSLNVol is the shifted lognormal implied volatility of the swap rate
 *   \arg \a SwapTenor is the tenor of the swap rate, expressed in months.
 *   \arg \a FixTenor is the tenor of the fixed leg of the swap rate, expressed in months.
 *   \arg \a Model can be 0 (no adjustment), 1 (normal doust formula), 2 (lognormal doust formula).
 *   \arg \a DayCount is the optional convention used to compute payment-start delay (default 30/360)
 *           1 - 30E / 360
 *           2 - 360 / 360
 *           3 - 30E + 360
 *           4 - ACT / 360
 *           5 - ACT / 365
 *           6 - 365 / 365
 *           7 - ACT / ACT
 *           8 - 30 / 360 
 *   \arg \a Multiplier is a constant factor [optional, default=1]
 *	  \arg \a SwapRateShift is the shift \f$\Delta\f$ (e.g. 0.01 for 1%) [optional, default=0] 
 *
 * \section Returns
 *   the convexity-adjusted CMS rate multiplied by Multiplier
 *
 * \section Example
 *   =xlAdjustCMSRate("01-Jun-2012","01-Jun-2032","03-Jun-2032","03-Jun-2033",1.5%,45%,6,6,1,8,1,0)
 *	 returns 1.45475%
 * \sa
 *  A non linearized version with SABR smile support can be used through  \ref xlsabrconvexity_page  
 */
LPXLOPER EXCEL_EXPORT xlAdjustCMSRate(XlfOper xlTrade,XlfOper xlFixing, XlfOper xlStartDate, XlfOper xlPaymentDate,
                                      XlfOper xlSwapRate,XlfOper xlSwapSLNVol,XlfOper xlSwapTenor,XlfOper xlFixTenor, 
                                      XlfOper xlModel, XlfOper xlDayCount, XlfOper xlMultiplier,XlfOper xlSwapRateShift)
{
  EXCEL_BEGIN;

  long tradeDate = xloper_cast<long>(xlTrade);
  long fixingDate = xloper_cast<long>(xlFixing);
  long startDate = xloper_cast<long>(xlStartDate);
  long paymDate = xloper_cast<long>(xlPaymentDate);
  long swapTenor = xloper_cast<long>(xlSwapTenor);
  long fixTenor = xloper_cast<long>(xlFixTenor);
  long model = xloper_cast<long>(xlModel);
  long paymDayCnt = xlDayCount.IsMissing() ? 8 : xloper_cast<long>(xlDayCount);//default is 30/360, consistently with EUR fixed leg conventions
  double rateShift =  xlSwapRateShift.IsMissing() ? 0.0 : xloper_cast<double>(xlSwapRateShift);

  double adjRate;
  pdgerr_t res = pdg_adjustCMSRate(tradeDate, 
                                    fixingDate, 
                                    startDate,
                                    paymDate,
                                    xlSwapRate.AsDouble(),
                                    xlSwapSLNVol.AsDouble(),
                                    swapTenor, fixTenor,
                                    model, paymDayCnt, rateShift,
                                   &adjRate);

  if (!xlMultiplier.IsMissing() && xlMultiplier.AsDouble()>1.e-10) adjRate *= xlMultiplier.AsDouble();
  return (res.code) ? XlfOper(res.des) : XlfOper((double)adjRate);

  EXCEL_END;
}


/** \page xlshmtoday_page xlShmToday
 *
 *   Returns Today of requested libor curve.
 *
 * \section Syntax
 *   xlShmToday(LiborName)
 *
 * \section Parameters
 *   \arg \a LiborName is the name of the libor curve.
 *
 * \section Returns
 *   An Excel date (serial number).
 *
 */
LPXLOPER EXCEL_EXPORT xlShmToday(XlfOper xlCurveName)
{
   EXCEL_BEGIN;

   pdgerr_t res=RES_OK;
   long reference=0;

   try
   {
      long hLibor;
      std::string liborName(xlCurveName.AsString());
      //uppercase all characters
      std::transform(liborName.begin(), liborName.end(), liborName.begin(), toupper);

      res = pdg_liborStringToHandle(liborName.c_str(), &hLibor);
      if (!res.code) res = pdg_shmToday(hLibor, &reference);
   }
   catch(pdg::Error e) {res=e.getInfo();}
   catch(...) {}

   return (res.code) ? XlfOper(res.des) : XlfOper((double)reference);

   EXCEL_END;
}

LPXLOPER EXCEL_EXPORT xlDoubleCurveCorrection(XlfOper xlFwdCurve, XlfOper xlDiscCurve, XlfOper xlFirstFixCurve, XlfOper xlValueDate, XlfOper xlMatDate, XlfOper xlDifference)
{
   EXCEL_BEGIN;
   double correction;
   
   std::string firstFixCurveName;   
   if(!xlFirstFixCurve.IsMissing()) 
      firstFixCurveName = xlFirstFixCurve.AsString();
   
   pdgerr_t res = pdg_doubleCurveCorrection(xlFwdCurve.AsString(), 
                                            xlDiscCurve.AsString(),
                                            firstFixCurveName.c_str(),
                                            xlValueDate.AsLong(), 
                                            xlMatDate.AsLong(), 
                                            xlDifference.AsLong(), 
                                            &correction);

   return res.code ? XlfOper(res.des) : XlfOper(correction);
   
   EXCEL_END;
}


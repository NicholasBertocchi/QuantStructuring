//ciLibor - C Interface Dates Source File
#include <memory>
#include "safe_begin.h"
#include "xtos.h"
#include "libport.h"
#include "eDeliveryDate.h"
#include "ciLibor.h"
#include "cShmLibor.h"
#include "cDiscTermStructure.h"
#include "cShmLiborClientManager.h"
#include "eCurrency.h"
#include "eLibor.h"
#include "hTypes.h"
#include "eValue.h"
#include "eInterpolator.h"
#include "cIROptions.h"
#include "eForwardingCurve.h"
#include "cRTDebugger.h"
#include "rateBootstrapUtils.h"
#include <boost/thread.hpp>
#include "boost/thread/mutex.hpp"

#include <vector>
#include "matrix.h"
#include "cError.h"
#include <math.h>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////
// Note : the undocumented functions are not exported.
// see ciLibor.h (undocumented functions) and ciLiborExport.h (documented functions)
////////////////////////////////////////////////////////////////////////////////////

using namespace pdg::pdgbase;

PDGLIB_API pdgerr_t pdg_curStringToHandle(const char *cur, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_curStringToHandle]Currency: " << cur << std::endl;

      std::string uCurName(cur);
      std::transform(uCurName.begin(), uCurName.end(), uCurName.begin(), toupper);
      if (!uCurName.size()) throw pdg::Error(2, "Invalid libor name.");

      *pVal = static_cast<long>(currency::stringToHandle(uCurName));

      MTD(mt_ios::essential) << "[pdg_curStringToHandle]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_liborNameToCurHandle(const char *liborName, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_liborNameToCurHandle]LiborName: " << liborName << std::endl;

      std::string uMktName(liborName);
      std::transform(uMktName.begin(),uMktName.end(), uMktName.begin(), toupper);
      if (!uMktName.size()) throw pdg::Error(2, "Invalid libor name.");

      *pVal = libor_client::Instance().getCurveByName<ShmLibor<> >(uMktName).getCurrency();

      MTD(mt_ios::essential) << "[pdg_liborNameToCurHandle]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_liborHandleToCurHandle(long hLibor, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_liborHandleToCurHandle]LiborHandke: " << hLibor << std::endl;

      *pVal = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).getCurrency();

      MTD(mt_ios::essential) << "[pdg_liborHandleToCurHandle]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_liborNameToCurString(const char *liborName, char *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_liborNameToCurString]LiborName: " << liborName << std::endl;

      std::string uMktName(liborName);
      std::transform(uMktName.begin(),uMktName.end(), uMktName.begin(), toupper);
      if (!uMktName.size()) throw pdg::Error(2, "Invalid libor name.");

      currency_code curID = libor_client::Instance().getCurveByName<ShmLibor<> >(uMktName).getCurrency();
      std::string curName = currency::handleToString(curID);
      strcpy(pVal, curName.c_str());

      MTD(mt_ios::essential) << "[pdg_liborNameToCurString]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_curStringToCalHandle(const char *cur, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_curStringToCalHandle]Currency: " << cur << std::endl;

      std::string uCurName(cur);
      std::transform(uCurName.begin(), uCurName.end(), uCurName.begin(), toupper);
      if (!uCurName.size()) throw pdg::Error(2, "Invalid libor name.");

      *pVal = static_cast<long>(currency::stringToCalendarCode(uCurName));

      MTD(mt_ios::essential) << "[pdg_curStringToCalHandle]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_curHandleToCalHandle(long code, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_curHandleToCalHandle]Currency: " << code << std::endl;

      *pVal = currency::handleToCalendarCode(static_cast<currency_code>(code));

      MTD(mt_ios::essential) << "[pdg_curHandleToCalHandle]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_marketValueDate(const char *mktName, long today, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_marketValueDate]Market: " << mktName << std::endl;
      MTD(mt_ios::essential) << "[pdg_marketValueDate]Today: " << today << std::endl;

      *pVal = static_cast<long>(currency::valueDate(currency::stringToHandle(mktName), today).getExcelDate());

      MTD(mt_ios::essential) << "[pdg_marketValueDate]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_liborStringToHandle(const char *mktName, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_liborStringToHandle]LiborName: " << mktName << std::endl;

      std::string uMktName(mktName);
      std::transform(uMktName.begin(),uMktName.end(), uMktName.begin(), toupper);
      if (!uMktName.size()) throw pdg::Error(2, "Invalid libor name.");

      if (!libor_client::Instance().isCurvePresentByName(uMktName)) throw pdg::Error(2, "Curve (" + uMktName + ") is not present in shared memory.");
      *pVal = libor_client::Instance().getHandleByName<ShmLibor<> >(uMktName);

      MTD(mt_ios::essential) << "[pdg_liborStringToHandle]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_curHandleToDepositDayCount(long code, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_curHandleToDepositDayCount]Currency: " << code << std::endl;

      *pVal = currency::handleToDepositDayCount(static_cast<currency_code>(code));

      MTD(mt_ios::essential) << "[pdg_curHandleToDepositDayCount]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_curHandleToSwapFixLegDayCount(long code, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_curHandleToSwapFixLegDayCount]Currency: " << code << std::endl;

      *pVal = currency::handleToSwapFixLegDayCount(static_cast<currency_code>(code));

      MTD(mt_ios::essential) << "[pdg_curHandleToSwapFixLegDayCount]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_curHandleToSwapFixLegFreq(long code, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_curHandleToSwapFixLegFreq]Currency: " << code << std::endl;

      *pVal = currency::handleToSwapFixLegFreq(static_cast<currency_code>(code));

      MTD(mt_ios::essential) << "[pdg_curHandleToSwapFixLegFreq]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_curHandleToNumberValueDays(long code, long *pVal)
{
   try {
      MTD(mt_ios::essential) << "[pdg_curHandleToNumberValueDays]Currency: " << code << std::endl;

      *pVal = currency::numberValueDay(static_cast<currency_code>(code));

      MTD(mt_ios::essential) << "[pdg_curHandleToNumberValueDays]Output: " << *pVal << std::endl;
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

#if defined(_MSC_VER)
#define COMPARESTRING stricmp
#else
//POSIX
#define COMPARESTRING strcasecmp
#endif

PDGLIB_API pdgerr_t pdg_getInterpSeed(char *interpOn, long *pVal)
{
	try {

		if (!COMPARESTRING(interpOn, "r") || !COMPARESTRING(interpOn, "linear") || !COMPARESTRING(interpOn, "annual") || !COMPARESTRING(interpOn, "continuous")) *pVal = 1;
		else if (!COMPARESTRING(interpOn, "rt") || !COMPARESTRING(interpOn, "rate_time")) *pVal = 2;
		else if (!COMPARESTRING(interpOn, "d") || !COMPARESTRING(interpOn, "discount")) *pVal = 3;
		else if (!COMPARESTRING(interpOn, "f") || !COMPARESTRING(interpOn, "forward")) *pVal = 4;
		else if (!COMPARESTRING(interpOn, "cf") || !COMPARESTRING(interpOn, "const_forward")) *pVal = 5;
      else if (!COMPARESTRING(interpOn, "rs")) *pVal = 21;
		else if (!COMPARESTRING(interpOn, "rts")) *pVal = 22;
		else if (!COMPARESTRING(interpOn, "ds")) *pVal = 23;
		else if (!COMPARESTRING(interpOn, "fs")) *pVal = 24;
		else if (!COMPARESTRING(interpOn, "cfs")) *pVal = 25;
		else return pdg::Error(2,"Wrong interpolation underlying code").getInfo();

	}
	catch(pdg::Error &e) {
		return e.getInfo();
	}
	catch(...) {
		return RES_FAIL;
	}
	return RES_OK;
}

PDGLIB_API pdgerr_t pdg_getInterpMethod(char *interpType, long *pVal)
{
	try {

		if (!COMPARESTRING(interpType, "linear")) *pVal = 1;
		else if (!COMPARESTRING(interpType, "quadra") || !COMPARESTRING(interpType, "quadratic")) *pVal = 2;
		else if (!COMPARESTRING(interpType, "const") || !COMPARESTRING(interpType, "piecewise")) *pVal = 3;
		else if (!COMPARESTRING(interpType, "spline")) *pVal = 4;
		else if (!COMPARESTRING(interpType, "kruger")) *pVal = 5;
		else if (!COMPARESTRING(interpType, "monotonicspline")) *pVal = 6;
		else return pdg::Error(2,"Wrong interpolation method code").getInfo();

	}
	catch(pdg::Error &e) {
		return e.getInfo();
	}
	catch(...) {
		return RES_FAIL;
	}
	return RES_OK;
}

PDGLIB_API pdgerr_t pdg_getCompoundingMethod(char *compType, long *pVal)
{
	try {

		if (!COMPARESTRING(compType, "simple") || !COMPARESTRING(compType, "linear")) *pVal = 1;
		else if (!COMPARESTRING(compType, "annual")) *pVal = 2;
		else if (!COMPARESTRING(compType, "cont") || !COMPARESTRING(compType, "continuous")) *pVal = 3;
		else *pVal = 0; //return pdg::Error(2,"Wrong compounding method code").getInfo();

	}
	catch(pdg::Error &e) {
		return e.getInfo();
	}
	catch(...) {
		return RES_FAIL;
	}
	return RES_OK;
}

#undef COMPARESTRING


//***************************************************************************************************
//**  DISCOUNTING/FORWARDING CURVE BOOTSTRAP FUNCTIONS                                             **
//*********************************************************+*****************************************
namespace{
	pdg::ZCData ptrToZCData(long today, const char* exo_name, long exo_currcode, long exo_disc_sz, 
										const long *exo_disc_dates, const double *exo_disc_vals, long exo_disc_val_type,
										long exo_disc_type_interp, long exo_disc_interp_on, long exo_disc_comp, long exo_disc_day_count)
	{
		std::vector<long> exoDates(exo_disc_sz);
		std::vector<double> exoDiscs(exo_disc_sz);
		if(exo_disc_sz>0){
			exoDates.assign(exo_disc_dates,exo_disc_dates+exo_disc_sz);
			if(0==exo_disc_val_type){//values are discount, no conversion needed
				exoDiscs.assign(exo_disc_vals,exo_disc_vals+exo_disc_sz);
			} 
			else{//convert from zero rates to discs
				pdgerr_t res;
				double tmpdisc=0;
				long local_comp = exo_disc_comp - 1;//composition code needs to be rescaled to use pdg_rateToDiscExt   
				for(long i=0; i<exo_disc_sz ; ++i){
					res = pdg_rateToDiscExt(exo_disc_vals[i], &tmpdisc, today, exo_disc_dates[i], local_comp, exo_disc_day_count);
					if (res.code > 0 ) throw pdg::Error(2,res.des);
					exoDiscs.at(i) = tmpdisc;
				}   
			}
		}
		
		std::string str_name(exo_name);
		return pdg::ZCData(str_name, str_name, static_cast<currency_code>(exo_currcode), today, exoDates, exoDiscs,
												  exo_disc_type_interp, exo_disc_interp_on, exo_disc_comp, exo_disc_day_count);
	}
}
/* 
* Function meant to be called from XL interface or any other application 
* where curves are found and pushed in shared memory (eventually process memory)
*/

pdgerr_t pdg_liborCurveCustom(long today, long currency, const char *curve_name, 
                                         long n_depo, const char *type_depo, long *len_depo, double *rate_depo, 
                                         long n_fra, long *date_fra, double *rate_fra, 
                                         long n_futu, long *date_futu, double *price_futu, 
                                         long n_swap, long *len_swap, double *rate_swap,
                                         long type_interp, long interp_on, long comp, long day_count, 
                                         long custom_sz, const char ** custom_fields, const double *custom_values,
                                         long *out_sz, long *out_dates, double *out_vals)
{
   try {

      const std::string errMsg("#Error in pdg_liborCurveCustom, ");

      std::string curveName(curve_name);
      std::transform(curveName.begin(), curveName.end(), curveName.begin(), toupper);

      //Read customizable fields
      IROptions customOpt(custom_fields, custom_values, custom_sz);
      
      //if needed, get handle to exogenous discounting curve and retrieve its data
      pdg::ZCData exo_disc_curve_data;
      std::string exoDiscName = customOpt.get_string("DiscCurveName","");
      std::transform(exoDiscName.begin(), exoDiscName.end(), exoDiscName.begin(), toupper);
      if (exoDiscName.size()>0){
         bool curveOK = libor::getShmZCData(exoDiscName, exo_disc_curve_data);
         if(!curveOK)
            throw pdg::Error(2,errMsg + "curve " + exoDiscName + " not found");
      }

      long exo_disc_sz = exo_disc_curve_data.get_size();
      const long * exo_disc_dates = safe_begin(exo_disc_curve_data.dates);
      const double * exo_disc_vals = safe_begin(exo_disc_curve_data.discounts);
      
      //input value types are : 0=discounts, 1=zero rates
      const long  in_val_type = 0;// only discounts are currently supported as input through shared memory curves and ZCData

      pdgerr_t res = pdg_liborCurveCustomStatic(today, currency, curveName.c_str(), 
                                         n_depo, type_depo,len_depo, rate_depo, 
                                         n_fra, date_fra, rate_fra, 
                                         n_futu, date_futu, price_futu, 
                                         n_swap, len_swap, rate_swap,
                                         type_interp, interp_on, comp, day_count, 
                                         custom_sz, custom_fields, custom_values,
                                         exo_disc_sz, exo_disc_dates, exo_disc_vals, in_val_type,
                                         exo_disc_curve_data.typeInterp, exo_disc_curve_data.interpOn, 
                                         exo_disc_curve_data.comp, exo_disc_curve_data.dayCount,
                                         out_sz, out_dates, out_vals);

      //no errors, get the discounts and push the bootstrapped curve in shared memory
      if (res.code > 0 ) throw pdg::Error(2,res.des);
      std::vector<double> out_discs(*out_sz);
      long out_val_type = customOpt.get("OutputType",0.) > 0.5? 1 : 0;
      if (out_val_type==1){//zero rates (1) must be converted into discs (0) to push the curve
         double tmpdisc=0; 
         long local_comp = comp - 1;//composition code needs to be rescaled to use pdg_rateToDiscExt
         for(long i=0, imax = *out_sz; i < imax ; ++i){
            res = pdg_rateToDiscExt(out_vals[i], &tmpdisc, today, out_dates[i], local_comp, day_count);
            if (res.code > 0 ) throw pdg::Error(2,res.des);
            out_discs.at(i) = tmpdisc;
         }
      }
      else{
         out_discs.assign(out_vals,out_vals+(*out_sz));
      }
      std::string curStr = currency::handleToString(static_cast<currency_code>(currency));
      res = pdg_pushShmLiborCurveName(curStr.c_str(), curveName.c_str(), out_dates, safe_begin(out_discs), type_interp, interp_on, comp, day_count, *out_sz);
      if (res.code > 0 ) throw pdg::Error(2,res.des);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}  

PDGLIB_API pdgerr_t pdg_liborCurveCustomStatic(long today, long currency, const char *curve_name, 
                                         long n_depo, const char *type_depo, long *len_depo, double *rate_depo, 
                                         long n_fra, long *date_fra, double *rate_fra, 
                                         long n_futu, long *date_futu, double *price_futu, 
                                         long n_swap, long *len_swap, double *rate_swap,
                                         long type_interp, long interp_on, long comp, long day_count, 
                                         long custom_sz, const char ** custom_fields, const double *custom_values,
                                         long exo_disc_sz, const long *exo_disc_dates, const double *exo_disc_vals, long exo_disc_val_type,
                                         long exo_disc_type_interp, long exo_disc_interp_on, long exo_disc_comp, long exo_disc_day_count,
                                         long *out_sz, long *out_dates, double *out_vals)
{
   try {
      pdgerr_t res;

      const std::string errMsg("#Error in pdg_liborCurveCustomStatic, ");

      //Unpack deposits
      std::vector<long> lenDepo(len_depo, len_depo + n_depo);
      std::vector<double> rateDepo(rate_depo, rate_depo + n_depo);
      std::vector<depositlength_type> typeDepo(n_depo);
      for(long i = 0; i < n_depo; ++i) {
         typeDepo.at(i) = libor::charToDepositType(type_depo[i]);
      }

      //Unpack fra
      std::vector<double> rateFra(rate_fra, rate_fra + n_fra);
      std::vector<Date> dateFra(n_fra);
      for(long i = 0; i < n_fra; ++i) {
         dateFra.at(i) = date_fra[i];
      }

      //Unpack futures
      std::vector<double> priceFutu(price_futu, price_futu + n_futu);
      std::vector<Date> dateFutu(n_futu);
      for(long i = 0; i < n_futu; ++i) {
         dateFutu.at(i) = date_futu[i];
      }

      //Unpack swaps
      std::vector<long> lenSwap(len_swap, len_swap + n_swap);
      std::vector<double> rateSwap(rate_swap, rate_swap + n_swap);

      //rescale type_interp
      long typeInterp = type_interp - 1;
      //convert couple (interp_on,comp) into a value_type
      value_type valueType = getValueType(interp_on, comp);

      //Read customizable fields
      IROptions customOpt(custom_fields, custom_values, custom_sz);

      long futEndFlag = getFutEndFlag(customOpt,"standard");
      
      //Priority rules
      long matBlockCons = getMatBlockCons(customOpt,"on");  
      long futBlockCons = getFutBlockCons(customOpt,"start"); 
      
      //Convert ord
      std::vector<char> order = getBlockOrder(customOpt,"2341"); // Default priority is futures, swaps, deposits
      
      //fill in a ZCData with exogenous discounting curve infos
      const std::string& exoDiscName = customOpt.get_string("DiscCurveName","");
		std::string locName("local_"+exoDiscName);
		pdg::ZCData exo_disc_curve_data = ptrToZCData(today, locName.c_str(), currency, exo_disc_sz, exo_disc_dates, exo_disc_vals, exo_disc_val_type, 
																	 exo_disc_type_interp, exo_disc_interp_on, exo_disc_comp, exo_disc_day_count);
		
      std::auto_ptr<Curve> curve = libor::liborCurve(today, static_cast<currency_code>(currency), std::string(curve_name),
                                                     typeDepo, lenDepo, rateDepo,
                                                     dateFra, rateFra,
                                                     futEndFlag >0, dateFutu, priceFutu,
                                                     lenSwap, rateSwap,
                                                     matBlockCons >0, 
                                                     static_cast<futureblockcons_type>(futBlockCons),
                                                     order,
                                                     static_cast<interp_type>(typeInterp),
                                                     valueType,
                                                     static_cast<daycount_type>(day_count),
                                                     exo_disc_curve_data,
                                                     customOpt);

      long nInstruments = static_cast<SolvableCurve *>(curve.get())->getNInstruments();

      //verify that the caller has reserved enough place for all discounts
      if(*out_sz < nInstruments) throw pdg::Error(2, errMsg+" nInstruments ("+xtos(nInstruments)+") <= szdisc ("+xtos(*out_sz)+") needed");
      *out_sz = nInstruments;

      Date date;
      Value *discInter = 0;
      double tmpDisc=0;
      long out_val_type = customOpt.get("OutputType",0.) > 0.5? 1 : 0;
      for(long i = 0; i < nInstruments; ++i) {
         static_cast<SolvableCurve *>(curve.get())->getPair(i, date, discInter);
         out_dates[i] = static_cast<long>(date.getExcelDate());
         tmpDisc = dynamic_cast<DiscountInterface *>(discInter)->getDiscount();
         if (out_val_type==0){
            out_vals[i] = tmpDisc;
         }
         else{//discounts must be converted into zero rates
            long local_comp = comp - 1;//composition code needs to be rescaled to use pdg_discToRateExt
            res = pdg_discToRateExt(out_vals+i, tmpDisc, today, out_dates[i], local_comp, day_count);
            if (res.code > 0 ) throw pdg::Error(2,res.des);
         }
      }

   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}


pdgerr_t pdg_crossCurrencyBasisCurve(const char *basis_curve_name, long today, long dom_currency, long for_currency, double spot_fx_dom2for,
                                         long n_depo, const char *type_depo, long *len_depo, double *rate_depo, 
                                         long n_fxswap, const char *type_fxswap, long *len_fxswap, double *fx_points, 
                                         long n_ccswap, long *len_ccswap, double *rate_ccswap,
                                         long type_interp, long interp_on, long comp, long day_count, 
                                         long custom_sz, const char ** custom_fields, const double *custom_values,
                                         long *out_sz, long *out_dates, double *out_vals)
{
   try {

		pdgerr_t res;

      const std::string errMsg("#Error in pdg_crossCurrencyBasisCurve, ");

      //Read customizable fields
      IROptions customOpt(custom_fields, custom_values, custom_sz);
      
      //get handles to domestic discount and forwarding, foreign forwarding curves and retrieve data (checks are don at a lower level)
      pdg::ZCData dom_disc_data;
      const std::string& domDiscName = customOpt.get_string("DomDiscCurveName","");
      libor::getShmZCData(domDiscName, dom_disc_data);
      
      pdg::ZCData dom_fwd_data;
      const std::string& domFwdName = customOpt.get_string("DomFwdCurveName","");
      libor::getShmZCData(domFwdName, dom_fwd_data);
		
      pdg::ZCData for_fwd_data;
      const std::string& forFwdName = customOpt.get_string("ForFwdCurveName","");
      libor::getShmZCData(forFwdName, for_fwd_data);

      //input value types are : 0=discounts, 1=zero rates
      const long  in_val_type = 0;// only discounts are currently supported as input through shared memory curves and ZCData

      res = pdg_crossCurrencyBasisCurveStatic(basis_curve_name, today, dom_currency, for_currency, spot_fx_dom2for, 
                                         n_depo, type_depo,len_depo, rate_depo, 
                                         n_fxswap, type_fxswap, len_fxswap, fx_points, 
                                         n_ccswap, len_ccswap, rate_ccswap,
                                         type_interp, interp_on, comp, day_count, 
                                         custom_sz, custom_fields, custom_values,
                                         dom_disc_data.get_size(), safe_begin(dom_disc_data.dates), safe_begin(dom_disc_data.discounts), in_val_type, 
                                         dom_disc_data.typeInterp, dom_disc_data.interpOn, dom_disc_data.comp, dom_disc_data.dayCount, 
                                         dom_fwd_data.get_size(), safe_begin(dom_fwd_data.dates), safe_begin(dom_fwd_data.discounts), in_val_type,
                                         dom_fwd_data.typeInterp, dom_fwd_data.interpOn, dom_fwd_data.comp, dom_fwd_data.dayCount,
                                         for_fwd_data.get_size(), safe_begin(for_fwd_data.dates), safe_begin(for_fwd_data.discounts), in_val_type,
                                         for_fwd_data.typeInterp, for_fwd_data.interpOn, for_fwd_data.comp, for_fwd_data.dayCount,
			                             out_sz, out_dates, out_vals);

      //no errors, get the discounts and push the bootstrapped basis curve in shared memory
      if (res.code > 0 ) throw pdg::Error(2,res.des);
      std::vector<double> out_discs(*out_sz);
      long out_val_type = customOpt.get("OutputType",0.) > 0.5? 1 : 0;
      if (out_val_type==1){//zero rates (1) must be converted into discs (0) to push the curve
         double tmpdisc=0; 
         long local_comp = comp - 1;//composition code needs to be rescaled to use pdg_rateToDiscExt
         for(long i=0, imax = *out_sz; i < imax ; ++i){
            res = pdg_rateToDiscExt(out_vals[i], &tmpdisc, today, out_dates[i], local_comp, day_count);
            if (res.code > 0 ) throw pdg::Error(2,res.des);
            out_discs.at(i) = tmpdisc;
         }
      }
      else{
         out_discs.assign(out_vals,out_vals+(*out_sz));
      }
      std::string curStr = currency::handleToString(static_cast<currency_code>(for_currency));
      res = pdg_pushShmLiborCurveName(curStr.c_str(), basis_curve_name, out_dates, safe_begin(out_discs), type_interp, interp_on, comp, day_count, *out_sz);
     if (res.code > 0 ) throw pdg::Error(2,res.des);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

PDGLIB_API pdgerr_t pdg_crossCurrencyBasisCurveStatic(const char *curve_name, long today, long dom_currency, long for_currency, double spot_fx_dom2for, 
                                         long n_depo, const char *type_depo, long *len_depo, double *rate_depo, 
                                         long n_fxswap, const char *type_fxswap, long *len_fxswap, double *fx_points, 
                                         long n_swap, long *len_swap, double *rate_swap,
                                         long type_interp, long interp_on, long comp, long day_count, 
                                         long custom_sz, const char ** custom_fields, const double *custom_values,
                                         long exo_domDisc_sz, const long *exo_domDisc_dates, const double *exo_domDisc_vals, long exo_domDisc_val_type,
                                         long exo_domDisc_type_interp, long exo_domDisc_interp_on, long exo_domDisc_comp, long exo_domDisc_day_count,
                                         long exo_domFwd_sz, const long *exo_domFwd_dates, const double *exo_domFwd_vals, long exo_domFwd_val_type,
                                         long exo_domFwd_type_interp, long exo_domFwd_interp_on, long exo_domFwd_comp, long exo_domFwd_day_count,
                                         long exo_forFwd_sz, const long *exo_forFwd_dates, const double *exo_forFwd_vals, long exo_forFwd_val_type,
                                         long exo_forFwd_type_interp, long exo_forFwd_interp_on, long exo_forFwd_comp, long exo_forFwd_day_count,
                                         long *out_sz, long *out_dates, double *out_vals)
{
   try {
      pdgerr_t res;

      const std::string errMsg("#Error in pdg_crossCurrencyBasisCurveStatic, ");

		currency_code domCurr = static_cast<currency_code>(dom_currency);
      currency_code forCurr = static_cast<currency_code>(for_currency);

		//Unpack deposits
      std::vector<long> lenDepo(len_depo, len_depo + n_depo);
      std::vector<double> rateDepo(rate_depo, rate_depo + n_depo);
      std::vector<depositlength_type> typeDepo(n_depo);
      for(long i = 0; i < n_depo; ++i) {
         typeDepo.at(i) = libor::charToDepositType(type_depo[i]);
      }

      //Unpack fx swaps
      std::vector<double> fxPoints(fx_points, fx_points + n_fxswap);
      std::vector<std::string> fxSwapLabels(n_fxswap);
      for(long i = 0; i < n_fxswap; ++i) {
         fxSwapLabels.at(i) = xtos(len_fxswap[i]) + char(toupper(type_fxswap[i]));
      }

      //Unpack (cross-currency) swaps
      std::vector<long> lenSwap(len_swap, len_swap + n_swap);
      std::vector<double> rateSwap(rate_swap, rate_swap + n_swap);

      //Convert type_interp
      long typeInterp = type_interp - 1;
      //convert couple (interp_on,comp) into a value_type
      value_type valueType = getValueType(interp_on, comp);
      
      //Read customizable fields
      IROptions customOpt(custom_fields, custom_values, custom_sz);

      long futEndFlag = getFutEndFlag(customOpt,"standard");
      
      //Priority rules
      long matBlockCons = getMatBlockCons(customOpt,"on");  
      long futBlockCons = getFutBlockCons(customOpt,"start"); 
      
      //Convert ord
      std::vector<char> order = getBlockOrder(customOpt,"231"); // Default priority is fra, futures, swaps, deposits
 
      //fill in appropriate ZCData for exogenously bootstrapped curves (checks are don at a lower level)
      pdg::ZCData exo_domDisc_curve_data = ptrToZCData(today, "local_domDisc", domCurr, exo_domDisc_sz, exo_domDisc_dates, exo_domDisc_vals, exo_domDisc_val_type, 
            exo_domDisc_type_interp, exo_domDisc_interp_on, exo_domDisc_comp, exo_domDisc_day_count);
		
      pdg::ZCData exo_domFwd_curve_data = ptrToZCData(today, "local_domFwd", domCurr, exo_domFwd_sz, exo_domFwd_dates, exo_domFwd_vals, exo_domFwd_val_type, 
            exo_domFwd_type_interp, exo_domFwd_interp_on, exo_domFwd_comp, exo_domFwd_day_count);
		
      pdg::ZCData exo_forFwd_curve_data = ptrToZCData(today, "local_forFwd", forCurr, exo_forFwd_sz, exo_forFwd_dates, exo_forFwd_vals, exo_forFwd_val_type, 
            exo_forFwd_type_interp, exo_forFwd_interp_on, exo_forFwd_comp, exo_forFwd_day_count);

	  pdg::ZCData backbone_curve_data;

      boost::shared_ptr<Curve> curve = libor::crossCurrencyBasisCurve(std::string(curve_name), today, domCurr, forCurr, spot_fx_dom2for, 
                                                     typeDepo, lenDepo, rateDepo,
                                                     futEndFlag >0, fxSwapLabels, fxPoints,
                                                     lenSwap, rateSwap,
                                                     matBlockCons >0, 
                                                     static_cast<futureblockcons_type>(futBlockCons),
                                                     order,
                                                     static_cast<interp_type>(typeInterp),
                                                     valueType,
                                                     static_cast<daycount_type>(day_count),
                                                     exo_domDisc_curve_data, exo_domFwd_curve_data, exo_forFwd_curve_data,
                                                     backbone_curve_data,
                                                     customOpt);

      long nInstruments = static_cast<SolvableCurve *>(curve.get())->getNInstruments();

      //verify that the caller has reserved enough place for all discounts
      if(*out_sz < nInstruments) throw pdg::Error(2, errMsg+" nInstruments ("+xtos(nInstruments)+") <= szdisc ("+xtos(*out_sz)+") needed");
      *out_sz = nInstruments;

      Date date;
      Value *discInter = 0;
      double tmpDisc=0;
      long out_val_type = customOpt.get("OutputType",0.) > 0.5? 1 : 0;
      for(long i = 0; i < nInstruments; ++i) {
         static_cast<SolvableCurve *>(curve.get())->getPair(i, date, discInter);
         out_dates[i] = static_cast<long>(date.getExcelDate());
         tmpDisc = dynamic_cast<DiscountInterface *>(discInter)->getDiscount();
         if (out_val_type==0){
            out_vals[i] = tmpDisc;
         }
         else{//discounts must be converted into zero rates
            long local_comp = comp - 1;//composition code needs to be rescaled to use pdg_discToRateExt
            res = pdg_discToRateExt(out_vals+i, tmpDisc, today, out_dates[i], local_comp, day_count);
            if (res.code > 0 ) throw pdg::Error(2,res.des);
         }
      }

   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_mergeFRAFut(long* futdates,double* futvals,long nfut, 
                                    long* fradates,double* fravals,long nFRA,
                                    long tenor,long futfracons,long escons,long calcode,long modfollow,
                                    long* outsize,long* outdates,double* outvals)
{
   try{
      std::vector<long> fut_dates(futdates,futdates+nfut);
      std::vector<double> fut_vals(futvals,futvals+nfut);
      std::vector<long> fra_dates(fradates,fradates+nFRA);
      std::vector<double> fra_vals(fravals,fravals+nFRA);

      std::vector<long> out_dates;
      std::vector<double> out_vals;
      libor::mergeFRAFut(fut_dates,fut_vals,fra_dates,fra_vals,tenor,futfracons,escons,calcode,(modfollow>0),
                         out_dates,out_vals); 
      *outsize = out_vals.size();
      for (long  j=0;j<*outsize;++j){
         *(outdates+j) = out_dates.at(j);
         *(outvals+j) = out_vals.at(j);
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
  return RES_OK;
}

//******************************************************
//**  SHM LIBOR FUNCTIONS  *****************************
//******************************************************
pdgerr_t pdg_clearShmCurveName(const char *liborName)
{
   try {
      libor_client::Instance().getCurveByName<ShmLibor<> >(liborName).clearCurve();
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmInterpDisc(long hLibor, long out_sz, long *out_date, double *out_disc)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();

      long i;
      for(i = 0; i < out_sz; ++i) out_disc[i] = libor_client::Instance().getValueByHandle(hLibor, Date(out_date[i]));
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmInterpRate(long hLibor, long comp, long day_count, long out_sz, long *out_date, double *out_rate)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();

      Date dateStart(libor_client::Instance().getCalcDateByHandle(hLibor).getExcelDate());
      double disc;
      long i;
      for(i = 0; i < out_sz; ++i) {
         disc = libor_client::Instance().getValueByHandle(hLibor, Date(out_date[i]));
         out_rate[i] = libor::discToRateExt(disc, dateStart, out_date[i], comp, day_count);
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmInterpRateFwd(long hLibor, long fwd_date, long comp, long day_count, long out_sz, long *out_date, double *out_rate)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();

      double fwdDisc = libor_client::Instance().getValueByHandle(hLibor, Date(fwd_date));

      double disc;
      long i;
      for(i = 0; i < out_sz; ++i) {
         disc = libor_client::Instance().getValueByHandle(hLibor, Date(out_date[i])) / fwdDisc;
         out_rate[i] = libor::discToRateExt(disc, fwd_date, out_date[i], comp, day_count);
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmInterpDiscName(const char *libor_name, long out_sz, long *out_date, double *out_disc)
{
   try {
      std::string name(libor_name);
      libor_client::Instance().getCurveByName<ShmLibor<> >(name).updateTermStructure();

      long i;
      for(i = 0; i < out_sz; ++i) out_disc[i] = libor_client::Instance().getValueByName(name, Date(out_date[i]));
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmInterpDiscFwd(long hLibor, long fwd_date, long out_sz, long *out_date, double *out_disc)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();

      double fwdDisc = libor_client::Instance().getValueByHandle(hLibor, Date(fwd_date));

      long i;
      for(i = 0; i < out_sz; ++i) out_disc[i] = (libor_client::Instance().getValueByHandle(hLibor, Date(out_date[i])) / fwdDisc);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmFwRate(long hLibor, long start_date, long end_date, long calc_method, double *rate)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();

      double disc;
      disc = libor_client::Instance().getValueByHandle(hLibor, Date(end_date)) /
             libor_client::Instance().getValueByHandle(hLibor, Date(start_date));

      *rate = libor::discToRate(disc, start_date, end_date, calc_method);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmToday(long hLibor, long *pVal)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();

      *pVal = static_cast<long>(libor_client::Instance().getCalcDateByHandle(hLibor).getExcelDate());
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmImplRates(long hLibor, long nDateFixing, long *dateFixing, long periodMonths,
                                     long lagFixing, long modFollowing, long dayCount,pdg_impl_rates_type *out_disc)
{
   try {
//      DiscountSolvableCurve *curve = switchCurve(hLibor);


   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

// File con defnizione della cache per il DiscTermStructure
#include "cDTSCache.h"

namespace
{
   boost::mutex ciLibor_mutex;
}

//******************************************************
//**  LIBOR FUNCTIONS  *********************************
//******************************************************
PDGLIB_API pdgerr_t pdg_interpDisc(long in_sz, long *in_date, double *in_disc,
                                   long out_sz, long *out_date, double *out_disc,
                                   long type_interp, long interp_on, long comp, long day_count)
{
   try {
	   // Inizializzo la cache
	   DTSCache::Instance().init(1000, 0.0, 1000.0);

	   // Indice esterno (Definito qui a causa del VS C++ 6.0)
	   long 		i;
	   boost::shared_ptr<DiscTermStructure> ts(new DiscTermStructure());

	   DTSCache_Input	input(in_sz, in_date, in_disc, type_interp, interp_on, comp, day_count);
	   // Se non ho trovato la DiscTermStructure nella cache entro nel
	   // corpo sottostante e ne creo uno nuovo.
	   if (!DTSCache::Instance().Find(input,ts))
	   {
		   value_type valueType = getValueType(interp_on, comp);
			pdg::Assertion(valueType <= vtDiscount, "#Error in pdg_interpDisc, interpolation on instantaneous forwards is not supported");

		   std::vector<Date> vecDate(in_sz);
			std::vector<Value *> vecValue(in_sz);

		   Date calcDate = Date(in_date[0]);
		   for (i = 0; i < in_sz; ++i) {
			   vecDate[i] = Date(in_date[i]);
			   DiscountInterface *di = static_cast<DiscountInterface *>(value::createValue(&(*ts), calcDate, Date(in_date[i]), 0, valueType, static_cast<daycount_type>(day_count)).release());
			   di->putDiscount(in_disc[i]);
				vecValue[i] = di;
			}
		   ts->init(calcDate, static_cast<value_type>(valueType), static_cast<daycount_type>(day_count), static_cast<TermStructure::interpolation_type>(type_interp - 1), vecDate, vecValue);
		    // Inserisco in cache il nuovo DiscTermStructure
		    DTSCache::Instance().Insert(input, ts, 0.1);
	   }

      boost::mutex::scoped_lock lock(ciLibor_mutex);
	   for(i = 0; i < out_sz; ++i) out_disc[i] = static_cast<DiscountInterface *>(ts->interp(Date(out_date[i])))->getDiscount();
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_interpDisc_LinearRateContAct365(long in_sz, long *in_date, double *in_disc,
                                   long out_sz, long *out_date, double *out_disc)
{
   try {
      size_t pos;
      size_t greaterTermIter = 0;
      size_t lessTermIter = 0;

      double x0, x, x1, y0, y1;
      bool gr = false;
      for(pos = 0; pos < static_cast<unsigned long>(out_sz); ++pos) {
         if((out_date[pos] < in_date[lessTermIter]) || (out_date[pos] > in_date[greaterTermIter])) {
            while (out_date[pos] > in_date[lessTermIter]) ++lessTermIter;
            if(lessTermIter == 0)
               throw pdg::Error(2, "Bad date in discount function interpolation process.");
            else if(lessTermIter == in_sz){
               lessTermIter--;
               gr = true; // to continue linearly from two last values
            }

            greaterTermIter = lessTermIter--;
         }

         if(gr || (out_date[pos] == in_date[lessTermIter]))
            out_disc[pos] = in_disc[lessTermIter];
         else if(out_date[pos] == in_date[greaterTermIter])
            out_disc[pos] = in_disc[greaterTermIter];
         else {
            x0 = static_cast<double>(in_date[lessTermIter] - in_date[0]) / 365;
            x  = static_cast<double>(out_date[pos] - in_date[0]) / 365;
            x1 = static_cast<double>(in_date[greaterTermIter] - in_date[0]) / 365;
            y0 = -log(in_disc[lessTermIter]) / x0;
            y1 = -log(in_disc[greaterTermIter]) / x1;

            if(!x0) y0 = y1;
            out_disc[pos] = exp(-(y0 + ((x - x0) / (x1 - x0) * (y1 - y0))) * x);
         }
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_rateToDisc(double rate, double *disc, long date_start, long date_end, long calc_method)
{
   try {
      *disc = libor::rateToDisc(rate, date_start, date_end, calc_method);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_rateToDiscExt(double rate, double *disc, long date_start, long date_end, long comp, long day_count)
{
   try {
      *disc = libor::rateToDiscExt(rate, date_start, date_end, comp, day_count);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_discToRate(double *rate, double disc, long date_start, long date_end, long calc_method)
{
   try {
      *rate = libor::discToRate(disc, date_start, date_end, calc_method);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

PDGLIB_API pdgerr_t pdg_discToRateExt(double *rate, double disc, long date_start, long date_end, long comp, long day_count)
{
   try {
      *rate = libor::discToRateExt(disc, date_start, date_end, comp, day_count);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

/********************** DEPRECATED FUNCTIONS ********************************************************
pdgerr_t pdg_shmImplMarketSwap(long hLibor, long start_date, long mat_months, double *impl_rate)
{
   try {
//      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();
      ShmLibor<>& monocurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor);
		monocurve.updateTermStructure();
		const bool firstFltFixed = false;
		const double firstFltRate = 0.;
		const bool fullFltLegEval = false;

      currency_code curID = monocurve.getCurrency();
      std::string liborName = libor_client::Instance().getNameByHandle(hLibor);

      Swap<> swp(curID, 100.0, start_date, start_date, 0, mtMaturityMonth, mat_months, firstFltFixed, firstFltRate, fullFltLegEval, liborName);
      *impl_rate = swp.getImplRate(static_cast<Curve *>(&monocurve), static_cast<Curve *>(&monocurve));
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmImplSwapName(long h_libor, const char *libor_name, long start_date, long maturity, long mat_months,
                                        long freq, long mod_follow, long flag_adj, long day_count, double *impl_rate)
{
   try {
      std::string name(libor_name);
      libor_client::Instance().getCurveByName<ShmLibor<> >(name).updateTermStructure();
      currency_code curID = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_libor).getCurrency();

      CustomSwapConvention csc(curID);
      csc.fixYrf() = static_cast<daycount_type>(day_count);
      csc.fixFreq() = freq;

      adjustment_type adjust = atFwModFollowing;
      if (!mod_follow) adjust = atForward;
      if (!flag_adj) adjust = atNoAdjust;

      csc.adjust() = adjust;

      BusinessDay startDate(convManager::Instance()[curID].getCalendarCode(), start_date);
      startDate -= convManager::Instance()[curID].getSettlementDays();
      Swap<> swap(curID, csc, 100, startDate, startDate, 0, mat_months ? mtMaturityMonth : mtMaturity, mat_months ? mat_months : maturity, 0, false, name);

      *impl_rate = swap.getImplRate();
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}*/

pdgerr_t pdg_shmImplSwap(long h_libor, long start_date, long maturity, long mat_months,
                                    long freq, long mod_follow, long flag_adj, long day_count, bool first_cpn_fixed, double first_cpn, double *impl_rate)
{
   try {
      ShmLibor<>& monocurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_libor);
      monocurve.updateTermStructure();
      currency_code curID = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_libor).getCurrency();
		const bool fullFltLegEval = false;

      std::string liborName = libor_client::Instance().getNameByHandle(h_libor);

      CustomSwapConvention csc(curID);
      csc.fixYrf() = static_cast<daycount_type>(day_count);
      csc.fixFreq() = freq;

      adjustment_type adjust = atFwModFollowing;
      if (!mod_follow) adjust = atForward;
      if (!flag_adj) adjust = atNoAdjust;

      csc.adjust() = adjust;

      BusinessDay startDate(convManager::Instance()[curID].getCalendarCode(), start_date);
      startDate -= convManager::Instance()[curID].getSettlementDays();
      Swap<> swap(curID, csc, 100, startDate, startDate, 0, mat_months ? mtMaturityMonth : mtMaturity, mat_months ? mat_months : maturity, first_cpn_fixed, first_cpn, fullFltLegEval, liborName);

      *impl_rate = swap.getImplRate(static_cast<Curve *>(&monocurve),static_cast<Curve *>(&monocurve));
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmImplSwap2(long h_disc, long h_fwd, long start_date, long maturity, long mat_months,
                                    long fix_freq,  long fix_day_count,long flt_freq, long flt_day_count,
												long mod_follow, long flag_adj, bool first_cpn_fixed, double first_cpn, 
                                    double *impl_rate)
{
   try {
      //get curve handles (check existence) and check currency consistency
      ShmLibor<>& discCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_disc);
      discCurve.updateTermStructure();
      ShmLibor<>& fwdCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_fwd);
      fwdCurve.updateTermStructure();

      currency_code curID = discCurve.getCurrency();
      currency_code curID_ = fwdCurve.getCurrency();
		pdg::Assertion(curID==curID_,"#Error in pdg_shmImplSwap2, forwarding and discounting curve have different currencies");
      

      //get std conventions for given currency and overrides them
		CustomSwapConvention csc(curID);
      

      pdg::Assertion(fix_day_count < MAX_DAYCOUNT_SIZE,"#Error in pdg_shmImplSwap2, fix_day_count is greater than daycount_type.");
      pdg::Assertion(fix_day_count > -1,"#Error in pdg_shmImplSwap2, fix_day_count is negative.");
      csc.fixYrf() = static_cast<daycount_type>(fix_day_count);
      csc.fixFreq() = fix_freq;
      pdg::Assertion(flt_day_count < MAX_DAYCOUNT_SIZE,"#Error in pdg_shmImplSwap2, flt_day_count is greater than daycount_type.");
      pdg::Assertion(flt_day_count > -1,"#Error in pdg_shmImplSwap2, flt_day_count is negative.");
      csc.floatYrf() = static_cast<daycount_type>(flt_day_count);
      csc.floatFreq() = flt_freq;
      adjustment_type adjust = atFwModFollowing;
      if (!mod_follow) adjust = atForward;
      if (!flag_adj) adjust = atNoAdjust;
      csc.adjust() = adjust;

      //start_date is assumed to be an XL date, otherwise conversion in BusinessDay is wrong
      BusinessDay startDate(convManager::Instance()[curID].getCalendarCode(), start_date);
      startDate -= convManager::Instance()[curID].getSettlementDays();
      bool fullFloatLegEval = true;
		Swap<> swap(curID, csc, 100, startDate, startDate, 0, mat_months ? mtMaturityMonth : mtMaturity, mat_months ? mat_months : maturity, first_cpn_fixed, first_cpn, fullFloatLegEval);
		
      *impl_rate = swap.getImplRate(static_cast<Curve *>(&discCurve),static_cast<Curve *>(&fwdCurve));
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmImplAnnuity(long h_disc, long h_fwd, long start_date, long maturity, long mat_months,
                                       long fix_freq,  long fix_day_count,long flt_freq, long flt_day_count,
												   long mod_follow, long flag_adj, bool first_cpn_fixed, double first_cpn, long cash_settled, 
                                       double *annuity)
{
   try {
      //get curve handles (check existence) and check currency consistency
      ShmLibor<>& discCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_disc);
      discCurve.updateTermStructure();
      currency_code curID = discCurve.getCurrency();
      ShmLibor<>& fwdCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_fwd);
      fwdCurve.updateTermStructure();
      currency_code curID_ = fwdCurve.getCurrency();
		pdg::Assertion(curID==curID_,"#Error in pdg_shmImplSwap2, forwarding and discounting curve have different currencies");

      //get std conventions for given currency and overrides them
		CustomSwapConvention csc(curID);
      csc.fixYrf() = static_cast<daycount_type>(fix_day_count);
      csc.fixFreq() = fix_freq;
      csc.floatYrf() = static_cast<daycount_type>(flt_day_count);
      csc.floatFreq() = flt_freq;
      adjustment_type adjust = atFwModFollowing;
      if (!mod_follow) adjust = atForward;
      if (!flag_adj) adjust = atNoAdjust;
      csc.adjust() = adjust;

      //start_date is assumed to be an XL date, otherwise conversion in BusinessDay is wrong
      BusinessDay startDate(convManager::Instance()[curID].getCalendarCode(), start_date);
      startDate -= convManager::Instance()[curID].getSettlementDays();
      bool fullFloatLegEval = true;
		Swap<> swap(curID, csc, 100, startDate, startDate, 0, mat_months ? mtMaturityMonth : mtMaturity, mat_months ? mat_months : maturity, first_cpn_fixed, first_cpn, fullFloatLegEval);
		
      *annuity = swap.getAnnuity(static_cast<Curve *>(&discCurve),static_cast<Curve *>(&fwdCurve),cash_settled>0);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_pushShmLiborCurve(const char *liborName, long *date, double *disc, long type_interp, long interp_on, long comp,
                                          long day_count, long sz_disc)
{
   try {
      pdgerr_t res = pdg_pushShmLiborCurveName(liborName, liborName, date, disc, type_interp,
                                               interp_on, comp, day_count, sz_disc);
      if (res.code) throw pdg::Error(res);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}


pdgerr_t pdg_pushShmLiborCurveExt(const char *liborName, long ref, long *date, double *disc, long type_interp, long interp_on, long comp,
                                          long day_count, long sz_disc)
{
   try {
      pdgerr_t res = pdg_pushShmLiborCurveNameExt(liborName, liborName, ref, date, disc, type_interp,
                                               interp_on, comp, day_count, sz_disc);
      if (res.code) throw pdg::Error(res);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_pushShmLiborCurveName(const char *currency, const char *liborName, long *date, double *disc, long type_interp,
                                              long interp_on, long comp, long day_count, long sz_disc)
{
   try {
      currency_code curID = currency::stringToHandle(currency);

      std::vector<long> dateVec;
      dateVec.assign(&date[0], &date[sz_disc]);
      std::vector<double> discVec;
      discVec.assign(&disc[0], &disc[sz_disc]);

      libor::pushShmLiborCurve(curID, liborName, "", dateVec, discVec, type_interp, interp_on, comp, day_count);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_pushShmLiborCurveNameExt(const char *currency, const char *liborName, long ref, long *date, double *disc, long type_interp,
                                              long interp_on, long comp, long day_count, long sz_disc)
{
   try {
      currency_code curID = currency::stringToHandle(currency);

      std::vector<long> dateVec;
      dateVec.assign(&date[0], &date[sz_disc]);
      std::vector<double> discVec;
      discVec.assign(&disc[0], &disc[sz_disc]);

      libor::pushShmLiborCurveExt(curID, liborName, "", ref, dateVec, discVec, type_interp, interp_on, comp, day_count);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_pushShmLiborSpreadCurve(const char *currency, const char *liborName, const char* backboneName, long ref, long *date, double *disc, long type_interp,
                                     long interp_on, long comp, long day_count, long sz_disc)
{
   try {
      currency_code curID = currency::stringToHandle(currency);

      std::vector<long> dateVec;
      dateVec.assign(&date[0], &date[sz_disc]);
      std::vector<double> discVec;
      discVec.assign(&disc[0], &disc[sz_disc]);

      libor::pushShmLiborCurveExt(curID, liborName, backboneName, ref, dateVec, discVec, type_interp, interp_on, comp, day_count);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}


pdgerr_t pdg_interpDiscFwd(long in_sz, long *in_date, double *in_disc,
                                   long fwd_date, long out_sz, long *out_date, double *out_disc,
                                   long type_interp, long interp_on, long comp, long day_count)
{
   try {
	   // Inizializzo la cache
	   DTSCache::Instance().init(1000, 0.0, 1000.0);

	   // Indice esterno (Definito qui a causa del VS C++ 6.0)
	   long 		i;
	   // Shared pointer contente una struttura DiscTermStructure
	   boost::shared_ptr<DiscTermStructure> ts(new DiscTermStructure());

	   DTSCache_Input	input(in_sz, in_date, in_disc, type_interp, interp_on, comp, day_count);
	   // Se non ho trovato la DiscTermStructure nella cache entro nel
	   // corpo sottostante e ne creo uno nuovo.
	   if (!DTSCache::Instance().Find(input,ts))
	   {
   			value_type valueType = getValueType(interp_on, comp);

      		std::vector<Date> vecDate(in_sz);
      		std::vector<Value *> vecValue(in_sz);

      		Date calcDate = Date(in_date[0]);
      		for (i = 0; i < in_sz; ++i) {
         		vecDate[i] = Date(in_date[i]);
         		DiscountInterface *di = static_cast<DiscountInterface *>(value::createValue(&(*ts), calcDate, Date(in_date[i]), 0, valueType, static_cast<daycount_type>(day_count)).release());
         		di->putDiscount(in_disc[i]);
         		vecValue[i] = di;
      		}
		ts->init(calcDate, static_cast<value_type>(valueType), static_cast<daycount_type>(day_count), static_cast<TermStructure::interpolation_type>(type_interp - 1), vecDate, vecValue);
		// Inserisco in cache il nuovo DiscTermStructure
		DTSCache::Instance().Insert(input, ts, 0.1);
	   }

      boost::mutex::scoped_lock lock(ciLibor_mutex);
	   double fwdDisc = static_cast<DiscountInterface *>(ts->interp(Date(fwd_date)))->getDiscount();
	   for(i = 0; i < out_sz; ++i) out_disc[i] = (static_cast<DiscountInterface *>(ts->interp(Date(out_date[i])))->getDiscount() / fwdDisc);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_interpDiscFwd_LinearRateContAct365(long in_sz, long *in_date, double *in_disc,
                                      long fwd_date, long out_sz, long *out_date, double *out_disc)
{
   try {
      size_t pos = 0;
      size_t greaterTermIter = 0;
      size_t lessTermIter = 0;

      ++out_sz;
      if(out_sz > 199) throw pdg::Error(2, "Too many value to interpolate (max = 199).");
      long out_date_ext[200];
      double out_disc_ext[200];

      memcpy(&(out_date_ext[1]), out_date, 4*(out_sz - 1));
      out_date_ext[0] = fwd_date;

      double x0, x, x1, y0, y1;
      bool gr = false;
      for(pos = 0; pos < static_cast<unsigned long>(out_sz); ++pos) {
         if((out_date_ext[pos] < in_date[lessTermIter]) || (out_date_ext[pos] > in_date[greaterTermIter])) {
            while (out_date_ext[pos] > in_date[lessTermIter]) ++lessTermIter;
            if(lessTermIter == 0)
               throw pdg::Error(2, "Bad date in discount function interpolation process.");
            else if(lessTermIter == in_sz){
               lessTermIter--;
               gr = true; // to continue linearly from two last values
            }

            greaterTermIter = lessTermIter--;
         }

         if(gr || (out_date_ext[pos] == in_date[lessTermIter]))
            out_disc_ext[pos] = in_disc[lessTermIter];
         else if(out_date_ext[pos] == in_date[greaterTermIter])
            out_disc_ext[pos] = in_disc[greaterTermIter];
         else {
            x0 = static_cast<double>(in_date[lessTermIter] - in_date[0]) / 365;
            x  = static_cast<double>(out_date_ext[pos] - in_date[0]) / 365;
            x1 = static_cast<double>(in_date[greaterTermIter] - in_date[0]) / 365;
            y0 = -log(in_disc[lessTermIter]) / x0;
            y1 = -log(in_disc[greaterTermIter]) / x1;

            if(!x0) y0 = y1;
            out_disc_ext[pos] = exp(-(y0 + ((x - x0) / (x1 - x0) * (y1 - y0))) * x);
         }
      }

	   for(size_t i = 1; i < static_cast<unsigned long>(out_sz); ++i) out_disc[i - 1] = out_disc_ext[i] / out_disc_ext[0];
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_fwdRate(long in_sz, long *in_date, double *in_disc, long out_sz,
                                long *out_start, long *out_end, double *out_rate,
                                long type_interp, long interp_on, long comp,
                                long day_count, long calc_method)
{
   try {
      value_type valueType = getValueType(interp_on, comp);

      std::vector<Date> vecDate(in_sz);
      std::vector<Value *> vecValue(in_sz);

      Date calcDate = Date(in_date[0]);
      long i;
      DiscTermStructure ts;
      for (i = 0; i < in_sz; ++i) {
         vecDate[i] = Date(in_date[i]);
         DiscountInterface *di = static_cast<DiscountInterface *>(value::createValue(&ts, calcDate, Date(in_date[i]), 0, valueType, static_cast<daycount_type>(day_count)).release());
         di->putDiscount(in_disc[i]);
         vecValue[i] = di;
      }
      ts.init(calcDate, static_cast<value_type>(valueType), static_cast<daycount_type>(day_count), static_cast<DiscTermStructure::interpolation_type>(type_interp - 1), vecDate, vecValue);

      for(i = 0; i < out_sz; ++i) {
         double startDisc = static_cast<DiscountInterface *>(ts.interp(Date(out_start[i])))->getDiscount();
         double endDisc = static_cast<DiscountInterface *>(ts.interp(Date(out_end[i])))->getDiscount();

         out_rate[i] = libor::discToRate(endDisc / startDisc, Date(out_start[i]), Date(out_end[i]), calc_method);
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

// comp e day count vengono usati per calcolare il forward rate oltre che per costruire la libor
// non viene usato calc_method come nella pdg_fwdRate
pdgerr_t pdg_fwdRateExt(long in_sz, long *in_date, double *in_disc, long out_sz,
                                   long *out_start, long *out_end, double *out_rate,
                                   long type_interp, long interp_on, long comp,
                                   long day_count)
{
   try {
      value_type valueType = getValueType(interp_on, comp);

      std::vector<Date> vecDate(in_sz);
      std::vector<Value *> vecValue(in_sz);

      Date calcDate = Date(in_date[0]);
      long i;
      DiscTermStructure ts;
      for (i = 0; i < in_sz; ++i) {
         vecDate[i] = Date(in_date[i]);
         DiscountInterface *di = static_cast<DiscountInterface *>(value::createValue(&ts, calcDate, Date(in_date[i]), 0, valueType, static_cast<daycount_type>(day_count)).release());
         di->putDiscount(in_disc[i]);
         vecValue[i] = di;
      }
      ts.init(calcDate, static_cast<value_type>(valueType), static_cast<daycount_type>(day_count), static_cast<DiscTermStructure::interpolation_type>(type_interp - 1), vecDate, vecValue);

			// questo perchè, anche se è stata scelta l'interpolazione sul vtRateByTime, voglio
			// che la funzione restituisca un tasso zero(a composizione continua) 
			if(valueType == vtRateByTime) valueType = vtContinuousInterestRate;

      for(i = 0; i < out_sz; ++i) {
         double startDisc = static_cast<DiscountInterface *>(ts.interp(Date(out_start[i])))->getDiscount();
         double endDisc = static_cast<DiscountInterface *>(ts.interp(Date(out_end[i])))->getDiscount();

         out_rate[i] = libor::discToRateExt(endDisc / startDisc, Date(out_start[i]), Date(out_end[i]), valueType, day_count);
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_shmSwapNPV(long h_disc, long h_fwd, long start_date, long maturity, long mat_months,
                                       double fix_rate, long fix_freq,  long fix_day_count,long flt_freq, long flt_day_count,
												   long mod_follow, long flag_adj, bool first_cpn_fixed, double first_cpn, double nominal,
                                       double *npv)
{
   try {
       //get curve handles (check existence) and check currency consistency
      ShmLibor<>& discCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_disc);
      discCurve.updateTermStructure();
      ShmLibor<>& fwdCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_fwd);
      fwdCurve.updateTermStructure();

      currency_code curID = discCurve.getCurrency();
      currency_code curID_ = fwdCurve.getCurrency();
		pdg::Assertion(curID==curID_,"#Error in pdg_shmSwapNPV, forwarding and discounting curve have different currencies");

      //get std conventions for given currency and overrides them
		CustomSwapConvention csc(curID);
      

      pdg::Assertion(fix_day_count < MAX_DAYCOUNT_SIZE,"#Error in pdg_shmSwapNPV, fix_day_count is greater than daycount_type.");
      pdg::Assertion(fix_day_count > -1,"#Error in pdg_shmSwapNPV, fix_day_count is negative.");
      csc.fixYrf() = static_cast<daycount_type>(fix_day_count);
      csc.fixFreq() = fix_freq;
      pdg::Assertion(flt_day_count < MAX_DAYCOUNT_SIZE,"#Error in pdg_shmSwapNPV, flt_day_count is greater than daycount_type.");
      pdg::Assertion(flt_day_count > -1,"#Error in pdg_shmSwapNPV, flt_day_count is negative.");
      csc.floatYrf() = static_cast<daycount_type>(flt_day_count);
      csc.floatFreq() = flt_freq;
      adjustment_type adjust = atFwModFollowing;
      if (!mod_follow) adjust = atForward;
      if (!flag_adj) adjust = atNoAdjust;
      csc.adjust() = adjust;

      //start_date is assumed to be an XL date, otherwise conversion in BusinessDay is wrong
		//WARNING: Swap<> constructor pushes startDate forward according to settlement days currency convention, that is why we pull it back
      BusinessDay startDate(convManager::Instance()[curID].getCalendarCode(), start_date);
      startDate -= convManager::Instance()[curID].getSettlementDays();
      bool fullFloatLegEval = (h_disc!=h_fwd);//if handles are the same, single curve, turn off fullFloatLegEval
		Swap<> swap(curID, csc, nominal, startDate, startDate, fix_rate, mat_months ? mtMaturityMonth : mtMaturity, mat_months ? mat_months : maturity, first_cpn_fixed, first_cpn, fullFloatLegEval);
		
      *npv = swap.npv(Date(), static_cast<Curve *>(&discCurve), static_cast<Curve *>(&fwdCurve));

   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}
 pdgerr_t pdg_shmCCSwapNPV(long today, long startDate, long maturity, long matMonths, long hDomDisc, long hDomFwd, long hForDisc, long hForFwd,
                        long domFreq, long domYrf, double domNotional, double domMult, double domSpread, long domAdjFlag, long domModFollow, bool dom1stRateFixed, double dom1stCpnRate,
                        long forFreq, long forYrf, double forNotional, double forMult, double forSpread, long forAdjFlag, long forModFollow, bool for1stRateFixed, double for1stCpnRate,
								double fxSpotDom2For, long m2mCurrCode, 
								double* npv)
{
	try {

		std::string errMsg("#Error in pdg_shmCCSwapNPV, ");

		//get curve handles (check existence) and check currency consistency
      ShmLibor<>& domDiscCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hDomDisc);
      domDiscCurve.updateTermStructure();
      ShmLibor<>& domFwdCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hDomFwd);
      domFwdCurve.updateTermStructure();
      ShmLibor<>& forDiscCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hForDisc);
      forDiscCurve.updateTermStructure();
      ShmLibor<>& forFwdCurve = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hForFwd);
      forFwdCurve.updateTermStructure();

      currency_code domCurrID = domDiscCurve.getCurrency();
      currency_code forCurrID = forDiscCurve.getCurrency();
		pdg::Assertion(domCurrID == domFwdCurve.getCurrency(), errMsg+"domestic forwarding and discounting curve have different currencies");
		pdg::Assertion(forCurrID == forFwdCurve.getCurrency(), errMsg+"foreign forwarding and discounting curve have different currencies");

		//get std conventions for the given currency pair
		CCSConvHandle  ccsCH;
		bool convOK = CCSConventionManager::Instance().nothrow_get(CCSKey(domCurrID, forCurrID), ccsCH);
		pdg::Assertion(convOK, errMsg+" no default convention for given currencies ("+xtos(domCurrID)+", "+xtos(forCurrID)+")");
		CCSwapConvention::CCSLegInfo domLegInfo(ccsCH->getLegInfo(domCurrID));
		CCSwapConvention::CCSLegInfo forLegInfo(ccsCH->getLegInfo(forCurrID));

      //(eventually) override currency-linked leg defaults
		pdg::Assertion(domFreq>0, errMsg+"positive domestic frequency needed");
		pdg::Assertion(domYrf>-1 && domYrf < MAX_DAYCOUNT_SIZE, errMsg+"unknown domestic daycount code("+xtos(domYrf)+")");
		domLegInfo.freq = domFreq;
		domLegInfo.yrf = static_cast<daycount_type>(domYrf);
		adjustment_type domAdjust = atFwModFollowing;
      if (0==domModFollow) domAdjust = atForward;
      if (0==domAdjFlag) domAdjust = atNoAdjust;
      domLegInfo.adjust = domAdjust;

		pdg::Assertion(forFreq>0, errMsg+"positive foreign frequency needed");
		pdg::Assertion(forYrf>-1 && forYrf < MAX_DAYCOUNT_SIZE, errMsg+"unknown foreign daycount code("+xtos(forYrf)+")");
		forLegInfo.freq = forFreq;
		forLegInfo.yrf = static_cast<daycount_type>(forYrf);
		adjustment_type forAdjust = atFwModFollowing;
      if (0==forModFollow) forAdjust = atForward;
      if (0==forAdjFlag) forAdjust = atNoAdjust;
      forLegInfo.adjust = forAdjust;

		CCSwapConvention ccsConv(domLegInfo, forLegInfo, static_cast<currency_code>(m2mCurrCode), ccsCH->getPaymCalArray());
		CCSwap<> ccs(today, startDate, matMonths>0 ? mtMaturityMonth : mtMaturity, matMonths>0 ? matMonths : maturity, 
						 domCurrID, domNotional, domMult, domSpread, dom1stRateFixed, dom1stCpnRate,
						 forCurrID, forNotional, forMult, forSpread, for1stRateFixed, for1stCpnRate,
                   fxSpotDom2For, &ccsConv);

      *npv = ccs.npv(today, static_cast<Curve *>(&domDiscCurve), static_cast<Curve *>(&domFwdCurve), static_cast<Curve *>(&forDiscCurve), static_cast<Curve *>(&forFwdCurve));
		   
	}
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

 pdgerr_t pdg_rateInterpSolver(double *rates, double *times, double *spread, double *startValues, bool *parsToCal, long n, long nMin, double *solution)
{
   try {
      pdg::Vector r(n), t(n), s(n), sv(4);
      std::vector<bool> ptc(4);
      for(long i=0; i<4; i++){
         sv[i] = startValues[i];
         ptc[i] = parsToCal[i];
      }
      for(long i=0; i<n; i++){
         r[i] = rates[i];
         t[i] = times[i];
         s[i] = spread[i];
      }
      pdg::Vector sol = libor::rateInterpolator(r, t, s, sv, ptc, nMin);
      for(long i=0; i<sol.elem(); i++)
         solution[i] = sol[i];
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }

   return RES_OK;
}

pdgerr_t pdg_deliveryDateCurrency(long ExpiryDate, const char * cur1, const char * cur2, long *pVal)
{
   try {
      calendar_code cal1, cal2;
      cal1 = currency::stringToCalendarCode(cur1);
      cal2 = currency::stringToCalendarCode(cur2);

	   *pVal =static_cast<long>( deliveryDate(ExpiryDate, cal1, cal2).getExcelDate());
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_getLiborTermStruct(const char *mktName, long inSize, long *dates, double *discount, long *outSize)
{
   try {
      std::vector<Date> vDates;
      std::vector<DiscountInterface *> vValues;

      std::string uMktName(mktName);
      std::transform(uMktName.begin(),uMktName.end(), uMktName.begin(), toupper);

      libor::getLiborTermStruct(uMktName, vDates, vValues);
      *outSize = vDates.size();
      if (static_cast<unsigned long>(inSize) < vDates.size()) throw pdg::Error(2, "Not enough room.");

      unsigned long i;
      for(i = 0; i < vDates.size(); ++i) {
         dates[i] = static_cast<long>(vDates[i].getExcelDate());
         discount[i] = vValues[i]->getDiscount();

         delete vValues[i];
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_getLiborTermStructSize(const char *mktName, long *pVal)
{
   try {
      std::string uMktName(mktName);
      std::transform(uMktName.begin(),uMktName.end(), uMktName.begin(), toupper);

      *pVal = libor::getLiborTermStructSize(uMktName);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_adjustCMSRate(long trade, long fixing, long start, long payment,
                                         double swapRate, double swapVol, 
                                         long swapTenor, long fixTenor,
                                         long model, long dayCC, double rateShift,
                                         double* adjRate)
{
   try {
      *adjRate = libor::adjustCMSRate(trade, fixing, start, payment, 
                                       swapRate, swapVol, swapTenor, 
                                       fixTenor, model, dayCC, rateShift);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_liborConventions(long hLibor, long *hCal, long *nValueDaysDepo, long *dayCountDepo,
                                         long *nValueDaysSwap, long *dayCountSwap, long *freqSwap, long *flagStub,
                                         long *flagAdj, long *modFollow, long *flagEoM)
{
   try {
      currency_code curID = static_cast<currency_code>(hLibor);

      *hCal = static_cast<long>(convManager::Instance()[curID].getCalendarCode());
		*nValueDaysDepo	= static_cast<long>(convManager::Instance()[curID].getSettlementDays());
		*dayCountDepo	= static_cast<long>(convManager::Instance()[curID].getConv<DepositConvention>("deposit")._yearFraction);
		*nValueDaysSwap = *nValueDaysDepo; // Nella nuova libreria non ? previsto che siano diversi.
		*dayCountSwap	= static_cast<long>(convManager::Instance()[curID].getConv<SwapConvention>("swap")._fixYrf);
		*freqSwap		= static_cast<long>(convManager::Instance()[curID].getConv<SwapConvention>("swap")._fixFreq);
		*flagStub		= static_cast<long>(convManager::Instance()[curID].getConv<SwapConvention>("swap")._fStub);
		*flagAdj		= 1; // Nella nuova libreria non ? previsto che sia non sia true.
		*modFollow		= 1; // Nella nuova libreria non ? previsto che sia non sia true.
		*flagEoM		= static_cast<long>(convManager::Instance()[curID].getConv<SwapConvention>("swap")._fEom);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

// Convention code:
// c - Calendar Handle
// d - Value Days Deposit
// D - Daycount Deposit
// s - Value Days Swap
// S - Daycount Swap
// f - Frequency Swap
// u - Flag Stub
// a - Flag Adjustment
// m - Modify following
// e - Flag End of Month
pdgerr_t pdg_liborConventionsExt(long hLibor, const char *convString, long *convs)
{
   try {
      long hCal;
		long nValueDaysDepo;
		long dayCountDepo;
		long nValueDaysSwap;
		long dayCountSwap;
		long freqSwap;
		long flagStub;
		long flagAdj;
		long modFollow;
		long flagEoM;

      pdgerr_t res = pdg_liborConventions(hLibor, &hCal, &nValueDaysDepo, &dayCountDepo,
                                          &nValueDaysSwap, &dayCountSwap, &freqSwap, &flagStub,
                                          &flagAdj, &modFollow, &flagEoM);
      if (res.code) throw pdg::Error(res);

      long i, n;
      const long len = strlen(convString);
      for (i = 0, n = 0; i < len; ++i) {
         if (convString[i] == 'c') convs[n++] = hCal;
         else if (convString[i] == 'd') convs[n++] = nValueDaysDepo;
         else if (convString[i] == 'D') convs[n++] = dayCountDepo;
         else if (convString[i] == 's') convs[n++] = nValueDaysSwap;
         else if (convString[i] == 'S') convs[n++] = dayCountSwap;
         else if (convString[i] == 'f') convs[n++] = freqSwap;
         else if (convString[i] == 'u') convs[n++] = flagStub;
         else if (convString[i] == 'a') convs[n++] = flagAdj;
         else if (convString[i] == 'm') convs[n++] = modFollow;
         else if (convString[i] == 'e') convs[n++] = flagEoM;
      }
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_shmImplSwapPrice(long hLibor, long calc_date, long maturity, double swap_rate, double *swap_price)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();
      currency_code curID = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).getCurrency();

      std::string liborName = libor_client::Instance().getNameByHandle(hLibor);

      *swap_price = libor::shmImplSwapPrice(static_cast<currency_code>(hLibor), calc_date, maturity, swap_rate, liborName);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_shmImplSwapFromFwdRates(long hLibor, long start_date, long maturity, long mat_months,
                                                long freq, long mod_follow, long flag_adj, long day_count,
                                                bool firstRateFixed, double fixing, double *pVal)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).updateTermStructure();
      currency_code curID = libor_client::Instance().getCurveByHandle<ShmLibor<> >(hLibor).getCurrency();

      std::string liborName = libor_client::Instance().getNameByHandle(hLibor);

      *pVal = libor::shmImplSwapFromFwdRates(curID, start_date, maturity, mat_months,
                                             freq, mod_follow ? true : false, (flag_adj != 0) ? atFwModFollowing : atNoAdjust, static_cast<daycount_type>(day_count), firstRateFixed, fixing, liborName);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}


pdgerr_t pdg_shmImplSwapPriceUser(long h_libor, long start_date, long maturity, long mat_months,
                                             long freq, long mod_follow, long flag_adj, long day_count, double swap_rate,
                                             double *swap_price)
{
   try {
      libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_libor).updateTermStructure();
      currency_code curID = libor_client::Instance().getCurveByHandle<ShmLibor<> >(h_libor).getCurrency();

      std::string liborName = libor_client::Instance().getNameByHandle(h_libor);

      adjustment_type flagAdj = atNoAdjust;
      if (flag_adj && mod_follow) flagAdj = atFwModFollowing;
      else if (flag_adj && !mod_follow) flagAdj = atForward;

      *swap_price = libor::shmImplSwapPriceUser(curID, start_date, maturity, mat_months,
                                                freq, flagAdj, static_cast<daycount_type>(day_count), swap_rate, liborName);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

#if (_MSC_VER<=1600 && __GNUC__ <= 3)
// this function is equivalent to std::is_sorted that is in C++11
template <class ForwardIterator>
bool is_sorted (ForwardIterator first, ForwardIterator last)
{
  if (first==last) return true;
  ForwardIterator next = first;
  while (++next!=last) {
    if (*next<*first)     // or, if (comp(*next,*first)) for version (2)
      return false;
    ++first;
  }
  return true;
}
#endif


pdgerr_t pdg_liborInterpLinear(long inSize, double *inX, double *inY, long outSize, double *outX, double *outY)
{
   try {
      std::vector<double> inXVec(inX, inX + inSize);
      std::vector<double> inYVec(inY, inY + inSize);
      std::vector<double> outXVec(outX, outX + outSize);


	  // linearMultipleInterp uses lower_bound that assumes that inXVec is ordered
	  // so the following check is required. It should replace isStrictlyAscendentSorted
      pdg::Assertion(is_sorted(inXVec.begin(),inXVec.end()),"#Error in interp::pdg_liborInterpLinear, inX vector is not in ascendig order"); 
      std::vector<double> outYVec = interp::linearMultipleInterp<double>(inXVec, inYVec, outXVec);

      std::copy(outYVec.begin(), outYVec.end(), outY);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}


pdgerr_t pdg_doubleCurveCorrection(const char *fwdCurve, const char *discCurve, const char *firstFixCurve, long valueDate, long matDate, long difference, double *correction)
{
   try {
      *correction = libor::doubleCurveCorrection(std::string(fwdCurve),
                                                 std::string(discCurve),
                                                 std::string(firstFixCurve),
                                                 Date(valueDate),
                                                 Date(matDate),
                                                 difference > 0);
   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}

pdgerr_t pdg_mxInterpDisc(long hLibor, long day_count, long out_sz, double *out_yf, double *out_disc)
{
   try {

   }
   catch(pdg::Error e) {
      return e.getInfo();
   }
   catch(...) {
      return RES_FAIL;
   }
   return RES_OK;
}


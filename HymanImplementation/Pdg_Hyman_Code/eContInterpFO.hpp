//eContInterpFO.hpp
#ifndef _ECONTINTERPFO_HPP__
#define _ECONTINTERPFO_HPP__
#include <vector>
#include <iterator>
#include <typeinfo>

#include <boost/function.hpp>
#include <boost/utility.hpp>

#include "cError.h"
#include "fast_upper_bound.hpp"
#include "fast_lower_bound.hpp"
#include "eContInterp.hpp"
#include "eBSpline.h"

namespace cont_interp_fo {

double identity(double x);

template<class T> struct Identity {
    T operator()(const T& x) { return x; }
};

template<
   typename T,
   typename V,
   typename _It1, 
   typename _It2
>
double linear(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   const T &xx,
   boost::function1<double, const T &> tx = cont_interp_fo::Identity<T>(),
   boost::function1<double, const V &> ty = cont_interp_fo::Identity<V>()
)
{
   if(std::distance(x_begin, x_end) != std::distance(y_begin, y_end))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix = x_begin + pdg::fast_lower_bound(x_begin, x_end, xx);

   if(gix == x_end) 
      return ty(*boost::prior(y_end));
   if(xx == *gix) 
      return ty(*(y_begin + std::distance(x_begin, gix)));
   if(gix == x_begin) 
      throw pdg::Error(2, "Interpolation point before first abscissa value");

   _It1 lix = boost::prior(gix);

   double x0 = tx(*lix);
   double ix = tx(xx);
   double x1 = tx(*gix);
   double y0 = ty(*(y_begin + std::distance(x_begin, lix)));
   double y1 = ty(*(y_begin + std::distance(x_begin, gix)));

   return cont_interp::detail::linearInterp(x0, ix, x1, y0, y1);
}

// linear interpolator inside x_begin and x_end
// flat outside
template<
   typename _It1, 
   typename _It2
>
double linear_in_flat_out(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   const typename std::iterator_traits<_It1>::value_type &xx,
   boost::function1<double, const typename std::iterator_traits<_It1>::value_type &> tx = cont_interp_fo::Identity<typename std::iterator_traits<_It1>::value_type>(),
   boost::function1<double, const typename std::iterator_traits<_It2>::value_type &> ty = cont_interp_fo::Identity<typename std::iterator_traits<_It2>::value_type>()
)
{
   if(std::distance(x_begin, x_end) != std::distance(y_begin, y_end))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix = x_begin + pdg::fast_lower_bound(x_begin, x_end, xx);

   if(gix == x_end) 
      return ty(*boost::prior(y_end));
   if(xx == *gix) 
      return ty(*(y_begin + std::distance(x_begin, gix)));
   if(gix == x_begin) 
      return ty(*y_begin);

   _It1 lix = boost::prior(gix);

   double x0 = tx(*lix);
   double ix = tx(xx);
   double x1 = tx(*gix);
   double y0 = ty(*(y_begin + std::distance(x_begin, lix)));
   double y1 = ty(*(y_begin + std::distance(x_begin, gix)));

   return cont_interp::detail::linearInterp(x0, ix, x1, y0, y1);
}

template<
   typename _It1, 
   typename _It2, 
   typename _It3, 
   typename _It4
>
void linear_range(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin,
   _It3 xx_begin, _It3 xx_end,
   _It4 yy_begin,
   bool ordered = true,
   boost::function1<double, const typename std::iterator_traits<_It1>::value_type &> tx = cont_interp_fo::Identity<typename std::iterator_traits<_It1>::value_type>(),
   boost::function1<double, const typename std::iterator_traits<_It2>::value_type &> ty = cont_interp_fo::Identity<typename std::iterator_traits<_It2>::value_type>()
   )
{
   _It1 gix = x_begin;
   for(_It3 xx_i = xx_begin; xx_i != xx_end; ++xx_i, ++yy_begin) {
      gix = ordered ? gix : x_begin;
      gix = gix + pdg::fast_lower_bound(gix, x_end, *xx_i);

      if(gix == x_end) {
         if (ordered) {
            double val = ty(*boost::prior(y_begin + std::distance(x_begin, x_end)));
            for (;xx_i != xx_end; ++xx_i, ++yy_begin)
               *yy_begin = val;

            return;
         } 
         else
            *yy_begin = ty(*boost::prior(y_begin + std::distance(x_begin, x_end)));
      }
      else if(*xx_i == *gix) 
         *yy_begin = ty(*(y_begin + std::distance(x_begin, gix)));
      else if(gix == x_begin) 
         *yy_begin = ty(*y_begin);
      else {
         _It1 lix = boost::prior(gix);

         double x0 = tx(*lix);
         double ix = tx(*xx_i);
         double x1 = tx(*gix);
         double y0 = ty(*(y_begin + std::distance(x_begin, lix)));
         double y1 = ty(*(y_begin + std::distance(x_begin, gix)));

         *yy_begin = cont_interp::detail::linearInterp(x0, ix, x1, y0, y1);
      }
   }
}

template<
   typename T,
   typename V,
   typename _It1, 
   typename _It2
>
double linear_dep(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   const typename std::iterator_traits<_It1>::value_type &xx,
   boost::function1<double, const T &> tx,
   boost::function2<double, const T &, const V &> ty
)
{
if(std::distance(x_begin, x_end) != std::distance(y_begin, y_end))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix = x_begin + pdg::fast_lower_bound(x_begin, x_end, xx);

   if(gix == x_end) 
      return ty(*boost::prior(x_end), *boost::prior(y_end));
   if(xx == *gix) 
      return ty(*gix, *(y_begin + std::distance(x_begin, gix)));
   if(gix == x_begin) 
      throw pdg::Error(2, "Interpolation point before first abscissa value");

   _It1 lix = boost::prior(gix);

   double x0 = tx(*lix);
   double ix = tx(xx);
   double x1 = tx(*gix);
   double y0 = ty(*lix, *(y_begin + std::distance(x_begin, lix)));
   double y1 = ty(*gix, *(y_begin + std::distance(x_begin, gix)));

   return cont_interp::detail::linearInterp(x0, ix, x1, y0, y1);
}

//note: y can point to any arithmetic type.
template<
   typename _It1, 
   typename _It2, 
   typename _It3>
void kruger_preconditioning(
   _It1 in_x_begin, _It1 in_x_end,
   _It2 in_y_begin,
   _It3 out_a_begin, _It3 out_b_begin, _It3 out_c_begin,
   boost::function1<double, const typename std::iterator_traits<_It1>::value_type &> tx = cont_interp_fo::Identity<typename std::iterator_traits<_It1>::value_type>(),
   boost::function1<double, const typename std::iterator_traits<_It2>::value_type &> ty = cont_interp_fo::Identity<typename std::iterator_traits<_It2>::value_type>()
)
{   
   typedef typename std::iterator_traits<_It1>::difference_type diff_t;
   typedef typename std::iterator_traits<_It2>::value_type value_type;

   diff_t n = std::distance(in_x_begin, in_x_end);
   if (n <= 3) 
      throw pdg::Error(2, "#Error in cont_interp::kruger, too few input points");
   
   std::vector<value_type> tmp(n);
   size_t nminusone = static_cast<size_t>(n-1);
   std::vector<double> dx(nminusone);
   std::vector<value_type> s(nminusone);
   
   {
      _It2 y_pos = in_y_begin;
      std::vector<double>::iterator dx_pos = dx.begin();
      typename std::vector<value_type>::iterator s_pos = s.begin();
      for(_It1 x_pos = in_x_begin;
         x_pos != boost::prior(in_x_end); 
         ++x_pos, ++y_pos, ++dx_pos, ++s_pos
      ) {
         *dx_pos = tx(*(boost::next(x_pos))) - tx(*x_pos);
         *s_pos = (ty(*(boost::next(y_pos))) - ty(*y_pos)) / *dx_pos; 
      }
   }

   // intermediate points
   for (unsigned int i = 1; i < nminusone; ++i) {
      if (s[i - 1] * s[i] <= 0.0)
         // slope changes sign at point
         tmp[i] = 0.0;
      else
         // slope will be between the slopes of the adjacent
         // straight lines and should approach zero if the
         // slope of either line approaches zero
         tmp[i] = 2.0 / (1.0 / s[i - 1] + 1.0 / s[i]);
   }
   // end points
   tmp[0] = (3.0 * s[0] - tmp[1]) / 2.0;
   tmp[nminusone] = (3.0 * s[n - 2] - tmp[n - 2]) / 2.0;

   // cubic coefficients
   //std::vector<double> a(n - 1), b(n - 1), c(n - 1);
   for(unsigned int i = 0; i < nminusone; ++i, ++out_a_begin, ++out_b_begin, ++out_c_begin ) {
      *out_a_begin = tmp[i];
      *out_b_begin = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      *out_c_begin = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }
}

//note: y can point to any arithmetic type.
template<
   typename _It1, 
   typename _It2, 
   typename _It3, 
   typename _It4
>
void kruger(
   _It1 in_x_begin, _It1 in_x_end,
   _It2 in_y_begin, _It2 in_y_end,
   _It3 out_x_begin, _It3 out_x_end,
   _It4 out_y_begin,
   boost::function1<double, const typename std::iterator_traits<_It1>::value_type &> tx = cont_interp_fo::Identity<typename std::iterator_traits<_It1>::value_type>(),
   boost::function1<double, const typename std::iterator_traits<_It2>::value_type &> ty = cont_interp_fo::Identity<typename std::iterator_traits<_It2>::value_type>()
)
{
  typedef typename std::iterator_traits<_It2>::value_type value_type;
  typedef typename std::iterator_traits<_It1>::difference_type diff_t;

  diff_t n = std::distance(in_x_begin, in_x_end);
  if(n != std::distance(in_y_begin, in_y_end) || n<=0)
     throw pdg::Error(2, "InX and InY vectors must have the same strictly positive size.");
   
  size_t nminusone = static_cast<size_t>(n-1);
  
  std::vector<value_type> a(nminusone), b(nminusone), c(nminusone);

  kruger_preconditioning(in_x_begin, in_x_end, in_y_begin, a.begin(), b.begin(), c.begin(), tx, ty);
  cont_interp::kruger_preconditioned(in_x_begin, in_x_end, in_y_begin, a.begin(), b.begin(), c.begin(), out_x_begin, out_x_end, out_y_begin);
}

template<
   typename _It1, 
   typename _It2, 
   typename _It3, 
   typename _It4
>
void kruger_range(
   _It1 in_x_begin, _It1 in_x_end,
   _It2 in_y_begin,
   _It3 out_x_begin, _It3 out_x_end,
   _It4 out_y_begin,
   boost::function1<double, const typename std::iterator_traits<_It1>::value_type &> tx = cont_interp_fo::Identity<const typename std::iterator_traits<_It1>::value_type>(),
   boost::function1<double, const typename std::iterator_traits<_It2>::value_type &> ty = cont_interp_fo::Identity<const typename std::iterator_traits<_It2>::value_type>()
)
{
   unsigned int n = std::distance(in_x_begin, in_x_end);
   if (n <= 3) throw pdg::Error(2, "#Error in cont_interp::kruger, too few input points");
   
   std::vector<double> tmp(n);
   unsigned int nminusone = n-1;
   std::vector<double> dx(nminusone), s(nminusone);
   
   {
      _It2 y_pos = in_y_begin;
      std::vector<double>::iterator dx_pos = dx.begin();
      std::vector<double>::iterator s_pos = s.begin();
      for(_It1 x_pos = in_x_begin;
         x_pos != boost::prior(in_x_end); 
         ++x_pos, ++y_pos, ++dx_pos, ++s_pos
      ) {
         *dx_pos = tx(*(boost::next(x_pos))) - tx(*x_pos);
         *s_pos = (ty(*(boost::next(y_pos))) - ty(*y_pos)) / *dx_pos; 
      }
   }

   // intermediate points
   for (unsigned int i = 1; i < nminusone; ++i) {
      if (s[i - 1] * s[i] < 0.0)
         // slope changes sign at point
         tmp[i] = 0.0;
      else
         // slope will be between the slopes of the adjacent
         // straight lines and should approach zero if the
         // slope of either line approaches zero
         tmp[i] = 2.0 / (1.0 / s[i - 1] + 1.0 / s[i]);
   }
   // end points
   tmp[0] = (3.0 * s[0] - tmp[1]) / 2.0;
   tmp[nminusone] = (3.0 * s[n - 2] - tmp[n - 2]) / 2.0;

   // cubic coefficients
   std::vector<double> a(nminusone), b(nminusone), c(nminusone);
   for(unsigned int i = 0; i < nminusone; ++i) {
      a[i] = tmp[i];
      b[i] = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      c[i] = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }
   {
      _It4 y_pos = out_y_begin;
      for(_It3 x_pos = out_x_begin;
          x_pos != out_x_end;
          ++x_pos,
          ++y_pos
      ) {
         unsigned int j;
         if(*x_pos < *in_x_begin)
            j = 0;
         else if(*x_pos > *(boost::prior(in_x_end)))
            j = std::distance(in_x_begin, in_x_end) - 2;
         else
            j = std::distance(in_x_begin, std::upper_bound(in_x_begin, boost::prior(in_x_end), *x_pos)) - 1;
         
         double dx = *x_pos - *(boost::next(in_x_begin, j));
         *y_pos = *(boost::next(in_y_begin, j)) + dx *(a[j] + dx * (b[j] + dx * c[j]));   
      }
   }
}

/*namespace {
double linearInterp(double prevX, double x, double succX, double prevY, double y, double succY,
                    double Z00, double Z01, double Z10, double Z11)
{
   double denx = succX - prevX;
   double wxh = denx ? (x - prevX) / denx : 0.5;
   double wxl = denx ? (succX - x) / denx : 0.5;

   double deny = succY - prevY;
   double wyh = deny ? (y - prevY) / deny : 0.5;
   double wyl = deny ? (succY - y) / deny : 0.5;

   return Z00 * wxl * wyl + Z01 * wxl * wyh + Z10 * wxh * wyl + Z11 * wxh * wyh;
}
}*/

template <class ForwardIt, class T> 
void fast_lg_bounds(ForwardIt first, ForwardIt last, const T &value, ForwardIt &gix, ForwardIt &lix  )
{
   gix = first + pdg::fast_lower_bound(first, last, value);
   if(gix == first) lix = gix;
   else if(gix == last) lix = --gix;
   else lix = boost::prior(gix);
}

template<
   typename T,
   typename V,
   typename Z,
   typename _It1,
   typename _It2,
   typename _It3
>
typename std::iterator_traits<_It3>::value_type linear(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   _It3 z_begin, _It3 z_end,
   size_t z_cols,
   const T &xx, const V &yy,
   boost::function1<double, const T &> tx = cont_interp_fo::Identity<T>(),
   boost::function1<double, const V &> ty = cont_interp_fo::Identity<V>(),
   boost::function1<double, const Z &> tz = cont_interp_fo::Identity<Z>()
)
{
   if((std::distance(y_begin, y_end) != z_cols) || (std::distance(x_begin, x_end) != (std::distance(z_begin, z_end) / z_cols)))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix; _It1 lix;
   fast_lg_bounds(x_begin, x_end, xx, gix, lix);
   _It2 giy; _It2 liy;
   fast_lg_bounds(y_begin, y_end, yy, giy, liy);
   
   double x0 = tx(*lix);
   double ix = tx(xx);
   double x1 = tx(*gix);
   double y0 = ty(*liy);
   double iy = ty(yy);
   double y1 = ty(*giy);

   double z00 = tz(*(z_begin + ((std::distance(x_begin, lix) * z_cols) + std::distance(y_begin, liy))));
   double z01 = tz(*(z_begin + ((std::distance(x_begin, lix) * z_cols) + std::distance(y_begin, giy))));
   double z10 = tz(*(z_begin + ((std::distance(x_begin, gix) * z_cols) + std::distance(y_begin, liy))));
   double z11 = tz(*(z_begin + ((std::distance(x_begin, gix) * z_cols) + std::distance(y_begin, giy))));

   return cont_interp::detail::linearInterp(x0, ix, x1, y0, iy, y1, z00, z01, z10, z11);
}


// 1) interpolation time is xx
// 2) two kruger interpolations are performed on T0 < xx < T1 on the strike V
// 3) the two values are linearly interpolated
template<
   typename T,
   typename V,
   typename Z,
   typename _It1,
   typename _It2,
   typename _It3
>
typename std::iterator_traits<_It3>::value_type kruger2(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   _It3 z_begin, _It3 z_end,
   size_t z_cols,
   const T &xx, const V &yy,
   boost::function1<double, const T &> tx = cont_interp_fo::Identity<T>(),
   boost::function1<double, const V &> ty = cont_interp_fo::Identity<V>(),
   boost::function1<double, const Z &> tz = cont_interp_fo::Identity<Z>()
)
{
   // SAME CODE OF LINEAR START
   if((std::distance(y_begin, y_end) != z_cols) || (std::distance(x_begin, x_end) != (std::distance(z_begin, z_end) / z_cols)))
      throw pdg::Error(2, "The vectors' size must be the same.");

   // 1)
   _It1 gix; _It1 lix;
   fast_lg_bounds(x_begin, x_end, xx, gix, lix);
   _It2 giy; _It2 liy;
   fast_lg_bounds(y_begin, y_end, yy, giy, liy);
   
   double x0 = tx(*lix);
   double ix = tx(xx);
   double x1 = tx(*gix);
   // SAME CODE OF LINEAR END

   //----KRUGER START----
   _It3 in_z_begin0 = z_begin + (std::distance(x_begin, lix) * z_cols);
   _It3 in_z_end0   = in_z_begin0 + z_cols;
   _It3 in_z_begin1 = z_begin + (std::distance(x_begin, gix) * z_cols);
   _It3 in_z_end1   = in_z_begin1 + z_cols;

   // 2)
   double z0; cont_interp::kruger( y_begin, y_end, in_z_begin0, in_z_end0, &yy, boost::next(&yy), &z0);
   double z1; cont_interp::kruger( y_begin, y_end, in_z_begin1, in_z_end1, &yy, boost::next(&yy), &z1);
   
   // 3)
   return cont_interp::detail::linearInterp(x0, ix, x1, z0, z1);
   //----KRUGER END----

   /*
   // The following code interpolates linearly along times and then it applies one kruger interpolation
   // it's more efficient but produces different result from previous implementation. 
   // The same idea may be applied to cubic spline 
   std::vector<Z> izs(z_cols);
   if (x1==x0)
      for (size_t c=0; c<z_cols; c++)
         izs[c] =*( in_z_begin0 + c);
   else
      for (size_t c=0; c<z_cols; c++)
         izs[c] = *( in_z_begin0 + c) + (ix-x0)/(x1-x0) *(*( in_z_begin1 + c)- *( in_z_begin0 + c) );


   cont_interp::kruger( y_begin, y_end, izs.begin(), izs.end(), &yy, boost::next(&yy), &z0);
   return z0;
   */
   
}

// 1) interpolation time is xx
// 2) two cspline interpolations are performed on T0 < xx < T1 on the strike V
// 3) the two values are linearly interpolated
template<
   typename T,
   typename V,
   typename Z,
   typename _It1,
   typename _It2,
   typename _It3
>
typename std::iterator_traits<_It3>::value_type cubic_spline(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   _It3 z_begin, _It3 z_end,
   size_t z_cols,
   const T &xx, const V &yy,
   boost::function1<double, const T &> tx = cont_interp_fo::Identity<T>(),
   boost::function1<double, const V &> ty = cont_interp_fo::Identity<V>(),
   boost::function1<double, const Z &> tz = cont_interp_fo::Identity<Z>()
)
{
   // SAME CODE OF LINEAR START
   if((std::distance(y_begin, y_end) != z_cols) || (std::distance(x_begin, x_end) != (std::distance(z_begin, z_end) / z_cols)))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix; _It1 lix;
   fast_lg_bounds(x_begin, x_end, xx, gix, lix);
   _It2 giy; _It2 liy;
   fast_lg_bounds(y_begin, y_end, yy, giy, liy);

   double x0 = tx(*lix);
   double ix = tx(xx);
   double x1 = tx(*gix);
   // SAME CODE OF LINEAR END

   //----CUBIC START----
   _It3 in_z_begin0 = z_begin + (std::distance(x_begin, lix) * z_cols);
   _It3 in_z_end0   = in_z_begin0 + z_cols;
   _It3 in_z_begin1 = z_begin + (std::distance(x_begin, gix) * z_cols);
   _It3 in_z_end1   = in_z_begin1 + z_cols;

   std::vector<V> InY(std::distance(y_begin, y_end));
   std::copy(y_begin, y_end, InY.begin());
      
   std::vector<Z> InZ0(z_cols);
   std::copy(in_z_begin0, in_z_end0, InZ0.begin());

   std::vector<Z> InZ1(z_cols);
   std::copy(in_z_begin1, in_z_end1, InZ1.begin());
   
   std::vector<V> yyv(1,yy); 
   std::vector<double> z0v; 
   z0v=bspline::interpNaturalBSpline(InY, InZ0, yyv);
   double z0=z0v[0];

   std::vector<double> z1v; 
   z1v=bspline::interpNaturalBSpline(InY, InZ1, yyv);
   double z1=z1v[0];
   //----CUBIC SPLINE END----
   return cont_interp::detail::linearInterp(x0, ix, x1, z0, z1);

   /*
   // The following code interpolates linearly along times and then it applies one cspline interpolation
   // it's more efficient AND produces the same result of the previous implementation. 
   std::vector<Z> izs(z_cols);
   if (x1==x0)
      for (size_t c=0; c<z_cols; c++)
         izs[c] =*( in_z_begin0 + c);
   else
      for (size_t c=0; c<z_cols; c++)
         izs[c] = *( in_z_begin0 + c) + (ix-x0)/(x1-x0) *(*( in_z_begin1 + c)- *( in_z_begin0 + c) );
 
   z1v=bspline::interpNaturalBSpline(InY, izs, yyv);
   return z1v[0];
  */
}



} // cont_interp_fo

#endif //_ECONTINTERPFO_HPP__

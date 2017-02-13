//eContInterp.hpp
#ifndef _ECONTINTERP_HPP__
#define _ECONTINTERP_HPP__
#include <vector>
#include <iterator>

#include <boost/utility.hpp>

#include "cError.h"
#include "fast_upper_bound.hpp"
#include "fast_lower_bound.hpp"

namespace cont_interp {

namespace detail {
template<typename T, typename V>
INLINE V linearInterp(T prevX, T x, T succX, V prevY, V succY)
{
   T denx = succX - prevX;
   T theta = denx ? (x - prevX) / denx : 0.5;
   V diff = succY - prevY;

   return prevY + (theta * diff);
}
} // namespace detail

//-------+---------+---------+---------+---------+---------+---------+---------+
// linear
//-------+---------+---------+---------+---------+---------+---------+---------+
template<
   typename _It1, 
   typename _It2
>
typename std::iterator_traits<_It2>::value_type linear(
	_It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   const typename std::iterator_traits<_It2>::value_type &xx
)
{
   typedef typename std::iterator_traits<_It2>::value_type V;

   if(std::distance(x_begin, x_end) != std::distance(y_begin, y_end))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix = x_begin + pdg::fast_lower_bound(x_begin, x_end, xx);

   if(gix == x_end) return *boost::prior(y_end);
   if(xx == *gix) return *(y_begin + std::distance(x_begin, gix));
   if(gix == x_begin) throw pdg::Error(2, "Interpolation point before first abscissa value");

   _It1 lix = boost::prior(gix);

   V y0 = *(y_begin + std::distance(x_begin, lix));
   V y1 = *(y_begin + std::distance(x_begin, gix));

   return detail::linearInterp(*lix, xx, *gix, y0, y1);
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// linear_in_flat_out
//-------+---------+---------+---------+---------+---------+---------+---------+
// linear interpolator inside x_begin and x_end flat outside
//-------+---------+---------+---------+---------+---------+---------+---------+
template<
   typename _It1, 
   typename _It2
>
typename std::iterator_traits<_It2>::value_type linear_in_flat_out(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   const typename std::iterator_traits<_It1>::value_type &xx
)
{
   typedef typename std::iterator_traits<_It1>::value_type T;
   typedef typename std::iterator_traits<_It2>::value_type V;

   if(std::distance(x_begin, x_end) != std::distance(y_begin, y_end))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix = x_begin + pdg::fast_lower_bound(x_begin, x_end, xx);

   if(gix == x_end) 
      return *boost::prior(y_end);
   if(xx == *gix) 
      return *(y_begin + std::distance(x_begin, gix));
   if(gix == x_begin) 
      return *y_begin;

   _It1 lix = boost::prior(gix);

   T x0 = *lix;
   T ix = xx;
   T x1 = *gix;
   V y0 = *(y_begin + std::distance(x_begin, lix));
   V y1 = *(y_begin + std::distance(x_begin, gix));

   return detail::linearInterp(x0, ix, x1, y0, y1);
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// linear_range
//-------+---------+---------+---------+---------+---------+---------+---------+
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
   bool ordered = true
)
{
   typedef typename std::iterator_traits<_It1>::value_type T;
   typedef typename std::iterator_traits<_It2>::value_type V;

   _It1 gix = x_begin;
   for(_It3 xx_i = xx_begin; xx_i != xx_end; ++xx_i, ++yy_begin) {
      gix = ordered ? gix : x_begin;
      gix = gix + pdg::fast_lower_bound(gix, x_end, *xx_i);

      if(gix == x_end) {
         if (ordered) {
            double val = *boost::prior(y_begin + std::distance(x_begin, x_end));
            for (;xx_i != xx_end; ++xx_i, ++yy_begin)
               *yy_begin = val;

            return;
         } 
         else
            *yy_begin = *boost::prior(y_begin + std::distance(x_begin, x_end));
      }
      else if(*xx_i == *gix) 
         *yy_begin = *(y_begin + std::distance(x_begin, gix));
      else if(gix == x_begin) 
         *yy_begin = *y_begin;
      else {
         _It1 lix = boost::prior(gix);

         T x0 = *lix;
         T ix = *xx_i;
         T x1 = *gix;
         V y0 = *(y_begin + std::distance(x_begin, lix));
         V y1 = *(y_begin + std::distance(x_begin, gix));

         *yy_begin = detail::linearInterp(x0, ix, x1, y0, y1);
      }
   }
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// kruger_preconditioning
//-------+---------+---------+---------+---------+---------+---------+---------+
// Note: y can point to any arithmetic type.
//-------+---------+---------+---------+---------+---------+---------+---------+
template<
   typename _It1,
   typename _It2,
   typename _It3
>
void kruger_preconditioning(
   _It1 in_x_begin, _It1 in_x_end,
   _It2 in_y_begin,
   _It3 out_a_begin, _It3 out_b_begin, _It3 out_c_begin
)
{
   typedef typename std::iterator_traits<_It1>::value_type T;
   typedef typename std::iterator_traits<_It2>::value_type V;
   const size_t n(std::distance(in_x_begin, in_x_end));
	if(n <= 3) 
      throw pdg::Error(2, "#Error in cont_interp::kruger, too few input points");
   std::vector<V> tmp(n);
	const size_t nminusone = n - 1;
   std::vector<T> dx(nminusone);
   std::vector<V> s(nminusone);
   
   {
      _It2 y_pos = in_y_begin;
      typename std::vector<T>::iterator dx_pos = dx.begin();
      typename std::vector<V>::iterator s_pos = s.begin();
      for(_It1 x_pos = in_x_begin;
         x_pos != boost::prior(in_x_end); 
         ++x_pos, ++y_pos, ++dx_pos, ++s_pos
      ) {
         *dx_pos = *(boost::next(x_pos)) - *x_pos;
         *s_pos = (*(boost::next(y_pos)) - *y_pos) / *dx_pos; 
      }
   }

   // intermediate points
   for(size_t i = 1; i < nminusone; ++i) {
      if(s[i - 1] * s[i] <= 0.0)
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
   for(unsigned int i = 0; i < nminusone; ++i, ++out_a_begin, ++out_b_begin, ++out_c_begin) {
      *out_a_begin = tmp[i];
      *out_b_begin = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      *out_c_begin = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// kruger_preconditioned
//-------+---------+---------+---------+---------+---------+---------+---------+
// Note: y,a,b,c can point to any arithmetic type.
//-------+---------+---------+---------+---------+---------+---------+---------+
template<
   class _It1, 
   class _It2, 
   class _It3, 
   class _It4, 
   class _It5
>
INLINE void kruger_preconditioned(
   _It1 in_x_begin, _It1 in_x_end,
   _It2 in_y_begin,
   _It3 a_begin, _It3 b_begin, _It3 c_begin,                            
   _It4 out_x_begin, _It4 out_x_end,
   _It5 out_y_begin,
   bool ordered = false,
   bool flat_extrapolation = false
)
{ 
   typedef typename std::iterator_traits<_It1>::value_type T;
   typedef typename std::iterator_traits<_It2>::value_type V;

   _It5 y_pos = out_y_begin;
   _It1 lix = in_x_begin;
   _It1 in_x_last = boost::prior(in_x_end);
   T in_x_begin_v = *in_x_begin;
   T in_x_last_v = *in_x_last;

   if(flat_extrapolation) {
      const V &in_y_begin_v = *in_y_begin;
      const V &in_y_last_v = *(in_y_begin + (in_x_last - in_x_begin));

      size_t j;
      for(_It4 x_pos = out_x_begin;
         x_pos != out_x_end;
         ++x_pos,
         ++y_pos
      ) {
         if(*x_pos < in_x_begin_v)
            *y_pos = in_y_begin_v;
         else if(*x_pos > in_x_last_v)
            *y_pos = in_y_last_v;
         else {
            if(ordered) {
               //lix = std::upper_bound(lix, in_x_last, *x_pos);
               //j = std::distance(in_x_begin, lix) - 1;
               size_t i = pdg::fast_upper_bound(lix, in_x_last, *x_pos);
               lix += i;
               j = (lix - in_x_begin) - 1;
            }
            else
               j = pdg::fast_upper_bound(in_x_begin, in_x_last, *x_pos) - 1;

            double dx = *x_pos - *(in_x_begin + j);
            *y_pos = *(in_y_begin + j) + dx * ( *(a_begin + j) + dx * ( *(b_begin + j) + dx * *(c_begin + j) ) );
         }
      }   
   }
   else {
      size_t j;
      for(_It4 x_pos = out_x_begin;
         x_pos != out_x_end;
         ++x_pos,
         ++y_pos
      ) {
         if(ordered)
            j = (*x_pos < in_x_begin_v) ? 0 : (*x_pos > in_x_last_v) ? in_x_end - in_x_begin - 2 : pdg::fast_upper_bound(lix, in_x_last, *x_pos) - 1;
         else
            j = (*x_pos < in_x_begin_v) ? 0 : (*x_pos > in_x_last_v) ? in_x_end - in_x_begin - 2 : pdg::fast_upper_bound(in_x_begin, in_x_last, *x_pos) - 1;

         double dx = *x_pos - *(in_x_begin + j);
         *y_pos = *(in_y_begin + j) + dx * ( *(a_begin + j) + dx * ( *(b_begin + j) + dx * *(c_begin + j) ) );
      } 
   }
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// kruger_preconditioned_central
//-------+---------+---------+---------+---------+---------+---------+---------+
// Note: y,a,b,c can point to any arithmetic type.
//-------+---------+---------+---------+---------+---------+---------+---------+
template<
   class _It1, 
   class _It2, 
   class _It3, 
   class _It4, 
   class _It5
>
INLINE void kruger_preconditioned_central(
   _It1 in_x_begin, _It1 in_x_end,
   _It2 in_y_begin,
   _It3 a_begin, _It3 b_begin, _It3 c_begin,                            
   _It4 out_x_begin, _It4 out_x_end,
   _It5 out_y_begin,
   bool ordered = false,
   bool flat_extrapolation = false
)
{ 
   typedef typename std::iterator_traits<_It1>::value_type T;
   typedef typename std::iterator_traits<_It2>::value_type V;

   _It5 y_pos = out_y_begin;
   _It1 lix = in_x_begin;
   _It1 in_x_last = boost::prior(in_x_end);
   const T in_x_begin_v = *in_x_begin;
   const T in_x_last_v = *in_x_last;

   if(flat_extrapolation) {
      const V &in_y_begin_v = *in_y_begin;
      const V &in_y_last_v = *(in_y_begin + (in_x_last - in_x_begin));

      size_t j;
      for(_It4 x_pos = out_x_begin;
         x_pos != out_x_end;
         ++x_pos,
         ++y_pos
      ) {
         if(*x_pos < in_x_begin_v)
            *y_pos = in_y_begin_v;
         else if(*x_pos > in_x_last_v)
            *y_pos = in_y_last_v;
         else {
            if(ordered) {
               //lix = std::upper_bound(lix, in_x_last, *x_pos);
               //j = std::distance(in_x_begin, lix) - 1;
               size_t i = pdg::fast_upper_bound_central(lix, in_x_last, *x_pos);
               lix += i;
               j = (lix - in_x_begin) - 1;
            }
            else
               j = pdg::fast_upper_bound_central(in_x_begin, in_x_last, *x_pos) - 1;

            const double dx = *x_pos - *(in_x_begin + j);
            *y_pos = *(in_y_begin + j) + dx * ( *(a_begin + j) + dx * ( *(b_begin + j) + dx * *(c_begin + j) ) );
         }
      }   
   }
   else {
      size_t j;
      for(_It4 x_pos = out_x_begin;
         x_pos != out_x_end;
         ++x_pos,
         ++y_pos
      ) {
         if(ordered)
            j = (*x_pos < in_x_begin_v) ? 0 : (*x_pos > in_x_last_v) ? in_x_end - in_x_begin - 2 : pdg::fast_upper_bound_central(lix, in_x_last, *x_pos) - 1;
         else
            j = (*x_pos < in_x_begin_v) ? 0 : (*x_pos > in_x_last_v) ? in_x_end - in_x_begin - 2 : pdg::fast_upper_bound_central(in_x_begin, in_x_last, *x_pos) - 1;

         const double dx = *x_pos - *(in_x_begin + j);
         *y_pos = *(in_y_begin + j) + dx * ( *(a_begin + j) + dx * ( *(b_begin + j) + dx * *(c_begin + j) ) );
      } 
   }
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// kruger
//-------+---------+---------+---------+---------+---------+---------+---------+
// Note: y can point to any arithmetic type.
//-------+---------+---------+---------+---------+---------+---------+---------+
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
   _It4 out_y_begin
)
{
   typedef typename std::iterator_traits<_It1>::value_type T;
   typedef typename std::iterator_traits<_It2>::value_type V;

   if(std::distance(in_x_begin, in_x_end) != std::distance(in_y_begin, in_y_end))
      throw pdg::Error(2, "The InX e InY vectors must have the same size.");
   
   size_t n = std::distance(in_x_begin, in_x_end);
   size_t nminusone = n-1;
  
   std::vector<V> a(nminusone), b(nminusone), c(nminusone);

   kruger_preconditioning(
      in_x_begin, in_x_end, 
      in_y_begin, 
      a.begin(), b.begin(), c.begin()
   );
   kruger_preconditioned(
      in_x_begin, in_x_end, 
      in_y_begin, 
      a.begin(), b.begin(), c.begin(), 
      out_x_begin, out_x_end, 
      out_y_begin
   );
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// kruger_range
//-------+---------+---------+---------+---------+---------+---------+---------+
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
   _It4 out_y_begin
)
{
   unsigned int n = std::distance(in_x_begin, in_x_end);
   if(n <= 3) 
      throw pdg::Error(2, "#Error in cont_interp::kruger, too few input points");
   
   std::vector<double> tmp(n);
	size_t nminusone = n - 1;
   std::vector<double> dx(nminusone), s(nminusone);
   
   {
      _It2 y_pos = in_y_begin;
      std::vector<double>::iterator dx_pos = dx.begin();
      std::vector<double>::iterator s_pos = s.begin();
      for(_It1 x_pos = in_x_begin;
          x_pos != boost::prior(in_x_end); 
          ++x_pos, ++y_pos, ++dx_pos, ++s_pos
      ) {
         *dx_pos = *(boost::next(x_pos)) - *x_pos;
         *s_pos = (*(boost::next(y_pos)) - *y_pos) / *dx_pos; 
      }
   }

   // intermediate points
   for (size_t i = 1; i < nminusone; ++i) {
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
   for(size_t i = 0; i < nminusone; ++i) {
      a[i] = tmp[i];
      b[i] = (3.0 * s[i] - tmp[i + 1] - 2.0 * tmp[i]) / dx[i];
      c[i] = (tmp[i + 1] + tmp[i] - 2.0 * s[i]) / (dx[i] * dx[i]);
   }
   {
      _It4 y_pos = out_y_begin;
      for(_It3 x_pos = out_x_begin;
          x_pos != out_x_end;
          ++x_pos,
          ++y_pos) {
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

namespace detail {

template< 
   typename X,
   typename Y,
   typename Z
>
Z linearInterp(
   X prevX, X x, X succX, 
   Y prevY, Y y, Y succY,
   Z Z00, Z Z01, Z Z10, Z Z11
)
{
   X denx = succX - prevX;
   X wxh = denx ? (x - prevX) / denx : 0.5;
   X wxl = denx ? (succX - x) / denx : 0.5;

   Y deny = succY - prevY;
   Y wyh = deny ? (y - prevY) / deny : 0.5;
   Y wyl = deny ? (succY - y) / deny : 0.5;

   return Z00 * wxl * wyl + Z01 * wxl * wyh + Z10 * wxh * wyl + Z11 * wxh * wyh;
}
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// kruger_range
//-------+---------+---------+---------+---------+---------+---------+---------+
template<
   typename _It1, 
   typename _It2, 
   typename _It3
>
typename std::iterator_traits<_It3>::value_type linear(
   _It1 x_begin, _It1 x_end,
   _It2 y_begin, _It2 y_end,
   _It3 z_begin, _It3 z_end,
   size_t z_cols,
   const typename std::iterator_traits<_It1>::value_type &xx, 
   const typename std::iterator_traits<_It2>::value_type &yy
)
{
   typedef typename std::iterator_traits<_It1>::value_type X;
   typedef typename std::iterator_traits<_It2>::value_type Y;
   typedef typename std::iterator_traits<_It3>::value_type Z;

   if((std::distance(y_begin, y_end) != z_cols) || (std::distance(x_begin, x_end) != (std::distance(z_begin, z_end) / z_cols)))
      throw pdg::Error(2, "The vectors' size must be the same.");

   _It1 gix = x_begin + pdg::fast_lower_bound(x_begin, x_end, xx);

   _It1 lix;
   if(gix == x_begin) lix = gix;
   else if(gix == x_end) lix = --gix;
   else lix = boost::prior(gix);

   _It2 giy = y_begin + pdg::fast_lower_bound(y_begin, y_end, yy);

   _It2 liy;
   if(giy == y_begin) liy = giy;
   else if(giy == y_end) liy = --giy;
   else liy = boost::prior(giy);

   X x0 = *lix;
   X ix = xx;
   X x1 = *gix;
   Y y0 = *liy;
   Y iy = yy;
   Y y1 = *giy;

   Z z00 = *(z_begin + ((std::distance(x_begin, lix) * z_cols) + std::distance(y_begin, liy)));
   Z z01 = *(z_begin + ((std::distance(x_begin, lix) * z_cols) + std::distance(y_begin, giy)));
   Z z10 = *(z_begin + ((std::distance(x_begin, gix) * z_cols) + std::distance(y_begin, liy)));
   Z z11 = *(z_begin + ((std::distance(x_begin, gix) * z_cols) + std::distance(y_begin, giy)));

   return detail::linearInterp(x0, ix, x1, y0, iy, y1, z00, z01, z10, z11);
}

} // cont_interp

#endif //_ECONTINTERP_HPP__

/* floaterf.c: normal distribution integrals erf and the like */
/*
    Copyright (C) 2007 Wolf Lammen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.


    You may contact the author by:
       e-mail:  ookami1 <at> gmx <dot> de
       mail:  Wolf Lammen
              Oertzweg 45
              22307 Hamburg
              Germany

*************************************************************************/

#include "floaterf.h"
#include "floatconst.h"
#include "floatcommon.h"
#include "floatexp.h"

/* checks the quality of the asymptotic series for erfc.
   If the ratio of two subsequent summands from the series
   (the convergence rate) should not fall below `ratio'
   for a desired result precision (represented by digits), the number
   of summands n must not be greater than
    n <= (`digits'*ln 10 + 0.5 * ln 2)/(1 - ln `ratio')
   and the parameter x has to fullfil
    x >= sqrt(n/`ratio')
   `ratio' must be a value < 1, If you pick a value close to
   1, you finally have to add quite a lot of summands from the
   series (in low precision), that affect a few digits at the low
   end of the result only. On the other hand, choosing a good
   convergence rate pushes the validity range of the series towards
   larger x.
   Here, the convergence rate is chosen to be 0.5, meaning that the
   addition of a summand from the series at least halfs the magnitude
   of the tail of the series.
   The evaluation is carried out in low precision using interger
   arithmetic rather than floating point data.
   For a 100 digit result the lower boundary of the range of the
   asymptotic series (truncated when the convergence rate falls below 0.5)
   is x > approx. 16.5.
   The above formulas estimate the limit x slightly too small, especially
   when `digits' is small. So, to compensate for that, r should be at least
   <= 0.92 */
static char
_asymptotic_good(
  floatnum x,
  int digits)
{
  /* all constants scaled by 10000 */
  /* 1/ratio */
#define RATIO 20000
  /* (1 - ln ratio) */
#define C_RATIO 16931
  /* ln 10 */
#define LN10 23026
  /* 0.5*ln 2 */
#define LN2DIV2 3466

  int n, ix;

  if (!float_isvalidexp(float_getexponent(x) + 2)
      || (digits == 1 && float_cmp(x, &c2) >= 0))
    return 1;
  /* 10000 * n/ratio */
  n = RATIO*((digits * LN10 + LN2DIV2) / C_RATIO);
  float_addexp(x, 2);
  ix = float_asinteger(x);
  float_addexp(x, -2);
  return ix == 0 || ix >= 0x10000 || ix * ix >= n;
}

char
_erf(
  floatnum x,
  int digits)
{
  int workprec, expx;
  signed char sign;

  sign = float_getsign(x);
  float_abs(x);
  if (float_cmp(x, &c1Div2) > 0)
  {
    expx = float_getexponent(x);
    if (expx >= 0x1000)
      expx = 0x1000;
    workprec = digits - ((expx * expx * 73) >> 5);
    if (workprec < 0 || !_erfc(x, workprec))
      float_setzero(x);
    float_sub(x, &c1, x, digits);
  }
  else
  {
    erfnear0(x, digits);
    float_mul(x, x, &c2DivSqrtPi, digits+1);
  }
  float_setsign(x, sign);
  return 1;
}

char
_erfc(
  floatnum x,
  int digits)
{
  floatstruct tmp, t2, t3;
  int expx, prec;
  char result;

  expx = float_getexponent(x);
  if (expx < -digits || float_iszero(x))
  {
    float_copy(x, &c1, EXACT);
    return 1;
  }
  if (float_cmp(x, &c1Div2) <= 0)
  {
    _erf(x, digits + expx);
    float_sub(x, &c1, x, digits);
    return 1;
  }
  float_create(&tmp);
  if (_asymptotic_good(x, digits))
  {
    result = float_mul(&tmp, x, x, digits+1)
        && _exp(&tmp, digits+1)
        && float_mul(&tmp, &tmp, x, digits+1)
        && float_div(&tmp, &c1DivSqrtPi, &tmp, digits);
    if (!result)
      float_error = FLOAT_UNDERFLOW;
    result = result && erfcbigx(x, digits);
    if (!result)
      float_error = FLOAT_UNSTABLE;
    result = result && float_mul(x, x, &tmp, digits+1);
  }
  else
  {
    float_create(&t2);
    float_create(&t3);
    float_mul(&t2, x, x, digits+1);
    float_copy(&tmp, &t2, EXACT);
    erfcsum(&tmp, digits);
    float_add(&tmp, &tmp, &tmp, digits+1);
    float_copy(&t3, &t2, EXACT);
    float_reciprocal(&t2, digits);
    float_add(&tmp, &tmp, &t2, digits);
    float_neg(&t3);
    _exp(&t3, digits+1);
    float_mul(&t3, &t3, &tmp, digits);
    float_mul(&tmp, &erfcalpha, x, digits);
    float_mul(&t3, &tmp, &t3, digits);
    float_mul(&t3, &c1DivPi, &t3, digits);
    /* quick estimate to find the right working precision */
    float_div(&tmp, x, &erfcalpha, 4);
    float_mul(&tmp, &tmp, &c2Pi, 4);
    float_div(&tmp, &tmp, &cLn10, 4);
    prec = digits - float_getexponent(&t3) - float_asinteger(&tmp) + 1;
    /* add correction term */
    result = prec <= digits;
    if (result && prec > 0)
    {
      float_div(&tmp, x, &erfcalpha, prec+2);
      float_mul(&tmp, &tmp, &c2Pi, prec+2);
      _exp(&tmp, prec);
      float_sub(&tmp, &c1, &tmp, prec);
      float_div(&tmp, &c2, &tmp, prec);
      float_add(&t3, &t3, &tmp, digits);
    }
    float_free(&t2);
    float_move(x, &t3);
  }
  float_free(&tmp);
  if (!result)
    float_setnan(x);
  return result;
}

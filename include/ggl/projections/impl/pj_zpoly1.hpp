#ifndef _PROJECTIONS_ZPOLY1_HPP
#define _PROJECTIONS_ZPOLY1_HPP

// Generic Geometry Library - projections (based on PROJ4)
// This file is manually converted from PROJ4

// Copyright Barend Gehrels 1995-2009, Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Geometry Library by Barend Gehrels (Geodan, Amsterdam)

// Original copyright notice:

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#include <ggl/projections/impl/projects.hpp>


namespace ggl { namespace projection { namespace detail {

    /* evaluate complex polynomial */

    /* note: coefficients are always from C_1 to C_n
    **    i.e. C_0 == (0., 0)
    **    n should always be >= 1 though no checks are made
    */
    inline COMPLEX
    pj_zpoly1(COMPLEX z, COMPLEX *C, int n)
    {
        COMPLEX a;
        double t;

        a = *(C += n);
        while (n-- > 0)
        {
            a.r = (--C)->r + z.r * (t = a.r) - z.i * a.i;
            a.i = C->i + z.r * a.i + z.i * t;
        }
        a.r = z.r * (t = a.r) - z.i * a.i;
        a.i = z.r * a.i + z.i * t;
        return a;
    }

    /* evaluate complex polynomial and derivative */
    inline COMPLEX
    pj_zpolyd1(COMPLEX z, COMPLEX *C, int n, COMPLEX *der)
    {
        double t;
        bool first = true;

        COMPLEX a = *(C += n);
        COMPLEX b = a;
        while (n-- > 0)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                b.r = a.r + z.r * (t = b.r) - z.i * b.i;
                b.i = a.i + z.r * b.i + z.i * t;
            }
            a.r = (--C)->r + z.r * (t = a.r) - z.i * a.i;
            a.i = C->i + z.r * a.i + z.i * t;
        }
        b.r = a.r + z.r * (t = b.r) - z.i * b.i;
        b.i = a.i + z.r * b.i + z.i * t;
        a.r = z.r * (t = a.r) - z.i * a.i;
        a.i = z.r * a.i + z.i * t;
        *der = b;
        return a;
    }

}}} // namespace ggl::projection::impl
#endif

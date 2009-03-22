#ifndef _PROJECTIONS_MOLL_HPP
#define _PROJECTIONS_MOLL_HPP

// Generic Geometry Library - projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright Barend Gehrels (1995-2009), Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande (2008-2009)
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

#include <geometry/projections/impl/base_static.hpp>
#include <geometry/projections/impl/base_dynamic.hpp>
#include <geometry/projections/impl/projects.hpp>
#include <geometry/projections/impl/factory_entry.hpp>

namespace projection
{
	#ifndef DOXYGEN_NO_IMPL
	namespace impl
	{
		namespace moll
		{
			static const int MAX_ITER = 10;
			static const double LOOP_TOL = 1e-7;

			struct par_moll
			{
				double C_x, C_y, C_p;
			};

			// template class, using CRTP to implement forward/inverse
			template <typename LL, typename XY, typename PAR>
			struct base_moll_spheroid : public base_t_fi<base_moll_spheroid<LL, XY, PAR>, LL, XY, PAR>
			{

				typedef typename base_t_fi<base_moll_spheroid<LL, XY, PAR>, LL, XY, PAR>::LL_T LL_T;
				typedef typename base_t_fi<base_moll_spheroid<LL, XY, PAR>, LL, XY, PAR>::XY_T XY_T;

				par_moll m_proj_parm;

				inline base_moll_spheroid(const PAR& par)
					: base_t_fi<base_moll_spheroid<LL, XY, PAR>, LL, XY, PAR>(*this, par) {}

				inline void fwd(LL_T& lp_lon, LL_T& lp_lat, XY_T& xy_x, XY_T& xy_y) const
				{
					double k, V;
					int i;

					k = this->m_proj_parm.C_p * sin(lp_lat);
					for (i = MAX_ITER; i ; --i) {
						lp_lat -= V = (lp_lat + sin(lp_lat) - k) /
							(1. + cos(lp_lat));
						if (fabs(V) < LOOP_TOL)
							break;
					}
					if (!i)
						lp_lat = (lp_lat < 0.) ? -HALFPI : HALFPI;
					else
						lp_lat *= 0.5;
					xy_x = this->m_proj_parm.C_x * lp_lon * cos(lp_lat);
					xy_y = this->m_proj_parm.C_y * sin(lp_lat);
				}

				inline void inv(XY_T& xy_x, XY_T& xy_y, LL_T& lp_lon, LL_T& lp_lat) const
				{
					lp_lat = aasin(xy_y / this->m_proj_parm.C_y);
					lp_lon = xy_x / (this->m_proj_parm.C_x * cos(lp_lat));
					lp_lat += lp_lat;
					lp_lat = aasin((lp_lat + sin(lp_lat)) / this->m_proj_parm.C_p);
				}
			};

			template <typename PAR>
			void setup(PAR& par, par_moll& proj_parm, double p)
			{
				double r, sp, p2 = p + p;
				par.es = 0;
				sp = sin(p);
				r = sqrt(TWOPI * sp / (p2 + sin(p2)));
				proj_parm.C_x = 2. * r / PI;
				proj_parm.C_y = r / sp;
				proj_parm.C_p = p2 + sin(p2);
				// par.inv = s_inverse;
				// par.fwd = s_forward;
			}


			// Mollweide
			template <typename PAR>
			void setup_moll(PAR& par, par_moll& proj_parm)
			{
				setup(par, proj_parm, HALFPI);
			}

			// Wagner IV
			template <typename PAR>
			void setup_wag4(PAR& par, par_moll& proj_parm)
			{
				setup(par, proj_parm, PI/3.);
			}

			// Wagner V
			template <typename PAR>
			void setup_wag5(PAR& par, par_moll& proj_parm)
			{
				par.es = 0;
				proj_parm.C_x = 0.90977;
				proj_parm.C_y = 1.65014;
				proj_parm.C_p = 3.00896;
				// par.inv = s_inverse;
				// par.fwd = s_forward;
			}

		} // namespace moll
	} //namespaces impl
	#endif // doxygen

	/*!
		\brief Mollweide projection
		\ingroup projections
		\tparam LL latlong point type
		\tparam XY xy point type
		\tparam PAR parameter type
		\par Projection characteristics
		 - Pseudocylindrical
		 - Spheroid
		\par Example
		\image html ex_moll.gif
	*/
	template <typename LL, typename XY, typename PAR = parameters>
	struct moll_spheroid : public impl::moll::base_moll_spheroid<LL, XY, PAR>
	{
		inline moll_spheroid(const PAR& par) : impl::moll::base_moll_spheroid<LL, XY, PAR>(par)
		{
			impl::moll::setup_moll(this->m_par, this->m_proj_parm);
		}
	};

	/*!
		\brief Wagner IV projection
		\ingroup projections
		\tparam LL latlong point type
		\tparam XY xy point type
		\tparam PAR parameter type
		\par Projection characteristics
		 - Pseudocylindrical
		 - Spheroid
		\par Example
		\image html ex_wag4.gif
	*/
	template <typename LL, typename XY, typename PAR = parameters>
	struct wag4_spheroid : public impl::moll::base_moll_spheroid<LL, XY, PAR>
	{
		inline wag4_spheroid(const PAR& par) : impl::moll::base_moll_spheroid<LL, XY, PAR>(par)
		{
			impl::moll::setup_wag4(this->m_par, this->m_proj_parm);
		}
	};

	/*!
		\brief Wagner V projection
		\ingroup projections
		\tparam LL latlong point type
		\tparam XY xy point type
		\tparam PAR parameter type
		\par Projection characteristics
		 - Pseudocylindrical
		 - Spheroid
		\par Example
		\image html ex_wag5.gif
	*/
	template <typename LL, typename XY, typename PAR = parameters>
	struct wag5_spheroid : public impl::moll::base_moll_spheroid<LL, XY, PAR>
	{
		inline wag5_spheroid(const PAR& par) : impl::moll::base_moll_spheroid<LL, XY, PAR>(par)
		{
			impl::moll::setup_wag5(this->m_par, this->m_proj_parm);
		}
	};

	#ifndef DOXYGEN_NO_IMPL
	namespace impl
	{

		// Factory entry(s)
		template <typename LL, typename XY, typename PAR>
		class moll_entry : public impl::factory_entry<LL, XY, PAR>
		{
			public :
				virtual projection<LL, XY>* create_new(const PAR& par) const
				{
					return new base_v_fi<moll_spheroid<LL, XY, PAR>, LL, XY, PAR>(par);
				}
		};

		template <typename LL, typename XY, typename PAR>
		class wag4_entry : public impl::factory_entry<LL, XY, PAR>
		{
			public :
				virtual projection<LL, XY>* create_new(const PAR& par) const
				{
					return new base_v_fi<wag4_spheroid<LL, XY, PAR>, LL, XY, PAR>(par);
				}
		};

		template <typename LL, typename XY, typename PAR>
		class wag5_entry : public impl::factory_entry<LL, XY, PAR>
		{
			public :
				virtual projection<LL, XY>* create_new(const PAR& par) const
				{
					return new base_v_fi<wag5_spheroid<LL, XY, PAR>, LL, XY, PAR>(par);
				}
		};

		template <typename LL, typename XY, typename PAR>
		inline void moll_init(impl::base_factory<LL, XY, PAR>& factory)
		{
			factory.add_to_factory("moll", new moll_entry<LL, XY, PAR>);
			factory.add_to_factory("wag4", new wag4_entry<LL, XY, PAR>);
			factory.add_to_factory("wag5", new wag5_entry<LL, XY, PAR>);
		}

	} // namespace impl
	#endif // doxygen

}

#endif // _PROJECTIONS_MOLL_HPP

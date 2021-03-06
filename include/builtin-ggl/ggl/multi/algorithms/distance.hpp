// Generic Geometry Library
//
// Copyright Barend Gehrels 1995-2009, Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GGL_MULTI_ALGORITHMS_DISTANCE_HPP
#define GGL_MULTI_ALGORITHMS_DISTANCE_HPP


#include <boost/numeric/conversion/bounds.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <ggl/multi/core/tags.hpp>
#include <ggl/multi/core/is_multi.hpp>
#include <ggl/multi/core/geometry_id.hpp>
#include <ggl/multi/core/point_type.hpp>

#include <ggl/algorithms/distance.hpp>


namespace ggl {

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace distance {


template<typename Geometry, typename MultiGeometry, typename Strategy>
struct distance_single_to_multi
{
    typedef typename Strategy::return_type return_type;
    static inline return_type apply(Geometry const& geometry,
                MultiGeometry const& multi,
                Strategy const& strategy)
    {
        using namespace boost;

        return_type mindist = make_distance_result<return_type>(
                        numeric::bounds<typename select_coordinate_type<Geometry, MultiGeometry>::type>::highest());
        typedef typename range_const_iterator<MultiGeometry>::type iterator;
        for(iterator it = begin(multi); it != end(multi); ++it)
        {
            return_type dist = ggl::distance(geometry, *it);
            if (dist < mindist)
            {
                mindist = dist;
            }
        }

        return mindist;
    }
};

template<typename Multi1, typename Multi2, typename Strategy>
struct distance_multi_to_multi
{
    typedef typename Strategy::return_type return_type;

    static inline return_type apply(Multi1 const& multi1,
                Multi2 const& multi2, Strategy const& strategy)
    {
        using namespace boost;

        return_type mindist
            = make_distance_result<return_type>(
                numeric::bounds<typename select_coordinate_type<Multi1,
                            Multi2>::type>::highest());

        for(typename range_const_iterator<Multi1>::type it = begin(multi1);
                it != end(multi1);
                ++it)
        {
            return_type dist = distance_single_to_multi
                <
                    typename range_value<Multi1>::type,
                    Multi2,
                    Strategy
                >::apply(*it, multi2, strategy);
            if (dist < mindist)
            {
                mindist = dist;
            }
        }

        return mindist;
    }
};


}} // namespace detail::distance
#endif


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{

template <typename GeometryTag1, typename GeometryTag2,
        typename G1, typename G2, typename Strategy>
struct distance<GeometryTag1, GeometryTag2, G1, G2, strategy_tag_distance_point_point, Strategy, false, true>
    : detail::distance::distance_single_to_multi<G1, G2, Strategy>
{};

template <typename GeometryTag1, typename GeometryTag2,
        typename G1, typename G2, typename Strategy>
struct distance<GeometryTag1, GeometryTag2, G1, G2, strategy_tag_distance_point_point, Strategy, true, true>
    : detail::distance::distance_multi_to_multi<G1, G2, Strategy>
{};

} // namespace dispatch
#endif



} // namespace ggl


#endif // GGL_MULTI_ALGORITHMS_DISTANCE_HPP

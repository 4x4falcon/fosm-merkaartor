// Generic Geometry Library
//
// Copyright Barend Gehrels 1995-2009, Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GGL_MULTI_ALGORITHMS_OVERLAY_COPY_SEGMENTS_HPP
#define GGL_MULTI_ALGORITHMS_OVERLAY_COPY_SEGMENTS_HPP


#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <ggl/algorithms/overlay/copy_segments.hpp>

#include <ggl/multi/core/ring_type.hpp>
#include <ggl/multi/core/tags.hpp>


namespace ggl
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace copy_segments {


template
<
    typename MultiGeometry,
    typename SegmentIdentifier,
    typename RangeOut,
    typename Policy
>
struct copy_segments_multi
{
    static inline void apply(MultiGeometry const& multi_geometry,
            SegmentIdentifier const& seg_id, int to_index,
            RangeOut& current_output)
    {

        BOOST_ASSERT
            (
                seg_id.multi_index >= 0
                && seg_id.multi_index < boost::size(multi_geometry)
            );

        // Call the single-version
        Policy::apply(multi_geometry[seg_id.multi_index],
                    seg_id, to_index, current_output);
    }
};


}} // namespace detail::copy_segments
#endif // DOXYGEN_NO_DETAIL




#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch {



template <typename MultiPolygon, typename SegmentIdentifier, typename RangeOut>
struct copy_segments<multi_polygon_tag, MultiPolygon, SegmentIdentifier, RangeOut>
    : detail::copy_segments::copy_segments_multi
        <
            MultiPolygon,
            SegmentIdentifier,
            RangeOut,
            detail::copy_segments::copy_segments_polygon
                <
                    typename boost::range_value<MultiPolygon>::type,
                    SegmentIdentifier,
                    RangeOut
                >
        >
{};



} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH



} // namespace ggl

#endif // GGL_MULTI_ALGORITHMS_OVERLAY_COPY_SEGMENTS_HPP

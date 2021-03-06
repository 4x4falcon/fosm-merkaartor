// Generic Geometry Library
//
// Copyright Barend Gehrels 1995-2009, Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GGL_ALGORITHMS_OVERLAY_SEGMENT_IDENTIFIER_HPP
#define GGL_ALGORITHMS_OVERLAY_SEGMENT_IDENTIFIER_HPP

#include <vector>


#include <ggl/core/access.hpp>
#include <ggl/core/coordinate_dimension.hpp>

#include <ggl/strategies/distance_result.hpp>
#include <ggl/strategies/strategy_traits.hpp>

namespace ggl
{


// Internal struct to uniquely identify a segment
// on a linestring,ring
// or polygon (needs ring_index)
// or multi-geometry (needs multi_index)
struct segment_identifier
{
    inline segment_identifier()
        : source_index(-1)
        , multi_index(-1)
        , ring_index(-1)
        , segment_index(-1)
    {}

    inline segment_identifier(int src, int mul, int rin, int seg)
        : source_index(src)
        , multi_index(mul)
        , ring_index(rin)
        , segment_index(seg)
    {}

    inline bool operator<(segment_identifier const& other) const
    {
        return source_index != other.source_index ? source_index < other.source_index
            : multi_index !=other.multi_index ? multi_index < other.multi_index
            : ring_index != other.ring_index ? ring_index < other.ring_index
            : segment_index < other.segment_index
            ;
    }

    inline bool operator==(segment_identifier const& other) const
    {
        return source_index == other.source_index
            && segment_index == other.segment_index
            && ring_index == other.ring_index
            && multi_index == other.multi_index
            ;
    }

#ifdef GGL_DEBUG_INTERSECTION
    friend std::ostream& operator<<(std::ostream &os, segment_identifier const& seg_id)
    {
        std::cout
            << "s:" << seg_id.source_index
            << ", v:" << seg_id.segment_index // vertex
            ;
        if (seg_id.ring_index >= 0) std::cout << ", r:" << seg_id.ring_index;
        if (seg_id.multi_index >= 0) std::cout << ", m:" << seg_id.multi_index;
        return os;
    }
#endif

    int source_index;
    int multi_index;
    int ring_index;
    int segment_index;
};



} // namespace ggl

#endif // GGL_ALGORITHMS_OVERLAY_SEGMENT_IDENTIFIER_HPP

#pragma once

#include <span>
#include <sstream>
#include <vector>

#include <flow/core/application.hpp>
#include <flow/core/logger.hpp>
#include <flow/utility/iostream_view.hpp>
#include <flow/utility/stream_algorithm.hpp>

class utility_test final : public flow::application
{
public:
    struct interval
    {
        double begin;
        double end;
    };

    void start() final
    {
        using pos_type = flow::iostream_view::pos_type;
        using off_type = flow::iostream_view::off_type;

        std::stringstream ss;
        flow::iostream_view io_view{ ss };

        const std::vector<interval> intervals{
            { 0.0, 0.5 },
            { 0.5, 0.6 },
            { 0.6, 0.9 },
            { 1.0, 1.3 },
            { 1.8, 2.5 },
        };

        off_type start_offset = 0;
        io_view.seekp(start_offset);
        io_view.write(std::span(intervals));

        for (const auto value : { -5.3, 0.0, 0.3, 0.5, 0.8, 1.25, 2.5, 2.5, 2.8 })
        {
            auto idx = flow::stream_lower_bound<double, interval>(io_view,
                                                                  start_offset,
                                                                  0,
                                                                  intervals.size(),
                                                                  value,
                                                                  [](const interval& i, double val)
                                                                  {
                                                                      return i.begin < val;
                                                                  });

            if (idx != intervals.size())
            {
                FLOW_LOG_INFO("{} <= begin([{}, {})) at index {}", value, intervals[idx].begin, intervals[idx].end, idx);
            }
            else
            {
                FLOW_LOG_INFO("{} <= not found", value);
            }
        }

        const std::vector<int> values = { -5, -3, -1, 0, 1, 1, 1, 1, 3, 8, 9 };

        start_offset += static_cast<off_type>(intervals.size() * sizeof(interval));
        io_view.write(std::span(values));

        auto lower_index = flow::stream_lower_bound(io_view, start_offset, 0, values.size(), int{ 1 });
        auto upper_index = flow::stream_upper_bound(io_view, start_offset, 0, values.size(), int{ 1 });

        FLOW_LOG_INFO("values of 1 are in range [{}, {})", lower_index, upper_index);
        engine.quit();
    }
};

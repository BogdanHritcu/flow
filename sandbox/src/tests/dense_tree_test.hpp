#pragma once

#include <functional>
#include <sstream>

#include <flow/core/application.hpp>
#include <flow/core/logger.hpp>
#include <flow/utility/dense_tree.hpp>
#include <flow/utility/iostream_view.hpp>

class dense_tree_test final : public flow::application
{
public:
    void start() final
    {
        std::stringstream ss{};
        flow::iostream_view io_view(ss);

        using tree_type = flow::dense_tree<std::uint32_t, std::uint32_t, true, std::less<>>;
        tree_type tree1{};
        tree_type tree2{};

        // NOLINTBEGIN(*-avoid-magic-numbers)
        auto root = tree1.insert_after(tree1.before_begin(), 20);
        tree1.insert_after(root, 2);
        tree1.insert_after(root, 4);
        tree1.insert_after(root, 3);
        tree1.insert_after(root, 10);
        tree1.insert_after(root, 5);
        tree1.insert_after(root, 1);
        // NOLINTEND(*-avoid-magic-numbers)

        io_view.seekp(0).serialize(tree1);
        io_view.seekg(0).deserialize(tree2);

        ss = std::stringstream{};
        for (const auto& node : tree1)
        {
            ss << node << " ";
        }
        FLOW_LOG_INFO("tree1: {}", ss.view());

        ss = std::stringstream{};
        for (const auto& node : tree2)
        {
            ss << node << " ";
        }
        FLOW_LOG_INFO("tree2: {}", ss.view());
    }
};

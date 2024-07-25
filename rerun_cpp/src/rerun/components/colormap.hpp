// DO NOT EDIT! This file was auto-generated by crates/build/re_types_builder/src/codegen/cpp/mod.rs
// Based on "crates/store/re_types/definitions/rerun/components/colormap.fbs".

#pragma once

#include "../result.hpp"

#include <cstdint>
#include <memory>

namespace arrow {
    class Array;
    class DataType;
    class SparseUnionBuilder;
} // namespace arrow

namespace rerun::components {
    /// **Component**: Colormap for mapping scalar values within a given range to a color.
    ///
    /// This provides a number of popular pre-defined colormaps.
    /// In the future, the Rerun Viewer will allow users to define their own colormaps,
    /// but currently the Viewer is limited to the types defined here.
    enum class Colormap : uint8_t {

        /// A simple black to white gradient.
        ///
        /// This is a sRGB gray gradient which is perceptually uniform.
        Grayscale = 1,

        /// The Inferno colormap from Matplotlib.
        ///
        /// This is a perceptually uniform colormap.
        /// It interpolates from black to red to bright yellow.
        Inferno = 2,

        /// The Magma colormap from Matplotlib.
        ///
        /// This is a perceptually uniform colormap.
        /// It interpolates from black to purple to white.
        Magma = 3,

        /// The Plasma colormap from Matplotlib.
        ///
        /// This is a perceptually uniform colormap.
        /// It interpolates from dark blue to purple to yellow.
        Plasma = 4,

        /// Google's Turbo colormap map.
        ///
        /// This is a perceptually non-uniform rainbow colormap addressing many issues of
        /// more traditional rainbow colormaps like Jet.
        /// It is more perceptually uniform without sharp transitions and is more colorblind-friendly.
        /// Details: <https://research.google/blog/turbo-an-improved-rainbow-colormap-for-visualization/>
        Turbo = 5,

        /// The Viridis colormap from Matplotlib
        ///
        /// This is a perceptually uniform colormap which is robust to color blindness.
        /// It interpolates from dark purple to green to yellow.
        Viridis = 6,

        /// Rasmusgo's Cyan to Yellow colormap
        ///
        /// This is a perceptually uniform colormap which is robust to color blindness.
        /// It is especially suited for visualizing signed values.
        /// It interpolates from cyan to blue to dark gray to brass to yellow.
        CyanToYellow = 7,
    };
} // namespace rerun::components

namespace rerun {
    template <typename T>
    struct Loggable;

    /// \private
    template <>
    struct Loggable<components::Colormap> {
        static constexpr const char Name[] = "rerun.components.Colormap";

        /// Returns the arrow data type this type corresponds to.
        static const std::shared_ptr<arrow::DataType>& arrow_datatype();

        /// Serializes an array of `rerun::components::Colormap` into an arrow array.
        static Result<std::shared_ptr<arrow::Array>> to_arrow(
            const components::Colormap* instances, size_t num_instances
        );

        /// Fills an arrow array builder with an array of this type.
        static rerun::Error fill_arrow_array_builder(
            arrow::SparseUnionBuilder* builder, const components::Colormap* elements,
            size_t num_elements
        );
    };
} // namespace rerun

// NOTE: This file was autogenerated by re_types_builder; DO NOT EDIT.
// Based on "crates/re_types/definitions/rerun/datatypes/annotation_info.fbs"

#pragma once

#include "../components/color.hpp"
#include "../components/label.hpp"

#include <arrow/type_fwd.h>
#include <cstdint>
#include <optional>
#include <utility>

namespace rerun {
    namespace datatypes {
        /// Annotation info annotating a class id or key-point id.
        ///
        /// Color and label will be used to annotate entities/keypoints which reference the id.
        /// The id refers either to a class or key-point id
        struct AnnotationInfo {
            ///`ClassId` or `KeypointId` to which this annotation info belongs.
            uint16_t id;

            /// The label that will be shown in the UI.
            std::optional<rerun::components::Label> label;

            /// The color that will be applied to the annotated entity.
            std::optional<rerun::components::Color> color;

          public:
            // Extensions to generated type defined in 'annotation_info_ext.cpp'

            AnnotationInfo(
                uint16_t _id, std::optional<std::string> _label = std::nullopt,
                std::optional<components::Color> _color = std::nullopt
            )
                : id(_id), label(std::move(_label)), color(_color) {}

            AnnotationInfo(uint16_t _id, components::Color _color)
                : id(_id), label(std::nullopt), color(_color) {}

          public:
            AnnotationInfo() = default;

            /// Returns the arrow data type this type corresponds to.
            static const std::shared_ptr<arrow::DataType>& to_arrow_datatype();

            /// Creates a new array builder with an array of this type.
            static arrow::Result<std::shared_ptr<arrow::StructBuilder>> new_arrow_array_builder(
                arrow::MemoryPool* memory_pool
            );

            /// Fills an arrow array builder with an array of this type.
            static arrow::Status fill_arrow_array_builder(
                arrow::StructBuilder* builder, const AnnotationInfo* elements, size_t num_elements
            );
        };
    } // namespace datatypes
} // namespace rerun
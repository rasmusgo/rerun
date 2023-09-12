// DO NOT EDIT! This file was auto-generated by crates/re_types_builder/src/codegen/cpp/mod.rs
// Based on "crates/re_types/definitions/rerun/testing/components/fuzzy.fbs".

#include "affix_fuzzer2.hpp"

#include "../datatypes/affix_fuzzer1.hpp"

#include <arrow/builder.h>
#include <arrow/table.h>
#include <arrow/type_fwd.h>
#include <rerun/arrow.hpp>

namespace rerun {
    namespace components {
        const char AffixFuzzer2::NAME[] = "rerun.testing.components.AffixFuzzer2";

        const std::shared_ptr<arrow::DataType> &AffixFuzzer2::arrow_datatype() {
            static const auto datatype = rerun::datatypes::AffixFuzzer1::arrow_datatype();
            return datatype;
        }

        Result<std::shared_ptr<arrow::StructBuilder>> AffixFuzzer2::new_arrow_array_builder(
            arrow::MemoryPool *memory_pool
        ) {
            if (!memory_pool) {
                return Error(ErrorCode::UnexpectedNullArgument, "Memory pool is null.");
            }

            return Result(rerun::datatypes::AffixFuzzer1::new_arrow_array_builder(memory_pool).value
            );
        }

        Error AffixFuzzer2::fill_arrow_array_builder(
            arrow::StructBuilder *builder, const AffixFuzzer2 *elements, size_t num_elements
        ) {
            if (!builder) {
                return Error(ErrorCode::UnexpectedNullArgument, "Passed array builder is null.");
            }
            if (!elements) {
                return Error(
                    ErrorCode::UnexpectedNullArgument,
                    "Cannot serialize null pointer to arrow array."
                );
            }

            static_assert(sizeof(rerun::datatypes::AffixFuzzer1) == sizeof(AffixFuzzer2));
            RR_RETURN_NOT_OK(rerun::datatypes::AffixFuzzer1::fill_arrow_array_builder(
                builder,
                reinterpret_cast<const rerun::datatypes::AffixFuzzer1 *>(elements),
                num_elements
            ));

            return Error::ok();
        }

        Result<rerun::DataCell> AffixFuzzer2::to_data_cell(
            const AffixFuzzer2 *instances, size_t num_instances
        ) {
            // TODO(andreas): Allow configuring the memory pool.
            arrow::MemoryPool *pool = arrow::default_memory_pool();

            auto builder_result = AffixFuzzer2::new_arrow_array_builder(pool);
            RR_RETURN_NOT_OK(builder_result.error);
            auto builder = std::move(builder_result.value);
            if (instances && num_instances > 0) {
                RR_RETURN_NOT_OK(
                    AffixFuzzer2::fill_arrow_array_builder(builder.get(), instances, num_instances)
                );
            }
            std::shared_ptr<arrow::Array> array;
            ARROW_RETURN_NOT_OK(builder->Finish(&array));

            auto schema = arrow::schema(
                {arrow::field(AffixFuzzer2::NAME, AffixFuzzer2::arrow_datatype(), false)}
            );

            rerun::DataCell cell;
            cell.component_name = AffixFuzzer2::NAME;
            const auto ipc_result = rerun::ipc_from_table(*arrow::Table::Make(schema, {array}));
            RR_RETURN_NOT_OK(ipc_result.error);
            cell.buffer = std::move(ipc_result.value);

            return cell;
        }
    } // namespace components
} // namespace rerun
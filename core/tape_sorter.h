#pragma once
#include <memory>

#include "tmp_tape_factory.h"

class TapeSorter {
public:
    TapeSorter(size_t const memory_block, std::unique_ptr<ITapeFactory> factory)
        : memory_block_(memory_block), factory_(std::move(factory)) {}

    void Sort(ITape& input_tape, ITape& output_tape) const;

private:
    size_t memory_block_;
    std::unique_ptr<ITapeFactory> factory_;

    static void Merge(std::vector<std::unique_ptr<ITape>> const& tmp_tapes, ITape& output_tape);

    std::vector<std::unique_ptr<ITape>> Split(ITape& input_tape) const;
};

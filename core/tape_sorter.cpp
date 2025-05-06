#include "tape_sorter.h"

#include <iostream>
#include <queue>

std::vector<std::unique_ptr<ITape>> TapeSorter::Split(ITape& input_tape) const {
    std::vector<std::unique_ptr<ITape>> tmp_tapes;
    input_tape.Rewind();

    std::vector<int32_t> buffer;
    buffer.reserve(memory_block_);

    int32_t value;
    while (input_tape.Read(value)) {
        buffer.clear();

        for (size_t i = 0; i < memory_block_ && input_tape.Read(value); ++i) {
            buffer.push_back(value);
            input_tape.Move(MoveDirection::kForward);
        }

        std::sort(buffer.begin(), buffer.end());
        auto tmp_tape = factory_->Create();
        for (auto const& val : buffer) {
            tmp_tape->Write(val);
            tmp_tape->Move(MoveDirection::kForward);
        }
        tmp_tape->Rewind();
        tmp_tapes.push_back(std::move(tmp_tape));
    }

    return tmp_tapes;
}

void TapeSorter::Merge(std::vector<std::unique_ptr<ITape>> const& tmp_tapes, ITape& output_tape) {
    using Element = std::pair<int32_t, size_t>;
    std::priority_queue<Element, std::vector<Element>, std::greater<>> heap;

    for (auto& tape : tmp_tapes) {
        tape->Rewind();
    }

    for (size_t idx = 0; idx < tmp_tapes.size(); ++idx) {
        int32_t value;
        if (tmp_tapes[idx]->Read(value)) {
            heap.emplace(value, idx);
            tmp_tapes[idx]->Move(MoveDirection::kForward);
        }
    }

    while (!heap.empty()) {
        auto [current_val, tape_idx] = heap.top();
        heap.pop();

        output_tape.Write(current_val);
        output_tape.Move(MoveDirection::kForward);

        int32_t next_val;
        if (tmp_tapes[tape_idx]->Read(next_val)) {
            heap.emplace(next_val, tape_idx);
            tmp_tapes[tape_idx]->Move(MoveDirection::kForward);
        }
    }
}

void TapeSorter::Sort(ITape& input_tape, ITape& output_tape) const {
    input_tape.Rewind();
    int32_t first_value;
    if (!input_tape.Read(first_value)) {
        return;
    }

    input_tape.Rewind();
    auto tmp_tapes = Split(input_tape);
    std::cout << tmp_tapes.size() << std::endl;
    if (tmp_tapes.empty()) {
        throw std::runtime_error("No temporary tapes created");
    }

    output_tape.Rewind();
    Merge(tmp_tapes, output_tape);
}

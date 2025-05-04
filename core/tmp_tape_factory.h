#pragma once
#include <memory>
#include <vector>

#include "i_tape.h"
#include "tape_config.h"

class ITapeFactory {
public:
    virtual ~ITapeFactory() = default;
    virtual std::unique_ptr<ITape> Create() = 0;
};

class TmpTapeFactory : public ITapeFactory {
public:
    TmpTapeFactory(std::string const& dir_name, TapeDelays const& delays);

    std::unique_ptr<ITape> Create() override;
    ~TmpTapeFactory() override;

protected:
    void CleanupTempFiles() const;

private:
    std::string dir_name_;
    TapeDelays delays_;
    std::vector<std::string> created_tapes_;

    std::string GenerateTapeName() const;
};

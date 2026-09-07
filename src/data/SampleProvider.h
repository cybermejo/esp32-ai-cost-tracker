#pragma once
#include "model.h"
#include "IDataProvider.h"
class SampleProvider : public IDataProvider {
 public: bool load(std::vector<Day>& out) override;
};

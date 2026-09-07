#pragma once
#include "model.h"
class IDataProvider {
 public:
  virtual ~IDataProvider() = default;
  virtual bool load(std::vector<Day>& out) = 0;
};

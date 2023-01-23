#pragma once
#include <cstdint>

using OpcodeType = uint8_t;
enum Opcode : OpcodeType {
  kUndefined = 0,
  kData,
  kRegisterNewMonitor,
  kStartSim,
  kEndSim
};

enum DataType : char {
  kDouble = 'd',
};

struct DataHeader {
  int64_t internal_id;
  double time;
  int64_t dim1;
  int64_t dim2;
  int64_t dim3;
  DataType type;
};

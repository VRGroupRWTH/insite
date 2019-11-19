#ifndef NEURON_INFO_HPP
#define NEURON_INFO_HPP

#include <vector>

// Includes from nest kernel:
#include <gid_collection.h>

namespace insite {

struct NeuronInfo {
  bool operator<(const NeuronInfo& that) const {
    return gid < that.gid;
  }

  nest::index gid;
  nest::GIDCollectionPTR gid_collection;
  std::vector<double> position;
};

}  // namespace insite

#endif

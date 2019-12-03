#ifndef NEURON_INFO_HPP
#define NEURON_INFO_HPP

#include <vector>

// Includes from nest kernel:
#include <node_collection.h>

namespace insite {

struct NeuronInfo {
  bool operator<(const NeuronInfo& that) const { return gid < that.gid; }

  nest::index gid;
  nest::NodeCollectionPTR gid_collection;
  std::vector<double> position;
};

}  // namespace insite

#endif

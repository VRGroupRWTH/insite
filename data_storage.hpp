#ifndef DATA_STORATE_HPP
#define DATA_STORATE_HPP

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace insite {

struct Spike {
  double simulation_time;
  std::uint64_t gid;
};
static_assert(sizeof(Spike) == 2 * 8);

class DataStorage {
 public:
  DataStorage(const std::string& filename
              /*, hsize_t time_chunk_size, hsize_t neuronids_chunk_size*/);

  void AddSpike(double simulation_time, std::uint64_t gid);
  std::vector<Spike> GetSpikes();
  void Flush();

 private:
  // std::unique_ptr<H5::H5File> h5_file_;

  // uint64_t flushed_spikes_count = 0;
  std::vector<Spike> buffered_spikes_;
  // H5::DataSet spikes_times_dataset_;
  // H5::DataSet spikes_neurons_dataset_;
  std::mutex spike_mutex_;
};

}  // namespace insite

#endif
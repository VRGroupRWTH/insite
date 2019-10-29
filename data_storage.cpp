#include "data_storage.hpp"

#include <algorithm>

namespace insite {

// We are only using C++11 :(
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace {
constexpr size_t TIME_DIMENSION = 0;
constexpr size_t NEURON_DIMENSION = 1;
}  // namespace

DataStorage::DataStorage(
    const std::string& filename
    /*, hsize_t time_chunk_size, hsize_t neuronids_chunk_size*/) {
  // h5_file_ = make_unique<H5::H5File>(filename.c_str(), H5F_ACC_TRUNC);

  // // Create dataset for spikes
  // constexpr hsize_t spikes_initial_size[] = {0};
  // constexpr hsize_t spikes_max_size[] = {H5S_UNLIMITED};
  // const H5::DataSpace spikes_data_space{1, spikes_initial_size,
  //                                       spikes_max_size};

  // const hsize_t spikes_chunk_size[] = {time_chunk_size};
  // H5::DSetCreatPropList spikes_set_properties_;
  // spikes_set_properties_.setChunk(1, spikes_chunk_size);

  // spikes_times_dataset_ =
  //     h5_file_->createDataSet("spikes/times", H5::PredType::NATIVE_DOUBLE,
  //                             spikes_data_space, spikes_set_properties_);

  // spikes_neurons_dataset_ =
  //     h5_file_->createDataSet("spikes/neurons", H5::PredType::NATIVE_UINT64,
  //                             spikes_data_space, spikes_set_properties_);
}

void DataStorage::AddSpike(double time, std::uint64_t neuron_id) {
  for (const auto& spike : buffered_spikes_) {
    if (spike.time == time && spike.neuron_id == neuron_id) {
      return;
    }
  }
  buffered_spikes_.push_back({time, neuron_id});
}

void DataStorage::Flush() {
  constexpr auto spike_occured_before = [](const Spike& lhs, const Spike& rhs) {
    return lhs.time < rhs.time;
  };
  std::sort(buffered_spikes_.begin(), buffered_spikes_.end(),
            spike_occured_before);

  // const hsize_t spikes_extend[] = {flushed_spikes_count +
  //                                  buffered_spikes_.size()};
  // const hsize_t count[] = {buffered_spikes_.size()};
  // const hsize_t dest_start[] = {flushed_spikes_count};

  // const hsize_t src_space_extend[] = {buffered_spikes_.size()};
  // const H5::DataSpace src_space{1, src_space_extend};

  // {
  //   spikes_times_dataset_.extend(spikes_extend);
  //   const auto dest_space = spikes_times_dataset_.getSpace();
  //   dest_space.selectHyperslab(H5S_SELECT_SET, count, dest_start);
  //   spikes_times_dataset_.write(buffered_spikes_.data(),
  //                               H5::PredType::NATIVE_UINT64, src_space,
  //                               dest_space);
  // }

  // {
  //   spikes_neurons_dataset_.extend(spikes_extend);
  //   const auto dest_space = spikes_neurons_dataset_.getSpace();
  //   dest_space.selectHyperslab(H5S_SELECT_SET, count, dest_start);
  //   spikes_times_dataset_.write(buffered_spikes_.data(),
  //                               H5::PredType::NATIVE_DOUBLE, src_space,
  //                               dest_space);
  // }

  // flushed_spikes_count += buffered_spikes_.size();
  // buffered_spikes_.clear();
}

}  // namespace insite
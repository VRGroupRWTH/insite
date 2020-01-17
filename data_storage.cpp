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

void DataStorage::AddSpike(double simulation_time, std::uint64_t gid) {
  std::unique_lock<std::mutex> lock(spike_mutex_);
  constexpr auto spike_occured_before = [](const Spike& lhs, const Spike& rhs) {
    return lhs.simulation_time < rhs.simulation_time;
  };
  const Spike spike{simulation_time, gid};
  const auto equal_range =
      std::equal_range(buffered_spikes_.begin(), buffered_spikes_.end(), spike,
                       spike_occured_before);
  for (auto i = equal_range.first; i != equal_range.second; ++i) {
    if (i->gid == gid) {
      return;
    }
  }
  buffered_spikes_.insert(equal_range.second, spike);
}

std::vector<Spike> DataStorage::GetSpikes() {
  std::unique_lock<std::mutex> lock(spike_mutex_);
  std::vector<Spike> spikes = buffered_spikes_;
  return spikes;
}

void DataStorage::Flush() {
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

void DataStorage::AddMultimeterMeasurement(std::uint64_t device_id, 
    const std::string& attribute_name, const double simulation_time,
    const std::uint64_t gid, const double value) {
  std::unique_lock<std::mutex> lock(measurement_mutex_);
  auto& measurement = buffered_measurements_[device_id][attribute_name];
  auto& simulation_times = measurement.simulation_times;
  auto& gids = measurement.gids;
  auto& values = measurement.values;

  auto time_iterator = std::find(simulation_times.begin(), 
      simulation_times.end(), simulation_time);
  auto time_index = std::size_t();
  if (time_iterator != simulation_times.end())
    time_index = std::distance(simulation_times.begin(), time_iterator);
  else
  {
    simulation_times.push_back(simulation_time);
    time_index = simulation_times.size() - 1;
  }
  
  auto gid_iterator = std::find(gids.begin(), gids.end(), gid);
  auto gid_index = std::size_t();
  if (gid_iterator != gids.end())
    gid_index = std::distance(gids.begin(), gid_iterator);
  else
  {
    auto ordered_iterator = std::lower_bound(gids.begin(), gids.end(), gid);
    gids.insert(ordered_iterator, gid);
    gid_index = std::distance(gids.begin(), std::find(gids.begin(), gids.end(), gid));
  }
  
  if (values.size() != simulation_times.size() * gids.size())
    values.resize(simulation_times.size() * gids.size());

  measurement.values[time_index * measurement.gids.size() + gid_index] = value;
}

std::unordered_map<std::uint64_t, std::unordered_map<std::string, 
    MultimeterMeasurements>> DataStorage::GetMultimeterMeasurements() {
  std::unique_lock<std::mutex> lock(measurement_mutex_);
  auto measurements = buffered_measurements_;
  return measurements;
}

}  // namespace insite
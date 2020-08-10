#include "data_storage.hpp"

#include <algorithm>
#include <cstring>

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
  buffered_spikes_.reserve(32 * 1024 * 1024 / sizeof(Spike)); // Reserve 32mb
  SetCurrentSimulationTime(0.0);
}

void DataStorage::AddNeuronId(uint64_t neuron_id) {
  std::unique_lock<std::mutex> lock(neuron_ids_mutex_);
  const auto insert_position =
      std::lower_bound(neuron_ids_.begin(), neuron_ids_.end(), neuron_id);
  if (insert_position == neuron_ids_.end() || *insert_position != neuron_id) {
    neuron_ids_.insert(insert_position, neuron_id);
  }
}

std::vector<uint64_t> DataStorage::GetNeuronIds() {
  std::unique_lock<std::mutex> lock(neuron_ids_mutex_);
  std::vector<uint64_t> temp_neuron_ids = neuron_ids_;
  return temp_neuron_ids;
}

void DataStorage::AddSpike(double simulation_time, std::uint64_t gid) {
  std::unique_lock<std::mutex> lock(spike_mutex_);
  const auto spike_occured_before = [](const Spike& lhs, const Spike& rhs) {
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
                                           const std::string& attribute_name,
                                           const double simulation_time,
                                           const std::uint64_t gid,
                                           const double value) {
  std::unique_lock<std::mutex> lock(measurement_mutex_);
  auto& measurement = buffered_measurements_[device_id][attribute_name];
  auto& simulation_times = measurement.simulation_times;
  auto& gids = measurement.gids;
  auto& values = measurement.values;

  auto time_iterator = std::lower_bound(
      simulation_times.begin(), simulation_times.end(), simulation_time);
  auto time_index = std::distance(simulation_times.begin(), time_iterator);
  if (time_iterator == simulation_times.end() ||
      *time_iterator != simulation_time) {
    simulation_times.insert(time_iterator, simulation_time);

    auto new_values =
        std::vector<double>(simulation_times.size() * gids.size(), 0.0);
    for (std::size_t t = 0; t < simulation_times.size(); ++t)
      for (std::size_t g = 0; g < gids.size(); ++g)
        if (t != time_index)
          new_values[t * gids.size() + g] =
              values[(t > time_index ? t - 1 : t) * gids.size() + g];
    values = new_values;
  }

  auto gid_iterator = std::lower_bound(gids.begin(), gids.end(), gid);
  auto gid_index = std::distance(gids.begin(), gid_iterator);
  if (gid_iterator == gids.end() || *gid_iterator != gid) {
    gids.insert(gid_iterator, gid);

    auto new_values =
        std::vector<double>(simulation_times.size() * gids.size(), 0.0);
    for (std::size_t t = 0; t < simulation_times.size(); ++t)
      for (std::size_t g = 0; g < gids.size(); ++g)
        if (g != gid_index)
          new_values[t * gids.size() + g] =
              values[t * gids.size() + (g > gid_index ? g - 1 : g)];
    values = new_values;
  }

  values[time_index * gids.size() + gid_index] = value;
}

std::unordered_map<std::uint64_t,
                   std::unordered_map<std::string, MultimeterMeasurements>>
DataStorage::GetMultimeterMeasurements() {
  std::unique_lock<std::mutex> lock(measurement_mutex_);
  auto measurements = buffered_measurements_;
  return measurements;
}

void DataStorage::SetCurrentSimulationTime(double simulation_time) {
  uint64_t simulation_time_int;
  memcpy(&simulation_time_int, &simulation_time, sizeof(simulation_time_int));
  current_simulation_time_ = simulation_time_int;
}

double DataStorage::GetCurrentSimulationTime() const {
  const uint64_t simulation_time_int =current_simulation_time_;
  double simulation_time;
  memcpy(&simulation_time, &simulation_time_int, sizeof(simulation_time));
  return simulation_time;
}

}  // namespace insite

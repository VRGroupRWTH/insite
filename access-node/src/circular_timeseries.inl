#include <circular_timeseries.h>

namespace insite {

template <typename T>
CircularTimeSeries<T>::CircularTimeSeries(SizeType capacity,
                                          std::vector<int> dimensions)
    : capacity_(capacity), dimensions_(std::move(dimensions)) {
}

template <typename T>
typename CircularTimeSeries<T>::CircularDataView CircularTimeSeries<T>::Back() {
  SizeType index = (end_ == 0 ? Size() : end_) - 1;
  PointerType start = CalculateDataPointer(index);
  PointerType end = start + strides_product_;
  return {timesteps_[index], start, end};
}

template <typename T>
typename CircularTimeSeries<T>::SizeType CircularTimeSeries<T>::Size() {
  return timesteps_.size();
}

template <typename T>
typename CircularTimeSeries<T>::SizeType CircularTimeSeries<T>::Capacity() {
  return capacity_;
}

template <typename T>
bool CircularTimeSeries<T>::Full() {
  return Size() == capacity_;
}

template <typename T>
typename CircularTimeSeries<T>::PointerType
CircularTimeSeries<T>::CalculateDataPointer(SizeType offset) {
  return data_.data() + (offset * strides_product_);
}

template <typename T>
typename CircularTimeSeries<T>::SizeType
CircularTimeSeries<T>::GetLowerBoundIndex(double time) {
  TimeIt lb_itr = std::lower_bound(time_itr_begin(), time_itr_end(), time);
  SPDLOG_DEBUG("GetLowerBound Offset: {}, IsEnd: {}", lb_itr.GetOffset(),
               lb_itr.IsEnd());

  return lb_itr.GetOffset();
}

template <typename T>
std::pair<typename CircularTimeSeries<T>::SizeType,
          typename CircularTimeSeries<T>::SizeType>
CircularTimeSeries<T>::GetIndiciesFromToTime(double from_time, double to_time) {
  TimeIt lb_itr = std::lower_bound(time_itr_begin(), time_itr_end(), from_time);
  TimeIt ub_itr = std::lower_bound(time_itr_begin(), time_itr_end(), to_time);

  auto lower_index = int(lb_itr.GetOffset() - head_);
  if (lower_index < 0) {
    lower_index += Capacity();
  }
  auto upper_index = int(ub_itr.GetOffset() - head_);
  if (upper_index < 0) {
    upper_index += Capacity();
  }

  if (lower_index < upper_index) {
    return std::make_pair(lower_index, upper_index);
  }

  if (ub_itr.IsEnd()) {
    return std::make_pair(lb_itr.GetOffset(), ub_itr.GetOffset() + Capacity());
  }
  // if (lb_itr.GetOffset() > ub_itr.GetOffset()) {
  //   return std::make_pair(lb_itr.GetOffset(), ub_itr.GetOffset() + )
  // }
}

template <typename T>
typename CircularTimeSeries<T>::CircularTimeSeriesIt
CircularTimeSeries<T>::GetLowerBoundItr(double time) {
  CircularTimeSeriesIt lb_itr = std::lower_bound(
      begin(), end(), time, [&time](const CircularTimeSeries& a, double b) {
        return a.time_ < time;
      });
  return lb_itr;
}

template <typename T>
typename CircularTimeSeries<T>::PointerType
CircularTimeSeries<T>::GetTimeHead() {
  return timesteps_.data() + end_;
}

// adhjwkldjhad
template <typename T>
typename CircularTimeSeries<T>::TimeIt CircularTimeSeries<T>::time_itr_begin() {
  if (Size() == 0) {
    return {this, 0, true};
  }

  return {this, head_, false};
}

template <typename T>
typename CircularTimeSeries<T>::CircularTimeSeriesIt
CircularTimeSeries<T>::begin() {
  if (Size() == 0) {
    return {this, 0, true};
  }

  return {this, head_, false};
}

template <typename T>
typename CircularTimeSeries<T>::CircularTimeSeriesIt
CircularTimeSeries<T>::end() {
  return {this, 0, true};
}

template <typename T>
typename CircularTimeSeries<T>::TimeIt CircularTimeSeries<T>::time_itr_end() {
  return {this, 0, true};
}

template <typename T>
double CircularTimeSeries<T>::GetTimeByIndex(SizeType index) {
  index = head_ + index;
  if (index >= capacity_) {
    index -= capacity_;
  }
  return timesteps_[index];
}

template <typename T>
typename CircularTimeSeries<T>::CircularDataView
CircularTimeSeries<T>::GetByTimeIndex(SizeType index) {
  index = head_ + index;
  if (index >= capacity_) {
    index -= capacity_;
  }
  auto start = data_.data() + index * strides_product_;
  // SPDLOG_DEBUG("Getting data from index: {}", index);
  return {timesteps_[index], start, start + strides_product_};
}

template <typename T>
typename CircularTimeSeries<T>::CircularDataView
CircularTimeSeries<T>::GetVarByIndex(SizeType index, uint8_t var) {
  index = head_ + index;
  if (index >= capacity_) {
    index -= capacity_;
  }
  auto start = data_.data() + index * strides_product_ + var * dimensions_[1];
  return {timesteps_[index], start, start + dimensions_[1]};
}

template <typename T>
void CircularTimeSeries<T>::PushBack(DataHeader header,
                                     ConstPointerType items_begin,
                                     ConstPointerType items_end) {
  if (strides_product_ == 0) {
    dimensions_.clear();
    dimensions_ = {static_cast<int>(header.dim1), static_cast<int>(header.dim2), static_cast<int>(header.dim3)};
    strides_product_ = header.dim1 * header.dim2 * header.dim3;
    timesteps_.reserve(capacity_);
    data_.reserve(capacity_ * strides_product_);
  }
  if (Full()) {
    *(timesteps_.data() + head_) = header.time;
    std::copy(items_begin, items_end, CalculateDataPointer(head_));
    Increment(head_);
    end_ = head_;
  } else {
    timesteps_.push_back(header.time);
    data_.insert(data_.end(), items_begin, items_end);
    Increment(end_);
  }
  // SPDLOG_DEBUG(timesteps_);
}

template <typename T>
void CircularTimeSeries<T>::Increment(SizeType& index) {
  if (++index == capacity_) {
    index = 0;
  }
}

template <typename T>
CircularTimeSeries<T>::TimeIt::TimeIt(
    CircularTimeSeries* circular_time_series_ptr,
    SizeType offset,
    bool is_end)
    : offset_(offset),
      is_end_(is_end),
      circular_time_series_ptr_(circular_time_series_ptr),
      time_series_ptr_(&(circular_time_series_ptr->timesteps_)),
      strides_product_(circular_time_series_ptr->strides_product_) {}

template <typename T>
typename CircularTimeSeries<T>::TimeIt::reference
CircularTimeSeries<T>::TimeIt::operator*() {
  return time_series_ptr_->at(offset_);
};

template <typename T>
typename CircularTimeSeries<T>::TimeIt::pointer
CircularTimeSeries<T>::TimeIt::operator->() {
  return &time_series_ptr_->at(offset_);
};

template <typename T>
typename CircularTimeSeries<T>::SizeType
CircularTimeSeries<T>::TimeIt::GetOffset() {
  return offset_;
};

template <typename T>
bool CircularTimeSeries<T>::TimeIt::IsEnd() {
  return is_end_;
};

template <typename T>
typename CircularTimeSeries<T>::TimeIt&
CircularTimeSeries<T>::TimeIt::operator++() {
  circular_time_series_ptr_->Increment(offset_);
  if (offset_ == circular_time_series_ptr_->end_) {
    is_end_ = true;
    offset_ = 0;
  }
  return *this;
}

template <typename T>
typename CircularTimeSeries<T>::TimeIt
CircularTimeSeries<T>::TimeIt::operator++(int) {
  TimeIt tmp = *this;
  circular_time_series_ptr_->Increment(offset_);
  if (offset_ == circular_time_series_ptr_->end_) {
    is_end_ = true;
    offset_ = 0;
  }
  return *tmp;
}

template <typename T>
bool operator==(const typename CircularTimeSeries<T>::TimeIt& a,
                const typename CircularTimeSeries<T>::TimeIt& b) {
  return a.is_end_ == b.is_end_ && a.offset_ == b.offset_ &&
         a.circular_time_series_ptr_ == b.circular_time_series_ptr_;
}

template <typename T>
bool operator!=(const typename CircularTimeSeries<T>::TimeIt& a,
                const typename CircularTimeSeries<T>::TimeIt& b) {
  return !(a == b);
}

};  // namespace insite

#pragma once
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

namespace insite {

template <typename T>
class CircularTimeSeries {
  using ValueType = T;
  using Reference = T&;
  using SizeType = size_t;
  using PointerType = T*;
  using ConstPointerType = const T*;
  using DataBuffer = std::vector<T>;
  using TimeBuffer = std::vector<double>;
  public:
  std::vector<int> dimensions_;
  uint32_t strides_product_ = 1;
  DataBuffer data_;
  TimeBuffer timesteps_;
  SizeType head_ = 0;
  SizeType end_ = 0;
  SizeType capacity_;
  SizeType offset_ = 0;

  PointerType data_start_;
  PointerType data_head_{};
  PointerType time_start_;
  PointerType time_end_;
  PointerType time_head_ptr_;
  SizeType data_tail_{};

 private:
  struct CircularDataView {
    using DataIterator = T*;

    CircularDataView() : time_(0.0), begin_(nullptr), end_(nullptr) {}
    CircularDataView(double time, DataIterator begin, DataIterator end)
        : time_(time), begin_(begin), end_(end) {}

    double Time() { return time_; };
    DataIterator begin() { return begin_; };  // NOLINT
    DataIterator end() { return end_; };      // NOLINT

    double time_;
    DataIterator begin_;
    DataIterator end_;
  };

  struct TimeIt {
    using iterator_category = std::forward_iterator_tag;  // NOLINT
    using difference_type = std::ptrdiff_t;               // NOLINT
    using value_type = double;                            // NOLINT
    using pointer = value_type*;                          // NOLINT
    using reference = value_type&;                        // NOLINT

    TimeIt(CircularTimeSeries* circular_time_series_ptr,
           SizeType offset,
           bool is_end)
        : offset_(offset),
          is_end_(is_end),
          circular_time_series_ptr_(circular_time_series_ptr),
          time_series_ptr_(&(circular_time_series_ptr->timesteps_)),
          strides_product_(circular_time_series_ptr->strides_product_) {}

    reference operator*() { return time_series_ptr_->at(offset_); };
    pointer operator->() { return &time_series_ptr_->at(offset_); };
    SizeType GetOffset() { return offset_; };
    bool IsEnd() { return is_end_; };

    TimeIt& operator++() {
      circular_time_series_ptr_->Increment(offset_);
      if (offset_ == circular_time_series_ptr_->end_) {
        is_end_ = true;
        offset_ = 0;
      }
      return *this;
    }

    TimeIt operator++(int) {
      TimeIt tmp = *this;
      circular_time_series_ptr_->Increment(offset_);
      if (offset_ == circular_time_series_ptr_->end_) {
        is_end_ = true;
        offset_ = 0;
      }
      return *tmp;
    }

    friend bool operator==(const TimeIt& a, const TimeIt& b) {
      return a.is_end_ == b.is_end_ && a.offset_ == b.offset_ &&
             a.circular_time_series_ptr_ == b.circular_time_series_ptr_;
    }

    friend bool operator!=(const TimeIt& a, const TimeIt& b) {
      return !(a == b);
    }

   private:
    bool is_end_ = false;
    SizeType strides_product_;
    SizeType offset_;
    TimeBuffer* time_series_ptr_;
    CircularTimeSeries* circular_time_series_ptr_;
  };

  struct CircularTimeSeriesIt {
    using iterator_category = std::forward_iterator_tag;  // NOLINT
    using difference_type = std::ptrdiff_t;               // NOLINT
    using value_type = CircularDataView;                  // NOLINT
    using pointer = value_type*;                          // NOLINT
    using reference = value_type&;                        // NOLINT

    CircularTimeSeriesIt(CircularTimeSeries* circular_time_series_ptr,
                         SizeType offset,
                         bool is_end)
        : data_buffer_ptr_(&(circular_time_series_ptr->data_)),
          offset_(offset),
          is_end_(is_end),
          circular_time_series_ptr_(circular_time_series_ptr),
          time_series_ptr_(&(circular_time_series_ptr->timesteps_)),
          strides_product_(circular_time_series_ptr->strides_product_) {}

    reference operator*() {
      view_.time_ = time_series_ptr_->at(offset_);
      view_.begin_ = data_buffer_ptr_->data() + offset_ * strides_product_;
      view_.end_ = view_.begin_ + strides_product_;
      return view_;
    };
    pointer operator->() {
      view_.time_ = time_series_ptr_->at(offset_);
      view_.begin_ = data_buffer_ptr_->data() + offset_ * strides_product_;
      view_.end_ = view_.begin_ + strides_product_;
      return &view_;
    };

    CircularTimeSeriesIt& operator++() {
      circular_time_series_ptr_->Increment(offset_);
      if (offset_ == circular_time_series_ptr_->end_) {
        is_end_ = true;
        offset_ = 0;
      }
      return *this;
    }

    CircularTimeSeriesIt operator++(int) {
      CircularTimeSeriesIt tmp = *this;
      circular_time_series_ptr_->Increment(offset_);
      if (offset_ == circular_time_series_ptr_->end_) {
        is_end_ = true;
        offset_ = 0;
      }
      return *tmp;
    }

    friend bool operator==(const CircularTimeSeriesIt& a,
                           const CircularTimeSeriesIt& b) {
      return a.is_end_ == b.is_end_ && a.offset_ == b.offset_ &&
             a.circular_time_series_ptr_ == b.circular_time_series_ptr_;
    }

    friend bool operator!=(const CircularTimeSeriesIt& a,
                           const CircularTimeSeriesIt& b) {
      return !(a == b);
    }

   private:
    bool is_end_ = false;
    SizeType strides_product_;
    SizeType offset_;
    CircularDataView view_{};
    TimeBuffer* time_series_ptr_;
    DataBuffer* data_buffer_ptr_;
    CircularTimeSeries* circular_time_series_ptr_;
  };

 public:
  CircularTimeSeries(SizeType capacity, std::vector<int> dimensions)
      : capacity_(capacity), dimensions_(std::move(dimensions)) {
    for (auto& elem : dimensions_) {
      strides_product_ *= elem;
    }
    timesteps_.reserve(capacity_);
    data_.reserve(capacity_ * strides_product_);
  }

  CircularDataView Back() {
    SizeType index = (end_ == 0 ? Size() : end_) - 1;
    PointerType start = CalculateDataPointer(index);
    PointerType end = start + strides_product_;
    return {timesteps_[index], start, end};
  }

  void Clear();

  SizeType Size() { return timesteps_.size(); }

  SizeType Capacity() { return capacity_; }

  bool Full() { return Size() == capacity_; }

  PointerType CalculateDataPointer(SizeType offset) {
    return data_.data() + (offset * strides_product_);
  }

  SizeType GetLowerBoundIndex(double time) {
    TimeIt lb_itr = std::lower_bound(time_itr_begin(), time_itr_end(), time);
    spdlog::debug("GetLowerBound Offset: {}, IsEnd: {}",lb_itr.GetOffset(),lb_itr.IsEnd());
    
    return lb_itr.GetOffset();
  }

  std::pair<SizeType,SizeType> GetIndiciesFromToTime(double from_time, double to_time)
  {
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
      return std::make_pair(lower_index,upper_index);
    }
   
    if (ub_itr.IsEnd()) {
      return std::make_pair(lb_itr.GetOffset(), ub_itr.GetOffset() + Capacity()); 
    }
    // if (lb_itr.GetOffset() > ub_itr.GetOffset()) {
    //   return std::make_pair(lb_itr.GetOffset(), ub_itr.GetOffset() + )
    // }
  }

  CircularTimeSeriesIt GetLowerBoundItr(double time) {
    CircularTimeSeriesIt lb_itr = std::lower_bound(
        begin(), end(), time, [&time](const CircularDataView& a, double b) {
          return a.time_ < time;
        });
    return lb_itr;
  }

  PointerType GetTimeHead() { return timesteps_.data() + end_; }

  TimeIt time_itr_begin() {
    if (Size() == 0) {
      return {this, 0, true};
    }

    return {this, head_, false};
  }

  CircularTimeSeriesIt begin() {
    if (Size() == 0) {
      return {this, 0, true};
    }

    return {this, head_, false};
  }

  CircularTimeSeriesIt end() { return {this, 0, true}; }

  TimeIt time_itr_end() { return {this, 0, true}; }

  double GetTimeByIndex(SizeType index) {
    index = head_ + index;
    if (index >= capacity_) {
      index -= capacity_;
    }
    return timesteps_[index];
  }

  CircularDataView GetByTimeIndex(SizeType index) {
    index = head_ + index;
    if (index >= capacity_) {
      index -= capacity_;
    }
    auto start = data_.data() + index * strides_product_;
    // spdlog::debug("Getting data from index: {}", index);
    return {timesteps_[index], start, start + strides_product_};
  }

  CircularDataView GetVarByIndex(SizeType index, uint8_t var) {
    index = head_ + index;
    if (index >= capacity_) {
      index -= capacity_;
    }
    auto start = data_.data() + index * strides_product_ + var * dimensions_[1];
    // spdlog::debug("Getting data from index: {}", index);
    return {timesteps_[index], start, start + dimensions_[1]};
  }

  void PushBack(double time,
                ConstPointerType items_begin,
                ConstPointerType items_end) {
    if (Full()) {
      *(timesteps_.data() + head_) = time;
      std::copy(items_begin, items_end, CalculateDataPointer(head_));
      Increment(head_);
      end_ = head_;
    } else {
      timesteps_.push_back(time);
      data_.insert(data_.end(), items_begin, items_end);
      Increment(end_);
    }
    // spdlog::debug(timesteps_);
  }

  void Increment(SizeType& index) {
    if (++index == capacity_) {
      index = 0;
    }
  };
};
}  // namespace insite

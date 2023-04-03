#pragma once
#include <opcodes.h>
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
  uint32_t strides_product_ = 0;
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

  struct CircularDataView {
    using DataIterator = T*;

    CircularDataView();
    CircularDataView(double time, DataIterator begin, DataIterator end);

    double Time();
    DataIterator begin();  // NOLINT
    DataIterator end();    // NOLINT

   private:
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
           bool is_end);

    reference operator*();
    pointer operator->();
    SizeType GetOffset();
    bool IsEnd();

    TimeIt& operator++();
    TimeIt operator++(int);

    template <typename C>
    friend bool operator==(const TimeIt& a, const TimeIt& b);

    template <typename C>
    friend bool operator!=(const TimeIt& a, const TimeIt& b);

   private:
    bool is_end_ = false;
    SizeType strides_product_ = 0;
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
                         bool is_end);

    reference operator*();

    pointer operator->();

    CircularTimeSeriesIt& operator++();

    CircularTimeSeriesIt operator++(int);

    template <typename C>
    friend bool operator==(const CircularTimeSeriesIt& a,
                           const CircularTimeSeriesIt& b);

    template <typename C>
    friend bool operator!=(const CircularTimeSeriesIt& a,
                           const CircularTimeSeriesIt& b);

   private:
    bool is_end_ = false;
    SizeType strides_product_ = strides_product_;
    SizeType offset_;
    CircularDataView view_{};
    TimeBuffer* time_series_ptr_;
    DataBuffer* data_buffer_ptr_;
    CircularTimeSeries* circular_time_series_ptr_;
  };

 public:
  CircularTimeSeries(SizeType capacity, std::vector<int> dimensions);

  CircularDataView Back();

  void Clear();

  SizeType Size();

  SizeType Capacity();

  bool Full();

  PointerType CalculateDataPointer(SizeType offset);

  SizeType GetLowerBoundIndex(double time);

  std::pair<SizeType, SizeType> GetIndiciesFromToTime(double from_time,
                                                      double to_time);

  CircularTimeSeriesIt GetLowerBoundItr(double time);

  PointerType GetTimeHead();

  TimeIt time_itr_begin();

  CircularTimeSeriesIt begin();

  CircularTimeSeriesIt end();

  TimeIt time_itr_end();

  double GetTimeByIndex(SizeType index);

  CircularDataView GetByTimeIndex(SizeType index);

  CircularDataView GetVarByIndex(SizeType index, uint8_t var);

  void PushBack(DataHeader time,
                ConstPointerType items_begin,
                ConstPointerType items_end);

  void Increment(SizeType& index);
};
}  // namespace insite

#include "../../src/circular_dataview.inl"
#include "../../src/circular_timeseries.inl"
#include "../../src/circular_timeseries_it.inl"

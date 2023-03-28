#include <circular_timeseries.h>

namespace insite {

template <typename T>
CircularTimeSeries<T>::CircularTimeSeriesIt::CircularTimeSeriesIt(
    CircularTimeSeries<T>* circular_time_series_ptr,
    CircularTimeSeries<T>::SizeType offset,
    bool is_end)
    : data_buffer_ptr_(&(circular_time_series_ptr->data_)),
      offset_(offset),
      is_end_(is_end),
      circular_time_series_ptr_(circular_time_series_ptr),
      time_series_ptr_(&(circular_time_series_ptr->timesteps_)),
      strides_product_(circular_time_series_ptr->strides_product_) {}

template <typename T>
typename CircularTimeSeries<T>::CircularTimeSeriesIt::reference
CircularTimeSeries<T>::CircularTimeSeriesIt::operator*() {
  view_.time_ = time_series_ptr_->at(offset_);
  view_.begin_ = data_buffer_ptr_->data() + offset_ * strides_product_;
  view_.end_ = view_.begin_ + strides_product_;
  return view_;
};

template <typename T>
typename CircularTimeSeries<T>::CircularTimeSeriesIt::pointer
CircularTimeSeries<T>::CircularTimeSeriesIt::operator->() {
  view_.time_ = time_series_ptr_->at(offset_);
  view_.begin_ = data_buffer_ptr_->data() + offset_ * strides_product_;
  view_.end_ = view_.begin_ + strides_product_;
  return &view_;
};

template <typename T>
typename CircularTimeSeries<T>::CircularTimeSeriesIt&
CircularTimeSeries<T>::CircularTimeSeriesIt::operator++() {
  circular_time_series_ptr_->Increment(offset_);
  if (offset_ == circular_time_series_ptr_->end_) {
    is_end_ = true;
    offset_ = 0;
  }
  return *this;
}

template <typename T>
typename CircularTimeSeries<T>::CircularTimeSeriesIt
CircularTimeSeries<T>::CircularTimeSeriesIt::operator++(int) {
  CircularTimeSeriesIt tmp = *this;
  circular_time_series_ptr_->Increment(offset_);
  if (offset_ == circular_time_series_ptr_->end_) {
    is_end_ = true;
    offset_ = 0;
  }
  return *tmp;
}

template <typename T>
bool operator==(const typename CircularTimeSeries<T>::CircularTimeSeriesIt& a,
                const typename CircularTimeSeries<T>::CircularTimeSeriesIt& b) {
  return a.is_end_ == b.is_end_ && a.offset_ == b.offset_ &&
         a.circular_time_series_ptr_ == b.circular_time_series_ptr_;
}

template <typename T>
bool operator!=(const typename CircularTimeSeries<T>::CircularTimeSeriesIt& a,
                const typename CircularTimeSeries<T>::CircularTimeSeriesIt& b) {
  return !(a == b);
}

}  // namespace insite
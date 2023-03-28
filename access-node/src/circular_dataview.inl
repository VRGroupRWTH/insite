#include <circular_timeseries.h>

namespace insite {

template <typename T>
CircularTimeSeries<T>::CircularDataView::CircularDataView()
    : time_(0.0), begin_(nullptr), end_(nullptr) {}

template <typename T>
CircularTimeSeries<T>::CircularDataView::CircularDataView(double time,
                                                          DataIterator begin,
                                                          DataIterator end)
    : time_(time), begin_(begin), end_(end) {}

template <typename T>
double CircularTimeSeries<T>::CircularDataView::Time() {
  return time_;
};

template <typename T>
typename CircularTimeSeries<T>::CircularDataView::DataIterator
CircularTimeSeries<T>::CircularDataView::begin() {
  return begin_;
};

template <typename T>
typename CircularTimeSeries<T>::CircularDataView::DataIterator
CircularTimeSeries<T>::CircularDataView::end() {
  return end_;
};

};  // namespace insite
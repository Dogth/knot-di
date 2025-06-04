#ifndef STATIC_MAP_HPP
#define STATIC_MAP_HPP

#include <cstddef>
#include <utility>

// TODO: refactor
namespace Knot {
template <typename K, typename V, std::size_t N> class StaticMap {
public:
  typedef K key_type;
  typedef V mapped_type;
  typedef std::pair<K, V> value_type;
  typedef std::size_t size_type;

  size_type _size;
  StaticMap() : _size(0) {}

  bool insert(const K &key, const V &value) {
    size_type pos;
    if (find_pos(key, pos)) {
      _data[pos].second = value;
      return true;
    }
    if (_size >= N)
      return false;
    for (size_type i = _size; i > pos; --i)
      _data[i] = _data[i - 1];
    _data[pos] = std::make_pair(key, value);
    ++_size;
    return true;
  }

  V *find(const K &key) {
    size_type pos;
    if (find_pos(key, pos) && pos < _size && _data[pos].first == key)
      return &_data[pos].second;
    return 0;
  }

  const V *find(const K &key) const {
    size_type pos;
    if (find_pos(key, pos) && pos < _size && _data[pos].first == key)
      return &_data[pos].second;
    return 0;
  }

  void clear() { _size = 0; }

  value_type *begin() { return _data; }
  value_type *end() { return _data + _size; }

  size_type size() const { return _size; }
  size_type capacity() const { return N; }

private:
  bool find_pos(const K &key, size_type &pos) const {
    size_type left = 0, right = _size;
    while (left < right) {
      size_type mid = left + (right - left) / 2;
      if (_data[mid].first < key)
        left = mid + 1;
      else
        right = mid;
    }
    pos = left;
    return (pos < _size && !(key < _data[pos].first));
  }

  value_type _data[N];
};
}; // namespace Knot

#endif // STATIC_MAP_HPP

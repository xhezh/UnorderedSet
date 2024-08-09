#pragma once
#include <vector>
#include <list>
#include <functional>
#include <algorithm>

template <class KeyT>
class UnorderedSet {
  std::vector<std::list<KeyT>> buckets_;
  size_t size_;

  size_t GetIndex(const KeyT& key, size_t count) const {
    return count == 0 ? 0 : std::hash<KeyT>{}(key) % count;
  }

 public:
  UnorderedSet() : buckets_(), size_(0) {
  }

  explicit UnorderedSet(size_t count) : buckets_(count == 0 ? 1 : count), size_(0) {
  }

  template <class Iterator, class = std::enable_if_t<std::is_base_of_v<
                                std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>>>
  UnorderedSet(Iterator first, Iterator last)
      : buckets_(std::distance(first, last) == 0 ? 1 : std::distance(first, last)), size_(0) {
    for (auto it = first; it != last; ++it) {
      Insert(*it);
    }
  }

  UnorderedSet(const UnorderedSet& other) : buckets_(other.buckets_), size_(other.size_) {
  }

  UnorderedSet(UnorderedSet&& other) noexcept : buckets_(std::move(other.buckets_)), size_(other.size_) {
    other.size_ = 0;
  }

  UnorderedSet& operator=(const UnorderedSet& other) {
    if (this != &other) {
      buckets_ = other.buckets_;
      size_ = other.size_;
    }
    return *this;
  }

  UnorderedSet& operator=(UnorderedSet&& other) noexcept {
    if (this != &other) {
      buckets_ = std::move(other.buckets_);
      size_ = other.size_;
      other.size_ = 0;
    }
    return *this;
  }

  size_t Size() const {
    return size_;
  }

  bool Empty() const {
    return size_ == 0;
  }

  void Clear() {
    buckets_.clear();
    size_ = 0;
  }

  void Insert(const KeyT& key) {
    if (Find(key)) {
      return;
    }
    if (size_ >= buckets_.size()) {
      Rehash(buckets_.empty() ? 1 : buckets_.size() * 2);
    }
    buckets_[GetIndex(key, buckets_.size())].push_back(key);
    ++size_;
  }

  void Insert(KeyT&& key) {
    if (Find(key)) {
      return;
    }
    if (size_ >= buckets_.size()) {
      Rehash(buckets_.empty() ? 1 : buckets_.size() * 2);
    }
    buckets_[GetIndex(key, buckets_.size())].push_back(std::move(key));
    ++size_;
  }

  void Erase(const KeyT& key) {
    if (buckets_.empty()) {
      return;
    }
    size_t index = GetIndex(key, buckets_.size());
    auto& bucket = buckets_[index];
    auto it = std::find(bucket.begin(), bucket.end(), key);
    if (it != bucket.end()) {
      bucket.erase(it);
      --size_;
    }
  }

  bool Find(const KeyT& key) const {
    if (buckets_.empty()) {
      return false;
    }
    size_t index = GetIndex(key, buckets_.size());
    const auto& bucket = buckets_[index];
    return std::find(bucket.begin(), bucket.end(), key) != bucket.end();
  }

  void Rehash(size_t new_count) {
    if (new_count < size_ || new_count == buckets_.size()) {
      return;
    }
    std::vector<std::list<KeyT>> new_buckets(new_count);
    for (const auto& bucket : buckets_) {
      for (const auto& key : bucket) {
        new_buckets[GetIndex(key, new_count)].push_back(key);
      }
    }
    buckets_ = std::move(new_buckets);
  }

  void Reserve(size_t new_count) {
    if (new_count > buckets_.size()) {
      Rehash(new_count);
    }
  }

  size_t BucketCount() const {
    return buckets_.size();
  }

  size_t BucketSize(size_t index) const {
    if (index >= buckets_.size()) {
      return 0;
    }
    return buckets_[index].size();
  }

  size_t Bucket(const KeyT& key) const {
    return GetIndex(key, buckets_.size());
  }

  float LoadFactor() const {
    return buckets_.empty() ? 0.0f : static_cast<float>(size_) / buckets_.size();
  }
};

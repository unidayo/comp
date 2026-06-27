#pragma once

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

constexpr bool kEnableDebug = false;

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64 = int64_t;
using uint64 = uint64_t;

// Policy: Don't overuse using. Limit its use to extremely common types and
// functions.
using std::array;
using std::nullopt;
using std::optional;
using std::pair;
using std::string;
using std::tuple;
using std::vector;
using std::views::iota;

#define UNEXPECTED()                                                 \
  {                                                                  \
    std::cerr << "Unexpected@" << __FILE__ << ":" << __LINE__ << "!" \
              << std::endl;                                          \
    exit(1);                                                         \
  }

// Override operator<< for ostream to handle vector.
template <typename T>
std::ostream &operator<<(std::ostream &os, const vector<T> &v) {
  os << "[";
  for (const int i : iota(0, int(v.size()))) {
    if (i > 0)
      os << ", ";
    os << v[i];
  }
  os << "]";
  return os;
}

template <std::integral T>
constexpr auto closed_iota(T first, T last) {
  return std::views::iota(first, last + 1);
}

class TimeMeasurement {
public:
  TimeMeasurement() : start_(std::chrono::steady_clock::now()) {}

  int64 CurrentMs() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_)
        .count();
  }

  int64 CurrentUs() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - start_)
        .count();
  }

private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
};
TimeMeasurement g_time;

class Random {
 public:
  Random() : state_(3141592653589793238ULL) {}

  double RandomDouble(double a, double b) {
    Update();
    const double r = state_ * kScale_;
    return a + (b - a) * r;
  }

  int RandomInt(int a, int b) {
    return static_cast<int>(RandomDouble(a, b));
  }

  int RandomIntInclusive(int a, int b) {
    return RandomInt(a, b + 1);
  }

 private:
  uint64 state_;

  // 2^(-64)
  static constexpr double kScale_ = 1.0 / 18446744073709551616.0;

  void Update() {
    // xorshift
    state_ ^= state_ >> 13;
    state_ ^= state_ << 7;
    state_ ^= state_ >> 17;
  }
};
Random g_random;

double ExpInterpolation(const double x0, const double y0, const double x1,
                        const double y1, const double x) {
  const double log_y0 = std::log(y0);
  const double log_y1 = std::log(y1);
  const double log_y = log_y0 + (x - x0) * (log_y1 - log_y0) / (x1 - x0);
  return std::exp(log_y);
}

double LinearInterpolation(const double x0, const double y0, const double x1,
                           const double y1, const double x) {
  return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

double Sigmoid(const double x, const double half, const double slope) {
  return 1.0 / (1.0 + std::exp(slope * (half - x)));
}

double Squash(const double x, const double half) {
  return x / (x + half);
}

double SquashBidi(const double x, const double half) {
  if (x >= 0.0) {
    return Squash(x, half);
  } else {
    return -Squash(-x, half);
  }
}

#pragma once


#pragma once



struct Int2D {
  int x, y;

  Int2D() : x(0), y(0) {}
  Int2D(const int x, const int y) : x(x), y(y) {}

  int Distance2(const Int2D& other) const {
    return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
  }
  float Distance(const Int2D& other) const {
    return std::sqrt(float(Distance2(other)));
  }
  bool operator==(const Int2D& other) const {
    return x == other.x && y == other.y;
  }
};

struct Segment {
  Int2D p0, p1;
  Segment() : p0(0, 0), p1(0, 0) {}
  Segment(const Int2D& p0, const Int2D& p1) : p0(p0), p1(p1) {}
  bool IsEqualToUnordered(const Segment& other) const {
    return (p0 == other.p0 && p1 == other.p1) ||
           (p0 == other.p1 && p1 == other.p0);
  }
};

struct Rectangle {
  int x_min, y_min, x_max, y_max;
  Rectangle() : x_min(0), y_min(0), x_max(0), y_max(0) {}
  Rectangle(const int x_min, const int y_min, const int x_max, const int y_max)
      : x_min(x_min), y_min(y_min), x_max(x_max), y_max(y_max) {}
};

#pragma once

#include <cassert>



template <typename T>
struct Matrix {
 private:
  int r_size_;
  int c_size_;
  vector<T> data_;

 public:
  int r_size() const { return r_size_; }
  int c_size() const { return c_size_; }
  Matrix(const int r_size, const int c_size)
      : r_size_(r_size), c_size_(c_size), data_(r_size * c_size) {}
  Matrix() : r_size_(0), c_size_(0), data_() {}
  T& operator[](const int r, const int c) { return data_[r * c_size_ + c]; }
  const T& operator[](const int r, const int c) const {
    return data_[r * c_size_ + c];
  }
};


// Processor position nodes: <= 20
// Sorter position nodes: <= 1000
// Inlet node : 1
constexpr int kMaxNodeCount = 1021;

const Int2D kInletPosition = Int2D(0, 5000);

struct Config {
  int trash_type_count = 0;
  int sorter_position_count = 0;
  int sorter_type_count = 0;
  vector<Int2D> node_positions;
  // sorter_configs[2 * i, *] and sorter_configs[2 * i + 1, *] are the
  // configurations of the sorter type i.
  Matrix<float> sorter_configs;

  // Node ID methods
  int ProcessorNodeId(const int i) const {
    return i;
  }

  bool IsProcessorNode(const int node_id) const {
    return node_id < trash_type_count;
  }

  int ProcessorNodeBegin() const {
    return 0;
  }

  int ProcessorNodeEnd() const {
    return trash_type_count;
  }

  int SorterNodeId(const int i) const {
    return trash_type_count + i;
  }

  bool IsSorterNode(const int node_id) const {
    return node_id >= trash_type_count &&
           node_id < trash_type_count + sorter_position_count;
  }

  int SorterNodeBegin() const {
    return trash_type_count;
  }

  int SorterNodeEnd() const {
    return trash_type_count + sorter_position_count;
  }

  int InletNodeId() const {
    return trash_type_count + sorter_position_count;
  }

  bool IsInletNode(const int node_id) const {
    return node_id == InletNodeId();
  }

  int NodeCount() const {
    return trash_type_count + sorter_position_count + 1;
  }
};

struct State {
  // For a processor position node, the value is the processor type.
  // For a sorter position node, the value is the sorter type or -1 if the node
  // is empty.
  vector<int> node_types;
  vector<pair<int, int>> node_destination_pairs;
};

#pragma once

#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <limits>

#pragma once

#include <algorithm>
#include <limits>



#pragma once

#include <algorithm>


bool SegmentsShareEndPoint(const Int2D& p1, const Int2D& p2, const Int2D& q1,
                           const Int2D& q2) {
  return (p1 == q1) || (p1 == q2) || (p2 == q1) || (p2 == q2);
}

bool SegmentsIntersect(const Int2D& p1, const Int2D& p2, const Int2D& q1,
                       const Int2D& q2) {
  auto sign = [](const int x) -> int { return (x > 0) - (x < 0); };
  auto orientation = [&](const Int2D& a, const Int2D& b,
                         const Int2D& c) -> int {
    const int cross_product =
        (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return sign(cross_product);
  };

  if (std::max(p1.x, p2.x) < std::min(q1.x, q2.x) ||
      std::max(q1.x, q2.x) < std::min(p1.x, p2.x) ||
      std::max(p1.y, p2.y) < std::min(q1.y, q2.y) ||
      std::max(q1.y, q2.y) < std::min(p1.y, p2.y)) {
    return false;
  }

  const int o1 = orientation(p1, p2, q1);
  const int o2 = orientation(p1, p2, q2);
  const int o3 = orientation(q1, q2, p1);
  const int o4 = orientation(q1, q2, p2);
  return (o1 * o2 <= 0) && (o3 * o4 <= 0);
}

// Returns whether the two segments intersect without sharing an endpoint.
bool SegmentsDisallowed(const Segment& s0, const Segment& s1) {
  auto sign = [](const int x) -> int { return (x > 0) - (x < 0); };
  auto orientation = [&](const Int2D& a, const Int2D& b,
                         const Int2D& c) -> int {
    const int cross_product =
        (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return sign(cross_product);
  };

  const Int2D& p0 = s0.p0;
  const Int2D& p1 = s0.p1;
  const Int2D& q0 = s1.p0;
  const Int2D& q1 = s1.p1;

  if (std::max(p0.x, p1.x) < std::min(q0.x, q1.x) ||
      std::max(q0.x, q1.x) < std::min(p0.x, p1.x) ||
      std::max(p0.y, p1.y) < std::min(q0.y, q1.y) ||
      std::max(q0.y, q1.y) < std::min(p0.y, p1.y)) {
    return false;
  }

  if (SegmentsShareEndPoint(p0, p1, q0, q1)) return false;

  const int o0 = orientation(p0, p1, q0);
  const int o1 = orientation(p0, p1, q1);
  const int o2 = orientation(q0, q1, p0);
  const int o3 = orientation(q0, q1, p1);
  return (o0 * o1 <= 0) && (o2 * o3 <= 0);
}

// Returns whether the segment (p1, p2) intersects with the rectangle r.
// This includes the case where the segment is entirely contained within the
// rectangle.
bool SegmentRectangleIntersect(const Int2D& p1, const Int2D& p2,
                               const Rectangle& r) {
  // Check if the segment's bounding box intersects with the rectangle
  if (std::max(p1.x, p2.x) < r.x_min || std::min(p1.x, p2.x) > r.x_max ||
      std::max(p1.y, p2.y) < r.y_min || std::min(p1.y, p2.y) > r.y_max) {
    return false;
  }

  // Check if either endpoint is inside the rectangle
  if ((p1.x >= r.x_min && p1.x <= r.x_max && p1.y >= r.y_min &&
       p1.y <= r.y_max) ||
      (p2.x >= r.x_min && p2.x <= r.x_max && p2.y >= r.y_min &&
       p2.y <= r.y_max)) {
    return true;  // At least one endpoint is inside
  }

  // Check if the segment intersects with any of the four edges of the rectangle
  const Int2D q0(r.x_min, r.y_min);
  const Int2D q1(r.x_max, r.y_min);
  const Int2D q2(r.x_max, r.y_max);
  const Int2D q3(r.x_min, r.y_max);
  
  // Check intersection with each edge of the rectangle
  return SegmentsIntersect(p1, p2, q0, q1) ||
         SegmentsIntersect(p1, p2, q1, q2) ||
         SegmentsIntersect(p1, p2, q2, q3) ||
         SegmentsIntersect(p1, p2, q3, q0);
}

// Returns whether the point q is inside or on the boundary of the triangle
// (p0, p1, p2).
bool InTriangle(const Int2D& p0, const Int2D& p1, const Int2D& p2,
                const Int2D& q) {
  auto sign = [](const int x) -> int { return (x > 0) - (x < 0); };
  auto orientation = [&](const Int2D& a, const Int2D& b,
                         const Int2D& c) -> int {
    const int cross_product =
        (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return sign(cross_product);
  };

  // Degenerate triangle (area == 0): treat as segment/point; include boundary.
  const int area_sign = orientation(p0, p1, p2);
  if (area_sign == 0) {
    // Check collinearity with the line and bounding box within the hull of p0,p1,p2.
    if (orientation(p0, p1, q) != 0) return false;
    const int x_min = std::min({p0.x, p1.x, p2.x});
    const int x_max = std::max({p0.x, p1.x, p2.x});
    const int y_min = std::min({p0.y, p1.y, p2.y});
    const int y_max = std::max({p0.y, p1.y, p2.y});
    return (q.x >= x_min && q.x <= x_max && q.y >= y_min && q.y <= y_max);
  }

  const int o0 = orientation(p0, p1, q);
  const int o1 = orientation(p1, p2, q);
  const int o2 = orientation(p2, p0, q);
  // Inside or on boundary if all have the same sign (zeros allowed).
  const bool non_negative = (o0 >= 0) && (o1 >= 0) && (o2 >= 0);
  const bool non_positive = (o0 <= 0) && (o1 <= 0) && (o2 <= 0);
  return non_negative || non_positive;
}

// NOTE: This file is mostly generated by Cursor Agent.
// TODO: Clean up the code.

// Maximum number of segments in a leaf node
static constexpr int kMaxLeafSize = 8;

// AABB (Axis-Aligned Bounding Box) structure
struct AABB {
  int x_min, y_min, x_max, y_max;

  AABB()
      : x_min(std::numeric_limits<int>::max()),
        y_min(std::numeric_limits<int>::max()),
        x_max(std::numeric_limits<int>::min()),
        y_max(std::numeric_limits<int>::min()) {}

  AABB(int x_min, int y_min, int x_max, int y_max)
      : x_min(x_min), y_min(y_min), x_max(x_max), y_max(y_max) {}

  // Create AABB from a segment
  static AABB FromSegment(const Segment& seg) {
    return AABB(std::min(seg.p0.x, seg.p1.x), std::min(seg.p0.y, seg.p1.y),
                std::max(seg.p0.x, seg.p1.x), std::max(seg.p0.y, seg.p1.y));
  }

  // Merge two AABBs
  void Merge(const AABB& other) {
    x_min = std::min(x_min, other.x_min);
    y_min = std::min(y_min, other.y_min);
    x_max = std::max(x_max, other.x_max);
    y_max = std::max(y_max, other.y_max);
  }

  // Check if this AABB intersects with another AABB
  bool Intersects(const AABB& other) const {
    return !(x_max < other.x_min || other.x_max < x_min ||
             y_max < other.y_min || other.y_max < y_min);
  }

  // Check if this AABB intersects with a segment
  bool IntersectsSegment(const Segment& seg) const {
    Rectangle rect(x_min, y_min, x_max, y_max);
    return SegmentRectangleIntersect(seg.p0, seg.p1, rect);
  }
};

// Hilbert key calculation for 2D coordinates
class HilbertKey {
 public:
  // Rotate/flip quadrant appropriately
  static void Rotate(int n, int& x, int& y, int rx, int ry) {
    if (ry == 0) {
      if (rx == 1) {
        x = n - 1 - x;
        y = n - 1 - y;
      }
      // Swap x and y
      std::swap(x, y);
    }
  }

  // Convert (x, y) to Hilbert curve distance
  static uint64_t XYToHilbert(int n, int x, int y) {
    uint64_t distance = 0;
    for (int s = n / 2; s > 0; s /= 2) {
      int rx = (x & s) > 0;
      int ry = (y & s) > 0;
      distance += s * s * ((3 * rx) ^ ry);
      Rotate(s, x, y, rx, ry);
    }
    return distance;
  }

  // Calculate Hilbert key for a point
  static uint64_t Calculate(int x, int y, int min_coord, int max_coord) {
    // Shift to positive range and normalize to power of 2
    int range = max_coord - min_coord + 1;
    
    // Find the smallest power of 2 that contains the range
    int n = 1;
    while (n < range) {
      n *= 2;
    }
    
    // Normalize coordinates to [0, n)
    int norm_x = static_cast<int>(
        (static_cast<int64_t>(x - min_coord) * n) / range);
    int norm_y = static_cast<int>(
        (static_cast<int64_t>(y - min_coord) * n) / range);
    
    // Ensure coordinates are within bounds
    norm_x = std::min(norm_x, n - 1);
    norm_y = std::min(norm_y, n - 1);
    
    return XYToHilbert(n, norm_x, norm_y);
  }
};

// AABB Tree node structure
struct AABBNode {
  AABB aabb;
  int left_child;     // Index of left child node (-1 for leaf)
  int right_child;    // Index of right child node (-1 for leaf)
  int segment_start;  // Start index in segment array (for leaf nodes)
  int segment_count;  // Number of segments (for leaf nodes)

  AABBNode()
      : left_child(-1), right_child(-1), segment_start(-1), segment_count(0) {}

  bool IsLeaf() const { return left_child == -1 && right_child == -1; }
};

// AABB Tree class for managing a collection of segments
class AABBTree {
 private:
  vector<Segment> segments_;
  vector<int> segment_indices_;  // Sorted indices
  vector<AABBNode> nodes_;
  int root_node_index_;

  // Build tree recursively
  int BuildNode(int start, int end) {
    int node_index = static_cast<int>(nodes_.size());
    nodes_.emplace_back();
    AABBNode& node = nodes_[node_index];

    // Calculate AABB for this node
    for (int i = start; i < end; ++i) {
      AABB segment_aabb = AABB::FromSegment(segments_[segment_indices_[i]]);
      node.aabb.Merge(segment_aabb);
    }

    int count = end - start;

    // Create leaf node if segment count is small enough
    if (count <= kMaxLeafSize) {
      node.segment_start = start;
      node.segment_count = count;
      return node_index;
    }

    // Split segments and create child nodes
    int mid = start + count / 2;
    node.left_child = BuildNode(start, mid);
    node.right_child = BuildNode(mid, end);

    return node_index;
  }

  // Recursive intersection check
  bool IntersectsRecursive(const int node_index, const Segment& query_segment,
                           const AABB& query_aabb,
                           const optional<Segment>& opt_segment_removed) const {
    if (node_index == -1) return false;

    const AABBNode& node = nodes_[node_index];

    // Early exit if AABBs don't intersect
    if (!node.aabb.Intersects(query_aabb)) {
      return false;
    }

    if (node.IsLeaf()) {
      // Check each segment in the leaf
      for (int i = 0; i < node.segment_count; ++i) {
        const Segment& seg =
            segments_[segment_indices_[node.segment_start + i]];
        if (SegmentsDisallowed(query_segment, seg)) {
          if (opt_segment_removed &&
              seg.IsEqualToUnordered(*opt_segment_removed)) {
            continue;
          }
          return true;
        }
      }
      return false;  // No intersection found
    }

    // Check child nodes
    return IntersectsRecursive(node.left_child, query_segment, query_aabb,
                               opt_segment_removed) ||
           IntersectsRecursive(node.right_child, query_segment, query_aabb,
                               opt_segment_removed);
  }

 public:
  AABBTree() : root_node_index_(-1) {}

  // Constructor: build AABB tree from segments
  AABBTree(const vector<Segment>& segments) : segments_(segments) {
    if (segments.empty()) {
      root_node_index_ = -1;
      return;
    }

    // Calculate bounding box of all segments
    int min_x = std::numeric_limits<int>::max();
    int min_y = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int max_y = std::numeric_limits<int>::min();

    for (const auto& seg : segments) {
      min_x = std::min(min_x, std::min(seg.p0.x, seg.p1.x));
      min_y = std::min(min_y, std::min(seg.p0.y, seg.p1.y));
      max_x = std::max(max_x, std::max(seg.p0.x, seg.p1.x));
      max_y = std::max(max_y, std::max(seg.p0.y, seg.p1.y));
    }

    int min_coord = std::min(min_x, min_y);
    int max_coord = std::max(max_x, max_y);

    // Create indices and sort by Hilbert key of segment midpoints
    struct SegmentWithHilbert {
      int index;
      uint64_t hilbert_key;
    };

    vector<SegmentWithHilbert> segments_with_hilbert;
    segments_with_hilbert.reserve(segments.size());

    for (size_t i = 0; i < segments.size(); ++i) {
      const Segment& seg = segments[i];
      int mid_x = (seg.p0.x + seg.p1.x) / 2;
      int mid_y = (seg.p0.y + seg.p1.y) / 2;
      uint64_t hilbert =
          HilbertKey::Calculate(mid_x, mid_y, min_coord, max_coord);
      segments_with_hilbert.push_back({static_cast<int>(i), hilbert});
    }

    // Sort by Hilbert key
    std::sort(segments_with_hilbert.begin(), segments_with_hilbert.end(),
              [](const SegmentWithHilbert& a, const SegmentWithHilbert& b) {
                return a.hilbert_key < b.hilbert_key;
              });

    // Extract sorted indices
    segment_indices_.reserve(segments.size());
    for (const auto& item : segments_with_hilbert) {
      segment_indices_.push_back(item.index);
    }

    // Build the tree
    nodes_.reserve(2 * segments.size());  // Reserve space for nodes
    root_node_index_ = BuildNode(0, static_cast<int>(segments.size()));
  }

  // Check if any segment in the tree intersects with the given segment
  bool HasIntersection(const Segment& query_segment,
                       const optional<Segment>& opt_segment_removed) const {
    // std::cerr << "HasIntersection\n";
    if (root_node_index_ == -1) return false;

    AABB query_aabb = AABB::FromSegment(query_segment);
    bool result =
        IntersectsRecursive(root_node_index_, query_segment, query_aabb,
          opt_segment_removed);
    // if (!result) {
    //   std::cerr << "check_count: " << check_count << "\n";
    // }
    return result;
  }
};

#pragma once


#pragma once



template <typename T, int N>
class ArrayMap {
 private:
  array<pair<bool, T>, N> data_;
  vector<int> keys_;

 public:
  ArrayMap() { data_.fill({false, T()}); }

  void Clear() {
    for (const int key : keys_) {
      data_[key] = {false, T()};
    }
    keys_.clear();
  }

  T& operator[](const int key) {
    if (!data_[key].first) {
      keys_.push_back(key);
      data_[key].first = true;
    }
    return data_[key].second;
  }

  const T& operator[](const int key) const {
    return data_[key].second;
  }
};



// Transition of:
// - Adding a node v3.
// - Changing (v0, v1) to (v0, v3), (v3, v1) and (v3, v2).
struct AddTransition {
  int v0 = -1;
  int v1 = -1;
  int v2 = -1;
  int v3 = -1;

  // -1 if the sorter type is not determined.
  int sorter_type = -1;

  // 0: The order of edges from v3 is (v3, v1), (v3, v2).
  // 1: The order of edges from v3 is (v3, v2), (v3, v1).
  // -1 if the parity is not determined.
  int parity = -1;
  AddTransition()
      : v0(-1), v1(-1), v2(-1), v3(-1), sorter_type(-1), parity(-1) {}
  AddTransition(const int v0, const int v1, const int v2, const int v3)
      : v0(v0), v1(v1), v2(v2), v3(v3), sorter_type(-1), parity(-1) {}
  AddTransition(const int v0, const int v1, const int v2, const int v3,
                const int sorter_type, const int parity)
      : v0(v0), v1(v1), v2(v2), v3(v3), sorter_type(sorter_type),
        parity(parity) {}
};

void ApplyTransition(const AddTransition& transition, State& state) {
  state.node_types[transition.v3] = transition.sorter_type;

  if (state.node_destination_pairs[transition.v0].first == transition.v1) {
    state.node_destination_pairs[transition.v0].first = transition.v3;
  } else if (state.node_destination_pairs[transition.v0].second == transition.v1) {
    state.node_destination_pairs[transition.v0].second = transition.v3;
  } else {
    UNEXPECTED();
  }

  if (transition.parity == 0) {
    state.node_destination_pairs[transition.v3] = {transition.v1, transition.v2};
  } else if (transition.parity == 1) {
    state.node_destination_pairs[transition.v3] = {transition.v2, transition.v1};
  } else {
    UNEXPECTED();
  }
}

#pragma once

#include <queue>
#include <tuple>
#include <unordered_map>





#pragma once




#pragma once






#pragma once

#include <cassert>



// NOTE: Generated by Cursor Agent.
struct BitMatrix {
  private:
   int r_size_;
   int c_size_;
   int words_per_row_;  // Number of uint64 words per row
   vector<uint64> data_;
   
   // Helper function to get the position of bit (r, c) in data_
   int get_word_index(const int r, const int c) const {
     return r * words_per_row_ + c / 64;
   }
   
   int get_bit_index([[maybe_unused]] const int r, const int c) const {
     return c % 64;
   }
 
  public:
   int r_size() const { return r_size_; }
   int c_size() const { return c_size_; }
   int words_per_row() const { return words_per_row_; }  // For advanced usage
 
   // Get pointer to the start of row r (for high-speed operations)
   uint64* get_row_ptr(const int r) { return &data_[r * words_per_row_]; }
   const uint64* get_row_ptr(const int r) const {
     return &data_[r * words_per_row_];
   }
 
   // Constructors
   BitMatrix(const int r_size, const int c_size)
       : r_size_(r_size), c_size_(c_size) {
     words_per_row_ = (c_size + 63) / 64;  // Round up division
     int total_words = r_size * words_per_row_;
     data_.resize(total_words, 0);
   }
 
   BitMatrix() : r_size_(0), c_size_(0), words_per_row_(0), data_() {}
 
   // Get bit value at (r, c)
   bool Get(const int r, const int c) const {
     int word_idx = get_word_index(r, c);
     int bit_idx = get_bit_index(r, c);
     return (data_[word_idx] >> bit_idx) & 1;
   }
   
   // Set bit value at (r, c)
   void Set(const int r, const int c, const bool value) {
     int word_idx = get_word_index(r, c);
     int bit_idx = get_bit_index(r, c);
     
     if (value) {
       data_[word_idx] |= (1ULL << bit_idx);
     } else {
       data_[word_idx] &= ~(1ULL << bit_idx);
     }
   }
 
   // Copy another row to this row
   void CopyRow(const int dest_row, const int src_row) {
     uint64* dest_ptr = get_row_ptr(dest_row);
     const uint64* src_ptr = get_row_ptr(src_row);
     for (int i = 0; i < words_per_row_; ++i) {
       dest_ptr[i] = src_ptr[i];
     }
   }
 
   // OR another row into this row (dest_row |= src_row)
   void OrRow(const int dest_row, const int src_row) {
     uint64* dest_ptr = get_row_ptr(dest_row);
     const uint64* src_ptr = get_row_ptr(src_row);
     for (int i = 0; i < words_per_row_; ++i) {
       dest_ptr[i] |= src_ptr[i];
     }
   }
 };

#pragma once





// Change (v0, v1) to (v0, v2).
struct ChangeTransition {
  int v0 = -1;
  int v1 = -1;
  int v2 = -1;
};

#pragma once

constexpr bool kEnableDeterministicMode = false;

int v3_time_us = 0;
int scorer_type_time_us = 0;
int candidate_generation_time_us = 0;
int pre_process_entry_time_us = 0;
int companion_time_us = 0;
int change_transition_time_us = 0;
int change_transition_companion_time_us = 0;
int sorter_types_optimization_time_us = 0;
int aabb_tree_time_us = 0;
int node_neighbor_time_us = 0;
int compainon_others_time_us = 0;
int companion_copy_time_us = 0;


#pragma once




optional<vector<int>> GetTopologicalOrder(const Config& config,
                                          const State& state) {
  const int node_count = config.NodeCount();

  // 0: unvisited, 1: visiting, 2: visited
  std::vector<int8> node_statuses(node_count, 0);
  std::vector<int> topological_order;
  auto dfs = [&](auto self, const int node_id) -> bool {
    node_statuses[node_id] = 1;
    const auto [v1, v2] = state.node_destination_pairs[node_id];
    if (v1 >= 0) {
      if (node_statuses[v1] == 1) {
        // Cycle detected.
        return false;
      }
      if (node_statuses[v1] == 0) {
        if (!self(self, v1)) {
          return false;
        }
      }
    }
    if (v2 >= 0) {
      if (node_statuses[v2] == 1) {
        // Cycle detected.
        return false;
      }
      if (node_statuses[v2] == 0) {
        if (!self(self, v2)) {
          return false;
        }
      }
    }
    node_statuses[node_id] = 2;
    topological_order.push_back(node_id);
    return true;
  };
  const bool result = dfs(dfs, config.InletNodeId());
  if (!result) {
    return nullopt;
  }
  std::reverse(topological_order.begin(), topological_order.end());
  return topological_order;
}


// Companion precomputed data for State.
struct StateCompanion {
  // Active nodes in the topological order.
  vector<int> active_nodes;

  // node_id_to_active_index[v] is the index of v in active_nodes. -1 if v is
  // not active.
  vector<int> node_id_to_active_index;

  BitMatrix transitive_closure;

  vector<Segment> edges;

  // node_distance_scores[v] is the minimum Euclidean distance to any active
  // node. 0 for active nodes themselves.
  vector<float> node_distance_scores;

  // active_node_neighbors[k] is the list of active nodes that are neighbors of
  // active_nodes[k]. Note that neighbors are not necessarily active and
  // represented in the raw node indices.
  vector<vector<int>> active_node_neighbors;

  // active_node_bounding_boxes[k] is the bounding box of
  // active_node_neighbors[k].
  vector<Rectangle> active_node_bounding_boxes;

  AABBTree aabb_tree;
};

namespace state_companion_internal {

vector<int> MakeActiveNodes(const Config& config, const State& state) {
  const optional<vector<int>> opt_topological_order =
      GetTopologicalOrder(config, state);
  if (!opt_topological_order) {
    UNEXPECTED();
  }
  return *opt_topological_order;
}

vector<int> MakeNodeIdToActiveIndex(const Config& config,
                                    const vector<int>& active_nodes) {
  vector<int> node_id_to_active_index(config.NodeCount(), -1);
  for (const int k : iota(0, int(active_nodes.size()))) {
    node_id_to_active_index[active_nodes[k]] = k;
  }
  return node_id_to_active_index;
}

BitMatrix MakeTransitiveClosure(const State& state,
                                const vector<int>& active_nodes,
                                const vector<int>& node_id_to_active_index) {
  const int active_node_count = active_nodes.size();
  BitMatrix transitive_closure(active_node_count, active_node_count);
  for (const int k : iota(0, active_node_count) | std::views::reverse) {
    transitive_closure.Set(k, k, true);
    const auto [v0, v1] = state.node_destination_pairs[active_nodes[k]];
    if (v0 >= 0) {
      const int k0 = node_id_to_active_index[v0];
      transitive_closure.OrRow(k, k0);
    }
    if (v1 >= 0) {
      const int k1 = node_id_to_active_index[v1];
      transitive_closure.OrRow(k, k1);
    }
  }
  return transitive_closure;
}

vector<Segment> MakeEdges(const Config& config, const State& state,
                          const vector<int>& active_nodes) {
  const int active_node_count = active_nodes.size();
  vector<pair<Segment, int>> scored_edges;
  for (const int k : iota(0, active_node_count)) {
    const int v = active_nodes[k];
    const auto [v0, v1] = state.node_destination_pairs[v];
    const Int2D& v_p = config.node_positions[v];
    if (v0 >= 0) {
      const Int2D& v0_p = config.node_positions[v0];
      scored_edges.push_back({Segment(v_p, v0_p), v_p.Distance2(v0_p)});
    }
    if (v1 >= 0) {
      const Int2D& v1_p = config.node_positions[v1];
      scored_edges.push_back({Segment(v_p, v1_p), v_p.Distance2(v1_p)});
    }
  }
  // Sort by decreasing order of length to increase chance of early rejection.
  std::sort(scored_edges.begin(), scored_edges.end(),
            [&](const pair<Segment, int>& a, const pair<Segment, int>& b) {
              return a.second > b.second;
            });
  vector<Segment> edges;
  edges.reserve(scored_edges.size());
  for (const auto& [edge, _] : scored_edges) {
    edges.push_back(edge);
  }
  return edges;
}

vector<float> MakeNodeDistanceScores(
    const Config& config, const vector<int>& active_nodes,
    const vector<int>& node_id_to_active_index) {
  const int node_count = config.NodeCount();
  vector<float> node_distance_scores(node_count);

  for (const int v : iota(0, node_count)) {
    // Skip active nodes.
    if (node_id_to_active_index[v] >= 0) continue;

    const Int2D& v_p = config.node_positions[v];
    int min_distance2 = INT_MAX;
    for (const int u : active_nodes) {
      const Int2D& u_p = config.node_positions[u];
      min_distance2 = std::min(min_distance2, v_p.Distance2(u_p));
    }
    node_distance_scores[v] = std::sqrt(float(min_distance2));
  }

  return node_distance_scores;
}

// Delta update of node_distance_scores when a new active node v3 is added.
vector<float> MakeNodeDistanceScoresByAddTransition(
    const Config& config, const vector<float>& base_node_distance_scores,
    const AddTransition& transition) {
  vector<float> node_distance_scores = base_node_distance_scores;
  const int v3 = transition.v3;
  const Int2D& v3_p = config.node_positions[v3];
  node_distance_scores[v3] = 0.0f;
  for (const int v : iota(0, config.NodeCount())) {
    if (v == v3) continue;
    const Int2D& v_p = config.node_positions[v];
    const float d = v_p.Distance(v3_p);
    if (d < node_distance_scores[v]) {
      node_distance_scores[v] = d;
    }
  }
  return node_distance_scores;
}

pair<vector<vector<int>>, vector<Rectangle>>
MakeActiveNodeNeighborsAndBoundingBoxes(const Config& config,
                                        const vector<int>& active_nodes,
                                        const vector<Segment>& edges) {
  const int active_node_count = active_nodes.size();
  vector<vector<int>> active_node_neighbors(active_node_count);
  vector<Rectangle> active_node_bounding_boxes(active_node_count);

  for (const int k0 : iota(0, active_node_count)) {
    const int v0 = active_nodes[k0];
    const Int2D& v0_p = config.node_positions[v0];
    for (const int v1 : iota(0, config.NodeCount())) {
      if (v0 == v1) continue;
      const Int2D& v1_p = config.node_positions[v1];
      const Segment segment_v0_v1(v0_p, v1_p);
      bool is_neighbor = true;
      for (const Segment& edge : edges) {
        if (SegmentsDisallowed(segment_v0_v1, edge)) {
          is_neighbor = false;
          break;
        }
      }
      if (is_neighbor) {
        active_node_neighbors[k0].push_back(v1);
      }
    }
  }

  for (const int k0 : iota(0, active_node_count)) {
    const int v0 = active_nodes[k0];
    const Int2D& v0_p = config.node_positions[v0];
    int x_min = v0_p.x;
    int y_min = v0_p.y;
    int x_max = v0_p.x;
    int y_max = v0_p.y;
    for (const int v1 : active_node_neighbors[k0]) {
      const Int2D& v1_p = config.node_positions[v1];
      x_min = std::min(x_min, v1_p.x);
      y_min = std::min(y_min, v1_p.y);
      x_max = std::max(x_max, v1_p.x);
      y_max = std::max(y_max, v1_p.y);
    }
    active_node_bounding_boxes[k0] = Rectangle(x_min, y_min, x_max, y_max);
  }

  return {active_node_neighbors, active_node_bounding_boxes};
}

pair<vector<vector<int>>, vector<Rectangle>>
MakeActiveNodeNeighborsAndBoundingBoxesByAddTransition(
    const Config& config, const State& state,
    const StateCompanion& base_state_companion, const vector<int>& active_nodes,
    const vector<int>& node_id_to_active_index, const AABBTree& aabb_tree,
    const AddTransition& transition) {
  const int active_node_count = active_nodes.size();
  vector<vector<int>> active_node_neighbors(active_node_count);
  vector<Rectangle> active_node_bounding_boxes(active_node_count);

  const vector<int>& node_id_to_base_active_node_index =
      base_state_companion.node_id_to_active_index;

  const int v0 = transition.v0;
  const int v1 = transition.v1;
  const int v2 = transition.v2;
  const int v3 = transition.v3;
  const Int2D& v0_p = config.node_positions[v0];
  const Int2D& v1_p = config.node_positions[v1];
  const Int2D& v2_p = config.node_positions[v2];
  const Int2D& v3_p = config.node_positions[v3];

  for (const int k : iota(0, active_node_count)) {
    const int v = active_nodes[k];
    const Int2D& v_p = config.node_positions[v];
    if (v == v3) {
      // v is the new active node.
      for (const int u : iota(0, config.NodeCount())) {
        if (u == v) continue;
        const Int2D& u_p = config.node_positions[u];
        if (aabb_tree.HasIntersection(Segment(v_p, u_p), nullopt)) continue;
        active_node_neighbors[k].push_back(u);
      }

      int x_min = v_p.x;
      int y_min = v_p.y;
      int x_max = v_p.x;
      int y_max = v_p.y;
      for (const int u : active_node_neighbors[k]) {
        const Int2D& u_p = config.node_positions[u];
        x_min = std::min(x_min, u_p.x);
        y_min = std::min(y_min, u_p.y);
        x_max = std::max(x_max, u_p.x);
        y_max = std::max(y_max, u_p.y);
      }
      active_node_bounding_boxes[k] = Rectangle(x_min, y_min, x_max, y_max);
    } else {
      const int base_k = node_id_to_base_active_node_index[v];
      if (base_k < 0) UNEXPECTED();
      const Rectangle& bounding_box =
          base_state_companion.active_node_bounding_boxes[base_k];
      const bool check_03 = SegmentRectangleIntersect(v0_p, v3_p, bounding_box);
      const bool check_31 = SegmentRectangleIntersect(v3_p, v1_p, bounding_box);
      const bool check_32 = SegmentRectangleIntersect(v3_p, v2_p, bounding_box);
      if (!check_03 && !check_31 && !check_32) {
        // If no segments intersect with the bounding box, keep the neighbors
        // in the base state.
        active_node_neighbors[k] =
            base_state_companion.active_node_neighbors[base_k];
        active_node_bounding_boxes[k] =
            base_state_companion.active_node_bounding_boxes[base_k];
        continue;
      }

      for (const int u : base_state_companion.active_node_neighbors[base_k]) {
        const Int2D& u_p = config.node_positions[u];
        if (check_03 && v != v0 && v != v3 && u != v0 && u != v3 &&
            SegmentsIntersect(v_p, u_p, v0_p, v3_p)) {
          continue;
        }
        if (check_31 && v != v3 && v != v1 && u != v3 && u != v1 &&
            SegmentsIntersect(v_p, u_p, v3_p, v1_p)) {
          continue;
        }
        if (check_32 && v != v3 && v != v2 && u != v3 && u != v2 &&
            SegmentsIntersect(v_p, u_p, v3_p, v2_p)) {
          continue;
        }

        active_node_neighbors[k].push_back(u);
      }

      int x_min = v_p.x;
      int y_min = v_p.y;
      int x_max = v_p.x;
      int y_max = v_p.y;
      for (const int u : active_node_neighbors[k]) {
        const Int2D& u_p = config.node_positions[u];
        x_min = std::min(x_min, u_p.x);
        y_min = std::min(y_min, u_p.y);
        x_max = std::max(x_max, u_p.x);
        y_max = std::max(y_max, u_p.y);
      }
      active_node_bounding_boxes[k] = Rectangle(x_min, y_min, x_max, y_max);
    }
  }

  // Process removal of (v0, v1).

  // When there are two parallel (v0, v1) edges, removal of (v0, v1) does not
  // affect the neighbors of any node.
  const bool v0_has_parallel_edges = state.node_destination_pairs[v0].first ==
                                     state.node_destination_pairs[v0].second;
  if (v0_has_parallel_edges) {
    return {active_node_neighbors, active_node_bounding_boxes};
  }

  // For affected pairs (v, u), either v or u should be in the triangle
  // (v0, v1, v3).
  // Only handle the cases where v is an active node and u is an empty node.
  // We didn't observe the opposite cases in simple tests.

  // Collect nodes in the triangle (v0, v1, v3).
  vector<int> nodes_in_triangle;
  for (const int v : iota(0, config.NodeCount())) {
    if (v == v0 || v == v1) continue;
    if (InTriangle(v0_p, v1_p, v3_p, config.node_positions[v])) {
      nodes_in_triangle.push_back(v);
    }
  }

  // Update visibility of (v, u) where v is visible from v0 or v1 and u is in
  // the trianvle (v0, v1, v3). This constraint on v doesn't always hold, but we
  // apply this heuristic for computational efficiency.
  static ArrayMap<int, kMaxNodeCount> is_v_candidate_flags;
  is_v_candidate_flags.Clear();
  for (const int v : active_node_neighbors[node_id_to_active_index[v0]]) {
    is_v_candidate_flags[v] = 1;
  }
  for (const int v : active_node_neighbors[node_id_to_active_index[v1]]) {
    is_v_candidate_flags[v] = 1;
  }

  for (const int k : iota(0, active_node_count)) {
    const int v = active_nodes[k];
    if (!is_v_candidate_flags[v]) continue;
    if (v == v0 || v == v1) continue;
    const Int2D& v_p = config.node_positions[v];

    for (const int u : nodes_in_triangle) {
      const Int2D& u_p = config.node_positions[u];

      if (!SegmentsIntersect(v_p, u_p, v0_p, v1_p)) continue;

      if (aabb_tree.HasIntersection(Segment(v_p, u_p), nullopt)) {
        continue;
      }

      active_node_neighbors[k].push_back(u);
      active_node_bounding_boxes[k].x_min =
          std::min(active_node_bounding_boxes[k].x_min, u_p.x);
      active_node_bounding_boxes[k].y_min =
          std::min(active_node_bounding_boxes[k].y_min, u_p.y);
      active_node_bounding_boxes[k].x_max =
          std::max(active_node_bounding_boxes[k].x_max, u_p.x);
      active_node_bounding_boxes[k].y_max =
          std::max(active_node_bounding_boxes[k].y_max, u_p.y);
    }
  }

  return {active_node_neighbors, active_node_bounding_boxes};
}

pair<vector<vector<int>>, vector<Rectangle>>
MakeActiveNodeNeighborsAndBoundingBoxesByChangeTransition(
    const Config& config, const StateCompanion& base_state_companion,
    const vector<int>& active_nodes, const vector<int>& node_id_to_active_index,
    const AABBTree& aabb_tree, const ChangeTransition& transition) {
  const int active_node_count = active_nodes.size();
  vector<vector<int>> active_node_neighbors(active_node_count);
  vector<Rectangle> active_node_bounding_boxes(active_node_count);

  const vector<int>& node_id_to_base_active_node_index =
      base_state_companion.node_id_to_active_index;

  const int v0 = transition.v0;
  const int v1 = transition.v1;
  const int v2 = transition.v2;

  const Int2D& v0_p = config.node_positions[v0];
  const Int2D& v1_p = config.node_positions[v1];
  const Int2D& v2_p = config.node_positions[v2];

  // Process addition of (v0, v2).
  for (const int v : active_nodes) {
    const Int2D& v_p = config.node_positions[v];
    const int k = node_id_to_active_index[v];
    const int base_k = node_id_to_base_active_node_index[v];
    const Rectangle& bounding_box =
        base_state_companion.active_node_bounding_boxes[base_k];
    if (!SegmentRectangleIntersect(v0_p, v2_p, bounding_box)) {
      active_node_neighbors[k] =
          base_state_companion.active_node_neighbors[base_k];
      active_node_bounding_boxes[k] =
          base_state_companion.active_node_bounding_boxes[base_k];
      continue;
    }

    for (const int u : base_state_companion.active_node_neighbors[base_k]) {
      const Int2D& u_p = config.node_positions[u];
      if (SegmentsDisallowed(Segment(v_p, u_p), Segment(v0_p, v2_p))) continue;
      active_node_neighbors[k].push_back(u);
    }

    int x_min = v_p.x;
    int y_min = v_p.y;
    int x_max = v_p.x;
    int y_max = v_p.y;
    for (const int u : active_node_neighbors[k]) {
      const Int2D& u_p = config.node_positions[u];
      x_min = std::min(x_min, u_p.x);
      y_min = std::min(y_min, u_p.y);
      x_max = std::max(x_max, u_p.x);
      y_max = std::max(y_max, u_p.y);
    }
    active_node_bounding_boxes[k] = Rectangle(x_min, y_min, x_max, y_max);
  }

  // Update visibility of (u0, u1) where u0 and u1 are visible from either v0 or
  // v1. This constraint on u0 and u1 doesn't always hold, but we apply this
  // heuristic for computational efficiency.
  static ArrayMap<int, kMaxNodeCount> is_u_candidate_flags;
  vector<int> u_candidates;
  is_u_candidate_flags.Clear();
  for (const int v : active_node_neighbors[node_id_to_active_index[v0]]) {
    is_u_candidate_flags[v] = 1;
    u_candidates.push_back(v);
  }
  for (const int v : active_node_neighbors[node_id_to_active_index[v1]]) {
    if (is_u_candidate_flags[v]) continue;
    is_u_candidate_flags[v] = 1;
    u_candidates.push_back(v);
  }

  for (const int k : iota(0, active_node_count)) {
    const int u0 = active_nodes[k];
    if (!is_u_candidate_flags[u0]) continue;
    const Int2D& u0_p = config.node_positions[u0];
    for (const int u1 : u_candidates) {
      const Int2D& u1_p = config.node_positions[u1];

      // Not affected by removal of (v0, v1).
      if (!SegmentsDisallowed(Segment(u0_p, u1_p), Segment(v0_p, v1_p))) {
        continue;
      }

      if (aabb_tree.HasIntersection(Segment(u0_p, u1_p), nullopt)) {
        continue;
      }

      active_node_neighbors[k].push_back(u1);
      active_node_bounding_boxes[k].x_min =
          std::min(active_node_bounding_boxes[k].x_min, u1_p.x);
      active_node_bounding_boxes[k].y_min =
          std::min(active_node_bounding_boxes[k].y_min, u1_p.y);
      active_node_bounding_boxes[k].x_max =
          std::max(active_node_bounding_boxes[k].x_max, u1_p.x);
      active_node_bounding_boxes[k].y_max =
          std::max(active_node_bounding_boxes[k].y_max, u1_p.y);
    }
  }

  return {active_node_neighbors, active_node_bounding_boxes};
}

AABBTree MakeAABBTree(const vector<Segment>& edges) { return AABBTree(edges); }

StateCompanion MakeStateCompanionInternal(const Config& config,
                                          const State& state) {
  StateCompanion state_companion;

  state_companion.active_nodes = MakeActiveNodes(config, state);
  state_companion.node_id_to_active_index =
      MakeNodeIdToActiveIndex(config, state_companion.active_nodes);

  // Set the transitive closure.
  state_companion.transitive_closure =
      MakeTransitiveClosure(state, state_companion.active_nodes,
                            state_companion.node_id_to_active_index);

  state_companion.edges =
      MakeEdges(config, state, state_companion.active_nodes);

  state_companion.node_distance_scores =
      MakeNodeDistanceScores(config, state_companion.active_nodes,
                             state_companion.node_id_to_active_index);

  std::tie(state_companion.active_node_neighbors,
           state_companion.active_node_bounding_boxes) =
      MakeActiveNodeNeighborsAndBoundingBoxes(
          config, state_companion.active_nodes, state_companion.edges);
  state_companion.aabb_tree = MakeAABBTree(state_companion.edges);
  return state_companion;
}

StateCompanion MakeStateCompanionByAddTransitionInternal(
    const Config& config, const State& new_state,
    const StateCompanion& base_state_companion,
    const AddTransition& transition) {
  const vector<int> active_nodes = MakeActiveNodes(config, new_state);
  const vector<int> node_id_to_active_index =
      MakeNodeIdToActiveIndex(config, active_nodes);

  const BitMatrix transitive_closure =
      MakeTransitiveClosure(new_state, active_nodes, node_id_to_active_index);

  const vector<Segment> edges = MakeEdges(config, new_state, active_nodes);

  const vector<float> node_distance_scores =
      MakeNodeDistanceScoresByAddTransition(
          config, base_state_companion.node_distance_scores, transition);

  const AABBTree aabb_tree = MakeAABBTree(edges);

  auto [active_node_neighbors, active_node_bounding_boxes] =
      MakeActiveNodeNeighborsAndBoundingBoxesByAddTransition(
          config, new_state, base_state_companion, active_nodes,
          node_id_to_active_index, aabb_tree, transition);

  StateCompanion new_state_companion;
  new_state_companion.active_nodes = std::move(active_nodes);
  new_state_companion.node_id_to_active_index =
      std::move(node_id_to_active_index);
  new_state_companion.transitive_closure = std::move(transitive_closure);
  new_state_companion.edges = std::move(edges);
  new_state_companion.node_distance_scores = std::move(node_distance_scores);
  new_state_companion.aabb_tree = std::move(aabb_tree);
  new_state_companion.active_node_neighbors = std::move(active_node_neighbors);
  new_state_companion.active_node_bounding_boxes =
      std::move(active_node_bounding_boxes);
  return new_state_companion;
}

StateCompanion MakeStateCompanionByChangeTransitionInternal(
    const Config& config, const State& new_state,
    const StateCompanion& base_state_companion,
    const ChangeTransition& transition) {
  const vector<int> active_nodes = MakeActiveNodes(config, new_state);
  const vector<int> node_id_to_active_index =
      MakeNodeIdToActiveIndex(config, active_nodes);

  const BitMatrix transitive_closure =
      MakeTransitiveClosure(new_state, active_nodes, node_id_to_active_index);

  const vector<Segment> edges = MakeEdges(config, new_state, active_nodes);

  const vector<float> node_distance_scores =
      base_state_companion.node_distance_scores;

  const AABBTree aabb_tree = MakeAABBTree(edges);

  const auto [active_node_neighbors, active_node_bounding_boxes] =
      MakeActiveNodeNeighborsAndBoundingBoxesByChangeTransition(
          config, base_state_companion, active_nodes, node_id_to_active_index,
          aabb_tree, transition);

  StateCompanion new_state_companion;
  new_state_companion.active_nodes = active_nodes;
  new_state_companion.node_id_to_active_index = node_id_to_active_index;
  new_state_companion.transitive_closure = transitive_closure;
  new_state_companion.edges = edges;
  new_state_companion.node_distance_scores = node_distance_scores;
  new_state_companion.aabb_tree = aabb_tree;
  new_state_companion.active_node_neighbors = active_node_neighbors;
  new_state_companion.active_node_bounding_boxes = active_node_bounding_boxes;
  return new_state_companion;
}

}  // namespace state_companion_internal

StateCompanion MakeStateCompanion(const Config& config, const State& state) {
  return state_companion_internal::MakeStateCompanionInternal(config, state);
}

StateCompanion MakeStateCompanionByAddTransition(
    const Config& config, const State& state,
    const StateCompanion& base_state_companion,
    const AddTransition& transition) {
  return state_companion_internal::MakeStateCompanionByAddTransitionInternal(
      config, state, base_state_companion, transition);
}

StateCompanion MakeStateCompanionByChangeTransition(
    const Config& config, const State& state,
    const StateCompanion& base_state_companion,
    const ChangeTransition& transition) {
  return state_companion_internal::MakeStateCompanionByChangeTransitionInternal(
      config, state, base_state_companion, transition);
}

#if defined(__AVX2__)
#include <immintrin.h>
#endif  // __AVX2__

optional<tuple<AddTransition, float>> FinalizeAddTransition(
    const Config& config, const State& state,
    [[maybe_unused]] const StateCompanion& state_companion,
    const float base_score, const float min_required_score,
    const Matrix<float>& p_table, const Matrix<float>& grad_table,
    const AddTransition& transition) {
  const int v0 = transition.v0;
  const int v1 = transition.v1;
  const int v2 = transition.v2;
  const int v3 = transition.v3;

  const int k0 = state_companion.node_id_to_active_index[v0];
  const int k1 = state_companion.node_id_to_active_index[v1];
  const int k2 = state_companion.node_id_to_active_index[v2];

  const int v0_sorter_type = state.node_types[v0];

  float max_delta = 0.0f;
  int max_delta_sorter_type = -1;
  int max_delta_parity = -1;

  vector<float> p_row_v3((config.trash_type_count + 7) / 8 * 8, 0.0f);
  if (config.IsInletNode(v0)) {
#if defined(__AVX2__)
    {
      const float* src = &p_table[k0, 0];
      float* dst = p_row_v3.data();
      {
        const __m256 v0x = _mm256_loadu_ps(src + 0);
        _mm256_storeu_ps(dst + 0, v0x);
      }
      if (config.trash_type_count > 8) {
        const __m256 v8x = _mm256_loadu_ps(src + 8);
        _mm256_storeu_ps(dst + 8, v8x);
      }
      if (config.trash_type_count > 16) {
        const __m256 v16x = _mm256_loadu_ps(src + 16);
        _mm256_storeu_ps(dst + 16, v16x);
      }
    }
#else
    for (const int i : iota(0, config.trash_type_count)) {
      p_row_v3[i] = p_table[k0, i];
    }
#endif
  } else if (config.IsSorterNode(v0)) {
    if (state.node_destination_pairs[v0].first == v1) {
#if defined(__AVX2__)
      {
        const float* p0 = &p_table[k0, 0];
        const float* c = &config.sorter_configs[2 * v0_sorter_type, 0];
        float* dst = p_row_v3.data();
        {
          const __m256 vp = _mm256_loadu_ps(p0 + 0);
          const __m256 vc = _mm256_loadu_ps(c + 0);
          _mm256_storeu_ps(dst + 0, _mm256_mul_ps(vp, vc));
        }
        if (config.trash_type_count > 8) {
          const __m256 vp = _mm256_loadu_ps(p0 + 8);
          const __m256 vc = _mm256_loadu_ps(c + 8);
          _mm256_storeu_ps(dst + 8, _mm256_mul_ps(vp, vc));
        }
        if (config.trash_type_count > 16) {
          const __m256 vp = _mm256_loadu_ps(p0 + 16);
          const __m256 vc = _mm256_loadu_ps(c + 16);
          _mm256_storeu_ps(dst + 16, _mm256_mul_ps(vp, vc));
        }
      }
#else
      for (const int i : iota(0, config.trash_type_count)) {
        p_row_v3[i] =
            p_table[k0, i] * config.sorter_configs[2 * v0_sorter_type, i];
      }
#endif
    } else if (state.node_destination_pairs[v0].second == v1) {
#if defined(__AVX2__)
      {
        const float* p0 = &p_table[k0, 0];
        const float* c = &config.sorter_configs[2 * v0_sorter_type + 1, 0];
        float* dst = p_row_v3.data();
        {
          const __m256 vp = _mm256_loadu_ps(p0 + 0);
          const __m256 vc = _mm256_loadu_ps(c + 0);
          _mm256_storeu_ps(dst + 0, _mm256_mul_ps(vp, vc));
        }
        if (config.trash_type_count > 8) {
          const __m256 vp = _mm256_loadu_ps(p0 + 8);
          const __m256 vc = _mm256_loadu_ps(c + 8);
          _mm256_storeu_ps(dst + 8, _mm256_mul_ps(vp, vc));
        }
        if (config.trash_type_count > 16) {
          const __m256 vp = _mm256_loadu_ps(p0 + 16);
          const __m256 vc = _mm256_loadu_ps(c + 16);
          _mm256_storeu_ps(dst + 16, _mm256_mul_ps(vp, vc));
        }
      }
#else
      for (const int i : iota(0, config.trash_type_count)) {
        p_row_v3[i] =
            p_table[k0, i] * config.sorter_configs[2 * v0_sorter_type + 1, i];
      }
#endif
    } else {
      UNEXPECTED();
    }
  } else {
    UNEXPECTED();
  }

  // Delta by removing the edge (v0, v1).
  float base_delta = 0.0f;
#if defined(__AVX2__)
  {
    const float* pr = p_row_v3.data();
    const float* g1 = &grad_table[k1, 0];
    __m256 acc = _mm256_setzero_ps();
    {
      const __m256 vpr = _mm256_loadu_ps(pr + 0);
      const __m256 vg1 = _mm256_loadu_ps(g1 + 0);
      acc = _mm256_add_ps(acc, _mm256_mul_ps(vpr, vg1));
    }
    if (config.trash_type_count > 8) {
      const __m256 vpr = _mm256_loadu_ps(pr + 8);
      const __m256 vg1 = _mm256_loadu_ps(g1 + 8);
      acc = _mm256_add_ps(acc, _mm256_mul_ps(vpr, vg1));
    }
    if (config.trash_type_count > 16) {
      const __m256 vpr = _mm256_loadu_ps(pr + 16);
      const __m256 vg1 = _mm256_loadu_ps(g1 + 16);
      acc = _mm256_add_ps(acc, _mm256_mul_ps(vpr, vg1));
    }
    const __m128 low = _mm256_castps256_ps128(acc);
    const __m128 high = _mm256_extractf128_ps(acc, 1);
    __m128 sum128 = _mm_add_ps(low, high);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    base_delta = -_mm_cvtss_f32(sum128);
  }
#else
  for (const int i : iota(0, config.trash_type_count)) {
    base_delta -= p_row_v3[i] * grad_table[k1, i];
  }
#endif

  float delta_upper_bound = base_delta;
#if defined(__AVX2__)
  {
    const float* pr = p_row_v3.data();
    const float* g1 = &grad_table[k1, 0];
    const float* g2 = &grad_table[k2, 0];
    __m256 acc = _mm256_setzero_ps();
    {
      const __m256 vpr = _mm256_loadu_ps(pr + 0);
      const __m256 vg1 = _mm256_loadu_ps(g1 + 0);
      const __m256 vg2 = _mm256_loadu_ps(g2 + 0);
      const __m256 vmax = _mm256_max_ps(vg1, vg2);
      acc = _mm256_add_ps(acc, _mm256_mul_ps(vpr, vmax));
    }
    if (config.trash_type_count > 8) {
      const __m256 vpr = _mm256_loadu_ps(pr + 8);
      const __m256 vg1 = _mm256_loadu_ps(g1 + 8);
      const __m256 vg2 = _mm256_loadu_ps(g2 + 8);
      const __m256 vmax = _mm256_max_ps(vg1, vg2);
      acc = _mm256_add_ps(acc, _mm256_mul_ps(vpr, vmax));
    }
    if (config.trash_type_count > 16) {
      const __m256 vpr = _mm256_loadu_ps(pr + 16);
      const __m256 vg1 = _mm256_loadu_ps(g1 + 16);
      const __m256 vg2 = _mm256_loadu_ps(g2 + 16);
      const __m256 vmax = _mm256_max_ps(vg1, vg2);
      acc = _mm256_add_ps(acc, _mm256_mul_ps(vpr, vmax));
    }
    const __m128 low = _mm256_castps256_ps128(acc);
    const __m128 high = _mm256_extractf128_ps(acc, 1);
    __m128 sum128 = _mm_add_ps(low, high);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    delta_upper_bound += _mm_cvtss_f32(sum128);
  }
#else
  for (const int i : iota(0, config.trash_type_count)) {
    delta_upper_bound +=
        p_row_v3[i] * std::max(grad_table[k1, i], grad_table[k2, i]);
  }
#endif
  if (base_score + delta_upper_bound < min_required_score) return nullopt;

  for (const int sorter_type : iota(0, config.sorter_type_count)) {
    // parity = 0
    {
      float delta = base_delta;
#if defined(__AVX2__)
      {
        const float* pr = p_row_v3.data();
        const float* c1 = &config.sorter_configs[2 * sorter_type, 0];
        const float* c2 = &config.sorter_configs[2 * sorter_type + 1, 0];
        const float* g1 = &grad_table[k1, 0];
        const float* g2 = &grad_table[k2, 0];
        __m256 acc = _mm256_setzero_ps();
        {
          const __m256 vpr = _mm256_loadu_ps(pr + 0);
          const __m256 vc1 = _mm256_loadu_ps(c1 + 0);
          const __m256 vc2 = _mm256_loadu_ps(c2 + 0);
          const __m256 vg1 = _mm256_loadu_ps(g1 + 0);
          const __m256 vg2 = _mm256_loadu_ps(g2 + 0);
          const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc1), vg1);
          const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc2), vg2);
          acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
        }
        if (config.trash_type_count > 8) {
          const __m256 vpr = _mm256_loadu_ps(pr + 8);
          const __m256 vc1 = _mm256_loadu_ps(c1 + 8);
          const __m256 vc2 = _mm256_loadu_ps(c2 + 8);
          const __m256 vg1 = _mm256_loadu_ps(g1 + 8);
          const __m256 vg2 = _mm256_loadu_ps(g2 + 8);
          const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc1), vg1);
          const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc2), vg2);
          acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
        }
        if (config.trash_type_count > 16) {
          const __m256 vpr = _mm256_loadu_ps(pr + 16);
          const __m256 vc1 = _mm256_loadu_ps(c1 + 16);
          const __m256 vc2 = _mm256_loadu_ps(c2 + 16);
          const __m256 vg1 = _mm256_loadu_ps(g1 + 16);
          const __m256 vg2 = _mm256_loadu_ps(g2 + 16);
          const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc1), vg1);
          const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc2), vg2);
          acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
        }
        const __m128 low = _mm256_castps256_ps128(acc);
        const __m128 high = _mm256_extractf128_ps(acc, 1);
        __m128 sum128 = _mm_add_ps(low, high);
        sum128 = _mm_hadd_ps(sum128, sum128);
        sum128 = _mm_hadd_ps(sum128, sum128);
        delta += _mm_cvtss_f32(sum128);
      }
#else
      for (const int i : iota(0, config.trash_type_count)) {
        delta += p_row_v3[i] * config.sorter_configs[2 * sorter_type, i] *
                     grad_table[k1, i] +
                 p_row_v3[i] * config.sorter_configs[2 * sorter_type + 1, i] *
                     grad_table[k2, i];
      }
#endif
      if (delta > max_delta) {
        max_delta = delta;
        max_delta_sorter_type = sorter_type;
        max_delta_parity = 0;
      }
    }
    // parity = 1
    {
      float delta = base_delta;
#if defined(__AVX2__)
      {
        const float* pr = p_row_v3.data();
        const float* c1 = &config.sorter_configs[2 * sorter_type, 0];
        const float* c2 = &config.sorter_configs[2 * sorter_type + 1, 0];
        const float* g1 = &grad_table[k2, 0];
        const float* g2 = &grad_table[k1, 0];
        __m256 acc = _mm256_setzero_ps();
        {
          const __m256 vpr = _mm256_loadu_ps(pr + 0);
          const __m256 vc1 = _mm256_loadu_ps(c1 + 0);
          const __m256 vc2 = _mm256_loadu_ps(c2 + 0);
          const __m256 vg1 = _mm256_loadu_ps(g1 + 0);
          const __m256 vg2 = _mm256_loadu_ps(g2 + 0);
          const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc1), vg1);
          const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc2), vg2);
          acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
        }
        if (config.trash_type_count > 8) {
          const __m256 vpr = _mm256_loadu_ps(pr + 8);
          const __m256 vc1 = _mm256_loadu_ps(c1 + 8);
          const __m256 vc2 = _mm256_loadu_ps(c2 + 8);
          const __m256 vg1 = _mm256_loadu_ps(g1 + 8);
          const __m256 vg2 = _mm256_loadu_ps(g2 + 8);
          const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc1), vg1);
          const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc2), vg2);
          acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
        }
        if (config.trash_type_count > 16) {
          const __m256 vpr = _mm256_loadu_ps(pr + 16);
          const __m256 vc1 = _mm256_loadu_ps(c1 + 16);
          const __m256 vc2 = _mm256_loadu_ps(c2 + 16);
          const __m256 vg1 = _mm256_loadu_ps(g1 + 16);
          const __m256 vg2 = _mm256_loadu_ps(g2 + 16);
          const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc1), vg1);
          const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vpr, vc2), vg2);
          acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
        }
        const __m128 low = _mm256_castps256_ps128(acc);
        const __m128 high = _mm256_extractf128_ps(acc, 1);
        __m128 sum128 = _mm_add_ps(low, high);
        sum128 = _mm_hadd_ps(sum128, sum128);
        sum128 = _mm_hadd_ps(sum128, sum128);
        delta += _mm_cvtss_f32(sum128);
      }
#else
      for (const int i : iota(0, config.trash_type_count)) {
        delta += p_row_v3[i] * config.sorter_configs[2 * sorter_type, i] *
                     grad_table[k2, i] +
                 p_row_v3[i] * config.sorter_configs[2 * sorter_type + 1, i] *
                     grad_table[k1, i];
      }
#endif
      if (delta > max_delta) {
        max_delta = delta;
        max_delta_sorter_type = sorter_type;
        max_delta_parity = 1;
      }
    }
  }

  if (max_delta == 0.0f) return nullopt;

  if (base_score + max_delta < min_required_score) return nullopt;

  const AddTransition chosen_transition(v0, v1, v2, v3, max_delta_sorter_type,
                                        max_delta_parity);
  return std::make_tuple(chosen_transition, base_score + max_delta);
}


#pragma once



uint64 Hash(const uint64 x) {
  // Based on SplitMix64.
  uint64 z = x;
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  z = z ^ (z >> 31);
  return z;
}


#if defined(__AVX2__)
#include <immintrin.h>
#endif  // __AVX2__

struct AddTransitionScore {
  float transition_score = 0.0f;
  // The actual score of the state in [0, 1].
  float p_score = 0.0f;
};

// Common types for scoring priority queue.
using ScoredTransition = std::tuple<int, AddTransition, AddTransitionScore>;
struct MinScoreCompare {
  bool operator()(const ScoredTransition& a, const ScoredTransition& b) const {
    return std::get<2>(a).transition_score > std::get<2>(b).transition_score;
  }
};
using ScoredPQ =
    std::priority_queue<ScoredTransition, std::vector<ScoredTransition>,
                        MinScoreCompare>;


void BuildAddTransitionCandidates(
    const Config& config, const State& state,
    const StateCompanion& state_companion, const float base_score,
    const Matrix<float>& p_table, const Matrix<float>& grad_table,
    const uint64 base_state_hash, const int parent_state_index,
    const int max_scored_to_keep,
    std::unordered_map<uint64, float>& hash_score_map, ScoredPQ& scored_pq) {
  vector<pair<int, float>> scored_v3_candidates;
  for (const int v : iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
    if (state.node_types[v] >= 0) continue;
    scored_v3_candidates.push_back(
        {v, state_companion.node_distance_scores[v]});
  }
  std::sort(scored_v3_candidates.begin(), scored_v3_candidates.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

  const vector<int> active_nodes = state_companion.active_nodes;
  const vector<int> node_id_to_active_index =
      state_companion.node_id_to_active_index;

  bool found = false;
  for (const auto [v3, _] : scored_v3_candidates) {
    if (found) break;
    for (const int v0 : active_nodes) {
      if (config.IsProcessorNode(v0)) continue;
      const int k0 = node_id_to_active_index[v0];
    const Int2D& v0_p = config.node_positions[v0];

    const bool v0_has_parallel_edges = state.node_destination_pairs[v0].first ==
                                       state.node_destination_pairs[v0].second;
      static ArrayMap<int, kMaxNodeCount> is_v0_neighbor;
      is_v0_neighbor.Clear();
      for (const int u : state_companion.active_node_neighbors[k0]) {
        is_v0_neighbor[u] = 1;
      }
      // v0 should be a neighbor of v3.
      if (!is_v0_neighbor[v3]) continue;

      for (const int v1 : {state.node_destination_pairs[v0].first,
                           state.node_destination_pairs[v0].second}) {
        if (v1 < 0) continue;
        const int k1 = node_id_to_active_index[v1];
        const Int2D& v1_p = config.node_positions[v1];
        static ArrayMap<int, kMaxNodeCount> is_v1_neighbor;
        is_v1_neighbor.Clear();
        for (const int u : state_companion.active_node_neighbors[k1]) {
          is_v1_neighbor[u] = 1;
        }
        // v1 should be a neighbor of v3.
        if (!is_v1_neighbor[v3]) continue;

        // v2 should be a processor node or an installed sorter node.
        // The graph should remain acyclic even after adding the extra edge
        // (v0, v2). Additionally, impose the constraint that at least one of
        // (v0, v2) or (v1, v2) can be connected by a segment without
        // intersecting any other segments. This is not a necessary condition,
        // but it is a good heuristic because the probability of rejection is
        // high when (v0, v2) intersects with other edges.
        static vector<int> k2_candidates;
        static ArrayMap<int8, kMaxNodeCount> is_k2_candidate_map;
        k2_candidates.clear();
        is_k2_candidate_map.Clear();
        for (const int v2 : state_companion.active_node_neighbors[k0]) {
          if (config.IsInletNode(v2)) continue;
          // v2 should not be an empty sorter node.
          if (state_companion.node_id_to_active_index[v2] < 0) continue;

          const int k2 = state_companion.node_id_to_active_index[v2];

          // Reject if v0 is reachable from v2 in the directed graph.
          if (state_companion.transitive_closure.Get(k2, k0)) continue;

          k2_candidates.push_back(k2);
          is_k2_candidate_map[k2] = 1;
        }
        for (const int v2 : state_companion.active_node_neighbors[k1]) {
          if (config.IsInletNode(v2)) continue;
          // v2 should not be an empty sorter node.
          if (state_companion.node_id_to_active_index[v2] < 0) continue;

          const int k2 = state_companion.node_id_to_active_index[v2];
          if (is_k2_candidate_map[k2]) continue;

          // Reject if v0 is reachable from v2 in the directed graph.
          if (state_companion.transitive_closure.Get(k2, k0)) continue;

          k2_candidates.push_back(k2);
          is_k2_candidate_map[k2] = 1;
        }

        if (k2_candidates.empty()) continue;

        for (const int k2 : k2_candidates) {
          const int v2 = state_companion.active_nodes[k2];
          const Int2D& v2_p = config.node_positions[v2];

          static ArrayMap<int, kMaxNodeCount> is_v2_neighbor;
          is_v2_neighbor.Clear();
          for (const int u : state_companion.active_node_neighbors[k2]) {
            is_v2_neighbor[u] = 1;
          }

          const Int2D& v3_p = config.node_positions[v3];

          const bool is_v3_valid = [&]() -> bool {
            if (is_v2_neighbor[v3]) return true;

            // If v3 is not a neighbor of v2 and (v3, v2) does not intersect
            // with (v0, v1), then v3 is not a neighbor of v2 even if we remove
            // the edge (v0, v1).
            if (!SegmentsIntersect(v3_p, v2_p, v0_p, v1_p)) return false;

            // When there are two parallel (v0, v1) edges and (v0, v1) and (v3,
            // v2) intersect, (v0, v1) intersects with (v3, v2) after removing
            // the single (v0, v1) edge.
            if (v0_has_parallel_edges) return false;

            if (state_companion.aabb_tree.HasIntersection(
                    Segment(v3_p, v2_p), Segment(v0_p, v1_p))) {
              return false;
            }
            return true;
          }();

          if (!is_v3_valid) continue;

          // Finalize and push immediately.
          // Use the minimum p_score for early rejection with margin. This might
          // not be ideal, but it should be OK.
          const float min_required_p_score = [&]() -> float {
            // Use this when tuning the scoring function.
            // return 0.0f;
            if (int(scored_pq.size()) < max_scored_to_keep) return 0.0f;
            const float kMargin = 0.0f;
            return std::max(0.0f,
                            std::get<2>(scored_pq.top()).p_score - kMargin);
          }();

          const optional<tuple<AddTransition, float>>
              opt_transition_result = FinalizeAddTransition(
                  config, state, state_companion, base_score,
                  min_required_p_score, p_table, grad_table,
                  AddTransition(v0, v1, v2, v3));
          if (!opt_transition_result) continue;

          const auto& [transition, new_score] = *opt_transition_result;

          const float transition_score = new_score;
          const AddTransitionScore transition_score_struct = {transition_score,
                                                              new_score};

          const int nv0 = transition.v0;
          const int nv1 = transition.v1;
          const int nv2 = transition.v2;
          const int nv3 = transition.v3;
          const uint64 new_state_hash =
              base_state_hash ^ Hash((nv0 << 10) | nv1) ^
              Hash((nv0 << 10) | nv3) ^ Hash((nv3 << 10) | nv1) ^
              Hash((nv3 << 10) | nv2);
          if (new_score < hash_score_map[new_state_hash]) continue;
          hash_score_map[new_state_hash] = new_score;

          auto try_push = [&](int base_index, const AddTransition& tr,
                              AddTransitionScore atc) -> void {
            if (static_cast<int>(scored_pq.size()) < max_scored_to_keep) {
              scored_pq.push(std::make_tuple(base_index, tr, atc));
            } else {
              // Compare with the minimum (top) score.
              if (atc.transition_score > std::get<2>(scored_pq.top()).transition_score) {
                scored_pq.pop();
                scored_pq.push(std::make_tuple(base_index, tr, atc));
              }
            }
          };
          try_push(parent_state_index, transition, transition_score_struct);

          found = true;
        }
      }
    }
  }
}





#pragma once

#include <chrono>
#include <iomanip>




#pragma once





// Computes the grad table where grad_table[k, i] represents the amount by which
// the flow to the processor corresponding to type i increases when the flow of
// type i at node v = topological_order[k] is increased by 1.
Matrix<float> ComputeGradTable(const Config& config, const State& state,
const vector<int>& topological_order) {
  const int type_count = config.trash_type_count;
  const int active_node_count = topological_order.size();
  vector<int> node_id_to_active_node_id(config.NodeCount(), -1);
  for (const int k : iota(0, int(topological_order.size()))) {
    node_id_to_active_node_id[topological_order[k]] = k;
  }

  // Make the number of columns a multiple of 8.
  Matrix<float> grad_table(active_node_count, (type_count + 7) / 8 * 8);
  for (const int k0 : iota(0, active_node_count) | std::views::reverse) {
    const int v0 = topological_order[k0];
    if (config.IsProcessorNode(v0)) {
      const int processor_type = state.node_types[v0];
      grad_table[k0, processor_type] = 1.0f;
    } else if (config.IsSorterNode(v0)) {
      const int v1 = state.node_destination_pairs[v0].first;
      const int k1 = node_id_to_active_node_id[v1];
      const int v2 = state.node_destination_pairs[v0].second;
      const int k2 = node_id_to_active_node_id[v2];
      const int sorter_type = state.node_types[v0];
      if (sorter_type < 0) {
        for (const int i : iota(0, type_count)) {
          grad_table[k0, i] += grad_table[k1, i] * 0.5f;
          grad_table[k0, i] += grad_table[k2, i] * 0.5f;
        }
      } else {
        for (const int i : iota(0, type_count)) {
          grad_table[k0, i] +=
              grad_table[k1, i] * config.sorter_configs[2 * sorter_type, i];
          grad_table[k0, i] +=
              grad_table[k2, i] * config.sorter_configs[2 * sorter_type + 1, i];
        }
      }
    }
  }
  return grad_table;
}


#pragma once






#if defined(__AVX2__)
#include <immintrin.h>
#endif  // __AVX2__

namespace scoring_internal {

void PropagateProbabilityFromSorter(
    const Config& config, [[maybe_unused]] const State& state,
    [[maybe_unused]] const vector<int>& topological_order, const int k0,
    const int k1, const int k2, const int sorter_type, Matrix<float>& p_table) {
  const int type_count = config.trash_type_count;
#if defined(__AVX2__)
  const static __m256 kHalfPS = _mm256_set1_ps(0.5f);
  const static __m256 kOnesPS = _mm256_set1_ps(1.0f);
#endif

  if (sorter_type < 0) {
#if defined(__AVX2__)
    {
      float* base0 = &p_table[k0, 0];
      float* base1 = &p_table[k1, 0];
      float* base2 = &p_table[k2, 0];
      const __m256 half = kHalfPS;
      // Always process the first 8 columns.
      {
        const __m256 p0 = _mm256_loadu_ps(base0 + 0);
        const __m256 r0 = _mm256_mul_ps(p0, half);
        _mm256_storeu_ps(base1 + 0,
                         _mm256_add_ps(_mm256_loadu_ps(base1 + 0), r0));
        _mm256_storeu_ps(base2 + 0,
                         _mm256_add_ps(_mm256_loadu_ps(base2 + 0), r0));
      }
      // If there are more than 8 types, process 8..15.
      if (type_count > 8) {
        const __m256 p8 = _mm256_loadu_ps(base0 + 8);
        const __m256 r8 = _mm256_mul_ps(p8, half);
        _mm256_storeu_ps(base1 + 8,
                         _mm256_add_ps(_mm256_loadu_ps(base1 + 8), r8));
        _mm256_storeu_ps(base2 + 8,
                         _mm256_add_ps(_mm256_loadu_ps(base2 + 8), r8));
      }
      // If there are more than 16 types, process 16..23.
      if (type_count > 16) {
        const __m256 p16 = _mm256_loadu_ps(base0 + 16);
        const __m256 r16 = _mm256_mul_ps(p16, half);
        _mm256_storeu_ps(base1 + 16,
                         _mm256_add_ps(_mm256_loadu_ps(base1 + 16), r16));
        _mm256_storeu_ps(base2 + 16,
                         _mm256_add_ps(_mm256_loadu_ps(base2 + 16), r16));
      }
    }
#else
    for (const int i : iota(0, type_count)) {
      p_table[k1, i] += p_table[k0, i] * 0.5f;
      p_table[k2, i] += p_table[k0, i] * 0.5f;
    }
#endif
  } else {
#if defined(__AVX2__)
    {
      float* base0 = &p_table[k0, 0];
      float* base1 = &p_table[k1, 0];
      float* base2 = &p_table[k2, 0];
      const float* sorter1 = &config.sorter_configs[2 * sorter_type, 0];
      const float* sorter2 = &config.sorter_configs[2 * sorter_type + 1, 0];
      // Always process the first 8 columns.
      {
        const __m256 p0 = _mm256_loadu_ps(base0 + 0);
        const __m256 c10 = _mm256_loadu_ps(sorter1 + 0);
        const __m256 c20 = _mm256_loadu_ps(sorter2 + 0);
        const __m256 q10 = _mm256_mul_ps(p0, c10);
        const __m256 q20 = _mm256_mul_ps(p0, c20);
        _mm256_storeu_ps(base1 + 0,
                         _mm256_add_ps(_mm256_loadu_ps(base1 + 0), q10));
        _mm256_storeu_ps(base2 + 0,
                         _mm256_add_ps(_mm256_loadu_ps(base2 + 0), q20));
      }
      // If there are more than 8 types, process 8..15.
      if (type_count > 8) {
        const __m256 p8 = _mm256_loadu_ps(base0 + 8);
        const __m256 c18 = _mm256_loadu_ps(sorter1 + 8);
        const __m256 c28 = _mm256_loadu_ps(sorter2 + 8);
        const __m256 q18 = _mm256_mul_ps(p8, c18);
        const __m256 q28 = _mm256_mul_ps(p8, c28);
        _mm256_storeu_ps(base1 + 8,
                         _mm256_add_ps(_mm256_loadu_ps(base1 + 8), q18));
        _mm256_storeu_ps(base2 + 8,
                         _mm256_add_ps(_mm256_loadu_ps(base2 + 8), q28));
      }
      // If there are more than 16 types, process 16..23.
      if (type_count > 16) {
        const __m256 p16 = _mm256_loadu_ps(base0 + 16);
        const __m256 c116 = _mm256_loadu_ps(sorter1 + 16);
        const __m256 c216 = _mm256_loadu_ps(sorter2 + 16);
        const __m256 q116 = _mm256_mul_ps(p16, c116);
        const __m256 q216 = _mm256_mul_ps(p16, c216);
        _mm256_storeu_ps(base1 + 16,
                         _mm256_add_ps(_mm256_loadu_ps(base1 + 16), q116));
        _mm256_storeu_ps(base2 + 16,
                         _mm256_add_ps(_mm256_loadu_ps(base2 + 16), q216));
      }
    }
#else
    for (const int i : iota(0, type_count)) {
      p_table[k1, i] +=
          p_table[k0, i] * config.sorter_configs[2 * sorter_type, i];
      p_table[k2, i] +=
          p_table[k0, i] * config.sorter_configs[2 * sorter_type + 1, i];
    }
#endif
  }
}

}  // namespace scoring_internal

Matrix<float> ComputePTable(const Config& config, const State& state,
                            const vector<int>& acvive_nodes,
                            const vector<int>& node_id_to_active_index) {
  const int active_node_count = acvive_nodes.size();

  // Make the number of columns a multiple of 8.
  Matrix<float> p_table(active_node_count,
                        ((config.trash_type_count + 7) / 8) * 8);
  for (const int i : iota(0, config.trash_type_count)) {
    p_table[0, i] = 1.0f / config.trash_type_count;
  }

  for (const int v0 : acvive_nodes) {
    const int k0 = node_id_to_active_index[v0];
    if (config.IsSorterNode(v0)) {
      const int v1 = state.node_destination_pairs[v0].first;
      const int k1 = node_id_to_active_index[v1];
      const int v2 = state.node_destination_pairs[v0].second;
      const int k2 = node_id_to_active_index[v2];
      const int sorter_type = state.node_types[v0];
      scoring_internal::PropagateProbabilityFromSorter(
          config, state, acvive_nodes, k0, k1, k2, sorter_type, p_table);
    } else if (config.IsInletNode(v0)) {
      const int v1 = state.node_destination_pairs[v0].first;
      const int k1 = node_id_to_active_index[v1];
      if (v1 >= 0) {
        #if defined(__AVX2__)
        {
          float* base0 = &p_table[k0, 0];
          float* base1 = &p_table[k1, 0];
          // Always process the first 8 columns.
          {
            const __m256 p0 = _mm256_loadu_ps(base0 + 0);
            _mm256_storeu_ps(base1 + 0,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 0), p0));
          }
          // If there are more than 8 types, process 8..15.
          if (config.trash_type_count > 8) {
            const __m256 p8 = _mm256_loadu_ps(base0 + 8);
            _mm256_storeu_ps(base1 + 8,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 8), p8));
          }
          // If there are more than 16 types, process 16..23.
          if (config.trash_type_count > 16) {
            const __m256 p16 = _mm256_loadu_ps(base0 + 16);
            _mm256_storeu_ps(base1 + 16,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 16), p16));
          }
        }
        #else
        for (const int i : iota(0, config.trash_type_count)) {
          p_table[k1, i] += p_table[k0, i];
        }
        #endif
      }
    }
  }
  return p_table;
}

// NOTE: Generated by Cursor Agent.
// TODO: Revisit the code for efficiency.
float Score(const Config& config, const State& state,
            const vector<int>& active_nodes) {
  const int node_count = config.NodeCount();

  vector<int> node_id_to_active_node_id(node_count, -1);
  for (const int k : iota(0, int(active_nodes.size()))) {
    node_id_to_active_node_id[active_nodes[k]] = k;
  }

  const Matrix<float> p_table = ComputePTable(config, state, active_nodes,
                                              node_id_to_active_node_id);

  float score = 0.0;
  for (const int processor_id : iota(0, config.trash_type_count)) {
    const int v = config.ProcessorNodeId(processor_id);
    const int k = node_id_to_active_node_id[v];
    if (k < 0) continue;
    const int trash_type = state.node_types[processor_id];
    score += p_table[k, trash_type];
  }
  return score;
}

float Score(const Config& config, const State& state) {
  const optional<vector<int>> opt_topological_order =
      GetTopologicalOrder(config, state);
  if (!opt_topological_order) {
    UNEXPECTED();
  }
  return Score(config, state, opt_topological_order.value());
}

int ConvertToFinalScore(const float score) {
  return static_cast<int>(std::round((1.0 - score) * 1.0E9));
}



optional<ChangeTransition> BuildChangeTransitionCandidate(
    const Config& config, const State& state,
    const StateCompanion& state_companion) {
  Matrix<float> p_table =
      ComputePTable(config, state, state_companion.active_nodes,
                    state_companion.node_id_to_active_index);
  Matrix<float> grad_table =
      ComputeGradTable(config, state, state_companion.active_nodes);

  float max_tranisition_score = 0.0f;
  optional<ChangeTransition> opt_max_tranisition_change_transition;

  const vector<int>& active_nodes = state_companion.active_nodes;

  vector<int> degrees(config.NodeCount(), 0);
  for (const int v0 : active_nodes) {
    if (state.node_destination_pairs[v0].first >= 0) {
      degrees[state.node_destination_pairs[v0].first]++;
    }
    if (state.node_destination_pairs[v0].second >= 0) {
      degrees[state.node_destination_pairs[v0].second]++;
    }
  }

  for (const int v0 : active_nodes) {
    // TODO: Handle inlet nodes.
    if (config.IsInletNode(v0)) continue;

    const int k0 = state_companion.node_id_to_active_index[v0];
    for (const int edge_index : {0, 1}) {
      const int v1 = [&]() -> int {
        if (edge_index == 0) {
          return state.node_destination_pairs[v0].first;
        } else if (edge_index == 1) {
          return state.node_destination_pairs[v0].second;
        } else {
          UNEXPECTED();
        }
      }();
      if (v1 < 0) continue;
      if (degrees[v1] == 0) UNEXPECTED();
      if (degrees[v1] == 1) continue;

      const int k1 = state_companion.node_id_to_active_index[v1];
      for (const int v2 : state_companion.active_node_neighbors[k0]) {
        // Skip non-active nodes.
        if (state.node_types[v2] < 0) continue;

        const int k2 = state_companion.node_id_to_active_index[v2];

        // If v0 is reachable from v2, we cannot add (v0, v2) to keep the graph
        // acyclic.
        if (state_companion.transitive_closure.Get(k2, k0)) continue;

        const int sorter_type = state.node_types[v0];

        float delta = 0.0;
        for (const int i : iota(0, config.trash_type_count)) {
          delta += p_table[k0, i] *
                   config.sorter_configs[2 * sorter_type + edge_index, i] *
                   (grad_table[k2, i] - grad_table[k1, i]);
        }

        if (delta <= 0.0f) continue;

        const Int2D& v0_p = config.node_positions[v0];
        const Int2D& v1_p = config.node_positions[v1];
        const Int2D& v2_p = config.node_positions[v2];

        const double core_score = delta;

        const double progress =
            1.0 * state_companion.active_nodes.size() / config.NodeCount();

        const double length_01 = v0_p.Distance(v1_p);
        const double length_02 = v0_p.Distance(v2_p);
        // [-0.5, 0.5]
        const double length_score =
            Sigmoid(length_01 - length_02, 0.0, 1.0E-3) - 0.5;
        const double length_score_weight =
            ExpInterpolation(0.0, 1E-7, 1.0, 1E-8, progress);
        const double transition_score =
            core_score + length_score_weight * length_score;
        if (transition_score > max_tranisition_score) {
          max_tranisition_score = transition_score;
          opt_max_tranisition_change_transition = ChangeTransition(v0, v1, v2);
        }
      }
    }
  }

  return opt_max_tranisition_change_transition;
}






#pragma once




#if defined(__AVX2__)
#include <immintrin.h>
#endif  // __AVX2__

// Optimizes the sorter types of all sorter nodes.
void OptimizeSorterTypes(const Config& config,
                         const vector<int>& topological_order,
                         const vector<int>& node_id_to_active_node_id,
                         State& state) {
  const int type_count = config.trash_type_count;
  const Matrix<float> grad_table =
      ComputeGradTable(config, state, topological_order);
  Matrix<float> p_table(topological_order.size(), ((type_count + 7) / 8) * 8);
  for (const int i : iota(0, type_count)) {
    p_table[0, i] = 1.0f;
  }

  for (const int v0 : topological_order) {
    const int k0 = node_id_to_active_node_id[v0];
    if (config.IsSorterNode(v0)) {
      const int v1 = state.node_destination_pairs[v0].first;
      const int k1 = node_id_to_active_node_id[v1];
      const int v2 = state.node_destination_pairs[v0].second;
      const int k2 = node_id_to_active_node_id[v2];

      // Choose the sorter type and the swap parity.
      double max_score = -1.0;
      int max_score_sorter_type = -1;
      int max_sorter_swap = -1;
      for (const int sorter_type : iota(0, config.sorter_type_count)) {
        for (const int swap : {0, 1}) {
          if (swap == 0) {
            double score = 0.0;
#if defined(__AVX2__)
            {
              const float* p0 = &p_table[k0, 0];
              const float* c1 = &config.sorter_configs[2 * sorter_type, 0];
              const float* c2 = &config.sorter_configs[2 * sorter_type + 1, 0];
              const float* g1 = &grad_table[k1, 0];
              const float* g2 = &grad_table[k2, 0];
              __m256 acc = _mm256_setzero_ps();
              {
                const __m256 vp = _mm256_loadu_ps(p0 + 0);
                const __m256 vc1 = _mm256_loadu_ps(c1 + 0);
                const __m256 vc2 = _mm256_loadu_ps(c2 + 0);
                const __m256 vg1 = _mm256_loadu_ps(g1 + 0);
                const __m256 vg2 = _mm256_loadu_ps(g2 + 0);
                const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vp, vc1), vg1);
                const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vp, vc2), vg2);
                acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
              }
              if (type_count > 8) {
                const __m256 vp = _mm256_loadu_ps(p0 + 8);
                const __m256 vc1 = _mm256_loadu_ps(c1 + 8);
                const __m256 vc2 = _mm256_loadu_ps(c2 + 8);
                const __m256 vg1 = _mm256_loadu_ps(g1 + 8);
                const __m256 vg2 = _mm256_loadu_ps(g2 + 8);
                const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vp, vc1), vg1);
                const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vp, vc2), vg2);
                acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
              }
              if (type_count > 16) {
                const __m256 vp = _mm256_loadu_ps(p0 + 16);
                const __m256 vc1 = _mm256_loadu_ps(c1 + 16);
                const __m256 vc2 = _mm256_loadu_ps(c2 + 16);
                const __m256 vg1 = _mm256_loadu_ps(g1 + 16);
                const __m256 vg2 = _mm256_loadu_ps(g2 + 16);
                const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vp, vc1), vg1);
                const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vp, vc2), vg2);
                acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
              }
              const __m128 low = _mm256_castps256_ps128(acc);
              const __m128 high = _mm256_extractf128_ps(acc, 1);
              __m128 sum128 = _mm_add_ps(low, high);
              sum128 = _mm_hadd_ps(sum128, sum128);
              sum128 = _mm_hadd_ps(sum128, sum128);
              score = static_cast<double>(_mm_cvtss_f32(sum128));
            }
#else
            for (const int i : iota(0, type_count)) {
              score += p_table[k0, i] * config.sorter_configs[2 * sorter_type, i] *
                       grad_table[k1, i];
              score += p_table[k0, i] *
                       config.sorter_configs[2 * sorter_type + 1, i] *
                       grad_table[k2, i];
            }
#endif
            if (score > max_score) {
              max_score = score;
              max_score_sorter_type = sorter_type;
              max_sorter_swap = swap;
            }
          } else {
            double score = 0.0;
#if defined(__AVX2__)
            {
              const float* p0 = &p_table[k0, 0];
              const float* c1 = &config.sorter_configs[2 * sorter_type, 0];
              const float* c2 = &config.sorter_configs[2 * sorter_type + 1, 0];
              const float* g1 = &grad_table[k2, 0];
              const float* g2 = &grad_table[k1, 0];
              __m256 acc = _mm256_setzero_ps();
              {
                const __m256 vp = _mm256_loadu_ps(p0 + 0);
                const __m256 vc1 = _mm256_loadu_ps(c1 + 0);
                const __m256 vc2 = _mm256_loadu_ps(c2 + 0);
                const __m256 vg1 = _mm256_loadu_ps(g1 + 0);
                const __m256 vg2 = _mm256_loadu_ps(g2 + 0);
                const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vp, vc1), vg1);
                const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vp, vc2), vg2);
                acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
              }
              if (type_count > 8) {
                const __m256 vp = _mm256_loadu_ps(p0 + 8);
                const __m256 vc1 = _mm256_loadu_ps(c1 + 8);
                const __m256 vc2 = _mm256_loadu_ps(c2 + 8);
                const __m256 vg1 = _mm256_loadu_ps(g1 + 8);
                const __m256 vg2 = _mm256_loadu_ps(g2 + 8);
                const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vp, vc1), vg1);
                const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vp, vc2), vg2);
                acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
              }
              if (type_count > 16) {
                const __m256 vp = _mm256_loadu_ps(p0 + 16);
                const __m256 vc1 = _mm256_loadu_ps(c1 + 16);
                const __m256 vc2 = _mm256_loadu_ps(c2 + 16);
                const __m256 vg1 = _mm256_loadu_ps(g1 + 16);
                const __m256 vg2 = _mm256_loadu_ps(g2 + 16);
                const __m256 t1 = _mm256_mul_ps(_mm256_mul_ps(vp, vc1), vg1);
                const __m256 t2 = _mm256_mul_ps(_mm256_mul_ps(vp, vc2), vg2);
                acc = _mm256_add_ps(acc, _mm256_add_ps(t1, t2));
              }
              const __m128 low = _mm256_castps256_ps128(acc);
              const __m128 high = _mm256_extractf128_ps(acc, 1);
              __m128 sum128 = _mm_add_ps(low, high);
              sum128 = _mm_hadd_ps(sum128, sum128);
              sum128 = _mm_hadd_ps(sum128, sum128);
              score = static_cast<double>(_mm_cvtss_f32(sum128));
            }
#else
            for (const int i : iota(0, type_count)) {
              score += p_table[k0, i] * config.sorter_configs[2 * sorter_type, i] *
                       grad_table[k2, i];
              score += p_table[k0, i] *
                       config.sorter_configs[2 * sorter_type + 1, i] *
                       grad_table[k1, i];
            }
#endif
            if (score > max_score) {
              max_score = score;
              max_score_sorter_type = sorter_type;
              max_sorter_swap = swap;
            }
          }
        }
      }
      if (max_score < 0.0) {
        UNEXPECTED();
      }

      // Update the state
      const int sorter_type = max_score_sorter_type;
      const int swap = max_sorter_swap;
      state.node_types[v0] = sorter_type;
      if (swap == 1) {
        state.node_destination_pairs[v0] = {v2, v1};
      }

      // Update p_table of children nodes.
      if (swap == 0) {
#if defined(__AVX2__)
        {
          const float* base0 = &p_table[k0, 0];
          float* base1 = &p_table[k1, 0];
          float* base2 = &p_table[k2, 0];
          const float* sorter1 = &config.sorter_configs[2 * sorter_type, 0];
          const float* sorter2 = &config.sorter_configs[2 * sorter_type + 1, 0];
          {
            const __m256 p0 = _mm256_loadu_ps(base0 + 0);
            const __m256 c10 = _mm256_loadu_ps(sorter1 + 0);
            const __m256 c20 = _mm256_loadu_ps(sorter2 + 0);
            const __m256 q10 = _mm256_mul_ps(p0, c10);
            const __m256 q20 = _mm256_mul_ps(p0, c20);
            _mm256_storeu_ps(base1 + 0,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 0), q10));
            _mm256_storeu_ps(base2 + 0,
                             _mm256_add_ps(_mm256_loadu_ps(base2 + 0), q20));
          }
          if (type_count > 8) {
            const __m256 p8 = _mm256_loadu_ps(base0 + 8);
            const __m256 c18 = _mm256_loadu_ps(sorter1 + 8);
            const __m256 c28 = _mm256_loadu_ps(sorter2 + 8);
            const __m256 q18 = _mm256_mul_ps(p8, c18);
            const __m256 q28 = _mm256_mul_ps(p8, c28);
            _mm256_storeu_ps(base1 + 8,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 8), q18));
            _mm256_storeu_ps(base2 + 8,
                             _mm256_add_ps(_mm256_loadu_ps(base2 + 8), q28));
          }
          if (type_count > 16) {
            const __m256 p16 = _mm256_loadu_ps(base0 + 16);
            const __m256 c116 = _mm256_loadu_ps(sorter1 + 16);
            const __m256 c216 = _mm256_loadu_ps(sorter2 + 16);
            const __m256 q116 = _mm256_mul_ps(p16, c116);
            const __m256 q216 = _mm256_mul_ps(p16, c216);
            _mm256_storeu_ps(base1 + 16,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 16), q116));
            _mm256_storeu_ps(base2 + 16,
                             _mm256_add_ps(_mm256_loadu_ps(base2 + 16), q216));
          }
        }
#else
        for (const int i : iota(0, type_count)) {
          p_table[k1, i] +=
              p_table[k0, i] * config.sorter_configs[2 * sorter_type, i];
          p_table[k2, i] +=
              p_table[k0, i] * config.sorter_configs[2 * sorter_type + 1, i];
        }
#endif
      } else if (swap == 1) {
#if defined(__AVX2__)
        {
          const float* base0 = &p_table[k0, 0];
          float* base2 = &p_table[k2, 0];
          float* base1 = &p_table[k1, 0];
          const float* sorter1 = &config.sorter_configs[2 * sorter_type, 0];
          const float* sorter2 = &config.sorter_configs[2 * sorter_type + 1, 0];
          {
            const __m256 p0 = _mm256_loadu_ps(base0 + 0);
            const __m256 c10 = _mm256_loadu_ps(sorter1 + 0);
            const __m256 c20 = _mm256_loadu_ps(sorter2 + 0);
            const __m256 q10 = _mm256_mul_ps(p0, c10);
            const __m256 q20 = _mm256_mul_ps(p0, c20);
            _mm256_storeu_ps(base2 + 0,
                             _mm256_add_ps(_mm256_loadu_ps(base2 + 0), q10));
            _mm256_storeu_ps(base1 + 0,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 0), q20));
          }
          if (type_count > 8) {
            const __m256 p8 = _mm256_loadu_ps(base0 + 8);
            const __m256 c18 = _mm256_loadu_ps(sorter1 + 8);
            const __m256 c28 = _mm256_loadu_ps(sorter2 + 8);
            const __m256 q18 = _mm256_mul_ps(p8, c18);
            const __m256 q28 = _mm256_mul_ps(p8, c28);
            _mm256_storeu_ps(base2 + 8,
                             _mm256_add_ps(_mm256_loadu_ps(base2 + 8), q18));
            _mm256_storeu_ps(base1 + 8,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 8), q28));
          }
          if (type_count > 16) {
            const __m256 p16 = _mm256_loadu_ps(base0 + 16);
            const __m256 c116 = _mm256_loadu_ps(sorter1 + 16);
            const __m256 c216 = _mm256_loadu_ps(sorter2 + 16);
            const __m256 q116 = _mm256_mul_ps(p16, c116);
            const __m256 q216 = _mm256_mul_ps(p16, c216);
            _mm256_storeu_ps(base2 + 16,
                             _mm256_add_ps(_mm256_loadu_ps(base2 + 16), q116));
            _mm256_storeu_ps(base1 + 16,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 16), q216));
          }
        }
#else
        for (const int i : iota(0, type_count)) {
          p_table[k2, i] +=
              p_table[k0, i] * config.sorter_configs[2 * sorter_type, i];
          p_table[k1, i] +=
              p_table[k0, i] * config.sorter_configs[2 * sorter_type + 1, i];
        }
#endif
      } else {
        UNEXPECTED();
      }
    } else if (config.IsInletNode(v0)) {
      const int v1 = state.node_destination_pairs[v0].first;
      if (v1 >= 0) {
        const int k1 = node_id_to_active_node_id[v1];
        #if defined(__AVX2__)
        {
          float* base0 = &p_table[k0, 0];
          float* base1 = &p_table[k1, 0];
          {
            const __m256 p0 = _mm256_loadu_ps(base0 + 0);
            _mm256_storeu_ps(base1 + 0,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 0), p0));
          }
          if (type_count > 8) {
            const __m256 p8 = _mm256_loadu_ps(base0 + 8);
            _mm256_storeu_ps(base1 + 8,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 8), p8));
          }
          if (type_count > 16) {
            const __m256 p16 = _mm256_loadu_ps(base0 + 16);
            _mm256_storeu_ps(base1 + 16,
                             _mm256_add_ps(_mm256_loadu_ps(base1 + 16), p16));
          }
        }
        #else
        for (const int i : iota(0, type_count)) {
          p_table[k1, i] += p_table[k0, i];
        }
        #endif
      }
    }
  }
}



namespace beam_search {

struct BeamSearchEntry {
  State state;
  float score = 0.0f;
  optional<StateCompanion> opt_state_companion;

  // These are used to do the delayed computation of state_companion.
  optional<StateCompanion> opt_parent_state_companion;
  optional<AddTransition> opt_parent_transition;
};

vector<BeamSearchEntry> RunSingleStep(
    const Config& config, [[maybe_unused]] const int step,
    const int step_time_limit_us, vector<BeamSearchEntry>& parent_entries,
    pair<State, float>& max_score_state_pair) {
  const int step_begin_time_us = g_time.CurrentUs();
  // Tuple of (index of base state, transition, score) is defined in
  // add_transition_candidate_genearation.cpp as ScoredTransition.
  const int max_scored_to_keep =
      (step == 0) ? 65536
                  : 2 * static_cast<int>(parent_entries.size());
  ScoredPQ scored_pq;
  int completed = 0;

  auto pre_process_entry = [&](BeamSearchEntry& entry) -> void {
    // Do the delayed computation of state_companion and local optimization of
    // the parent state at this point.

    if (!entry.opt_state_companion) {
      if (!entry.opt_parent_state_companion ||
          !entry.opt_parent_transition) {
        UNEXPECTED();
      }
      const StateCompanion& parent_state_companion =
          *entry.opt_parent_state_companion;
      const AddTransition& parent_transition =
          *entry.opt_parent_transition;
      const int companion_time_us_begin = g_time.CurrentUs();
      entry.opt_state_companion = MakeStateCompanionByAddTransition(
          config, entry.state, parent_state_companion, parent_transition);
      const int companion_time_us_end = g_time.CurrentUs();
      companion_time_us += companion_time_us_end - companion_time_us_begin;
    }

    if (entry.score > max_score_state_pair.second) {
      max_score_state_pair = {entry.state, entry.score};
    }

    constexpr bool kEnableChangeTransition = true;
    const int change_transition_time_us_begin = g_time.CurrentUs();
    const optional<ChangeTransition> opt_change_transition =
        kEnableChangeTransition
            ? BuildChangeTransitionCandidate(
                  config, entry.state, *entry.opt_state_companion)
            : nullopt;
    const int change_transition_time_us_end = g_time.CurrentUs();
    change_transition_time_us += change_transition_time_us_end - change_transition_time_us_begin;
    if (opt_change_transition) {
      const int change_transition_companion_time_us_begin = g_time.CurrentUs();
      const int v0 = opt_change_transition->v0;
      const int v1 = opt_change_transition->v1;
      const int v2 = opt_change_transition->v2;
      
      // State new_state = entry.state;
      if (entry.state.node_destination_pairs[v0].first == v1) {
        entry.state.node_destination_pairs[v0].first = v2;
      } else if (entry.state.node_destination_pairs[v0].second == v1) {
        entry.state.node_destination_pairs[v0].second = v2;
      } else {
        UNEXPECTED();
      }
      const float new_score = Score(config, entry.state);

      entry.score = new_score;
      entry.opt_state_companion = MakeStateCompanionByChangeTransition(
          config, entry.state, *entry.opt_state_companion,
          *opt_change_transition);
      if (entry.score > max_score_state_pair.second) {
        max_score_state_pair = {entry.state, entry.score};
      }
      const int change_transition_companion_time_us_end = g_time.CurrentUs();
      change_transition_companion_time_us += change_transition_companion_time_us_end - change_transition_companion_time_us_begin;
    }

    const int sorter_types_optimization_time_us_begin = g_time.CurrentUs();
    OptimizeSorterTypes(config, entry.opt_state_companion->active_nodes,
                        entry.opt_state_companion->node_id_to_active_index,
                        entry.state);
    const int sorter_types_optimization_time_us_end = g_time.CurrentUs();
    sorter_types_optimization_time_us += sorter_types_optimization_time_us_end - sorter_types_optimization_time_us_begin;
    entry.score = Score(config, entry.state);
    if (entry.score > max_score_state_pair.second) {
      max_score_state_pair = {entry.state, entry.score};
    }
  };

  static std::unordered_map<uint64, float> hash_score_map;
  hash_score_map.clear();

  for (const int parent_state_index : iota(0, int(parent_entries.size()))) {
    // Do pre-processing of the parent state. This includes:
    // - Do delayed local optimization of the state.
    //   - Optimize the sorter types.
    // - Compute the state companion from the parent state companion.
    const int pre_process_entry_time_us_begin = g_time.CurrentUs();
    pre_process_entry(parent_entries[parent_state_index]);
    const int pre_process_entry_time_us_end = g_time.CurrentUs();
    pre_process_entry_time_us += pre_process_entry_time_us_end - pre_process_entry_time_us_begin;

    const BeamSearchEntry& parent_entry = parent_entries[parent_state_index];
    const State& base_state = parent_entry.state;
    if (!parent_entry.opt_state_companion) {
      UNEXPECTED();
    }
    const StateCompanion& base_state_companion =
        *parent_entry.opt_state_companion;
    const float base_state_score = parent_entry.score;

    const uint64 base_state_hash = [&]() -> uint64 {
      uint64 hash = 0;
      for (const int v : iota(0, config.NodeCount())) {
        if (base_state.node_destination_pairs[v].first >= 0) {
          const int v0 = base_state.node_destination_pairs[v].first;
          hash ^= Hash((v0 << 10) | v);
          if (base_state.node_destination_pairs[v].second >= 0) {
            const int v1 = base_state.node_destination_pairs[v].second;
            hash ^= Hash((v1 << 10) | v);
          }
        }
      }
      return hash;
    }();

    const Matrix<float> p_table =
        ComputePTable(config, base_state, base_state_companion.active_nodes,
                      base_state_companion.node_id_to_active_index);
    const Matrix<float> grad_table =
        ComputeGradTable(config, base_state, base_state_companion.active_nodes);

    const int candidate_generation_time_us_begin = g_time.CurrentUs();
    BuildAddTransitionCandidates(
        config, base_state, base_state_companion, base_state_score, p_table,
        grad_table, base_state_hash, parent_state_index, max_scored_to_keep,
        hash_score_map, scored_pq);
    const int candidate_generation_time_us_end = g_time.CurrentUs();
    candidate_generation_time_us += candidate_generation_time_us_end - candidate_generation_time_us_begin;

    completed++;

    if (!kEnableDeterministicMode) {
      const int time_us = g_time.CurrentUs();
      const bool has_scored = !scored_pq.empty();
      if (has_scored && time_us >= step_time_limit_us) {
        break;
      }
    }
  }

  // Materialize elements into the vector from the heap (keeps top-M if limited).
  vector<ScoredTransition> scored_transitions;
  scored_transitions.reserve(scored_pq.size());
  while (!scored_pq.empty()) {
    scored_transitions.push_back(scored_pq.top());
    scored_pq.pop();
  }

  if (scored_transitions.empty()) return {};

  // Make scored_transitions sorted by score in descending order.
  std::reverse(scored_transitions.begin(), scored_transitions.end());

  vector<BeamSearchEntry> results;
  const int top_k = [&]() -> int {
    if (kEnableDeterministicMode) return 1;

    if (completed == int(parent_entries.size())) {
      const int time_consumed_us = g_time.CurrentUs() - step_begin_time_us;
      const int time_per_state = time_consumed_us / completed;
      const int target_state_count =
          (step_time_limit_us - step_begin_time_us) / time_per_state;
      return std::max(target_state_count, 1);
    }
    return std::max(completed, 1);
  }();

  if (kEnableDebug) {
    std::cerr << "width: " << top_k << "\n";
  }
  static std::unordered_set<uint64> state_hash_set;
  state_hash_set.clear();
  for (const int i : iota(0, int(scored_transitions.size()))) {
    // Decide a hash based on edges.
    auto make_state_hash = [&](const State& state) -> uint64 {
      uint64 hash = 0;
      for (const int v : iota(0, config.NodeCount())) {
        if (state.node_destination_pairs[v].first >= 0) {
          const int v0 = state.node_destination_pairs[v].first;
          hash ^= Hash((v0 << 10) | v);
          if (state.node_destination_pairs[v].second >= 0) {
            const int v1 = state.node_destination_pairs[v].second;
            hash ^= Hash((v1 << 10) | v);
          }
        }
      }
      return hash;
    };

    const auto [base_state_index, transition, score] = scored_transitions[i];
    const BeamSearchEntry& parent_entry = parent_entries[base_state_index];
    const State& base_state = parent_entry.state;
    const StateCompanion& base_state_companion =
        *parent_entry.opt_state_companion;
    State new_state = base_state;
    ApplyTransition(transition, new_state);
    const uint64 state_hash = make_state_hash(new_state);
    if (state_hash_set.contains(state_hash)) continue;
    state_hash_set.insert(state_hash);

    BeamSearchEntry new_entry;
    new_entry.state = new_state;
    new_entry.score = score.p_score;
    new_entry.opt_parent_state_companion = base_state_companion;
    new_entry.opt_parent_transition = transition;
    results.push_back(new_entry);

    if (int(results.size()) >= top_k) break;
  }

  return results;
}

State RunBeamSearchInternal(const Config& config, const int time_limit_ms,
                            const State& initial_state) {
  const int time_limit_us = time_limit_ms * 1000;

  StateCompanion state_companion = MakeStateCompanion(config, initial_state);
  const float inital_state_score = Score(config, initial_state);

  pair<State, float> max_score_state_pair = {initial_state, inital_state_score};

  BeamSearchEntry initial_entry;
  initial_entry.state = initial_state;
  initial_entry.score = inital_state_score;
  initial_entry.opt_state_companion = state_companion;
  vector<BeamSearchEntry> entries = {initial_entry};

  const int node_count = config.NodeCount();
  const int step_count = node_count - state_companion.active_nodes.size() + 1;

  for (const int step : iota(0, step_count)) {
    const int current_time_us = g_time.CurrentUs();
    const int step_max_time_us =
        current_time_us +
        (time_limit_us - current_time_us) / (step_count - step);

    entries = RunSingleStep(config, step, step_max_time_us, entries,
                            max_score_state_pair);

    if (entries.empty()) break;
    if (!kEnableDeterministicMode && g_time.CurrentUs() >= time_limit_us) break;
  }
  std::cerr << "v3_time_us: " << v3_time_us << "\n";
  std::cerr << "scorer_type_time_us: " << scorer_type_time_us << "\n";
  std::cerr << "candidate_generation_time_us: " << candidate_generation_time_us << "\n";
  std::cerr << "pre_process_entry_time_us: " << pre_process_entry_time_us << "\n";
  std::cerr << "companion_time_us: " << companion_time_us << "\n";
  std::cerr << "change_transition_time_us: " << change_transition_time_us << "\n";
  std::cerr << "change_transition_companion_time_us: " << change_transition_companion_time_us << "\n";
  std::cerr << "sorter_types_optimization_time_us: " << sorter_types_optimization_time_us << "\n";
  std::cerr << "aabb_tree_time_us: " << aabb_tree_time_us << "\n";
  std::cerr << "node_neighbor_time_us: " << node_neighbor_time_us << "\n";
  std::cerr << "compainon_others_time_us: " << compainon_others_time_us << "\n";
  std::cerr << "companion_copy_time_us: " << companion_copy_time_us << "\n";
  return max_score_state_pair.first;
}

}  // namespace beam_search

State RunBeamSearch(const Config& config, const int time_limit_ms,
                    const State& state) {
  return beam_search::RunBeamSearchInternal(config, time_limit_ms, state);
}

#pragma once








namespace initial_solution {

// Make an initial tree graph. This function does not assign sorter types (it
// temporarily assigns them to 0).
State MakeInitialTree(const Config& config) {
  State state;
  state.node_types = vector<int>(
      config.trash_type_count + config.sorter_position_count + 1, -1);
  state.node_destination_pairs = vector<pair<int, int>>(
      config.trash_type_count + config.sorter_position_count + 1, {-1, -1});

  for (const int i : iota(0, config.trash_type_count)) {
    state.node_types[i] = i;
  }

  vector<int> active_nodes;
  active_nodes.push_back(config.InletNodeId());
  for (const int i : iota(0, config.trash_type_count)) {
    active_nodes.push_back(config.ProcessorNodeId(i));
  }

  // node_distance_scores[v] is the minimum distance between v and any active
  // node.
  vector<float> node_distance_scores(config.NodeCount(), 0.0f);
  for (const int v : iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
    int min_distance2 = INT_MAX;
    for (const int u : active_nodes) {
      const int distance2 =
          config.node_positions[v].Distance2(config.node_positions[u]);
      if (distance2 < min_distance2) {
        min_distance2 = distance2;
      }
    }
    node_distance_scores[v] = std::sqrt(float(min_distance2));
  }

  vector<pair<int, int>> edges;

  vector<int> processor_used(config.trash_type_count, 0);
  {
    // Add (v0, v3), (v3, v1), (v3, v2) where v0 is the inlet node,
    // v1 and v2 are processor nodes, and v3 is a sorter node.
    // Choose (v1, v2, v3) to maximize transition_score consistent with later logic.

    const int v0 = config.InletNodeId();
    const Int2D v0_p = config.node_positions[v0];

    const float kEps = 1.0e-9f;
    const float kLengthDeltaWeight = 0.1f;
    const float kBackwardXPenaltyWeight = 0.0f;  // can be tuned

    auto backward_x_amount = [&](const Int2D &pa, const Int2D &pb) -> float {
      const int dx = pb.x - pa.x;
      return dx < 0 ? float(-dx) : 0.0f;
    };

    float best_transition_score = -1.0e18f;
    float best_total_length = 1.0e18f;  // tie-breaker
    float best_backward = 1.0e18f;      // secondary tie-breaker
    int best_v1 = -1;
    int best_v2 = -1;
    int best_v3 = -1;

    for (const int v3 : iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
      if (state.node_types[v3] != -1) continue;  // sorter already used (should not happen here)
      const float node_score = node_distance_scores[v3];
      const Int2D v3_p = config.node_positions[v3];

      for (const int v1 : iota(config.ProcessorNodeBegin(), config.ProcessorNodeEnd())) {
        if (processor_used[v1 - config.ProcessorNodeBegin()]) continue;
        const Int2D v1_p = config.node_positions[v1];
        for (const int v2 : iota(v1 + 1, config.ProcessorNodeEnd())) {
          if (processor_used[v2 - config.ProcessorNodeBegin()]) continue;
          const Int2D v2_p = config.node_positions[v2];

          // No existing edge to remove here. Use absolute sums.
          const float total_len = v0_p.Distance(v3_p) + v3_p.Distance(v1_p) + v3_p.Distance(v2_p);
          const float new_backward = backward_x_amount(v0_p, v3_p) +
                                     backward_x_amount(v3_p, v1_p) +
                                     backward_x_amount(v3_p, v2_p);

          const float transition_score =
              node_score - kLengthDeltaWeight * total_len -
              kBackwardXPenaltyWeight * new_backward;

          const bool better = (transition_score > best_transition_score + kEps);
          const bool tie_better =
              (std::abs(transition_score - best_transition_score) <= kEps &&
               (total_len + 1e-12f < best_total_length ||
                (std::abs(total_len - best_total_length) <= 1e-12f &&
                 new_backward < best_backward)));

          if (better || tie_better) {
            best_transition_score = transition_score;
            best_total_length = total_len;
            best_backward = new_backward;
            best_v1 = v1;
            best_v2 = v2;
            best_v3 = v3;
          }
        }
      }
    }

    if (best_v3 < 0 || best_v1 < 0 || best_v2 < 0) UNEXPECTED();

    const int v1 = best_v1;
    const int v2 = best_v2;
    const int v3 = best_v3;

    processor_used[v1 - config.ProcessorNodeBegin()] = 1;
    processor_used[v2 - config.ProcessorNodeBegin()] = 1;
    state.node_destination_pairs[v0] = {v3, -1};
    state.node_destination_pairs[v3] = {v1, v2};
    state.node_types[v3] = 0;

    active_nodes.push_back(v3);
    for (const int v : iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
      const Int2D v_p = config.node_positions[v];
      node_distance_scores[v] = std::min(node_distance_scores[v], v0_p.Distance(v_p));
    }

    edges.push_back({v0, v3});
    edges.push_back({v3, v1});
    edges.push_back({v3, v2});
  }

  for ([[maybe_unused]] const int iter : iota(2, config.trash_type_count)) {
    // Replace v0 -> v1 with v0 -> v3, v3 -> v1, v3 -> v2.
    // v0 -> v1 should be an existing edge.
    // v2 should be a processor node which is not used yet.

    // Find these tuples with the largest transition_score.
    // - transition_score := 1.0 * node_distance_scores[v3]
    //                       - kLengthDeltaWeight * length_delta
    //                       - kBackwardXPenaltyWeight * backward_x_delta
    // - length_delta := v0_p.Distance(v3_p) + v3_p.Distance(v1_p) + v3_p.Distance(v2_p)
    //                   - v0_p.Distance(v1_p)
    // - backward_x_delta := sum of negative-x movement of new edges
    //                       minus that of the removed edge (v0->v1)

    const float kLengthDeltaWeight = 0.1f;

    const float kEps = 1.0e-9f;
    const float kBackwardXPenaltyWeight = 0.0f;

    auto backward_x_amount = [&](const Int2D &pa, const Int2D &pb) -> float {
      const int dx = pb.x - pa.x;
      return dx < 0 ? float(-dx) : 0.0f;
    };

    float best_transition_score = -1.0e18f;
    float best_extra_length = 1.0e18f;  // used only for tie-breaks
    int best_v0 = -1;
    int best_v1 = -1;
    int best_v2 = -1;
    int best_v3 = -1;
    int best_p_id = -1;

    // Try every unused sorter node as v3.
    for (const int v3 : iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
      if (state.node_types[v3] != -1) continue;  // sorter already used

      const Int2D v3_p = config.node_positions[v3];

      float v3_best_extra_length = 1.0e18f;
      float v3_best_backward_x_delta = 1.0e18f;
      float v3_best_transition_score = -1.0e18f;
      int v3_best_v0 = -1, v3_best_v1 = -1, v3_best_v2 = -1, v3_best_p_id = -1;

      // Enumerate existing edges (v0 -> v1).
      for (const auto &edge : edges) {
        const int v0 = edge.first;
        const int v1 = edge.second;

        const Int2D v0_p = config.node_positions[v0];
        const Int2D v1_p = config.node_positions[v1];

        // Enumerate unused processor node as v2.
        for (const int p_id : iota(0, config.trash_type_count)) {
          if (processor_used[p_id]) continue;
          const int v2 = config.ProcessorNodeId(p_id);
          const Int2D v2_p = config.node_positions[v2];

          // Intersection check: new edges (v0,v3), (v1,v3), (v2,v3)
          // must not intersect with any existing edge, except the edge (v0,v1)
          // which will be removed. Endpoints sharing is allowed.
          bool has_intersection = false;
          for (const auto &e2 : edges) {
            const int u0 = e2.first;
            const int u1 = e2.second;

            // Skip the edge (v0, v1) because it will be removed.
            if (u0 == v0 && u1 == v1) continue;

            const Int2D &u0_p = config.node_positions[u0];
            const Int2D &u1_p = config.node_positions[u1];

            // Check (v0, v3) vs (u0, u1), allowing shared endpoints.
            if (v0 != u0 && v0 != u1 && v3 != u0 && v3 != u1) {
              if (SegmentsIntersect(v0_p, v3_p, u0_p, u1_p)) {
                has_intersection = true;
              }
            }
            if (has_intersection) break;

            // Check (v1, v3) vs (u0, u1).
            if (v1 != u0 && v1 != u1 && v3 != u0 && v3 != u1) {
              if (SegmentsIntersect(v1_p, v3_p, u0_p, u1_p)) {
                has_intersection = true;
              }
            }
            if (has_intersection) break;

            // Check (v2, v3) vs (u0, u1).
            if (v2 != u0 && v2 != u1 && v3 != u0 && v3 != u1) {
              if (SegmentsIntersect(v2_p, v3_p, u0_p, u1_p)) {
                has_intersection = true;
              }
            }
            if (has_intersection) break;
          }

          if (has_intersection) continue;

          const float extra_len =
              v0_p.Distance(v3_p) + v3_p.Distance(v1_p) + v3_p.Distance(v2_p) -
              v0_p.Distance(v1_p);

          const float old_backward = backward_x_amount(v0_p, v1_p);
          const float new_backward = backward_x_amount(v0_p, v3_p) +
                                     backward_x_amount(v3_p, v1_p) +
                                     backward_x_amount(v3_p, v2_p);
          const float backward_delta = new_backward - old_backward;

          const float node_score = node_distance_scores[v3];
          const float local_transition_score =
              node_score - kLengthDeltaWeight * extra_len -
              kBackwardXPenaltyWeight * backward_delta;

          const bool better =
              (local_transition_score > v3_best_transition_score + kEps);
          const bool tie_better =
              (std::abs(local_transition_score - v3_best_transition_score) <=
                   kEps &&
               (extra_len + 1e-12f < v3_best_extra_length ||
                (std::abs(extra_len - v3_best_extra_length) <= 1e-12f &&
                 backward_delta < v3_best_backward_x_delta)));

          if (better || tie_better) {
            v3_best_transition_score = local_transition_score;
            v3_best_extra_length = extra_len;
            v3_best_backward_x_delta = backward_delta;
            v3_best_v0 = v0;
            v3_best_v1 = v1;
            v3_best_v2 = v2;
            v3_best_p_id = p_id;
          }
        }
      }

      if (v3_best_transition_score <= -1.0e17f) continue;  // no feasible tuple

      const bool better =
          (v3_best_transition_score > best_transition_score + kEps);
      const bool tie_better_cost =
          (std::abs(v3_best_transition_score - best_transition_score) <= kEps &&
           v3_best_extra_length < best_extra_length);

      if (better || tie_better_cost) {
        best_transition_score = v3_best_transition_score;
        best_extra_length = v3_best_extra_length;
        best_v0 = v3_best_v0;
        best_v1 = v3_best_v1;
        best_v2 = v3_best_v2;
        best_v3 = v3;
        best_p_id = v3_best_p_id;
      }
    }

    if (best_v3 < 0) {
      // No feasible insertion found; return current state.
      return state;
    }

    // Apply the replacement: replace edge (v0 -> v1) with
    // (v0 -> v3), (v3 -> v1), (v3 -> v2).
    if (state.node_destination_pairs[best_v0].first == best_v1) {
      state.node_destination_pairs[best_v0].first = best_v3;
    } else if (state.node_destination_pairs[best_v0].second == best_v1) {
      state.node_destination_pairs[best_v0].second = best_v3;
    } else {
      UNEXPECTED();
    }
    state.node_destination_pairs[best_v3] = {best_v1, best_v2};
    state.node_types[best_v3] = 0;  // placeholder sorter type

    // Mark the processor as used.
    if (best_p_id >= 0) processor_used[best_p_id] = 1;

    // Update node_distance_scores by newly activated sorter node best_v3.
    const Int2D best_v3_p = config.node_positions[best_v3];
    for (const int v : iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
      const Int2D v_p = config.node_positions[v];
      node_distance_scores[v] =
          std::min(node_distance_scores[v], best_v3_p.Distance(v_p));
    }

    // Update edge set: remove (best_v0, best_v1) and add new edges.
    vector<pair<int, int>> new_edges;
    new_edges.reserve(edges.size() + 2);
    for (const auto &e : edges) {
      if (e.first == best_v0 && e.second == best_v1) continue;
      new_edges.push_back(e);
    }
    new_edges.push_back({best_v0, best_v3});
    new_edges.push_back({best_v3, best_v1});
    new_edges.push_back({best_v3, best_v2});
    edges.swap(new_edges);
  }

  return state;
}

State MakeInitialSolutionInternal(const Config& config) {
  State state = MakeInitialTree(config);
  const optional<vector<int>> opt_active_nodes =
      GetTopologicalOrder(config, state);
  if (!opt_active_nodes) {
    UNEXPECTED();
  }
  const vector<int>& active_nodes = *opt_active_nodes;
  vector<int> node_id_to_active_index(config.NodeCount(), -1);
  for (const int k : iota(0, int(active_nodes.size()))) {
    node_id_to_active_index[active_nodes[k]] = k;
  }
  OptimizeSorterTypes(config, active_nodes, node_id_to_active_index, state);

  float score = Score(config, state);
  std::cerr << "time: " << g_time.CurrentMs() << " initial_score: " << score
            << "\n";

  const int hill_climb_count =
      config.trash_type_count * config.trash_type_count * 5;
  for ([[maybe_unused]] const int iter : iota(0, hill_climb_count)) {
    State new_state = state;
    int i0 = g_random.RandomInt(0, config.trash_type_count);
    int i1 = -1;
    while (true) {
      i1 = g_random.RandomInt(0, config.trash_type_count);
      if (i1 != i0) break;
    }
    std::swap(new_state.node_types[i0], new_state.node_types[i1]);
    // Set all sorter types to -1.
    for (const int node :
          iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
      if (new_state.node_types[node] >= 0) {
        new_state.node_types[node] = -1;
      }
    }
    OptimizeSorterTypes(config, active_nodes, node_id_to_active_index,
                        new_state);
    const float new_score = Score(config, new_state);
    if (score < new_score) {
      score = new_score;
      state = new_state;
    }
  }

  std::cerr << "initial_solution_hill_climb_count: " << hill_climb_count
            << "\n";

  return state;
}

}  // namespace initial_solution

State MakeInitialSolution(const Config& config) {
  return initial_solution::MakeInitialSolutionInternal(config);
}



Config ReadConfig() {
  Config config;
  std::cin >> config.trash_type_count;
  std::cin >> config.sorter_position_count;
  std::cin >> config.sorter_type_count;
  config.node_positions =
      vector<Int2D>(config.trash_type_count + config.sorter_position_count + 1);
  for (const int i :
       iota(config.ProcessorNodeBegin(), config.ProcessorNodeEnd())) {
    std::cin >> config.node_positions[i].x >> config.node_positions[i].y;
  }
  for (const int i : iota(config.SorterNodeBegin(), config.SorterNodeEnd())) {
    std::cin >> config.node_positions[i].x >> config.node_positions[i].y;
  }
  config.node_positions[config.InletNodeId()] = kInletPosition;
  // Make the number of columns a multiple of 8.
  config.sorter_configs = Matrix<float>(
      2 * config.sorter_type_count, ((config.trash_type_count + 7) / 8) * 8);
  for (const int i : iota(0, config.sorter_type_count)) {
    for (const int j : iota(0, config.trash_type_count)) {
      std::cin >> config.sorter_configs[2 * i, j];
      config.sorter_configs[2 * i + 1, j] =
          1.0f - config.sorter_configs[2 * i, j];
    }
  }
  return config;
}

void WriteOutput(const Config& config, const State& state) {
  // Write mapping from processor to trash type.
  for (const int i : iota(0, config.trash_type_count)) {
    std::cout << state.node_types[i] << " ";
  }
  std::cout << "\n";

  // Write destination of the inlet.
  std::cout << state.node_destination_pairs[config.InletNodeId()].first << "\n";

  // Write state of each sorter position.
  for (const int i : iota(0, config.sorter_position_count)) {
    if (state.node_types[config.SorterNodeId(i)] == -1) {
      std::cout << -1 << "\n";
    } else {
      std::cout << state.node_types[config.SorterNodeId(i)] << " "
                << state.node_destination_pairs[config.SorterNodeId(i)].first
                << " "
                << state.node_destination_pairs[config.SorterNodeId(i)].second
                << "\n";
    }
  }
}

void Solve() {
  const Config config = ReadConfig();
  const State initial_state = MakeInitialSolution(config);
  [[maybe_unused]] const float initial_score = Score(config, initial_state);
  const State beam_search_state = RunBeamSearch(config, 1900, initial_state);
  const float beam_search_score = Score(config, beam_search_state);
  std::cerr << "score: " << std::setprecision(6) << beam_search_score << "\n";
  std::cerr << "final_score: " << ConvertToFinalScore(beam_search_score)
            << "\n";
  WriteOutput(config, beam_search_state);
  // WriteOutput(config, initial_state);
  std::cerr << "time: " << g_time.CurrentMs() << "\n";
}

int main() { Solve(); }

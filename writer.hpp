#pragma once

#include <ostream>
#include "extract_topology.hpp"

namespace writer
{
class SVG
{
public:
  SVG(std::ostream &os, const double &width = 1);
  void operator<<(const extractTopology::Graph &graph);
  void operator<<(const std::vector<std::vector<extractTopology::Vertex>> &paths);

private:
  std::ostream &os;
  double width;
  void header();
  void footer();
};
}

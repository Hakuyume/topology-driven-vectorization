#pragma once

#include <ostream>
#include "extract_topology.hpp"

namespace writer
{
class SVG
{
public:
  SVG(std::ostream &os);
  void operator<<(const extractTopology::Graph &graph);
  void operator<<(const std::vector<std::vector<extractTopology::Vertex>> &paths);

private:
  std::ostream &os;
  void header();
  void footer();
};
}

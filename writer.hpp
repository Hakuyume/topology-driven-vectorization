#pragma once

#include <ostream>
#include "extract_topology.hpp"

namespace writer
{
class SVG
{
public:
  SVG(std::ostream &os, const double &thickness = 1);
  void operator<<(const extractTopology::Graph &graph);
  template <typename Container>
  void operator<<(const Container &paths)
  {
    header();
    for (const auto &path : paths) {
      os << "<path d=\"M";
      for (const auto &p : path)
        os << " " << p()(0) << " " << p()(1);
      os << "\"/>" << std::endl;
    }
    footer();
  }

private:
  std::ostream &os;
  double thickness;
  void header();
  void footer();
};
}

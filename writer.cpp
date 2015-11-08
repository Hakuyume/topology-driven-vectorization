#include "writer.hpp"

using namespace writer;

SVG::SVG(std::ostream &os)
    : os{os} {};

void SVG::header()
{
  os << "<?xml version=\"1.0\"?>" << std::endl;
  os << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
  os << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
  os << "<g stroke=\"black\" stroke-width=\"0.1\" fill=\"none\">" << std::endl;
}

void SVG::footer()
{
  os << "</g>" << std::endl;
  os << "</svg>" << std::endl;
}

void SVG::operator<<(const extractTopology::Graph &graph)
{
  header();
  const auto edges = boost::edges(graph);
  for (auto it = edges.first; it != edges.second; it++) {
    const auto &v = graph[boost::source(*it, graph)];
    const auto &u = graph[boost::target(*it, graph)];
    os << "<line x1=\"" << v()(0) << "\" y1=\"" << v()(1) << "\" x2=\"" << u()(0) << "\" y2=\"" << u()(1) << "\"/>" << std::endl;
  }
  footer();
}

void SVG::operator<<(const std::vector<std::vector<extractTopology::Vertex>> &paths)
{
  header();
  for (const auto &path : paths) {
    os << "<path d=\"M";
    for (const auto &v : path)
      os << " " << v()(0) << " " << v()(1);
    os << "\"/>" << std::endl;
  }
  footer();
}

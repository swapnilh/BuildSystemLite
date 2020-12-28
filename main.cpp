#include "build_system.h"

void TestGraph() {
  Graph graph;
  graph.InsertDependency({Task::Type::READ_FILE, "B.txt"}, {Task::Type::PARSE_FILE, "B.txt"});
  graph.InsertDependency({Task::Type::GENERATE_TITLE, "A.txt"}, {Task::Type::READ_FILE, "A.txt"}); 
  graph.InsertDependency({Task::Type::GENERATE_TITLE, "A.txt"}, {Task::Type::READ_FILE, "B.txt"});   graph.TopologicalView();
}

int main() {
  BuildSystem build;
  std::cout << build.Run({Task::Type::GENERATE_HTML, "index.rst"});
  build.Run({Task::Type::GENERATE_HTML, "tutorial.rst"});
  build.Run({Task::Type::GENERATE_HTML, "advanced.rst"});
  return 0;
}
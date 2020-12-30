#include "build_system.h"

void TestGraph() {
  Graph graph;
  graph.InsertDependency({Task::Type::READ_FILE, "B.txt"}, {Task::Type::PARSE_BODY, "B.txt"});
  graph.InsertDependency({Task::Type::PARSE_BODY, "A.txt"}, {Task::Type::READ_FILE, "A.txt"}); 
  graph.InsertDependency({Task::Type::PARSE_TITLE, "A.txt"}, {Task::Type::READ_FILE, "B.txt"});   graph.TopologicalView();
}

int main() {
  BuildSystem build;
  build.RebuildAll();
  std::cout << "Rebuilding -should use cache" << std::endl;
  build.RebuildAll();
  return 0;
}
#include "build_system.h"

void ToString(const std::vector<Task>& task_list) {
  for (const Task& task: task_list) {
    std::cout << task.ToString() << ", ";
  }
  std::cout << std::endl;
}

void TestGraph() {
  Graph graph;
  Task read_a = {Task::Type::READ_FILE, "A.txt"};
  Task parse_a = {Task::Type::PARSE_BODY, "A.txt"};
  Task read_b = {Task::Type::READ_FILE, "B.txt"};
  Task parse_b = {Task::Type::PARSE_BODY, "B.txt"};
  graph.InsertDependency(read_b, parse_b);
  graph.InsertDependency(read_a, parse_a ); 
  graph.InsertDependency(parse_a, parse_b);   
  ToString(graph.GetImmediateDependents(read_a));
  ToString(graph.GetDependentsRecursively(read_a));
  ToString(graph.GetImmediateDependents(read_b));
  ToString(graph.GetDependentsRecursively(read_b));
}

int main() {
  BuildSystem build;
  build.RebuildAll();
  std::cout << "\nRebuilding -should use cache\n\n\n";
  build.RebuildAll();
  // TestGraph();
  return 0;
}
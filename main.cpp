#include "build_system.h"

void ToString(const std::vector<Task>& task_list) {
  std::cout << "ToString:\n";
  for (const Task& task: task_list) {
    std::cout << task.ToString() << "\n";
  }
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
  ToString(graph.GetDependentsRecursively({read_a}));
  ToString(graph.GetImmediateDependents(read_b));
  ToString(graph.GetDependentsRecursively({read_b, read_a}));
}

void Print(const std::string& message) {
  std::cout << "\n ******" << message << "****** \n\n"; 
}

int main() {
  BuildSystem build;
  build.RebuildAll();
  Print("Rebuilding -should use cache");
  build.RebuildAll();
  build.EditFile("advanced.rst", 
  R"(2. Advanced Topics\
Wow, you have progressed to the advanced section.
Make sure to first check out <tutorial.rst>.)");
  Print("Rebuilding after dummy edit: Only ReadFile recomputed");
  build.RebuildAll();
  build.EditFile("advanced.rst", 
  R"(2. Advanced Topics\
Wow, you have *successfully* progressed to the advanced section.
Make sure to first check out <tutorial.rst>.)");
  Print("Rebuilding after body edit: All advanced.rst tasks     recomputed");
  build.RebuildAll();
  build.EditFile("advanced.rst", 
  R"(2. Very Advanced Topics\
Wow, you have *successfully* progressed to the advanced section.
Make sure to first check out <tutorial.rst>.)");
  Print("Rebuilding after body edit: Only tutorial and index     recomputed");
  build.RebuildAll();
  // TestGraph();
  return 0;
}
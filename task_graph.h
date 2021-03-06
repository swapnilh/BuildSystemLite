#include "task.h"
#include "graph.h"

#include <stack>
#include <iostream>

class TaskGraph {
  public:
  void PreFunc(Task task);

  void PostFunc();

  void PrintGraph() const {
    task_graph_.TopologicalView();
  }

  const Graph* GetTaskGraph() const { return &task_graph_; }

  private:
  std::stack<Task> task_stack_;
  Graph task_graph_;
};
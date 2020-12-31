#include "task_graph.h"

#include <iostream>

void TaskGraph::PreFunc(Task task) {
  if (!task_stack_.empty()) {
    task_graph_.InsertDependency(task, task_stack_.top());
  }
  std::cout << "--> Task:" << task.ToString() << std::endl;
  task_stack_.push(task);
}

void TaskGraph::PostFunc() {
  task_stack_.pop();
}
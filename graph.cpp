#include "graph.h"

#include <iostream>

void Graph::TopologicalView() const {
  std::stack<Task> dfs_stack;
  std::unordered_set<Task, TaskHash> visited;
  for (const auto& [task, _]: forward_edges_) {
    TopologicalViewHelper(task, dfs_stack, visited);
  }
  while (!dfs_stack.empty()) {
    std::cout << dfs_stack.top().ToString() 
              << std::endl;
    dfs_stack.pop();
  }
}

void Graph::TopologicalViewHelper(
  const Task& task,
  std::stack<Task>& dfs_stack, 
  std::unordered_set<Task, TaskHash>& visited) const {
    if (visited.find(task) != visited.end()) return;
    const auto itr = forward_edges_.find(task);
    if (itr != forward_edges_.end()) {
      for (const auto& next_task : itr->second) {
        TopologicalViewHelper(
          next_task, dfs_stack, visited);
        if (visited.find(next_task) != visited.end()) { 
          continue;
        }
        visited.insert(next_task);
        dfs_stack.push(next_task);
      }
    }
    if (visited.find(task) != visited.end()) return;
    visited.insert(task);
    dfs_stack.push(task);
  }
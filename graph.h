#include "task.h"

#include <unordered_map>
#include <vector>
#include <stack>
#include <unordered_set>


class Graph {
public:
  // one -> two 
  void InsertDependency (const Task& one, const Task& two) {
    forward_edges_[one].push_back(two);
    backward_edges_[two].push_back(one);
  }

  std::vector<Task>
    GetImmediateDependents(const Task& task) const;

  std::vector<Task>
    GetDependentsRecursively(const Task& task) const;

  void TopologicalView() const;

private:
  void TopologicalViewHelper(
    const Task& task,
    std::stack<Task>& dfs_stack, std::unordered_set<Task, TaskHash>& visited) const;

  std::unordered_map <Task, std::vector<Task>, TaskHash>
    forward_edges_;
  std::unordered_map <Task, std::vector<Task>, TaskHash>
    backward_edges_;
};
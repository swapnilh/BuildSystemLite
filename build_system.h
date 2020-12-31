#include "task_graph.h"

#include <string>
#include <unordered_map>
#include <utility>

class BuildSystem {
  public:
  // Populates the `filesystem_`.
  BuildSystem();

  void RebuildAll();

  // Updates the `filename` with `new_contents`.
  void EditFile(
    const std::string& filename, 
    const std::string& new_contents);

  void PrintTaskGraph() const {
    task_graph_.PrintGraph();
  }

  private:
  // Returns file contents.
  std::string ReadFile(const std::string& filename);

  // Extracts body text from file.
  std::string ParseBody(const std::string& filename);

  // Extracts title from file.
  std::string ParseTitle(const std::string& filename);

  // "Compiles" input file and prints result.
  std::string GenerateHTML(const std::string& filename);

  // Returns <title, body> from file. 
  std::pair<std::string, std::string> ParseFile(
    const std::string&filename);

  // For every link in the input content (e.g., "<tutorial.rst>"),
  // replaces it with the title of the link (e.g., *Tutoraial*).
  std::string InsertLinks(const std::string& content);

  // Runs task and computes result!
  std::string Run(const Task& task);

  TaskGraph task_graph_;
  // Task -> cached result.
  std::unordered_map<Task, std::string, TaskHash> task_cache_;
  std::unordered_set<Task, TaskHash> todos_;
  std::unordered_map<std::string, std::string> filesystem_;
};
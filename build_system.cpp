#include "build_system.h"

#include <iostream>
#include <sstream> 

BuildSystem::BuildSystem() {
  filesystem_["index.rst"] = R"(Table of Contents\
<tutorial.rst>
<advanced.rst>)";
  filesystem_["tutorial.rst"] = R"(1. Tutorial\
Welcome to this book. You will learn a lot!
Just make sure to read till the end.)";
  filesystem_["advanced.rst"] = R"(2. Advanced Topics\
Wow, you have progressed to the advanced section.
Make sure to first check out <tutorial.rst>.)";
  for (const auto& [filename, body] : filesystem_) {
    todos_.insert(
      {Task::Type::GENERATE_HTML, filename});
  }
}

void BuildSystem::RebuildAll() {
  const Graph* graph = task_graph_.GetTaskGraph();
  while (!todos_.empty()) {
    std::vector<Task> dependent_tasks = 
      graph->GetDependentsRecursively(
        {todos_.begin(), todos_.end()});
    for (const Task& task : dependent_tasks) {
      if (todos_.find(task) == todos_.end()) continue;
      todos_.erase(task);
      const std::string cached_result = task_cache_[task];
      // Remove the cached item to force recomputation.
      task_cache_.erase(task);
      std::string new_result = Run(task);
      const bool unchanged = cached_result == new_result;
      task_cache_[task] = std::move(new_result);
      if (unchanged) continue;
      // Else add immediate dependents to TODO.
      const auto nbrs = graph->GetImmediateDependents(task);
      todos_.insert(nbrs.begin(), nbrs.end());
    }
  }

  for (const auto& [filename, body] : filesystem_) {
    std::cout << Run({Task::Type::GENERATE_HTML, filename});
  }
}

void BuildSystem::EditFile(
  const std::string& filename, 
  const std::string& new_contents) {
    filesystem_[filename] = new_contents;
    todos_.insert({Task::Type::READ_FILE, filename});
}

std::string BuildSystem::ReadFile(const std::string& filename) {
  const auto itr = filesystem_.find(filename);
  if (itr == filesystem_.end()) return "";
  return itr->second;
}

std::string BuildSystem::ParseBody(
  const std::string& filename) {
  return ParseFileHelper(filename).second;
}

std::string BuildSystem::ParseTitle(
  const std::string& filename) {
  return ParseFileHelper(filename).first;
}

std::string BuildSystem::GenerateHTML(const std::string& filename) {
  std::stringstream output_html;    
  const std::string title = Run(
    {Task::Type::PARSE_TITLE, filename});
  const std::string body = Run(
    {Task::Type::PARSE_BODY, filename});
  output_html << "***** " << title << " *****" 
            << std::endl << std::endl;
  output_html << InsertLinksHelper(body) << std::endl;
  output_html << "-------------" << std::endl;
  return output_html.str();
}

std::pair<std::string, std::string> BuildSystem::ParseFileHelper(
  const std::string&filename) {
  const std::string contents = Run(
    {Task::Type::READ_FILE, filename});
  if (contents.empty()) return {"", ""};

  const auto split_pos = contents.find('\\');
  std::string title = contents.substr(0, split_pos);
  std::string body = contents.substr(split_pos + 1);
  return {std::move(title), std::move(body)};
}

std::string BuildSystem::InsertLinksHelper(
  const std::string& content) {
  std::string processed_content;
  processed_content.reserve(content.size());
  for (int i = 0; i < content.size(); ++i) {
    if (content[i] != '<') {
      processed_content.append(1, content[i]);
      continue;
    }
    int end = i;
    // Iterate till the end of the link.
    while (content[++end] != '>'); 
    const std::string linked_file = 
      content.substr(i + 1, end - i - 1);
    std::string linked_title = 
      "*" + Run({Task::Type::PARSE_TITLE, linked_file}) + "*";
    processed_content.append(linked_title);
    i = end; // i will be incremented when the iteration ends.
  }
  return processed_content;
}

std::string BuildSystem::Run(const Task& task) {
  task_graph_.PreFunc(task);
  // Return cached output if available.
  const auto cache_itr = task_cache_.find(task);    
  if (cache_itr != task_cache_.end()) {
    task_graph_.PostFunc();
    return cache_itr->second;
  }  
  std::string output;
  switch (task.task_type) {
    case Task::Type::READ_FILE: {
      output =  ReadFile(task.input_file);
      break;
    }
    case Task::Type::PARSE_BODY: { 
      output = ParseBody(task.input_file);
      break;
    }
    case Task::Type::PARSE_TITLE: { 
      output = ParseTitle(task.input_file);
      break;
    }
    case Task::Type::GENERATE_HTML: { 
      output = GenerateHTML(task.input_file);
      break;
    }
  }
  // Cache output and return.
  task_cache_[task] = output;
  task_graph_.PostFunc();
  return output;
}
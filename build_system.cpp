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
    std::cout << GenerateHTML(filename);
  }
}

void BuildSystem::EditFile(
  const std::string& filename, 
  const std::string& new_contents) {
    filesystem_[filename] = new_contents;
    todos_.insert({Task::Type::READ_FILE, filename});
}

std::string BuildSystem::ReadFile(const std::string& filename) {
  Task read_file = {Task::Type::READ_FILE, filename};
  task_graph_.PreFunc(read_file);
  const auto cache_itr = task_cache_.find(read_file);
  if (cache_itr != task_cache_.end()) {
    task_graph_.PostFunc();
     return cache_itr->second;
  }
  const auto itr = filesystem_.find(filename);
  if (itr == filesystem_.end()) return "";
  std::string file_contents = itr->second;
  task_cache_[read_file] = file_contents;
  task_graph_.PostFunc();
  return file_contents;
}

std::string BuildSystem::ParseBody(
  const std::string& filename) {
  Task parse_body = {Task::Type::PARSE_BODY, filename}; 
  task_graph_.PreFunc(parse_body);
  const auto cache_itr = task_cache_.find(parse_body);  
  if (cache_itr != task_cache_.end()) {
    task_graph_.PostFunc();
    return cache_itr->second;
  }
  std::string body = ParseFile(filename).second;
  task_graph_.PostFunc();
  task_cache_[parse_body] = body;
  return body;
}

std::string BuildSystem::ParseTitle(
  const std::string& filename) {
  Task parse_title = 
    {Task::Type::PARSE_TITLE, filename}; 
    task_graph_.PreFunc(parse_title);
    const auto cache_itr = task_cache_.find(parse_title);  
  if (cache_itr != task_cache_.end()) {
    task_graph_.PostFunc();
    return cache_itr->second;
  }
  std::string title = ParseFile(filename).first;
  task_graph_.PostFunc();
  task_cache_[parse_title] = title;
  return title;
}

std::string BuildSystem::GenerateHTML(const std::string& filename) {
  Task generate_html = {Task::Type::GENERATE_HTML, filename}; 
  task_graph_.PreFunc(generate_html);
  const auto cache_itr = task_cache_.find(generate_html);    
  if (cache_itr != task_cache_.end()) {
    task_graph_.PostFunc();
    return cache_itr->second;
  }
  std::stringstream output_html;    
  const std::string title = ParseTitle(filename);
  const std::string body = ParseBody(filename);
  output_html << "***** " << title << " *****" 
            << std::endl << std::endl;
  output_html << InsertLinks(body) << std::endl;
  output_html << "-------------" << std::endl;
  task_graph_.PostFunc();
  task_cache_[generate_html] = output_html.str();
  return output_html.str();
}

std::pair<std::string, std::string> BuildSystem::ParseFile(
  const std::string&filename) {
  const std::string contents = ReadFile(filename);
  if (contents.empty()) return {"", ""};

  const auto split_pos = contents.find('\\');
  std::string title = contents.substr(0, split_pos);
  std::string body = contents.substr(split_pos + 1);
  return {std::move(title), std::move(body)};
}

std::string BuildSystem::InsertLinks(const std::string& content) {
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
    std::string linked_title = "*" + ParseTitle(linked_file) + "*";
    processed_content.append(linked_title);
    i = end; // i will be incremented when the iteration ends.
  }
  return processed_content;
}

std::string BuildSystem::Run(const Task& task) {
  switch (task.task_type) {
    case Task::Type::READ_FILE: {
      return ReadFile(task.input_file);
    }
    case Task::Type::PARSE_BODY: { 
      return ParseBody(task.input_file);
    }
    case Task::Type::PARSE_TITLE: { 
      return ParseTitle(task.input_file);
    }
    case Task::Type::GENERATE_HTML: { 
      return GenerateHTML(task.input_file);
    }
  }
}
#include "build_system.h"

#include <iostream>
#include <sstream> 

BuildSystem::BuildSystem() {
  filesystem_["index.rst"] = R"(Table of Contents\
  <tutorial.rst>
  <advanced.rst>
  )";
  filesystem_["tutorial.rst"] = R"(1. Tutorial\
  Welcome to this book. You will learn a lot!
  Just make sure to read till the end.
  )";
  filesystem_["advanced.rst"] = R"(2. Advanced Topics\
  Wow, you have progressed to the advanced section.
  Make sure to first check out <tutorial.rst>.
  )";
}

std::string BuildSystem::Run(const Task& task) {
  switch (task.task_type) {
    case Task::Type::READ_FILE: {
      return ReadFile(task.input_file);
    }
    case Task::Type::PARSE_FILE: { 
      const auto [header, body] = ParseFile(task.input_file);
      return header + body;
    }
    case Task::Type::GENERATE_TITLE: { 
      return GetTitle(task.input_file);
    }
    case Task::Type::GENERATE_HTML: { 
      return GenerateHTML(task.input_file);
    }
  }
}


std::string BuildSystem::ReadFile(const std::string& filename) {
  task_graph_.PreFunc({Task::Type::READ_FILE, filename});
  const auto itr = filesystem_.find(filename);
  if (itr == filesystem_.end()) return "";
  task_graph_.PostFunc();
  return itr->second;
}

std::pair<std::string, std::string> BuildSystem::ParseFile(
  const std::string&filename) {
  task_graph_.PreFunc({Task::Type::PARSE_FILE, filename});
  const std::string contents = ReadFile(filename);
  if (contents.empty()) return {"", ""};

  const auto split_pos = contents.find('\\');
  std::string title = contents.substr(0, split_pos);
  std::string body = contents.substr(split_pos + 1);
  task_graph_.PostFunc();
  return {std::move(title), std::move(body)};
}

std::string BuildSystem::GenerateHTML(const std::string& filename) {
  std::stringstream output_html;        
  task_graph_.PreFunc({Task::Type::GENERATE_HTML, filename});
  const auto [title, body] = ParseFile(filename);
  output_html << "***** " << title << " *****" 
            << std::endl << std::endl;
  output_html << InsertLinks(body) << std::endl;
  output_html << "-------------" << std::endl;
  task_graph_.PostFunc();
  return output_html.str();
}

std::string BuildSystem::GetTitle(const std::string& filename) {
  task_graph_.PreFunc({Task::Type::GENERATE_TITLE, filename});
  task_graph_.PostFunc();
  return ParseFile(filename).first;
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
    std::string linked_title = "*" + GetTitle(linked_file) + "*";
    processed_content.append(linked_title);
    i = end; // i will be incremented when the iteration ends.
  }
  return processed_content;
}

#ifndef TASK_H
#define TASK_H
#include <string>

struct Task {
  enum class Type {
    READ_FILE,
    PARSE_FILE,
    GENERATE_TITLE,
    GENERATE_HTML,
  };

  Task(Type task_type, const std::string& input_file) 
  : task_type(task_type), input_file (input_file) {}

  std::string ToString() const {
    std::string debug_string;
    switch (task_type) {
      case Type::READ_FILE: debug_string = "READ_FILE"; break;
      case Type::PARSE_FILE: debug_string = "PARSE_FILE"; break;
      case Type::GENERATE_TITLE: debug_string = "GENERATE_TITLE"; break;
      case Type::GENERATE_HTML: debug_string = "GENERATE_HTML"; break;
    }
    debug_string.append("  ");
    return debug_string.append(input_file);
  }

  bool operator==(Task const& other)
  {
    return (task_type == other.task_type) && 
            (input_file == other.input_file); 
  }

  friend bool operator==(
  const Task& lhs, const Task& rhs)  {
    return (lhs.task_type == rhs.task_type) && 
            (lhs.input_file == rhs.input_file); 
  }
  Type task_type;
  std::string input_file;
};

struct TaskHash {
   size_t operator() (const Task &task) const {
     return std::hash<std::string>{}(task.ToString());
   }
};
#endif
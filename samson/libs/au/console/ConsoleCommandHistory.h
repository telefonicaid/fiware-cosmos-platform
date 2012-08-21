#ifndef _AU_CONSOLE_COMMAND_HISTORY
#define _AU_CONSOLE_COMMAND_HISTORY

#include <sstream>
#include <string>
#include <vector>


namespace au {
class ConsoleCommand;

class ConsoleCommandHistory {
  std::vector<ConsoleCommand *> commands;
  size_t pos;

  std::string file_name;

public:

  ConsoleCommandHistory();
  ~ConsoleCommandHistory();

  ConsoleCommand *current();


  void recover_history();
  void save_history();

  void move_up();
  void move_down();
  void new_command();


  std::string str_history(size_t limit) {
    std::ostringstream output;

    size_t pos = 0;

    if (limit > 0) {
      if (commands.size() > limit) {
        pos = commands.size() - limit;
      }
    }

    for (; pos < commands.size(); pos++) {
      output << commands[pos]->getCommand() << "\n";
    }

    return output.str();
  }
};
}

#endif // ifndef _AU_CONSOLE_COMMAND_HISTORY

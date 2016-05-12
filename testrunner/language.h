#include <vector>
#include <map>
#include <string>

struct Run
{
    std::string name;
    std::string command;
};

struct Language
{
    bool compiled = true;
    std::vector<Run> runs;
    std::vector<std::string> preparationCommands;
};

std::map<std::string, Language> languages{
    {"cpp",
     {true,
      {
          {"gcc",
           "g++ -g -std=c++11 -pedantic -Wall -Wextra -fsanitize=undefined"},
          {"clang",
           "clang++ -std=c++11 -pedantic -Wall -Wextra -fsanitize=integer "
           "-fsanitize=undefined"},
          {"perf", "g++ -std=c++11 -pedantic -Wall -Wextra -O2 "},
      },
      {"clang-format -i ", "cppcheck "}}},
    {"py",
     {false,
      {
          {"diag", "python "}, {"diag", "python -O "},
      },
      {}

     }}

};

#pragma once

#include <fstream>
#include <string>
#include <vector>

class Keybind {
public:
  std::string comment;
  std::string shortcut;

  Keybind();
  Keybind(std::string _comment, std::string _shortcut);

  bool is_complete();
};

std::vector<Keybind> parse_file(const std::string &path);
void parse_line(const std::string &line, Keybind *keybind);
Keybind *parse_keybind(std::ifstream *file);

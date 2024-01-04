#include <glib.h>

#include "./config_parser.hpp"

std::vector<Keybind> parse_file(const std::string &path) {
  std::vector<Keybind> keybinds;
  std::ifstream file;
  file.open(path);

  if (file.is_open()) {
    while (file) {
      Keybind *parsed_keybind = parse_keybind(&file);
      if (parsed_keybind) {
        keybinds.push_back(*parsed_keybind);
        delete parsed_keybind;
      }
    }
  } else {
    g_error("Could not open file at %s\n", path.c_str());
  }

  return keybinds;
}

void parse_line(const std::string &line, Keybind *keybind) {
  if (line[0] == '#') { // Comment
    // Only copy what's after the hashtags and spaces
    for (size_t i = 0; i < line.size(); i++) {
      if (line[i] != '#' && line[i] != ' ') {
        keybind->comment = line.substr(i);
        return;
      }
    }
  } else if (!isspace(line[0])) { // True if the line is not a comment nor an action
    keybind->shortcut = line;
    return;
  }
}

Keybind *parse_keybind(std::ifstream *file) {
  Keybind *keybind = new Keybind(); // Create an empty keybind

  while (true) {
    std::string line;
    std::getline(*file, line);
    parse_line(line, keybind);

    if (file->eof() || keybind->shortcut.size() > 0) // Allow shortcuts with no comments
      break;
  }

  if (keybind->shortcut.size() == 0 && keybind->comment.size() == 0)
    return nullptr; // Return nullptr if we have parsed nothing

  if (!keybind->is_complete())
    g_warning("Parsed an incomplete keybind:\n  Description: %s\n  Shortcut: %s\n", keybind->comment.c_str(),
              keybind->shortcut.c_str());

  return keybind;
}

Keybind::Keybind(std::string _comment, std::string _shortcut) : comment{_comment}, shortcut{_shortcut} {}

bool Keybind::is_complete() {
  return this->comment.size() > 0 && this->shortcut.size() > 0;
}

Keybind::Keybind() : comment{}, shortcut{} {}

#pragma once

#include <string>
#include <vector>
#include <glib.h>

#include "./config_parser.hpp"

std::string expand_path(const std::string &path);
void parse_command_line_arguments(int argc, char* argv[]);
void print_for_rofi(const std::vector<Keybind> &keybinds);
void print_help(GOptionContext *context, gboolean main_help, GOptionGroup *group);
std::string search_for_existing_file(const std::vector<std::string> &paths);
char *string_to_char_pointer(const std::string &s);

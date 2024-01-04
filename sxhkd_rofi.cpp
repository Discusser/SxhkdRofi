#include <climits>
#include <cstring>
#include <filesystem>
#include <glib.h>
#include <regex>
#include <stdexcept>

#include "./config_parser.hpp"
#include "./sxhkd_rofi.hpp"

gchar *sxhkd_config_path;
gboolean align_entries = false;

GOptionEntry command_line_arguments[] = {
    {"file", 'f', G_OPTION_FLAG_NONE, G_OPTION_ARG_FILENAME, &sxhkd_config_path,
     "The path to the sxhkdrc file containing the keybindings. Default: $HOME/.config/sxhkd/sxhkdrc or "
     "$XDG_CONFIG_HOME/sxhkd/sxhkdrc",
     "/path/to/sxhkdrc"},
    {"align-entries", 'a', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &align_entries,
     "Whether or not entries should be aligned on the semicolon (with right padding). Default: false", "true"},
    nullptr,
};

int main(int argc, char *argv[]) {
  parse_command_line_arguments(argc, argv);

  std::vector<Keybind> keybinds = parse_file(sxhkd_config_path);
  print_for_rofi(keybinds);

  // Unassign allocated memory
  delete sxhkd_config_path;

  return 0;
}

void parse_command_line_arguments(int argc, char *argv[]) {
  GError *error = nullptr;
  GOptionContext *context = g_option_context_new(nullptr);
  g_option_context_add_main_entries(context, command_line_arguments, nullptr);

  g_option_context_set_help_enabled(context, true);

  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    print_help(context, true, nullptr);

    g_error("An error occured while parsing command line arguments: %s\n", error->message);
    g_error_free(error);
  }

  g_option_context_free(context);

  // True when the user selected an entry. See `man rofi-script.5`
  char *rofi_retv_raw = std::getenv("ROFI_RETV");
  if (rofi_retv_raw) {
    try {
      int rofi_retv = std::stoi(rofi_retv_raw);
      if (rofi_retv == 1 || rofi_retv == 2) {
        std::exit(0);
      }
    } catch (const std::invalid_argument &e) {
      g_warning("Could not parse ROFI_RETV environment variable (has value %s) into an integer\n", rofi_retv_raw);
    } catch (const std::out_of_range &e) {
      g_warning("ROFI_RETV environment variable (value %s) is not between %i and %i\n", rofi_retv_raw, INT_MIN,
                INT_MAX);
    }
  }

  if (sxhkd_config_path == nullptr) {
    sxhkd_config_path = string_to_char_pointer(search_for_existing_file({
        "$HOME/.config/sxhkd/sxhkdrc",
        "$XDG_CONFIG_HOME/sxhkd/sxhkdrc",
    }));
  }
}

std::string search_for_existing_file(const std::vector<std::string> &paths) {
  for (std::string path : paths) {
    std::string expanded_path = expand_path(path);
    if (std::filesystem::exists(expanded_path)) {
      return expanded_path;
    }
  }

  std::string error_data;
  error_data.reserve(paths.data()->size() +
                     paths.size() * 3); // Try to reserve the memory in advance, I'm not sure if this is correct
  for (std::string path : paths) {
    error_data.append("  ").append(path).append("\n");
  }
  g_error("No file exists in list:\n%s", error_data.c_str());

  return "";
}

std::string expand_path(const std::string &path) {
  std::string expanded_path;
  std::regex regex(R"(\$[[:alpha:]]\w*\b)", std::regex_constants::ECMAScript | std::regex_constants::match_any);

  std::vector<std::smatch> matches;

  auto path_begin = std::sregex_iterator(path.begin(), path.end(), regex);
  auto path_end = std::sregex_iterator();
  for (std::sregex_iterator i = path_begin; i != path_end; i++) {
    std::smatch match = *i;
    matches.push_back(match);
  }

  if (matches.size() == 0)
    return path;

  for (size_t i = 0; i < path.size(); i++) {
    if (static_cast<size_t>(matches[0].position()) == i) {
      // substr(1) to discard the $
      const char *expanded_variable = std::getenv(matches[0].str().substr(1).c_str());
      if (!expanded_variable)
        expanded_variable = "";
      expanded_path.append(expanded_variable);

      i += matches[0].str().size() - 1;
      matches.erase(matches.begin());
    } else {
      expanded_path.push_back(path[i]);
    }
  }

  return expanded_path;
}

char *string_to_char_pointer(const std::string &s) {
  size_t size = s.size() + 1;
  char *pointer = new char[size];
  std::snprintf(pointer, size, "%s", s.c_str());

  return pointer;
}

void print_for_rofi(const std::vector<Keybind> &keybinds) {
  int shortcut_padding = 0;
  if (align_entries) {
    size_t max_shortcut_length_index = 0;
    for (size_t i = 0; i < keybinds.size(); i++) {
      if (keybinds[i].shortcut.size() > keybinds[max_shortcut_length_index].shortcut.size()) {
        max_shortcut_length_index = i;
      }
    }

    shortcut_padding = keybinds[max_shortcut_length_index].shortcut.size() + 4;
  }

  for (Keybind keybind : keybinds) {
    if (shortcut_padding > 0) {
      g_print("%-*s: %s\n", shortcut_padding, keybind.shortcut.c_str(), keybind.comment.c_str());
    } else {
      g_print("%s: %s\n", keybind.shortcut.c_str(), keybind.comment.c_str());
    }
  }
}

void print_help(GOptionContext *context, gboolean main_help, GOptionGroup *group) {
  gchar *help_string = g_option_context_get_help(context, main_help, group);
  g_print("%s", help_string);
  g_free(help_string);
}

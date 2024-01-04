# SxhkdRofi
SxhkdRofi is a program that parses your `sxhkdrc` file, used with [bspwm](https://github.com/baskerville/bspwm) to define keybindings, and displays them using [Rofi](https://github.com/davatorium/rofi). 
I created this because there are a lot of keybindings to know when using [bspwm](https://github.com/baskerville/bspwm), and having to open the `sxhkdrc` file everytime you want to find a keybinding can get annoying.

# Installation
## Prebuilt binaries
At the moment of writing this, prebuilt binaries are not available.
## From source
You can compile this yourself using `meson`.
### Dependencies
- [rofi](https://github.com/davatorium/rofi)
- meson
- libglib2.0

You can install these on Debian based systems with `apt`:
```bash
apt install rofi meson libglib2.0-dev
``` 
### Compiling
```bash
git clone https://github.com/Discusser/SxhkdRofi.git && cd SxhkdRofi
meson setup builddir && cd builddir
meson compile
cp SxhkdRofi ~/.local/bin/ # You can copy this anywhere in your PATH and it'll work
```
# Usage
SxhkdRofi outputs keybindings to the terminal to be used with Rofi. You can achieve this with the following command:
```bash
rofi -modi 'sxhkd:SxhkdRofi' -show sxhkd
```
I recommend the following command to make it a bit prettier. You can change the value of `theme` and `theme-str` to your liking, but that's just how I've configured it.
```bash
rofi -modi "sxhkd:SxhkdRofi -a" -show sxhkd -theme $HOME/.config/rofi/launchers/type-1/style-4 -theme-str 'window { width: 50%; }'
```
The command above creates a menu that looks something like this:
![image](https://github.com/Discusser/SxhkdRofi/assets/47938380/0530712f-f486-4c19-b60c-d4e76edac6b4)

If your `sxhkdrc` file is not located in the directories SxhkdRofi searches in (see below), you can specify it by passing the `-f` or `--file=` flag to `SxhkdRofi`:
```bash
rofi -modi 'sxhkd:SxhkdRofi -f /path/to/sxhkdrc' -show sxhkd
```
## `--help`
```
Usage:
  SxhkdRofi [OPTION?]

Help Options:
  -h, --help                      Show help options

Application Options:
  -f, --file=/path/to/sxhkdrc     The path to the sxhkdrc file containing the keybindings. Default: $HOME/.config/sxhkd/sxhkdrc or $XDG_CONFIG_HOME/sxhkd/sxhkdrc
  -a, --align-entries=true        Whether or not entries should be aligned on the semicolon (with right padding). Default: false
```

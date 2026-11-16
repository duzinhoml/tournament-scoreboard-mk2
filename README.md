# Tournament Scoreboard

Qt Widgets desktop tournament manager. Three GUI tasks: register teams, record match results, and compare ranked standings under two scoring rules. Team and match history can be saved and loaded as versioned JSON.

## Verified environment

- macOS 15.1 (24B83), Apple Silicon arm64
- Apple Clang 16.0.0 (clang-1600.0.26.6), Xcode command line toolchain
- C++17, configured in both qmake projects
- Qt 6.10.3: Core, Gui, Widgets; qmake and make

Other operating systems have not been verified. Install Qt 6.10.3 with its macOS desktop kit and Xcode command line tools. No Qt Creator installation is needed for these commands.

## Build and run

From the repository root (adjust the Qt path if installed elsewhere):

```sh
export QT_BIN="$HOME/Qt/6.10.3/macos/bin"
mkdir -p build
"$QT_BIN/qmake" tournament-scoreboard.pro -o build/Makefile
make -C build -j4
./build/tournament-scoreboard.app/Contents/MacOS/tournament-scoreboard
```

Alternatively, open `tournament-scoreboard.pro` in Qt Creator, select the Qt 6.10.3 macOS kit, Build, then Run.

Scores are non-negative integers; the GUI limits each to 1,000,000. Domain validation also rejects negative scores and unregistered opponents. Names are case-sensitive; GUI input is trimmed. Loading replaces the tournament only after every record passes validation. Saving uses QSaveFile's atomic commit, avoiding partial overwrites. No autosave: save explicitly before closing.

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

A Caro (Gomoku/5-in-a-row) game built in C++ with [raylib](https://www.raylib.com/), themed as a hero-battle game ("Caro Battle"). 15x15 board, standard 5-in-a-row win condition. Supports PvP, vs-bot, and bot-vs-bot modes with save/load slots.

## Build & Run

Build (macOS, Apple Silicon, via clang++ + raylib from Homebrew):

```bash
clang++ -std=c++17 *.cpp -o caro_game \
  -L/opt/homebrew/lib -I/opt/homebrew/include -lraylib \
  -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
```

This mirrors the VS Code default build task in [.vscode/tasks.json](.vscode/tasks.json) ("Game Caro"). There is no Makefile/CMake — all `.cpp` files in the repo root are compiled together in one invocation.

Run: `./caro_game` (must be run from the repo root so relative `assets/` paths resolve).

There is no test suite, linter, or CI config in this repo.

## Architecture

The codebase is a single executable split into four header/source pairs, all aggregated through [Library.h](Library.h):

- **[DataStruct.h](DataStruct.h)** — Plain data only: `Point` (one board cell), `Player`, and `GameState` (the entire game/session state: board, both players, turn, mode, bot config, win-line, save metadata). No logic lives here.
- **[LogicControl.h](LogicControl.h)/.cpp** — Pure game rules, independent of rendering: move validation (`MakeMove`), win detection (`CheckWin`, scanning 4 directions from the last move), the bot AI (`EvaluatePosition` + `BotMove`), and binary save/load (`fwrite`/`fread` of the raw `GameState` struct to `save_<slot>.bin`).
- **[GUI.h](GUI.h)/.cpp** — Owns `UIState` (all loaded textures, fonts, sounds, animation state, particle systems) and the top-level `InitGUI`/`UpdateGUI`/`DrawGUI` dispatch. Routes to either the in-game screen or the menu screens based on `UIState::currentScreen`.
- **[GUI_Game.h](GUI_Game.h)/.cpp** — Rendering and input handling for the active match (`currentScreen == 1`): board, hero sprite animations, attack effects, round badge.
- **[GUI_Menu.h](GUI_Menu.h)/.cpp** — Rendering and input handling for every non-gameplay screen (main menu, settings, load/save, hero select, help, credits) via numeric `currentScreen` states.

### Screen state machine

`UIState::currentScreen` is the global navigation switch, checked across `GUI.cpp`, `GUI_Menu.cpp`, and `GUI_Game.cpp`:

| Value | Screen |
|---|---|
| 0 | Main menu |
| 1 | In-game (board/match) |
| 2 | Settings |
| 3 | Help |
| 5 | Load game |
| 6 | Save game (entered from in-game via `L`) |
| 7 | Legacy P1/P2 name entry (superseded by screen 8's flow; still reachable via `case 3` in the main menu switch, which is currently a no-op) |
| 8 | Mode select + hero/name select (pre-match setup) |

When adding a new screen, follow the existing pattern: add a numeric branch in both `UpdateMenuScreens` and `DrawMenuScreens` (or `GUI_Game.cpp` if it's gameplay-related), and transition into/out of it by assigning `ui.currentScreen` directly — there is no enum, just `int` literals.

### Rendering pipeline

`main.cpp` always renders to a fixed `1920x1080` `RenderTexture2D`, then blits that texture to the actual (resizable) window scaled to preserve 16:9 aspect ratio with letterboxing. This means **all draw/layout code in `GUI*.cpp` should use the fixed 1920x1080 coordinate space**, never `GetScreenWidth()/GetScreenHeight()` directly for game content positioning (mouse input is already remapped into this space via `SetMouseOffset`/`SetMouseScale` in `main.cpp`).

### Hero / animation system

Each of the 6 heroes (index 0 = `black_knight`, hidden/unused; 1–5 = selectable) has five `CharAnim` sprite-sheet animations (`idle`, `attack_s1/s2/s3`, `death`) plus a static icon (used as the board piece) and an attack-effect sprite sheet, all loaded once in `InitGUI` (`GUI.cpp`) and arrays-indexed by hero asset index — not by player number. `HERO_MAP` in [GUI.h](GUI.h) maps a UI selection index (0–4, what the player picks) to the asset array index (1–5). When a match starts or restarts, call `ResetHeroAnimState` (inline helper in `GUI.h`) to clear stale animation frames/timers — every `currentScreen = 1` transition must do this or attack/death animations can appear "stuck" from a previous match.

### Bot AI

`EvaluatePosition` (LogicControl.cpp) scores an empty cell by summing pattern-based attack/defense scores (from lookup tables keyed by `[blocks][count]`) across the 4 line directions; `BotMove` picks uniformly at random among all empty cells tied for the highest score. This is a one-ply heuristic — it does not search ahead. Bot-vs-bot mode just calls `BotMove` for both sides on alternating turns.

`GetLineStatus` (exported in `LogicControl.h`) scans one direction from a cell and returns `count` (consecutive marks of the target player) and `blocks` (how many ends are blocked by an edge or opponent mark). Both the AI evaluator and `CheckBlockVariant` in `GUI_Game.cpp` rely on it; its semantics are important: it does **not** include the queried cell itself in `count`.

### Attack variant logic

Each placed piece triggers one of three animations, chosen in `UpdateGUIGame` (GUI_Game.cpp) immediately after `MakeMove`:
- `attack_s1` — normal move (default)
- `attack_s2` — the move blocks an opponent sequence (`CheckBlockVariant` returns true: ≥3 opponent marks in a row, or 4 with both ends closed)
- `attack_s3` — the move wins the round (or draws)

The win/draw case defers the real `matchStatus` reveal: `game.matchStatus` is stored in `ui.pendingWinStatus`, temporarily reset to 0 (so the board stays in-play during the animation), and restored only once `attack_s3` finishes and `triggerDeath` has run the loser's death animation.

### Round lifecycle

`InitGame` (LogicControl.cpp) — full reset: clears board, zeros both players' win/loss/step counts, sets `roundCount = 1`. Call once at startup.

`ResetRound` — next-round reset: clears board and step counts, swaps starting player (`isPlayer1Turn = !isPlayer1Turn`), increments `roundCount`. Win/loss records are preserved. Always pair with `ResetHeroAnimState(ui)` (see Hero / animation system above).

### Hero roster and asset convention

The 5 selectable heroes (UI index 0–4, asset index 1–5 via `HERO_MAP`):

| UI idx | Asset idx | Folder |
|---|---|---|
| 0 | 1 | `black_knight` |
| 1 | 2 | `fire_knight` |
| 2 | 3 | `green_archer` |
| 3 | 4 | `metal_blade` |
| 4 | 5 | `water_mage` |
| — | — | `wind_assassin` (asset idx 5 in some builds; check `GUI.cpp` load order) |

Each hero folder under `assets/Character/<name>/` must contain exactly: `idle.png`, `attack_s1.png`, `attack_s2.png`, `attack_s3.png`, `attack_effect.png`, `attack_icon.png`, `death.png`, `attack_sound.wav`. The load order in `InitGUI` determines the array index — add a new hero by appending to that loop, not by inserting.

### Persistence

Saves are raw binary dumps of the whole `GameState` struct (`fwrite(&game, sizeof(GameState), 1, file)`) to `save_<slot>.bin` in the working directory — not portable across struct layout changes, no versioning. Any change to `DataStruct.h::GameState` invalidates existing save files silently (`fread` will just produce garbage/zeros for shifted fields).

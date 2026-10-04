# Rogue

A small terminal dungeon game written in C with ncurses. Explore connected rooms, avoid or fight monsters, and keep an eye on your character's status.

The project began as an adaptation of the [Bad Coding Habits Rogue tutorial series](https://www.youtube.com/playlist?list=PLkTXsX7igf8erbWGYT4iSAhpnJLJ0Nk5G) and its [example repository](https://github.com/wadsworj/rogue).

## Build and run

Install GCC, Make, and the ncurses development library, then run:

```sh
make
./rogue
```

Or build and launch with `make run`. The terminal should be at least 52 columns wide and 22 rows tall. The menu uses ncurses colors when the terminal supports them.

## Main menu

- Use the Up/Down arrow keys or `w`/`s` to select an option.
- Press Enter to choose **Start Game** or **End Game**.
- Press `q` or Escape to exit from the menu.

## Game controls

| Key | Action |
| --- | --- |
| `w` | Move up |
| `a` | Move left |
| `s` | Move down |
| `d` | Move right |
| `h` | Open the help screen; this does not advance the turn |
| `q` | Return to the main menu |

Move into a monster to begin combat. The faster fighter attacks first; if speeds tie, the player attacks first. Defense absorbs damage before health and is depleted as it absorbs hits. Defeating a monster ends its retaliation for that exchange.

Monsters wander within their rooms until the player enters the same room, then pursue the player while staying clear of the doors.

## Status bar

The bottom bar displays the current level, health, attack, defense, experience, and gold.

## Map symbols

| Symbol | Meaning |
| --- | --- |
| `@` | Player |
| `-`, `\|` | Room walls |
| `.` | Room floor |
| `#` | Hallway |
| `+` | Door |
| `X`, `G`, `T`, `D` | Monsters |

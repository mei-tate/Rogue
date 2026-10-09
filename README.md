# The Hollow Crown

A terminal roguelike about reclaiming a lost crown from six dangerous floors below an abandoned road. Explore randomized rooms, collect relics, grow stronger, and survive the Ash Dragon.

## Build and run

Install GCC, Make, and the ncurses development library, then run:

```sh
make
./rogue
```

The terminal should be at least 52 columns wide and 22 rows tall. The game uses ncurses colors when the terminal supports them.

## Controls

| Key | Action |
| --- | --- |
| `W A S D` or arrow keys | Move |
| `I` | Open your pack; press an item number to use it |
| `R` | Rest to recover one mana; enemies take a turn |
| `H` | Open the field guide |
| `Q` | Return to the main menu |

Walking into a monster opens a battle window:

| Key | Action |
| --- | --- |
| `A` | Attack with your weapon |
| `S` | Arcane strike; costs 2 mana and deals extra damage |
| `P` | Drink a healing draught |
| `F` or `Esc` | Try to flee |

## Items

| Symbol | Item | Effect |
| --- | --- | --- |
| `!` | Healing draught | Restores up to 10 health |
| `?` | Aether vial | Restores up to 4 mana |
| `)` | Whetstone | Permanently raises attack |
| `]` | Iron sigil | Permanently raises defense |
| `$` | Gold | Adds directly to your purse |

The pack holds ten carried items. Gold does not use pack space. Defeating enemies grants gold and experience; gaining a rank restores health and mana and raises your attack.

## The descent

Each floor has randomly placed, connected rooms and separated hallways. Search for supplies, defeat every monster, then descend through the `O` that appears where the final monster fell. The enemies grow stronger through floor five; the Ash Dragon guards floor six. Clearing the last floor completes the run.

| Symbol | Meaning |
| --- | --- |
| `@` | You |
| `-`, `|` | Room walls |
| `.` | Room floor |
| `#` | Hallway |
| `+` | Door |
| `O` | Open stairway |
| `X`, `G`, `T`, `W`, `D` | Monsters |

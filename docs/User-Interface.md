# User Interface

This page lists all user interface additions, changes, fixes that are implemented in Vinifera.

## Miscellaneous

- Vinifera adds support for 8-bit (paletted and non-paletted) PCX and 8-bit PNG cameos. This system auto-detects and prioritises the PNG or PCX file if found, no additional settings are required.
- Vinifera thickens the waypoint and rally point lines and adds stroke/outline to the waypoint number.
- Vinifera adds a "Load Game" button to the retry dialog shown after a failed mission.
- Vinifera changes the game to save screenshots as a PNG file instead of PCX file. In addition to this, it also changes the filename format to be unique. Instead of writing `SCRN[0000-9999].PNG`, the game now writes `SCRN_[date-time].PNG` (example, `SCRN_02-06-2021_12-51-40.PNG`).

## Audio

## Hotkey Commands

### `[ ]` Place Building

- Enters the manual placement mode when a building is complete and pending on the sidebar. Defaults to `Z`.

### `[ ]` Toggle Special Timers

- Toggles the visibility of Super Weapon timers. Defaults to `<none>`.

### `[ ]` Repeat Last Building

- Queue the last structure that was built. Defaults to `Ctrl` + `Z`.

### `[ ]` Repeat Last Infantry

- Queue the last infantry that was built. Defaults to `<none>`.

### `[ ]` Repeat Last Unit

- Queue the last vehicle that was built. Defaults to `<none>`.

### `[ ]` Repeat Last Aircraft

- Queue the last aircraft that was built. Defaults to `<none>`.

### `[ ]` Jump Camera West

- Jump the tactical map camera to the west edge of the map. Defaults to `Ctrl` + `Left Arrow`.

### `[ ]` Jump Camera East

- Jump the tactical map camera to the east edge of the map. Defaults to `Ctrl` + `Right Arrow`.

### `[ ]` Jump Camera North

- Jump the tactical map camera to the north edge of the map. Defaults to `Ctrl` + `Up Arrow`.

### `[ ]` Jump Camera South

- Jump the tactical map camera to the north edge of the map. Defaults to `Ctrl` + `Down Arrow`.

### `[ ]` Scroll North-East

- Scroll the camera North-East. Defaults to `<none>`.

### `[ ]` Scroll South-East

- Scroll the camera South-East. Defaults to `<none>`.

### `[ ]` Scroll South-West

- Scroll the camera South-West. Defaults to `<none>`.

### `[ ]` Scroll North-West

- Scroll the camera North-West. Defaults to `<none>`.

### `[ ]` Previous Track

- Plays the previous music track. Defaults to `[`.

### `[ ]` Next Track

- Plays the next music track. Defaults to `]`.

![image](https://user-images.githubusercontent.com/73803386/123566309-4ade4600-d7b7-11eb-9b77-5c9de7959822.png)

## Dropship Loadout

- The Tiberian Sun Map theme is now played on the Dropship Loadout screen (`DSHPLOAD` can be defined in THEME.INI to customise this.)
- Help text is now shown on the screen to aid the user.

![image](https://user-images.githubusercontent.com/73803386/120932514-13b3d200-c6ee-11eb-9538-3f812323cb9f.png)

## Loading screen

- PNG images can be used as an alternative to PCX images. This new system scans for the requested filename with the .PNG extension and use that if found, otherwise it will fall back to scanning and load the .PCX file.
```{note}
This system only supports 8-bit PNG. All other formats such as Greyscale, Paletted, Alpha and 16-bit are not supported.
```
Attached is a set of the original loading screens with a minor edit and saved as PNG for testing;
[PNG_Loading_Screens.zip](https://github.com/Vinifera-Developers/Vinifera/files/7392707/PNG_Loading_Screens.zip)

## Sidebar / Battle UI

### Super Weapon Timers

- Super Weapon timers, similar to those found in Red Alert 2, can now be displayed on the tactical view. This is disabled by default and each relevant SuperWeaponType must have it enabled. Superweapons that are offline due to low power or are disabled via other purposes will not show.
In `RULES.INI`:
```ini
[SOMESUPERWEAPON]  ; SuperWeaponType
ShowTimer=no       ; boolean, when this superweapon is active, does its recharge timer display on the tactical view?
```

### Chat Improvements

- Vinifera adds a background behind the user typed messages that appear in-game to provide better readability.
- Additionally, Vinifera implements the system to echo the user's sent messages back to them in-game as a confirmation they were sent. This is an enhancement from Red Alert 2.
![image](https://user-images.githubusercontent.com/73803386/137031682-3f265d48-7f28-410f-bf0d-3260e24f1748.png)


In `UI.INI`:
```ini
[Ingame]
TextLabelOutline=yes                ; boolean, should the text be drawn with a black outline?
TextLabelBackgroundTransparency=50  ; unsigned integer, the transparency of the text background fill. Ranged between 0 and 100.
```

### Unit Health Bar

- Vinifera allows customizing the position of the heath bar.

In `UI.INI`:
```ini
[Ingame]
UnitHealthBarPos=-25,-16     ; Point2D, the draw position of the unit health bar
InfantryHealthBarPos=-24,-5  ; Point2D, the draw position of the infantry health bar
```

### Unit Pips

- The location of the control group number and veterancy pips can now be customized.

In `UI.INI`:
```ini
[Ingame]
UnitGroupNumberOffset=-4,-4            ; Point2D, the group number offset for units
InfantryGroupNumberOffset=-4,-4        ; Point2D, the group number offset for infantry
BuildingGroupNumberOffset=-4,-4        ; Point2D, the group number offset for buildings
AircraftGroupNumberOffset=-4,-4        ; Point2D, the group number offset for aircraft

UnitWithPipGroupNumberOffset=-4,-8     ; Point2D, the group number offset for units with pips
InfantryWithPipGroupNumberOffset=-4,-8 ; Point2D, the group number offset for infantry with pips
BuildingWithPipGroupNumberOffset=-4,-8 ; Point2D, the group number offset for buildings with pips
AircraftWithPipGroupNumberOffset=-4,-8 ; Point2D, the group number offset for aircraft with pips

UnitVeterancyPipOffset=10,6            ; Point2D, the veterancy pip offset for units
InfantryVeterancyPipOffset=5,4         ; Point2D, the veterancy pip offset for infantry
BuildingVeterancyPipOffset=10,6        ; Point2D, the veterancy pip offset for buildings
AircraftVeterancyPipOffset=10,6        ; Point2D, the veterancy pip offset for aircraft

UnitSpecialPipOffset=0,-8              ; Point2D, the special pip offset for units
InfantrySpecialPipOffset=0,-8          ; Point2D, the special pip offset for infantry
BuildingSpecialPipOffset=0,-8          ; Point2D, the special pip offset for buildings
AircraftSpecialPipOffset=0,-8          ; Point2D, the special pip offset for aircraft
```

- TS Patches changes some of the default values. Below are the values that match them:

In `UI.INI`:
```ini
[Ingame]
UnitGroupNumberOffset=-8,-33
InfantryGroupNumberOffset=-8,-33
BuildingGroupNumberOffset=-8,-33
AircraftGroupNumberOffset=-8,-33
UnitWithPipGroupNumberOffset=-8,-33
InfantryWithPipGroupNumberOffset=-8,-33
BuildingWithPipGroupNumberOffset=-8,-33
AircraftWithPipGroupNumberOffset=-8,-33
```

- You can also customize how many pips can be drawn per PipScale.

In `RULES.INI`:
```ini
[AudioVisual]
MaxPips=5,5,5,10,8  ; list of integers - Ammo, Tiberium, Passengers, Power, Charge.
```

## Tooltips

## Miscellaneous

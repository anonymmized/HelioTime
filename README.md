# HelioTime

**HelioTime** is a small **C** command-line tool that calculates **sunrise and sunset times** using **latitude/longitude** and prints a **weekly report** inside a clean ASCII/Unicode box.

It stores your coordinates in a config file so you don’t have to enter them every time.

---

## Features

- **Sunrise / Sunset** calculation
- Based on:
  - day of year (`day of year`)
  - latitude (`lat`)
  - longitude (`lon`)
  - time zone (roughly derived from longitude)
- **This week** report (7 days)
- Nice terminal output with box borders
- Cross-platform config path (**Linux/macOS + Windows**)

---

## Example output

Example (approximate style):

```text
┌───────────────────────────────────────────────┐
│              SUN REPORT (THIS WEEK)           │
│               New York (Example)              │
│                   Time Zone 3                 │
├───────────────────────────────────────────────┤
│ Day   Date        Sunrise   Sunset            │
│ ----  ----------  -------   ------            │
│ Wed   14.01.2026   08:52    16:25  < Current  │
│ Thu   15.01.2026   08:51    16:27             │
│ ...                                           │
└───────────────────────────────────────────────┘
```

---

## Build

### macOS / Linux

```bash
gcc heliotime.c calculations.c -o heliotime -lm
```

Run:

```bash
./heliotime
```

---

## Usage

### 1) Set location (save coordinates)

```bash
./heliotime --setloc --lat 55.75 --lon 37.62
```

You can also swap arguments:

```bash
./heliotime --setloc --lon 37.62 --lat 55.75
```

Valid ranges:
- `lat` must be in `[-90; 90]`
- `lon` must be in `[-180; 180]`

---

### 2) Print weekly report

```bash
./heliotime
```

The tool will:
- load `lat/lon` from the config file
- read the current date from your system
- print a weekly sunrise/sunset table

---

## Config file location

### Linux/macOS

```text
~/.config/sunweek/config
```

### Windows

```text
%APPDATA%\sunweek\config
```

Example config:

```text
#sunweek config
lat = 55.75
lon = 37.62
```

---

## Project files

- `heliotime.c`  
	  Main program: reads config, handles date logic, builds table lines, prints the box.

- `calculations.c`  
	  Math module: sunrise/sunset calculation (hour angle, longitude correction, equation of time).

- `calculations.h`  
	  Header file for the calculations module.

---

## Math (short version)

This project uses a simplified model:
- solar declination via `sin(gamma)` approximation
- standard horizon correction `h0 = -0.833°`
- Equation of Time (EOT) in minutes
- longitude correction relative to the time zone meridian

Note: Results may differ from services like Yandex/Google by a few minutes due to approximation.

---

## Limitations / TODO

- Time zone is currently approximated from longitude (`lon / 15`)
- No city name lookup yet
- No DST (daylight saving time) support
- Could be improved with a more accurate model (NOAA / SPA)

---

## License

Free educational project.  
Use and modify it however you want.


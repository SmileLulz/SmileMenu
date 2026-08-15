> This project was meant to be a personal project, but I'm sharing anyways. Therefore, I am not accepting any contributions. Thank you.
>
> Also, if newly installed or modified applications are not executing/launching/showing or not launching correctly, just run `smilemenu --dcache app` or manually delete the cache file (`rm ~/.cache/smilemenu/apps_cache.json`).


# ❤️ SmileMenu

**A modern and simple launcher for Linux desktops.**


### Features

- Native Qt/QML interface
- Wayland layer-shell support
- Dynamic window sizing
- Desktop application launcher
- Application category searching
- Recent applications
- Fuzzy searching
- Provider scripting system (`list` / `run`)
- JSON config and QML theming
- And many more...
- See [WIKI.md](https://codeberg.org/SmileLulz404/SmileMenu/src/branch/main/WIKI.md) for everything


# 🏞️ Screenshots

| ![Screenshot 1](https://codeberg.org/SmileLulz404/SmileMenu/raw/branch/main/0-Screenshots/0.png) | ![Screenshot 2](https://codeberg.org/SmileLulz404/SmileMenu/raw/branch/main/0-Screenshots/1.png) |
| ------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------ |


# ⭐ Usage

Read from [WIKI.md](https://codeberg.org/SmileLulz404/SmileMenu/src/branch/main/WIKI.md).


## 📌 Runtime Dependencies (Arch Linux)

- `qt6-base`
- `qt6-declarative`
- `layer-shell-qt`

```sh
sudo pacman -S --needed qt6-base qt6-declarative layer-shell-qt
```


# ⚙️ Build & Installation

**IMPORTANT:** All below guides are wrote for/in Arch Linux only; since I am using Arch.

### Running from source

Clone the repository:

```sh
git clone https://codeberg.org/SmileLulz404/SmileMenu.git
cd SmileMenu
```

Build:

```sh
# Prepare
mkdir build && cd build

# Build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Or run the build script
../build.sh

# Rebuild
make clean
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Or run the rebuild script
../rebuild.sh
```

Run:

```sh
# Start the daemon
./smilemenu --daemon

# Use
./smilemenu
```

Install with `make` (optional):

```sh
sudo make install
```

### Build for Arch

Dependencies:

- `cmake`
- `make`
- `gcc`

```sh
sudo pacman -S --needed cmake make gcc
```

Build and install:

```sh
makepkg -si
```

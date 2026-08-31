<div align="center">
  <h1>🚀 SmileMenu 🚀</h1>
  <h3>A fast and lightweight application launcher and utility menu</h3>
  <p>SmileMenu is a lightweight app launcher and utility menu for Wayland desktops. It's highly themable, supports custom scripting, and much more.</p>
  <br></br>
  <img alt="Screenshot" src="https://raw.githubusercontent.com/SmileLulz/SmileMenu/refs/heads/main/0-Screenshots/0.png" width="80%" />
  <br></br>
  <h3><a href="https://github.com/SmileLulz/SmileMPlayer/blob/main/WIKI.md">Wiki</a>  &ensp;&ensp;  <a href="https://github.com/SmileLulz/SmileMPlayer/blob/main/CHANGELOG.md">Changelogs</a></h2>
</div>

<br></br>

> [!NOTE]
> This project was meant to be a personal project, but I'm feeling happy to share it with others. Therefore, I am not accepting contributions. Thank you. Hope you like this simple utility :)

<br></br>

# ✨ Features

- Native Qt/QML interface
- Fully customizable/themable via custom QML(s)
- Application category searching
- Shows recently used apps by score
- Fuzzy searching
- Custom scripting system (`list` / `run`)
- And much more...
- See [Wiki](https://github.com/SmileLulz/SmileMenu/wiki) for more help

**See [CHANGELOG.md](https://github.com/SmileLulz/SmileMenu/blob/main/CHANGELOG.md) for latest update information.**

<br></br>

# 🔗 Dependencies

### Arch Linux

```sh
sudo pacman -S --needed qt6-base qt6-declarative layer-shell-qt
```

### Debian

```sh
sudo apt install qt6-base-dev qt6-declarative-dev layer-shell-qt
```

### Fedora

```sh
sudo dnf install qt6-qtbase qt6-qtdeclarative layer-shell-qt
```

<br></br>

# 📥 Install

### Arch Linux

1. Download the latest `.tar.zst` file from the [Releases](https://github.com/SmileLulz/SmileMenu/releases) page
2. Install it with `sudo pacman -U /path/to/smilemenu-*.pkg.tar.zst`

### Debian

1. Download the latest `.deb` file from the [Releases](https://github.com/SmileLulz/SmileMenu/releases) page
2. Install it with `sudo apt install /path/to/smilemenu-*.deb`

### Fedora

1. Download the latest `.rpm` file from the [Releases](https://github.com/SmileLulz/SmileMenu/releases) page
2. Install it with `sudo dnf install /path/to/smilemenu-*.rpm`

<br></br>

# 📦 Build by yourself

### Clone the repository

```sh
git clone https://github.com/SmileLulz/SmileMPlayer.git && cd SmileMPlayer
```

### Running from source

_For test and debugging purposes._

1. Build:

```sh
# Build
./build.sh

# Or rebuild
./rebuild.sh
```

2. Run directly:

```sh
# Start the daemon
./build/smilemenu --daemon

# Run
./build/smilemenu
```

### Build for Arch Linux

1. Install build dependencies:

```sh
sudo pacman -S --needed cmake gcc
```

2. Build:

```sh
# Build & install
makepkg -si

# Or build first, then install manually/optionally
makepkg -s
sudo pacman -U smilemenu-*.pkg.tar.zst
```

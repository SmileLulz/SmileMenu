<div align="center">
  <h1>🚀 SmileMenu 🚀</h1>
  <h3>A fast and lightweight application launcher and utility menu</h3>
  <p>SmileMenu is a lightweight app launcher and utility menu for Wayland desktops. It's highly themable, supports custom scripting, and much more.</p>
  <br></br>
  <img alt="Screenshot" src="https://raw.githubusercontent.com/SmileLulz/SmileMenu/refs/heads/main/Screenshots/0.png" width="80%" />
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
- Provider scripting system (`list` / `run`)
- And much more...
- See [Wiki](https://github.com/SmileLulz/SmileMenu/wiki) for more help

**See [CHANGELOG.md](https://github.com/SmileLulz/SmileMenu/blob/main/CHANGELOG.md) for latest update information.**

<br></br>

# 🔗 Dependencies

### Arch Linux

- `qt6-base`
- `qt6-declarative`
- `layer-shell-qt`

### Debian

- `qt6-base-dev`
- `qt6-declarative-dev`
- `layer-shell-qt`

### Fedora

- `qt6-qtbase`
- `qt6-qtdeclarative`
- `layer-shell-qt`

<br></br>

# 📥 Install

### Automatic installation

This will install the latest release on any Arch/Debain/Fedora based systems.

Install:

```bash
curl -fsSL https://raw.githubusercontent.com/SmileLulz/SmileMenu/main/install.sh | bash
```

Or, if you want to inspect the installation script first:

```bash
# Download the script
curl -fsSL https://raw.githubusercontent.com/SmileLulz/SmileMenu/main/install.sh -o install.sh

# Inspect
less install.sh

# Then you can install with the downloaded script
bash install.sh
```

### Manual installation

**Arch Linux:**

1. Download the latest `.tar.zst` file from the [Releases](https://github.com/SmileLulz/SmileMenu/releases) page
2. Install it with `sudo pacman -U /path/to/smilemenu-*.pkg.tar.zst`

**Debian:**

1. Download the latest `.deb` file from the [Releases](https://github.com/SmileLulz/SmileMenu/releases) page
2. Install it with `sudo apt install /path/to/smilemenu-*.deb`

**Fedora:**

1. Download the latest `.rpm` file from the [Releases](https://github.com/SmileLulz/SmileMenu/releases) page
2. Install it with `sudo dnf install /path/to/smilemenu-*.rpm`

<br></br>

# 📦 Build by yourself

### Clone the repository

```sh
git clone https://github.com/SmileLulz/SmileMPlayer.git && cd SmileMPlayer
```

### Running for testing/debugging

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

### Build for Debian

> [!WARNING]
> Debian build is unstable due to older version of LayerShellQt it ships.
>
> If you want to build anyway, then comment out the `LayerShellQt.Window.wantsToBeOnActiveScreen: true` line in `qml/Main.qml`. But keep in mind that it would exclude a feature from SmileMenu.

1. Install build dependencies:

```sh
sudo apt install \
    build-essential \
    cmake \
    g++ \
    qt6-base-dev \
    qt6-declarative-dev \
    liblayershellqtinterface-dev \
    layer-shell-qt \
    qml6-module-org-kde-layershell
```

2. Build:

```sh
dpkg-buildpackage -b -us -uc
```

3. Install:

```sh
sudo apt install ../smilemenu_*.deb
```

### Build for Fedora

Install build dependencies:

```sh
sudo dnf install \
    cmake \
    gcc-c++ \
    qt6-qtbase-devel \
    qt6-qtdeclarative-devel \
    layer-shell-qt-devel
```

Create required directories:

```sh
mkdir -p rpm/{BUILD,BUILDROOT,RPMS,SOURCES,SRPMS}
```

> [!NOTE]
> Now, you have two options to build:
> 
> Option 1: Use the release source archive.
>
> Option 2: Use a specific commit source archive.

**Option 1:**

_This is for most users who just want to build the release version._

> Make sure to `git checkout` to that commit tag first (i.e. `git checkout v4.4`).

1. Download the release source archive:

```sh
spectool --define "_topdir $PWD/rpm" rpm/SPECS/smilemenu.spec
```

2. Build:

```sh
rpmbuild --define "_topdir $PWD/rpm" -ba rpm/SPECS/smilemenu.spec
```

3. Install:

```sh
sudo dnf install rpm/RPMS/x86_64/smilemenu-*.rpm
```

**Option 2:**

_This is mostly for local testing or building from a specific commit._

> Make sure to `git checkout` to that commit first (e.g. `git checkout <commit_hash_or_tag>`).

1. Create the source archive from current commit (replace `X.Y` with the actual version):

```sh
git archive --format=tar.gz --prefix=SmileMenu-X.Y/ HEAD > rpm/SOURCES/vX.Y.tar.gz
```

2. Build:

```sh
rpmbuild --define "_topdir $PWD/rpm" -ba rpm/SPECS/smilemenu.spec
```

3. Install:

```sh
sudo dnf install rpm/RPMS/x86_64/smilemenu-*.rpm
```

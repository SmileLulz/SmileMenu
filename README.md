# ❤️ SmileMenu

**A modern and simple launcher for Linux desktops.**

SmileMenu is a modern & lightweight GUI launcher built with Python and Qt (PySide6) for Linux desktops.


> This project was meant to be a personal project, but I'm sharing anyways. Therefore, I am not accepting any contributions. Thank you.
> Also, if newly installed or modified applications are not executing/launching or not launching correctly, just run `smilemenu --dcache app` or manually delete the cache file (`rm ~/.cache/smilemenu/apps_cache.json`).


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

<div style="display: flex; gap: 10px; align-items: center;">
  <img src="https://codeberg.org/SmileLulz404/SmileMenu/raw/branch/main/0-Screenshots/0.png" alt="Screenshot 1" style="max-width: 100%; height: auto;">
  <img src="https://codeberg.org/SmileLulz404/SmileMenu/raw/branch/main/0-Screenshots/0.png" alt="Screenshot 2" style="max-width: 100%; height: auto;">
</div>


# ⭐ Usage

Read from [WIKI.md](https://codeberg.org/SmileLulz404/SmileMenu/src/branch/main/WIKI.md).


## 📌 Runtime Dependencies (Arch Linux)

- `python`
- `qt6-base`
- `qt6-declarative`
- `layer-shell-qt`
- `shiboken6`
- `pyside6-essentials` or `pyside6`

```sh
sudo pacman -S --needed python pyside6 qt6-base qt6-declarative shiboken6 layer-shell-qt
```


# ⚙️ Build & Installation

**IMPORTANT:** All below guides are wrote for/in Arch Linux only; since I am using Arch, I can't guarentee that other guides will work correctly.

### Running from source

Clone the repository:

```sh
git clone https://codeberg.org/SmileLulz/SmileMenu.git
cd SmileMenu
```

Run:

```sh
# Start the daemon
python -m smilemenu --daemon # or --daemon &

# Use
python -m smilemenu
```

### Build for any distro (python pip)

```sh
# Build the package
python -m build

# Install locally
python -m pip install .

# For development
python -m pip install -e .
```

### Build for Arch

Dependencies:

- `python-build`
- `python-installer`
- `python-wheel`
- `python-setuptools`

```sh
sudo pacman -S --needed python-build python-installer python-wheel python-setuptools
```

Build and install:

```sh
makepkg -csfCi
```

### Build for Debian

**NOTE:** Not confirmed to be work; since I am not using Debian.

Dependencies:

The Debian package can be built using a Debian container. This avoids installing Debian packaging tools on Arch or any non-Debian distro.

- `podman` (or Docker)

```sh
sudo pacman -S podman
```

Build and run the container from the SmileMenu source directory:

```sh
podman run -it --rm -v "$PWD:/src" -w /src debian:stable bash
```

Install dependencies inside the container:

```sh
apt update
```

```sh
apt install -y \
    build-essential \
    debhelper \
    devscripts \
    python3 \
    python3-build \
    python3-installer \
    python3-setuptools \
    python3-wheel
```

Build the package:

```sh
dpkg-buildpackage -us -uc -tc && mv ../smilemenu_*.deb .
```

Install locally inside the container if you want to:

```sh
apt install ./smilemenu_x.x.x-1_all.deb
```

Exit the container using `exit` command after you're done.

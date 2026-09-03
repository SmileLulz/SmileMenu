# Changelogs

### v4.7

- Fixed `Theme.qml` not added in the `qmldir`

### v4.6

- Fixed frontend issues
- Added `Theme.qml` to make the theming easy (coloring and sizing only)

### v4.5

- Fixed persistent LayerShell activation from the daemon workspace affecting application launches
- Split the QML files into modules
- `--gen-theme` now extracts all the default QML theme files
- Added optional cyclic keyboard navigation with `cycle: false` by default in config
- Replaced provider `path` field mode with explicit `file` mode
- Provider `file` fields now fall back to an empty value for non-file output

### v4.4

- Fixed debian build support
- Fixed rounded corners not clipping

### v4.3

- Fixed provider system bugs
- Added `--version, -v`
- Added short forms for every options/args
- Fixed icon preview style

### v4.2

- Updated license
- Harden daemon IPC and provider lifecycle
- Fixed bundled QML resource loading and theme reloads
- Fixed application cache invalidation and atomic state writes
- Bound provider output and clipboard display text
- Fixed Qt6/LayerShell Debian build metadata
- Removed --help-all argument
- Added proper builds support for different Linux distributions

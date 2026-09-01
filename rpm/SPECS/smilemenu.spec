Name:           smilemenu
Version:        4.5
Release:        1%{?dist}
Summary:        A fast and lightweight application launcher and utility menu

License:        GPL-3.0-only
URL:            https://github.com/SmileLulz/SmileMenu
Source0:        https://github.com/SmileLulz/SmileMenu/archive/refs/tags/v%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  layer-shell-qt-devel

Requires:       layer-shell-qt
Requires:       qt6-qtdeclarative

%global debug_package %{nil}

%description
SmileMenu is a fast and lightweight application launcher and utility menu.

%prep
%autosetup -n SmileMenu-%{version}

%build
%cmake -DCMAKE_BUILD_TYPE=Release
%cmake_build

%install
%cmake_install

install -Dpm0644 LICENSE \
    %{buildroot}%{_datadir}/licenses/smilemenu/LICENSE

install -Dpm0644 data/metainfo/io.github.SmileLulz.SmileMenu.metainfo.xml \
    %{buildroot}%{_metainfodir}/io.github.SmileLulz.SmileMenu.metainfo.xml

%files
%license %{_datadir}/licenses/smilemenu/LICENSE
%{_bindir}/smilemenu
%{_metainfodir}/io.github.SmileLulz.SmileMenu.metainfo.xml

%changelog
* Wed Sep 02 2026 SmileLulz - 4.5-1
- Fixed persistent LayerShell activation from the daemon workspace affecting application launches
- Split the QML files into modules
- `--gen-theme` now extracts all the default QML theme files
- Added optional cyclic keyboard navigation with `cycle: false` by default in config
- Replaced provider `path` field mode with explicit `file` mode
- Provider `file` fields now fall back to an empty value for non-file output

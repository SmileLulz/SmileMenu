# Maintainer: SmileLulz <SmileLulz@noreply.codeberg.org>

pkgname=smilemenu
pkgver=1.4.8
pkgrel=1
pkgdesc="A modern and lightweight Qt launcher"
arch=('any')
url="https://codeberg.org/SmileLulz/SmileMenu"
license=('MIT')

depends=(
    'python'
    'pyside6'
    'qt6-base'
    'qt6-declarative'
    'shiboken6'
    'layer-shell-qt'
)

makedepends=(
    'python-build'
    'python-installer'
    'python-wheel'
    'python-setuptools'
)

source=()
sha256sums=()

build() {
    cd "$startdir"

    python -m build \
        --wheel \
        --no-isolation
}

package() {
    cd "$startdir"

    python -m installer \
        --destdir="$pkgdir" \
        dist/*.whl

    install -Dm644 \
        data/icons/smilemenu.png \
        "$pkgdir/usr/share/icons/hicolor/256x256/apps/smilemenu.png"

    install -Dm644 \
        data/smilemenu.desktop \
        "$pkgdir/usr/share/applications/smilemenu.desktop"

    install -Dm644 \
        LICENSE \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
# Maintainer: SmileLulz <SmileLulz404@noreply.codeberg.org>

pkgname=smilemenu
pkgver=3.0.0
pkgrel=1
pkgdesc="A modern and simple launcher for Linux"
arch=('any')
url="https://codeberg.org/SmileLulz404/SmileMenu"
license=('MIT')

depends=(
    'python'
    'qt6-base'
    'qt6-declarative'
    'layer-shell-qt'
    'shiboken6'
    'pyside6'
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
        LICENSE \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

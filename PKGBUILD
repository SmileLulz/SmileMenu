# Maintainer: SmileLulz404 <SmileLulz404@noreply.codeberg.org>

pkgname=smilemenu
pkgver=4.0.1
pkgrel=1
pkgdesc="A modern and simple launcher for Linux"
arch=('x86_64')
url="https://codeberg.org/SmileLulz404/SmileMenu"
license=('MIT')

depends=(
    'qt6-base'
    'qt6-declarative'
    'layer-shell-qt'
)

makedepends=(
    'cmake'
    'make'
    'gcc'
)

source=()
sha256sums=()

build() {
    cmake -S "$startdir" -B "$startdir/build" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        # -DBUILD_TESTING=OFF

    cmake --build "$startdir/build"
}

package() {
    DESTDIR="$pkgdir" cmake --install "$startdir/build"

    install -Dm644 \
        "$startdir/LICENSE" \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

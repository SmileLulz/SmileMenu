# Maintainer: SmileLulz <SmileLulz404@noreply.codeberg.org>

pkgname=smilemenu
pkgver=4.0.0
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
    'qt6-base'
    'qt6-declarative'
    'layer-shell-qt'
)

source=()
sha256sums=()

build() {
    cd "$srcdir/$pkgname-$pkgver"

    mkdir -p build && cd build

    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_INSTALL_BINDIR=bin \
        -DBUILD_TESTING=OFF
    make
}

package() {
    cd "$srcdir/$pkgname-$pkgver/build"

    make DESTDIR="$pkgdir" install

    install -Dm644 \
        "$srcdir/$pkgname-$pkgver/LICENSE" \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

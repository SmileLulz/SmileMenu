# Maintainer: SmileLulz <SmileLulz@users.noreply.github.com>

pkgname=smilemenu
pkgver=4.0
pkgrel=1
pkgdesc="A fast and lightweight application launcher and utility menu"
arch=('x86_64')
url="https://github.com/SmileLulz/SmileMenu"
license=('GPL-3.0-only')

depends=(
    'qt6-base'
    'qt6-declarative'
    'layer-shell-qt'
)

makedepends=(
    'cmake'
    'gcc'
)

source=()
sha256sums=()

build() {
    cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr

    cmake --build build --parallel
}

package() {
    DESTDIR="$pkgdir" cmake --install build

    install -Dm644 LICENSE \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"

    install -Dm644 data/metainfo/io.github.SmileLulz.SmileMenu.metainfo.xml \
        "$pkgdir/usr/share/metainfo/io.github.SmileLulz.SmileMenu.metainfo.xml"
}

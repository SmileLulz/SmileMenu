# Maintainer: SmileLulz <SmileLulz@users.noreply.github.com>

pkgname=smilemenu
pkgver=4.4
pkgrel=1
pkgdesc="A fast and lightweight application launcher and utility menu"
arch=('x86_64')
url="https://github.com/SmileLulz/SmileMenu"
license=('GPL-3.0-only')
options=('!debug')

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
    cmake -S "$startdir" -B "$srcdir/build" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr

    cmake --build "$srcdir/build" --parallel
}

package() {
    DESTDIR="$pkgdir" cmake --install "$srcdir/build"

    install -Dm644 "$startdir/LICENSE" \
        "$pkgdir/usr/share/licenses/$pkgname/LICENSE"

    install -Dm644 \
        "$startdir/data/metainfo/io.github.SmileLulz.SmileMenu.metainfo.xml" \
        "$pkgdir/usr/share/metainfo/io.github.SmileLulz.SmileMenu.metainfo.xml"
}

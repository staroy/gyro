CURRENT_DIR=$PWD
PARENT_DIR="$(dirname "$PWD")"
TMP_DIR=$PARENT_DIR/tmp
BUILD_DIR=$TMP_DIR/gyro-release
SOURCE_DIR=$PWD
OUT_DIR=$PARENT_DIR/out-release

mkdir -p $TMP_DIR && mkdir -p $BUILD_DIR \
 && cd $BUILD_DIR \
 && cmake $SOURCE_DIR \
 -DCMAKE_BUILD_TYPE=Release \
 -DINSTALL_VENDORED_LIBUNBOUND=ON \
 -DUSE_DEVICE_TREZOR=OFF \
 -DUSE_DEVICE_TREZOR_LIBUSB=OFF \
 -DBUILD_GUI_DEPS=ON \
 -DMANUAL_SUBMODULES=1 \
 -DBUILD_TESTS=OFF \
 -DCMAKE_INSTALL_PREFIX=$OUT_DIR \
 && make -j3 && make install

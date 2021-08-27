CURRENT_DIR=$PWD
PARENT_DIR="$(dirname "$PWD")"
TMP_DIR=$PARENT_DIR/tmp
BUILD_DIR=$TMP_DIR/gyro-debug
SOURCE_DIR=$PWD
OUT_DIR=$PARENT_DIR/out-debug
PREFIX_PATH=/home/dumo/opt/Qt/6.0.3/gcc_64

mkdir -p $TMP_DIR && mkdir -p $BUILD_DIR \
 && cd $BUILD_DIR \
 && cmake $SOURCE_DIR \
 -DCMAKE_BUILD_TYPE=Debug \
 -DBOOST_ROOT=$OUT_DIR/include \
 -DBOOST_INCLUDEDIR=$OUT_DIR/include \
 -DBOOST_LIBRARYDIR=$OUT_DIR/lib/ \
 -DBUILD_GUI_DEPS=ON \
 -DMANUAL_SUBMODULES=1 \
 -DBUILD_TESTS=OFF \
 -DCMAKE_INSTALL_PREFIX=$OUT_DIR \
 -DCMAKE_PREFIX_PATH=$PREFIX_PATH \
 && make -j3 install

rm $OUT_DIR/lib/libeasylogging.so
rm $OUT_DIR/lib/librandomx.so
rm $OUT_DIR/lib/libunbound.so
rm $OUT_DIR/lib/libblockchain_db.so
rm $OUT_DIR/lib/libcheckpoints.so
rm $OUT_DIR/lib/libcommon.so
rm $OUT_DIR/lib/libcncrypto.so
rm $OUT_DIR/lib/libcryptonote_basic.so
rm $OUT_DIR/lib/libcryptonote_core.so
rm $OUT_DIR/lib/libcryptonote_protocol.so
rm $OUT_DIR/lib/libhardforks.so
rm $OUT_DIR/lib/libdaemonizer.so
rm $OUT_DIR/lib/libdevice.so
rm $OUT_DIR/lib/libdevice_trezor.so
rm $OUT_DIR/lib/libmnemonics.so
rm $OUT_DIR/lib/libmultisig.so
# rm $OUT_DIR/lib/libjp.so
rm $OUT_DIR/lib/libnet.so
rm $OUT_DIR/lib/libp2p.so
rm $OUT_DIR/lib/libringct_basic.so
rm $OUT_DIR/lib/libringct.so
rm $OUT_DIR/lib/libdaemon_messages.so  
rm $OUT_DIR/lib/libdaemon_rpc_server.so
# rm $OUT_DIR/lib/libdaemon_jp_server.so
rm $OUT_DIR/lib/librpc_base.so         
rm $OUT_DIR/lib/librpc.so              
rm $OUT_DIR/lib/libserialization.so
rm $OUT_DIR/lib/libwallet.so
rm $OUT_DIR/lib/libversion.so
rm $OUT_DIR/lib/liblmdb.so

cp -f $BUILD_DIR/external/easylogging++/libeasylogging.so            $OUT_DIR/lib
cp -f $BUILD_DIR/external/randomx/librandomx.so                      $OUT_DIR/lib
cp -f $BUILD_DIR/external/unbound/libunbound.so                      $OUT_DIR/lib
cp -f $BUILD_DIR/src/blockchain_db/libblockchain_db.so               $OUT_DIR/lib
cp -f $BUILD_DIR/src/checkpoints/libcheckpoints.so                   $OUT_DIR/lib
cp -f $BUILD_DIR/src/common/libcommon.so                             $OUT_DIR/lib
cp -f $BUILD_DIR/src/crypto/libcncrypto.so                           $OUT_DIR/lib
cp -f $BUILD_DIR/src/cryptonote_basic/libcryptonote_basic.so         $OUT_DIR/lib
cp -f $BUILD_DIR/src/cryptonote_core/libcryptonote_core.so           $OUT_DIR/lib
cp -f $BUILD_DIR/src/cryptonote_protocol/libcryptonote_protocol.so   $OUT_DIR/lib
cp -f $BUILD_DIR/src/hardforks/libhardforks.so                       $OUT_DIR/lib
cp -f $BUILD_DIR/src/daemonizer/libdaemonizer.so                     $OUT_DIR/lib
cp -f $BUILD_DIR/src/device/libdevice.so                             $OUT_DIR/lib
cp -f $BUILD_DIR/src/device_trezor/libdevice_trezor.so               $OUT_DIR/lib
cp -f $BUILD_DIR/src/mnemonics/libmnemonics.so                       $OUT_DIR/lib
cp -f $BUILD_DIR/src/multisig/libmultisig.so                         $OUT_DIR/lib
# cp -f $BUILD_DIR/src/jp/libjp.so                                     $OUT_DIR/lib
cp -f $BUILD_DIR/src/net/libnet.so                                   $OUT_DIR/lib
cp -f $BUILD_DIR/src/p2p/libp2p.so                                   $OUT_DIR/lib
cp -f $BUILD_DIR/src/ringct/libringct_basic.so                       $OUT_DIR/lib
cp -f $BUILD_DIR/src/ringct/libringct.so                             $OUT_DIR/lib
cp -f $BUILD_DIR/src/rpc/libdaemon_messages.so                       $OUT_DIR/lib
cp -f $BUILD_DIR/src/rpc/libdaemon_rpc_server.so                     $OUT_DIR/lib
# cp -f $BUILD_DIR/src/rpc/libdaemon_jp_server.so                      $OUT_DIR/lib
cp -f $BUILD_DIR/src/rpc/librpc_base.so                              $OUT_DIR/lib
cp -f $BUILD_DIR/src/rpc/librpc.so                                   $OUT_DIR/lib
cp -f $BUILD_DIR/src/serialization/libserialization.so               $OUT_DIR/lib
cp -f $BUILD_DIR/src/wallet/libwallet.so                             $OUT_DIR/lib
cp -f $BUILD_DIR/src/libversion.so                                   $OUT_DIR/lib
cp -f $BUILD_DIR/external/db_drivers/liblmdb/liblmdb.so              $OUT_DIR/lib

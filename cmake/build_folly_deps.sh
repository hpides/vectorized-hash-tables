DEPS_DIR=$1
MAKE_FLAGS=-j8

cd $DEPS_DIR
echo "Building double-conversion"
git clone https://github.com/google/double-conversion.git
cd double-conversion
cmake . -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=$DEPS_DIR/opt
make $MAKE_FLAGS
make install

cd $DEPS_DIR
echo "Building gflags"
git clone https://github.com/gflags/gflags.git
cd gflags
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=$DEPS_DIR/opt
make $MAKE_FLAGS
make install

cd $DEPS_DIR
echo "Building google log"
git clone https://github.com/google/glog.git
cd glog
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$DEPS_DIR/opt
cmake --build build $MAKE_FLAGS
cmake --build build --target install

rm -rf $DEPS_DIR/double-conversion
rm -rf $DEPS_DIR/gflags
rm -rf $DEPS_DIR/glog

cd $DEPS_DIR

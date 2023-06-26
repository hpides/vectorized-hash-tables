sed -i '/--with-python/d' $1/build/fbcode_builder/manifests/boost
sed -i '35 a set(FOLLY_BOOST_LINK_STATIC ON)\n' $1/CMake/folly-deps.cmake
sed -i 's/Boost 1.51.0/Boost 1.78.0/g' ${1}/CMake/folly-deps.cmake
sed -i 's/cmakebootstrap/nop/g' ${1}/build/fbcode_builder/manifests/cmake
sed -i '45d;46d' $1/build/fbcode_builder/manifests/cmake

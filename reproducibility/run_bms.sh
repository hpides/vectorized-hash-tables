pushd $PWD

PARENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." >/dev/null 2>&1 && pwd )"
cd $PARENT_DIR

vendor=$(lscpu | awk '/Vendor ID/{print $3}')
arch=$(lscpu | awk '/Architecture/{print $2}')

if [[ "$vendor" == "GenuineIntel" ]]; then
   NUMAPREF='numactl -C 0 -m 0'
elif [[ "$vendor" == "AuthenticAMD" ]]; then
   NUMAPREF='numactl -C 0 -m 0,1'
else
   echo "Unknown vendor: $vendor"
   if [[ "$arch" == "ppc64le" ]]; then
      echo "Inferring POWER9..."
      NUMAPREF='numactl -C 0 -m 0'
   elif [[ "$arch" == "aarch64" ]]; then
      echo "Inferring A64FX..."
      NUMAPREF='numactl -C 0 -m 0'
   else
      echo "Unknown architecture: $arch"
      exit 1
   fi
fi

if [ -d "/scratch/maximilian.boether/opt" ]; then
    source /scratch/maximilian.boether/opt/gcc-11.2.0/activate
    source /scratch/maximilian.boether/opt/cmake-3.21/activate
else
    echo "Warning: The precompiled gcc-11.2/cmake-3.21 toolchain does not exist. You are probably not running on HPI infrastructure."
fi

mkdir build_uniform
cd build_uniform
mkdir data
cmake .. -DCMAKE_BUILD_TYPE=Release -DHASHMAP_REPR=1
make -j8 hashmap-read-benchmark hashmap-write-benchmark

mkdir data
./hashmap-read-benchmark generate 8 16 32 64
./hashmap-write-benchmark generate 8 16 32 64
./hashmap-read-benchmark 8 16 32 64
./hashmap-write-benchmark 8 16 32 64
rm -rf data
mkdir data

./hashmap-read-benchmark generate
./hashmap-write-benchmark generate
eval $NUMAPREF ./hashmap-read-benchmark
eval $NUMAPREF ./hashmap-write-benchmark
cd ..

mkdir build_smallpointervals
cd build_smallpointervals
ln -s ../build_uniform/data data
cmake .. -DCMAKE_BUILD_TYPE=Release -DHASHMAP_POINTERVALUES=1
make -j8 hashmap-read-benchmark hashmap-write-benchmark
./hashmap-read-benchmark generate
./hashmap-write-benchmark generate
eval $NUMAPREF ./hashmap-read-benchmark
eval $NUMAPREF ./hashmap-write-benchmark
rm -rf data
rm -rf ../build_uniform/data
cd ..


mkdir build_strings
cd build_strings
mkdir data
cmake .. -DCMAKE_BUILD_TYPE=Release -DHASHMAP_STRINGKEYS=1
make -j8 hashmap-read-benchmark hashmap-write-benchmark
./hashmap-read-benchmark generate
./hashmap-write-benchmark generate
eval $NUMAPREF ./hashmap-read-benchmark
eval $NUMAPREF ./hashmap-write-benchmark
rm -rf data
cd ..

mkdir build_largevals
cd build_largevals
mkdir data
cmake .. -DCMAKE_BUILD_TYPE=Release -DHASHMAP_LARGEVALUES=1
make -j8 hashmap-read-benchmark hashmap-write-benchmark
./hashmap-read-benchmark generate
./hashmap-write-benchmark generate
eval $NUMAPREF ./hashmap-read-benchmark
eval $NUMAPREF ./hashmap-write-benchmark
cd ..


mkdir build_largepointervals
cd build_largepointervals
ln -s ../build_largevals/data data
cmake .. -DCMAKE_BUILD_TYPE=Release -DHASHMAP_POINTERVALUES=1 -DHASHMAP_LARGEVALUES=1
make -j8 hashmap-read-benchmark hashmap-write-benchmark
./hashmap-read-benchmark generate
./hashmap-write-benchmark generate
eval $NUMAPREF ./hashmap-read-benchmark
eval $NUMAPREF ./hashmap-write-benchmark
rm -rf data
rm -rf ../build_largevals/data
cd ..

if [[ "$vendor" == "GenuineIntel" ]]; then
   mkdir build_zipf
   cd build_zipf
   mkdir data
   cmake .. -DCMAKE_BUILD_TYPE=Release -DHASHMAP_ZIPF=1
   make -j8 hashmap-read-benchmark
   ./hashmap-read-benchmark generate
   eval $NUMAPREF ./hashmap-read-benchmark
   rm -rf data
   cd ..
fi

popd

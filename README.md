<h1 align="center">Analyzing Vectorized Hash Maps Across CPU Architectures</h1>
<p align="center">This repository contains the code to our <a href="https://hpi.de/fileadmin/user_upload/fachgebiete/rabl/publications/2023/vectorized_hash_tables_vldb23.pdf"> VLDB '23 paper<a/>.<p/>

This repository comes with implementations for three vectorized hashing schemes (vectorized linear probing, vectorized fingerprinting, bucket-based comparison) and a custom SIMD abstraction layer for x86 (SSE,AVX2,AVX-512), ARM (NEON, SVE), and Power.
Furthermore, we provide a benchmarking framework.
The plotting setup and benchmarking data can be found in the `notebooks/PaperPlots.ipynb` notebook.

## Project Structure

The `benchmark` and `benchmark-include` directories contain all files for running read and write microbenchmarks as in the paper.
The `include` and `src` directories contain code relevant to the hashmap implementations.
In the `notebooks-paper` you can find tooling for generating the plots and evaluating the result files.

## Build Instructions
The project uses CMake > 3.16 and requires C++20.
In the build directory, run `cmake ..` (potentially, with options such as `-DCMAKE_BUILD_TYPE=RELEASE`) and then `make -j8 hashmap-read-benchmark hashmap-write-benchmark` to compile the benchmarks and `make -j8 hashmap-tests` to compile the unit tests.

## Citation
If you use this code in your work, please cite us.

```bibtex
@article{boether_hashmaps_23,
  author = {B\"{o}ther, Maximilian and Benson, Lawrence and Klimovic, Ana and Rabl, Tilmann},
  title = {Analyzing Vectorized Hash Maps Across CPU Architectures},
  doi = {10.14778/3611479.3611485},
  journal = {Proceedings of the {VLDB} Endowment},
  pages = {2755 - 2768},
  number = {11},
  volume = {16},
  year = {2023},
}
```

## SIMD Abstraction layer

We employ metaprogramming to build an abstraction around the different vector ISAs.
The file containing this is found in `include/hashmap/simd_utils.hpp`.
In general, we utilize both preprocessor macros and C++'s compile-time metaprogramming features, such as `if constexpr`, to build a header that offers a common interface.
Additionally, there is maintenance overhead between compilers.
While gcc implements all vector functions we use as C-style functions, clang implements some as  macros or built-in functions.

We identify four key operations that our hashing schemes perform. 
We (1) need to detect how large our vector registers are, (2) compare each element in a vector to the key/fingerprint we are searching for, and either (VLP) (3) extract the index of the match, if we found the key/fingerprint within the vector, or (VFP, BBC) (4) iterate over all matches within the vector.
Finding a common point of abstraction is difficult.
Higher level abstractions are easier to maintain, but lower level abstractions allow for more optimization.
For example, compilers do not allow SVE data types to be class members, because the struct size would be undefined at compile time.
This can be problematic as we sometimes need constant state, i.e., it would be useful to initialize a vector once and then reuse it all the time.
For example, the ARM SVE implementation needs an additional index vector, which we initially generated each time we extracted the match in the SIMD abstraction layer.
However, we increased performance by doing this only once at the beginning of the hash map lookup.
This means that the hash map includes platform-dependent code for the sake of performance, although most of this should be abstracted away in the SIMD abstraction layer.
Libraries such as xSIMD or TVL allow for abstracting different ISAs, which is more maintainable than writing abstractions yourself.
We decided to use a custom abstraction layer for maximum performance.
The platforms have different ways of implementing the operations that can only be abstracted by a hash map-specific layer, not a generic abstraction layer.
We show this on the example of extracting the first match.
In VLP, we are interested in extracting the first match of the needle (key) in the haystack (vector register of keys).

__SSE/AVX2.__ On SSE and AVX2, the comparison result is a new vector where each entry represents whether there has been a match or not.
We then create a movemask from this vector. 
This movemask is an integer, where each bit indicates a match in the according byte block in the vector register. 
The least significant bit corresponds to the first byte in the vector.
Hence, we extract the index of the first match by counting the trailing zeros of the movemask integer, which is a native x86 instruction, and scaling that number accordingly by dividing it by the key size in bytes.

__AVX-512.__ On AVX-512, we obtain the compare result in a mask register.
This mask register is actually represented as an integer type, such that we can  handle it similarly to a movemask, without needing to explicitly use a movemask instruction.
However, there are two differences between SSE/AVX2 and AVX-512: First, on AVX-512, the comparison instructions respect the types.
This means that we do not obtain a mask on a byte level but on an element level.
Hence, we do not require to scale the trailing zero count by the key size. 
Second, we can now have 64-bit integers as masks, because a 512-bit register holding 8-bit integers requires 64-bit in the output mask. 
This requires us to use a different trailing zero count instruction. 

__NEON.__ On NEON, there is no native movemask instruction to convert the comparison result vector into an integer. 
We test two alternatives to extract the first match on NEON.
Alternative 1 is using an emulation of the SSE movemask instruction, provided by the SSE2NEON library.
After creating the movemask, we proceed as on SSE/AVX2. 
Note that ARM only offers a leading zero count instruction.
Therefore, we use a trailing zero count compiler intrinsic, which first natively inverts the bitstring, followed by a native leading zero count instruction.
Alternative 2 is the UMINV approach.
We call it that because the `UMINV` NEON instruction, which provides the minimum of a vector as a scalar, is at the
core of this alternative. 
In this approach, we first fill an _index vector_ register with the increasing integer sequence `0, 1, 2, ...`, until it is full.
This index vector has entries representing the indices themselves, i.e., the element at the first index is 0, at the second index is 1, and so on.
We create another _invalid vector_ by broadcasting an integer higher than the largest index into all entries.
Note that these two vectors can be initialized once at hash map initialization and reused for all operations.
We then use a bitwise select instruction on the index vector and the invalid vector, with the compare result vector as a selector. 
For each vector entry, this instruction takes the value from the index vector if there has been a match (comparator is one), and the value from the invalid vector if there has not been a match (comparator is zero).
If we then extract the minimum from the resulting vector, we obtain the first index within the vector where there has been a match.
It is interesting to compare these two approaches because it is not clear what is faster.
The first one is the direct porting of the x86 algorithm to NEON, while the second alternative is the native NEON solution.

__SVE.__ On SVE, we obtain the comparison result in a mask register.
However, unlike AVX-512, the mask registers are not representable as integers, as they are of variable length unknown to the compiler at compile time.
Hence, we have to use instructions that operate directly on the mask registers to extract the first match.
First, we make sure that we only keep the first match in case there are multiple matches by applying a _break after first true condition_ instruction, which is usually used to abort loops over mask registers.
By applying this operation with the comparison result register as both the operand and the predicate, we obtain a result where only the first match is set to true. 
Similar to the NEON UMINV approach, we create an index vector with the integer sequence `0, 1, 2, ...`. 
Now, we extract the element in the index vector at the index where our mask register has the match, thereby obtaining the index of the first match. 
Note that we have to apply the break after first true condition instruction because SVE only offers an instruction to extract the last true element.

__VSX.__ On VSX, we do not have any intermediate comparison result.
Instead,  we use the `vec_first_match_index` intrinsic, which is available since POWER9. 
Given the haystack register, and a register in which we broadcast the needle, this intrinsic directly gives us the index of the first match, showing that the point of abstraction is important.

Analogously, for VFP and BBC, we can build a scalar iterator to iterate over all matches.
On x86, this uses the movemask.
On Power and NEON, we use simulated movemasks using SSE2NEON and Dimsum.
On Power and NEON, we can also build a vectorized iterator, which operators on a vector register instead of simulated movemask.


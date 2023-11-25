cmake_minimum_required(VERSION 3.16.3)
project(datagram-reassembly C)

add_subdirectory(deps/piot/clog/src/lib)
add_subdirectory(deps/piot/flood-c/src/lib)
add_subdirectory(deps/piot/imprint/src/lib)
add_subdirectory(deps/piot/tiny-libc/src/lib)


add_subdirectory(lib)



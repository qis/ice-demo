all: build build-exceptions test test-exceptions benchmark benchmark-exceptions

# =================================================================================================
# build
# =================================================================================================

build: \
  build-test \
  build-benchmark

build-test: configure
	@cmake --build build/llvm --config Debug --target tests
	@cmake --build build/msvc --config Debug --target tests

build-benchmark: configure
	@cmake --build build/llvm --config Release --target benchmarks
	@cmake --build build/msvc --config Release --target benchmarks

.PHONY: build build-test build-benchmark

# =================================================================================================
# build-exceptions
# =================================================================================================

build-exceptions: \
  build-exceptions-test \
  build-exceptions-benchmark

build-exceptions-test: configure-exceptions
	@cmake --build build/exceptions-llvm --config Debug --target tests
	@cmake --build build/exceptions-msvc --config Debug --target tests

build-exceptions-benchmark: configure-exceptions
	@cmake --build build/exceptions-llvm --config Release --target benchmarks
	@cmake --build build/exceptions-msvc --config Release --target benchmarks

.PHONY: build-exceptions build-exceptions-test build-exceptions-benchmark


# =================================================================================================
# install
# =================================================================================================

install: \
  install-llvm \
  install-msvc

install-llvm: \
  install-llvm-debug \
  install-llvm-release

install-msvc: \
  install-msvc-debug \
  install-msvc-release

install-llvm-debug: build/llvm/build.ninja
	@cmake --build build/llvm --config Debug --target install

install-llvm-release: build/llvm/build.ninja
	@cmake --build build/llvm --config Release --target install

install-msvc-debug: build/msvc/build.ninja
	@cmake --build build/msvc --config Debug --target install

install-msvc-release: build/msvc/build.ninja
	@cmake --build build/msvc --config Release --target install

.PHONY: install install-llvm install-llvm-debug install-llvm-release install-msvc install-msvc-debug install-msvc-release

# =================================================================================================
# test
# =================================================================================================

test: \
  build-test \
  test-llvm \
  test-msvc \

test-llvm: build/llvm/build.ninja
	@cmake --build build/llvm --config Debug --target run-tests

test-msvc: build/msvc/build.ninja
	@cmake --build build/msvc --config Debug --target run-tests

.PHONY: test test-llvm test-msvc

# =================================================================================================
# test-exceptions
# =================================================================================================

test-exceptions: \
  build-exceptions-test \
  test-exceptions-llvm \
  test-exceptions-msvc

test-exceptions-llvm: build/exceptions-llvm/build.ninja
	@cmake --build build/exceptions-llvm --config Debug --target run-tests

test-exceptions-msvc: build/exceptions-msvc/build.ninja
	@cmake --build build/exceptions-msvc --config Debug --target run-tests

.PHONY: test-exceptions test-exceptions-llvm test-exceptions-msvc

# =================================================================================================
# benchmark
# =================================================================================================

benchmark: \
  build-benchmark \
  benchmark-llvm \
  benchmark-msvc

benchmark-llvm: build/llvm/build.ninja
	@cmake --build build/llvm --config Release --target run-benchmarks

benchmark-msvc: build/msvc/build.ninja
	@cmake --build build/msvc --config Release --target run-benchmarks

.PHONY: benchmark benchmark-llvm benchmark-msvc

# =================================================================================================
# benchmark-exceptions
# =================================================================================================

benchmark-exceptions: \
  build-exceptions-benchmark \
  benchmark-exceptions-llvm \
  benchmark-exceptions-msvc

benchmark-exceptions-msvc: build/exceptions-msvc/build.ninja
	@cmake --build build/exceptions-msvc --config Release --target run-benchmarks

benchmark-exceptions-llvm: build/exceptions-llvm/build.ninja
	@cmake --build build/exceptions-llvm --config Release --target run-benchmarks

.PHONY: benchmark-exceptions benchmark-exceptions-msvc benchmark-exceptions-llvm

# =================================================================================================
# configure
# =================================================================================================

configure: \
  build/llvm/build.ninja \
  build/msvc/build.ninja

build/llvm/build.ninja:
	@cmake -G "Ninja Multi-Config" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" \
	  -DCMAKE_TOOLCHAIN_FILE="$(ACE)/llvm.cmake" \
	  -DCMAKE_INSTALL_PREFIX="$(CURDIR)/build/install/llvm" \
	  -DCMAKE_INSTALL_RPATH="$(ACE)/lib" \
	  -DCMAKE_DEBUG_POSTFIX="d" \
	  -DBUILD_SHARED_LIBS=ON \
	  -B build/llvm

build/msvc/build.ninja:
	@cmake -G "Ninja Multi-Config" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" \
	  -DCMAKE_TOOLCHAIN_FILE="$(ACE)/msvc.cmake" \
	  -DCMAKE_INSTALL_PREFIX="$(CURDIR)/build/install/msvc" \
	  -DCMAKE_DEBUG_POSTFIX="d" \
	  -DBUILD_SHARED_LIBS=ON \
	  -B build/msvc

.PHONY: configure

# =================================================================================================
# configure-exceptions
# =================================================================================================

configure-exceptions: \
  build/exceptions-llvm/build.ninja \
  build/exceptions-msvc/build.ninja

build/exceptions-llvm/build.ninja:
	@cmake -G "Ninja Multi-Config" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" \
	  -DCMAKE_TOOLCHAIN_FILE="/opt/llvm/toolchain.cmake" \
	  -DCMAKE_INSTALL_PREFIX="$(CURDIR)/build/install/exceptions-llvm" \
	  -DLLVM_ENABLE_EXCEPTIONS=ON \
	  -B build/exceptions-llvm

build/exceptions-msvc/build.ninja:
	@cmake -G "Ninja Multi-Config" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" \
	  -DCMAKE_TOOLCHAIN_FILE="$(ACE)/msvc.cmake" \
	  -DCMAKE_INSTALL_PREFIX="$(CURDIR)/build/install/exceptions-msvc" \
	  -DLLVM_ENABLE_EXCEPTIONS=ON \
	  -B build/exceptions-msvc

.PHONY: configure-exceptions

# =================================================================================================
# format
# =================================================================================================

format:
	@cmake -P /opt/ace/format.cmake benchmarks tests src/ice

# =================================================================================================
# clean
# =================================================================================================

clean:
	@cmake -E rm -rf build/llvm
	@cmake -E rm -rf build/msvc
	@cmake -E rm -rf build/exceptions-llvm
	@cmake -E rm -rf build/exceptions-msvc

.PHONY: clean

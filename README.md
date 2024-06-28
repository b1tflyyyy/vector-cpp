## ⚙️ Vector C++20 (Simple Implementation)

## 📊 Current status: in development
## 📜 Vector implemented features list:
- [x] `push_back()`
- [x] `reserve()`
- [x] `operator[]`
- [x] `is_empty()`
- [x] `size()`
- [x] `capacity()`
- [x] `move ctor`
- [x] `move assignment operator`
- [x] `copy ctor`
- [x] `copy assignment ctor`
- [x] `basic dtor`
- [x] `3 basic ctors`

## 🔗 Vector Iterator
- The basic FI(Forward Iterator) is also implemented here, and in the near future I will implement RAI(Random Access Iterator).

## 📑 Technical Stack:
- C++ 20
- CMake >= 3.14
- GTF(Google Test framework) for tests
- CI will be soon

## 🖼 How to build & test
```
mkdir build
cd build
cmake ..
cmake --build .
cd tests
./Tests
```

## 🔒 Supported OS:
- [x] GNU/Linux (verified on Void Linux)
- [ ] Windows (not yet verified)
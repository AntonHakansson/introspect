# Introspect [![][img_license]](#license) [![][img_loc]][loc] [![Build status](https://ci.appveyor.com/api/projects/status/8w3txwv9ijiuc3g0?svg=true)](https://ci.appveyor.com/project/AntonHakansson/introspect)

[img_license]: https://img.shields.io/badge/License-MIT_or_Apache_2.0-blue.svg
[img_loc]: https://tokei.rs/b1/github/AntonHakansson/introspect
[loc]: https://github.com/Aaronepower/tokei

A very simple c++ introspection system. 
In simple terms, a header file `generated.h` is generated for any struct that is annotated with `INTROSPECT`.
The generated header file contains the type information for the annotated structs.

## Build
Before running `build.bat` run `shell/setVcArgs.bat` to configure x64 build environment.

## Example
```c++
INTROSPECT struct TestStruct {
  int anInt;
  char aChar;
  double aDouble;
};

INTROSPECT struct AnotherTestStruct {
  int anotherInt;
  char anotherChar;
  INTROSPECT_CSTRING char *str;
  TestStruct obj;
};

...

#include "generated.h"

TestStruct obj = {};
obj.anInt = 69;

AnotherTestStruct obj2 = {};
obj2.anotherChar = 's';
obj2.str = "HELLO, THIS IS A STRING";

DEBUGStruct(TestStruct, obj);
DEBUGStruct(AnotherTestStruct, obj2);
```

Outputs:
```
obj {
  anInt = 69
  aChar = 0x0
  aDouble = 0.000000
}
obj2 {
  anotherInt = 0
  anotherChar = 's'
  str = "HELLO, THIS IS A STRING"
  obj {
    anInt = 0
    aChar = 0x0
    aDouble = 0.000000
  }
}
```

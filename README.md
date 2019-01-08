# introspect [![][img_license]](#license) [![][img_loc]][loc] [![Build status](https://ci.appveyor.com/api/projects/status/8w3txwv9ijiuc3g0?svg=true)](https://ci.appveyor.com/project/AntonHakansson/introspect)

[img_license]: https://img.shields.io/badge/License-MIT_or_Apache_2.0-blue.svg
[img_loc]: https://tokei.rs/b1/github/AntonHakansson/rbreakout
[loc]: https://github.com/Aaronepower/tokei

A very simple c++ introspection system. 
In simple terms, a header file `generated.h` is generated for any struct that is annotated with `INTROSPECT`.
The generated header file contains the type information for the annotated structs.

## Build
Before running `build.bat` run `shell/setVcArgs.bat` to configure x64 build environment.

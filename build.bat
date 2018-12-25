@echo off

set CommonCompilerFlags=-WL -O2 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib kernel32.lib

cl %CommonCompilerFlags% /D_CRT_SECURE_NO_WARNINGS introspect.cpp /link %CommonLinkerFlags%
introspect.exe > generated.h

cl %CommonCompilerFlags% /D_CRT_SECURE_NO_WARNINGS test.cpp /link %CommonLinkerFlags%
test.exe

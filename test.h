#pragma once

#include "introspect.h"

/* This is some example comment that should
   not get read later on */

INTROSPECT struct TestStruct {
  // We want to access these elements later on
  int anInt;
  char aChar;
  double aDouble;
};

INTROSPECT struct AnotherTestStruct {
  // We want to access these elements later on
  int anotherInt;
  char anotherChar;
  INTROSPECT_CSTRING char *str;
  TestStruct obj;
};

#pragma once
#include <stdint.h>

#define INTROSPECT
#define INTROSPECT_CSTRING

enum MetaType {
  MetaType_int,
  MetaType_char,
  MetaType_float,
  MetaType_double,
  MetaType_TestStruct,
  MetaType_AnotherTestStruct,
};

enum MetaMemberFlag {
  MetaMemberFlag_None = 0,
  MetaMemberFlag_IsPointer = 1 << 0,
  MetaMemberFlag_IsCString = 1 << 1,
};

struct MemberDefinition {
  int flags;
  MetaType type;
  char member_name[16];
  size_t member_offset;
};


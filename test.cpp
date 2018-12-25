#include <stdio.h>
#include "test.h"
#include "generated.h"

#define ArrayCount(array) (sizeof((array))/sizeof((array)[0]))
#define DEBUGStruct(structType, structObject) DEBUGStructImpl(#structObject, ArrayCount(members_of_ ## structType), members_of_ ## structType, &structObject, 0)

static void
DEBUGChar(char c) {
  bool isAlpha = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  bool isDigit = (c >= '0' && c <= '9');

  if (isAlpha || isDigit) {
    printf("'%c'\n", c);
  }
  else {
    printf("0x%x\n", c);
  }
}

static void
DEBUGStructImpl(const char *structName, size_t member_count, MemberDefinition *members, void *base_pointer, int indentLevel = 0) {
#define HANDLE_META_PRINT(formatSpecifier, valueType) \
  PRINT_INDENT(indentLevel + 1);                      \
  if (isPointer) {                                    \
      if ((*(valueType**)value) == nullptr) {         \
        printf("%s = null\n", m->member_name);                       \
      }                                               \
      else {                                          \
        printf("*%s = " formatSpecifier "\n", m->member_name, **(valueType**)value); \
      }                                               \
  }                                                   \
  else {                                              \
    printf("%s = " formatSpecifier "\n",  m->member_name, *(valueType*)value);  \
  }

#define PRINT_INDENT(currentIndent) \
  for (int indent = 0; indent < currentIndent; indent++) { \
    printf("  "); \
  }

  PRINT_INDENT(indentLevel);
  printf("%s {\n", structName);

  for (size_t i = 0; i < member_count; i++) {
    MemberDefinition *m = members + i;
    void *value = ((char*)base_pointer + m->member_offset);
    bool isPointer = m->flags & MetaMemberFlag_IsPointer;

    switch(m->type) {
      case MetaType_char: {
        PRINT_INDENT(indentLevel + 1);
        if (isPointer) {
          bool isCString = m->flags & MetaMemberFlag_IsCString;

          if ((*(char**)value) == nullptr) {
            printf("%s = null\n", m->member_name);
          }
          else if(isCString) {
            printf("%s = \"%s\"\n", m->member_name, *(char**)value);
          }
          else {
            printf("*%s = ", m->member_name);
            DEBUGChar(**(char**)value);
          }
        }
        else {
          printf("%s = ", m->member_name);
          DEBUGChar(*(char*)value);
        }
      } break;
      case MetaType_int: {HANDLE_META_PRINT("%d", int);} break;
      case MetaType_float: {HANDLE_META_PRINT("%f", float);} break;
      case MetaType_double: {HANDLE_META_PRINT("%f", double);} break;
      META_HANDLE_TYPE_DUMP(value, indentLevel + 1);
    }
  }

  PRINT_INDENT(indentLevel);
  printf("}\n");
#undef HANDLE_META_PRINT
#undef PRINT_INDENT
}

int main(int num_args, char **arguments) {
  TestStruct obj = {};
  obj.anInt = 69;

  AnotherTestStruct obj2 = {};
  obj2.anotherChar = 's';
  obj2.str = "HELLO, THIS IS A STRING";

  DEBUGStruct(TestStruct, obj);
  DEBUGStruct(AnotherTestStruct, obj2);
}

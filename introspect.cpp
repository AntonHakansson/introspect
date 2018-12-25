#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "introspect.h"

typedef uint8_t u8;

static char *readEntireFileAndNullTerminate(const char *fileName, size_t *fileLen) {
  char *result = 0;

  FILE *file = fopen(fileName, "r");
  if(file) {
    fseek(file, 0, SEEK_END);
    size_t _fileLen = ftell(file);
    fseek(file, 0, SEEK_SET);

    result = (char *)malloc(_fileLen + 1);
    fread(result, _fileLen, 1, file);
    result[_fileLen] = '\0';

    fclose(file);

    if (fileLen) {
      *fileLen = _fileLen;
    }
  }

  return(result);
}

struct MetaStruct
{
  char *name;
  MetaStruct *next;
};
static MetaStruct *firstMetaStruct;

struct MetaMemberType
{
  char *name;
  MetaStruct *next;
};

enum TokenType {
  Token_Unknown,

  Token_Colon,
  Token_Semicolon,
  Token_Asterisk,
  Token_OpenParam,
  Token_CloseParam,
  Token_OpenBracket,
  Token_CloseBracket,
  Token_OpenBrace,
  Token_CloseBrace,

  Token_Identifier,
  Token_String,

  Token_EndOfStream,
};

struct Token {
  TokenType type;

  size_t textLength;
  char *text;
};

struct Tokenizer {
  char *at;
};

inline u8 isEndOfLine(char c) {
  return c == '\n' ||
         c == '\r';
}

inline u8 isWhitespace(char c) {
  return c == ' ' ||
         c == '\t' ||
         c == '\v' ||
         c == '\f' ||
         isEndOfLine(c);
}

inline u8 isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z');
}

inline u8 isDigit(char c) {
  return (c >= '0' && c <= '9');
}

static void eatAllWhitespace(Tokenizer *tokenizer) {
  while(1) {
    if (isWhitespace(tokenizer->at[0])) {
      ++tokenizer->at;
    }
    else if (tokenizer->at[0] == '/' &&
             tokenizer->at[1] == '/') {
      tokenizer->at += 2;
      while (!isEndOfLine(tokenizer->at[0])) {
        ++tokenizer->at;
      }
    }
    else if (tokenizer->at[0] == '/' &&
             tokenizer->at[1] == '*') {
      tokenizer->at += 2;
      while (tokenizer->at[0] &&
             !(tokenizer->at[0] == '*' && tokenizer->at[1] == '/')) {
        ++tokenizer->at;
      }

      if (tokenizer->at[0] == '*') {
        tokenizer->at += 2;
      }
    }
    else {
      break;
    }
  }
}

static u8 tokenEquals(Token token, char *text) {
  char *at = text;
  for (int index = 0;
       index < token.textLength;
       ++index, ++at) {
    if (*at == '\0' ||
        token.text[index] == '\0' ||
        *at != token.text[index]) {
      return 0;
    }
  }

  return *at == '\0';
}

static Token getToken(Tokenizer *tokenizer) {
  eatAllWhitespace(tokenizer);

  Token token = {};
  token.textLength = 1;
  token.text = tokenizer->at;
  char c = tokenizer->at[0];
  ++tokenizer->at;

  switch (c) {
    case '\0': {token.type = Token_EndOfStream;} break;

    case '(': {token.type = Token_OpenParam;} break;
    case ')': {token.type = Token_CloseParam;} break;
    case '[': {token.type = Token_OpenBracket;} break;
    case ']': {token.type = Token_CloseBracket;} break;
    case '{': {token.type = Token_OpenBrace;} break;
    case '}': {token.type = Token_CloseBrace;} break;
    case ':': {token.type = Token_Colon;} break;
    case ';': {token.type = Token_Semicolon;} break;
    case '*': {token.type = Token_Asterisk;} break;

    case '"': {
      token.type = Token_String;
      token.text = tokenizer->at;
      while (tokenizer->at[0] &&
             tokenizer->at[0] != '"') {
        if (tokenizer->at[0] == '\\' &&
            tokenizer->at[1]) {
          ++tokenizer->at;
        }
        ++tokenizer->at;
      }

      token.textLength = tokenizer->at - token.text;
      if (tokenizer->at[0] == '"') {
        ++tokenizer->at;
      }
    } break;

    default: {
      if (isAlpha(c)) {
        token.type = Token_Identifier;

        while (isAlpha(tokenizer->at[0]) ||
               isDigit(tokenizer->at[0]) ||
               tokenizer->at[0] == '_') {
          ++tokenizer->at;
        }

        token.textLength = tokenizer->at - token.text;
      }
#if 0
      else if (isDigit(c)) {
        parseNumber();
      }
#endif
      else {
        token.type = Token_Unknown;
      }
    } break;
  }

  return token;
}

static u8 requireToken(Tokenizer *tokenizer, TokenType type) {
  Token token = getToken(tokenizer);
  return type == token.type;
}

static void parseMember(Tokenizer *tokenizer, Token structTypeToken, Token memberTypeToken, u8 memberFlags) {
  u8 parsing = 1;
  u8 isPointer = 0;

  while (parsing) {
    Token token = getToken(tokenizer);
    switch(token.type) {
      case Token_Asterisk: {
        isPointer = 1;
      } break;

      case Token_Identifier: {
        printf("   {%s", isPointer ? "MetaMemberFlag_IsPointer" : "MetaMemberFlag_None");
        if (memberFlags & MetaMemberFlag_IsCString) {
          printf(" | MetaMemberFlag_IsCString");
        }
        printf(", MetaType_%.*s, \"%.*s\", ((size_t)&(((%.*s *)0)->%.*s))},\n",
              (int)memberTypeToken.textLength, memberTypeToken.text,
              (int)token.textLength, token.text,
              (int)structTypeToken.textLength, structTypeToken.text,
              (int)token.textLength, token.text);
      } break;

      case Token_Semicolon:
      case Token_EndOfStream: {
        parsing = false;
      } break;
    }
  }
}

static void parseIntrospectable(Tokenizer *tokenizer) {
  // We expect struct StructName {type name;}
  Token typeToken = getToken(tokenizer);
  if (tokenEquals(typeToken, "struct")) {
    Token structNameToken = getToken(tokenizer);
    if (requireToken(tokenizer, Token_OpenBrace)) {
      printf("MemberDefinition members_of_%.*s[] = {\n", (int)structNameToken.textLength, structNameToken.text);

      u8 memberFlags = 0;
      while (1) {
        Token memberToken = getToken(tokenizer);
        if (memberToken.type == Token_CloseBrace) {
          break;
        }
        else if (tokenEquals(memberToken, "INTROSPECT_CSTRING")) {
          memberFlags |= MetaMemberFlag_IsCString;
        }
        else {
          parseMember(tokenizer, structNameToken, memberToken, memberFlags);
          memberFlags = 0;
        }
      }
    }
    printf("};\n");

    MetaStruct *meta = (MetaStruct *)malloc(sizeof(MetaStruct));
    meta->name = (char *)malloc(sizeof(structNameToken.textLength + 1));
    memcpy(meta->name, structNameToken.text, structNameToken.textLength);
    meta->name[structNameToken.textLength] = '\0';
    meta->next = firstMetaStruct;
    firstMetaStruct = meta;
  }
  else {
    fprintf(stderr, "ERROR: Can only instrospect struct for now!");
  }
}

int main() {
  size_t contentLen = 0;
  char *fileContents = readEntireFileAndNullTerminate("test.h", &contentLen);

  Tokenizer tokenizer = {};
  tokenizer.at = fileContents;

  u8 parsing = true;
  while (parsing) {
    Token token = getToken(&tokenizer);

    switch (token.type) {
      case Token_EndOfStream: {
        parsing = false;
      } break;
      case Token_Identifier: {
        if (tokenEquals(token, "INTROSPECT")) {
          parseIntrospectable(&tokenizer);
        }
      }
      default: break;
    }
  }

  printf("\n#define META_HANDLE_TYPE_DUMP(MemberPtr, NextIndentLevel) \\\n");
  for (MetaStruct *meta = firstMetaStruct; meta; meta = meta->next) {
    printf("    case MetaType_%s: { DEBUGStructImpl(m->member_name, ArrayCount(members_of_%s), members_of_%s, MemberPtr, (NextIndentLevel));} break; %s\n",
           meta->name, meta->name, meta->name,
           meta->next ? "\\" : "");
  }

  // Note(Hakan): let windows free our allocated memory
  return 0;
}


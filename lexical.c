//
// Created by asus on 2019/11/18.
//

#include "defines.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int MAX = 0;
int t_types[10000];
int line_num[10000];
int is_error[10000];
int line_pointer = 1;
char tokens[10000][10000];

char constt[10] = "const";
char intt[10] = "int";
char charr[10] = "char";
char voidd[10] = "void";
char mainn[10] = "main";
char iff[10] = "if";
char elsee[10] = "else";
char doo[10] = "do";
char whilee[10] = "while";
char forr[10] = "for";
char scanff[10] = "scanf";
char printff[10] = "printf";
char returnn[10] = "return";

char ch, token[1000];
int num, symbol, tag = 0, pre_error = 0;
FILE *fin = NULL;

void my_getchar() {
  ch = fgetc(fin);
  if (ch == EOF) {
    tag = 1;
  }
}

void clearToken() { token[0] = '\0'; }

int isLetter() {
  if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
    return 1;
  } else {
    return 0;
  }
}

int isDigit() {
  if (ch >= '0' && ch <= '9') {
    return 1;
  } else {
    return 0;
  }
}

int isComma() {
  if (ch == ',') {
    return 1;
  } else {
    return 0;
  }
}

int isSemi() {
  if (ch == ';') {
    return 1;
  } else {
    return 0;
  }
}

int isPlus() {
  if (ch == '+') {
    return 1;
  } else {
    return 0;
  }
}

int isMinus() {
  if (ch == '-') {
    return 1;
  } else {
    return 0;
  }
}

int isDivi() {
  if (ch == '/') {
    return 1;
  } else {
    return 0;
  }
}

int isStar() {
  if (ch == '*') {
    return 1;
  } else {
    return 0;
  }
}

int isLpar() {
  if (ch == '(') {
    return 1;
  } else {
    return 0;
  }
}

int isRpar() {
  if (ch == ')') {
    return 1;
  } else {
    return 0;
  }
}

int isLss() {
  if (ch == '<') {
    return 1;
  } else {
    return 0;
  }
}

int isAssign() {
  if (ch == '=') {
    return 1;
  } else {
    return 0;
  }
}

int isGre() {
  if (ch == '>') {
    return 1;
  } else {
    return 0;
  }
}

int isExclaim() {
  if (ch == '!') {
    return 1;
  } else {
    return 0;
  }
}

int isLbrack() {
  if (ch == '[') {
    return 1;
  } else {
    return 0;
  }
}

int isRbrack() {
  if (ch == ']') {
    return 1;
  } else {
    return 0;
  }
}

int isLbrace() {
  if (ch == '{') {
    return 1;
  } else {
    return 0;
  }
}

int isRbrace() {
  if (ch == '}') {
    return 1;
  } else {
    return 0;
  }
}

int isSquotation() {
  if (ch == '\'') {
    return 1;
  } else {
    return 0;
  }
}

int isDquotation() {
  if (ch == '\"') {
    return 1;
  } else {
    return 0;
  }
}

int isChar() {
  if (isPlus() || isMinus() || isDivi() || isStar() || isLetter() ||
      isDigit()) {
    return 1;
  } else {
    return 0;
  }
}

int isString() {
  if ((int)ch == 32 || (int)ch == 33 || ((int)ch >= 35 && (int)ch <= 126)) {
    return 1;
  } else {
    return 0;
  }
}

void catToken() {
  int i;
  for (i = 0; i < 100; i++) {
    if (token[i] == '\0') {
      break;
    }
  }
  token[i] = ch;
  token[i + 1] = '\0';
}

void cat_trans() {
  int i;
  for (i = 0; i < 100; i++) {
    if (token[i] == '\0') {
      break;
    }
  }
  token[i] = '\\';
  token[i + 1] = '\0';
}

void retract() { fseek(fin, -1, SEEK_CUR); }

int reserver() {
  if (strcmp(token, constt) == 0) {
    return CONSTTK;
  } else if (strcmp(token, intt) == 0) {
    return INTTK;
  } else if (strcmp(token, charr) == 0) {
    return CHARTK;
  } else if (strcmp(token, voidd) == 0) {
    return VOIDTK;
  } else if (strcmp(token, mainn) == 0) {
    return MAINTK;
  } else if (strcmp(token, iff) == 0) {
    return IFTK;
  } else if (strcmp(token, elsee) == 0) {
    return ELSETK;
  } else if (strcmp(token, doo) == 0) {
    return DOTK;
  } else if (strcmp(token, whilee) == 0) {
    return WHILETK;
  } else if (strcmp(token, forr) == 0) {
    return FORTK;
  } else if (strcmp(token, scanff) == 0) {
    return SCANFTK;
  } else if (strcmp(token, printff) == 0) {
    return PRINTFTK;
  } else if (strcmp(token, returnn) == 0) {
    return RETURNTK;
  } else {
    return 0;
  }
}

int transNum(int *format_) {
  int re = 0;
  int i;
  for (i = 0; token[i] != '\0'; ++i) {
    re = re * 10 + token[i] - '0';
  }
  if (token[0] == '0' && i > 1) {
    *format_ = 0;
  }
  return re;
}

void lexic_error() {
  pre_error = 1;
  // printf("ERROR!");
}

int getsym() {
  my_getchar();
  if (ch == EOF) {
    return -1;
  }
  clearToken();
  while (isspace(ch)) {
    if (ch == '\n') {
      line_pointer++;
    }
    my_getchar();
  }
  if (isLetter()) {
    // my_getchar();
    while (isLetter() || isDigit()) {
      catToken();
      my_getchar();
    }
    retract();
    int resultValue = reserver();
    if (resultValue == 0) {
      symbol = IDENFR;
    } else {
      symbol = resultValue;
    }
  } else if (isSquotation()) {
    my_getchar();
    if (!isChar()) { //如果字符不合法
      lexic_error();
    }
    catToken();
    my_getchar();
    if (!isSquotation()) { //如果结尾不是单引号
      lexic_error();
    }
    symbol = CHARCON;
  } else if (isDquotation()) {
    my_getchar();
    while (isString()) {
      if (ch == '\\') {
        cat_trans();
      }
      catToken();
      my_getchar();
    }
    retract();
    my_getchar();
    if (!isDquotation()) {
      lexic_error(); //缺少右边的“
    }
    symbol = STRCON;
  } else if (isDigit()) {
    // my_getchar();
    while (isDigit()) {
      catToken();
      my_getchar();
    }
    retract();
    int digit_format = 1;
    num = transNum(&digit_format);
    if (digit_format == 0) {
      lexic_error(); //数字为0打头
    }
    symbol = INTCON;
  } else if (isPlus()) {
    catToken();
    symbol = PLUS;
  } else if (isMinus()) {
    catToken();
    symbol = MINU;
  } else if (isStar()) {
    catToken();
    symbol = MULT;
  } else if (isLpar()) {
    catToken();
    symbol = LPARENT;
  } else if (isRpar()) {
    catToken();
    symbol = RPARENT;
  } else if (isComma()) {
    catToken();
    symbol = COMMA;
  } else if (isSemi()) {
    catToken();
    symbol = SEMICN;
  } else if (isDivi()) {
    catToken();
    symbol = DIV;
  } else if (isLss()) {
    catToken();
    my_getchar();
    if (isAssign()) {
      catToken();
      symbol = LEQ;
    } else {
      retract();
      symbol = LSS;
    }
  } else if (isGre()) {
    catToken();
    my_getchar();
    if (isAssign()) {
      catToken();
      symbol = GEQ;
    } else {
      retract();
      symbol = GRE;
    }
  } else if (isAssign()) {
    catToken();
    my_getchar();
    if (isAssign()) {
      catToken();
      symbol = EQL;
    } else {
      retract();
      symbol = ASSIGN;
    }
  } else if (isExclaim()) {
    catToken();
    my_getchar();
    if (!isAssign()) {
      lexic_error(); //！后面不是=
    }
    catToken();
    symbol = NEQ;
  } else if (isLbrace()) {
    catToken();
    symbol = LBRACE;
  } else if (isRbrace()) {
    catToken();
    symbol = RBRACE;
  } else if (isLbrack()) {
    catToken();
    symbol = LBRACK;
  } else if (isRbrack()) {
    catToken();
    symbol = RBRACK;
  } else {
    lexic_error();
    symbol = ERROE;
  }
  return 0;
}

void lexical() {
  fin = fopen("testfile.txt", "r");
  while (1) {
    int re = getsym();
    if (re == -1 || tag == 1) {
      break;
    }
    // printf("%s %s\n", types[symbol - 1], token);
    t_types[MAX] = symbol;
    strcpy(tokens[MAX], token);
    line_num[MAX] = line_pointer;
    is_error[MAX] = pre_error;
    pre_error = 0;
    MAX++;
    // printf(fout, "%s %s\n", types[symbol - 1], token);
  }
}

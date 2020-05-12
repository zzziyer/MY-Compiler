//
// Created by asus on 2019/11/08.
//
#include <stdio.h>
#ifndef UNTITLED3_DEFINES_H
#define UNTITLED3_DEFINES_H

void print_();
void mips_gener();
void lexical();
void mcode_gener(int op, int p1, int p2, int p3, int t1, int t2, int t3,
                 int a_off_dst, int a_off_type);
// int error(char type_code);

// op,arg1,arg2,result
enum OP_TYPE {
  _CONST_STATE = 1,
  _VAR_STATE,  // 2
  _GENE_LABEL, // 3
  _GENE_MAIN,  // 4
  _SCAN,       // 5
  _PRINT,      // 6
  _ADD,        // 7
  _SUB,        // 8
  _MULT,       // 9
  _DIV,        // 10
  _ASSIGN,     // 11
  _GET_ARRAY,  // 12
  _MOVE_PARA_TO_MEM,
  _RETURN,
  _PUSH, //调用函数时候 保存现场压栈
  _POP,  //函数调用结束，栈帧退回，参数取回
  _GENE_FUN_LABEL,
  _JAL_FUN,
  _MOVE_PARA_TO_STACK,
  _GENE_J, //无条件跳转
  _GENE_CONDI_J,
  _J_TO_MAIN,
  _MOVE_SP,
  _SAVE_V0
};

typedef struct temp_variate {
  //  char name[1000]; //目前可能不会使用//
  int name_index; //目前可能不会使用//
  int value;
  int offset;
  int type; // 1:int 2:char
} temp_variate;

typedef struct mcode {
  int op_type;   //是什么运算
  int arg1;      //第一个参数索引
  int arg2;      //第二个参数索引
  int result;    //结果索引
  int arg1_type; //参数1类型
                 // 1:内存变量，2:函数返回值(2为int型，-2为char型)，3:寄存器
  //,4:字符串, 5:立即数, 6:字符
  int arg2_type; //参数1类型 1:内存变量，2:函数返回值，3:寄存器
  //,4:字符串, 5:立即数, 6:字符
  int result_type; //参数1类型 1:内存变量，2:函数返回值，3:寄存器
  //,4:字符串, 5:立即数, 6:字符
  int a_off_dst;  //数组偏移量索引
  int a_off_type; //数组偏移量类型
} mcode;

typedef struct variate {
  int domain_num;
  char name[1000];
  int type;     // int 为1，char为2
  int is_const; //是为1 不是为0
  int value;
  int offset;
  int is_array;
} variate;

typedef struct function {
  char name[1000];
  int return_type; // none:0  int:1  char:2
  int para_num;
  int para_types[100]; // int：1   char:2
  char para_names[100][1000];
} function;

enum TYPE {
  IDENFR = 1,
  INTCON,
  CHARCON,
  STRCON,
  CONSTTK,  // 5
  INTTK,    // 6
  CHARTK,   // 7
  VOIDTK,   // 8
  MAINTK,   // 9
  IFTK,     // 10
  ELSETK,   // 11
  DOTK,     // 12
  WHILETK,  // 13
  FORTK,    // 14
  SCANFTK,  // 15
  PRINTFTK, // 16
  RETURNTK, // 17
  PLUS,
  MINU,
  MULT,
  DIV,
  LSS,
  LEQ,
  GRE,
  GEQ,
  EQL,
  NEQ,
  ASSIGN,
  SEMICN,
  COMMA,
  LPARENT,
  RPARENT,
  LBRACK,
  RBRACK,
  LBRACE,
  RBRACE,
  ERROE
};

#endif // UNTITLED3_DEFINES_H

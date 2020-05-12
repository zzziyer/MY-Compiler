#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char types[40][10] = {{"IDENFR"},   {"INTCON"}, {"CHARCON"}, {"STRCON"},
                      {"CONSTTK"},  {"INTTK"},  {"CHARTK"},  {"VOIDTK"},
                      {"MAINTK"},   {"IFTK"},   {"ELSETK"},  {"DOTK"},
                      {"WHILETK"},  {"FORTK"},  {"SCANFTK"}, {"PRINTFTK"},
                      {"RETURNTK"}, {"PLUS"},   {"MINU"},    {"MULT"},
                      {"DIV"},      {"LSS"},    {"LEQ"},     {"GRE"},
                      {"GEQ"},      {"EQL"},    {"NEQ"},     {"ASSIGN"},
                      {"SEMICN"},   {"COMMA"},  {"LPARENT"}, {"RPARENT"},
                      {"LBRACK"},   {"RBRACK"}, {"LBRACE"},  {"RBRACE"}};

extern int MAX; // token个数
extern int t_types[10000];
extern char tokens[10000][10000];
extern int line_num[10000];
extern int is_error[10000];

FILE *fout = NULL;
FILE *file_out = NULL;

// char return_names[1000][1000];
// char non_return_names[1000][1000];
int pointer = 0, temp_pointer = -1, fun_pointer = 0, var_pointer = 0,
    str_pointer = 0, temp_variate_pointer = 0, offset_pointer = 0;
int fp_pointer = 0, label_pointer = 0;
int current_type, temp_type;

temp_variate temp_vars[10000];
variate vars[10000];
function funs[10000];
char const_string[100][1000];

int domain_pointer = 0, domain_index[1000];
int statement_list(int fun_type);
int statement_single(int fun_type);
int expression(int *m_dst, int *m_type);
void value_para_list(int dst);
void call_return();
void assign_statement();

void retract_token() {
  pointer -= 1;
  temp_pointer -= 1;
}

int find_repe_name(int dest,
                   int type) { // type:0 查找常量或变量名，1：查找函数名
  int re = 0, i;
  if (type == 0) {
    if (domain_pointer == 0) { //是最外层常、变量
      for (i = 0; i < var_pointer; ++i) {
        if (strcmp(tokens[dest], vars[i].name) == 0) {
          re = 1;
        }
      }
    } else {
      //      for (i = 0; i < domain_index[0]; i++) {
      //        if (strcmp(tokens[dest], vars[i].name) == 0) {
      //          re = 1;
      //        }
      //      }
      for (i = domain_index[domain_pointer - 1]; i < var_pointer; i++) {
        if (strcmp(tokens[dest], vars[i].name) == 0) {
          re = 1;
        }
      }
    }
  } else {
    for (i = 0; i < domain_index[0]; i++) { //函数和全局常量不能重名
      if (strcmp(tokens[dest], vars[i].name) == 0) {
        re = 1;
      }
    }
    for (i = 0; i < fun_pointer; i++) { //函数和之前的函数不能重名
      if (strcmp(tokens[dest], funs[i].name) == 0) {
        re = 1;
      }
    }
  }
  return re;
}

int search_name(int k) { //只查找有返回值函数，若是为1，不是为0
  int i;
  int re_tag = 0;
  for (i = 0; i < fun_pointer; ++i) {
    if (strcmp(tokens[k], funs[i].name) == 0) {
      if (funs[i].return_type != 0) { //不为无返回值函数
        re_tag = 1;
      }
      break;
    }
  }
  return re_tag;
}

int find_undefined_name(int dest,
                        int type, // type:0 查找常量或变量名，1：查找函数名
                        int *re_type, // int :1，char:2
                        int *is_const) {
  int re = -1, i;
  if (type == 0) { //查变量
    for (i = domain_index[domain_pointer - 1]; i < var_pointer; i++) {
      if (strcmp(tokens[dest], vars[i].name) == 0) {
        *re_type = vars[i].type;
        *is_const = vars[i].is_const;
        re = i;
      }
    }               //在本层查找
    if (re == -1) { //若本层未找到则在全局查找
      for (i = 0; i < domain_index[0]; i++) {
        if (strcmp(tokens[dest], vars[i].name) == 0) {
          *re_type = vars[i].type;
          *is_const = vars[i].is_const;
          re = i;
        }
      }
    }
  } else {
    for (i = 0; i < fun_pointer; i++) {
      if (strcmp(tokens[dest], funs[i].name) == 0) {
        *re_type = funs[i].return_type;
        re = i;
      }
    }
  }
  return re;
}

void get_token() {
  if (pointer >= MAX) {
    exit(0);
  }
  while (t_types[pointer] == ERROE) {
    // fprintf(file_out, "%d a\n", line_num[pointer]);
    pointer++;
  }
  if (is_error[pointer]) {
    // fprintf(file_out, "%d a\n", line_num[pointer]);
  }
  current_type = t_types[pointer];
  temp_pointer = pointer;
  pointer++;
}

int get_temp_token(int off) {
  if (temp_pointer + off >= MAX) {
    return -1;
  } else {
    return t_types[temp_pointer + off];
  }
}

int non_sym_integer() { //<无符号整数> d
  int re = 0;
  for (int i = 0; tokens[pointer - 1][i] != '\0'; ++i) {
    re = re * 10 + tokens[pointer - 1][i] - '0';
  }
  // fprintf(fout, "<无符号整数>\n");
  return re;
}

int integer_() { //整数 d
  int m_tag = 0;
  if (current_type == PLUS || current_type == MINU) {
    if (current_type == MINU) {
      m_tag = 1;
    }
    get_token();
  }
  int re = non_sym_integer();
  if (m_tag == 1) {
    re = re * -1;
  }
  // fprintf(fout, "<整数>\n");
  return re;
}

void const_define() { //常量定义 √
  int c_tag = 1;
  int this_type = current_type;
  while (c_tag) {
    mcode_gener(_CONST_STATE, var_pointer, -1, -1, 1, 0, 0, -1, -1);
    get_token(); // biaozhifu
    strcpy(vars[var_pointer].name, tokens[pointer - 1]);
    if (find_repe_name(pointer - 1, 0)) {
      // fprintf(file_out, "%d b\n", line_num[pointer - 1]);
      //名字重定义
    }
    vars[var_pointer].domain_num = domain_pointer;
    get_token(); //=
    get_token();
    if (this_type == INTTK) {
      if (!(current_type == INTCON || current_type == PLUS ||
            current_type == MINU)) {
        // fprintf(file_out, "%d o\n", line_num[pointer - 1]);
        //常量定义中=后面只能是整型或字符型常量
      }
      vars[var_pointer].type = 1;
      vars[var_pointer].value = integer_();
    } else if (this_type == CHARTK) { //是字符则不管
      if (current_type != CHARCON) {
        // fprintf(file_out, "%d o\n", line_num[pointer - 1]);
        //常量定义中=后面只能是整型或字符型常量
      }
      vars[var_pointer].type = 2;
      vars[var_pointer].value = tokens[pointer - 1][0];
    }
    vars[var_pointer].offset = offset_pointer;
    offset_pointer += 1;
    vars[var_pointer].is_const = 1;
    var_pointer++;
    temp_type = get_temp_token(1);
    if (temp_type != COMMA) {
      c_tag = 0;
    } else {
      get_token();
    }
  }
  // fprintf(fout, "<常量定义>\n");
}

void variable_define() { //变量定义 d
  int v_tag = 1;
  int v_type = current_type;
  int num = 1;
  while (v_tag) {
    mcode_gener(_VAR_STATE, var_pointer, -1, -1, 1, 0, 0, -1, -1);
    if (v_type == INTTK) {
      vars[var_pointer].type = 1;
    } else if (v_type == CHARTK) {
      vars[var_pointer].type = 2;
    }
    vars[var_pointer].domain_num = domain_pointer;
    get_token(); // get biaozhif
    strcpy(vars[var_pointer].name, tokens[pointer - 1]);
    if (find_repe_name(pointer - 1, 0)) {
      // fprintf(file_out, "%d b\n", line_num[pointer - 1]);
      //名字重定义
    }
    temp_type = get_temp_token(1);
    if (temp_type == LBRACK) { //为[
      get_token();
      get_token();
      num = non_sym_integer();
      get_token(); //右括号]
      if (current_type != RBRACK) {
        // fprintf(file_out, "%d m\n", line_num[pointer - 2]);
        retract_token();
        //应为右中括号’]’
      }
    }
    vars[var_pointer].offset = offset_pointer;
    offset_pointer += num;
    vars[var_pointer].is_const = 0;
    var_pointer++;
    temp_type = get_temp_token(1);
    if (temp_type != COMMA) {
      v_tag = 0;
    } else {
      get_token();
    }
  }
  // fprintf(fout, "<变量定义>\n");
}

void const_state() { //常量说明 √
  int temp_tag = 1;
  while (temp_tag) {
    get_token();
    const_define();
    get_token(); // eat ';'
    if (current_type != SEMICN) {
      // fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
    temp_type = get_temp_token(1);
    if (temp_type != CONSTTK) {
      temp_tag = 0;
    } else {
      get_token();
    }
  }
  // fprintf(fout, "<常量说明>\n");
}

void variable_state() { //变量说明 d
  int temp_tag = 1;
  while (temp_tag) {
    variable_define();
    get_token(); //吞“；”
    if (current_type != SEMICN) {
      // fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
    temp_type = get_temp_token(1);
    if (temp_type == INTTK || temp_type == CHARTK) {
      temp_type = get_temp_token(3);
      if (temp_type == LPARENT) {
        temp_tag = 0;
      } else {
        get_token();
      }
    } else {
      temp_tag = 0;
    }
  }
  // fprintf(fout, "<变量说明>\n");
}

int state_head() { //<声明头部> d 有返回值  //返回当前函数索引
  //当前为int/char
  int footer = fun_pointer;
  if (current_type == INTTK) {
    funs[fun_pointer].return_type = 1;
  } else if (current_type == CHARTK) {
    funs[fun_pointer].return_type = 2;
  }
  get_token();
  strcpy(funs[fun_pointer].name, tokens[pointer - 1]);
  mcode_gener(_GENE_FUN_LABEL, fun_pointer, -1, -1, -1, -1, -1, -1, -1);
  if (find_repe_name(pointer - 1, 1)) {
    // fprintf(file_out, "%d b\n", line_num[pointer - 1]);
    //名字重定义
  }
  fun_pointer++;
  // fprintf(fout, "<声明头部>\n");
  return footer;
}

void param_list(int footer) { //<参数表>
  temp_type = get_temp_token(1);
  if (temp_type == RPARENT) {
    // fprintf(fout, "<参数表>\n");
    funs[footer].para_num = 0;
    return;
  }
  int p_num = 0;
  get_token(); // int/char
  int p_tag = 1;
  while (p_tag) {
    int p_type = 0;
    if (current_type == INTTK) {
      p_type = 1;
    } else if (current_type == CHARTK) {
      p_type = 2;
    }
    get_token(); // biaozhifu
    strcpy(funs[footer].para_names[p_num], tokens[pointer - 1]);
    funs[footer].para_types[p_num] = p_type;
    if (p_num <= 3) {
      mcode_gener(_MOVE_PARA_TO_MEM, p_num, -1, var_pointer, 5, -1, 1, footer,
                  p_num);
    } else {
      mcode_gener(_MOVE_PARA_TO_MEM, p_num - 4, -1, var_pointer, 1, -1, 1,
                  footer, p_num);
    }
    p_num++;
    vars[var_pointer].is_const = 0;
    vars[var_pointer].domain_num = domain_pointer;
    vars[var_pointer].type = p_type;
    vars[var_pointer].offset = offset_pointer;
    offset_pointer++;
    strcpy(vars[var_pointer].name, tokens[pointer - 1]);
    if (find_repe_name(pointer - 1, 0)) {
      // fprintf(file_out, "%d b\n", line_num[pointer - 1]);
      //名字重定义
    }
    var_pointer++;
    temp_type = get_temp_token(1);
    if (temp_type != COMMA) {
      p_tag = 0;
    } else {
      get_token(); // eat ','
      get_token();
    }
  }
  funs[footer].para_num = p_num;
  // fprintf(fout, "<参数表>\n");
}

int factor(int *f_dst, int *f_type) { //<因子>
  int re_type = 0;
  int whatever = 0;
  if (current_type == IDENFR) {
    temp_type = get_temp_token(1);
    if (temp_type == LBRACK) { //数组
      int d = find_undefined_name(pointer - 1, 0, &re_type, &whatever);
      if (d == -1) {
        // fprintf(file_out, "%d c\n", line_num[pointer - 1]);
        //名字未定义
      }
      get_token(); //[
      get_token();
      int array_dst, array_type;
      int type = expression(&array_dst, &array_type);
      if (type != 1) {
        // fprintf(file_out, "%d i\n", line_num[pointer - 1]);
        //数组元素的下标只能是整型表达式
      }
      get_token(); //]
      if (current_type != RBRACK) {
        // fprintf(file_out, "%d m\n", line_num[pointer - 2]);
        retract_token();
        //应为右中括号’]’
      }
      mcode_gener(_GET_ARRAY, d, array_dst, temp_variate_pointer, 1, array_type,
                  3, -1, -1);
      temp_vars[temp_variate_pointer].offset = offset_pointer;
      *f_dst = temp_variate_pointer;
      *f_type = 3;
      offset_pointer++;
      temp_variate_pointer++;
    } else if (temp_type == LPARENT) { //有返回值函数调用
      int d = find_undefined_name(pointer - 1, 1, &re_type, &whatever);
      if (d == -1) {
        // fprintf(file_out, "%d c\n", line_num[pointer - 1]);
        //名字未定义
      }
      call_return();
      //      *f_dst = -1;
      //      if (funs[d].return_type == 1) {
      //        *f_type = 2;
      //      } else {
      //        *f_type = -2;
      //      }
      temp_vars[temp_variate_pointer].offset = offset_pointer;
      temp_vars[temp_variate_pointer].type = funs[d].return_type;
      mcode_gener(_SAVE_V0, temp_variate_pointer, -1, -1, 3, -1, -1, -1, -1);
      *f_dst = temp_variate_pointer;
      *f_type = 3;
      offset_pointer++;
      temp_variate_pointer++;
    } else { //否则就是单独标识符  do nothing
      int d = find_undefined_name(pointer - 1, 0, &re_type, &whatever);
      if (d == -1) {
        // fprintf(file_out, "%d c\n", line_num[pointer - 1]);
      }
      *f_dst = d;
      *f_type = 1;
    }
  } else if (current_type == LPARENT) {
    int temp_dst, temp_t;
    re_type = 1;
    get_token();
    expression(&temp_dst, &temp_t);
    get_token(); //)
    if (current_type != RPARENT) {
      // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
      retract_token();
      //应为右小括号’)’
    }
    *f_dst = temp_dst;
    *f_type = temp_t;
  } else if (current_type == CHARCON) { //字符
    re_type = 2;
    // do nothing
    int num = tokens[pointer - 1][0];
    *f_dst = num;
    *f_type = 6;
  } else { //为整数
    re_type = 1;
    int num = integer_();
    *f_dst = num;
    *f_type = 5;
  }
  // fprintf(fout, "<因子>\n");
  return re_type;
}

int item(int *i_dst, int *i_type) { //<项>
  int re_type = 0;
  int i_tag = 1;
  int is_single = 1;
  int f_dst = 0, f_type = 0, pre_f_dst = 0, pre_f_type = 0;
  int m_d_tag = 0;  // mult:1,divi:2
  int minu_tag = 0; //为1时表示项有一个负号
  if (current_type == PLUS || current_type == MINU) {
    // temp_type = get_temp_token(1);
    // if (temp_type == PLUS || temp_type == MINU) {
    if (current_type == MINU) {
      minu_tag = 1;
    }
    get_token();
    //}
  }
  while (i_tag) {
    pre_f_dst = f_dst;
    pre_f_type = f_type;
    int temp = factor(&f_dst, &f_type);
    if (is_single) {
      re_type = temp;
      is_single = 0;
    } else {
      if (m_d_tag == 1) {
        mcode_gener(_MULT, pre_f_dst, f_dst, temp_variate_pointer, pre_f_type,
                    f_type, 3, -1, -1);
      } else {
        mcode_gener(_DIV, pre_f_dst, f_dst, temp_variate_pointer, pre_f_type,
                    f_type, 3, -1, -1);
      }
      temp_vars[temp_variate_pointer].offset = offset_pointer;
      offset_pointer++;
      //    pre_f_dst=f_dst;
      //    pre_f_type=f_type;
      f_dst = temp_variate_pointer;
      f_type = 3;
      temp_variate_pointer++;
      re_type = 1;
    }
    temp_type = get_temp_token(1);
    if (temp_type == MULT || temp_type == DIV) {
      if (temp_type == MULT) {
        m_d_tag = 1;
      } else {
        m_d_tag = 2;
      }
      get_token(); // */
      get_token();
    } else {
      i_tag = 0;
    }
  }
  if (minu_tag == 1) {
    mcode_gener(_SUB, 0, f_dst, temp_variate_pointer, 5, f_type, 3, -1, -1);
    temp_vars[temp_variate_pointer].offset = offset_pointer;
    offset_pointer++;
    f_dst = temp_variate_pointer;
    f_type = 3;
    temp_variate_pointer++;
  }
  *i_dst = f_dst;
  *i_type = f_type;
  // fprintf(fout, "<项>\n");
  return re_type;
}

int expression(int *m_dst, int *m_type) { //<表达式>
  int re_type = 0;
  int is_single = 1; //是否是单独项
  int p_m_tag = 0;   // plus:1,minu:2
  int i_dst = 0, i_type = 0, pre_i_dst = 0, pre_i_type = 0;
  int e_tag = 1;
  while (e_tag) {
    pre_i_dst = i_dst;
    pre_i_type = i_type;
    int temp = item(&i_dst, &i_type);
    if (is_single) {
      re_type = temp;
      is_single = 0;
    } else {
      if (p_m_tag == 1) {
        mcode_gener(_ADD, pre_i_dst, i_dst, temp_variate_pointer, pre_i_type,
                    i_type, 3, -1, -1);
      } else {
        mcode_gener(_SUB, pre_i_dst, i_dst, temp_variate_pointer, pre_i_type,
                    i_type, 3, -1, -1);
      }
      temp_vars[temp_variate_pointer].offset = offset_pointer;
      offset_pointer++;
      //      pre_i_dst = i_dst;
      //      pre_i_type = i_type;
      i_dst = temp_variate_pointer;
      i_type = 3;
      temp_variate_pointer++;
      re_type = 1;
    }
    temp_type = get_temp_token(1);
    if (temp_type == PLUS || temp_type == MINU) {
      if (temp_type == PLUS) {
        p_m_tag = 1;
      } else {
        p_m_tag = 2;
      }
      get_token(); //+-
      get_token();
    } else {
      e_tag = 0;
    }
  }
  *m_dst = i_dst;
  *m_type = i_type;
  // fprintf(fout, "<表达式>\n");
  return re_type;
}

void conditionnnn(int label) { //<条件>
  int exp1_dst, exp1_type;
  int exp2_dst, exp2_type;
  int tag = 0;
  int type = expression(&exp1_dst, &exp1_type);
  if (type != 1) {
    // fprintf(file_out, "%d f\n", line_num[pointer - 1]);
    //条件判断中出现不合法的类型
  }
  temp_type = get_temp_token(1);
  if (temp_type == LSS || temp_type == LEQ || temp_type == GRE ||
      temp_type == GEQ || temp_type == EQL || temp_type == NEQ) {
    int temp = temp_type;
    tag = 1;
    get_token();
    get_token();
    type = expression(&exp2_dst, &exp2_type);
    mcode_gener(_GENE_CONDI_J, exp1_dst, temp, exp2_dst, exp1_type, -1,
                exp2_type, label, -1);
    if (type != 1) {
      // fprintf(file_out, "%d f\n", line_num[pointer - 1]);
      //条件判断中出现不合法的类型
    }
  }
  if (tag == 0) {
    mcode_gener(_GENE_CONDI_J, exp1_dst, -1, -1, exp1_type, -1, -1, label, -1);
  }
  // fprintf(fout, "<条件>\n");
}

void condition_statement(int fun_type) { //<条件语句>
  int lab1 = label_pointer++, lab2 = label_pointer++;
  get_token();
  get_token();
  conditionnnn(lab1); //条件
  get_token();        // est )
  if (current_type != RPARENT) {
    // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  get_token();
  statement_single(fun_type);
  mcode_gener(_GENE_J, lab2, -1, -1, -1, -1, -1, -1, -1);
  mcode_gener(_GENE_LABEL, lab1, -1, -1, -1, -1, -1, -1, -1);
  temp_type = get_temp_token(1);
  if (temp_type == ELSETK) {
    get_token(); // est else
    get_token();
    statement_single(fun_type);
  }
  mcode_gener(_GENE_LABEL, lab2, -1, -1, -1, -1, -1, -1, -1);
  // fprintf(fout, "<条件语句>\n");
}

void step_length() { //<步长>
  non_sym_integer();
  // fprintf(fout, "<步长>\n");
}

void circul_statement(int fun_type) { //<循环语句>
  int whatever;
  if (current_type == DOTK) {
    int lab1 = label_pointer++, lab2 = label_pointer++;
    //    printf("lable1=%d\n", lab1);
    //    printf("lable2=%d\n", lab2);
    mcode_gener(_GENE_LABEL, lab1, -1, -1, -1, -1, -1, -1, -1);
    //    printf("lable1=%d\n", lab1);
    //    printf("lable2=%d\n", lab2);
    get_token(); //
    statement_single(fun_type);
    get_token(); // while
    if (current_type != WHILETK) {
      // //fprintf(file_out, "%d n\n", line_num[pointer - 2]);
      retract_token();
      retract_token();
      // do-while应为语句中缺少while
    }
    get_token(); //(
    get_token();
    conditionnnn(lab2);
    mcode_gener(_GENE_J, lab1, -1, -1, -1, -1, -1, -1, -1);
    mcode_gener(_GENE_LABEL, lab2, -1, -1, -1, -1, -1, -1, -1);
    //    printf("lable1=%d\n", lab1);
    //    printf("lable2=%d\n", lab2);
    get_token(); //)
    if (current_type != RPARENT) {
      // //fprintf(file_out, "%d l\n", line_num[pointer - 2]);
      retract_token();
      //应为右小括号’)’
    }
  } else if (current_type == WHILETK) {
    int lab1 = label_pointer++, lab2 = label_pointer++;
    mcode_gener(_GENE_LABEL, lab1, -1, -1, -1, -1, -1, -1, -1);
    get_token(); //(
    get_token();
    conditionnnn(lab2);
    get_token(); //)
    if (current_type != RPARENT) {
      // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
      retract_token();
      //应为右小括号’)’
    }
    get_token();
    statement_single(fun_type);
    mcode_gener(_GENE_J, lab1, -1, -1, -1, -1, -1, -1, -1);
    mcode_gener(_GENE_LABEL, lab2, -1, -1, -1, -1, -1, -1, -1);
  } else if (current_type == FORTK) {
    int lab1 = label_pointer++, lab2 = label_pointer++;
    int lab3 = label_pointer++, lab4 = label_pointer++;
    get_token();        //(
    get_token();        //标识符
    assign_statement(); //初始化
    mcode_gener(_GENE_LABEL, lab1, -1, -1, -1, -1, -1, -1, -1);
    //    if (find_undefined_name(pointer - 1, 0, &whatever, &whatever) == -1) {
    //      //fprintf(file_out, "%d c\n", line_num[pointer - 1]);
    //      //名字未定义
    //    }
    //    get_token(); //=
    //    get_token();
    //    expression(&whatever, &whatever);
    get_token(); //;
    if (current_type != SEMICN) {
      //  //fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
    get_token();
    conditionnnn(lab4);
    mcode_gener(_GENE_J, lab3, -1, -1, -1, -1, -1, -1, -1);
    mcode_gener(_GENE_LABEL, lab2, -1, -1, -1, -1, -1, -1, -1);
    get_token(); //;
    if (current_type != SEMICN) {
      // //fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
    get_token();        //标识符
    assign_statement(); //自增
    mcode_gener(_GENE_J, lab1, -1, -1, -1, -1, -1, -1, -1);
    mcode_gener(_GENE_LABEL, lab3, -1, -1, -1, -1, -1, -1, -1);

    //    if (find_undefined_name(pointer - 1, 0, &whatever, &whatever) == -1) {
    //      //fprintf(file_out, "%d c\n", line_num[pointer - 1]);
    //      //名字未定义
    //    }
    //    get_token(); //=
    //    get_token(); //标识符
    //    if (find_undefined_name(pointer - 1, 0, &whatever, &whatever) == -1) {
    //      //fprintf(file_out, "%d c\n", line_num[pointer - 1]);
    //      //名字未定义
    //    }
    //    get_token(); //+\-
    //    get_token();
    //    step_length(); //步长
    get_token(); //)
    if (current_type != RPARENT) {
      // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
      retract_token();
      //应为右小括号’)’
    }
    get_token();
    statement_single(fun_type);
    mcode_gener(_GENE_J, lab2, -1, -1, -1, -1, -1, -1, -1);
    mcode_gener(_GENE_LABEL, lab4, -1, -1, -1, -1, -1, -1, -1);
  }
  // fprintf(fout, "<循环语句>\n");
}
// TODO 未考虑：名字未定义中，函数调用时未分有返回值和无返回值
void assign_statement() { //<赋值语句>
  int whatever, is_const;
  int dst1, type1;
  int d = find_undefined_name(pointer - 1, 0, &whatever, &is_const);
  if (d == -1) {
    // //fprintf(file_out, "%d c\n", line_num[pointer - 1]);
    //名字未定义
  }
  if (is_const == 1) {
    // //fprintf(file_out, "%d j\n", line_num[pointer - 1]);
    //不能改变常量的值
  }
  temp_type = get_temp_token(1);
  if (temp_type == ASSIGN) {
    get_token(); //=
    get_token();
    expression(&dst1, &type1);
    mcode_gener(_ASSIGN, dst1, -1, d, type1, -1, 1, -1, -1);
  } else { //是[
    int dst2, type2;
    get_token(); //[
    get_token();
    int type = expression(&dst2, &type2);
    if (type != 1) {
      // //fprintf(file_out, "%d i\n", line_num[pointer - 1]);
      //数组元素的下标只能是整型表达式
    }
    get_token(); //]
    if (current_type != RBRACK) {
      //  //fprintf(file_out, "%d m\n", line_num[pointer - 2]);
      retract_token();
      //应为右中括号’]’
    }
    get_token(); //=
    get_token();
    expression(&dst1, &type1);
    mcode_gener(_ASSIGN, dst1, -1, d, type1, -1, 1, dst2, type2);
  }
  // fprintf(fout, "<赋值语句>\n");
}

int find_func_dst(int name_dst) {
  int func_dst = 0, i;
  for (i = 0; i < fun_pointer; i++) {
    if (strcmp(funs[i].name, tokens[name_dst]) == 0) {
      func_dst = i;
      break;
    }
  }
  return func_dst;
}

void value_para_list(int dst) { //<值参数表>
  int valu_para = 0;
  int whatever;
  int key = find_func_dst(dst);
  temp_type = get_temp_token(1);
  if (temp_type == RPARENT) {
    if (valu_para != funs[key].para_num) {
      // fprintf(file_out, "%d d\n", line_num[dst]);
      //参数个数不匹配
    }
    // fprintf(fout, "<值参数表>\n");
    return;
  }
  get_token();
  int v_tag = 1;
  while (v_tag) {
    int e_dst, e_typ;
    int type = expression(&e_dst, &e_typ);
    if (type != funs[key].para_types[valu_para]) {
      // //fprintf(file_out, "%d e\n", line_num[dst]);
      //参数类型不匹配
    }
    if (valu_para <= 3) {
      mcode_gener(_MOVE_PARA_TO_STACK, e_dst, -1, valu_para, e_typ, -1, 5, -1,
                  -1);
    } else {
      mcode_gener(_MOVE_PARA_TO_STACK, e_dst, -1, valu_para - 4, e_typ, -1, -1,
                  -1, -1);
    }
    valu_para++;
    temp_type = get_temp_token(1);
    if (temp_type != COMMA) {
      v_tag = 0;
    } else {
      get_token(); //,
      get_token();
    }
  }
  if (valu_para != funs[key].para_num) {
    // fprintf(file_out, "%d d\n", line_num[dst]);
    //参数个数不匹配
  }
  // fprintf(fout, "<值参数表>\n");
}

void call_return() { // <有返回值函数调用语句>
  mcode_gener(_PUSH, fp_pointer, -1, -1, -1, -1, -1, -1, -1);
  fp_pointer += 5;
  // mcode_gener(_MOVE_SP, 128, -1, -1, -1, -1, 1, -1, -1);
  int whatever;
  int d = find_undefined_name(pointer - 1, 1, &whatever, &whatever);
  if (d == -1) {
    // fprintf(file_out, "%d c\n", line_num[pointer - 1]);
    //名字未定义
  }
  int dst = pointer - 1;
  get_token(); //(
  value_para_list(dst);
  mcode_gener(_JAL_FUN, d, -1, -1, -1, -1, -1, -1, -1);
  // mcode_gener(_GENE_FUN_LABEL, pointer - 1, -1, -1, -1, -1, -1, -1, -1);
  get_token(); //)
  /*
  if (current_type != RPARENT) {
    //fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  */
  // mcode_gener(_MOVE_SP, -128, -1, -1, -1, -1, 1, -1, -1);
  fp_pointer -= 5;
  mcode_gener(_POP, fp_pointer, -1, -1, -1, -1, -1, -1, -1);
  // fprintf(fout, "<有返回值函数调用语句>\n");
}

void call_non_return() { //<无返回值函数调用语句>
  mcode_gener(_PUSH, fp_pointer, -1, -1, -1, -1, -1, -1, -1);
  fp_pointer += 5;
  // mcode_gener(_MOVE_SP, 128, -1, -1, -1, -1, 1, -1, -1);
  int whatever;
  int d = find_undefined_name(pointer - 1, 1, &whatever, &whatever);
  if (d == -1) {
    // fprintf(file_out, "%d c\n", line_num[pointer - 1]);
    //名字未定义
  }
  int dst = pointer - 1;
  get_token(); //(
  value_para_list(dst);
  mcode_gener(_JAL_FUN, d, -1, -1, -1, -1, -1, -1, -1);
  get_token(); //)
  if (current_type != RPARENT) {
    // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  fp_pointer -= 5;
  mcode_gener(_POP, fp_pointer, -1, -1, -1, -1, -1, -1, -1);
  // fprintf(fout, "<无返回值函数调用语句>\n");
}

void scan_statement() { //<读语句>
  int whatever;
  get_token(); //(
  int s_tag = 1;
  while (s_tag) {
    get_token(); // 读标识符
    int read = find_undefined_name(pointer - 1, 0, &whatever, &whatever);
    if (read == -1) {
      //  //fprintf(file_out, "%d c\n", line_num[pointer - 1]);
      //名字未定义
    }
    mcode_gener(_SCAN, read, -1, -1, 1, 0, 0, -1, -1);
    temp_type = get_temp_token(1);
    if (temp_type != COMMA) {
      s_tag = 0;
    } else {
      get_token(); //,
    }
  }
  get_token(); //)
  if (current_type != RPARENT) {
    // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  // fprintf(fout, "<读语句>\n");
}

void print_statement() { //<写语句>
  int m_dst, m_type;
  get_token(); //(
  temp_type = get_temp_token(1);
  if (temp_type == STRCON) {
    get_token();
    strcpy(const_string[str_pointer], tokens[pointer - 1]);
    int temp_s = str_pointer;
    str_pointer++;
    // fprintf(fout, "<字符串>\n");
    temp_type = get_temp_token(1);
    if (temp_type == COMMA) {
      get_token(); //,
      get_token();
      expression(&m_dst, &m_type);
      mcode_gener(_PRINT, temp_s, -1, -1, 4, 0, 0, 1, -1);
      mcode_gener(_PRINT, m_dst, -1, -1, m_type, 0, 0, -1, -1);
    } else {
      mcode_gener(_PRINT, temp_s, -1, -1, 4, 0, 0, -1, -1);
    }
  } else {
    get_token();
    expression(&m_dst, &m_type);
    mcode_gener(_PRINT, m_dst, -1, -1, m_type, 0, 0, -1, -1);
  }
  get_token(); //)
  if (current_type != RPARENT) {
    // //fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  // //fprintf(fout, "<写语句>\n");
}

int return_statement(
    int fun_type) { //<返回语句> int返回1，char返回2 无返回值为0
  int whatever;
  temp_type = get_temp_token(1);
  if (temp_type != LPARENT) {
    // fprintf(fout, "<返回语句>\n");
    /*
    if (fun_type != 0) {
      //fprintf(file_out, "%d h\n", line_num[pointer - 1]);
      //有返回值的函数存在不匹配的return语句
    }
    */
    mcode_gener(_RETURN, 0, -1, -1, -1, -1, -1, -1, -1);
    // mcode_gener(_MOVE_SP, -128, -1, -1, -1, -1, 1, -1, -1);
    return 0;
  }
  /*
  if (fun_type == 0) {
    //fprintf(file_out, "%d g\n", line_num[pointer - 1]);
    //无返回值的函数存在不匹配的return语句
  }
  */
  get_token(); //(
  get_token();
  int dst, typ;
  int type = expression(&dst, &typ);
  mcode_gener(_RETURN, type, -1, dst, -1, -1, typ, -1, -1);
  // mcode_gener(_MOVE_SP, -128, -1, -1, -1, -1, 1, -1, -1);

  /*
  if (type != fun_type) {
    //fprintf(file_out, "%d h\n", line_num[pointer - 1]);
    //有返回值的函数存在不匹配的return语句
  }
  */
  get_token(); //)
  if (current_type != RPARENT) {
    // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  // fprintf(fout, "<返回语句>\n");
}

int statement_single(int fun_type) { //<语句>
  int has_return = 0;
  if (current_type == IFTK) {
    condition_statement(fun_type);
  } else if (current_type == DOTK || current_type == WHILETK ||
             current_type == FORTK) {
    circul_statement(fun_type);
  } else if (current_type == LBRACE) {
    //不get
    statement_list(fun_type);
    get_token(); //}
  } else if (current_type == IDENFR) {
    temp_type = get_temp_token(1);
    if (temp_type == ASSIGN || temp_type == LBRACK) { //赋值语句
      assign_statement();
    } else {
      if (search_name(temp_pointer)) { //是有返回
        call_return();
      } else {
        call_non_return();
      }
    }
    get_token(); // eat ';'
    if (current_type != SEMICN) {
      // fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
  } else if (current_type == SCANFTK) {
    scan_statement();
    get_token();
    if (current_type != SEMICN) {
      // fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
  } else if (current_type == PRINTFTK) {
    print_statement();
    get_token();
    if (current_type != SEMICN) {
      // fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
  } else if (current_type == SEMICN) {
    // get_token();
    if (current_type != SEMICN) {
      // fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
  } else if (current_type == RETURNTK) {
    has_return = 1;
    return_statement(fun_type);
    get_token();
    if (current_type != SEMICN) {
      // fprintf(file_out, "%d k\n", line_num[pointer - 2]);
      retract_token();
      //应为分号
    }
  }
  // fprintf(fout, "<语句>\n");
  return has_return;
}

int statement_list(int fun_type) { //<语句列>
  // 有return语句 返回1，无返回0，根据传入的type报错
  int has_return = 0;
  temp_type = get_temp_token(1);
  if (temp_type == RBRACE) {
    // fprintf(fout, "<语句列>\n");
    return 0;
  }
  get_token();
  int s_tag = 1;
  while (s_tag) {
    int x = statement_single(fun_type);
    if (has_return == 0) {
      has_return = x;
    }
    temp_type = get_temp_token(1);
    if (temp_type == RBRACE) {
      s_tag = 0;
    } else {
      get_token();
    }
  }
  // fprintf(fout, "<语句列>\n");
  return has_return;
}

int compound_statement(int fun_type) { //<复合语句>
  // 有return语句 返回1，无返回0，根据传入的type报错
  temp_type = get_temp_token(1);
  if (temp_type == RBRACE) {
    // fprintf(fout, "<语句列>\n");
    // fprintf(fout, "<复合语句>\n");
    return 0;
  }
  temp_type = get_temp_token(1);
  if (temp_type == CONSTTK) {
    get_token();
    const_state();
  }
  temp_type = get_temp_token(1);
  if (temp_type == RBRACE) {
    // fprintf(fout, "<语句列>\n");
    // fprintf(fout, "<复合语句>\n");
    return 0;
  }
  temp_type = get_temp_token(1);
  if (temp_type == INTTK || temp_type == CHARTK) {
    get_token();
    variable_state();
  }
  int has_retun = statement_list(fun_type);
  // fprintf(fout, "<复合语句>\n");
  return has_retun;
}

//约定:进入参数表之前不get，因为参数表可能为空
//复合语句、语句列、值参列表同理，还有主程序的部分与各个函数大括号中的部分

int return_fun_state() { //<有返回值函数定义>
  offset_pointer = 0;
  int func_type = 0;
  if (current_type == INTTK) {
    func_type = 1;
  } else {
    func_type = 2;
  }
  int temp_fun_footer = state_head(); //声明头部
  get_token();                        // eat '('
  param_list(temp_fun_footer);
  get_token(); // eat )
  /*
  if (current_type != RPARENT) {
    //fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  */
  get_token(); // eat {
  // get_token();
  int has_return = compound_statement(func_type);
  get_token(); // eat }
  // fprintf(fout, "<有返回值函数定义>\n");
  return has_return;
}

void non_return_fun_state() { //<无返回值函数定义>
  offset_pointer = 0;
  int footer = fun_pointer;
  get_token(); // biaozhifu
  funs[fun_pointer].return_type = 0;
  strcpy(funs[fun_pointer].name, tokens[pointer - 1]);
  mcode_gener(_GENE_FUN_LABEL, fun_pointer, -1, -1, -1, -1, -1, -1, -1);
  if (find_repe_name(pointer - 1, 1)) {
    // fprintf(file_out, "%d b\n", line_num[pointer - 1]);
    //名字重定义
  }
  fun_pointer++;
  get_token(); // eat (
  // get_token();
  param_list(footer); //参数表
  get_token();        //)
  if (current_type != RPARENT) {
    // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  get_token(); //{
  // get_token();
  int has_return = compound_statement(0);
  if (has_return == 0) {
    mcode_gener(_RETURN, 0, -1, -1, -1, -1, -1, -1, -1);
    // mcode_gener(_MOVE_SP, -128, -1, -1, -1, -1, 1, -1, -1);
  }
  get_token(); //}
  // fprintf(fout, "<无返回值函数定义>\n");
}

void main_func() { //<主函数>
  // current=void
  get_token(); // main
  mcode_gener(_GENE_MAIN, -1, -1, -1, 0, 0, 0, -1, -1);
  get_token(); //(
  get_token(); //)
  if (current_type != RPARENT) {
    // fprintf(file_out, "%d l\n", line_num[pointer - 2]);
    retract_token();
    //应为右小括号’)’
  }
  get_token(); //{
  //
  compound_statement(0);
  get_token();
  // fprintf(fout, "<主函数>\n");
  mcode_gener(_RETURN, 0, -1, -1, -1, -1, -1, -1, -1);
}

void program() { //<程序>
  temp_type = get_temp_token(1);
  if (temp_type == CONSTTK) {
    get_token();
    const_state();
  }
  temp_type = get_temp_token(3); // variable or returnable func?
  int temp_type1 = get_temp_token(1);
  if ((temp_type1 == INTTK || temp_type1 == CHARTK) && temp_type != LPARENT) {
    get_token();
    variable_state();
  }
  mcode_gener(_J_TO_MAIN, -1, -1, 1, -1, -1, -1, -1, -1);
  domain_index[domain_pointer] = var_pointer;
  domain_pointer++; //进入下一个定义域
  temp_type1 = get_temp_token(1);
  temp_type = get_temp_token(2);
  while (temp_type1 == INTTK || temp_type1 == CHARTK ||
         (temp_type1 == VOIDTK && temp_type == IDENFR)) {
    int type = 0;
    if (temp_type1 == INTTK || temp_type1 == CHARTK) {
      get_token();
      type = return_fun_state();
      if (type == 0) {
        // fprintf(file_out, "%d h\n", line_num[pointer - 1]);
        //有返回值的函数缺少return语句
      }
    } else {
      get_token();
      non_return_fun_state();
    }
    temp_type1 = get_temp_token(1);
    temp_type = get_temp_token(2);
    domain_index[domain_pointer] = var_pointer;
    domain_pointer++; //进入下一个定义域
  }
  get_token();
  main_func();
  domain_index[domain_pointer] = var_pointer;
  domain_pointer++; //进入下一个定义域
  // fprintf(fout, "<程序>\n");
}

int main() {
  fout = fopen("output.txt", "w");
  file_out = fopen("error.txt", "w");
  lexical();
  // get_token();
  program();
  print_();
  mips_gener();
}

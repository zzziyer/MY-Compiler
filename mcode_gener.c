//
// Created by asus on 2019/11/19.
//

#include "defines.h"
#include <stdio.h>
#include <string.h>

FILE *m_code_be;
FILE *m_code_af;

mcode mcode_list[10000];
int mcode_pointer = 0;
extern variate vars[10000];
extern function funs[10000];
extern char const_string[100][1000];
void mcode_printer(int t, FILE *out);

void print_() {
  int i;
  m_code_be = fopen("17373301_校怡哲_优化前中间代码.txt", "w");
  m_code_af = fopen("17373301_校怡哲_优化后中间代码.txt", "w");
  for (i = 0; i < mcode_pointer; ++i) {
    mcode_printer(i, m_code_be);
    mcode_printer(i, m_code_af);
  }
}

void mcode_printer(int t, FILE *out) {
  switch (mcode_list[t].op_type) {
  case _CONST_STATE: {
    fprintf(out, "const ");
    if (vars[mcode_list[t].arg1].type == 1) {
      fprintf(out, "int ");
    } else {
      fprintf(out, "char ");
    }
    fprintf(out, "%s=%d\n", vars[mcode_list[t].arg1].name,
            vars[mcode_list[t].arg1].value);
    break;
  }
  case _VAR_STATE: {
    if (vars[mcode_list[t].arg1].type == 1) {
      fprintf(out, "int ");
    } else {
      fprintf(out, "char ");
    }
    fprintf(out, "%s\n", vars[mcode_list[t].arg1].name);
    break;
  }
  case _GENE_LABEL: {
    fprintf(out, "label%d:\n", mcode_list[t].arg1);
    break;
  }
  case _GENE_MAIN: {
    fprintf(out, "main:\n");
    break;
  }
  case _SCAN: {
    fprintf(out, "scan ");
    fprintf(out, "%s\n", vars[mcode_list[t].arg1].name);
    break;
  }
  case _PRINT: {
    fprintf(out, "print ");
    if (mcode_list[t].arg1_type == 4) {
      fprintf(out, "\"%s\"\n", const_string[mcode_list[t].arg1]);
    } else if (mcode_list[t].arg1_type == 5) {
      fprintf(out, "%d\n", mcode_list[t].arg1);
    } else if (mcode_list[t].arg1_type == 6) {
      fprintf(out, "%c\n", mcode_list[t].arg1);
    } else if (mcode_list[t].arg1_type == 1) {
      fprintf(out, "%s\n", vars[mcode_list[t].arg1].name);
    } else if (mcode_list[t].arg1_type == 3) {
      fprintf(out, "t%d\n", mcode_list[t].arg1);
    }
    break;
  }
  case _ADD: {
    int temp1 = mcode_list[t].arg1_type;
    int temp2 = mcode_list[t].arg2_type;
    int temp3 = mcode_list[t].result_type;
    if (temp3 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].result].name);
    } else if (temp3 == 3) {
      fprintf(out, "t%d", mcode_list[t].result);
    } else if (temp3 == 5) {
      fprintf(out, "%d", mcode_list[t].result);
    }
    fprintf(out, " = ");
    if (temp1 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg1].name);
    } else if (temp1 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg1);
    } else if (temp1 == 5) {
      fprintf(out, "%d", mcode_list[t].arg1);
    }
    fprintf(out, " + ");
    if (temp2 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg2].name);
    } else if (temp2 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg2);
    } else if (temp2 == 5) {
      fprintf(out, "%d", mcode_list[t].arg2);
    }
    fprintf(out, "\n");
    break;
  }
  case _SUB: {
    int temp1 = mcode_list[t].arg1_type;
    int temp2 = mcode_list[t].arg2_type;
    int temp3 = mcode_list[t].result_type;
    if (temp3 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].result].name);
    } else if (temp3 == 3) {
      fprintf(out, "t%d", mcode_list[t].result);
    } else if (temp3 == 5 || temp3 == 6) {
      fprintf(out, "%d", mcode_list[t].result);
    }
    fprintf(out, " = ");
    if (temp1 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg1].name);
    } else if (temp1 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg1);
    } else if (temp1 == 5 || temp1 == 6) {
      fprintf(out, "%d", mcode_list[t].arg1);
    }
    fprintf(out, " - ");
    if (temp2 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg2].name);
    } else if (temp2 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg2);
    } else if (temp2 == 5 || temp2 == 6) {
      fprintf(out, "%d", mcode_list[t].arg2);
    }
    fprintf(out, "\n");
    break;
  }
  case _MULT: {
    int temp1 = mcode_list[t].arg1_type;
    int temp2 = mcode_list[t].arg2_type;
    int temp3 = mcode_list[t].result_type;
    if (temp3 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].result].name);
    } else if (temp3 == 3) {
      fprintf(out, "t%d", mcode_list[t].result);
    } else if (temp3 == 5 || temp3 == 6) {
      fprintf(out, "%d", mcode_list[t].result);
    }
    fprintf(out, " = ");
    if (temp1 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg1].name);
    } else if (temp1 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg1);
    } else if (temp1 == 5 || temp1 == 6) {
      fprintf(out, "%d", mcode_list[t].arg1);
    }
    fprintf(out, " * ");
    if (temp2 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg2].name);
    } else if (temp2 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg2);
    } else if (temp2 == 5 || temp2 == 6) {
      fprintf(out, "%d", mcode_list[t].arg2);
    }
    fprintf(out, "\n");
    break;
  }
  case _DIV: {
    int temp1 = mcode_list[t].arg1_type;
    int temp2 = mcode_list[t].arg2_type;
    int temp3 = mcode_list[t].result_type;
    if (temp3 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].result].name);
    } else if (temp3 == 3) {
      fprintf(out, "t%d", mcode_list[t].result);
    } else if (temp3 == 5 || temp3 == 6) {
      fprintf(out, "%d", mcode_list[t].result);
    }
    fprintf(out, " = ");
    if (temp1 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg1].name);
    } else if (temp1 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg1);
    } else if (temp1 == 5 || temp1 == 6) {
      fprintf(out, "%d", mcode_list[t].arg1);
    }
    fprintf(out, " \\ ");
    if (temp2 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg2].name);
    } else if (temp2 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg2);
    } else if (temp2 == 5 || temp2 == 6) {
      fprintf(out, "%d", mcode_list[t].arg2);
    }
    fprintf(out, "\n");
    break;
  }
  case _ASSIGN: {
    int temp1 = mcode_list[t].arg1_type;
    int temp3 = mcode_list[t].result_type;
    if (temp1 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg1].name);
      if (mcode_list[t].a_off_dst != -1) {
        fprintf(out, "[%d]", mcode_list[t].a_off_dst);
      }
    } else if (temp1 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg1);
    } else if (temp1 == 5 || temp1 == 6) {
      fprintf(out, "%d", mcode_list[t].arg1);
    }
    fprintf(out, " = ");
    if (temp3 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].result].name);
    } else if (temp3 == 3) {
      fprintf(out, "t%d", mcode_list[t].result);
    } else if (temp3 == 5 || temp3 == 6) {
      fprintf(out, "%d", mcode_list[t].result);
    }
    fprintf(out, "\n");
    break;
  }
  case _GET_ARRAY: {
    int temp1 = mcode_list[t].arg1_type;
    int temp2 = mcode_list[t].arg2_type;
    int temp3 = mcode_list[t].result_type;
    if (temp3 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].result].name);
    } else if (temp3 == 3) {
      fprintf(out, "t%d", mcode_list[t].result);
    }
    fprintf(out, " = ");
    fprintf(out, "%s", vars[mcode_list[t].arg1].name);
    fprintf(out, "[");
    if (temp2 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].arg2].name);
    } else if (temp2 == 3) {
      fprintf(out, "t%d", mcode_list[t].arg2);
    } else if (temp2 == 5 || temp2 == 6) {
      fprintf(out, "%d", mcode_list[t].arg2);
    }
    fprintf(out, "]");
    fprintf(out, "\n");
    break;
  }
  case _MOVE_PARA_TO_MEM: {
    int f = mcode_list[t].a_off_dst;
    int p = mcode_list[t].a_off_type;
    if (funs[f].para_types[p] == 1) {
      fprintf(out, "para int %s\n", funs[f].para_names[p]);
    } else {
      fprintf(out, "para char %s\n", funs[f].para_names[p]);
    }
    break;
  }
  case _RETURN: {
    fprintf(out, "ret ");
    if (mcode_list[t].arg1 != 0) {
      fprintf(out, "(");
      int temp3 = mcode_list[t].result_type;
      if (temp3 == 1) {
        fprintf(out, "%s", vars[mcode_list[t].result].name);
      } else if (temp3 == 3) {
        fprintf(out, "t%d", mcode_list[t].result);
      } else if (temp3 == 5) {
        fprintf(out, "%d", mcode_list[t].result);
      } else if (temp3 == 6) {
        fprintf(out, "%c", mcode_list[t].result);
      }
      fprintf(out, ")");
    }
    fprintf(out, "\n");
    break;
  }
  case _GENE_FUN_LABEL: {
    int f = mcode_list[t].arg1;
    if (funs[f].return_type == 0) {
      fprintf(out, "void ");
    } else if (funs[f].return_type == 1) {
      fprintf(out, "int ");
    } else {
      fprintf(out, "char ");
    }
    fprintf(out, "%s\n", funs[f].name);
    break;
  }
  case _JAL_FUN: {
    fprintf(out, "call %s\n", funs[mcode_list[t].arg1].name);
    break;
  }
  case _MOVE_PARA_TO_STACK: {
    fprintf(out, "push ");
    int temp1 = mcode_list[t].arg1_type;
    if (temp1 == 1) {
      fprintf(out, "%s", vars[mcode_list[t].result].name);
    } else if (temp1 == 3) {
      fprintf(out, "t%d", mcode_list[t].result);
    } else if (temp1 == 5) {
      fprintf(out, "%d", mcode_list[t].result);
    } else if (temp1 == 6) {
      fprintf(out, "%c", mcode_list[t].result);
    }
    fprintf(out, "\n");
    break;
  }
  case _GENE_J: {
    fprintf(out, "GOTO label_%d\n", mcode_list[t].arg1);
    break;
  }
  case _GENE_CONDI_J: {
    if (mcode_list[t].arg2 == -1) {
      fprintf(out, "BEQ label_%d\n", mcode_list[t].a_off_dst);
    } else {
      int type = mcode_list[t].arg2;
      if (type == LSS) {
        fprintf(out, "BGEZ label_%d\n", mcode_list[t].a_off_dst);
      } else if (type == LEQ) {
        fprintf(out, "BGTZ label_%d\n", mcode_list[t].a_off_dst);
      } else if (type == GRE) {
        fprintf(out, "BLEZ label_%d\n", mcode_list[t].a_off_dst);
      } else if (type == GEQ) {
        fprintf(out, "BLTZ label_%d\n", mcode_list[t].a_off_dst);
      } else if (type == EQL) {
        fprintf(out, "BNE label_%d\n", mcode_list[t].a_off_dst);
      } else if (type == NEQ) {
        fprintf(out, "BEQ label_%d\n", mcode_list[t].a_off_dst);
      }
    }
    break;
  }
  case _J_TO_MAIN: {
    fprintf(out, "call main\n");
    break;
  }
  default:
    break;
  }
}

// op,arg1,arg2,result,a1t,a2t,st
void mcode_gener(int op, int p1, int p2, int p3, int t1, int t2, int t3,
                 int a_off_dst, int a_off_type) {

  int t = mcode_pointer;
  mcode_list[t].op_type = op;
  mcode_list[t].arg1 = p1;
  mcode_list[t].arg2 = p2;
  mcode_list[t].result = p3;
  mcode_list[t].arg1_type = t1;
  mcode_list[t].arg2_type = t2;
  mcode_list[t].result_type = t3;
  mcode_list[t].a_off_dst = a_off_dst;
  mcode_list[t].a_off_type = a_off_type;
  mcode_pointer++;
}

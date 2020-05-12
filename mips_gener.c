//
// Created by asus on 2019/11/20.
//
#include "defines.h"
#include <stdio.h>
#include <stdlib.h>

FILE *mips_out;

extern mcode mcode_list[10000];
extern int mcode_pointer;

extern variate vars[10000];
extern function funs[10000];
extern temp_variate temp_vars[10000];
extern char const_string[100][1000];
extern int str_pointer;

int get_offset(int t, int n) { // 1:arg1,2:arg2,3:result,4:array_off
  int off = 0;
  switch (n) {
  case 1: {
    if (mcode_list[t].arg1_type == 1) {
      off = vars[mcode_list[t].arg1].offset;
    } else if (mcode_list[t].arg1_type == 3) {
      off = temp_vars[mcode_list[t].arg1].offset;
    } else if (mcode_list[t].arg1_type == 5 || mcode_list[t].arg1_type == 6) {
      off = -1;
    }
    break;
  }
  case 2: {
    if (mcode_list[t].arg2_type == 1) {
      off = vars[mcode_list[t].arg2].offset;
    } else if (mcode_list[t].arg2_type == 3) {
      off = temp_vars[mcode_list[t].arg2].offset;
    } else if (mcode_list[t].arg2_type == 5 || mcode_list[t].arg2_type == 6) {
      off = -1;
    }
    break;
  }
  case 3: {
    if (mcode_list[t].result_type == 1) {
      off = vars[mcode_list[t].result].offset;
    } else if (mcode_list[t].result_type == 3) {
      off = temp_vars[mcode_list[t].result].offset;
    } else if (mcode_list[t].result_type == 5 ||
               mcode_list[t].result_type == 6) {
      off = -1;
    }
    break;
  }
  case 4: {
    if (mcode_list[t].a_off_type == 1) {
      off = vars[mcode_list[t].a_off_dst].offset;
    } else if (mcode_list[t].a_off_type == 3) {
      off = temp_vars[mcode_list[t].a_off_dst].offset;
    } else if (mcode_list[t].a_off_type == 5 || mcode_list[t].a_off_type == 6) {
      off = -1;
    }
    break;
  }
  default:
    break;
  }
  return off * 4;
}

void mips_gener() {
  mips_out = fopen("mips.txt", "w");
  ////generate .data
  fprintf(mips_out, ".data\n");
  fprintf(mips_out, "memory: .space 5000\n");
  fprintf(mips_out, "fun_stack: .space 5000\n");
  for (int i = 0; i < str_pointer; ++i) {
    fprintf(mips_out, "string%d: .asciiz \"%s\"\n", i, const_string[i]);
  }
  fprintf(mips_out, "\n");
  fprintf(mips_out, ".text\n");
  fprintf(mips_out, "la $sp,memory\n");
  fprintf(mips_out, "la $gp,memory\n");
  fprintf(mips_out, "la $fp,fun_stack\n");
  ////
  int offset;
  for (int t = 0; t < mcode_pointer; ++t) {
    //    printf("%d %d %d %d %d %d %d %d %d\n", mcode_list[t].op_type,
    //           mcode_list[t].arg1, mcode_list[t].arg2, mcode_list[t].result,
    //           mcode_list[t].arg1_type, mcode_list[t].arg2_type,
    //           mcode_list[t].result_type, mcode_list[t].a_off_dst,
    //           mcode_list[t].a_off_type);
    switch (mcode_list[t].op_type) {
    case _CONST_STATE: {
      offset = get_offset(t, 1);
      fprintf(mips_out, "li $t0,%d\n", vars[mcode_list[t].arg1].value);
      fprintf(mips_out, "sw $t0,%d($sp)\n", offset);
      break;
    }
    case _GENE_MAIN: {
      fprintf(mips_out, "main:\n");
      break;
    }
    case _SCAN: {
      offset = get_offset(t, 1);
      if (vars[mcode_list[t].arg1].type == 1) {
        fprintf(mips_out, "li $v0,5\n");
      } else {
        fprintf(mips_out, "li $v0,12\n");
      }
      fprintf(mips_out, "syscall\n");
      if (mcode_list[t].arg1_type == 1 &&
          vars[mcode_list[t].arg1].domain_num == 0) {
        fprintf(mips_out, "sw $v0,%d($gp)\n", offset);
      } else {
        fprintf(mips_out, "sw $v0,%d($sp)\n", offset);
      }
      // fprintf(mips_out, "sw $v0,%d($sp)\n", offset);
      break;
    }
    case _PRINT: {
      if (mcode_list[t].a_off_dst == 1) {
        fprintf(mips_out, "move $t3,$v0\n");
      }
      int v0 = 0;
      if (abs(mcode_list[t].arg1_type) == 2) {
        fprintf(mips_out, "move $a0,$v0\n");
        if (mcode_list[t].arg1_type == 2) {
          v0 = 1;
        } else {
          v0 = 11;
        }
      } else if (mcode_list[t].arg1_type == 4) {
        fprintf(mips_out, "la $a0,string%d\n", mcode_list[t].arg1);
        v0 = 4;
      } else {
        offset = get_offset(t, 1);
        if (offset == -4) {
          fprintf(mips_out, "li $a0,%d\n", mcode_list[t].arg1);
          if (mcode_list[t].arg1_type == 6) {
            v0 = 11;
          } else {
            v0 = 1;
          }
        } else {
          if (mcode_list[t].arg1_type == 1 &&
              vars[mcode_list[t].arg1].domain_num == 0) {
            fprintf(mips_out, "lw $a0,%d($gp)\n", offset);
          } else {
            fprintf(mips_out, "lw $a0,%d($sp)\n", offset);
          }
          if (mcode_list[t].arg1_type == 1 &&
              vars[mcode_list[t].arg1].type == 2) {
            v0 = 11;
          } else if (mcode_list[t].arg1_type == 3 &&
                     temp_vars[mcode_list[t].arg1].type == 2) {
            v0 = 11;
          } else {
            v0 = 1;
          }
        }
      }
      fprintf(mips_out, "li $v0,%d\n", v0);
      fprintf(mips_out, "syscall\n");
      if (mcode_list[t].a_off_dst != 1) {
        fprintf(mips_out, "li $a0,'\\n'\n");
        fprintf(mips_out, "li $v0,11\n");
        fprintf(mips_out, "syscall\n");
      }
      if (mcode_list[t].a_off_dst == 1) {
        fprintf(mips_out, "move $v0,$t3\n");
      }
      break;
    }
    case _ADD: {
      offset = get_offset(t, 1);
      if (abs(mcode_list[t].arg1_type) == 2) {
        fprintf(mips_out, "move $t0,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
      } else {
        if (mcode_list[t].arg1_type == 1 &&
            vars[mcode_list[t].arg1].domain_num == 0) {
          fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
        }
      }
      offset = get_offset(t, 2);
      // printf("offset=%d\n", offset);
      if (abs(mcode_list[t].arg2_type) == 2) {
        fprintf(mips_out, "move $t1,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t1,%d\n", mcode_list[t].arg2);
      } else {
        if (mcode_list[t].arg2_type == 1 &&
            vars[mcode_list[t].arg2].domain_num == 0) {
          fprintf(mips_out, "lw $t1,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
      }
      fprintf(mips_out, "addu $t2,$t0,$t1\n");
      offset = get_offset(t, 3);
      if (mcode_list[t].result_type == 1 &&
          vars[mcode_list[t].result].domain_num == 0) {
        fprintf(mips_out, "sw $t2,%d($gp)\n", offset);
      } else {
        fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      }
      // fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      break;
    }
    case _SUB: {
      offset = get_offset(t, 1);
      if (abs(mcode_list[t].arg1_type) == 2) {
        fprintf(mips_out, "move $t0,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
      } else {
        if (mcode_list[t].arg1_type == 1 &&
            vars[mcode_list[t].arg1].domain_num == 0) {
          fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
      }
      offset = get_offset(t, 2);
      if (abs(mcode_list[t].arg2_type) == 2) {
        fprintf(mips_out, "move $t1,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t1,%d\n", mcode_list[t].arg2);
      } else {
        if (mcode_list[t].arg2_type == 1 &&
            vars[mcode_list[t].arg2].domain_num == 0) {
          fprintf(mips_out, "lw $t1,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
      }
      fprintf(mips_out, "subu $t2,$t0,$t1\n");
      offset = get_offset(t, 3);
      if (mcode_list[t].result_type == 1 &&
          vars[mcode_list[t].result].domain_num == 0) {
        fprintf(mips_out, "sw $t2,%d($gp)\n", offset);
      } else {
        fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      }
      // fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      break;
    }
    case _MULT: {
      offset = get_offset(t, 1);
      if (abs(mcode_list[t].arg1_type) == 2) {
        fprintf(mips_out, "move $t0,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
      } else {
        if (mcode_list[t].arg1_type == 1 &&
            vars[mcode_list[t].arg1].domain_num == 0) {
          fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
      }
      offset = get_offset(t, 2);
      if (abs(mcode_list[t].arg2_type) == 2) {
        fprintf(mips_out, "move $t1,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t1,%d\n", mcode_list[t].arg2);
      } else {
        if (mcode_list[t].arg2_type == 1 &&
            vars[mcode_list[t].arg2].domain_num == 0) {
          fprintf(mips_out, "lw $t1,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
      }
      fprintf(mips_out, "mult $t0,$t1\n");
      fprintf(mips_out, "mflo $t2\n");
      offset = get_offset(t, 3);
      if (mcode_list[t].result_type == 1 &&
          vars[mcode_list[t].result].domain_num == 0) {
        fprintf(mips_out, "sw $t2,%d($gp)\n", offset);
      } else {
        fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      }
      // fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      break;
    }
    case _DIV: {
      offset = get_offset(t, 1);
      if (abs(mcode_list[t].arg1_type) == 2) {
        fprintf(mips_out, "move $t0,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
      } else {
        if (mcode_list[t].arg1_type == 1 &&
            vars[mcode_list[t].arg1].domain_num == 0) {
          fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
      }
      offset = get_offset(t, 2);
      if (abs(mcode_list[t].arg2_type) == 2) {
        fprintf(mips_out, "move $t1,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t1,%d\n", mcode_list[t].arg2);
      } else {
        if (mcode_list[t].arg2_type == 1 &&
            vars[mcode_list[t].arg2].domain_num == 0) {
          fprintf(mips_out, "lw $t1,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
      }
      fprintf(mips_out, "div $t0,$t1\n");
      fprintf(mips_out, "mflo $t2\n");
      offset = get_offset(t, 3);
      if (mcode_list[t].result_type == 1 &&
          vars[mcode_list[t].result].domain_num == 0) {
        fprintf(mips_out, "sw $t2,%d($gp)\n", offset);
      } else {
        fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      }
      // fprintf(mips_out, "sw $t2,%d($sp)\n", offset);
      break;
    }
    case _ASSIGN: {
      offset = get_offset(t, 1);
      if (abs(mcode_list[t].arg1_type) == 2) {
        fprintf(mips_out, "move $t0,$v0\n");
      } else if (offset == -4) {
        fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
      } else {
        if (mcode_list[t].arg1_type == 1 &&
            vars[mcode_list[t].arg1].domain_num == 0) {
          fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
      }
      offset = get_offset(t, 3);
      if (mcode_list[t].a_off_dst == -1) {
        if (mcode_list[t].result_type == 1 &&
            vars[mcode_list[t].result].domain_num == 0) {
          fprintf(mips_out, "sw $t0,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "sw $t0,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "sw $t0,%d($sp)\n", offset);
      } else { //是数组
        int offset1 = get_offset(t, 4);
        if (offset1 == -4) {
          fprintf(mips_out, "li $t1,%d\n", mcode_list[t].a_off_dst * 4);
        } else {
          if (mcode_list[t].a_off_type == 1 &&
              vars[mcode_list[t].a_off_dst].domain_num == 0) {
            fprintf(mips_out, "lw $t1,%d($gp)\n", offset1);
          } else {
            fprintf(mips_out, "lw $t1,%d($sp)\n", offset1);
          }
          // fprintf(mips_out, "lw $t1,%d($sp)\n", offset1);
          fprintf(mips_out, "sll $t1,$t1,2\n");
        }
        fprintf(mips_out, "addi $t1,$t1,%d\n", offset);
        if (mcode_list[t].result_type == 1 &&
            vars[mcode_list[t].result].domain_num == 0) {
          fprintf(mips_out, "addu $t1,$t1,$gp\n");
        } else {
          fprintf(mips_out, "addu $t1,$t1,$sp\n");
        }
        // fprintf(mips_out, "addu $t1,$t1,$sp\n");
        fprintf(mips_out, "sw $t0,0($t1)\n");
      }
      break;
    }
    case _GET_ARRAY: {
      offset = get_offset(t, 1);
      int offset1 = get_offset(t, 2);
      if (abs(mcode_list[t].arg2_type) == 2) {
        fprintf(mips_out, "move $t1,$v0\n");
      } else if (offset1 == -4) {
        fprintf(mips_out, "li $t1,%d\n", mcode_list[t].arg2 * 4);
      } else {
        if (mcode_list[t].arg2_type == 1 &&
            vars[mcode_list[t].arg2].domain_num == 0) {
          fprintf(mips_out, "lw $t1,%d($gp)\n", offset1);
        } else {
          fprintf(mips_out, "lw $t1,%d($sp)\n", offset1);
        }
        // fprintf(mips_out, "lw $t1,%d($sp)\n", offset1);
        fprintf(mips_out, "sll $t1,$t1,2\n");
      }
      fprintf(mips_out, "addi $t1,$t1,%d\n", offset);
      if (mcode_list[t].arg1_type == 1 &&
          vars[mcode_list[t].arg1].domain_num == 0) {
        fprintf(mips_out, "addu $t1,$t1,$gp\n");
      } else {
        fprintf(mips_out, "addu $t1,$t1,$sp\n");
      }
      // fprintf(mips_out, "addu $t1,$t1,$sp\n");
      fprintf(mips_out, "lw $t0,0($t1)\n");
      offset = get_offset(t, 3);
      if (mcode_list[t].result_type == 1 &&
          vars[mcode_list[t].result].domain_num == 0) {
        fprintf(mips_out, "sw $t0,%d($gp)\n", offset);
      } else {
        fprintf(mips_out, "sw $t0,%d($sp)\n", offset);
      }
      // fprintf(mips_out, "sw $t0,%d($sp)\n", offset);
      break;
    }
    case _MOVE_PARA_TO_MEM: {
      offset = get_offset(t, 3);
      if (mcode_list[t].arg1_type == 5) { //是前四个参数
        if (mcode_list[t].result_type == 1 &&
            vars[mcode_list[t].result].domain_num == 0) {
          fprintf(mips_out, "sw $a%d,%d($gp)\n", mcode_list[t].arg1, offset);
        } else {
          fprintf(mips_out, "sw $a%d,%d($sp)\n", mcode_list[t].arg1, offset);
        }
        // fprintf(mips_out, "sw $a%d,%d($sp)\n", mcode_list[t].arg1, offset);
      } else { //是在函数参数栈中保存的参数
        fprintf(mips_out, "lw $t0,%d($fp)\n", mcode_list[t].arg1 * 4);
        if (mcode_list[t].result_type == 1 &&
            vars[mcode_list[t].result].domain_num == 0) {
          fprintf(mips_out, "sw $t0,%d($gp)\n", offset);
        } else {
          fprintf(mips_out, "sw $t0,%d($sp)\n", offset);
        }
        // fprintf(mips_out, "sw $t0,%d($sp)\n", offset);
      }
      break;
    }
    case _RETURN: { // arg1代表返回类型
      // result是将要被存在v0里的值
      if (mcode_list[t].arg1 != 0) {
        if (mcode_list[t].result_type == 2) {
          //相当于从v0中取出再存回v0
          // do nothing
        } else {
          offset = get_offset(t, 3);
          if (offset == -4) {
            fprintf(mips_out, "li $v0,%d\n", mcode_list[t].result);
          } else {
            if (mcode_list[t].result_type == 1 &&
                vars[mcode_list[t].result].domain_num == 0) {
              fprintf(mips_out, "lw $v0,%d($gp)\n", offset);
            } else {
              fprintf(mips_out, "lw $v0,%d($sp)\n", offset);
            }
            // fprintf(mips_out, "lw $v0,%d($sp)\n", offset);
          }
        }
      }
      fprintf(mips_out, "jr $ra\n");
      break;
    }
    case _PUSH: {
      //      fprintf(mips_out, "sw $a0,0($fp)\n");
      //      fprintf(mips_out, "addi $fp,$fp,4\n");
      //      fprintf(mips_out, "sw $a1,0($fp)\n");
      //      fprintf(mips_out, "addi $fp,$fp,4\n");
      //      fprintf(mips_out, "sw $a2,0($fp)\n");
      //      fprintf(mips_out, "addi $fp,$fp,4\n");
      //      fprintf(mips_out, "sw $a3,0($fp)\n");
      //      fprintf(mips_out, "addi $fp,$fp,4\n");
      fprintf(mips_out, "sw $ra,0($fp)\n");
      fprintf(mips_out, "addi $fp,$fp,4\n");
      break;
    }
    case _POP: {
      fprintf(mips_out, "subi $fp,$fp,4\n");
      fprintf(mips_out, "lw $ra,0($fp)\n");
      //      fprintf(mips_out, "subi $fp,$fp,4\n");
      //      fprintf(mips_out, "lw $a3,0($fp)\n");
      //      fprintf(mips_out, "subi $fp,$fp,4\n");
      //      fprintf(mips_out, "lw $a2,0($fp)\n");
      //      fprintf(mips_out, "subi $fp,$fp,4\n");
      //      fprintf(mips_out, "lw $a1,0($fp)\n");
      //      fprintf(mips_out, "subi $fp,$fp,4\n");
      //      fprintf(mips_out, "lw $a0,0($fp)\n");
      break;
    }
    case _GENE_FUN_LABEL: {
      fprintf(mips_out, "%s:\n", funs[mcode_list[t].arg1].name);
      break;
    }
    case _JAL_FUN: {
      fprintf(mips_out, "addi $sp,$sp,128\n");
      fprintf(mips_out, "jal %s\n", funs[mcode_list[t].arg1].name);
      fprintf(mips_out, "subi $sp,$sp,128\n");
      break;
    }
    case _MOVE_PARA_TO_STACK: {
      if (mcode_list[t].result_type == 5) { //是前四个参数
        offset = get_offset(t, 1);
        if (mcode_list[t].arg1_type == 2) {
          fprintf(mips_out, "move $a%d,$v0\n", mcode_list[t].result);
        } else if (offset == -4) {
          fprintf(mips_out, "li $a%d,%d\n", mcode_list[t].result,
                  mcode_list[t].arg1);
        } else {
          if (mcode_list[t].arg1_type == 1 &&
              vars[mcode_list[t].arg1].domain_num == 0) {
            fprintf(mips_out, "lw $a%d,%d($gp)\n", mcode_list[t].result,
                    offset);
          } else {
            fprintf(mips_out, "lw $a%d,%d($sp)\n", mcode_list[t].result,
                    offset);
          }
          // fprintf(mips_out, "lw $a%d,%d($sp)\n", mcode_list[t].result,
          // offset);
        }
      } else {
        offset = get_offset(t, 1);
        if (mcode_list[t].arg1_type == 2) {
          fprintf(mips_out, "sw $v0,%d($fp)\n", mcode_list[t].result * 4);
        } else if (offset == -4) {
          fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
          fprintf(mips_out, "sw $t0,%d($fp)\n", mcode_list[t].result * 4);
        } else {
          if (mcode_list[t].arg1_type == 1 &&
              vars[mcode_list[t].arg1].domain_num == 0) {
            fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
          } else {
            fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
          }
          // fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
          fprintf(mips_out, "sw $t0,%d($fp)\n", mcode_list[t].result * 4);
        }
      }
      break;
    }
    case _GENE_LABEL: {
      fprintf(mips_out, "label_%d:\n", mcode_list[t].arg1);
      break;
    }
    case _GENE_J: {
      fprintf(mips_out, "j label_%d\n", mcode_list[t].arg1);
      break;
    }
    case _GENE_CONDI_J: {
      if (mcode_list[t].arg2 == -1) {
        offset = get_offset(t, 1);
        if (mcode_list[t].arg1_type == 2) {
          fprintf(mips_out, "beq $v0,$zero,label_%d\n",
                  mcode_list[t].a_off_dst);
        } else if (offset == -4) {
          fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
          fprintf(mips_out, "beq $t0,$zero,label_%d\n",
                  mcode_list[t].a_off_dst);
        } else {
          if (mcode_list[t].arg1_type == 1 &&
              vars[mcode_list[t].arg1].domain_num == 0) {
            fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
          } else {
            fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
          }
          // fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
          fprintf(mips_out, "beq $t0,$zero,label_%d\n",
                  mcode_list[t].a_off_dst);
        }
      } else {
        offset = get_offset(t, 1);
        if (mcode_list[t].arg1_type == 2) {
          fprintf(mips_out, "move $t0,$v0\n");
        } else if (offset == -4) {
          fprintf(mips_out, "li $t0,%d\n", mcode_list[t].arg1);
        } else {
          if (mcode_list[t].arg1_type == 1 &&
              vars[mcode_list[t].arg1].domain_num == 0) {
            fprintf(mips_out, "lw $t0,%d($gp)\n", offset);
          } else {
            fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
          }
          // fprintf(mips_out, "lw $t0,%d($sp)\n", offset);
        }
        offset = get_offset(t, 3);
        if (mcode_list[t].result_type == 2) {
          fprintf(mips_out, "move $t1,$v0\n");
        } else if (offset == -4) {
          fprintf(mips_out, "li $t1,%d\n", mcode_list[t].result);
        } else {
          if (mcode_list[t].result_type == 1 &&
              vars[mcode_list[t].result].domain_num == 0) {
            fprintf(mips_out, "lw $t1,%d($gp)\n", offset);
          } else {
            fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
          }
          // fprintf(mips_out, "lw $t1,%d($sp)\n", offset);
        }
        fprintf(mips_out, "subu $t2,$t0,$t1\n");
        int type = mcode_list[t].arg2;
        if (type == LSS) {
          fprintf(mips_out, "bgez $t2,label_%d\n", mcode_list[t].a_off_dst);
        } else if (type == LEQ) {
          fprintf(mips_out, "bgtz $t2,label_%d\n", mcode_list[t].a_off_dst);
        } else if (type == GRE) {
          fprintf(mips_out, "blez $t2,label_%d\n", mcode_list[t].a_off_dst);
        } else if (type == GEQ) {
          fprintf(mips_out, "bltz $t2,label_%d\n", mcode_list[t].a_off_dst);
        } else if (type == EQL) {
          fprintf(mips_out, "bne $t2,$zero,label_%d\n",
                  mcode_list[t].a_off_dst);
        } else if (type == NEQ) {
          fprintf(mips_out, "beq $t2,$zero,label_%d\n",
                  mcode_list[t].a_off_dst);
        }
      }
      break;
    }
    case _J_TO_MAIN: {
      fprintf(mips_out, "addi $sp,$sp,128\n");
      fprintf(mips_out, "jal main\n\n");
      fprintf(mips_out, "subi $sp,$sp,128\n");
      // fprintf(mips_out, "jal main\n\n");
      fprintf(mips_out, "li $v0,10\n");
      fprintf(mips_out, "syscall\n");
      break;
    }
    case _MOVE_SP: {
      if (mcode_list[t].arg1 == 128) {
        fprintf(mips_out, "addi $sp,$sp,128\n");
      } else {
        fprintf(mips_out, "subi $sp,$sp,128\n");
      }
      break;
    }
    case _SAVE_V0: {
      offset = get_offset(t, 1);
      fprintf(mips_out, "sw $v0,%d($sp)\n", offset);
      break;
    }
    default:
      break;
    }
  }
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* SOLUTION CODE BELOW */
const int TWO_POW_SEVENTEEN = 131072;    // 2^17

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li and other pseudoinstructions. Your pseudoinstruction 
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are 
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number 
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addiu instruction. Otherwise, expand it into 
        a lui-ori pair.

    For mul, quo, and rem, the expansions should be pretty straight forward if 
    you paid attention to the lecture slides about the subtleties of $hi and $lo
    registers.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that 
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
    if (strcmp(name, "li") == 0) {
      if (num_args != 2) {
        return 0;
      }
      long int imm;
      int rt = translate_reg(args[0]);
      int err1 = translate_num(&imm, args[1], INT16_MIN, INT16_MAX);
      int err2 = translate_num(&imm, args[1], 0, INT32_MAX);
      /* Make sure that the number is representable by 32 bits.*/
      if ((err1 != 0) && (err2 != 0)) {
        return 0;
      }
      /* If the immediate can fit in the imm field.*/
      if (err1 == 0) {
        fprintf(output, "%s %s %s %s\n", "addiu",args[0],,);
      }
      return 0;  
    } else if (strcmp(name, "mul") == 0) {
      if (num_args != 3) {
        return -1;
      }
        return 0;  
    } else if (strcmp(name, "quo") == 0) {
      if (num_args != 3) {
        return -1;
      }
        return 0;  
    } else if (strcmp(name, "rem") == 0) {
      if (num_args != 3) {
        return -1;
      }
        return 0;  
    }
    write_inst_string(output, name, args, num_args);
    return 1;

}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
   
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS. 

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros. 

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write 
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Some function declarations for the write_*() functions are provided in translate.h, and you MUST implement these
   and use these as function headers. You may use helper functions, but you still must implement
   the provided write_* functions declared in translate.h.

   Returns 0 on success and -1 on error. 
 */
int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr,
    SymbolTable* symtbl, SymbolTable* reltbl) {
    if (strcmp(name, "addu") == 0)       return write_rtype (0x21, output, args, num_args);//
    else if (strcmp(name, "or") == 0)    return write_rtype (0x25, output, args, num_args);//
    else if (strcmp(name, "slt") == 0)   return write_rtype (0x2a, output, args, num_args);//
    else if (strcmp(name, "sltu") == 0)  return write_rtype (0x2b, output, args, num_args);//
    else if (strcmp(name, "sll") == 0)   return write_shift (0x00, output, args, num_args);//
    /* YOUR CODE HERE */
    else if (strcmp(name, "jr") == 0)   return write_jr (0x08, output, args, num_args);//
    else if (strcmp(name, "addiu") == 0)   return write_addiu (0x09, output, args, num_args);//
    else if (strcmp(name, "ori") == 0)   return write_rtype (0x0d, output, args, num_args);//
    else if (strcmp(name, "lui") == 0)   return write_lui (0x0f, output, args, num_args);// 
    else if (strcmp(name, "lb") == 0)   return write_mem (0x20, output, args, num_args);//
    else if (strcmp(name, "lbu") == 0)   return write_mem (0x24, output, args, num_args);//
    else if (strcmp(name, "lw") == 0)   return write_mem (0x23, output, args, num_args);//  
    else if (strcmp(name, "sb") == 0)   return write_mem (0x28, output, args, num_args);//
    else if (strcmp(name, "sw") == 0)   return write_mem (0x2b, output, args, num_args);//
    else if (strcmp(name, "beq") == 0)   return write_branch (0x04, output, args, num_args,symtbl);
    else if (strcmp(name, "bne") == 0)   return write_branch (0x05, output, args, num_args,symtbl);
    else if (strcmp(name, "j") == 0)   return write_jump (0x02, output, args, num_args,reltbl);//
    else if (strcmp(name, "jal") == 0)   return write_jump (0x03, output, args, num_args,reltbl);//

    else if (strcmp(name, "mult") == 0)   return write_mult_div (0x18, output, args, num_args);//
    else if (strcmp(name, "div") == 0)   return write_mult_div (0x1a, output, args, num_args);//
    else if (strcmp(name, "mfhi") == 0)   return write_mfhi (0x10, output, args, num_args);//
    else if (strcmp(name, "mflo") == 0)   return write_mfhi (0x12, output, args, num_args);//
    else                                 return -1;
}
/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to 
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {
    /* Error when the number of input argument is not correct.*/
    if (num_args != 3) {
      return -1;
    }
    int rd = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int rt = translate_reg(args[2]);
    /* Check if these variables are valid.*/
    if ((rd == -1) || (rs == -1) || (rt == -1)) {
      return -1;
    }
    /* Initialize the instruction as opcode(0).*/
    uint32_t instruction = 0;
    /* Add in the upcode into the instructions.*/
    /* Since for the R type, the upcode is 0, so we will leave the leftmost6 bits as zeros.*/
    instruction = (instruction << 5) + rs;
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 5) + rd;
    /* Insert the shamt value into the 10th to the 6th bit of output.*/
    /* And since args have only three arguments, we should leave it as zero.*/
    instruction = (instruction << 5);
    /* Insert funct into the 0th to the 5th bit of of the instructions.*/
    instruction = (instruction << 6) = funct;
    write_inst_hex(output, instruction);
    return 0;
}
/* A helper function for writing shift instructions. You should use 
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {
	/* Check the number of input argument.*/
  if (num_args != 3) {
    retunr -1;
  }
    long int shamt;
    int rd = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int err = translate_num(&shamt, args[2], 0, 31);
  /* Check if all of these three arguments are valid.*/
    if ((rd == -1) || (re == -1) || (err == -1)) {
      return -1;
    }
    /* opcode is 0.*/
    uint32_t instruction = 0;
    /* Insert these three different argument into the instructions.*/
    /* Leave the rs part 5 bits of zeros.*/
    instruction = (instruction << 5);
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 5) + rd;
    instruction = (instruction << 5) + err;
    instruction = (instruction << 6) + funct;
    write_inst_hex(output, instruction);
    return 0;
}
/* The rest of your write_*() functions below */

int write_jr(uint8_t funct, FILE* output, char** args, size_t num_args) {
    /* For j type function, the upcode and the funct values are both 2.*/
    /* Jump register instruction has only one input argument.*/
    if (num_args != 1) {
      return -1;
    }
    int rs = translate_reg(args[0]);
    int label_addr = get_addr_for_symbol(reltbl, args[0])
    /* check if rs is valid.*/
    if ((rs == -1) || (label_addr == -1)){
      return -1;
    }
    /* The opcode in this case is 2.*/
    uint32_t instruction = 2;
    /* Insert the uncode into the instruction, which is the 31th to the 26th bit.*/
    instruction = (instruction << 26) + label_addr;
    write_inst_hex(output, instruction);
    return 0;
}

int write_addiu(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    /* There are three argument parsed in.*/
    if (num_args != 3) {
      return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, INT16_MAX);
    if ((rt == -1) || (rs == -1) || (err == -1)) {
      return -1;
    }
    /* Initialize the instruction as the upcode since upcode is the first term in the instruction.*/
    uint32_t instruction = opcode;
    instruction = (instruction << 5) + rs;
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 16) + err; 
    
    write_inst_hex(output, instruction);
    return 0;
}

int write_ori(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    /* There are three input arguments for ori.*/
    if (num_args != 3) {
      return -1;
    }
    
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], 0, UINT16_MAX);
    /* Check if they are all valid.*/
    if ((rt == -1) || (rs == -1) || (err == -1)) {
      return -1;
    }
    /* Initialize the instruction as the opcode.*/
    uint32_t instruction = opcode;
    instruction = (instruction << 5) + rs;
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 16) + err;  
    write_inst_hex(output, instruction);
    return 0;
}

int write_lui(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    /* There are two input argument.*/
    if (num_args != 2) {
      return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int err = translate_num(&imm, args[1], 0, UINT16_MAX);
    if ((rt == -1) || (err == -1)) {
      return -1;
    }
    uint32_t instruction = 0;
    instruction = (instruction << 5);
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 5) + err;
    write_inst_hex(output, instruction);
    return 0;
}

int write_mem(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    /* There are two input arguments for this function.*/
    if (num_args != 3) {
      return -1;
    }
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[2]);
    int err = translate_num(&imm, args[1], INT16_MIN, INT16_MAX);
    /* Check if the input arguments are valid.*/
    if ((rt == -1) || (rs == -1) || (err == -1)) {
      return -1;
    }
    uint32_t instruction = opcode;
    instruction = (instruction << 5) + rs;
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 5) + err;
    write_inst_hex(output, instruction);
    return 0;
}

/*  A helper function to determine if a destination address
    can be branched to
*/
static int can_branch_to(uint32_t src_addr, uint32_t dest_addr) {
    int32_t diff = dest_addr - src_addr;
    return (diff >= 0 && diff <= TWO_POW_SEVENTEEN) || (diff < 0 && diff >= -(TWO_POW_SEVENTEEN - 4));
}
int write_branch(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    // Perhaps perform some error checking?
    /* Three arguments for this function.*/
    if (num_args != 3) {
      return -1;
    }
    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int label_addr = get_addr_for_symbol(symtbl, args[2]);
    /* Check if they are all valid.*/
    if ((rs == -1) || (rt == -1) || (label_addr == -1)) {
      return -1;
    }
    //Please compute the branch offset using the MIPS rules.
    int32_t offset = ((label_addr) - ((int32_t)addr + 4)) / 4;
    uint32_t instruction = opcode;
    instruction = (instruction << 5) + rs;
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 5) + offset;
    write_inst_hex(output, instruction);        
    return 0;
}

int write_jump(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* reltbl) {
    /* YOUR CODE HERE */
    /* Both Jump and Jump Register function has only one input argument.*/
    if (num_args != 1) {
      return -1;
    }
    int label_addr = get_addr_for_symbol(symtbl, args[0]);
    uint32_t instruction = opcode;
    instruction = (instruction << 26) + label_addr;
    write_inst_hex(output, instruction);
    return 0;
}


int write_mult_div(uint8_t funct, FILE* output, char** args, size_t num_args) {
  /* Check if the number of input argument is 2.*/
  if (num_args != 2) {
    return -1;
  }
    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    /* Check if these variables are valid.*/
    if ((rs == -1) || (rt == -1)) {
      return -1;
    }
    uint32_t instruction = 0;
    instruction = (instruction << 5) + rs;
    instruction = (instruction << 5) + rt;
    instruction = (instruction << 5);
    instruction = (instruction << 5);
    instruction = (instruction << 6) = funct;
    write_inst_hex(output, instruction);
    return 0;
}

int write_mfhi(uint8_t funct, FILE* output, char** args, size_t num_args) {
  /* Check if the number of input argument is 2.*/
    if (num_args != 1) {
      return -1;
    }
    int rd = translate_reg(args[0]);
    /* Check if these variables are valid.*/
    if (rd == -1) {
      return -1;
    }
    uint32_t instruction = 0;
    instruction = (instruction << 5);
    instruction = (instruction << 5);
    instruction = (instruction << 5) + rd;
    instruction = (instruction << 5);
    instruction = (instruction << 6) = funct;
    write_inst_hex(output, instruction);
    return 0;
}

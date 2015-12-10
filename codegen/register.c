#include "register.h"

// convers a register number (0-15) into a register name (%rax...%r15)
const char * register_name(int r)
{
	switch(r){
		case RAX:
			return "%rax";
			break;
		case RBX:
			return "%rbx";
			break;
		case RCX:
			return "%rcx";
			break;
		case RDX:
			return "%rdx";
			break;
		case RSI:
			return "%rsi";
			break;
		case RDI:
			return "%rdi";
			break;
		case RBP:
			return "%rbp";
			break;
		case RSP:
			return "%rsp";
			break;
		case R8:
			return "%r8";
			break;
		case R9:
			return "%r9";
			break;
		case R10:
			return "%r10";
			break;
		case R11:
			return "%r11";
			break;
		case R12:
			return "%r12";
			break;
		case R13:
			return "%r13";
			break;
		case R14:
			return "%r14";
			break;
		case R15:
			return "%r15";
			break;
		default:			// should never get here - only 0-15 should be passed into this function
			return "ERROR";
			break;
        }
}

// changes the first open 0 register (not in use) to 1 (in use) and returns that value
int register_alloc()
{
  int i = 0;
  for(i = 0; i < 16; i++){		// loop through reg array
    if(reg[i] == 0){		// if the element is 0
      reg[i] = 1;			// set it to 1 and return it
      return i;
    }
  }
  printf("error: out of registers\n");		// if there are no more registers, exit (should not happen)
  exit(1);
}

// frees a register that is in use by setting it to 0 in the reg array
void register_free(int r){
  if(r >= 0 && r <= 15){
    reg[r] = 0;
  } else {
    printf("error: trying to free register out of range (%d)\n",r);
    exit(1);
  }
}

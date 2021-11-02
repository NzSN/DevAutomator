#include "general.h"
#include "list.h"

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

typedef struct Inst {
    int opcode;
    // List args;
    void *ret;
} Inst;

typedef struct Variable {

} Variable;


/* Instructions constructors */
Inst * instOperateConst(char *target, char *command, char *ret);
Inst * instAssignConst(Variable *cond, void *value);
Inst * instCondJmpConst(Variable *cond, int tLine, int fLine);
Inst * instPlusConst(Variable *l, Variable *r);
Inst * instMinusConst(Variable *l, Variable *r);
Inst * instDivConst(Variable *l, Variable *r);

/* Inst functions as macros */
#define instOpCode(I) ((I)->opcode)
#define instArgs(I)   ((I)->args)
#define instRet(I)    ((I)->ret)

/* Prototypes */
//Inst * instCreate(int opcode, List args, void *ret);

#endif /* _INSTRUCTION_H_ */

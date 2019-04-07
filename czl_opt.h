#ifndef CZL_OPT_H
#define CZL_OPT_H

#include "czl_vm.h"

///////////////////////////////////////////////////////////////
//释放字符串: string free
#define CZL_SF(gp, str) CZL_STR_FREE(gp, str.Obj, CZL_SL(str.size))

//重新调整字符串长度: string resize
#define CZL_SR(gp, str, len) \
(void**)CZL_STR_REALLOC(gp, str.Obj, CZL_SL(len), CZL_SL(str.size))
///////////////////////////////////////////////////////////////
//字符串引用计数加一: str ref_cnt add 1
#define CZL_SRCA1(str) ++CZL_STR(str.Obj)->rc

//字符串引用计数减一: string ref_cnt dec 1
#define CZL_SRCD1(gp, str) \
{ if (0 == --CZL_STR(str.Obj)->rc) CZL_SF(gp, str); }

//哈希表键字符串引用计数减一: key string ref_cnt dec 1
#define CZL_KEY_SRCD1(gp, s) \
{ if (0 == --CZL_STR(s)->rc) CZL_STR_FREE(gp, s, CZL_SL(CZL_STR(s)->len)); }
///////////////////////////////////////////////////////////////
//对象引用计数加一: obj ref_cnt add 1
#define CZL_ORCA1(obj) ++CZL_INS(obj)->rc

//对象引用计数减一: obj ref_cnt dec 1
#define CZL_ORCD1(obj) --CZL_INS(obj)->rc
///////////////////////////////////////////////////////////////
//对象引用标记加一: obj ref_flag add 1
#define CZL_ORFA1(ref) { \
    unsigned long i = 0; \
    while (i < ref->cnt) ++CZL_INS(ref->objs[i++])->rf; \
}

//对象引用标记减一: obj ref_flag def 1
#define CZL_ORFD1(ref) { \
    unsigned long i = 0; \
    while (i < ref->cnt) --CZL_INS(ref->objs[i++])->rf; \
}
///////////////////////////////////////////////////////////////
//释放实例: instance free
#ifdef CZL_MM_MODULE
    #define CZL_IF(gp, obj, ins) \
            czl_free(gp, obj, \
                     CZL_IL(ins->pclass->parents_count, \
                            ins->pclass->vars_count), \
                     &ins->pclass->pool)
#else
    #define CZL_IF(gp, obj, ins) \
            czl_free(gp, ins, \
                     CZL_IL(ins->pclass->parents_count, \
                            ins->pclass->vars_count))
#endif //#ifdef CZL_MM_MODULE
///////////////////////////////////////////////////////////////
//获取函数节点大小 get fun node size
#define CZL_GFNS(type) (CZL_SYS_FUN == type ? 64 : sizeof(czl_fun))
///////////////////////////////////////////////////////////////
//哈希索引计算宏: index必须为有符号型long，不能为无符号
#define CZL_INDEX_CAC(index, hash, mask) \
(index=((index=hash|mask) < 0 ? -index-1 : index-1))
///////////////////////////////////////////////////////////////
//检查AST节点是否是常量:
#define CZL_IS_CONST(node) \
(CZL_OPERAND == node->flag && \
 (CZL_ENUM == node->type || \
 (CZL_ARRAY_LIST == node->type && \
  CZL_CONST_VAR == CZL_ARR_LIST(((czl_var*)node->op.obj)->val.Obj)->quality) || \
 (CZL_TABLE_LIST == node->type && \
  CZL_CONST_VAR == CZL_TAB_LIST(((czl_var*)node->op.obj)->val.Obj)->quality) || \
  CZL_FUN_REF == node->type || \
  CZL_NEW == node->type || \
  CZL_NIL == node->type || \
  CZL_OBJ_REF == node->type))

//检查AST节点是否是变量:
#define CZL_IS_NOT_VAR(node) \
(node->type != CZL_LG_VAR && \
 node->type != CZL_INS_VAR && \
 !czl_is_member_var(node->type, (czl_obj_member*)node->op.obj)) \

//检查表达式是否是引用变量
#define CZL_IS_REF_VAR(exp) \
(CZL_OP_END == exp[1].flag && \
 CZL_UNARY_OPT == exp->flag && CZL_REF_VAR == exp->kind)
///////////////////////////////////////////////////////////////
//获取引用变量: get ref var
#define CZL_GRV(p) \
((p)->val.ref.inx < 0 ? (p)->val.ref.var : \
 (czl_var*)(((czl_array*)(p)->val.ref.var)->vars + (p)->val.ref.inx))
///////////////////////////////////////////////////////////////
#define CZL_1POCF(kind, opr) \
czl_1p_opt_cac_funs[kind](opr)

#define CZL_2POCF(gp, kind, res, opr) \
czl_2p_opt_cac_funs[kind](gp, res, opr)
///////////////////////////////////////////////////////////////
//表达式是否是操作数: exp is opr
#define CZL_EIO(exp) (CZL_OPERAND == exp->flag && CZL_OPERAND == exp->lt)

//结果是否为真: res is true
#define CZL_EIT(res) \
(CZL_FLOAT == (res)->type ? (res)->val.fnum : (res)->val.inum)
///////////////////////////////////////////////////////////////
//检查释放字符串连接缓存: string link buf check free
#define CZL_SLB_CF(gp, res) \
if (CZL_ARRBUF_VAR == res->quality || CZL_FUNRET_VAR == res->quality) { \
    czl_val_del(gp, res); \
    res->quality = CZL_DYNAMIC_VAR; \
}
///////////////////////////////////////////////////////////////
//获取下一条指令: get next order， 用于减少指令分派过程提高VM性能
#define CZL_GNO(pc) \
if (CZL_BLOCK_BEGIN == pc->flag) \
    pc = (CZL_EIT(lo) ? pc->msg.bp.pc : pc->msg.bp.next); \
else if (CZL_LOGIC_JUMP == pc->flag) \
    pc = pc->msg.bp.pc;

//与、或运算检查跳跃: and or operator check jump
#define CZL_AO_CJ(gp, lo, pc) \
if (CZL_EIT(lo)) { \
    if (pc->lo) { \
        pc += pc->rt; \
        CZL_SLB_CF(gp, pc->res); \
        pc->res->type = CZL_INT; \
        pc->res->val.inum = 1; \
    } \
} \
else { \
    if (pc->ro) { \
        pc += pc->rt; \
        CZL_SLB_CF(gp, pc->res); \
        pc->res->type = CZL_INT; \
        pc->res->val.inum = 0; \
    } \
} \
++pc;

//执行三目运算符指令: run three opt
#define CZL_RTO(lo, pc) \
pc += (CZL_EIT(lo) ? 1 : pc->rt);

//三目运算符结果搬移: three opt res move
#define CZL_TORM(gp, res, pc, lo) \
if (res) { \
    CZL_SLB_CF(gp, res); \
    *res = *lo; \
    if (CZL_ARRBUF_VAR == lo->quality) \
        lo->quality = CZL_DYNAMIC_VAR; \
    else if (CZL_FUNRET_VAR == lo->quality) \
        lo->type = CZL_INT; \
} \
pc += pc->rt;

//检查操作数尾: check opr tail
#define CZL_COT(gp, lo, pc) \
switch (pc->lt) \
{ \
case CZL_OPERAND: ++pc; break; \
case CZL_CONDITION: CZL_AO_CJ(gp, lo, pc); break; \
case CZL_THREE_OPT: CZL_RTO(lo, pc); break; \
default: CZL_TORM(gp, pc->lo, pc, lo); break; \
}

//获取操作数: get opr res
#define CZL_GOR(gp, lo, pc) \
if (CZL_REG_VAR == pc->kind && pc->res->type != CZL_OBJ_REF) \
    lo = pc->res; \
else if (!(lo=czl_get_opr(gp, pc->kind, pc->res))) {\
    if (CZL_USR_FUN == pc->kind) goto CZL_RUN_FUN; \
    else goto CZL_EXCEPTION_CATCH; \
}

//执行单目运算符指令: run unary opt
#define CZL_RUO(gp, lo, pc) \
if (CZL_REG_VAR == pc->lt && pc->lo->type != CZL_OBJ_REF) \
    lo = pc->lo; \
else if (!(lo=czl_get_opr(gp, pc->lt, pc->lo))) { \
    if (CZL_USR_FUN == pc->lt) goto CZL_RUN_FUN; \
    else goto CZL_EXCEPTION_CATCH; \
} \
if (pc->res) { \
    CZL_SLB_CF(gp, pc->res); \
    if (!CZL_2POCF(gp, pc->kind, pc->res, lo)) \
        goto CZL_EXCEPTION_CATCH; \
    lo = pc->res; \
} \
else if (!CZL_1POCF(pc->kind, lo)) \
    goto CZL_EXCEPTION_CATCH; \
++pc;

//执行有临时结果的双目运算符指令: run binary2 opt
#define CZL_RB2O(gp, lo, ro, pc) \
if (pc->lo != pc->res) \
    CZL_SLB_CF(gp, pc->res); \
if (CZL_ADD_A-CZL_NUMBER_NOT == pc->kind && \
    pc->res->quality != CZL_ARRLINK_VAR && \
    pc->res->quality != CZL_ARRBUF_VAR) \
    pc->res->quality = CZL_ARRLINK_VAR; \
if (CZL_REG_VAR == pc->lt && pc->lo->type != CZL_OBJ_REF) { \
    lo = pc->lo; \
    if (pc->lo != pc->res) { \
        pc->res->type = lo->type; \
        pc->res->val = lo->val; \
    } \
} \
else { \
    if (!(lo=czl_get_opr(gp, pc->lt, pc->lo))) { \
        if (CZL_USR_FUN == pc->lt) goto CZL_RUN_FUN; \
        else goto CZL_EXCEPTION_CATCH; \
    } \
    pc->res->type = lo->type; \
    pc->res->val = lo->val; \
    if (CZL_FUNRET_VAR == lo->quality) { \
        pc->res->quality = CZL_FUNRET_VAR; \
        lo->type = CZL_INT; \
    } \
} \
if (CZL_REG_VAR == pc->rt && pc->ro->type != CZL_OBJ_REF) \
    ro = pc->ro; \
else { \
    if (CZL_OBJ_ELE == lo->quality) \
        lo->quality = CZL_LOCK_ELE; \
    ro = czl_get_opr(gp, pc->rt, pc->ro); \
    if (CZL_LOCK_ELE == lo->quality) \
        lo->quality = CZL_OBJ_ELE; \
    if (!ro) { \
        if (CZL_USR_FUN == pc->rt) { \
            if (CZL_OBJ_ELE == lo->quality) { \
                lo->quality = CZL_LOCK_ELE; \
                pc->res->name = (char*)lo; \
            } \
            goto CZL_RUN_FUN; \
        } \
        else goto CZL_EXCEPTION_CATCH; \
    } \
} \
if (!CZL_2POCF(gp, pc->kind, (lo=pc->res), ro)) \
    goto CZL_EXCEPTION_CATCH; \
++pc;

//执行双目运算符指令: run binary opt
#define CZL_RBO(gp, lo, ro, pc) \
if (CZL_REG_VAR == pc->rt && pc->ro->type != CZL_OBJ_REF) \
    ro = pc->ro; \
else if (!(ro=czl_get_opr(gp, pc->rt, pc->ro))) { \
    if (CZL_USR_FUN == pc->rt) goto CZL_RUN_FUN; \
    else goto CZL_EXCEPTION_CATCH; \
} \
if (CZL_MEMBER == pc->lt && CZL_OBJ_ELE == ro->quality) \
    ro->quality = CZL_LOCK_ELE; \
if (CZL_REG_VAR == pc->lt && pc->lo->type != CZL_OBJ_REF) \
    lo = pc->lo; \
else \
    lo = czl_get_opr(gp, pc->lt, pc->lo); \
if (CZL_LOCK_ELE == ro->quality) \
    ro->quality = CZL_OBJ_ELE; \
if (!lo || !CZL_2POCF(gp, pc->kind, lo, ro)) { \
    if (lo && CZL_STR_ELE == lo->quality) \
        czl_set_char(gp); \
    goto CZL_EXCEPTION_CATCH; \
} \
if (CZL_STR_ELE == lo->quality) \
    czl_set_char(gp); \
++pc;

//执行赋值运算符指令: run ass opt
#define CZL_RAO(gp, lo, ro, pc) \
if (CZL_REG_VAR == pc->rt &&  \
    (pc->ro->type != CZL_OBJ_REF || \
     CZL_REF_ELE == pc->ro->quality)) \
    ro = pc->ro; \
else if (!(ro=czl_get_opr(gp, pc->rt, pc->ro))) { \
    if (CZL_USR_FUN == pc->rt) goto CZL_RUN_FUN; \
    else goto CZL_EXCEPTION_CATCH; \
} \
if (CZL_MEMBER == pc->lt) \
    gp->cur_var = ro; \
if (CZL_OBJ_REF == ro->type) { \
    if (CZL_INS_VAR == pc->lt) \
        lo = ((czl_ins_var*)pc->lo)->var; \
    else lo = pc->lo; \
} \
else {\
    if (CZL_MEMBER == pc->lt && CZL_OBJ_ELE == ro->quality) \
        ro->quality = CZL_LOCK_ELE; \
    if (CZL_REG_VAR == pc->lt && pc->lo->type != CZL_OBJ_REF) \
        lo = pc->lo; \
    else \
        lo = czl_get_opr(gp, pc->lt, pc->lo); \
    if (CZL_LOCK_ELE == ro->quality) \
        ro->quality = CZL_OBJ_ELE; \
    if (!lo) \
        goto CZL_EXCEPTION_CATCH; \
} \
if (CZL_CIRCLE_REF_VAR == gp->tmp_var.quality) \
    ro = &gp->tmp_var; \
if (!czl_ass_cac(gp, lo, ro)) { \
    if (CZL_STR_ELE == lo->quality) \
        czl_set_char(gp); \
    goto CZL_EXCEPTION_CATCH; \
} \
if (CZL_STR_ELE == lo->quality) \
    czl_set_char(gp); \
++pc;

//执行foreach语句: run foreach sentence
#define CZL_RFS(gp, pc) \
switch (pc->kind ? \
        czl_foreach_range(gp, (czl_foreach*)pc->res, pc->lt) : \
        czl_foreach_object(gp, (czl_foreach*)pc->res, pc->lt)) \
{ \
case 1: pc = pc->msg.bp.pc; break; \
case 0: pc = pc->msg.bp.next; break; \
default: goto CZL_EXCEPTION_CATCH; \
}

//执行switch语句: run switch sentence
#define CZL_RSS(gp, pc, lo) \
CZL_SLB_CF(gp, pc->res); \
*pc->res = *lo; \
if (CZL_ARRBUF_VAR == lo->quality) \
    lo->quality = CZL_DYNAMIC_VAR; \
else if (CZL_FUNRET_VAR == lo->quality) \
    lo->type = CZL_INT; \
++pc;

//执行case/default语句: run case sentence
#define CZL_RCS(gp, pc, lo) \
*pc->res = *lo; \
if (CZL_FUNRET_VAR == lo->quality) \
    lo->type = CZL_INT; \
czl_equ_equ_cac(gp, pc->res, pc->lo); \
pc = (pc->res->val.inum ? pc->msg.bp.pc : pc->msg.bp.next);

//执行return/yeild语句: run return/yeild sentence
#define CZL_RRYS(gp, pc, lo) \
if (!pc->res) pc->ro->val.inum = 0; \
else if (!czl_val_copy(gp, pc->res, lo)) goto CZL_EXCEPTION_CATCH; \
goto CZL_FUN_RETURN;

//执行try语句: run try sentence
#define CZL_RTS(gp, pc, bp, stack, size) \
switch (pc->kind) \
{ \
case CZL_TRY_EXIT: \
    gp->exit_flag = 1; \
    gp->exit_code = CZL_EXIT_TRY; \
    goto CZL_EXCEPTION_CATCH; \
case CZL_TRY_BREAK: case CZL_TRY_GOTO: \
    pc = pc->msg.bp.next; \
    break; \
default: \
    pc = (CZL_FOREACH_BLOCK == bp->flag ? bp->msg.bp.next : bp+1); \
    break; \
} \
gp->exceptionCode = CZL_EXCEPTION_NO;
///////////////////////////////////////////////////////////////
//检查释放函数返回值: check free fun ret
#define CZL_CF_FR(gp, var) \
if (CZL_FUNRET_VAR == var->quality) { \
    czl_val_del(gp, var); \
    var->type = CZL_INT; \
}

//检查释放函数返回值: check free fun ret
#define CZL_CF_FR2(gp, var) \
if (var->type != CZL_INT) { \
    czl_val_del(gp, var); \
    var->type = CZL_INT; \
}

//执行函数单目运算符指令: run fun unary opt
#define CZL_RFUO(gp, lo, pc) \
CZL_SLB_CF(gp, pc->res); \
if (!CZL_2POCF(gp, pc->kind, pc->res, lo)) \
    goto CZL_EXCEPTION_CATCH; \
CZL_CF_FR2(gp, lo); \
lo = (pc++)->res;

//执行函数双目运算符指令: run fun binary opt
#define CZL_RFBO(gp, lo, ro, pc) \
if (!CZL_2POCF(gp, pc->kind, lo, ro)) { \
    if (CZL_STR_ELE == lo->quality) \
        czl_set_char(gp); \
    goto CZL_EXCEPTION_CATCH; \
} \
if (CZL_STR_ELE == lo->quality) \
    czl_set_char(gp); \
CZL_CF_FR2(gp, ro); \
++pc;

//执行函数带临时结果双目运算符指令: run fun binary2 opt
#define CZL_RFB2O(gp, lo, ro, pc) \
if (!CZL_2POCF(gp, pc->kind, lo, ro)) \
    goto CZL_EXCEPTION_CATCH; \
CZL_CF_FR(gp, lo); \
CZL_CF_FR(gp, ro); \
++pc;

//设置函数双目运算符操作数： set fun binary opt opr
#define CZL_SFBOO(gp, lo, ro, pc, cur) \
ro = &cur->fun->ret; \
if (CZL_ASS_OPT == pc->flag && CZL_MEMBER == pc->lt) \
    gp->cur_var = ro; \
if (CZL_ASS_OPT == pc->flag && CZL_OBJ_REF == ro->type) { \
    if (CZL_INS_VAR == pc->lt) \
        lo = ((czl_ins_var*)pc->lo)->var; \
    else if (CZL_MEMBER == pc->lt) \
        goto CZL_EXCEPTION_CATCH; \
    else lo = pc->lo; \
} \
else {\
    if (CZL_REG_VAR == pc->lt && pc->lo->type != CZL_OBJ_REF) \
        lo = pc->lo; \
    else if (!(lo=czl_get_opr(gp, pc->lt, pc->lo))) \
        goto CZL_EXCEPTION_CATCH; \
}

//设置函数带临时结果双目运算符操作数： set fun binary2 opt opr
#define CZL_SFB2OO(gp, lo, ro, pc, cur, index) \
if (pc->lt != CZL_USR_FUN) { \
    lo = pc->res; \
    ro = &cur->fun->ret; \
    if (pc->res->name) { \
        ((czl_var*)pc->res->name)->quality = CZL_OBJ_ELE; \
        pc->res->name = NULL; \
    } \
} \
else if (pc->rt != CZL_USR_FUN) { \
    if (CZL_REG_VAR == pc->rt && pc->ro->type != CZL_OBJ_REF) \
        ro = pc->ro; \
    else if (!(ro=czl_get_opr(gp, pc->rt, pc->ro))) \
        goto CZL_EXCEPTION_CATCH; \
    *pc->res = cur->fun->ret; \
    cur->fun->ret.type = CZL_INT; \
    lo = pc->res; \
} \
else if (CZL_NIL == pc->res->type) { \
    *pc->res = cur->fun->ret; \
    cur->fun->ret.type = CZL_INT; \
    if (!(cur->fun=czl_fun_run_prepare(gp, (czl_exp_fun*)pc->ro))) \
        goto CZL_EXCEPTION_CATCH; \
    ++index; \
    pc = (cur->fun->pc ? cur->fun->pc : cur->fun->opcode); \
    goto CZL_BEGIN; \
} \
else { \
    lo = pc->res; \
    ro = &cur->fun->ret; \
}

//检查调整用户函数栈大小: check resize usrfun stack size
//函数调用在16层以内不会导致stack内存变动
#define CZL_CRUFSS(gp, index, size, stack) \
if (index >= 16 && index <= size>>2) { \
    czl_usrfun_stack *tmp = (czl_usrfun_stack*)CZL_STACK_REALLOC(gp, stack, \
                            (size>>1)*sizeof(czl_usrfun_stack), \
                            size*sizeof(czl_usrfun_stack)); \
    if (tmp) { \
        stack = tmp; \
        size >>= 1; \
    } \
}

//用户函数返回: usr fun return
#define CZL_UFR(gp, index, size, stack, cur, pc, bp, lo, ro) \
if (0 == index) { \
    CZL_STACK_FREE(gp, stack, size*sizeof(czl_usrfun_stack)); \
    if (CZL_YEILD_SENTENCE == pc->flag) gp->yeild_pc = pc + 1; \
    return 1; \
} \
CZL_CRUFSS(gp, index, size, stack); \
cur = stack + --index; \
if (CZL_RETURN_SENTENCE == pc->flag) { \
    czl_fun_local_vars_clean(gp, cur->fun); \
} \
else { \
    cur->fun->pc = pc + 1; \
    cur->fun->state = CZL_IN_IDLE; \
    czl_coroutine_paras_backup(cur->fun, (czl_var*)cur->fun->backup_vars); \
    if (cur->fun->cur_ins) \
        cur->fun->cur_ins = NULL; \
} \
bp = cur->bp; \
pc = cur->pc; \
switch (pc->flag) \
{ \
case CZL_OPERAND: \
    lo = &cur->fun->ret; \
    CZL_COT(gp, lo, pc); \
    CZL_CF_FR2(gp, lo); \
    break; \
case CZL_UNARY_OPT: \
    lo = &cur->fun->ret; \
    CZL_RFUO(gp, lo, pc); \
    break; \
case CZL_ASS_OPT: case CZL_BINARY_OPT: \
    CZL_SFBOO(gp, lo, ro, pc, cur); \
    CZL_RFBO(gp, lo, ro, pc); \
    break; \
default: \
    CZL_SFB2OO(gp, lo, ro, pc, cur, index); \
    CZL_RFB2O(gp, lo, ro, pc); \
    break; \
} \
CZL_GNO(pc); \
goto CZL_BEGIN;

//执行用户函数: run usr fun
#define CZL_RUF(gp, index, size, stack, cur, pc, bp, lo) \
if (index == size) { \
    unsigned long new_size = (size ? size<<1 : 1); \
    czl_usrfun_stack *tmp = (czl_usrfun_stack*)CZL_STACK_REALLOC(gp, stack, \
                            new_size*sizeof(czl_usrfun_stack), \
                            size*sizeof(czl_usrfun_stack)); \
    if (!tmp) \
        goto CZL_EXCEPTION_CATCH; \
    stack = tmp; \
    size = new_size; \
} \
cur = stack + index; \
cur->pc = pc; \
cur->bp = bp; \
switch (pc->flag) \
{ \
case CZL_OPERAND: lo = pc->res; break; \
case CZL_UNARY_OPT: lo = pc->lo; break; \
case CZL_ASS_OPT: case CZL_BINARY_OPT: lo = pc->ro; break; \
default: \
    lo = (CZL_USR_FUN == pc->lt ? pc->lo : pc->ro); \
    if (CZL_USR_FUN == pc->lt) pc->res->type = CZL_NIL; \
    break; \
} \
if ((cur->fun=czl_fun_run_prepare(gp, (czl_exp_fun*)lo))) { \
    ++index; \
    pc = (cur->fun->pc ? cur->fun->pc : cur->fun->opcode); \
    goto CZL_BEGIN; \
} \
if (pc->res->name) { \
    ((czl_var*)pc->res->name)->quality = CZL_OBJ_ELE; \
    pc->res->name = NULL; \
}
///////////////////////////////////////////////////////////////
//编译表达式: compile exp
#define CZL_CE(exp, buf, cnt)  \
if (exp) { \
    cnt = czl_get_exp_cnt(exp); \
    memcpy(buf, exp, cnt*sizeof(czl_exp_ele)); \
    buf += cnt; \
}
///////////////////////////////////////////////////////////////
extern char (*const czl_1p_opt_cac_funs[])(czl_var*);
extern char (*const czl_2p_opt_cac_funs[])(czl_gp*, czl_var*, czl_var*);
///////////////////////////////////////////////////////////////
char czl_str_resize(czl_gp*, czl_str*);
char czl_val_copy(czl_gp*, czl_var*, czl_var*);
///////////////////////////////////////////////////////////////
char czl_ass_cac(czl_gp*, czl_var*, czl_var*);
char czl_add_a_cac(czl_gp*, czl_var*, czl_var*);
char czl_equ_equ_cac(czl_gp*, czl_var*, czl_var*);
///////////////////////////////////////////////////////////////
#endif //CZL_OPT_H

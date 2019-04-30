#include "czl_vm.h"
#include "czl_opt.h"
#include "czl_lib.h"
#include "czl_paser.h"

///////////////////////////////////////////////////////////////
//单目运算符列表
const czl_unary_operator czl_unary_opt_table[] =
{
    {"!",   CZL_LOGIC_NOT,  CZL_POS_LEFT},
    {"~",   CZL_LOGIC_FLIP, CZL_POS_LEFT},
    {"&",   CZL_REF_VAR,    CZL_POS_LEFT},
    {"++",  CZL_ADD_SELF,   CZL_POS_LR},
    {"--",  CZL_DEC_SELF,   CZL_POS_LR},
    {"-",   CZL_NUMBER_NOT, CZL_POS_LEFT},
    {"#",   CZL_OBJ_CNT,    CZL_POS_LEFT},
};
const unsigned long czl_unary_opt_table_num =
        sizeof(czl_unary_opt_table) / sizeof(czl_unary_operator);

//双目运算符列表
const czl_binary_operator czl_binary_opt_table[] =
{
    //
    {"|=",  CZL_OR_A,       4,   CZL_RIGHT_TO_LEFT},
    {"^=",  CZL_XOR_A,      4,   CZL_RIGHT_TO_LEFT},
    {"&=",  CZL_AND_A,      4,   CZL_RIGHT_TO_LEFT},
    {">>=", CZL_R_SHIFT_A,  4,   CZL_RIGHT_TO_LEFT},
    {"<<=", CZL_L_SHIFT_A,  4,   CZL_RIGHT_TO_LEFT},
    {"-=",  CZL_DEC_A,      4,   CZL_RIGHT_TO_LEFT},
    {"+=",  CZL_ADD_A,      4,   CZL_RIGHT_TO_LEFT},
    {"%=",  CZL_MOD_A,      4,   CZL_RIGHT_TO_LEFT},
    {"/=",  CZL_DIV_A,      4,   CZL_RIGHT_TO_LEFT},
    {"*=",  CZL_MUL_A,      4,   CZL_RIGHT_TO_LEFT},
    {"><",  CZL_SWAP,       4,   CZL_RIGHT_TO_LEFT},
    //
    {"||",  CZL_OR_OR,      6,   CZL_LEFT_TO_RIGHT},
    {"&&",  CZL_AND_AND,    7,   CZL_LEFT_TO_RIGHT},
    {"^^",  CZL_XOR_XOR,    6,   CZL_LEFT_TO_RIGHT},
    //
    {">>",  CZL_R_SHIFT,    11,  CZL_LEFT_TO_RIGHT},
    {"<<",  CZL_L_SHIFT,    11,  CZL_LEFT_TO_RIGHT},
    //
    {"??",  CZL_CMP,        8,   CZL_LEFT_TO_RIGHT},
    {"=>",  CZL_ELE_DEL,    8,   CZL_LEFT_TO_RIGHT},
    {"->",  CZL_ELE_INX,    8,   CZL_LEFT_TO_RIGHT},
    {"===", CZL_EQU_3,      8,   CZL_LEFT_TO_RIGHT},
    {"<=",  CZL_LESS_EQU,   8,   CZL_LEFT_TO_RIGHT},
    {"==",  CZL_EQU_EQU,    8,   CZL_LEFT_TO_RIGHT},
    {"!=",  CZL_NOT_EQU,    8,   CZL_LEFT_TO_RIGHT},
    {">=",  CZL_MORE_EQU,   8,   CZL_LEFT_TO_RIGHT},
    {"<",   CZL_LESS,       8,   CZL_LEFT_TO_RIGHT},
    {">",   CZL_MORE,       8,   CZL_LEFT_TO_RIGHT},
    //
    {"=",   CZL_ASS,        4,   CZL_RIGHT_TO_LEFT},
    //
    {"|",   CZL_OR,         9,   CZL_LEFT_TO_RIGHT},
    {"^",   CZL_XOR,        10,  CZL_LEFT_TO_RIGHT},
    {"&",   CZL_AND,        10,  CZL_LEFT_TO_RIGHT},
    //
    {"+",   CZL_ADD,        12,  CZL_LEFT_TO_RIGHT},
    {"-",   CZL_DEC,        12,  CZL_LEFT_TO_RIGHT},
    {"*",   CZL_MUL,        13,  CZL_LEFT_TO_RIGHT},
    {"/",   CZL_DIV,        13,  CZL_LEFT_TO_RIGHT},
    {"%",   CZL_MOD,        13,  CZL_LEFT_TO_RIGHT},
    //"?"与":"组成三目运算符"?:"
    {"?",   CZL_QUE,        5,   CZL_RIGHT_TO_LEFT},
    {":",   CZL_COL,        5,   CZL_RIGHT_TO_LEFT},
};
const unsigned long czl_binary_opt_table_num =
        sizeof(czl_binary_opt_table) / sizeof(czl_binary_operator);
///////////////////////////////////////////////////////////////
static void czl_set_array_list_reg(czl_gp*, czl_array_list*);
static void czl_set_table_list_reg(czl_gp*, czl_table_list*);
static void czl_set_new_reg(czl_gp*, czl_new_sentence*);
static void czl_set_exp_reg(czl_gp*, czl_exp_ele*, char flag);
static czl_var* czl_exp_stack_cac(czl_gp*, czl_exp_stack);
static char czl_fun_run(czl_gp*, czl_exp_ele*, czl_fun*);
static void czl_set_char(czl_gp*);
static czl_var* czl_get_member_res(czl_gp*, const czl_obj_member*);
///////////////////////////////////////////////////////////////
static void czl_ref_vars_update
(
    czl_var *var,
    czl_var *q,
    unsigned long cnt
)
{
    czl_ref_var *p = ((czl_ref_obj*)var->name)->head;
    while (p)
    {
        //递归时原局部变量为新的递归函数的入参变量时，不需要更新引用
        //即: >= q && < q + cnt 时
        if (p->var < q || p->var >= q + cnt)
            p->var->val.ref.var = var;
        p = p->next;
    }
}

static void czl_loc_vars_set(czl_fun *fun)
{
    czl_var *reg = fun->backup_vars[fun->backup_cnt-1];
    czl_var *p = reg;
    czl_var *q = fun->vars;
    czl_var tmp;

    unsigned long i, j = fun->enter_vars_count;
    for (i = 0; i < j; ++i, ++p, ++q)
    {
        if (CZL_OBJ_REF == q->type && q->val.ref.inx < 0 &&
            (q->val.ref.var >= reg &&
             q->val.ref.var < reg + fun->dynamic_vars_cnt))
        {
            q->val.ref.var = fun->vars + (q->val.ref.var - reg);
            ((czl_ref_var*)q->name)->var = p;
        }
        CZL_VAL_SWAP(*p, *q, tmp);
        if (CZL_OBJ_REF == q->type)
        {
            ((czl_ref_var*)q->name)->var = q;
        }
        else if (q->name)
            czl_ref_vars_update(q, fun->vars, fun->enter_vars_count);
    }

    j = fun->dynamic_vars_cnt;
    while (i++ < j)
    {
        *q = *p;
        if (CZL_OBJ_REF == q->type)
            ((czl_ref_var*)q->name)->var = q;
        else if (q->name)
        {
            czl_ref_vars_update(q, fun->vars, fun->enter_vars_count);
            p->name = NULL;
        }
        p->quality = CZL_DYNAMIC_VAR;
        p->type = CZL_INT;
        ++p; ++q;
    }

    fun->backup_vars[fun->backup_cnt-1] = fun->vars;
    fun->vars = reg;
}

static char czl_fun_local_vars_backup(czl_gp *gp, czl_fun *fun)
{
	czl_var *p, *q, *vars;
	unsigned long i, j = fun->enter_vars_count;
    unsigned long cnt = fun->dynamic_vars_cnt;

    if (fun->type != CZL_SYS_FUN)
        cnt += (fun->reg_cnt + fun->foreach_cnt);
    if (!cnt)
        return 1;

    if (fun->backup_cnt == fun->backup_size)
    {
        unsigned long new_size = (fun->backup_size ?
                                  fun->backup_size<<1 : 1);
        czl_var_arr *vars = (czl_var**)CZL_STACK_REALLOC(gp, fun->backup_vars,
                             new_size*sizeof(czl_var_arr),
                             fun->backup_size*sizeof(czl_var_arr));
        if (!vars)
            return 0;
        fun->backup_vars = vars;
        fun->backup_size = new_size;
    }

    if (!(vars=(czl_var*)CZL_STACK_MALLOC(gp, cnt*sizeof(czl_var))))
        return 0;
    p = vars;
	q = fun->vars;
    for (i = 0; i < j; ++i)
    {
        p->name = NULL;
        p->type = CZL_INT;
        p->quality = CZL_DYNAMIC_VAR;
        (p++)->ot = (q++)->ot;
    }
    fun->backup_vars[fun->backup_cnt++] = fun->vars;
    fun->vars = vars;

    if (fun->type != CZL_SYS_FUN)
    {
        czl_var *tmp = vars + fun->dynamic_vars_cnt;
        if (fun->reg_cnt)
        {
            unsigned long size = fun->reg_cnt*sizeof(czl_var);
            memcpy(tmp, fun->reg, size);
            memset(fun->reg, 0, size);
        }
        if (fun->foreach_cnt)
        {
            czl_foreach *f = fun->foreachs;
            unsigned long i, j = fun->foreach_cnt;
			tmp += fun->reg_cnt;
            for (i = 0; i < j; ++i, ++tmp, ++f)
            {
                tmp->type = f->type;
                tmp->val.msg.obj = f->c;
                tmp->val.msg.inx = f->para.inx;
            }
        }
    }

    fun->state = CZL_IN_RECURSION;
    return 1;
}

static void czl_reg_check(czl_gp *gp, czl_var *head, unsigned long cnt)
{
    unsigned long i;
    for (i = 0; i < cnt; ++i)
    {
        if (CZL_ARRBUF_VAR == head[i].quality ||
            CZL_FUNRET_VAR == head[i].quality)
        {
            czl_val_del(gp, head + i);
            head[i].quality = CZL_DYNAMIC_VAR;
        }
    }
}

void czl_ref_obj_update(czl_var *var)
{
    czl_ref_var *p = ((czl_ref_obj*)var->name)->head;
    while (p)
    {
        p->var->val.ref.var = var;
        p = p->next;
    }
}

static void czl_fun_local_vars_restore(czl_gp *gp, czl_fun *fun)
{
	czl_var *vars;
	unsigned long cnt;

    if (!fun->dynamic_vars_cnt && !fun->reg_cnt && !fun->foreach_cnt)
        return;

    vars = fun->backup_vars[fun->backup_cnt-1];

    if (fun->dynamic_vars_cnt)
    {
        czl_var *p = fun->vars;
        unsigned long i, j = fun->dynamic_vars_cnt;
        for (i = 0; i < j; ++i, ++p, ++vars)
        {
            if (CZL_VAR_EXIST_REF(p))
                czl_ref_obj_delete(gp, p);
            czl_val_del(gp, p);
            *p = *vars;
            if (CZL_OBJ_REF == p->type)
                ((czl_ref_var*)p->name)->var = p;
            else if (p->name)
                czl_ref_obj_update(p);
        }
    }

    if (fun->type != CZL_SYS_FUN)
    {
        if (fun->reg_cnt)
        {
            czl_reg_check(gp, fun->reg, fun->reg_cnt);
            memcpy(fun->reg, vars, fun->reg_cnt*sizeof(czl_var));
        }
        if (fun->foreach_cnt)
        {
            czl_foreach *f = fun->foreachs;
            unsigned long i, j = fun->foreach_cnt;
			vars += fun->reg_cnt;
            for (i = 0; i < j; ++i, ++vars, ++f)
            {
                f->type = vars->type;
                f->c = (czl_exp_ele*)vars->val.msg.obj;
                f->para.inx = vars->val.msg.inx;
            }
        }
    }

    cnt = fun->dynamic_vars_cnt;
    if (fun->type != CZL_SYS_FUN)
        cnt += (fun->reg_cnt + fun->foreach_cnt);

    CZL_STACK_FREE(gp,
                   fun->backup_vars[fun->backup_cnt-1],
                   cnt*sizeof(czl_var));

    if (fun->backup_cnt <= fun->backup_size>>2)
    {
        czl_var **tmp = (czl_var**)CZL_STACK_REALLOC(gp, fun->backup_vars,
                        (fun->backup_size>>1)*sizeof(czl_var_arr),
                         fun->backup_size*sizeof(czl_var_arr));
        if (tmp)
        {
            fun->backup_vars = tmp;
            fun->backup_size >>= 1;
        }
    }

    if (0 == --fun->backup_cnt)
        fun->state = CZL_IN_BUSY;
}

static void czl_loc_vars_free
(
    czl_gp *gp,
    czl_var *p,
    unsigned long cnt
)
{
    unsigned long i;
    for (i = 0; i < cnt; ++i, ++p)
    {
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, p);
        switch (p->type)
        {
        case CZL_INT:
            continue;
        case CZL_FLOAT:
            if (CZL_FLOAT == p->ot)
                continue;
            break;
        case CZL_FUN_REF:
            if (CZL_FUN_REF == p->ot)
            {
                p->val.fun = NULL;
                continue;
            }
            break;
        case CZL_OBJ_REF:
            //第一层函数调用完不释放指针内存，
            //避免带&参数的函数调用出现频繁的指针内存分配和释放问题
            czl_ref_break(gp, (czl_ref_var*)p->name, CZL_GRV(p));
            break;
        case CZL_STRING:
            CZL_SRCD1(gp, p->val.str);
            break;
        case CZL_FILE:
            if (0 == CZL_ORCD1(p->val.Obj))
                czl_file_delete(gp, p->val.Obj);
            break;
        case CZL_INSTANCE:
            if (0 == CZL_ORCD1(p->val.Obj))
                czl_instance_delete(gp, p->val.Obj);
            break;
        case CZL_TABLE:
            if (0 == CZL_ORCD1(p->val.Obj))
                czl_table_delete(gp, p->val.Obj);
            break;
        case CZL_ARRAY:
            if (0 == CZL_ORCD1(p->val.Obj))
                czl_array_delete(gp, p->val.Obj);
            break;
        default: //CZL_STACK/CZL_QUEUE
            if (0 == CZL_ORCD1(p->val.Obj))
                czl_sq_delete(gp, p->val.Obj);
            break;
        }
        p->type = CZL_INT;
    }
}

static void czl_fun_local_vars_free(czl_gp *gp, czl_fun *fun)
{
    czl_loc_vars_free(gp, fun->vars, fun->dynamic_vars_cnt);

    if (fun->type != CZL_SYS_FUN)
    {
        czl_reg_check(gp, fun->reg, fun->reg_cnt);
        if (fun->cur_ins)
            fun->cur_ins = NULL;
        if (fun->yeild_flag)
            fun->pc = NULL;
    }

    fun->state = CZL_IN_IDLE;
}

static void czl_vars_delete(czl_gp *gp, czl_var *vars, unsigned long cnt, char flag)
{
    czl_var *p = vars;
    unsigned long i;
    for (i = 0; i < cnt; ++i, ++p)
    {
        if (p->name && p->type != CZL_OBJ_REF)
        {
            czl_ref_obj *obj = (czl_ref_obj*)p->name;
            if (obj->cnt <= CZL_MAX_MEMBER_INDEX_LAYER)
                czl_ref_obj_delete(gp, p);
            else //第一层函数调用中没有释放的指针内存
                CZL_REF_FREE(gp, obj);
            p->name = NULL;
        }
        czl_val_del(gp, p);
        p->type = CZL_INT;
        p->val.inum = 0;
    }
    if (flag)
        CZL_RT_FREE(gp, vars, cnt*sizeof(czl_var));
}

static void czl_backup_vars_delete(czl_gp *gp, czl_fun *fun)
{
    if ((CZL_SYS_FUN == fun->type && fun->enter_vars_count <= 0) ||
        (fun->type != CZL_SYS_FUN &&
         ((0 == (fun->dynamic_vars_cnt ||
                fun->reg_cnt || fun->foreach_cnt)) || fun->yeild_flag)))
        return;

    if (fun->dynamic_vars_cnt)
        CZL_STACK_FREE(gp, fun->backup_vars, fun->backup_size*sizeof(czl_var_arr));
}
///////////////////////////////////////////////////////////////
static char czl_set_fun_class_ptr_vars
(
    czl_ins *ins,
    const czl_class_ptr_vars *ptr_vars
)
{
	unsigned long i, j = ins->pclass->parents_count;

    //利用地址一致性避免耗时的字符串比较
    if (ins->pclass == ptr_vars->pclass)
    {
        czl_ins_var *p = ptr_vars->vars;
        czl_var *vars = CZL_GIV(ins);
        do {
            p->var = vars + p->index;
        } while ((p=p->next));
        if (!(ptr_vars=ptr_vars->next))
            return 1;
    }

    for (i = 0; i < j; i++)
        if (czl_set_fun_class_ptr_vars(CZL_INS(ins->parents[i]), ptr_vars))
            return 1;

    return 0;
}

static char czl_set_class_fun_vars(czl_gp *gp, czl_fun *fun)
{
    //每个类成员函数同一时刻只能被一个实例占用，发生抢用时抛出异常
    if (fun->cur_ins)
    {
        if (fun->cur_ins != gp->cur_ins)
        {
            gp->exceptionCode = CZL_EXCEPTION_CLASS_FUNCTION_GRAB;
            return 0;
        }
        return 1;
    }

    //@class_fun情况没有指定实例，这种情况是不允许的
    if (!gp->cur_ins)
    {
        gp->exceptionCode = CZL_EXCEPTION_FUNCTION_CALL_NO_INSTANCE;
        return 0;
    }

    fun->cur_ins = gp->cur_ins;
    czl_set_fun_class_ptr_vars(CZL_INS(fun->cur_ins), fun->class_vars);
    return 1;
}
///////////////////////////////////////////////////////////////
char czl_is_member_var(char type, const czl_obj_member *obj)
{
	czl_member_index *index;

    if (type != CZL_MEMBER || !obj->index || CZL_CONST_VAR == obj->obj->quality)
        return 0;

    index = obj->index;

    while (index->next)
        index = index->next;

    if (CZL_ARRAY_INDEX == index->type)
    {
        if (index->index.arr.exp_fun)
            return 0;
    }
    else //CZL_INSTANCE_INDEX
    {
        if (index->index.ins.exp_fun)
            return 0;
    }

    return 1;
}

static char czl_is_ref_or_var(czl_gp *gp, czl_exp_ele *exp)
{
    if (CZL_UNARY_OPT == exp->flag && exp->rt)
        return 1;
    else if (CZL_EIO(exp) &&
             (CZL_LG_VAR == exp->kind ||
              CZL_INS_VAR == exp->kind ||
              CZL_REG_VAR == exp->kind || //只有运行动态函数时出现该情况
              (CZL_MEMBER == exp->kind &&
               czl_is_member_var(CZL_MEMBER, (czl_obj_member*)exp->res))))
    {
        if (CZL_MEMBER == exp->kind)
            ((czl_obj_member*)exp->res)->flag = CZL_REF_VAR;
        exp->lt = exp->kind;
        exp->ro = exp->lo = exp->res;
        exp->res = gp->exp_reg;
        exp->flag = CZL_UNARY2_OPT;
        exp->kind = CZL_REF_VAR-CZL_NUMBER_NOT;
        return 1;
    }

    return 0;
}

static char czl_fun_para_explain_check
(
    czl_gp *gp,
    const czl_fun *fun,
    czl_exp_fun *exp_fun
)
{
	int i = 0;
    czl_para *p = exp_fun->paras;
    czl_para_explain *e = fun->para_explain;

    if (exp_fun->paras_count > fun->enter_vars_count)
    {
        sprintf(gp->log_buf, "paras number of function %s is not matched, ", fun->name);
        return 0;
    }
    else if (exp_fun->paras_count < fun->enter_vars_count)
        exp_fun->flag = 0;

    while (p)
    {
        if (!p->para)
            exp_fun->flag = 0;
        if (!p->para && (!e || !e->para || e->index != i))
        {
            sprintf(gp->log_buf, "para %d of function %s has no default para, ", i+1, fun->name);
            return 0;
        }
        ++i;
        if (e && e->index < i)
        {
            if (e->ref_flag && (!p->para || !czl_is_ref_or_var(gp, p->para)))
            {
                sprintf(gp->log_buf, "para %d of function %s should be a variable for &, ", i, fun->name);
                return 0;
            }
            e = e->next;
        }
        p = p->next;
    }

    while (e)
    {
        if (!e->para || e->index != i)
        {
            sprintf(gp->log_buf, "para %d of function %s has no default para, ", i+1, fun->name);
            return 0;
        }
        ++i;
        e = e->next;
    }

    if (i != fun->enter_vars_count)
    {
        sprintf(gp->log_buf, "paras number of function %s is not matched, ", fun->name);
        return 0;
    }

    return 1;
}

static char czl_fun_unsure_paras_check(czl_gp *gp, const czl_para *p)
{
    while (p)
    {
        if (!p->para)
        {
            sprintf(gp->log_buf, "para should not be empty, ");
            return 0;
        }
        if (CZL_EIO(p->para) && CZL_NEW == p->para->kind)
        {
            sprintf(gp->log_buf, "new should not be used in function of unsure paras number, ");
            return 0;
        }
        p = p->next;
    }

    return 1;
}

static char czl_fun_paras_null_check(czl_gp *gp, czl_exp_fun *exp_fun)
{
    czl_para *p = exp_fun->paras;

    while (p)
    {
        if (!p->para)
        {
            if (exp_fun->fun->yeild_flag)
                sprintf(gp->log_buf, "para of coroutine %s should not be empty, ", exp_fun->fun->name);
            else
                sprintf(gp->log_buf, "para of function %s should not be empty, ", exp_fun->fun->name);
            return 0;
        }
        p = p->next;
    }

    return 1;
}

char czl_fun_paras_check
(
    czl_gp *gp,
    czl_exp_fun *exp_fun,
    const czl_fun *fun
)
{
    if (fun->type != CZL_SYS_FUN && fun->yeild_flag)
    {
        if (exp_fun->paras_count > fun->enter_vars_count)
        {
            sprintf(gp->log_buf, "paras number of coroutine %s is not matched, ", fun->name);
            return 0;
        }
        return czl_fun_paras_null_check(gp, exp_fun);
    }

    if (fun->para_explain)
    {
        return czl_fun_para_explain_check(gp, fun, exp_fun);
    }
    else
    {
        if (fun->enter_vars_count < 0) {
            if (exp_fun->paras_count >= -fun->enter_vars_count)
                return czl_fun_unsure_paras_check(gp, exp_fun->paras);
        }
        else {
            if (exp_fun->paras_count == fun->enter_vars_count)
                return czl_fun_paras_null_check(gp, exp_fun);
        }
        sprintf(gp->log_buf, "paras number of function %s is not matched, ", fun->name);
        return 0;
    }
}

static void czl_coroutine_paras_copy(czl_fun *fun, czl_var *vars)
{
    czl_var *p = fun->vars;
    unsigned long i, j = fun->dynamic_vars_cnt;
    for (i = 0; i < j; ++i, ++vars, ++p)
    {
        p->type = vars->type;
        p->val = vars->val;
        if (vars->name)
        {
            p->name = vars->name;
            if (vars->type != CZL_OBJ_REF)
                czl_ref_obj_update(p);
            vars->name = NULL;
        }
        vars->type = CZL_INT;
    }

    j = fun->foreach_cnt;
    for (i = 0; i < j; ++i)
    {
        fun->foreachs[i].type = vars[i].type;
        fun->foreachs[i].c = (czl_exp_ele*)vars[i].val.msg.obj;
        fun->foreachs[i].para.inx = vars[i].val.msg.inx;
    }
}

static void czl_coroutine_paras_backup(czl_fun *fun, czl_var *vars)
{
    czl_var *p = fun->vars;
    unsigned long i, j = fun->dynamic_vars_cnt;
    for (i = 0; i < j; ++i, ++vars, ++p)
    {
        vars->type = p->type;
        vars->val = p->val;
        if (p->name)
        {
            vars->name = p->name;
            if (p->type != CZL_OBJ_REF)
                czl_ref_obj_update(vars);
            p->name = NULL;
        }
        p->type = CZL_INT;
    }

    j = fun->foreach_cnt;
    for (i = 0; i < j; ++i)
    {
        vars[i].type = fun->foreachs[i].type;
        vars[i].val.msg.obj = fun->foreachs[i].c;
        vars[i].val.msg.inx = fun->foreachs[i].para.inx;
    }
}

void czl_coroutine_paras_reset(czl_gp *gp, czl_var *vars, unsigned long cnt)
{
    czl_vars_delete(gp, vars, cnt, 0);
}

void czl_coroutine_delete(czl_gp *gp, czl_var *vars, void **obj)
{
    czl_coroutine *c = CZL_COR(obj);

    czl_vars_delete(gp, vars, c->fun->dynamic_vars_cnt, 0);
    CZL_STACK_FREE(gp, c->vars, (c->fun->dynamic_vars_cnt+
                                 c->fun->foreach_cnt)*sizeof(czl_var));

    if (c->last)
        CZL_COR(c->last)->next = c->next;
    else
        gp->coroutines_head = c->next;
    if (c->next)
        CZL_COR(c->next)->last = c->last;

    CZL_COR_FREE(gp, obj);
    czl_sys_hash_delete(gp, (unsigned long)obj, &gp->coroutines_hash);
}

czl_var* czl_coroutine_run
(
    czl_gp *gp,
    czl_para *p,
    unsigned long paras_cnt,
    void **obj
)
{
    czl_coroutine *c = CZL_COR(obj);
    czl_fun *fun = c->fun;
	char ret = 1;
    czl_var *vars = fun->vars;
    unsigned long i, j = fun->enter_vars_count;

    //检查释放函数返回值
    if (fun->ret.type != CZL_INT)
    {
        czl_val_del(gp, &fun->ret);
        fun->ret.type = CZL_INT;
    }

    //类成员函数
    if (fun->class_vars && !czl_set_class_fun_vars(gp, fun))
        return NULL;

    if (CZL_IN_IDLE == fun->state)
    {
        if (c->pc)
        {
            czl_loc_vars_free(gp, c->vars, paras_cnt);
            czl_coroutine_paras_copy(fun, c->vars);
        }
        fun->state = CZL_IN_BUSY;
    }
    else //CZL_IN_BUSY
    {
        //调度器yeild函数不允许递归调用
        gp->exceptionCode = CZL_EXCEPTION_YEILD_FUNCTION_GRAB;
        return NULL;
    }

    for (i = 0; i < j && p; p = p->next, ++i)
    {
        if (!czl_val_copy(gp, vars+i, czl_exp_stack_cac(gp, p->para)))
        {
            ret = 0;
            break;
        }
    }

    if (ret)
        ret = czl_fun_run(gp, c->pc ? c->pc : fun->opcode, fun);

    fun->state = CZL_IN_IDLE;

    if (gp->yeild_pc)
    {
        gp->yeild_end = 0;
        c->pc = gp->yeild_pc;
        gp->yeild_pc = NULL;
        czl_coroutine_paras_backup(fun, c->vars);
        if (fun->cur_ins)
            fun->cur_ins = NULL;
    }
    else
    {
        gp->yeild_end = 1;
        if (fun->cur_ins)
            fun->cur_ins = NULL;
        fun->pc = NULL;
        if (c->type)
        {
            c->pc = NULL;
            czl_vars_delete(gp, vars, fun->dynamic_vars_cnt, 0);
        }
        else
            czl_coroutine_delete(gp, vars, obj);
    }

    return (ret ? &fun->ret : NULL);
}

static void czl_fun_local_vars_clean(czl_gp *gp, czl_fun *fun)
{
    if (CZL_IN_BUSY == fun->state)
        czl_fun_local_vars_free(gp, fun);
    else //CZL_IN_RECURSION
        czl_fun_local_vars_restore(gp, fun);
}

static czl_fun* czl_dinamic_fun_check(czl_gp *gp, czl_exp_fun *exp_fun)
{
    czl_fun *fun;

    if (CZL_INS_STATIC_FUN == exp_fun->quality)
        fun = exp_fun->fun;
    else
    {
        czl_var *var = (CZL_LG_VAR_DYNAMIC_FUN == exp_fun->quality ?
                        (czl_var*)exp_fun->fun :
                        ((czl_ins_var*)exp_fun->fun)->var);
        if (CZL_OBJ_REF == var->type)
            var = CZL_GRV(var);
        if (var->type != CZL_FUN_REF || !(fun=var->val.fun))
        {
            gp->exceptionCode = CZL_EXCEPTION_FUNCTION_CALL_NO_FUN_PTR;
            return NULL;
        }
    }

    if (!czl_fun_paras_check(gp, exp_fun, fun))
    {
        gp->exceptionCode = CZL_EXCEPTION_FUNCTION_CALL_PARAS_NOT_MATCH;
        return NULL;
    }
    return fun;
}

static czl_fun* czl_fun_run_prepare(czl_gp *gp, czl_exp_fun *exp_fun)
{
    czl_fun *fun;
    czl_var *var;
    const czl_para *p;

    //获取函数指针
    if (exp_fun->quality == CZL_STATIC_FUN)
        fun = exp_fun->fun;
    else if (!(fun=czl_dinamic_fun_check(gp, exp_fun)))
        return NULL;

    //检查释放函数返回值
    if (fun->ret.type != CZL_INT)
    {
        czl_val_del(gp, &fun->ret);
        fun->ret.type = CZL_INT;
    }

    //系统参数个数不确定函数独立执行
    if (fun->enter_vars_count < 0)
    {
        fun->vars = (czl_var*)exp_fun->paras;
        fun->paras_cnt = exp_fun->paras_count;
        return fun;
    }

    //类成员函数
    if (fun->type != CZL_SYS_FUN &&
        fun->class_vars && !czl_set_class_fun_vars(gp, fun))
        return NULL;

    //检查函数是否发生递归调用
    if (CZL_IN_IDLE == fun->state)
    {
        if (fun->type != CZL_SYS_FUN && fun->yeild_flag && fun->pc)
        {
            czl_loc_vars_free(gp, (czl_var*)fun->backup_vars, exp_fun->paras_count);
            czl_coroutine_paras_copy(fun, (czl_var*)fun->backup_vars);
        }
        fun->state = CZL_IN_BUSY;
    }
    else //CZL_IN_BUSY、CZL_IN_RECURSION
    {
        if (fun->type != CZL_SYS_FUN && fun->yeild_flag)
        {
            //调度器yeild函数不允许递归调用
            gp->exceptionCode = CZL_EXCEPTION_YEILD_FUNCTION_GRAB;
            return NULL;
        }
        if (!czl_fun_local_vars_backup(gp, fun))
            return NULL;
    }

    var = fun->vars;
    p = exp_fun->paras;

    if (exp_fun->flag)
    {
        while (p)
        {
            if (!czl_val_copy(gp, var++, czl_exp_stack_cac(gp, p->para)))
                goto CZL_END;
            p = p->next;
        }
    }
    else
    {
        unsigned long i = 0, j = fun->enter_vars_count;
        czl_para_explain *q = fun->para_explain;
        while (i++ < j)
        {
            if (!czl_val_copy(gp, var++, czl_exp_stack_cac(gp, ((p && p->para) ?
                                                                p->para : q->para))))
                goto CZL_END;
            if (q && q->index < i) q = q->next;
            if (p) p = p->next;
        }
    }

    if (CZL_IN_RECURSION == fun->state && fun->dynamic_vars_cnt)
        czl_loc_vars_set(fun);
    return fun;

CZL_END:
    if (CZL_IN_RECURSION == fun->state && fun->dynamic_vars_cnt)
        czl_loc_vars_set(fun);
    czl_fun_local_vars_clean(gp, fun);
    return NULL;
}

static czl_var* czl_exp_fun_run(czl_gp *gp, czl_exp_fun *exp_fun)
{
	char ret;
	czl_fun *fun;

    if (gp->fun_deep > CZL_MAX_STACK_SIZE)
    {
        gp->exceptionCode = CZL_EXCEPTION_STACK_OVERFLOW;
        return NULL;
    }

    if (!(fun=czl_fun_run_prepare(gp, exp_fun)))
        return NULL;

    ++gp->fun_deep;

    //执行函数
    if (CZL_SYS_FUN == fun->type)
    {
        char code = gp->exceptionCode;
        ret = fun->sys_fun(gp, fun);
        if (ret && gp->exceptionCode != CZL_EXCEPTION_NO && CZL_EXCEPTION_NO == code)
            gp->exceptionCode = CZL_EXCEPTION_NO;
        else if (!ret && CZL_EXCEPTION_NO == gp->exceptionCode)
            gp->exceptionCode = CZL_EXCEPTION_SYSFUN_RUN_ERROR;
        if (gp->fun_ret)
        {
            czl_val_del(gp, gp->fun_ret);
            gp->fun_ret->type = CZL_INT;
            gp->fun_ret = NULL;
        }
    }
    else if (!(ret=czl_fun_run(gp, fun->pc ? fun->pc : fun->opcode, fun)))
    {
        if (!fun->yeild_flag && gp->yeild_pc)
            gp->yeild_pc = NULL;
    }

    if (fun->type != CZL_SYS_FUN && gp->yeild_pc)
    {
        gp->yeild_end = 0;
        fun->pc = gp->yeild_pc;
        fun->state = CZL_IN_IDLE;
        gp->yeild_pc = NULL;
        czl_coroutine_paras_backup(fun, (czl_var*)fun->backup_vars);
        if (fun->cur_ins)
            fun->cur_ins = NULL;
    }
    else
    {
        if (fun->type != CZL_SYS_FUN || 1 == ret)
            gp->yeild_end = 1;
        if (fun->enter_vars_count >= 0)
            czl_fun_local_vars_clean(gp, fun);
    }

    --gp->fun_deep;

    return (ret ? &fun->ret : NULL);
}

static czl_var* czl_class_fun_run(czl_gp *gp, czl_exp_fun *exp_fun, void **ins)
{
	czl_var *ret;
    void **ins_backup = gp->cur_ins;

    gp->cur_ins = ins;
    ret = czl_exp_fun_run(gp, exp_fun);
    gp->cur_ins = ins_backup;

    return ret;
}

#ifdef CZL_VC6
static
#else
inline
#endif
czl_var* czl_get_opr(czl_gp *gp, unsigned char kind, void *obj)
{
    switch (kind)
    {
    case CZL_USR_FUN:
        return NULL;
    case CZL_FUNCTION:
        return czl_exp_fun_run(gp, (czl_exp_fun*)obj);
    case CZL_MEMBER:
        return czl_get_member_res(gp, (czl_obj_member*)obj);
    case CZL_REG_VAR:
        return CZL_GRV((czl_var*)obj);
    default: //CZL_INS_VAR
        return (CZL_OBJ_REF == ((czl_ins_var*)obj)->var->type ?
                                CZL_GRV(((czl_ins_var*)obj)->var) :
                                ((czl_ins_var*)obj)->var);
    }
}
///////////////////////////////////////////////////////////////
void czl_eles_delete(czl_gp *gp, czl_ele *p)
{
    czl_ele *q;

    while (p)
    {
        q = p->next;
        switch (p->type)
        {
        case CZL_NEW:
            czl_new_sentence_delete(gp, (czl_new_sentence*)p->val.Obj);
            break;
        case CZL_ARRAY_LIST:
            czl_array_list_delete(gp, CZL_ARR_LIST(p->val.Obj));
            break;
        case CZL_TABLE_LIST:
            czl_table_list_delete(gp, CZL_TAB_LIST(p->val.Obj));
            break;
        default:
            break;
        }
        CZL_RT_FREE(gp, p, sizeof(czl_ele));
        p = q;
    }
}

czl_var* czl_ele_create(czl_gp *gp, char type, void *val)
{
	czl_ele *p;

    if ((CZL_NIL == type || CZL_OBJ_REF == type) && gp->eles_head)
    {
        if (type == gp->eles_head->type)
            return (czl_var*)gp->eles_head;
        else if (gp->eles_head->next &&
                 type == gp->eles_head->next->type)
            return (czl_var*)gp->eles_head->next;
    }

    if (!(p=(czl_ele*)CZL_RT_MALLOC(gp, sizeof(czl_ele))))
        return NULL;

    p->type = type;
    p->val.Obj = val;
    if (CZL_OBJ_REF == type)
    {
        p->quality = CZL_REF_ELE;
        p->ot = CZL_NULL;
    }
    else
    {
        p->quality = CZL_CONST_VAR;
        p->ot = CZL_NIL;
    }

    if (gp->eles_head &&
        (CZL_NIL == gp->eles_head->type ||
         CZL_OBJ_REF == gp->eles_head->type))
    {
        if (gp->eles_head->next &&
            (CZL_NIL == gp->eles_head->next->type ||
             CZL_OBJ_REF == gp->eles_head->next->type))
        {
            p->next = gp->eles_head->next->next;
            gp->eles_head->next->next = p;
        }
        else
        {
            p->next = gp->eles_head->next;
            gp->eles_head->next = p;
        }
    }
    else
    {
        p->next = gp->eles_head;
        gp->eles_head = p;
    }

    return (czl_var*)p;
}

czl_var* czl_const_create(czl_gp *gp, char type, const czl_value *val)
{
    char *key = (CZL_STRING == type ? CZL_STR(val->str.Obj)->str : (char*)val);

    czl_glo_var *p = (czl_glo_var*)czl_sys_hash_find(CZL_ENUM, type,
                                                     key, &gp->tmp->consts_hash);
    if (p)
    {
        if (CZL_STRING == type)
            CZL_SF(gp, val->str);
        return (czl_var*)p;
    }

    if (!(p=(czl_glo_var*)CZL_RT_MALLOC(gp, sizeof(czl_glo_var))))
    {
        if (CZL_STRING == type)
            CZL_SF(gp, val->str);
        return NULL;
    }

    p->name = NULL;
    p->optimizable = 1;
    p->quality = CZL_CONST_VAR;
    p->ot = type;
    p->type = type;
    p->val = *val;

    p->next = gp->consts_head;
    gp->consts_head = p;

    if (!czl_sys_hash_insert(gp, CZL_ENUM, p, &gp->tmp->consts_hash))
    {
        if (CZL_STRING == type)
            CZL_SF(gp, val->str);
        CZL_RT_FREE(gp, p, sizeof(czl_glo_var));
        return NULL;
    }

    return (czl_var*)p;
}

char czl_is_tmp_unary_opt(char macro)
{
    switch (macro)
    {
    case CZL_NUMBER_NOT: case CZL_LOGIC_NOT:
    case CZL_LOGIC_FLIP: case CZL_REF_VAR:
    case CZL_SELF_ADD: case CZL_SELF_DEC:
    case CZL_OBJ_CNT:
        return 1;
    default:
        return 0;
    }
}

char czl_is_obj_unary_opt(czl_exp_node *p)
{
    while (p)
    {
        switch (p->type)
        {
        case CZL_REF_VAR:
        case CZL_ADD_SELF: case CZL_DEC_SELF:
        case CZL_SELF_ADD: case CZL_SELF_DEC:
            return 1;
        default:
            break;
        }
        p = p->left;
    }

    return 0;
}

char czl_is_obj_binary_opt(char macro)
{
    switch (macro)
    {
    case CZL_SWAP: case CZL_ASS:
    case CZL_ADD_A: case CZL_DEC_A:
    case CZL_MUL_A: case CZL_DIV_A: case CZL_MOD_A:
    case CZL_OR_A: case CZL_XOR_A: case CZL_AND_A:
    case CZL_L_SHIFT_A: case CZL_R_SHIFT_A:
        return 1;
    default:
        return 0;
    }
}

char czl_exp_unary_const_merge(czl_gp *gp, czl_exp_node *node)
{
    czl_var *var = (czl_var*)node->right->op.obj;
    unsigned long cnt = 0;
    czl_exp_node *p;
    czl_var res;

    for (p = node; p; p = p->left, cnt++)
    {
        if (p == node)
        {
            if (!CZL_2POCF(gp, p->type-CZL_NUMBER_NOT, &res, var))
                return 0;
        }
        else
        {
            if (!CZL_2POCF(gp, p->type-CZL_NUMBER_NOT, &res, &res))
                return 0;
        }
    }

    if (!(var=czl_const_create(gp, res.type, &res.val)))
        return 0;
    node->left = node->right = NULL;
    node->flag = CZL_OPERAND;
    node->type = CZL_ENUM;
    node->op.obj = var;

    return 1;
}

char czl_exp_binary_const_merge(czl_gp *gp, czl_exp_node *node)
{
	czl_var *var;
    czl_var *left = (czl_var*)node->left->op.obj;
    czl_var *right = (czl_var*)node->right->op.obj;
    czl_var res;

    if (CZL_ARRAY_LIST == left->type)
        czl_set_array_list_reg(gp, CZL_ARR_LIST(left->val.Obj));
    else if (CZL_TABLE_LIST == left->type)
        czl_set_table_list_reg(gp, CZL_TAB_LIST(left->val.Obj));
    if (CZL_ARRAY_LIST == right->type)
        czl_set_array_list_reg(gp, CZL_ARR_LIST(right->val.Obj));
    else if (CZL_TABLE_LIST == right->type)
        czl_set_table_list_reg(gp, CZL_TAB_LIST(right->val.Obj));

    switch (node->type)
    {
    case CZL_OR_OR:
        if (CZL_EIT(left))
        {
            res.type = CZL_INT;
            res.val.inum = 1;
        }
        else if (!CZL_2POCF(gp, CZL_OR_OR-CZL_NUMBER_NOT, &res, right))
            return 0;
        break;
    case CZL_AND_AND:
        if (!CZL_EIT(left))
        {
            res.type = CZL_INT;
            res.val.inum = 0;
        }
        else if (!CZL_2POCF(gp, CZL_AND_AND-CZL_NUMBER_NOT, &res, right))
            return 0;
        break;
    default:
        res.type = left->type;
        res.val = left->val;
        res.quality = CZL_ARRLINK_VAR;
        if (node->type >= CZL_ADD)
            node->type = CZL_ADD_A + (node->type - CZL_ADD);
        node->type -= CZL_NUMBER_NOT;
        if (!CZL_2POCF(gp, node->type, &res, right))
            return 0;
        break;
    }

    if (!(var=czl_const_create(gp, res.type, &res.val)))
        return 0;
    node->left = node->right = NULL;
    node->flag = CZL_OPERAND;
    node->type = CZL_ENUM;
    node->op.obj = var;

    return 1;
}

char* czl_get_unary_opt_name(unsigned char macro)
{
	unsigned long i;

    if (CZL_SELF_ADD == macro)
        macro = CZL_ADD_SELF;
    else if (CZL_SELF_DEC == macro)
        macro = CZL_DEC_SELF;

    for (i = 0; i < czl_unary_opt_table_num; i++)
        if (macro == czl_unary_opt_table[i].macro)
            return czl_unary_opt_table[i].name;
    return NULL;
}

char* czl_get_binary_opt_name(unsigned char macro)
{
    unsigned long i;
    for (i = 0; i < czl_binary_opt_table_num; i++)
        if (macro == czl_binary_opt_table[i].macro)
            return czl_binary_opt_table[i].name;
    return NULL;
}

char czl_exp_const_merge(czl_gp *gp, czl_exp_node *node)
{
    if (!node) return 1;

    if (CZL_UNARY_OPT == node->flag && node->parent &&
        CZL_UNARY_OPT == node->parent->flag &&
        node == node->parent->left)
        return 1;

    if (!czl_exp_const_merge(gp, node->left))
        return 0;

    if (CZL_OPERAND == node->flag)
        return 1;

    if (CZL_UNARY_OPT == node->flag)
    {
        if (CZL_OPERAND == node->right->flag)
        {
            //1++
            if (czl_is_obj_unary_opt(node) && CZL_IS_NOT_VAR(node->right))
            {
                sprintf(gp->log_buf, "%s should be used with a variable, ",
                        czl_get_unary_opt_name(node->type));
                return 0;
            }
            if (CZL_REF_VAR == node->type)
            {
                if ((!node->parent || node->parent->flag != CZL_BINARY_OPT ||
                     node->parent->type != CZL_ASS))
                {
                    sprintf(gp->log_buf, "& should be used with =, ");
                    return 0;
                }
                if (CZL_LG_VAR == node->parent->left->type)
                    ((czl_loc_var*)node->parent->left->op.obj)->optimizable = 0;
            }
        }
        else
        {
            if (!czl_exp_const_merge(gp, node->right))
                return 0;
            //1++
            if (czl_is_obj_unary_opt(node) &&
                (CZL_BINARY_OPT == node->right->flag ||
                 CZL_IS_NOT_VAR(node->right->right)))
            {
                sprintf(gp->log_buf, "%s should be used with variable, ",
                        czl_get_unary_opt_name(node->type));
                return 0;
            }
        }
        //-2
        if (CZL_IS_CONST(node->right))
            return czl_exp_unary_const_merge(gp, node);
    }
    else //CZL_BINARY_OPT
    {
        //1+=
        if (czl_is_obj_binary_opt(node->type))
        {
            if (CZL_IS_NOT_VAR(node->left) ||
                (CZL_SWAP == node->type && CZL_IS_NOT_VAR(node->right)))
            {
                sprintf(gp->log_buf, "%s should be used with variable, ",
                        czl_get_binary_opt_name(node->type));
                return 0;
            }
            if (CZL_UNARY_OPT == node->right->flag &&
                CZL_REF_VAR == node->right->type &&
                node->left->type != CZL_LG_VAR &&
                node->left->type != CZL_INS_VAR)
            {
                sprintf(gp->log_buf, "object member should not be reference for %s, ",
                        czl_get_unary_opt_name(node->right->type));
                return 0;
            }
        }
        if (!czl_exp_const_merge(gp, node->right))
            return 0;
        //三目运算符 ?: 直接返回
        if (CZL_QUE == node->type || CZL_COL == node->type)
            return 1;
        //1+2
        if (CZL_IS_CONST(node->left) && CZL_IS_CONST(node->right))
            return czl_exp_binary_const_merge(gp, node);
    }

    return 1;
}

//表达式树完整性检查
char czl_exp_integrity_check(czl_gp *gp, czl_exp_handle *h)
{
    //允许表达式为空
    if (!h->root)
        return 1;

    //检查运算符是否缺少操作数
    if (CZL_OPERAND == h->cur->flag || CZL_BRACKET_YES == h->cur->bracket)
    {

    }
    else if (CZL_UNARY_OPT == h->cur->flag && !h->cur->right)
    {
        sprintf(gp->log_buf, "missed operand after unary operator %s, ",
                czl_get_unary_opt_name(h->cur->type));
        return 0;
    }
    else if (CZL_BINARY_OPT == h->cur->flag)
    {
        if (!h->cur->right)
        {
            sprintf(gp->log_buf, "missed operand after binary operator %s, ",
                    czl_get_binary_opt_name(h->cur->type));
            return 0;
        }
        else if (!h->cur->right->right && h->cur->right->flag != CZL_OPERAND)
        {
            sprintf(gp->log_buf, "missed operand after unary operator %s, ",
                    czl_get_unary_opt_name(h->cur->right->type));
            return 0;
        }
    }

    if (h->mtoc)
    {
        sprintf(gp->log_buf, "missed ':' for '?', ");
        return 0;
    }

    //nil/null 必须配合 = 使用
    if (CZL_OPERAND == h->root->flag &&
        (CZL_NIL == h->root->type || CZL_OBJ_REF == h->root->type))
    {
        sprintf(gp->log_buf, "nil/null should be used with '=' , ");
        return 0;
    }

    //检查如++i的i是否是变量，合并如1+2可简化为3
    return czl_exp_const_merge(gp, h->root);
}

char czl_exp_is_integrity(czl_gp *gp, czl_exp_handle *h, char sign)
{
    if (!h->root)
        return (gp->tmp->condition_flag && '{' == sign) ? 1 : 0;

    //检查运算符是否缺少操作数
    if (CZL_OPERAND == h->cur->flag || CZL_BRACKET_YES == h->cur->bracket)
    {

    }
    else if (CZL_UNARY_OPT == h->cur->flag && !h->cur->right)
    {
        return 0;
    }
    else if (CZL_BINARY_OPT == h->cur->flag)
    {
        if (!h->cur->right)
            return 0;
        else if (!h->cur->right->right && h->cur->right->flag != CZL_OPERAND)
            return 0;
    }

    if (h->mtoc)
        return 0;

    return 1;
}
///////////////////////////////////////////////////////////////
//创建表达式树节点
czl_exp_node* czl_exp_node_create(czl_gp *gp)
{
	czl_exp_node *p;

    if (gp->tmp->exp_node_cnt >= CZL_EXP_NODE_MAX_SIZE)
        return NULL;

    p = gp->tmp->exp_node_pool + gp->tmp->exp_node_cnt++;

    p->bracket = CZL_BRACKET_NO;
    p->left = p->right = p->parent = NULL;
    //
    p->err_line = gp->error_line;
    p->err_file = gp->error_file;

    return p;
}

//创建操作数节点
czl_exp_node* czl_opr_node_create(czl_gp *gp, char type, void *val)
{
    czl_exp_node *node = czl_exp_node_create(gp);
    if (!node)
        return NULL;

    switch (type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_STRING:
        if (!(val=czl_const_create(gp, type, (czl_value*)val)))
            return NULL;
        type = CZL_ENUM;
        break;
    case CZL_FUN_REF:
    case CZL_NEW: case CZL_NIL: case CZL_OBJ_REF:
    case CZL_ARRAY_LIST: case CZL_TABLE_LIST:
        if (!(val=czl_ele_create(gp, type, val)))
            return NULL;
        break;
    default:
        break;
    }

    node->flag = CZL_OPERAND;
    node->type = type;
    node->op.obj = val;

    return node;
}

//创建单目运算符节点
czl_exp_node* czl_unary_opt_node_create
(
    czl_gp *gp,
    char macro,
    char position
)
{
    czl_exp_node *node = czl_exp_node_create(gp);
    if (!node)
        return NULL;

    node->flag = CZL_UNARY_OPT;
    node->type = macro;
    node->op.opt.position = position;

    return node;
}

//创建双目运算符节点
czl_exp_node* czl_binary_opt_node_create
(
    czl_gp *gp,
    char macro,
    char priority,
    char associativity
)
{
    czl_exp_node *node = czl_exp_node_create(gp);
    if (!node)
        return NULL;

    node->flag = CZL_BINARY_OPT;
    node->type = macro;
    node->op.opt.priority = priority;
    node->op.opt.associativity = associativity;

    return node;
}

//比较双目运算符优先级根据结合性找到新双目运算符插入节点位置
czl_exp_node* czl_binary_opt_node_insert_find
(
    char priority,
    czl_exp_node *cur
)
{
    czl_exp_node *p;

    //第一个运算符，直接作为根节点即可
    if (!cur->left)
        return NULL;

    //比较运算符优先级根据结合性找到新运算符插入位置
    for (p = cur; p; p = p->parent)
    {
        //如果是操作数或单目运算符或括号子树，直接跳过
        if (CZL_OPERAND == p->flag ||
                CZL_UNARY_OPT == p->flag ||
                CZL_BRACKET_YES == p->bracket)
            continue;
        //先按照LEFT_TO_RIGHT结合性找到插入节点
        if (priority >= p->op.opt.priority)
            break;
    }

    //如果p为NULL，则说明当前运算符优先级最低，直接作为根节点即可

    return p;
}

//三目运算符匹配检查
char czl_three_opt_match
(
    czl_gp *gp,
    czl_exp_node *node,
    czl_exp_handle *h
)
{
    czl_exp_node *p;

    for (p = h->cur; p; p = p->parent)
    {
        //如果是操作数或单目运算符或括号子树，直接跳过
        if (CZL_OPERAND == p->flag ||
                CZL_UNARY_OPT == p->flag ||
                CZL_BRACKET_YES == p->bracket)
            continue;
        if (CZL_COL == p->type)
            p = p->parent;
        else if (CZL_QUE == p->type)
            break;
    }

    if (!p)
        return 0;

    node->left = p->right;
    p->right->parent = node;
    p->right = node;
    node->parent = p;

    h->cur = node;
    h->mtoc--;
    gp->tmp->colon_flag--;

    return 1;
}

//插入双目运算符节点
char czl_binary_opt_node_insert
(
    czl_gp *gp,
    czl_exp_node *node,
    czl_exp_handle *h
)
{
	czl_exp_node *insert;

    //因为三目运算符?:通过两个伪双目运算符构造，所以这里需要特殊处理?:
    if (CZL_QUE == node->type)
    {
        h->mtoc++;
        gp->tmp->colon_flag++;
    }
    else if (CZL_COL == node->type)
        return czl_three_opt_match(gp, node, h);

    insert = czl_binary_opt_node_insert_find(node->op.opt.priority, h->cur);
    //insert == NULL 表示当前节点为第一个运算符节点或当前优先级最低节点，
    //把他直接作为根节点即可
    if (!insert)
    {
        node->left = h->cur;
        h->cur->parent = node;
        h->root = node;
    }
    else
    {
        if (node->op.opt.priority > insert->op.opt.priority)
        {
            node->left = insert->right;
            insert->right->parent = node;
            insert->right = node;
            node->parent = insert;
        }
        else //node->op.opt.priority == insert->op.opt.priority
        {
            //优先级相等看结合性，如果结合性为RIGHT_TO_LEFT，
            //则insert的右孩子是插入位置。
            if (CZL_RIGHT_TO_LEFT == node->op.opt.associativity)
            {
                node->left = insert->right;
                insert->right->parent = node;
                insert->right = node;
                node->parent = insert;
            }
            else
            {
                if (!insert->parent)
                {
                    node->left = insert;
                    insert->parent = node;
                    h->root = node;
                }
                else
                {
                    insert->parent->right = node;
                    node->parent = insert->parent;
                    node->left = insert;
                    insert->parent = node;
                }
            }
        }
    }

    //exp_current永远指向当前操作符节点
    h->cur = node;

    return 1;
}

//在已有至少一个单目运算符子树的左孩子链域插入一个新的单目运算符
void czl_unary_opt_node_insert
(
    czl_exp_node *insert,
    czl_exp_node *node,
    czl_exp_handle *h
)
{
    czl_exp_node *p, *q = NULL;

    if (CZL_POS_RIGHT == node->op.opt.position)
    {
        for (p = insert; p; p = p->left)
            q = p;
        q->left = node;
        node->parent = q;
    }
    else
    {
        node->left = insert;
        insert->parent = node;
        if (insert->right)
        {
            node->right = insert->right;
            insert->right->parent = node;
            insert->right = NULL;
        }
        if (insert == h->cur)
        {
            h->root = node;
            h->cur = node;
        }
        else
        {
            h->cur->right = node;
            node->parent = h->cur;
        }
    }
}

//单目运算符转双目运算符
void czl_opt_unary_to_binary(czl_exp_node *node, char macro)
{
    unsigned long i;
    czl_exp_op *op = &node->op;

    for (i = 0; i < czl_binary_opt_table_num; i++)
    {
        if (czl_binary_opt_table[i].macro == (char)macro)
        {
            node->flag = CZL_BINARY_OPT;
            node->type = czl_binary_opt_table[i].macro;
            op->opt.priority = czl_binary_opt_table[i].priority;
            op->opt.associativity = czl_binary_opt_table[i].associativity;
            break;
        }
    }
}

//运算符冲突检测纠正处理
void czl_opt_collision_detect
(
    czl_exp_node *node,
    czl_exp_node *cur
)
{
    if (CZL_OPERAND == node->flag || CZL_BRACKET_YES == node->bracket)
        return;

    //1. 左单目运算符与双目运算符同名，这里需要做冲突检测处理
    //2. 因为 ++ -- 默认为左结合性，这里需要检查是否需要纠正为右结合性

    if ((CZL_OPERAND == cur->flag || CZL_BRACKET_YES == cur->bracket) ||
        (CZL_UNARY_OPT == cur->flag && cur->right) ||
        (CZL_BINARY_OPT == cur->flag && cur->right &&
         (CZL_OPERAND == cur->right->flag || cur->right->right)))
    {
        switch (node->type)
        {
        case CZL_NUMBER_NOT: //NUMBER_NOT 转 DEC
            czl_opt_unary_to_binary(node, CZL_DEC);
            break;
        case CZL_ADD_SELF: //ADD_SELF 转 SELF_ADD
            node->type = CZL_SELF_ADD;
            break;
        case CZL_DEC_SELF: //DEC_SELF 转 SELF_DEC
            node->type = CZL_SELF_DEC;
            break;
        default:
            break;
        }
    }
}

//插入一个表达式树节点
char czl_exp_node_insert
(
    czl_gp *gp,
    czl_exp_node *node,
    czl_exp_handle *h
)
{
    if (!node)
        return 0;

    //插入第一个操作数节点
    if (!h->root)
    {
        if (node->flag != CZL_OPERAND)
        {
            //语法错误: 第一个节点不能是双目运算符
            if (CZL_BINARY_OPT == node->flag)
                goto CZL_SYNTAX_ERROR;
            //语法错误: 第一个节点不能是右位置性的单目运算符
            if (CZL_POS_RIGHT == node->op.opt.position)
                goto CZL_SYNTAX_ERROR;
        }
        h->root = node;
        h->cur = node;
        return 1;
    }

    //单目运算符与双目运算符同名冲突检测处理，左右单目运算符宏值纠正检测处理
    czl_opt_collision_detect(node, h->cur);

    //语法根据exp_current与node的3*3=9种组合进行分析，各分别是:
    //操作数(圆括号表达式)、单目运算符、双目运算符3种情况

    //操作数(圆括号表达式)
    if (CZL_OPERAND == h->cur->flag || CZL_BRACKET_YES == h->cur->bracket)
    {
        //语法错误: 两个操作数间没有双目运算符
        if (CZL_OPERAND == node->flag || CZL_BRACKET_YES == node->bracket)
            goto CZL_SYNTAX_ERROR;
        else if (CZL_UNARY_OPT == node->flag)
        {
            //语法错误: 单目左运算符不能放在操作数右边
            if (CZL_POS_LEFT == node->op.opt.position)
                goto CZL_SYNTAX_ERROR;
            //插入单目右运算符
            node->right = h->cur;
            h->cur->parent = node;
            h->root = node;
            h->cur = node;
        }
        else //CZL_BINARY_OPT
        {
            //插入双目运算符
            if (!czl_binary_opt_node_insert(gp, node, h))
                goto CZL_SYNTAX_ERROR;
        }
    }
    //单目运算符
    else if (CZL_UNARY_OPT == h->cur->flag)
    {
        if (CZL_OPERAND == node->flag || CZL_BRACKET_YES == node->bracket)
        {
            //语法错误: 两个操作数间没有双目运算符
            if (h->cur->right)
                goto CZL_SYNTAX_ERROR;
            //插入单目运算符操作数
            h->cur->right = node;
            node->parent = h->cur;
        }
        else if (CZL_UNARY_OPT == node->flag)
        {
            //语法错误: 单目右运算符缺少操作数
            if (!h->cur->right && CZL_POS_RIGHT == node->op.opt.position)
                goto CZL_SYNTAX_ERROR;
            //语法错误: 单目左运算符不能放在操作数右边
            else if (h->cur->right && CZL_POS_LEFT == node->op.opt.position)
                goto CZL_SYNTAX_ERROR;
            //插入单目左运算符
            czl_unary_opt_node_insert(h->cur, node, h);
        }
        else //CZL_BINARY_OPT
        {
            //语法错误: 单目运算符缺少操作数
            if (!h->cur->right)
                goto CZL_SYNTAX_ERROR;
            //插入双目运算符
            if (!czl_binary_opt_node_insert(gp, node, h))
                goto CZL_SYNTAX_ERROR;
        }
    }
    //双目运算符
    else //CZL_BINARY_OPT
    {
        if (CZL_OPERAND == node->flag || CZL_BRACKET_YES == node->bracket)
        {
            if (h->cur->right)
            {
                //语法错误: 双目运算符已经存在右操作数
                if (CZL_OPERAND == h->cur->right->flag ||
                    CZL_BRACKET_YES == h->cur->right->bracket)
                    goto CZL_SYNTAX_ERROR;
                else if (CZL_UNARY_OPT == h->cur->right->flag)
                {
                    //插入单目运算符操作数
                    h->cur->right->right = node;
                    node->parent = h->cur->right->right;
                }
            }
            else
            {
                //插入双目运算符右操作数
                h->cur->right = node;
                node->parent = h->cur;
            }
        }
        else if (CZL_UNARY_OPT == node->flag)
        {
            if (h->cur->right)
            {
                if (CZL_OPERAND == h->cur->right->flag ||
                    CZL_BRACKET_YES == h->cur->right->bracket)
                {
                    //语法错误: 单目左运算符不能放在操作数右边
                    if (CZL_POS_LEFT == node->op.opt.position)
                        goto CZL_SYNTAX_ERROR;
                    //插入单目左运算符
                    node->right = h->cur->right;
                    h->cur->right->parent = node;
                    h->cur->right = node;
                    node->parent = h->cur;
                }
                else if (CZL_UNARY_OPT == h->cur->right->flag)
                {
                    //语法错误: 单目右运算符缺少操作数
                    if (!h->cur->right->right && CZL_POS_RIGHT == node->op.opt.position)
                        goto CZL_SYNTAX_ERROR;
                    //语法错误: 单目左运算符不能放在操作数右边
                    else if (h->cur->right->right && CZL_POS_LEFT == node->op.opt.position)
                        goto CZL_SYNTAX_ERROR;
                    //插入单目左运算符
                    czl_unary_opt_node_insert(h->cur->right, node, h);
                }
            }
            else
            {
                //语法错误: 单目右运算符缺少操作数
                if (CZL_POS_RIGHT == node->op.opt.position)
                    goto CZL_SYNTAX_ERROR;
                //插入单目左运算符
                h->cur->right = node;
                node->parent = h->cur;
            }
        }
        else //CZL_BINARY_OPT
        {
            if (h->cur->right)
            {
                //单目运算符缺少操作数
                if (h->cur->right->flag != CZL_OPERAND && !h->cur->right->right)
                    goto CZL_SYNTAX_ERROR;
                //插入双目运算符
                if (!czl_binary_opt_node_insert(gp, node, h))
                    goto CZL_SYNTAX_ERROR;
            }
            else
            {
                //双目运算符缺少右操作数
                goto CZL_SYNTAX_ERROR;
            }
        }
    }

    //标记临时结果双目运算符，用于一条语句中子表达式的寄存器起始分配地址确定，
    //如： a + b[i+1]; 中 i+1 就是子表达式，它的起始地址应该在 a+ 后面，
    //如： a[i+1] = j+1; 中 i+1 不能和 j+1共用一个寄存器，
    //这里针对上面两种情况做处理，否则会导致寄存器共用临时结果数据被覆盖。
    if (0 == gp->tmp->reg_flag && CZL_BINARY_OPT == node->flag)
    {
        if (czl_is_obj_binary_opt(node->type))
        {
            if (CZL_OPERAND == node->left->flag && CZL_MEMBER == node->left->type)
                gp->tmp->reg_sign = 1;
        }
        else if (node->type != CZL_QUE && node->type != CZL_COL)
        {
            gp->tmp->reg_flag = 1;
            gp->reg_cnt += 2;
            //这里不做加1还是加2判断，统一加2保证正确性，节省大量的代码成本。
            //经过深入分析，任意复杂的一条表达式最多只需要2个寄存器保存临时结果，
            //因为指令最大地址是二地址指令。
        }
    }

    return 1;

CZL_SYNTAX_ERROR:
    return 0;
}

char czl_val_del(czl_gp *gp, czl_var *var)
{
    if (CZL_OBJ_IS_LOCK(var))
    {
        gp->exceptionCode = CZL_EXCEPTION_OBJECT_LOCK;
        return 0;
    }

    switch (var->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_FUN_REF:
        break;
    case CZL_OBJ_REF:
        czl_ref_break(gp, (czl_ref_var*)var->name, CZL_GRV(var));
        CZL_REF_FREE(gp, var->name);
        var->name = NULL;
        break;
    case CZL_STRING:
        CZL_SRCD1(gp, var->val.str);
        break;
    case CZL_FILE:
        if (0 == CZL_ORCD1(var->val.Obj))
            czl_file_delete(gp, var->val.Obj);
        break;
    case CZL_INSTANCE:
        if (0 == CZL_ORCD1(var->val.Obj))
            return czl_instance_delete(gp, var->val.Obj);
        break;
    case CZL_TABLE:
        if (0 == CZL_ORCD1(var->val.Obj))
            return czl_table_delete(gp, var->val.Obj);
        break;
    case CZL_ARRAY:
        if (0 == CZL_ORCD1(var->val.Obj))
            return czl_array_delete(gp, var->val.Obj);
        break;
    default: //CZL_STACK/CZL_QUEUE
        if (0 == CZL_ORCD1(var->val.Obj))
            return czl_sq_delete(gp, var->val.Obj);
        break;
    }

    return 1;
}

//释放表达式栈内存
void czl_exp_stack_delete(czl_gp *gp, czl_exp_ele *exp)
{
    CZL_RT_FREE(gp, exp, CZL_EL(exp)*sizeof(czl_exp_ele));
}

unsigned short czl_get_exp_tree_node_num
(
    czl_gp *gp,
    czl_exp_node *p,
    unsigned short sum
)
{
    if (!p) return sum;

    ++sum;
    if ((CZL_BINARY_OPT == p->flag &&
        (CZL_COL == p->type ||
         CZL_AND_AND == p->type ||
         CZL_OR_OR == p->type)))
    {
        ++sum;
    }

    sum = czl_get_exp_tree_node_num(gp, p->left, sum);
    sum = czl_get_exp_tree_node_num(gp, p->right, sum);

    return sum;
}

unsigned short czl_get_order_num
(
    czl_exp_node *p,
    unsigned short sum
)
{
    if (!p) return sum;

    if (p->flag != CZL_OPERAND)
    {
        ++sum;
        if ((CZL_BINARY_OPT == p->flag &&
            (CZL_COL == p->type ||
             CZL_AND_AND == p->type ||
             CZL_OR_OR == p->type)))
        {
            ++sum;
        }
    }

    sum = czl_get_order_num(p->left, sum);
    sum = czl_get_order_num(p->right, sum);

    return sum;
}

//树节点入栈
void czl_exp_stack_push
(
    czl_exp_node *node,
    czl_exp_ele *ele
)
{
    ele->flag = node->flag;
    ele->kind = node->type;
    ele->pl.msg.line = node->err_line;

    if (CZL_OPERAND == node->flag)
        ele->res = (czl_var*)node->op.obj;
}

//单目运算符集合入栈
void czl_unary_opt_push
(
    czl_exp_node *node,
    czl_exp_ele *head,
    int *index
)
{
    while (node)
    {
        //单目运算符入栈
        czl_exp_stack_push(node, head + (*index)++);
        node = node->left;
    }
}

//树转栈
void czl_tree_to_stack
(
    czl_exp_node *node,
    czl_exp_ele *head,
    int *index
)
{
    if (!node) return;

    if (CZL_UNARY_OPT == node->flag && node->parent &&
        CZL_UNARY_OPT == node->parent->flag &&
        node == node->parent->left) return;

    czl_tree_to_stack(node->left, head, index);

    if (CZL_OPERAND == node->flag)
    {
        //双目运算符操作数入栈
        czl_exp_stack_push(node, head + (*index)++);
    }
    else if (CZL_UNARY_OPT == node->flag)
    {
        //单目运算符操作数符入栈
        if (CZL_OPERAND == node->right->flag)
            czl_exp_stack_push(node->right, head + (*index)++);
        else
            czl_tree_to_stack(node->right, head, index);
        //单目运算符集合入栈
        czl_unary_opt_push(node, head, index);
    }
    else //BINARY_OPT
    {
        czl_exp_ele *ele;
        unsigned char lt, rt;
        switch (node->type)
        {
        case CZL_AND_AND: case CZL_OR_OR:
            ele = head + (*index)++;
            ele->flag = CZL_CONDITION;
            if (CZL_AND_AND == node->type) {
                ele->lo = NULL;
                ele->ro = (czl_var*)ele;
            }
            else {
                ele->lo = (czl_var*)ele;
                ele->ro = NULL;
            }
            ele->kind = czl_get_order_num(node->left, 0);
            if (!(ele->rt=czl_get_order_num(node->right, 0)))
                ele->rt = 1;
            czl_tree_to_stack(node->right, head, index);//右子树结果
            //双目运算符入栈
            czl_exp_stack_push(node, head + (*index)++);
            break;
        case CZL_QUE:
            ele = head + (*index)++;
            node->flag = CZL_THREE_OPT;
            czl_exp_stack_push(node, ele);
            //
            ele->kind = czl_get_order_num(node->left, 0);
            node = node->right;
            lt = czl_get_order_num(node->left, 0);
            ele->lt = lt + 2;
            rt = czl_get_order_num(node->right, 0);
            ele->rt = rt + ele->lt + 1;
            //
            czl_tree_to_stack(node->left, head, index);
            ele = head + (*index)++;
            ele->flag = CZL_THREE_END;
            ele->kind = lt;
            //
            czl_tree_to_stack(node->right, head, index);
            ele = head + (*index)++;
            ele->flag = CZL_THREE_END;
            ele->kind = rt;
            break;
        default:
            czl_tree_to_stack(node->right, head, index);//右子树结果
            //双目运算符入栈
            czl_exp_stack_push(node, head + (*index)++);
            break;
        }
    }
}

void czl_set_special_opt_opr
(
    unsigned char flag,
    czl_exp_ele *pc,
    czl_exp_ele *opr
)
{
    if (flag)
    {
        if (CZL_THREE_END == pc->flag)
            pc->lo = NULL;
    }
    else
    {
        pc->pl = opr->pl;
        pc->lt = pc->flag;
        pc->flag = CZL_OPERAND;
        pc->kind = opr->kind;
        pc->res = opr->res;
    }
}

static czl_var* czl_get_reg(czl_gp *gp, unsigned long reg_cnt)
{
    unsigned long cnt = gp->reg_cnt + gp->tmp->reg_sign;

    if (CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field ||
        CZL_IN_CLASS_FUN == gp->tmp->analysis_field)
    {
        if (cnt + reg_cnt + 1 > gp->cur_fun->reg_cnt)
        {
            gp->cur_fun->reg_cnt = cnt + reg_cnt + 1;
            if (gp->cur_fun->reg_cnt > CZL_MAX_REG_CNT)
            {
                sprintf(gp->log_buf, "layer number of expression should be less %d, ", CZL_MAX_REG_CNT/2);
                return NULL;
            }
        }
    }

    return gp->exp_reg + cnt + reg_cnt;
}

czl_var* czl_use_reg(czl_gp *gp, unsigned long *reg_cnt, czl_var *reg)
{
    if (*reg_cnt < 2)
        return czl_get_reg(gp, (*reg_cnt)++);
    else
        return gp->exp_reg + gp->reg_cnt + ((reg == gp->exp_reg+gp->reg_cnt) ? 1 : 0);
}

czl_exp_ele* czl_set_opt_opr
(
    czl_gp *gp,
    czl_exp_ele *pc,
    czl_exp_ele **res,
    unsigned long i,
    unsigned long *reg_cnt,
    czl_var *reg
)
{
    unsigned char lt;
    czl_exp_ele *l, *r;

    for (;;)
    {
        switch (pc->flag)
        {
        case CZL_OPERAND:
            pc->lt = CZL_OPERAND;
            res[i++] = pc++;
            break;
        case CZL_UNARY_OPT:
            l = res[i-1];
            pc->lt = l->kind;
            pc->lo = l->res;
            if (czl_is_tmp_unary_opt(pc->kind))
            {
                pc->rt = 0; //pc->rt 不为0 表示 fun(&v) 中 v 已经加&转换
                if (l->kind != CZL_REG_VAR)
                {
                    l->kind = CZL_REG_VAR;
                    //用寄存器存临时结果变量
                    if (!(l->res=czl_use_reg(gp, reg_cnt, reg)))
                        return NULL;
                }
                reg = pc->res = l->res;
                if (CZL_MEMBER == pc->lt && CZL_REF_VAR == pc->kind)
                    ((czl_obj_member*)pc->lo)->flag = CZL_REF_VAR;
                //
                if (CZL_LG_VAR == pc->lt && pc->kind != CZL_REF_VAR)
                {
                    if (CZL_INT == ((czl_loc_var*)pc->lo)->ot)
                        gp->tmp->int_reg_cnt = 2;
                    else if (CZL_FLOAT == ((czl_loc_var*)pc->lo)->ot)
                    {
                        if (CZL_LOGIC_NOT == pc->kind || CZL_LOGIC_FLIP == pc->kind)
                            gp->tmp->int_reg_cnt = 2;
                        else
                            gp->tmp->float_reg_cnt = 2;
                    }
                }
            }
            else
            {
                pc->res = NULL;
            }
            ++pc;
            break;
        case CZL_BINARY_OPT:
            l = res[i-2];
            r = res[i-1];
            if (czl_is_obj_binary_opt(pc->kind))
            {
                pc->lt = l->kind;
                pc->lo = l->res;
                pc->rt = r->kind;
                pc->ro = r->res;
                pc->res = NULL;
                if (CZL_MEMBER == pc->lt)
                    ((czl_obj_member*)pc->lo)->flag = pc->kind;
                if (CZL_MEMBER == pc->rt && CZL_SWAP == pc->kind)
                    ((czl_obj_member*)pc->ro)->flag = pc->kind;
            }
            else
            {
                //将 && || 转为单目运算符
                if (CZL_AND_AND == pc->kind || CZL_OR_OR == pc->kind)
                {
                    pc->flag = CZL_UNARY_OPT;
                    pc->lt = r->kind;
                    pc->lo = r->res;
                }
                else
                {
                    pc->lt = l->kind;
                    pc->lo = l->res;
                    pc->rt = r->kind;
                    pc->ro = r->res;
                    //
                    if ((CZL_LG_VAR == pc->lt || CZL_ENUM == pc->lt) &&
                        (CZL_LG_VAR == pc->rt || CZL_ENUM == pc->rt))
                    {
                        if (CZL_INT == ((czl_loc_var*)pc->lo)->ot)
                        {
                            if (CZL_INT == ((czl_loc_var*)pc->ro)->ot ||
                                CZL_FLOAT == ((czl_loc_var*)pc->ro)->ot)
                                gp->tmp->int_reg_cnt = 2;
                        }
                        else if (CZL_FLOAT == ((czl_loc_var*)pc->lo)->ot)
                        {
                            if (CZL_INT == ((czl_loc_var*)pc->ro)->ot ||
                                CZL_FLOAT == ((czl_loc_var*)pc->ro)->ot)
                            {
                                if (CZL_ADD == pc->kind || CZL_DEC == pc->kind ||
                                    CZL_MUL == pc->kind || CZL_DIV == pc->kind)
                                    gp->tmp->float_reg_cnt = 2;
                                else
                                    gp->tmp->int_reg_cnt = 2;
                            }
                        }
                    }
                }
                if (l->kind != CZL_REG_VAR)
                {
                    l->kind = CZL_REG_VAR;
                    //用寄存器存临时结果变量
                    if (!(l->res=czl_use_reg(gp, reg_cnt, reg)))
                        return NULL;
                }
                reg = pc->res = l->res;
            }
            --i;
            ++pc;
            break;
        case CZL_THREE_OPT:
            lt = pc->lt;
            czl_set_special_opt_opr(pc->kind, pc, res[i-1]);
            if (!(l=czl_set_opt_opr(gp, pc+1, res, i, reg_cnt, reg)))
                return NULL;
            if (!(r=czl_set_opt_opr(gp, l+1, res, i, reg_cnt, reg)))
                return NULL;
            if (0xFF == (r+1)->flag)
            {
                if (CZL_THREE_END == l->flag)
                    l->res = NULL;
                else //CZL_OPERAND
                    l->lo = NULL;
                if (CZL_THREE_END == r->flag)
                    r->res = NULL;
                else //CZL_OPERAND
                    r->lo = NULL;
            }
            else
            {
                if (res[i-1]->kind != CZL_REG_VAR)
                {
                    res[i-1]->kind = CZL_REG_VAR;
                    //用寄存器存临时结果变量
                    if (!(res[i-1]->res=czl_use_reg(gp, reg_cnt, reg)))
                        return NULL;
                }
                reg = res[i-1]->res;
                if (CZL_THREE_END == l->flag)
                    l->res = res[i-1]->res;
                else //CZL_OPERAND
                    l->lo = res[i-1]->res;
                if (CZL_THREE_END == r->flag)
                    r->res = res[i-1]->res;
                else //CZL_OPERAND
                    r->lo = res[i-1]->res;
            }
            l->rt = pc->rt - lt + 1;
            r->rt = 1;
            pc->rt = lt;
            pc = r+1;
            break;
        case CZL_CONDITION:
            czl_set_special_opt_opr(pc->kind, pc, res[i-1]);
            ++pc;
            break;
        case CZL_THREE_END:
            czl_set_special_opt_opr(pc->kind, pc, res[i-1]);
            return pc;
        default: //0XFF
            return pc;
        }
    }
}

czl_exp_ele* czl_stack_to_reg(czl_gp *gp, czl_exp_ele *pc)
{
    czl_exp_ele *res[CZL_EXP_NODE_MAX_SIZE];
    unsigned long reg_cnt = 0;
    czl_var *reg = NULL;
    return czl_set_opt_opr(gp, pc, res, 0, &reg_cnt, reg);
}

czl_exp_ele* czl_create_order
(
    czl_exp_ele *pc,
    czl_exp_ele *order,
    int *i
)
{
    for (;;)
    {
        switch (pc->flag)
        {
        case CZL_OPERAND:
            switch (pc->lt)
            {
            case CZL_OPERAND:
                ++pc;
                break;
            case CZL_CONDITION:
                order[(*i)++] = *(pc++);
                break;
            case CZL_THREE_OPT:
                order[(*i)++] = *(pc++);
                pc = czl_create_order(pc, order, i);
                pc = czl_create_order(pc, order, i);
                break;
            default: //CZL_THREE_END
                order[(*i)++] = *(pc++);
                return pc;
            }
            break;
        case CZL_BINARY_OPT: case CZL_UNARY_OPT: case CZL_CONDITION:
            order[(*i)++] = *(pc++);
            break;
        case CZL_THREE_OPT:
            order[(*i)++] = *(pc++);
            pc = czl_create_order(pc, order, i);
            pc = czl_create_order(pc, order, i);
            break;
        case CZL_THREE_END:
            order[(*i)++] = *(pc++);
            return pc;
        default: //0XFF
            return pc;
        }
    }
}

void czl_set_order_next(czl_exp_ele *order, unsigned long cnt)
{
    unsigned long i;
    for (i = 0; ; ++i)
    {
        if (i < cnt-1)
            order[i].next = order + i + 1;
        else
        {
            order[i].next = NULL;
            return;
        }
    }
}

//表达式树转表达式栈(波兰表达式)
char czl_exp_tree_to_stack(czl_gp *gp, czl_exp_node *exp_root)
{
	unsigned short num;

    if (!exp_root) return 1;

    num = czl_get_exp_tree_node_num(gp, exp_root, 0);

    if (num > CZL_EXP_NODE_MAX_SIZE)
    {
        sprintf(gp->log_buf, "node number of expression should be less %d, ", CZL_EXP_NODE_MAX_SIZE);
        return 0;
    }
    else if (1 == num)
    {
        gp->tmp->exp_head = czl_opr_create(gp, exp_root);
        return (gp->tmp->exp_head ? 1 : 0);
    }
    else
    {
        int index = 0;
        czl_exp_ele stack[CZL_EXP_NODE_MAX_SIZE];
        czl_tree_to_stack(exp_root, stack, &index);
        stack[num].flag = 0XFF;
        if (!czl_stack_to_reg(gp, stack))
            return 0;
        num = czl_get_order_num(exp_root, 0);
        gp->tmp->exp_head = (czl_exp_ele*)CZL_RT_CALLOC(gp, num, sizeof(czl_exp_ele));
        if (!gp->tmp->exp_head)
            return 0;
        index = 0;
        czl_create_order(stack, gp->tmp->exp_head, &index);
        czl_set_order_next(gp->tmp->exp_head, num);
        gp->tmp->exp_head->pl.msg.cnt = num;
        return 1;
    }
}

static char czl_callback_fun_run(czl_gp *gp, czl_fun *fun)
{
    czl_exp_fun exp_fun;
    exp_fun.fun = fun;
    exp_fun.paras = NULL;
    exp_fun.paras_count = 0;
    exp_fun.quality = CZL_STATIC_FUN;
    exp_fun.flag = 1;
    return czl_exp_fun_run(gp, &exp_fun) ? 1 : 0;
}

static void czl_runtime_error_report(czl_gp *gp, czl_exp_ele *err)
{
    if (0 == gp->exit_flag)
    {
        gp->exit_flag = 1;
        gp->error_file = NULL;
        gp->error_line = err->pl.msg.line;
        if (CZL_EXIT_ABNORMAL == gp->exit_code &&
            CZL_EXCEPTION_NO == gp->exceptionCode)
            gp->exceptionCode = CZL_EXCEPTION_ORDER_TYPE_NOT_MATCH;
        if (gp->exceptionFuns[gp->exceptionCode-1])
        {
            unsigned char exceptionCode = gp->exceptionCode;
            if (CZL_EXCEPTION_OUT_OF_MEMORY == exceptionCode)
            {
                if (CZL_MM_4GB == gp->mm_limit)
                {
                    gp->exceptionCode = CZL_EXCEPTION_DEAD;
                    return;
                }
                gp->mm_limit_backup = gp->mm_limit;
                gp->mm_limit = CZL_MM_4GB;
            }
            czl_callback_fun_run(gp, gp->exceptionFuns[exceptionCode-1]);
            if (CZL_MM_4GB == gp->mm_limit &&
                CZL_EXCEPTION_OUT_OF_MEMORY == exceptionCode)
                gp->mm_limit = gp->mm_limit_backup;
        }
    }
}

static void czl_fun_ret_check_free(czl_gp *gp, czl_var *res, czl_var *lo, czl_var *ro)
{
    if (res && CZL_FUNRET_VAR == res->quality)
    {
        czl_val_del(gp, res);
        res->type = CZL_INT;
    }
    if (lo && CZL_FUNRET_VAR == lo->quality)
    {
        czl_val_del(gp, lo);
        lo->type = CZL_INT;
    }
    if (ro && CZL_FUNRET_VAR == ro->quality)
    {
        czl_val_del(gp, ro);
        ro->type = CZL_INT;
    }
}

//表达式指令栈计算函数
static czl_var* czl_exp_stack_cac(czl_gp *gp, czl_exp_stack pc)
{
    czl_var *lo = NULL, *ro = NULL;

    if (CZL_EIO(pc))
    {
        if (CZL_REG_VAR == pc->kind && pc->res->type != CZL_OBJ_REF)
            return pc->res;
        if (!(lo=czl_get_opr(gp, pc->kind, pc->res)))
            goto CZL_EXCEPTION_CATCH;
        return lo;
    }

    do {
        switch (pc->flag)
        {
        case CZL_BINARY_OPT:
            CZL_RBO(gp, ro, pc);
            break;
        case CZL_BINARY2_OPT:
            CZL_RB2O(gp, lo, ro, pc);
            break;
        case CZL_ASS_OPT:
            CZL_RAO(gp, ro, pc);
            break;
        case CZL_UNARY_OPT:
            CZL_RUO(gp, pc);
            break;
        case CZL_UNARY2_OPT:
            CZL_RU2O(gp, pc);
            break;
        case CZL_AND_AND:
            CZL_AA_CJ(gp, (pc-1)->res, pc);
            break;
        case CZL_OR_OR:
            CZL_OO_CJ(gp, (pc-1)->res, pc);
            break;
        case CZL_OPERAND:
            CZL_GOR(gp, pc);
            CZL_COT(gp, pc);
            break;
        case CZL_THREE_OPT:
            CZL_RTO((pc-1)->res, pc);
            break;
        default: //CZL_THREE_END
            CZL_TORM(gp, pc->lo, (pc-1)->res, pc);
            break;
        }
        if (ro)
            CZL_CF_FR(gp, ro);
    } while ((pc-1)->next);

    return (pc-1)->res;

CZL_EXCEPTION_CATCH: CZL_RUN_FUN:
    czl_fun_ret_check_free(gp, pc->res, lo, ro);
    czl_runtime_error_report(gp, pc);
    return NULL;
}

czl_var* czl_exp_cac(czl_gp *gp, czl_exp_stack exp)
{
    czl_var *ret;

    if (gp->fun_ret)
    {
        czl_val_del(gp, gp->fun_ret);
        gp->fun_ret->type = CZL_INT;
        gp->fun_ret = NULL;
    }

    if (!(ret=czl_exp_stack_cac(gp, exp)))
        return NULL;

    //把函数返回值标记下来，方便实时回收其内存
    if (CZL_FUNRET_VAR == ret->quality)
        gp->fun_ret = ret;

    return ret;
}
///////////////////////////////////////////////////////////////
//构造一个10位的随机数作为哈希表的密钥
unsigned long czl_hash_key_create
(
    unsigned long seed,
    unsigned long old_key
)
{
	unsigned long i, key;
CZL_AGAIN:
    srand(seed);

	i = key = 0;
    do {
        key = key*10 + rand()%10;
    } while (++i < 10);

    if (key == old_key)
    {
        ++seed;
        goto CZL_AGAIN;
    }

    return key;
}

//数值哈希函数
unsigned long czl_num_hash
(
    czl_long num,
    unsigned long key,
    unsigned long attack_cnt
)
{
    if (0 == attack_cnt)
    {
    #ifdef CZL_SYSTEM_64bit
        if (num <= 4294967295UL)
            return num;
        //以下算法能解决64位数据顺序插入时全部落入固定槽问题
        return (num>>32)^(unsigned long)num;
    #else
        return num;
    #endif
    }
    else
    {
        //经过大量构造数据测试，以下算法在发生哈希碰撞攻击时，
        //通过修改key能很好地将碰撞数据离散化，该算法同样适用于字符串。
        //离散化程度达到平均链长1~4占超过90%，最大链长在15以内。
		long hash = 0;
        if (num < 0) num = -num;
        do {
            hash = (hash * 131) + ((num>>=1) * (key>>=1));
        } while (num);
        return hash;
    }
}

//字符串哈希函数
unsigned long czl_str_hash
(
    char *str,
    unsigned long len,
    unsigned long key,
    unsigned long attack_cnt
)
{
    long hash = 0;
    unsigned long i = 0;

    if (0 == attack_cnt)
    {
        while (i++ < len)
           hash = (hash * 131) + *str++;
    }
    else
    {
        unsigned long tmp = key;
        while (i++ < len)
        {
           hash = (hash * 131) + (*str++ * (key>>=1));
           if (0 == key)
               key = tmp;
        }
    }

    return hash;
}

//字符串哈希函数，实测对比众多hash算法， bkdr 是最高效的。
unsigned long czl_bkdr_hash
(
    char *str,
    unsigned long len
)
{
    long hash = 0;
    unsigned long i = 0;

    while (i++ < len)
       hash = (hash * 131) + *str++;

    return hash;
}
///////////////////////////////////////////////////////////////
static long czl_hash_index_get
(
    char type,
    void *kv,
    long mask
)
{
    long index; //必须有符号long型

    switch (type)
    {
    case CZL_INT: //用于协成、线程ID
        return CZL_INDEX_CAC(index, (unsigned long)kv, mask);
    case CZL_STRING: //用于变量、函数、类名、系统关键字等
        //只要是建有哈希表的存储结构，哈希对象地址必须放在结构的首地址，
        //比如czl_var/czl_fun/czl_class/czl_sys_fun/czl_name/czl_keyword等，
        //这样做可以避免对每种结构哈希时需要判断哈希对象的地址。
        return CZL_INDEX_CAC(index,
                             czl_bkdr_hash
                             (((czl_name*)kv)->name,
                              strlen(((czl_name*)kv)->name)),
                             mask);
    default: //CZL_ENUM 用于常量字符串
        switch (((czl_var*)kv)->type)
        {
        case CZL_STRING:
            return CZL_INDEX_CAC(index,
                                 czl_bkdr_hash
                                 (CZL_STR(((czl_var*)kv)->val.str.Obj)->str,
                                  CZL_STR(((czl_var*)kv)->val.str.Obj)->len),
                                 mask);
        case CZL_INT:
            return CZL_INDEX_CAC(index,
                                 czl_num_hash
                                 (((czl_var*)kv)->val.inum, 0, 0),
                                 mask);
        default: //CZL_FLOAT
            return CZL_INDEX_CAC(index,
                                 czl_num_hash
                                 (((czl_var*)kv)->val.fnum, 0, 0),
                                 mask);
        }
    }
}

static char czl_sys_hash_size_update
(
    czl_gp *gp,
    char type,
    unsigned long new_size,
    czl_sys_hash *table
)
{
	long index;
    unsigned long i, j = table->size;
    czl_bucket **datas, **buckets = table->buckets;
    czl_bucket *p, *q;

    if (0 == new_size) new_size = 1;

    if (!(datas=(czl_bucket**)CZL_TMP_CALLOC(gp, new_size, sizeof(czl_bucket*))))
        return 0;

    table->mask = -new_size;

    for (i = 0; i < j; i++)
    {
        for (p = buckets[i]; p; p = q)
        {
            q = p->next;
            index = czl_hash_index_get(type, p->key, table->mask);
            p->next = datas[index];
            datas[index] = p;
        }
    }

    CZL_TMP_FREE(gp, table->buckets, table->size*sizeof(czl_bucket*));

    table->size = new_size;
    table->buckets = datas;

    return 1;
}

void* czl_sys_hash_find
(
    char type,
    char flag, //用于 type==CZL_ENUM 时 key 的类型, type==CZL_STRING 时 key 的类型
    char *key,
    const czl_sys_hash *table
)
{
	long index; //必须有符号long型
	czl_bucket *p;

    if (0 == table->count) return NULL;

    switch (type)
    {
    case CZL_INT: //用于协成、线程ID
        CZL_INDEX_CAC(index, (unsigned long)key, table->mask);
        break;
    case CZL_STRING: //用于变量、函数、类名、系统关键字等
        //只要是建有哈希表的存储结构，哈希对象地址必须放在结构的首地址，
        //比如czl_var/czl_fun/czl_class/czl_sys_fun/czl_name/czl_keyword等，
        //这样做可以避免对每种结构哈希时需要判断哈希对象的地址。
        if (CZL_NIL == flag)
            CZL_INDEX_CAC(index, czl_bkdr_hash(key, strlen(key)), table->mask);
        else
            CZL_INDEX_CAC(index, *((unsigned long*)key), table->mask);
        break;
    default: //CZL_ENUM 用于常量字符串
        switch (flag)
        {
        case CZL_STRING:
            CZL_INDEX_CAC(index,
                          czl_bkdr_hash(key, strlen(key)),
                          table->mask);
            break;
        case CZL_INT:
            CZL_INDEX_CAC(index,
                          czl_num_hash(((czl_value*)key)->inum, 0, 0),
                          table->mask);
            break;
        default: //CZL_FLOAT
            CZL_INDEX_CAC(index,
                          czl_num_hash(((czl_value*)key)->fnum, 0, 0),
                          table->mask);
            break;
        }
        break;
    }

    p = table->buckets[index];

    switch (type)
    {
    case CZL_INT:
        while (p && (long)key != (long)p->key)
            p = p->next;
        break;
    case CZL_STRING:
        while (p && strcmp(key, ((czl_name*)p->key)->name))
            p = p->next;
        break;
    default: //CZL_ENUM
        switch (flag)
        {
        case CZL_STRING:
            while (p && (CZL_STRING != ((czl_var*)p->key)->type ||
                         strcmp(key, CZL_STR(((czl_var*)p->key)->val.str.Obj)->str)))
                p = p->next;
            break;
        case CZL_INT:
            while (p && (CZL_INT != ((czl_var*)p->key)->type ||
                         ((czl_value*)key)->inum != ((czl_var*)p->key)->val.inum))
                p = p->next;
            break;
        default: //CZL_FLOAT
            while (p && (CZL_FLOAT != ((czl_var*)p->key)->type ||
                         ((czl_value*)key)->fnum != ((czl_var*)p->key)->val.fnum))
                p = p->next;
            break;
        }
        break;
    }

    return p ? p->key : NULL;
}

char czl_sys_hash_insert
(
    czl_gp *gp,
    char type,
    void *kv,
    czl_sys_hash *table
)
{
	long index;
	czl_bucket *node;

    if (table->count+1 > table->size &&
        !czl_sys_hash_size_update(gp, type, table->size<<1, table))
        return 0;

    index = czl_hash_index_get(type, kv, table->mask);

    if (!(node=(czl_bucket*)CZL_TMP_MALLOC(gp, sizeof(czl_bucket))))
        return 0;

    node->key = kv;
    node->next = table->buckets[index];
    table->buckets[index] = node;
    table->count++;

    return 1;
}

void czl_sys_hash_delete
(
    czl_gp *gp,
    unsigned long id,
    czl_sys_hash *table
)
{
    //目前系统哈希表删除操作的type只有协程、线程的CZL_INT
    long index; //必须有符号long型
	czl_bucket *p, *q = NULL;
    CZL_INDEX_CAC(index, id, table->mask);
    p = table->buckets[index];

    while (p && id != (unsigned long)p->key) {
        q = p;
        p = p->next;
    }
    if (!p) return;

    if (q)
        q->next = p->next;
    else
        table->buckets[index] = p->next;

    CZL_TMP_FREE(gp, p, sizeof(czl_bucket));

    if (--table->count <= table->size>>2 && table->size >= 16)
        czl_sys_hash_size_update(gp, CZL_INT, table->size>>1, table);
}

void czl_hash_table_delete(czl_gp *gp, czl_sys_hash *table)
{
    czl_bucket *p, *q;
    unsigned long i, j = table->size;
    czl_bucket **buckets = table->buckets;

    for (i = 0; i < j; i++)
    {
        for (p = buckets[i]; p; p = q)
        {
            q = p->next;
            CZL_TMP_FREE(gp, p, sizeof(czl_bucket));
        }
    }

    CZL_TMP_FREE(gp, table->buckets, table->size*sizeof(czl_bucket*));
}
///////////////////////////////////////////////////////////////
static void czl_hash_tabkvs_insert(czl_table *tab)
{
    long index; //必须为有符号long型
    czl_tabkv *p;
    czl_tabkv **buckets = tab->buckets;

    memset(buckets, 0, tab->size*sizeof(czl_tabkv*));

    for (p = tab->eles_head; p; p = p->next)
    {
        CZL_INDEX_CAC(index,
                      (CZL_STRING == p->kt ?
                       p->key.str.size :
                       czl_num_hash(p->key.inum, tab->key, tab->attack_cnt)),
                      tab->mask);

        p->clsLast = (czl_tabkv*)(buckets+index);
        p->clsNext = buckets[index];
        if (buckets[index])
            buckets[index]->clsLast = p;
        buckets[index] = p;
    }
}

static void czl_hash_key_update(czl_gp *gp, czl_table *tab)
{
    czl_tabkv *p;

    ++tab->attack_cnt;
    tab->key = czl_hash_key_create(gp->mm_cnt, tab->key);

    for (p = tab->eles_head; p; p = p->next)
        if (CZL_STRING == p->kt)
            p->key.str.size = czl_str_hash(CZL_STR(p->key.str.Obj)->str,
                                           CZL_STR(p->key.str.Obj)->len,
                                           tab->key, 1);

    czl_hash_tabkvs_insert(tab);
}

static void czl_hash_collision_check(czl_gp *gp, czl_table *tab)
{
    unsigned long i, j = tab->size;

    for (i = 0; i < j; i++)
    {
        unsigned long cnt = 0;
        czl_tabkv *p;
        for (p = tab->buckets[i]; p; p = p->clsNext)
            if (++cnt >= CZL_MAX_HASH_COLLISION_CNT)
            {
                czl_hash_key_update(gp, tab);
                return;
            }
    }
}

static char czl_hash_size_update
(
    czl_gp *gp,
    unsigned long new_size,
    czl_table *tab
)
{
    czl_tabkv **buckets;

    if (0 == new_size) new_size = 1;

    if (!(buckets=(czl_tabkv**)CZL_TMP_REALLOC(gp, tab->buckets,
                                               new_size*sizeof(czl_tabkv*),
                                               tab->size*sizeof(czl_tabkv*))))
        return 0;

    tab->buckets = buckets;
    tab->mask = -new_size;
    tab->size = new_size;

    czl_hash_tabkvs_insert(tab);
    czl_hash_collision_check(gp, tab); //检查是否发生哈希碰撞攻击

    return 1;
}

static void czl_hash_tabkv_insert
(
    czl_table *tab,
    czl_tabkv *kv,
    long index,
    czl_tabkv *ins
)
{
    czl_tabkv **buckets = tab->buckets;

    kv->clsLast = (czl_tabkv*)(buckets+index);
    kv->clsNext = buckets[index];
    if (buckets[index])
        buckets[index]->clsLast = kv;
    buckets[index] = kv;

    if (ins)
    {
        kv->next = ins->next;
        kv->last = ins;
        ins->next = kv;
        if (kv->next)
            kv->next->last = kv;
        else
            tab->eles_tail = kv;
    }
    else
    {
        if (NULL == tab->eles_head)
        {
            tab->eles_head = kv;
            kv->last = NULL;
        }
        else
        {
            tab->eles_tail->next = kv;
            kv->last = tab->eles_tail;
        }
        kv->next = NULL;
        tab->eles_tail = kv;
    }

    tab->count++;
}

czl_tabkv* czl_create_str_tabkv
(
    czl_gp *gp,
    czl_table *tab,
    unsigned long hash,
    long len,
    char *str
)
{
    long index; //必须为有符号Long型
    czl_tabkv *kv = (czl_tabkv*)czl_malloc(gp, sizeof(czl_tabkv)
                                       #ifdef CZL_MM_MODULE
                                       , &tab->pool
                                       #endif
                                       );
    if (!kv)
        return NULL;

    if (len < 0)
    {
        void **obj = (void**)str;
        kv->key.str.Obj = obj;
        ++CZL_STR(obj)->rc;
    }
    else if (!czl_str_create(gp, &kv->key.str, len+1, len, str))
    {
        czl_free(gp, kv, sizeof(czl_tabkv)
                 #ifdef CZL_MM_MODULE
                 , &tab->pool
                 #endif
                 );
        return NULL;
    }

    kv->name = NULL;
    kv->quality = CZL_OBJ_ELE;
    kv->ot = CZL_NIL;
    kv->kt = CZL_STRING;
    kv->key.str.size = hash;
    kv->type = CZL_INT;

    czl_hash_tabkv_insert(tab, kv, CZL_INDEX_CAC(index, hash, tab->mask), NULL);

    return kv;
}

czl_tabkv* czl_create_num_tabkv(czl_gp *gp, czl_table *tab, czl_long num)
{
    long index; //必须为有符号Long型
    unsigned long hash = czl_num_hash(num, tab->key, tab->attack_cnt);
    czl_tabkv *kv = (czl_tabkv*)czl_malloc(gp, sizeof(czl_tabkv)
                                       #ifdef CZL_MM_MODULE
                                       , &tab->pool
                                       #endif
                                       );
    if (!kv)
        return NULL;

    kv->name = NULL;
    kv->quality = CZL_OBJ_ELE;
    kv->ot = CZL_NIL;
    kv->kt = CZL_INT;
    kv->key.inum = num;
    kv->type = CZL_INT;

    czl_hash_tabkv_insert(tab, kv, CZL_INDEX_CAC(index, hash, tab->mask), NULL);

    return kv;
}

void czl_swap_tabkv(czl_table *tab, czl_tabkv *ins, czl_tabkv *p)
{
    if (p->last)
        p->last->next = p->next;
    else
        tab->eles_head = p->next;
    if (p->next)
        p->next->last = p->last;
    else
        tab->eles_tail = p->last;

    p->next = ins->next;
    p->last = ins;
    ins->next = p;
    if (p->next)
        p->next->last = p;
    else
        tab->eles_tail = p;
}

czl_tabkv* czl_create_key_str_tabkv
(
    czl_gp *gp,
    czl_table *tab,
    czl_var *res,
    czl_tabkv *ins
)
{
    char *str = CZL_STR(res->val.str.Obj)->str;
    unsigned long collision_cnt = 0;
    unsigned long len = CZL_STR(res->val.str.Obj)->len;
    unsigned long hash = czl_str_hash(str, len, tab->key, tab->attack_cnt);
	czl_tabkv *p;
    long index; //必须为有符号Long型
    CZL_INDEX_CAC(index, hash, tab->mask);
    p = tab->buckets[index];

    while (p && (CZL_STRING != p->kt ||
                 hash != p->key.str.size ||
                 memcmp(str, CZL_STR(p->key.str.Obj)->str, len)))
    {
        p = p->clsNext;
        ++collision_cnt;
    }
    if (p)
    {
        if (ins && p != ins->next)
            czl_swap_tabkv(tab, ins, p);
        return p;
    }

    if (collision_cnt >= CZL_MAX_HASH_COLLISION_CNT)
        czl_hash_key_update(gp, tab);

    if (!(p=(czl_tabkv*)czl_malloc(gp, sizeof(czl_tabkv)
                       #ifdef CZL_MM_MODULE
                       , &tab->pool
                       #endif
                       )))
        return NULL;

    if (CZL_ARRBUF_VAR == res->quality)
    {
        if (!czl_str_resize(gp, &res->val.str))
            goto CZL_ERROR_END;
        res->quality = CZL_DYNAMIC_VAR;
        p->key = res->val;
    }
    else if (CZL_FUNRET_VAR == res->quality)
    {
        if (!czl_str_resize(gp, &res->val.str))
            goto CZL_ERROR_END;
        p->key.str = res->val.str;
        res->type = CZL_INT;
    }
    else if (res->val.str.size != CZL_STR(res->val.str.Obj)->len + 1)
    {
        if (!czl_str_create(gp, &p->key.str,
                            CZL_STR(res->val.str.Obj)->len+1,
                            CZL_STR(res->val.str.Obj)->len,
                            CZL_STR(res->val.str.Obj)->str))
            goto CZL_ERROR_END;
    }
    else
    {
        p->key.str = res->val.str;
        CZL_SRCA1(res->val.str);
    }

    p->name = NULL;
    p->quality = CZL_OBJ_ELE;
    p->ot = CZL_NIL;
    p->kt = CZL_STRING;
    p->key.str.size = hash;
    p->type = CZL_INT;

    czl_hash_tabkv_insert(tab, p, index, ins);
    return p;

CZL_ERROR_END:
    czl_free(gp, p, sizeof(czl_tabkv)
             #ifdef CZL_MM_MODULE
             , &tab->pool
             #endif
             );
    return NULL;
}

czl_tabkv* czl_create_key_num_tabkv
(
    czl_gp *gp,
    czl_table *tab,
    czl_long num,
    czl_tabkv *ins
)
{
    unsigned long collision_cnt = 0;
	czl_tabkv *p;
    long index; //必须为有符号Long型
    CZL_INDEX_CAC(index,
                  czl_num_hash(num, tab->key, tab->attack_cnt),
                  tab->mask);
    p = tab->buckets[index];

    while (p && (CZL_INT != p->kt || num != p->key.inum))
    {
        p = p->clsNext;
        ++collision_cnt;
    }
    if (p)
    {
        if (ins && p != ins->next)
            czl_swap_tabkv(tab, ins, p);
        return p;
    }

    if (collision_cnt >= CZL_MAX_HASH_COLLISION_CNT)
        czl_hash_key_update(gp, tab);

    if (!(p=(czl_tabkv*)czl_malloc(gp, sizeof(czl_tabkv)
                       #ifdef CZL_MM_MODULE
                       , &tab->pool
                       #endif
                       )))
        return NULL;

    p->name = NULL;
    p->quality = CZL_OBJ_ELE;
    p->ot = CZL_NIL;
    p->kt = CZL_INT;
    p->key.inum = num;
    p->type = CZL_INT;

    czl_hash_tabkv_insert(tab, p, index, ins);

    return p;
}

czl_tabkv* czl_create_tabkv
(
    czl_gp *gp,
    czl_table *tab,
    czl_var *key,
    czl_tabkv *ins
)
{
    if (tab->count+1 > tab->size &&
        !czl_hash_size_update(gp, tab->size<<1, tab))
        return NULL;

    switch (key->type)
    {
    case CZL_STRING:
        return czl_create_key_str_tabkv(gp, tab, key, ins);
    case CZL_INT:
        return czl_create_key_num_tabkv(gp, tab, key->val.inum, ins);
    case CZL_FLOAT:
        return czl_create_key_num_tabkv(gp, tab, key->val.fnum, ins);
    default:
        return NULL;
    }
}

char czl_delete_tabkv_node
(
    czl_gp *gp,
    czl_table *tab,
    czl_tabkv *p,
    long index //必须为有符号long型
)
{
    czl_tabkv **buckets = tab->buckets;

    if (CZL_VAR_EXIST_REF(p))
        czl_ref_obj_delete(gp, (czl_var*)p);
    if (!czl_val_del(gp, (czl_var*)p))
        return 0;
    if (p == tab->foreach_inx)
        tab->foreach_inx = p->next;

    if (p->last)
        p->last->next = p->next;
    else
        tab->eles_head = p->next;
    if (p->next)
        p->next->last = p->last;
    else
        tab->eles_tail = p->last;

    if ((czl_tabkv**)p->clsLast >= buckets &&
        (czl_tabkv**)p->clsLast <= buckets+tab->size)
        buckets[index] = p->clsNext;
    else
        p->clsLast->clsNext = p->clsNext;
    if (p->clsNext)
        p->clsNext->clsLast = p->clsLast;

    if (CZL_STRING == p->kt)
        CZL_KEY_SRCD1(gp, p->key.str.Obj);
    czl_free(gp, p, sizeof(czl_tabkv)
             #ifdef CZL_MM_MODULE
             , &tab->pool
             #endif
             );

    if (--tab->count <= tab->size>>2 && tab->size >= 16)
        czl_hash_size_update(gp, tab->size>>1, tab);

    return 1;
}

static czl_tabkv* czl_get_str_tabkv(czl_tabkv *p, czl_string *s, unsigned long hash)
{
    while (p && (CZL_STRING != p->kt ||
                 hash != p->key.str.size ||
                 memcmp(s->str, CZL_STR(p->key.str.Obj)->str, s->len)))
        p = p->clsNext;

    return p;
}

static czl_tabkv* czl_get_num_tabkv(czl_tabkv *p, czl_long num)
{
    while (p && (CZL_INT != p->kt || num != p->key.inum))
        p = p->clsNext;

    return p;
}

char czl_delete_str_tabkv
(
    czl_gp *gp,
    czl_value *val,
    czl_string *s
)
{
    czl_tabkv *p;
    czl_table *tab = CZL_TAB(val->Obj);
    unsigned long hash = czl_str_hash(s->str, s->len, tab->key, tab->attack_cnt);
    long index; //必须为有符号Long型
    CZL_INDEX_CAC(index, hash, tab->mask);
    
    if (!(p=czl_get_str_tabkv(tab->buckets[index], s, hash)))
        return 0;

    if (tab->rc > 1)
    {
        if (!czl_table_fork(gp, tab, val, 1))
            return 0;
        tab = CZL_TAB(val->Obj);
        p = czl_get_str_tabkv(tab->buckets[index], s, hash);
    }

    return czl_delete_tabkv_node(gp, tab, p, index);
}

char czl_delete_num_tabkv
(
    czl_gp *gp,
    czl_value *val,
    czl_long num
)
{
    czl_tabkv *p;
    czl_table *tab = CZL_TAB(val->Obj);
    long index; //必须为有符号Long型
    CZL_INDEX_CAC(index,
                  czl_num_hash(num, tab->key, tab->attack_cnt),
                  tab->mask);

    if (!(p=czl_get_num_tabkv(tab->buckets[index], num)))
        return 0;

    if (tab->rc > 1)
    {
        if (!czl_table_fork(gp, tab, val, 1))
            return 0;
        tab = CZL_TAB(val->Obj);
        p = czl_get_num_tabkv(tab->buckets[index], num);
    }

    return czl_delete_tabkv_node(gp, tab, p, index);
}

char czl_delete_tabkv
(
    czl_gp *gp,
    czl_value *tab,
    czl_var *key
)
{
    switch (key->type)
    {
    case CZL_STRING:
        return czl_delete_str_tabkv(gp, tab, CZL_STR(key->val.str.Obj));
    case CZL_INT:
        return czl_delete_num_tabkv(gp, tab, key->val.inum);
    case CZL_FLOAT:
        return czl_delete_num_tabkv(gp, tab, key->val.fnum);
    default:
        return 0;
    }
}

czl_tabkv* czl_find_str_tabkv
(
    czl_table *tab,
    czl_string *s
)
{
    unsigned long hash = czl_str_hash(s->str, s->len, tab->key, tab->attack_cnt);
    long index; //必须为有符号Long型
    CZL_INDEX_CAC(index, hash, tab->mask);

    return czl_get_str_tabkv(tab->buckets[index], s, hash);
}

czl_tabkv* czl_find_num_tabkv
(
    czl_table *tab,
    czl_long num
)
{
    long index; //必须为有符号Long型
    CZL_INDEX_CAC(index,
                  czl_num_hash(num, tab->key, tab->attack_cnt),
                  tab->mask);

    return czl_get_num_tabkv(tab->buckets[index], num);
}

czl_tabkv* czl_find_tabkv
(
    czl_table *tab,
    czl_var *key
)
{
    switch (key->type)
    {
    case CZL_STRING:
        return czl_find_str_tabkv(tab, CZL_STR(key->val.str.Obj));
    case CZL_INT:
        return czl_find_num_tabkv(tab, key->val.inum);
    case CZL_FLOAT:
        return czl_find_num_tabkv(tab, key->val.fnum);
    default:
        return NULL;
    }
}
///////////////////////////////////////////////////////////////
char czl_shell_name_save(czl_gp *gp, char *path, char flag)
{
    czl_name *node = (czl_name*)CZL_TMP_MALLOC(gp, sizeof(czl_name));
    if (!node)
        return 0;

    if (flag)
        node->name = path;
    else
    {
        if (!(node->name=(char*)CZL_TMP_MALLOC(gp, strlen(path)+1)))
        {
            CZL_TMP_FREE(gp, node, sizeof(czl_name));
            return 0;
        }
        strcpy(node->name, path);
    }

    node->next = gp->sn_head;
    gp->sn_head = node;

    return czl_sys_hash_insert(gp, CZL_STRING, node, &gp->tmp->sn_hash);
}

void czl_shell_name_delete(czl_gp *gp, czl_name *p)
{
    czl_name *q;

    while (p)
    {
        q = p->next;
        CZL_TMP_FREE(gp, p->name, strlen(p->name)+1);
        CZL_TMP_FREE(gp, p, sizeof(czl_name));
        p = q;
    }
}

czl_name* czl_shell_name_find(czl_gp *gp, char *name)
{
    return (czl_name*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                        name, &gp->tmp->sn_hash);
}
///////////////////////////////////////////////////////////////
void czl_loc_var_list_delete(czl_gp *gp, czl_loc_var *p, czl_var *flag)
{
    czl_loc_var *q;
    czl_var tmp;

    while (p)
    {
        q = p->next;
        if (NULL == flag)
        {
            CZL_TMP_FREE(gp, p->var, strlen((char*)p->var)+1);
            if (CZL_STATIC_VAR == p->quality)
            {
                tmp.type = p->type;
                tmp.val = p->val;
                czl_val_del(gp, &tmp);
            }
        }
        CZL_TMP_FREE(gp, p, sizeof(czl_loc_var));
        p = q;
    }
}

void czl_store_device_delete(czl_gp *gp, czl_fun *fun)
{
    czl_store_device *p, *q;

    for (p = fun->store_device_head; p; p = q)
    {
        q = p->next;
        czl_loc_var_list_delete(gp, p->vars, fun->vars);
        czl_enum_list_delete(gp, p->enums);
        CZL_TMP_FREE(gp, p, sizeof(czl_store_device));
    }
}

char czl_store_device_build
(
    czl_gp *gp,
    czl_fun *fun,
    czl_store_device **node
)
{
	czl_store_device *p;

    if (*node)
        return 1;

    if (!(p=(czl_store_device*)CZL_TMP_MALLOC(gp, sizeof(czl_store_device))))
        return 0;

    p->vars = NULL;
    p->enums = NULL;
    p->next = fun->store_device_head;
    *node = fun->store_device_head = p;

    return 1;
}

char czl_store_device_check(czl_gp *gp)
{
	czl_fun *fun;
	czl_code_block *block;

    if (gp->tmp->analysis_field != CZL_IN_GLOBAL_FUN &&
            gp->tmp->analysis_field != CZL_IN_CLASS_FUN)
        return 1;

    fun = gp->cur_fun;
    block = gp->tmp->code_blocks + gp->tmp->code_blocks_count - 1;
    switch (block->type)
    {
    case CZL_FUN_BLOCK:
        return czl_store_device_build
                (gp, fun, &block->block.fun->store_device);
    case CZL_LOOP_BLOCK:
        return czl_store_device_build
                (gp, fun, &block->block.loop->store_device);
    case CZL_TASK_BLOCK:
        return czl_store_device_build
                (gp, fun, &block->block.task->store_device);
    case CZL_TRY_BLOCK:
        return czl_store_device_build
                (gp, fun, &block->block.Try->store_device);
    default: //CZL_BRANCH_BLOCK
        if (!block->block.branch->childs_tail)
            return czl_store_device_build
                    (gp, fun, &block->block.branch->store_device);
        else
            return czl_store_device_build
                    (gp, fun, &block->block.branch->childs_tail->store_device);
    }
}

czl_store_device* czl_store_device_get(czl_gp *gp)
{
    czl_code_block *block = gp->tmp->code_blocks +
                            gp->tmp->code_blocks_count - 1;
    switch (block->type)
    {
    case CZL_FUN_BLOCK:
        return block->block.fun->store_device;
    case CZL_LOOP_BLOCK:
        return block->block.loop->store_device;
    case CZL_TASK_BLOCK:
        return block->block.task->store_device;
    case CZL_TRY_BLOCK:
        return block->block.Try->store_device;
    default: //CZL_BRANCH_BLOCK
        if (!block->block.branch->childs_tail)
            return block->block.branch->store_device;
        else
            return block->block.branch->childs_tail->store_device;
    }
}
///////////////////////////////////////////////////////////////
czl_glo_var* czl_var_node_find
(
    const char *name,
    czl_glo_var *p
)
{
    while (p)
    {
        if (strcmp(name, p->name) == 0)
            return p;
        p = p->next;
    }

    return NULL;
}

czl_var* czl_constant_find(const char *name, czl_enum_list p)
{
    czl_glo_var *target;

    while (p)
    {
        if ((target=czl_var_node_find(name, p->constants_head)))
            return (czl_var*)target;
        p = p->next;
    }

    return NULL;
}

czl_enum* czl_enum_node_create(czl_gp *gp)
{
    czl_enum *p = (czl_enum*)CZL_RT_MALLOC(gp, sizeof(czl_enum));
    if (!p)
        return NULL;

    p->constants_head = NULL;
    p->constants_tail = NULL;
    p->next = NULL;

    return p;
}

void czl_enum_node_insert
(
    czl_enum **head,
    czl_enum *node
)
{
    node->next = *head;
    *head = node;
}

void czl_enum_insert(czl_gp *gp, czl_enum *node)
{
    czl_store_device *store;

    switch (gp->tmp->analysis_field)
    {
    case CZL_IN_GLOBAL:
        czl_enum_node_insert(&gp->enums_head, node);
        break;
    case CZL_IN_CLASS:
        czl_enum_node_insert(&gp->tmp->cur_class->enums, node);
        break;
    default: //CZL_IN_GLOBAL_FUN、CZL_IN_CLASS_FUN
        store = czl_store_device_get(gp);
        czl_enum_node_insert(&store->enums, node);
        break;
    }
}

char czl_const_exp_init(czl_gp *gp, czl_var *var, char quality)
{
    //优化建议: 释放 czl_ele_create() 内存
    switch (gp->tmp->exp_head->res->type)
    {
    case CZL_ARRAY_LIST:
        czl_set_array_list_reg(gp, CZL_ARR_LIST(gp->tmp->exp_head->res->val.Obj));
        break;
    case CZL_TABLE_LIST:
        czl_set_table_list_reg(gp, CZL_TAB_LIST(gp->tmp->exp_head->res->val.Obj));
        break;
    case CZL_NEW:
        czl_set_new_reg(gp, (czl_new_sentence*)gp->tmp->exp_head->res->val.Obj);
        break;
    default:
        break;
    }

    if ((CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field ||
         CZL_IN_CLASS_FUN == gp->tmp->analysis_field) &&
         CZL_STATIC_VAR == quality)
    {
        czl_loc_var *loc = (czl_loc_var*)var;
        czl_var tmp;
        tmp.type = CZL_INT;
        tmp.ot = loc->ot;
        if (!czl_val_copy(gp, &tmp, gp->tmp->exp_head->res))
            return 0;
        loc->type = tmp.type;
        loc->val = tmp.val;
    }
    else if (!czl_val_copy(gp, var, gp->tmp->exp_head->res))
        return 0;

    if (CZL_CONST_VAR == var->quality)
        var->ot = var->type;

    czl_exp_stack_delete(gp, gp->tmp->exp_head);
    gp->tmp->exp_head = NULL;

    return 1;
}

void czl_var_list_delete(czl_gp *gp, czl_glo_var *p)
{
    czl_glo_var *q;

    while (p)
    {
        q = p->next;
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, (czl_var*)p);
        czl_val_del(gp, (czl_var*)p);
        CZL_RT_FREE(gp, p, sizeof(czl_glo_var));
        p = q;
    }
}

void czl_enum_list_delete(czl_gp *gp, czl_enum *p)
{
    czl_enum *q;

    while (p)
    {
        q = p->next;
        czl_var_list_delete(gp, p->constants_head);
        CZL_RT_FREE(gp, p, sizeof(czl_enum));
        p = q;
    }
}
///////////////////////////////////////////////////////////////
static void czl_init_var_type(czl_var *var, unsigned char ot)
{
    var->val.inum = 0;
    switch (ot)
    {
    case CZL_FLOAT: case CZL_FUN_REF:
        var->type = ot;
        break;
    default:
        var->type = CZL_INT;
        break;
    }
}

czl_loc_var* czl_loc_var_create
(
    czl_gp *gp,
    czl_loc_var **head,
    czl_loc_var **tail,
    char *name,
    char ot,
    char quality,
    char optimizable
)
{
    czl_loc_var *node = (czl_loc_var*)CZL_TMP_MALLOC(gp, sizeof(czl_loc_var));
    if (!node)
        return NULL;

    if (!(node->var=CZL_TMP_MALLOC(gp, strlen(name)+1)))
    {
        CZL_TMP_FREE(gp, node, sizeof(czl_loc_var));
        return NULL;
    }
    strcpy((char*)node->var, name);

    node->flag = CZL_NIL; //与 struct var 的 type 内存对其
    node->quality = quality;
    node->ot = ot;
    node->optimizable = optimizable;

    if (CZL_DYNAMIC_VAR == quality)
    {
        ++gp->cur_fun->dynamic_vars_cnt;
    }
    else //CZL_STATIC_VAR
    {
        ++gp->cur_fun->static_vars_cnt;
        node->type = CZL_INT;
        node->val.inum = 0;
    }

    if (tail)
    {
        node->next = NULL;
        if (NULL == *head)
            *head = node;
        else
            (*tail)->next = node;
        *tail = node;
    }
    else
    {
        node->next = *head;
        *head = node;
    }

    return node;
}

czl_glo_var* czl_glo_var_create
(
    czl_gp *gp,
    const char *name,
    char quality,
    char ot
)
{
    czl_glo_var *p = (czl_glo_var*)CZL_RT_MALLOC(gp, sizeof(czl_glo_var));
    if (!p)
        return NULL;

    //系统函数传入参数变量名为空
    //类实例变量名不分配新空间而是指向类变量名
    if (!name)
        p->name = NULL;
    else
    {
        if (!(p->name=(char*)CZL_TMP_MALLOC(gp, strlen(name)+1)))
        {
            CZL_RT_FREE(gp, p, sizeof(czl_glo_var));
            return NULL;
        }
        strcpy(p->name, name);
    }

    p->ot = ot;
    p->quality = quality;
    p->optimizable = 1;
    czl_init_var_type((czl_var*)p, ot);
    p->next = NULL;

    return p;
}

czl_glo_var* czl_class_var_create
(
    czl_gp *gp,
    char *name,
    char quality,
    char permission,
    char ot
)
{
	unsigned long len;
    czl_class_var *p = (czl_class_var*)CZL_RT_MALLOC(gp, sizeof(czl_class_var));
    if (!p)
        return NULL;

    len = strlen(name);
    if (!(p->name=(char*)CZL_RT_MALLOC(gp, len+1)))
    {
        CZL_RT_FREE(gp, p, sizeof(czl_class_var));
        return NULL;
    }
    strcpy(p->name, name);

    p->ot = ot;
    p->permission = permission;
    p->quality = quality;
    czl_init_var_type((czl_var*)p, ot);
    p->next = NULL;
    p->hash = czl_bkdr_hash(name, len);

    if (CZL_DYNAMIC_VAR == quality)
        p->index = gp->tmp->cur_class->vars_count++;

    return (czl_glo_var*)p;
}

void czl_class_var_list_delete(czl_gp *gp, czl_class_var *p)
{
    czl_class_var *q;

    while (p)
    {
        q = p->next;
        CZL_RT_FREE(gp, p->Name, strlen(p->Name)+1);
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, (czl_var*)p);
        czl_val_del(gp, (czl_var*)p);
        CZL_RT_FREE(gp, p, sizeof(czl_class_var));
        p = q;
    }
}

void czl_var_node_insert
(
    czl_glo_var **head,
    czl_glo_var **tail,
    czl_glo_var *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

void czl_class_var_insert
(
    czl_class_var **head,
    czl_class_var **tail,
    czl_class_var *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

czl_bucket* czl_hash_repeat_check
(
    unsigned long hash,
    czl_sys_hash *table,
    char type
)
{
	czl_bucket *p;
    long index; //必须有符号long型

    if (0 == table->count) return NULL;

    CZL_INDEX_CAC(index, hash, table->mask);
    p = table->buckets[index];

    if (CZL_LG_VAR == type)
        while (p && hash != ((czl_class_var*)p->key)->hash)
            p = p->next;
    else //CZL_FUNCTION
        while (p && hash != ((czl_fun*)p->key)->hash)
            p = p->next;

    return p;
}

czl_var* czl_var_create_by_field(czl_gp *gp, char *name, char quality, char ot)
{
    czl_glo_var *node;
    czl_store_device *store;

    switch (gp->tmp->analysis_field)
    {
    case CZL_IN_GLOBAL:
        if (!(node=czl_glo_var_create(gp, name, quality, ot)))
            return NULL;
        if (CZL_CONST_VAR == quality)
            czl_var_node_insert
                (&gp->enums_head->constants_head,
                 &gp->enums_head->constants_tail, node);
        else //变量
            czl_var_node_insert(&gp->vars_head,
                                &gp->tmp->global_vars_tail, node);
        if (!czl_sys_hash_insert(gp, CZL_STRING, node, &gp->tmp->vars_hash))
            return NULL;
        break;
    case CZL_IN_CLASS:
        if (CZL_CONST_VAR == quality)
        {
            if (!(node=czl_glo_var_create(gp, name, quality, ot)))
                return NULL;
            czl_var_node_insert
                (&gp->tmp->cur_class->enums->constants_head,
                 &gp->tmp->cur_class->enums->constants_tail, node);
        }
        else //变量
        {
            if (!(node=czl_class_var_create
                  (gp, name, quality, gp->tmp->permission, ot)))
                return NULL;
            czl_class_var_insert(&gp->tmp->cur_class->vars,
                                 &gp->tmp->class_vars_tail,
                                 (czl_class_var*)node);
            if (czl_hash_repeat_check(((czl_class_var*)node)->hash,
                                      &gp->tmp->cur_class->vars_hash,
                                      CZL_LG_VAR))
            {
                sprintf(gp->log_buf, "hash repeat of class var %s , ", name);
                return NULL;
            }
        }
        if (!czl_sys_hash_insert(gp, CZL_STRING, node,
                                 &gp->tmp->cur_class->vars_hash))
            return NULL;
        break;
    default: //CZL_IN_GLOBAL_FUN、CZL_IN_CLASS_FUN
        store = czl_store_device_get(gp);
        switch (quality)
        {
        case CZL_DYNAMIC_VAR: case CZL_STATIC_VAR:
            if (!(node=(czl_glo_var*)czl_loc_var_create
                 (gp, &store->vars, NULL, name, ot, quality, 1)))
                return NULL;
            break;
        default: //CZL_CONST_VAR
            if (!(node=czl_glo_var_create(gp, name, quality, ot)))
                return NULL;
            czl_var_node_insert(&store->enums->constants_head,
                                &store->enums->constants_tail, node);
            break;
        }
        break;
    }

    return (czl_var*)node;
}

czl_var* czl_var_find_in_global(czl_gp *gp, char *name)
{
    czl_var *node = (czl_var*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                name, &gp->tmp->vars_hash);

    if (CZL_IN_CONSTANT == gp->tmp->variable_field &&
        node && node->quality != CZL_CONST_VAR)
        return NULL;

    return node;
}

void* czl_loc_var_find
(
    const char *name,
    czl_loc_var *p
)
{
    while (p)
    {
        if (strcmp(name, (char*)p->var) == 0)
            return p;
        p = p->next;
    }

    return NULL;
}

czl_var* czl_var_find_in_store
(
    czl_gp *gp,
    const char *name,
    const czl_store_device *store
)
{
    czl_var *node;

    if (CZL_IN_ANY == gp->tmp->variable_field &&
        (node=(czl_var*)czl_loc_var_find(name, store->vars)))
    {
        return node;
    }

    if (CZL_IN_ANY == gp->tmp->variable_field ||
            CZL_IN_CONSTANT == gp->tmp->variable_field)
        return czl_constant_find(name, store->enums);

    return NULL;
}

czl_var* czl_var_find_in_fun
(
    czl_gp *gp,
    char *name,
    char mode
)
{
    czl_var *node = NULL;
    czl_code_block *block = gp->tmp->code_blocks +
                            gp->tmp->code_blocks_count - 1;

    do {
        switch (block->type)
        {
        case CZL_FUN_BLOCK:
            if (block->block.fun->store_device &&
                (node=czl_var_find_in_store(gp, name,
                    block->block.fun->store_device)))
                return node;
            if (CZL_IN_ANY == gp->tmp->variable_field &&
                (node=(czl_var*)czl_loc_var_find
                        (name, block->block.fun->loc_vars)))
            {
                return node;
            }
            return NULL;
        case CZL_LOOP_BLOCK:
            if (block->block.loop->store_device)
                node = czl_var_find_in_store(gp, name,
                        block->block.loop->store_device);
            break;
        case CZL_TASK_BLOCK:
            if (block->block.task->store_device)
                node = czl_var_find_in_store(gp, name,
                        block->block.task->store_device);
            break;
        case CZL_TRY_BLOCK:
            if (block->block.Try->store_device)
                node = czl_var_find_in_store(gp, name,
                        block->block.Try->store_device);
            break;
        default: //CZL_BRANCH_BLOCK
            if (!block->block.branch->childs_tail)
            {
                if (block->block.branch->store_device)
                    node = czl_var_find_in_store(gp, name,
                            block->block.branch->store_device);
            }
            else
            {
                if (block->block.branch->childs_tail->store_device)
                    node = czl_var_find_in_store(gp, name,
                            block->block.branch->childs_tail->store_device);
            }
            break;
        }
        block--;
    } while (!node && CZL_GLOBAL_FIND == mode);

    return node;
}

czl_class_var* czl_var_find_in_class_index
(
    czl_gp *gp,
    char *name,
    const czl_sys_hash *table
)
{
    czl_class_var *node = (czl_class_var*)czl_sys_hash_find(CZL_STRING, CZL_NIL, 
														    name, table);
    if (CZL_IN_CONSTANT == gp->tmp->variable_field &&
            node && node->quality != CZL_CONST_VAR)
        return NULL;

    return node;
}

czl_class_var* czl_var_find_in_class_parents
(
    czl_gp *gp,
    char *name,
    const czl_class_parent *p,
    char flag
)
{
    czl_class_var *node;
    czl_class *pclass;

    while (p)
    {
        if (flag || p->permission != CZL_PRIVATE)
        {
            pclass = p->pclass;
            node = czl_var_find_in_class_index
                    (gp, name, &pclass->vars_hash);
            if (node && node->permission != CZL_PRIVATE)
            {
                if (CZL_IN_CLASS_FUN == gp->tmp->analysis_field &&
                    CZL_DYNAMIC_VAR == node->quality)
                {
                    gp->tmp->current_class_var = node;
                    gp->tmp->current_var_class = pclass;
                }
                return node;
            }
            if (p->permission != CZL_PRIVATE)
            {
                if ((node=czl_var_find_in_class_parents
                     (gp, name, pclass->parents, 0)))
                    return node;
            }
        }
        p = p->next;
    }

    return NULL;
}

czl_var* czl_var_find_in_class
(
    czl_gp *gp,
    char *name,
    char find_parents_flag
)
{
    czl_class_var *node = czl_var_find_in_class_index
                          (gp, name, &gp->tmp->cur_class->vars_hash);
    if (node)
    {
        if (find_parents_flag &&
            CZL_IN_CLASS_FUN == gp->tmp->analysis_field &&
            CZL_DYNAMIC_VAR == node->quality)
        {
            gp->tmp->current_class_var = node;
            gp->tmp->current_var_class = gp->tmp->cur_class;
        }
        return (czl_var*)node;
    }

    if (find_parents_flag)
        return (czl_var*)czl_var_find_in_class_parents
                            (gp, name, gp->tmp->cur_class->parents, 1);

    return NULL;
}

czl_var* czl_var_find(czl_gp *gp, char *name, char mode)
{
    switch (gp->tmp->analysis_field)
    {
    case CZL_IN_GLOBAL:
        return czl_var_find_in_global(gp, name);
    case CZL_IN_CLASS:
        return czl_var_find_in_class(gp, name, 0);
    default: //CZL_IN_GLOBAL_FUN、CZL_IN_CLASS_FUN
        return czl_var_find_in_fun(gp, name, mode);
    }
}

czl_var* czl_var_find_in_exp
(
    czl_gp *gp,
    char *name,
    char global_flag,
    char this_flag
)
{
    czl_var *node;

    gp->tmp->current_class_var = NULL;

    if (global_flag)
        return czl_var_find_in_global(gp, name);
    else if (this_flag)
    {
        node = czl_var_find_in_class(gp, name, 1);
        if (gp->tmp->current_class_var)
            return czl_class_ref_var_node_create(gp);
        return node;
    }
    else
        return czl_var_find(gp, name, CZL_GLOBAL_FIND);
}
///////////////////////////////////////////////////////////////
char czl_nsef_create(czl_gp *gp, void *ef, char flag)
{
    czl_nsef *n = (czl_nsef*)CZL_TMP_MALLOC(gp, sizeof(czl_nsef));
    if (!n)
        return 0;

    n->flag = flag;
    n->ef = (czl_exp_fun*)ef;
    n->err_file = gp->error_file;
    n->err_line = gp->error_line;
    n->next = NULL;

    if (!gp->tmp->nsef_head)
        gp->tmp->nsef_head = n;
    else
        gp->tmp->nsef_tail->next = n;
     gp->tmp->nsef_tail = n;

    return 1;
}

czl_exp_fun* czl_exp_fun_create
(
    czl_gp *gp,
    czl_fun *fun,
    char quality
)
{
    czl_exp_fun *p = (czl_exp_fun*)CZL_RT_MALLOC(gp, sizeof(czl_exp_fun));
    if (!p)
        return NULL;

    p->fun = fun;
    p->paras = NULL;
    p->paras_count = 0;
    p->quality = quality;
    p->flag = 1;

    if (CZL_STATIC_FUN == quality && CZL_IN_STATEMENT == fun->state)
    {
        if (!czl_nsef_create(gp, p, 1))
        {
            CZL_RT_FREE(gp, p, sizeof(czl_exp_fun));
            return NULL;
        }
    }

    p->next = gp->expfun_head;
    gp->expfun_head = p;

    return p;
}

czl_para* czl_para_node_create(czl_gp *gp, char alloc_flag)
{
    czl_para *p;

    if (alloc_flag)
        p = (czl_para*)CZL_RT_MALLOC(gp, sizeof(czl_para));
    else
        p = (czl_para*)CZL_TMP_MALLOC(gp, sizeof(czl_para));
    if (!p)
        return NULL;

    p->para = gp->tmp->exp_head;
    p->next = NULL;

    gp->tmp->exp_head = NULL;

    return p;
}

void czl_para_node_insert
(
    czl_para **head,
    czl_para **tail,
    czl_para *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

void czl_paras_list_delete(czl_gp *gp, czl_para *p, unsigned char free_flag)
{
    czl_para *q;

    while (p)
    {
        q = p->next;
        czl_exp_stack_delete(gp, p->para);
        if (free_flag)
            CZL_RT_FREE(gp, p, sizeof(czl_para));
        else
            CZL_TMP_FREE(gp, p, sizeof(czl_para));
        p = q;
    }
}

static void czl_paras_list_free(czl_gp *gp, czl_para *p)
{
    czl_para *q;

    while (p)
    {
        q = p->next;
        CZL_TMP_FREE(gp, p, sizeof(czl_para));
        p = q;
    }
}

void czl_expfun_delete(czl_gp *gp, czl_exp_fun *p)
{
    czl_exp_fun *q;

    while (p)
    {
        q = p->next;
        czl_paras_list_delete(gp, p->paras, 1);
        CZL_RT_FREE(gp, p, sizeof(czl_exp_fun));
        p = q;
    }
}
///////////////////////////////////////////////////////////////
czl_para_explain* czl_para_explain_create
(
    czl_gp *gp,
    unsigned short index,
    unsigned char ref_flag
)
{
    czl_para_explain *p = (czl_para_explain*)CZL_RT_MALLOC(gp, sizeof(czl_para_explain));
    if (!p)
        return NULL;

    if (gp->tmp->exp_head)
    {
        switch (gp->tmp->exp_head->kind)
        {
        case CZL_ARRAY_LIST:
            czl_set_array_list_reg(gp, CZL_ARR_LIST(gp->tmp->exp_head->res->val.Obj));
            break;
        case CZL_TABLE_LIST:
            czl_set_table_list_reg(gp, CZL_TAB_LIST(gp->tmp->exp_head->res->val.Obj));
            break;
        default:
            break;
        }
        gp->tmp->exp_head->kind = CZL_REG_VAR;
    }

    p->index = index;
    p->ref_flag = ref_flag;
    p->para = gp->tmp->exp_head;
    p->next = NULL;

    gp->tmp->exp_head = NULL;

    return p;
}

void czl_para_explain_insert
(
    czl_para_explain **head,
    czl_para_explain **tail,
    czl_para_explain *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

void czl_para_explain_list_delete(czl_gp *gp, czl_para_explain *p)
{
    czl_para_explain *q;

    while (p)
    {
        q = p->next;
        czl_exp_stack_delete(gp, p->para);
        CZL_RT_FREE(gp, p, sizeof(czl_para_explain));
        p = q;
    }
}
///////////////////////////////////////////////////////////////
czl_class* czl_class_create(czl_gp *gp, char *name, char final_flag)
{
    unsigned long len = strlen(name);
    czl_class *p = (czl_class*)CZL_RT_MALLOC(gp, sizeof(czl_class));
    if (!p)
        return NULL;

    if (!(p->name=(char*)CZL_RT_MALLOC(gp, len+1)))
    {
        CZL_RT_FREE(gp, p, sizeof(czl_class));
        return NULL;
    }
    strcpy(p->name, name);

    p->vars = NULL;
    p->flag = CZL_NOT_SURE;
    p->final_flag = final_flag;
    p->vars_count = 0;
    p->hash = czl_bkdr_hash(name, len);
    p->enums = NULL;
    p->funs = NULL;
    p->parents = NULL;
    p->parents_count = 0;
    p->vars_hash.size = 0;
    p->vars_hash.count = 0;
    p->vars_hash.buckets = NULL;
    p->funs_hash.size = 0;
    p->funs_hash.count = 0;
    p->funs_hash.buckets = NULL;
    p->next = NULL;

#ifdef CZL_MM_MODULE
    p->pool.type = CZL_MM_UNDEF_SP;
#endif

    return p;
}

czl_class* czl_class_find(czl_gp *gp, char *name)
{
    return (czl_class*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, &gp->class_hash);
}

char czl_class_insert(czl_gp *gp, czl_class *node)
{
    node->next = gp->class_head;
    gp->class_head = node;
    return czl_sys_hash_insert(gp, CZL_STRING, node, &gp->class_hash);
}
///////////////////////////////////////////////////////////////
czl_class_parent* czl_class_parent_node_create
(
    czl_gp *gp,
    czl_class *pclass,
    char permission
)
{
    czl_class_parent *p = (czl_class_parent*)CZL_RT_MALLOC(gp, sizeof(czl_class_parent));
    if (!p)
        return NULL;

    p->permission = permission;
    p->pclass = pclass;
    p->next = NULL;

    return p;
}

void czl_class_parent_node_insert
(
    czl_class_parent **head,
    czl_class_parent **tail,
    czl_class_parent *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

void czl_class_parent_list_delete(czl_gp *gp, czl_class_parent *p)
{
    czl_class_parent *q;

    while (p)
    {
        q = p->next;
        CZL_RT_FREE(gp, p, sizeof(czl_class_parent));
        p = q;
    }
}
///////////////////////////////////////////////////////////////
void czl_ref_break(czl_gp *gp, czl_ref_var *ref, czl_var *obj)
{
    czl_ref_obj *ro = (czl_ref_obj*)obj->name;

    if (ref->last)
        ref->last->next = ref->next;
    else
        ro->head = ref->next;
    if (ref->next)
        ref->next->last = ref->last;

    if (ro->cnt)
    {
        CZL_ORFD1(ro);
        //对象成员变量引用为空时立即释放指针信息内存以便节省内存空间，
        //普通变量为空时不释放指针信息内存而是缓存指针信息加速执行速度。
        if (!ro->head)
        {
            CZL_TMP_FREE(gp, ro, CZL_RL(ro->cnt));
            obj->name = NULL;
        }
    }
}

//该函数需要在czl_val_del前调用，否则会导致obj的值czl_val_copy出错
void czl_ref_obj_delete(czl_gp *gp, czl_var *obj)
{
    czl_ref_obj *ro = (czl_ref_obj*)obj->name;
	czl_ref_var *p = ro->head, *q;

    if (ro->cnt)
        CZL_ORFD1(ro);

    while (p)
    {
        q = p->next;
        p->var->name = NULL;
        czl_val_copy(gp, p->var, obj);
        CZL_REF_FREE(gp, p);
        p = q;
    }

    CZL_TMP_FREE(gp, ro, CZL_RL(ro->cnt));
    obj->name = NULL;
}

char czl_ref_obj_create
(
    czl_gp *gp,
    czl_var *var,
    czl_var *obj,
    czl_var **objs,
    unsigned long cnt
)
{
    unsigned long i, count = 0;
    czl_ref_obj *node;
    void ***tmp;

    if (var->name && CZL_LOCK_ELE == var->quality)
        count = ((czl_ref_obj*)var->name)->cnt;

    if (!(node=(czl_ref_obj*)CZL_TMP_MALLOC(gp, CZL_RL(cnt+count))))
        return 0;

    obj->name = (char*)node;
    node->head = NULL;
    node->cnt = cnt + count;
    tmp = node->objs + count;

    if (count)
        memcpy(node->objs, ((czl_ref_obj*)var->name)->objs, count*sizeof(void*));

    for (i = 0; i < cnt; ++i)
        tmp[i] = objs[i]->val.Obj;

    return 1;
}
///////////////////////////////////////////////////////////////
void czl_file_delete(czl_gp *gp, void **obj)
{
    fclose(CZL_FIL(obj)->fp);
    CZL_FILE_FREE(gp, obj);
}
///////////////////////////////////////////////////////////////
char czl_ins_free(czl_gp *gp, void **obj, unsigned long cnt)
{
    unsigned long i;
    czl_ins *ins = CZL_INS(obj);
    czl_var *p = CZL_GIV(ins);
    for (i = 0; i < cnt; ++i, ++p)
    {
        if (CZL_OBJ_IS_LOCK(p))
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_LOCK;
            return 0;
        }
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, p);
        if (!czl_val_del(gp, p))
            return 0;
        p->type = CZL_INT;
    }

    CZL_IF(gp, obj, ins);
    return 1;
}

char czl_ins_delete(czl_gp *gp, void **obj)
{
    czl_ins *ins = CZL_INS(obj);
    unsigned long i, j = ins->pclass->parents_count;
    for (i = 0; i < j; i++)
        if (!czl_ins_delete(gp, ins->parents[i]))
            return 0;

    return czl_ins_free(gp, obj, ins->pclass->vars_count);
}

char czl_instance_delete(czl_gp *gp, void **obj)
{
    if (!czl_ins_delete(gp, obj))
    {
        CZL_INS(obj)->rc = 1;
        return 0;
    }

    return 1;
}

static void** czl_ins_create(czl_gp *gp, czl_class *pclass)
{
    czl_ins *ins;
    void **obj = (void**)czl_malloc(gp, CZL_IL(pclass->parents_count,
                                               pclass->vars_count)
                                    #ifdef CZL_MM_MODULE
                                    , &pclass->pool
                                    #endif
                                    );
    if (!obj)
        return NULL;

    ins = CZL_INS(obj);
    ins->rc = 1;
    ins->rf = 0;
    //
    ins->pclass = pclass;
    if (pclass->parents_count)
        memset(ins->parents, 0, pclass->parents_count*sizeof(czl_ins*));

    return obj;
}

static void** czl_ins_create_by_instance
(
    czl_gp *gp,
    czl_var *p,
    czl_class *pclass
)
{
	czl_var *q;
    unsigned long i = 0;
    unsigned long vars_cnt = pclass->vars_count;
    czl_ins *ins;
    void **obj = czl_ins_create(gp, pclass);
    if (!obj)
        return NULL;

    ins = CZL_INS(obj);
    for (q = CZL_GIV(ins); i < vars_cnt; ++p, ++q, ++i)
    {
        q->name = NULL;
        q->type = CZL_INT;
        q->quality = CZL_OBJ_ELE;
        q->permission = p->permission;
        q->ot = p->ot;
        if (!czl_val_copy(gp, q, p))
        {
            czl_ins_free(gp, obj, i);
            return NULL;
        }
    }

    return obj;
}

void** czl_instance_new_and_copy
(
    czl_gp *gp,
    czl_ins *ins
)
{
    unsigned long i, j = ins->pclass->parents_count;
    czl_ins *node;
    void **obj = czl_ins_create_by_instance(gp, CZL_GIV(ins), ins->pclass);
    if (!obj)
        return NULL;

    node = CZL_INS(obj);
    for (i = 0; i < j; i++)
    {
        void **tmp = czl_instance_new_and_copy(gp, CZL_INS(ins->parents[i]));
        if (!tmp)
        {
            czl_instance_delete(gp, obj);
            return NULL;
        }
        node->parents[i++] = tmp;
    }

    return obj;
}

static void** czl_ins_create_by_class
(
    czl_gp *gp,
    czl_class_var *p,
    czl_class *pclass,
    char copy_flag
)
{
	czl_var *q;
    czl_ins *ins;
    void **obj = czl_ins_create(gp, pclass);
    if (!obj)
        return NULL;

    ins = CZL_INS(obj);
    for (q = CZL_GIV(ins); p; p = p->next)
    {
        if (p->quality != CZL_DYNAMIC_VAR)
            continue;
        q->name = NULL;
        q->type = CZL_INT;
        q->quality = CZL_OBJ_ELE;
        q->permission = p->permission;
        q->ot = p->ot;
        if (copy_flag)
        {
            if (!czl_val_copy(gp, q, (czl_var*)p))
            {
                czl_ins_free(gp, obj, CZL_GIV(ins) - q);
                return NULL;
            }
        }
        ++q;
    }

    return obj;
}

void** czl_instance_fork
(
    czl_gp *gp,
    czl_class *pclass,
    char copy_flag
)
{
	int i = 0;
    czl_class_parent *p;
    czl_ins *ins;
    void **obj = czl_ins_create_by_class(gp, pclass->vars, pclass, copy_flag);
    if (!obj)
        return NULL;

    ins = CZL_INS(obj);
    for (p = pclass->parents; p; p = p->next)
    {
        void **tmp = czl_instance_fork(gp, p->pclass, copy_flag);
        if (!tmp)
        {
            czl_instance_delete(gp, obj);
            return NULL;
        }
        ins->parents[i++] = tmp;
    }

    return obj;
}
///////////////////////////////////////////////////////////////
czl_table_list* czl_table_list_create(czl_gp *gp)
{
    czl_table_list *p = (czl_table_list*)CZL_RT_MALLOC(gp, sizeof(czl_table_list));
    if (!p)
        return NULL;

    p->quality = (CZL_IN_CONSTANT == gp->tmp->variable_field ?
                  CZL_CONST_VAR : CZL_DYNAMIC_VAR);

    p->paras = NULL;
    p->paras_count = 0;

    return p;
}

czl_table_node* czl_table_node_create
(
    czl_gp *gp,
    czl_exp_stack key,
    czl_exp_stack val
)
{
    czl_table_node *p = (czl_table_node*)CZL_RT_MALLOC(gp, sizeof(czl_table_node));
    if (!p)
        return NULL;

    p->key = key;
    p->val = val;
    p->next = NULL;

    return p;
}

void czl_table_node_insert
(
    czl_table_node **head,
    czl_table_node **tail,
    czl_table_node *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

void czl_table_list_delete(czl_gp *gp, czl_table_list *list)
{
    czl_table_node *p = list->paras, *q;

    while (p)
    {
        q = p->next;
        czl_exp_stack_delete(gp, p->key);
        czl_exp_stack_delete(gp, p->val);
        CZL_RT_FREE(gp, p, sizeof(czl_table_node));
        p = q;
    }

    CZL_RT_FREE(gp, list, sizeof(czl_table_list));
}

char czl_hash_init(czl_gp *gp, czl_table *table, unsigned long size)
{
    if (0 == size)
        size = 1;

    table->buckets = (czl_tabkv**)CZL_TMP_CALLOC(gp, size, sizeof(czl_tabkv*));
    if (!table->buckets)
        return 0;

    table->size = size;
    table->mask = -size;

    return 1;
}

void** czl_table_create
(
    czl_gp *gp,
    unsigned long key,
    unsigned long attack_cnt
)
{
    czl_table *tab;
    void **obj = (void**)CZL_TAB_MALLOC(gp);
    if (!obj)
        return NULL;

    tab = CZL_TAB(obj);

#ifdef CZL_MM_MODULE
    czl_mm_pool_init(&tab->pool, sizeof(czl_tabkv), CZL_MM_VAR_SP);
    tab->pool.obj = obj;
#endif

    tab->rc = 1;
    tab->rf = 0;
    //
    tab->buckets = NULL;
    tab->size = 0;
    tab->count = 0;
    tab->key = key;
    tab->attack_cnt = attack_cnt;
    tab->eles_head = NULL;

    return obj;
}

void** czl_empty_table_create(czl_gp *gp)
{
    void **obj = czl_table_create(gp, 0, 0);
    if (!obj)
        return NULL;

    if (!czl_hash_init(gp, CZL_TAB(obj), 0))
    {
        czl_table_delete(gp, obj);
        return NULL;
    }

    return obj;
}

#ifdef CZL_MM_MODULE
char czl_table_delete(czl_gp *gp, void **obj)
{
    czl_table *tab = CZL_TAB(obj);
    czl_tabkv *p;

    for (p = tab->eles_head; p; p = p->next)
    {
        if (CZL_OBJ_IS_LOCK(p))
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_LOCK;
            tab->rc = 1;
            return 0;
        }
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, (czl_var*)p);
        if (!czl_val_del(gp, (czl_var*)p))
            return 0;
        if (CZL_STRING == p->kt)
            CZL_KEY_SRCD1(gp, p->key.str.Obj);
        p->type = CZL_INT;
    }

    czl_mm_sp_destroy(gp, &tab->pool);
    CZL_TMP_FREE(gp, tab->buckets, tab->size*sizeof(czl_tabkv*));
    CZL_TAB_FREE(gp, obj);
    return 1;
}
#else
char czl_table_delete(czl_gp *gp, void **obj)
{
    czl_table *tab = CZL_TAB(obj);
    czl_tabkv *p, *q;
    for (p = tab->eles_head; p; p = q)
    {
        q = p->next;
        if (CZL_OBJ_IS_LOCK(p))
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_LOCK;
            p->last = NULL;
            tab->eles_head = p;
            tab->rc = 1;
            return 0;
        }
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, (czl_var*)p);
        if (!czl_val_del(gp, (czl_var*)p))
            return 0;
        if (CZL_STRING == p->kt)
            CZL_KEY_SRCD1(gp, p->key.str.Obj);
        czl_free(gp, p, sizeof(czl_tabkv));
    }

    CZL_TMP_FREE(gp, tab->buckets, tab->size*sizeof(czl_tabkv*));
    CZL_TAB_FREE(gp, obj);
    return 1;
}
#endif //#ifdef CZL_MM_MODULE

static char czl_table_eles_new_by_list
(
    czl_gp *gp,
    czl_table *tab,
    const czl_table_node *p
)
{
    while (p)
    {
        czl_tabkv *ele;
        czl_var *key = czl_exp_stack_cac(gp, p->key);
        if (!key)
            return 0;
        if (!(ele=czl_create_tabkv(gp, tab, key, NULL)) ||
            !czl_val_copy(gp, (czl_var*)ele, czl_exp_stack_cac(gp, p->val)))
            return 0;
        p = p->next;
    }

    return 1;
}

char czl_table_new
(
    czl_gp *gp,
    const czl_table_list *list,
    czl_var *res
)
{
    unsigned long paras_count = (list ? list->paras_count : 0);
    void **obj = czl_table_create(gp, 0, 0);
    if (!obj)
        return 0;

    if (!czl_hash_init(gp, CZL_TAB(obj), paras_count) ||
        (list && !czl_table_eles_new_by_list(gp, CZL_TAB(obj), list->paras)))
    {
        czl_table_delete(gp, obj);
        return 0;
    }

    res->type = CZL_TABLE;
    res->val.Obj = obj;
    return 1;
}

static czl_tabkv* czl_table_ele_create(czl_gp *gp, czl_table *tab, czl_tabkv *p)
{
    czl_tabkv *ele = (CZL_STRING == p->kt ?
                      czl_create_str_tabkv(gp, tab, p->key.str.size,
                                           -1, (char*)p->key.str.Obj) :
                      czl_create_num_tabkv(gp, tab, p->key.inum));
    if (!ele || !czl_val_copy(gp, (czl_var*)ele, (czl_var*)p))
        return NULL;

    return ele;
}

static char czl_table_eles_new_by_table
(
    czl_gp *gp,
    czl_table *node,
    const czl_table *table
)
{
    czl_tabkv *p;
    czl_tabkv *ele;

    for (p = table->eles_head; p; p = p->next)
    {
        if (!(ele=czl_table_ele_create(gp, node, p)))
            return 0;
        if (p == table->foreach_inx)
            node->foreach_inx = ele;
    }

    return 1;
}

void** czl_table_new_by_table(czl_gp *gp, czl_table *tab)
{
    czl_table *node;
    void **obj = czl_table_create(gp, tab->key, tab->attack_cnt);
    if (!obj)
        return NULL;

    node = CZL_TAB(obj);

    if (!czl_hash_init(gp, node, tab->size) ||
        !czl_table_eles_new_by_table(gp, node, tab))
    {
        czl_table_delete(gp, obj);
        return NULL;
    }

    return obj;
}

char czl_table_union_by_table(czl_gp *gp, czl_table *tab, czl_tabkv *p)
{
    while (p)
    {
        if (!czl_find_tabkv(tab, (czl_var*)p) && !czl_table_ele_create(gp, tab, p))
            return 0;
        p = p->next;
    }

    return 1;
}

char czl_table_union_by_list(czl_gp *gp, czl_table *tab, czl_table_node *p)
{
    while (p)
    {
        czl_var *key = czl_exp_stack_cac(gp, p->key);
        if (!key)
            return 0;
        if (!czl_find_tabkv(tab, key))
        {
            czl_tabkv *ele;
            if (!(ele=czl_create_tabkv(gp, tab, key, NULL)) ||
                !czl_val_copy(gp, (czl_var*)ele, czl_exp_stack_cac(gp, p->val)))
                return 0;
        }
        p = p->next;
    }

    return 1;
}

void czl_table_mix_by_table(czl_gp *gp, czl_value *tab, czl_tabkv *p)
{
    while (p)
    {
        czl_delete_tabkv(gp, tab, (czl_var*)p);
        p = p->next;
    }
}

void czl_table_mix_by_array(czl_gp *gp,  czl_value *tab, czl_array *arr)
{
    unsigned long i;
    for (i = 0; i < arr->cnt; ++i)
        czl_delete_tabkv(gp, tab, arr->vars+i);
}

void czl_table_mix_by_sq(czl_gp *gp,  czl_value *tab, czl_glo_var *p)
{
    while (p)
    {
        czl_delete_tabkv(gp, tab, (czl_var*)p);
        p = p->next;
    }
}

char czl_table_mix_by_list(czl_gp *gp,  czl_value *tab, czl_para *p)
{
    while (p)
    {
        czl_var *key = czl_exp_stack_cac(gp, p->para);
        if (!key)
            return 0;
        czl_delete_tabkv(gp, tab, key);
        p = p->next;
    }

    return 1;
}
///////////////////////////////////////////////////////////////
char czl_str_create
(
    czl_gp *gp,
    czl_str *str,
    unsigned long size,
    unsigned long len,
    const void *init
)
{
    czl_string *s;
    if (!(str->Obj=(void**)CZL_STR_MALLOC(gp, CZL_SL(size))))
        return 0;

    s = CZL_STR(str->Obj);
    s->rc = 1;
    s->len = len;
    s->str[len] = '\0';
    str->size = size;

    if (init && len > 0)
        memcpy(s->str, init, len);

    return 1;
}

static czl_long czl_get_exp_number(czl_gp *gp, czl_exp_stack exp)
{
	czl_var *ret;

    if (!exp)
        return 0;

    if (!(ret=czl_exp_stack_cac(gp, exp)))
        return -1;

    switch (ret->type)
    {
    case CZL_INT:
        return ret->val.inum;
    case CZL_FLOAT:
        return ret->val.fnum;
    default:
        return -1;
    }
}

char czl_string_new
(
    czl_gp *gp,
    const czl_new_array *pnew,
    czl_var *res
)
{
	unsigned long len = 0;
	czl_var *init = (czl_var*)pnew->init_list;
    czl_long l = czl_get_exp_number(gp, pnew->len);
    if (l < 0)
    {
        gp->exceptionCode = CZL_EXCEPTION_ARRAY_LENGTH_LESS_ZERO;
        return 0;
    }

    if (init && (len=strlen(CZL_STR(init->val.str.Obj)->str)) > (unsigned long)l)
    {
        res->type = CZL_STRING;
        res->val = init->val;
        CZL_SRCA1(init->val.str);
    }
    else
    {
        czl_str str;
        if (!czl_str_create(gp, &str, l+1, l,
                            (init ? CZL_STR(init->val.str.Obj)->str : NULL)))
            return 0;
        memset(CZL_STR(str.Obj)->str+len, 0, str.size-len);
        res->type = CZL_STRING;
        res->val.str = str;
    }

    return 1;
}

char czl_instance_new
(
    czl_gp *gp,
    const czl_new_ins *pnew,
    czl_var *res
)
{
    if (!pnew->len)
    {
        void **obj = czl_instance_fork(gp, pnew->pclass, 1);
        if (!obj)
            return 0;
        res->type = CZL_INSTANCE;
        res->val.Obj = obj;
        if (pnew->init_fun && !czl_class_fun_run(gp, pnew->init_fun, obj))
        {
            czl_instance_delete(gp, obj);
            return 0;
        }
    }
    else //实例数组
    {
        void **arr;
		czl_var *vars;
		czl_long i;
        czl_long l = czl_get_exp_number(gp, pnew->len);
        if (l < 0)
        {
            gp->exceptionCode = CZL_EXCEPTION_ARRAY_LENGTH_LESS_ZERO;
            return 0;
        }
        if (!(arr=czl_array_create(gp, l, l)))
            return 0;
        vars = CZL_ARR(arr)->vars;
        for (i = 0; i < l; i++)
        {
            void **obj = czl_instance_fork(gp, pnew->pclass, 1);
            if (!obj)
            {
                czl_array_delete(gp, arr);
                return 0;
            }
            vars[i].type = CZL_INSTANCE;
            vars[i].val.Obj = obj;
            if (pnew->init_fun && !czl_class_fun_run(gp, pnew->init_fun, obj))
            {
                czl_array_delete(gp, arr);
                return 0;
            }
        }
        res->type = CZL_ARRAY;
        res->val.Obj = arr;
    }

    return 1;
}
///////////////////////////////////////////////////////////////
czl_array_list* czl_array_list_create(czl_gp *gp)
{
    czl_array_list *p = (czl_array_list*)CZL_RT_MALLOC(gp, sizeof(czl_array_list));
    if (!p)
        return NULL;

    p->quality = (CZL_IN_CONSTANT == gp->tmp->variable_field ?
                  CZL_CONST_VAR : CZL_DYNAMIC_VAR);

    p->paras_count = 0;
    p->paras = NULL;

    return p;
}

void czl_array_list_delete(czl_gp *gp, czl_array_list *p)
{
    if (!p)
        return;

    czl_paras_list_delete(gp, p->paras, 1);
    CZL_RT_FREE(gp, p, sizeof(czl_array_list));
}

void** czl_array_create(czl_gp *gp, unsigned long sum, unsigned long cnt)
{
	unsigned long i;
	czl_var *var;
    czl_array *arr;
    void **obj = (void**)CZL_ARR_MALLOC(gp);
    if (!obj)
        return NULL;

    arr = CZL_ARR(obj);

    arr->rc = 1;
    arr->rf = 0;
    //
    arr->cnt = cnt;
    arr->sum = sum;
    arr->vars = (czl_var*)CZL_TMP_CALLOC(gp, sum, sizeof(czl_var));
    if (!arr->vars && sum)
    {
        CZL_ARR_FREE(gp, obj);
        return NULL;
    }
    
    var = arr->vars;
    for (i = 0; i < sum; i++)
    {
        var->name = NULL;
        var->quality = CZL_OBJ_ELE;
        var->ot = CZL_NIL;
        var->type = CZL_INT;
        var->val.inum = 0;
        var++;
    }

    return obj;
}

char czl_array_delete(czl_gp *gp, void **obj)
{
    unsigned long i;
    czl_array *arr = CZL_ARR(obj);
    czl_var *var = arr->vars;

    for (i = 0; i < arr->cnt; ++i, ++var)
    {
        if (CZL_OBJ_IS_LOCK(var))
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_LOCK;
            arr->rc = 1;
            return 0;
        }
        if (CZL_VAR_EXIST_REF(var))
            czl_ref_obj_delete(gp, var);
        if (!czl_val_del(gp, (czl_var*)var))
            return 0;
        var->type = CZL_INT;
    }

    CZL_TMP_FREE(gp, arr->vars, sizeof(czl_var)*arr->sum);
    CZL_ARR_FREE(gp, obj);
    return 1;
}

char czl_array_vars_new_by_list
(
    czl_gp *gp,
    czl_var *var,
    const czl_para *p
)
{
    while (p)
    {
        if (!czl_val_copy(gp, var++, czl_exp_stack_cac(gp, p->para)))
            return 0;
        p = p->next;
    }

    return 1;
}

char czl_array_vars_new_by_array
(
    czl_gp *gp,
    czl_var *var,
    czl_var *p,
    int num
)
{
    int i;

    for (i = 0; i < num; ++i, ++var, ++p)
    {
        if (!czl_val_copy(gp, var, p))
            return 0;
    }

    return 1;
}

char czl_array_new
(
    czl_gp *gp,
    czl_exp_stack len,
    const czl_array_list *list,
    czl_var *res
)
{
	unsigned long num;
    void **obj;
    czl_long l = czl_get_exp_number(gp, len);
    if (l < 0)
    {
        gp->exceptionCode = CZL_EXCEPTION_ARRAY_LENGTH_LESS_ZERO;
        return 0;
    }

    if (list)
        num = (unsigned long)l > list->paras_count ?
              (unsigned long)l : list->paras_count;
    else
        num = l;

    if (!(obj=czl_array_create(gp, num, num)))
        return 0;

    if (list && !czl_array_vars_new_by_list(gp, CZL_ARR(obj)->vars, list->paras))
    {
        czl_array_delete(gp, obj);
        return 0;
    }

    res->type = CZL_ARRAY;
    res->val.Obj = obj;

    return 1;
}
///////////////////////////////////////////////////////////////
czl_glo_var* czl_sqele_node_create
(
    czl_gp *gp
#ifdef CZL_MM_MODULE
    , czl_mm_sp_pool *pool
#endif
)
{
    czl_glo_var *p = (czl_glo_var*)czl_malloc(gp, sizeof(czl_glo_var)
                                #ifdef CZL_MM_MODULE
                                , pool
                                #endif
                                );
    if (!p)
        return NULL;

    p->name = NULL;
    p->quality = CZL_OBJ_ELE;
    p->ot = CZL_NIL;
    p->type = CZL_INT;
    p->val.inum = 0;
    p->next = NULL;

    return p;
}

void** czl_sq_create(czl_gp *gp, unsigned long num)
{
	unsigned long i;
    czl_glo_var *ele;
    czl_sq *sq;
    void **obj = (void**)CZL_SQ_MALLOC(gp);
    if (!obj)
        return NULL;

    sq = CZL_SQ(obj);

#ifdef CZL_MM_MODULE
    czl_mm_pool_init(&sq->pool, sizeof(czl_glo_var), CZL_MM_BUF_SP);
#endif

    sq->rc = 1;
    sq->rf = 0;
    //
    sq->count = num;
    sq->eles_head = NULL;
    sq->eles_tail = NULL;

    for (i = 0; i < num; i++)
    {
        if (!(ele=czl_sqele_node_create(gp
                                         #ifdef CZL_MM_MODULE
                                         , &sq->pool
                                         #endif
                                         )))
        {
            czl_sq_delete(gp, obj);
            return NULL;
        }
        if (NULL == sq->eles_head)
            sq->eles_head = ele;
        else
            sq->eles_tail->next = ele;
        sq->eles_tail = ele;
    }

    return obj;
}

#ifdef CZL_MM_MODULE
char czl_sq_delete(czl_gp *gp, void **obj)
{
    czl_glo_var *p;
    czl_sq *sq = CZL_SQ(obj);

    for (p = sq->eles_head; p; p = p->next)
    {
        if (CZL_OBJ_IS_LOCK(p))
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_LOCK;
            sq->rc = 1;
            return 0;
        }
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, (czl_var*)p);
        if (!czl_val_del(gp, (czl_var*)p))
            return 0;
        p->type = CZL_INT;
    }

    czl_mm_sp_destroy(gp, &sq->pool);
    CZL_SQ_FREE(gp, obj);
    return 1;
}
#else
char czl_sq_delete(czl_gp *gp, void **obj)
{
    czl_glo_var *p, *q;
    czl_sq *sq = CZL_SQ(obj);

    for (p = sq->eles_head; p; p = q)
    {
        q = p->next;
        if (CZL_OBJ_IS_LOCK(p))
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_LOCK;
            sq->eles_head = p;
            sq->rc = 1;
            return 0;
        }
        if (CZL_VAR_EXIST_REF(p))
            czl_ref_obj_delete(gp, (czl_var*)p);
        if (!czl_val_del(gp, (czl_var*)p))
            return 0;
        czl_free(gp, p, sizeof(czl_glo_var));
    }

    CZL_SQ_FREE(gp, obj);
    return 1;
}
#endif //#ifdef CZL_MM_MODULE

static char czl_sq_eles_new_by_sq
(
    czl_gp *gp,
    czl_sq *node,
    const czl_sq *sq
)
{
    czl_glo_var *p, *ele = node->eles_head;

    for (p = sq->eles_head; p; p = p->next, ele = ele->next)
    {
        if (!czl_val_copy(gp, (czl_var*)ele, (czl_var*)p))
            return 0;
        if (p == sq->foreach_inx)
            node->foreach_inx = ele;
    }

    return 1;
}

void** czl_sq_new_by_sq(czl_gp *gp, const czl_sq *sq)
{
    void **node = czl_sq_create(gp, sq->count);
    if (!node)
        return NULL;

    if (!czl_sq_eles_new_by_sq(gp, CZL_SQ(node), sq))
    {
        czl_sq_delete(gp, node);
        return NULL;
    }

    return node;
}

static char czl_sq_eles_fork
(
    czl_gp *gp,
    czl_glo_var *ele,
    czl_para *p
)
{
    while (p)
    {
        if (!czl_val_copy(gp, (czl_var*)ele, czl_exp_stack_cac(gp, p->para)))
            return 0;
        ele = ele->next;
        p = p->next;
    }

    return 1;
}

void** czl_sq_new
(
    czl_gp *gp,
    czl_exp_stack len,
    const czl_array_list *list
)
{
	unsigned long num;
    void **obj;
    czl_long l = czl_get_exp_number(gp, len);
    if (l < 0)
    {
        gp->exceptionCode = CZL_EXCEPTION_ARRAY_LENGTH_LESS_ZERO;
        return NULL;
    }

    if (list)
        num = (unsigned long)l > list->paras_count ?
              (unsigned long)l : list->paras_count;
    else
        num = l;

    obj = czl_sq_create(gp, num);
    if (!obj || !list)
        return obj;

    if (!czl_sq_eles_fork(gp, CZL_SQ(obj)->eles_head, list->paras))
    {
        czl_sq_delete(gp, obj);
        return NULL;
    }

    return obj;
}
///////////////////////////////////////////////////////////////
czl_new_sentence* czl_new_sentence_create(czl_gp *gp, char type)
{
    czl_new_sentence *p = (czl_new_sentence*)CZL_RT_MALLOC(gp, sizeof(czl_new_sentence));
    if (!p)
        return NULL;

    p->type = type;
    p->new_obj.ins.pclass = NULL;
    p->new_obj.ins.len = NULL;
    p->new_obj.ins.init_fun = NULL;

    return p;
}

void czl_new_sentence_delete(czl_gp *gp, czl_new_sentence *p)
{
    switch (p->type)
    {
    case CZL_INSTANCE:
        czl_exp_stack_delete(gp, p->new_obj.ins.len);
        break;
    case CZL_TABLE:
        czl_table_list_delete(gp, p->new_obj.tab);
        break;
    case CZL_ARRAY: case CZL_STACK: case CZL_QUEUE:
        czl_exp_stack_delete(gp, p->new_obj.arr.len);
        czl_array_list_delete(gp, p->new_obj.arr.init_list);
        break;
    default: //CZL_STRING
        czl_exp_stack_delete(gp, p->new_obj.arr.len);
        break;
    }

    CZL_RT_FREE(gp, p, sizeof(czl_new_sentence));
}

char czl_obj_new
(
    czl_gp *gp,
    czl_new_sentence *New,
    czl_var *res
)
{
    switch (New->type)
    {
    case CZL_INSTANCE:
        return czl_instance_new(gp, &New->new_obj.ins, res);
    case CZL_STRING:
        return czl_string_new(gp, &New->new_obj.arr, res);
    case CZL_TABLE:
        return czl_table_new(gp, New->new_obj.tab, res);
    case CZL_ARRAY:
        return czl_array_new(gp, New->new_obj.arr.len,
                             New->new_obj.arr.init_list, res);
    default: //CZL_STACK/CZL_QUEUE
        if (!(res->val.Obj=czl_sq_new(gp, New->new_obj.arr.len,
                                      New->new_obj.arr.init_list)))
            return 0;
        res->type = New->type;
        return 1;
    }
}
///////////////////////////////////////////////////////////////
static char czl_ins_fork(czl_gp *gp, czl_ins *ins, czl_value *val, char flag)
{
    void **obj = czl_instance_new_and_copy(gp, ins);
    if (!obj)
        return 0;

    if (flag)
        --ins->rc;

    val->Obj = obj;

    return 1;
}

char czl_array_fork(czl_gp *gp, czl_array *arr, czl_value *val, char flag)
{
    void **obj = czl_array_create(gp, arr->sum, arr->cnt);
    if (!obj)
        return 0;

    if (!czl_array_vars_new_by_array(gp, CZL_ARR(obj)->vars, arr->vars, arr->cnt))
    {
        czl_array_delete(gp, obj);
        return 0;
    }

    if (flag)
        --arr->rc;

    val->Obj = obj;

    return 1;
}

char czl_table_fork(czl_gp *gp, czl_table *tab, czl_value *val, char flag)
{
    void **obj = czl_table_new_by_table(gp, tab);
    if (!obj)
        return 0;

    if (flag)
        --tab->rc;

    val->Obj = obj;

    return 1;
}

char czl_sq_fork(czl_gp *gp, czl_sq *sq, czl_value *val, char flag)
{
    void **obj = czl_sq_new_by_sq(gp, sq);
    if (!obj)
        return 0;

    if (flag)
        --sq->rc;

    val->Obj = obj;

    return 1;
}

char czl_obj_fork(czl_gp *gp, czl_var *left, czl_var *right)
{
    czl_var tmp = *right; //tmp 用于处理 a = a.v 情况

    switch (tmp.type)
    {
    case CZL_INSTANCE:
        if (!czl_ins_fork(gp, CZL_INS(tmp.val.Obj), &tmp.val, 0))
            return 0;
        break;
    case CZL_ARRAY:
        if (!czl_array_fork(gp, CZL_ARR(tmp.val.Obj), &tmp.val, 0))
            return 0;
        break;
    case CZL_TABLE:
        if (!czl_table_fork(gp, CZL_TAB(tmp.val.Obj), &tmp.val, 0))
            return 0;
        break;
    default:
        if (!czl_sq_fork(gp, CZL_SQ(tmp.val.Obj), &tmp.val, 0))
            return 0;
        break;
    }

    if (!czl_val_del(gp, left))
    {
        czl_val_del(gp, &tmp);
        return 0;
    }

    left->type = tmp.type;
    left->val = tmp.val;
    return 1;
}

static char czl_string_fork(czl_gp *gp, czl_str *str)
{
    czl_str tmp;
    czl_string *s = CZL_STR(str->Obj);
    if (!czl_str_create(gp, &tmp, str->size, s->len, s->str))
        return 0;

    --s->rc;
    *str = tmp;
    return 1;
}
///////////////////////////////////////////////////////////////
int czl_get_class_num(const czl_class_ptr_vars *p)
{
    int num = 0;

    while (p)
    {
        num++;
        p = p->next;
    }

    return num;
}

char czl_class_index_find
(
    const char *name,
    const czl_class *pclass,
    int *index
)
{
    czl_class_parent *p;

    if (name == pclass->name)
        return 1;

    *index += 1;

    for (p = pclass->parents; p; p = p->next)
        if (czl_class_index_find(name, p->pclass, index))
            return 1;

    return 0;
}

void czl_instance_sort_by_index
(
    int num,
    int *indexs,
    czl_class_ptr_vars **head
)
{
    int t;
    int i, j, k;
    czl_class_ptr_vars *p, *p_last;
    czl_class_ptr_vars *q, *q_last;
    czl_class_ptr_vars *min = NULL, *min_last = NULL;
    czl_class_ptr_vars *tmp;

    p_last = NULL;
    p = *head;
    for (i = 0; i < num; i++)
    {
        k = i;
        q_last = p;
        q = p->next;
        for (j = i+1; j < num; j++)
        {
            if (indexs[j] < indexs[k])
            {
                k = j;
                min = q;
                min_last = q_last;
            }
            q_last = q;
            q = q->next;
        }
        if (k != i)
        {
            t = indexs[i];
            indexs[i] = indexs[k];
            indexs[k] = t;
            //
            if (!p_last)
                *head = min;
            else
                p_last->next = min;
            min_last->next = p;
            tmp = p->next;
            p->next = min->next;
            min->next = tmp;
            //
            p = min;
        }
        p_last = p;
        p = p->next;
    }
}

char czl_class_sort
(
    czl_gp *gp,
    czl_class_ptr_vars **ptr_vars,
    const czl_class *current_class
)
{
	int i;
	int *class_indexs;
    czl_class_ptr_vars *p = *ptr_vars;
    int num = czl_get_class_num(*ptr_vars);
    if (0 == num)
        return 1;

    //构建class_indexs索引数组
    if (!(class_indexs=(int*)CZL_TMP_CALLOC(gp, num, sizeof(int))))
        return 0;
    
    for (i = 0; i < num; i++)
    {
        czl_class_index_find(p->pclass->name,
                             current_class,
                             &class_indexs[i]);
        p = p->next;
    }

    //根据class_indexs索引数组排序ptr_vars链表，这里采用选择排序算法
    czl_instance_sort_by_index(num, class_indexs, ptr_vars);

    //释放class_indexs内存
    CZL_TMP_FREE(gp, class_indexs, num*sizeof(int));

    return 1;
}

char czl_this_vars_class_sort(czl_gp *gp, czl_fun *fun)
{
    if (!gp->tmp->class_ptr_vars_head) return 1;

    fun->class_vars = gp->tmp->class_ptr_vars_head;
    gp->tmp->class_ptr_vars_head = NULL;

    return czl_class_sort(gp, &fun->class_vars, gp->tmp->cur_class);
}

void czl_ins_var_list_delete(czl_gp *gp, czl_ins_var *p)
{
    czl_ins_var *q;

    while (p)
    {
        q = p->next;
        CZL_RT_FREE(gp, p, sizeof(czl_ins_var));
        p = q;
    }
}

void czl_class_ptr_vars_delete(czl_gp *gp, czl_class_ptr_vars *p)
{
    czl_class_ptr_vars *q;

    while (p)
    {
        q = p->next;
        czl_ins_var_list_delete(gp, p->vars);
        CZL_RT_FREE(gp, p, sizeof(czl_class_ptr_vars));
        p = q;
    }
}

czl_var* czl_class_ref_var_node_create(czl_gp *gp)
{
    czl_class_ptr_vars *ptr_vars = gp->tmp->class_ptr_vars_head;
    czl_ins_var *node;

    while (ptr_vars)
    {
        if (gp->tmp->current_var_class == ptr_vars->pclass)
        {
            for (node=ptr_vars->vars; node; node = node->next)
                if (gp->tmp->current_class_var->index == node->index)
                    return (czl_var*)node;
            break;
        }
        ptr_vars = ptr_vars->next;
    }

    if (!ptr_vars)
    {
        if (!(ptr_vars=(czl_class_ptr_vars*)CZL_RT_MALLOC(gp, sizeof(czl_class_ptr_vars))))
            return NULL;
        ptr_vars->vars = NULL;
        ptr_vars->pclass = gp->tmp->current_var_class;
        ptr_vars->next = NULL;
        if (NULL == gp->tmp->class_ptr_vars_head)
            gp->tmp->class_ptr_vars_head = ptr_vars;
        else
            gp->tmp->class_ptr_vars_tail->next = ptr_vars;
        gp->tmp->class_ptr_vars_tail = ptr_vars;
    }

    if (!(node=(czl_ins_var*)CZL_RT_MALLOC(gp, sizeof(czl_ins_var))))
        return NULL;
    node->index = gp->tmp->current_class_var->index;
    node->next = NULL;
    if (NULL == ptr_vars->vars)
        ptr_vars->vars = node;
    else
        gp->tmp->ins_vars_tail->next = node;
    gp->tmp->ins_vars_tail = node;

    return (czl_var*)node;
}
///////////////////////////////////////////////////////////////
czl_obj_member* czl_obj_member_node_create
(
    czl_gp *gp,
    char type,
    void *obj
)
{
    czl_obj_member *p = (czl_obj_member*)CZL_RT_MALLOC(gp, sizeof(czl_obj_member));
    if (!p)
        return NULL;

    p->type = type;
    p->flag = 0;
    p->obj = obj;
    p->index = NULL;

    p->next = gp->member_head;
    gp->member_head = p;

    return p;
}

czl_member_index* czl_ins_index_create
(
    czl_gp *gp,
    char *var_name,
    char *class_name,
    czl_exp_fun *exp_fun
)
{
	unsigned long len;
    czl_member_index *p = (czl_member_index*)CZL_RT_MALLOC(gp, sizeof(czl_member_index));
    if (!p)
        return NULL;

    p->type = CZL_INSTANCE_INDEX;
    p->next = NULL;

    p->index.ins.hash = czl_bkdr_hash(var_name, strlen(var_name));
    p->index.ins.exp_fun = exp_fun;
    p->index.ins.class_name = NULL;

    if ((len=strlen(class_name)))
    {
        p->index.ins.class_name = (char*)CZL_RT_MALLOC(gp, len+1);
        if (!p->index.ins.class_name)
        {
            CZL_RT_FREE(gp, p->index.ins.class_name, len+1);
            return NULL;
        }
        strcpy(p->index.ins.class_name, class_name);
    }

    return p;
}

czl_member_index* czl_arr_index_create
(
    czl_gp *gp,
    czl_exp_ele *exp,
    czl_exp_fun *exp_fun
)
{
    czl_member_index *p = (czl_member_index*)CZL_RT_MALLOC(gp, sizeof(czl_member_index));
    if (!p)
        return NULL;

    p->type = CZL_ARRAY_INDEX;
    p->next = NULL;

    p->index.arr.exp = exp;
    p->index.arr.exp_fun = exp_fun;

    return p;
}

void czl_member_index_node_insert
(
    czl_member_index **head,
    czl_member_index **tail,
    czl_member_index *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

void czl_member_delete(czl_gp *gp, czl_obj_member *p)
{
    czl_obj_member *q;
    czl_member_index *a, *b;

    while (p)
    {
        q = p->next;
        for (a = p->index; a; a = b)
        {
            b = a->next;
            if (CZL_ARRAY_INDEX == a->type)
            {
                czl_exp_stack_delete(gp, a->index.arr.exp);
            }
            else //CZL_INSTANCE_INDEX
            {
                if (a->index.ins.class_name)
                    CZL_RT_FREE(gp, a->index.ins.class_name,
                                strlen(a->index.ins.class_name)+1);
            }
            CZL_RT_FREE(gp, a, sizeof(czl_member_index));
        }
        CZL_RT_FREE(gp, p, sizeof(czl_obj_member));
        p = q;
    }
}

czl_class_var* czl_class_var_find
(
    char *name,
    const czl_class *pclass
)
{
    czl_class_var *var;
    czl_class_parent *p;

    var = (czl_class_var*)czl_sys_hash_find(CZL_STRING, CZL_INT, name, &pclass->vars_hash);
    if (var && CZL_PUBLIC == var->permission)
        return var;

    for (p = pclass->parents; p; p = p->next)
    {
        if ((var=czl_class_var_find(name, p->pclass)))
            return var;
    }

    return NULL;
}

static void* czl_class_var_get
(
    unsigned long hash,
    const czl_sys_hash *table
)
{
    czl_bucket *p;
    long index;

    if (0 == table->count) return NULL;

    CZL_INDEX_CAC(index, hash, table->mask);
    p = table->buckets[index];

    while (p && hash != ((czl_class_var*)p->key)->hash)
        p = p->next;

    return p ? p->key : NULL;
}

static czl_var* czl_ins_var_find
(
    unsigned long hash,
    czl_ins *ins
)
{
    czl_class_var *var = (czl_class_var*)czl_class_var_get(hash, &ins->pclass->vars_hash);

    if (!var || var->permission != CZL_PUBLIC)
        return NULL;

    switch (var->quality)
    {
    case CZL_DYNAMIC_VAR:
        return CZL_GIV(ins) + var->index;
    case CZL_STATIC_VAR:
        return (czl_var*)var;
    default: //CZL_CONST_VAR
        //实例内部enum不可以被外部访问，
        //这样做可以在表达式计算中不用判断操作对象是否是变量
        return NULL;
    }
}

static czl_var* czl_member_var_find
(
    unsigned long hash,
    czl_ins *ins
)
{
    void ***parents;
	czl_class_parent *p;
    czl_var *var = czl_ins_var_find(hash, ins);
    if (var)
        return var;

    parents = ins->parents;
    for (p = ins->pclass->parents; p; p = p->next)
    {
        if (CZL_PUBLIC == p->permission)
        {
            if ((var=czl_member_var_find(hash, CZL_INS(*parents))))
                return var;
        }
        parents++;
    }

    return NULL;
}

static void* czl_class_fun_find
(
    unsigned long hash,
    const czl_sys_hash *table
)
{
    long index;
    czl_bucket *p;

    if (0 == table->count) return NULL;

    CZL_INDEX_CAC(index, hash, table->mask);
    p = table->buckets[index];

    while (p && hash != ((czl_fun*)p->key)->hash)
        p = p->next;

    return p ? p->key : NULL;
}

czl_fun* czl_member_fun_find
(
    unsigned long hash,
    const czl_class *pclass
)
{
    czl_class_parent *p;
    czl_fun *fun = (czl_fun*)czl_class_fun_find(hash, &pclass->funs_hash);
    if (fun && CZL_PUBLIC == fun->permission)
        return fun;

    for (p = pclass->parents; p; p = p->next)
        if (CZL_PUBLIC == p->permission)
            if ((fun=czl_member_fun_find(hash, p->pclass)))
                return fun;

    return NULL;
}

czl_class* czl_if_class_parent_repeat
(
    const char *name,
    czl_class *pclass
)
{
    czl_class *tmp;
    czl_class_parent *p;

    if (strcmp(name, pclass->name) == 0)
        return pclass;

    for (p = pclass->parents; p; p = p->next)
        if ((tmp=czl_if_class_parent_repeat(name, p->pclass)))
            return tmp;

    return NULL;
}

static czl_class* czl_class_parent_find
(
    const char *name,
    czl_class *pclass
)
{
    czl_class *tmp;
    czl_class_parent *p;

    if (strcmp(name, pclass->name) == 0)
        return pclass;

    for (p = pclass->parents; p; p = p->next)
        if (CZL_PUBLIC == p->permission &&
            (tmp=czl_class_parent_find(name, p->pclass)))
            return tmp;

    return NULL;
}

static czl_fun* czl_sure_class_fun_find
(
    unsigned long hash,
    char *class_name,
    czl_class *pclass
)
{
	czl_fun *fun;

    if (!(pclass=czl_class_parent_find(class_name, pclass)))
        return NULL;

    fun = (czl_fun*)czl_class_fun_find(hash, &pclass->funs_hash);
    if (fun && CZL_PUBLIC == fun->permission)
        return fun;

    return NULL;
}

static czl_ins* czl_instance_parent_find
(
    const char *name,
    czl_ins *ins
)
{
    czl_ins *tmp;
    void ***parents;
    czl_class_parent *p;

    if (strcmp(name, ins->pclass->name) == 0)
        return ins;

    parents = ins->parents;
    for (p = ins->pclass->parents; p; p = p->next)
    {
        if (CZL_PUBLIC == p->permission)
        {
            tmp = czl_instance_parent_find(name, CZL_INS(*parents));
            if (tmp)
                return tmp;
        }
        parents++;
    }

    return NULL;
}

static czl_var* czl_sure_class_var_find
(
    unsigned long hash,
    const char *class_name,
    czl_ins *instance
)
{
    if (!(instance=czl_instance_parent_find(class_name, instance)))
        return NULL;

    return czl_ins_var_find(hash, instance);
}

static czl_var* czl_get_ins_member_res
(
    czl_gp *gp,
    const czl_instance_index *inx,
    void **obj
)
{
    czl_ins *ins = CZL_INS(obj);

    if (inx->exp_fun)
    {
        czl_fun *fun;
        czl_exp_fun *exp_fun = inx->exp_fun;
        if (!inx->class_name)
            fun = czl_member_fun_find(inx->hash, ins->pclass);
        else
            fun = czl_sure_class_fun_find(inx->hash, inx->class_name, ins->pclass);
        if (fun)
        {
            exp_fun->quality = CZL_INS_STATIC_FUN;
            exp_fun->fun = fun;
        }
        else
        {
            czl_var *var;
            if (!inx->class_name)
                var = czl_member_var_find(inx->hash, ins);
            else
                var = czl_sure_class_var_find(inx->hash, inx->class_name, ins);
            if (!var)
            {
                gp->exceptionCode = CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND;
                return NULL;
            }
            exp_fun->quality = CZL_LG_VAR_DYNAMIC_FUN;
            exp_fun->fun = (czl_fun*)var;
        }
        return czl_class_fun_run(gp, exp_fun, obj);
    }
    else
    {
        czl_var *var;
        if (!inx->class_name)
            var = czl_member_var_find(inx->hash, ins);
        else
            var = czl_sure_class_var_find(inx->hash, inx->class_name, ins);
        if (!var)
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND;
        }
        return var;
    }
}

char czl_array_resize(czl_gp *gp, czl_array *arr, unsigned long sum)
{
	unsigned long j = arr->sum;
    czl_var* tmp = (czl_var*)CZL_TMP_REALLOC(gp, arr->vars,
                                   sum*2*sizeof(czl_var),
                                   arr->sum*sizeof(czl_var));
    if (!tmp)
        return 0;

    arr->cnt = sum;
    arr->sum = sum*2;
    arr->vars = tmp;
    while (j < arr->sum)
    {
        czl_var *var = tmp + j++;
        var->name = NULL;
        var->quality = CZL_OBJ_ELE;
        var->ot = CZL_NIL;
        var->type = CZL_INT;
        var->val.inum = 0;
    }

    return 1;
}

static czl_var* czl_arr_tab_exp_fun_run
(
    czl_gp *gp,
    czl_exp_fun *exp_fun,
    czl_fun *fun
)
{
    exp_fun->quality = CZL_LG_VAR_DYNAMIC_FUN;
    exp_fun->fun = fun;
    return czl_exp_fun_run(gp, exp_fun);
}

static czl_var* czl_get_arr_member_res
(
    czl_gp *gp,
    const czl_member_index *inx,
    czl_var *obj,
    unsigned char flag
)
{
    czl_array *arr;
    czl_long i = czl_get_exp_number(gp, inx->index.arr.exp);
    if (i < 0)
    {
        gp->exceptionCode = CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND;
        return NULL;
    }

    //防止czl_get_exp_number后obj的arr地址发生改变
    arr = CZL_ARR(obj->val.Obj);

    if ((unsigned long)i >= arr->cnt)
    {
        if (flag != CZL_ASS)
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND;
            return NULL;
        }
        if ((unsigned long)i+1 > arr->sum &&
            !czl_array_resize(gp, arr, 1+(i > 2*arr->cnt ? i/2 : arr->cnt)))
            return NULL;
        arr->cnt = i+1;
    }

    if (inx->index.arr.exp_fun)
        return czl_arr_tab_exp_fun_run(gp, inx->index.arr.exp_fun,
                                       (czl_fun*)(arr->vars+i));
    else
    {
        if (CZL_REF_VAR == flag && !inx->next)
        {
            gp->tmp_var.quality = CZL_REF_ELE;
            gp->tmp_var.name = (char*)arr;
            gp->tmp_var.val.inum = i;
        }
        return arr->vars+i;
    }
}

static czl_var* czl_get_tab_member_res
(
    czl_gp *gp,
    const czl_array_index *inx,
    czl_var *obj,
    unsigned char flag
)
{
	czl_tabkv *ele;
    czl_table *tab;
    unsigned long count;
    czl_var *res = czl_exp_stack_cac(gp, inx->exp);
    if (!res)
        return NULL;

    //防止czl_exp_stack_cac后obj的tab地址或count发生改变
    tab = CZL_TAB(obj->val.Obj);
    count = tab->count;

    if (!(ele=czl_create_tabkv(gp, tab, res, NULL)))
        return NULL;

    if (inx->exp_fun)
        return czl_arr_tab_exp_fun_run(gp, inx->exp_fun, (czl_fun*)ele);
    else
    {
        if (count != tab->count && flag != CZL_ASS)
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND;
            czl_delete_tabkv(gp, &obj->val, res);
            return NULL;
        }
        return (czl_var*)ele;
    }
}

static void czl_set_char(czl_gp *gp)
{
    czl_char_var *p = gp->ch_head;
    *p->ch = p->val.inum;

    if (p->next)
    {
        gp->ch_head = p->next;
        CZL_STACK_FREE(gp, p, sizeof(czl_char_var));
    }
    else
    {
        //重用最后一个节点避免频繁内存申请释放
        p->quality = CZL_DYNAMIC_VAR;
    }
}

static czl_var* czl_get_char(czl_gp *gp, char *ch)
{
	czl_char_var *p;

    if (gp->ch_head && CZL_DYNAMIC_VAR == gp->ch_head->quality)
    {
        gp->ch_head->quality = CZL_STR_ELE;
        gp->ch_head->val.inum = *ch;
        gp->ch_head->ch = ch;
        return (czl_var*)gp->ch_head;
    }

    if (!(p=(czl_char_var*)CZL_STACK_MALLOC(gp, sizeof(czl_char_var))))
        return NULL;

    p->quality = CZL_STR_ELE;
    p->type = p->ot = CZL_INT;
    p->val.inum = *ch;
    p->ch = ch;
    p->next = gp->ch_head;
    gp->ch_head = p;

    return (czl_var*)p;
}

static czl_var* czl_get_str_member_res
(
    czl_gp *gp,
    const czl_member_index *inx,
    czl_str *str,
    unsigned char flag
)
{
    czl_string *s = CZL_STR(str->Obj);
    czl_long i = czl_get_exp_number(gp, inx->index.arr.exp);
    if (i < 0)
    {
        gp->exceptionCode = CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND;
        return NULL;
    }

    if ((unsigned long)i >= s->len)
    {
        if (flag != CZL_ASS)
        {
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND;
            return NULL;
        }
        if ((unsigned long)i+1 >= str->size)
        {
            unsigned long size = (i >= 2*s->len ? i+2 : (s->len+1)*2);
            void **obj = CZL_SR(gp, (*str), size);
            if (!obj)
                return NULL;
            s = CZL_STR(obj);
            memset(s->str + str->size, 0, size - str->size);
            str->Obj = obj;
            str->size = size;
        }
        s->len = i+1;
    }

    if (inx->index.arr.exp_fun)
    {
        gp->exceptionCode = CZL_EXCEPTION_FUNCTION_CALL_NO_FUN_PTR;
        return NULL;
    }
    else
    {
        if (flag)
            return czl_get_char(gp, s->str+i);
        else
        {
            gp->tmp_var.quality = CZL_STR_ELE;
            gp->tmp_var.type = CZL_INT;
            gp->tmp_var.val.inum = s->str[i];
            return &gp->tmp_var;
        }
    }
}

static char czl_circle_ref_obj_fork(czl_gp *gp, czl_var *var, czl_var *tmp)
{
    switch (var->type)
    {
    case CZL_INSTANCE:
        if (!czl_ins_fork(gp, CZL_INS(var->val.Obj), &tmp->val, 0))
            return 0;
        break;
    case CZL_ARRAY:
        if (!czl_array_fork(gp, CZL_ARR(var->val.Obj), &tmp->val, 0))
            return 0;
        break;
    default: //CZL_TABLE
        if (!czl_table_fork(gp, CZL_TAB(var->val.Obj), &tmp->val, 0))
            return 0;
        break;
    }

    tmp->quality = CZL_CIRCLE_REF_VAR;
    tmp->type = var->type;
    return 1;
}

static unsigned char czl_circle_ref_obj_check
(
    czl_gp *gp,
    czl_var *obj,
    czl_var *var,
    czl_var *tmp
)
{
    unsigned long i;
    unsigned char circle_ref_flag = 1;
    czl_ref_obj *ele = (czl_ref_obj*)obj->name;
    for (i = 0; i < ele->cnt; ++i)
    {
        obj = ((czl_var**)ele->objs)[i];
        if (1 == circle_ref_flag && var->val.Obj == obj->val.Obj)
        {
            circle_ref_flag = 2;
            if (!czl_circle_ref_obj_fork(gp, var, tmp))
                return 0;
        }
        switch (obj->type)
        {
        case CZL_INSTANCE:
            if (CZL_INS(obj->val.Obj)->rc > 1 &&
                 !czl_ins_fork(gp, CZL_INS(obj->val.Obj), &obj->val, 1))
                goto CZL_ERROR_END;
            break;
        case CZL_ARRAY:
            if (CZL_ARR(obj->val.Obj)->rc > 1 &&
                 !czl_array_fork(gp, CZL_ARR(obj->val.Obj), &obj->val, 1))
                goto CZL_ERROR_END;
            break;
        default: //CZL_TABLE
            if (CZL_TAB(obj->val.Obj)->rc > 1 &&
                 !czl_table_fork(gp, CZL_TAB(obj->val.Obj), &obj->val, 1))
                goto CZL_ERROR_END;
            break;
        }
    }

    return circle_ref_flag;

CZL_ERROR_END:
    if (2 == circle_ref_flag)
    {
        czl_val_del(gp, tmp);
        tmp->quality = CZL_DYNAMIC_VAR;
    }
    return 0;
}

static czl_var* czl_get_member_res(czl_gp *gp, const czl_obj_member *member)
{
	czl_var *var;
    czl_var tmp;
    unsigned char circle_ref_flag = 0;
    unsigned long i = 0, cnt = 0;
	czl_member_index *inx;
    czl_var *objs[CZL_MAX_MEMBER_INDEX_LAYER];
    unsigned char quality[CZL_MAX_MEMBER_INDEX_LAYER];

    czl_var *obj = member->obj;
    if (CZL_INS_VAR == member->type)
        obj = (CZL_OBJ_REF == ((czl_ins_var*)obj)->var->type ?
               CZL_GRV(((czl_ins_var*)obj)->var) :
               ((czl_ins_var*)obj)->var);
    else if (CZL_OBJ_REF == obj->type)
        obj = CZL_GRV(obj);

    var = (CZL_REF_VAR == member->flag ? obj : gp->cur_var);

    if (CZL_ASS == member->flag &&
        (CZL_INSTANCE == var->type ||
         CZL_ARRAY == var->type ||
         CZL_TABLE == var->type))
    {
        if (!obj->name)
            circle_ref_flag = 1;
        else if (!(circle_ref_flag=czl_circle_ref_obj_check(gp, obj, var, &tmp)))
            return NULL;
    }

    inx = member->index;
    do {
        quality[cnt] = obj->quality;
        CZL_LOCK_OBJ(obj);
        objs[cnt++] = obj;
        if (1 == circle_ref_flag && var->val.Obj == obj->val.Obj)
        {
            if (!czl_circle_ref_obj_fork(gp, var, &tmp))
                goto CZL_ERROR_END;
            circle_ref_flag = 2;
        }
        switch (obj->type)
        {
        case CZL_INSTANCE:
            if (member->flag &&
                CZL_INS(obj->val.Obj)->rc > 1 &&
                 !czl_ins_fork(gp, CZL_INS(obj->val.Obj), &obj->val, 1))
                goto CZL_ERROR_END;
            if (inx->type != CZL_INSTANCE_INDEX ||
                !(obj=czl_get_ins_member_res(gp, &inx->index.ins, obj->val.Obj)))
                goto CZL_ERROR_END;
            break;
        case CZL_ARRAY:
            if (member->flag &&
                CZL_ARR(obj->val.Obj)->rc > 1 &&
                 !czl_array_fork(gp, CZL_ARR(obj->val.Obj), &obj->val, 1))
                goto CZL_ERROR_END;
            if (inx->type != CZL_ARRAY_INDEX ||
                !(obj=czl_get_arr_member_res(gp, inx, obj, member->flag)))
                goto CZL_ERROR_END;
            break;
        case CZL_TABLE:
            if (member->flag &&
                CZL_TAB(obj->val.Obj)->rc > 1 &&
                 !czl_table_fork(gp, CZL_TAB(obj->val.Obj), &obj->val, 1))
                goto CZL_ERROR_END;
            if (inx->type != CZL_ARRAY_INDEX ||
                !(obj=czl_get_tab_member_res(gp, &inx->index.arr, obj, member->flag)))
                goto CZL_ERROR_END;
            break;
        case CZL_STRING:
            if (CZL_REF_VAR == member->flag)
                goto CZL_ERROR_END;
            if (member->flag &&
                CZL_STR(obj->val.str.Obj)->rc > 1 &&
                 !czl_string_fork(gp, &obj->val.str))
                goto CZL_ERROR_END;
            if (inx->type != CZL_ARRAY_INDEX ||
                !(obj=czl_get_str_member_res(gp, inx, &obj->val.str, member->flag)))
                goto CZL_ERROR_END;
            break;
        default:
            gp->exceptionCode = CZL_EXCEPTION_OBJECT_TYPE_NOT_MATCH;
            goto CZL_ERROR_END;
        }
    } while ((inx=inx->next));

    if (2 == circle_ref_flag)
        gp->tmp_var = tmp;
    else if (CZL_REF_VAR == member->flag)
    {
        if (!obj->name && !czl_ref_obj_create(gp, var, obj, objs, cnt))
            return NULL;
        if (CZL_ARRAY == objs[cnt-1]->type)
            obj = &gp->tmp_var;
    }

    CZL_UNLOCK_OBJS(objs, quality, i, cnt);
    return obj;

CZL_ERROR_END:
    if (2 == circle_ref_flag)
        czl_val_del(gp, &tmp);
    CZL_UNLOCK_OBJS(objs, quality, i, cnt);
    return NULL;
}
///////////////////////////////////////////////////////////////
czl_exp_ele* czl_opr_create(czl_gp *gp, czl_exp_node *node)
{
    czl_exp_ele *p = (czl_exp_ele*)CZL_RT_MALLOC(gp, sizeof(czl_exp_ele));
    if (!p)
        return NULL;

    p->flag = CZL_OPERAND;
    p->kind = node->type;
    p->res = (czl_var*)node->op.obj;
    p->lt = CZL_OPERAND; //用于区别条件运算符&&、||和三目运算符
    p->pl.msg.line = gp->error_line;
    p->pl.msg.cnt = 1;
    p->next = NULL;

    return p;
}

char* czl_get_sysfun_para_ot(czl_gp *gp, char *e, char *ot)
{
    //int_v1=2,arr&v2
    char *tmp = e;
    int i = 0;
    char t[16];
    while (*e >= 'a' && *e <= 'z')
    {
        t[i++] = *e++;
        if (i >= 16)
            return tmp;
    }
    t[i] = '\0';

    if (0 == i)
        *ot = CZL_NIL;
    else
    {
        *ot = czl_get_para_ot(gp, t);
        if (CZL_NIL == *ot)
            return tmp;
    }

    return e;
}

char czl_sysfun_default_para_type_check(char ot, unsigned char type)
{
    if (CZL_NIL == ot)
        return 1;

    switch (type)
    {
    case CZL_INT: case CZL_FLOAT:
        switch (ot) {
        case CZL_INT: case CZL_FLOAT: case CZL_NUM: return 1;
        default: return 0;
        }
    default: //CZL_STRING
        return (CZL_STRING == ot ? 1 : 0);
    }
}

static char czl_build_sys_fun_paras_explain
(
    czl_gp *gp,
    czl_fun *fun,
    char *e
)
{
    czl_para_explain *tail = NULL, *node;
    czl_exp_node *constant;
    czl_var inx;
    char ot;
    char sign;
    char ref_flag;
    short index = 0;
    short cnt = 0;

    while (*e)
    {
        if (cnt == fun->enter_vars_count)
            return 0;

        ref_flag = ('&' == *e ? *e++ : 0);

        e = czl_get_sysfun_para_ot(gp, e, &ot);
        if (ot != CZL_NIL && *e++ != '_')
            return 0;

        if (*e++ != 'v')
            return 0;
        e = czl_get_number_from_str(e, &inx);
        if (!e ||
            inx.type != CZL_INT ||
            inx.val.inum <= index ||
            inx.val.inum > fun->enter_vars_count)
            return 0;

        fun->vars[inx.val.inum-1].ot = ot;
        fun->vars[inx.val.inum-1].type = CZL_INT;

        if ('=' == *e)
        {
			czl_var *var;
            if ('-' == *(++e))
            {
                sign = 1;
                e++;
            }
            else
                sign = 0;
            if (!czl_is_constant(gp, &e, &constant) || !constant)
                return 0;
            var = (czl_var*)constant->op.obj;
            if (!czl_sysfun_default_para_type_check(ot, var->type))
                return 0;
            if (sign)
            {
                czl_value val;
                switch (var->type)
                {
                case CZL_INT:
                    val.inum = -var->val.inum;
                    break;
                case CZL_FLOAT:
                    val.fnum = -var->val.fnum;
                    break;
                default: //CZL_STRING
                    return 0;
                }
                if (!(var=czl_const_create(gp, var->type, &val)))
                    return 0;
                constant->op.obj = var;
            }
            if (!(gp->tmp->exp_head=czl_opr_create(gp, constant)))
                return 0;
            --gp->tmp->exp_node_cnt;
        }
        if (ref_flag || gp->tmp->exp_head)
        {
            index = inx.val.inum;
            if (!(node=czl_para_explain_create(gp, index-1, ref_flag)))
                return 0;
            czl_para_explain_insert(&fun->para_explain, &tail, node);
        }

        ++cnt;
        if (',' == *e)
            e++;
    }

    return 1;
}

czl_fun* czl_sys_fun_create(czl_gp *gp, czl_sysfun *sysfun)
{
	short vars_cnt;
	czl_fun *fun;
    const czl_sys_fun *sys_fun = sysfun->sysfun;

    if (sysfun->fun)
        return sysfun->fun;

    if (!(fun=czl_fun_node_create(gp, NULL,
							   CZL_IN_IDLE,
							   CZL_SYS_FUN,
							   CZL_NIL,
							   sys_fun->sys_fun)))
        return NULL;

    vars_cnt = sys_fun->enter_vars_count;
    if (vars_cnt > 0)
    {
		short i;
        czl_var *p;
        czl_var_arr vars = (czl_var*)CZL_RT_CALLOC(gp, vars_cnt, sizeof(czl_var));
        if (!vars)
            return NULL;
        for (i = 0; i < vars_cnt; i++)
        {
            p = vars+i;
            p->type = CZL_INT;
            p->ot = CZL_NIL;
            p->quality = CZL_DYNAMIC_VAR;
        }
        fun->vars = vars;
    }

    fun->name = sys_fun->name;
    fun->dynamic_vars_cnt = fun->enter_vars_count = vars_cnt;

    if (sys_fun->paras_explain &&
        !czl_build_sys_fun_paras_explain(gp, fun, sys_fun->paras_explain))
    {
        sprintf(gp->log_buf, "paras error of system function %s, ", sys_fun->name);
        czl_block_delete(gp, CZL_FUN_BLOCK, fun, 1);
        return NULL;
    }

    sysfun->fun = fun;
    czl_fun_node_insert(&gp->funs_head, fun);

    return fun;
}

char czl_sys_lib_hash_create(czl_gp *gp)
{
	unsigned long i;
	czl_syslib *libs;

    if (0 == czl_syslibs_num) return 1;

    if (!(libs=(czl_syslib*)CZL_TMP_CALLOC(gp, czl_syslibs_num,
                                      sizeof(czl_syslib))))
        return 0;

    gp->tmp->syslibs = libs;

    for (i = 0; i < czl_syslibs_num; i++)
    {
        libs[i].name = czl_syslibs[i].name;
        libs[i].lib = czl_syslibs + i;
        if (czl_sys_hash_find(CZL_STRING, CZL_NIL, libs[i].name, &gp->tmp->syslibs_hash))
            return 0;
        if (!czl_sys_hash_insert(gp, CZL_STRING, libs+i, &gp->tmp->syslibs_hash))
            return 0;
    }

    return 1;
}

czl_sys_hash* czl_sys_fun_hash_create(czl_gp *gp, char *name)
{
	czl_sysfun *funs;
	unsigned long i, j;
    czl_syslib *table = (czl_syslib*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                                       name, &gp->tmp->syslibs_hash);
    if (!table)
    {
        sprintf(gp->log_buf, "undefined library %s, ", name);
        return NULL;
    }

    if (table->funs)
        return &table->hash;

    if (!(funs=(czl_sysfun*)CZL_TMP_CALLOC(gp, table->lib->num,
                                      sizeof(czl_sysfun))))
        return NULL;
    table->funs = funs;

    j = table->lib->num;
    for (i = 0; i < j; i++)
    {
        funs[i].name = table->lib->funs[i].name;
        funs[i].sysfun = table->lib->funs + i;
        if (czl_sys_hash_find(CZL_STRING, CZL_NIL, funs[i].name, &table->hash))
        {
            sprintf(gp->log_buf, "redefined library-function %s, ", funs[i].name);
            return NULL;
        }
        if (!czl_sys_hash_insert(gp, CZL_STRING, funs+i, &table->hash))
            return NULL;
    }

    return &table->hash;
}

czl_sysfun* czl_sys_fun_find(char *name, czl_sys_hash *hash)
{
    return (czl_sysfun*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, hash);
}

void czl_syslib_delete(czl_gp *gp, czl_syslib *libs)
{
	unsigned long i;

    if (!libs) return;

    for (i = 0; i < czl_syslibs_num; i++)
    {
        if (libs[i].funs)
        {
            czl_hash_table_delete(gp, &libs[i].hash);
            CZL_TMP_FREE(gp, libs[i].funs,
                         libs[i].lib->num*sizeof(czl_sysfun));
        }
    }
    CZL_TMP_FREE(gp, libs, czl_syslibs_num*sizeof(czl_syslib));
}
///////////////////////////////////////////////////////////////
char czl_sys_keyword_hash_create
(
    czl_gp *gp,
    const czl_keyword *key,
    unsigned long cnt
)
{
    unsigned long i;

    for (i = 0; i < cnt; i++)
        if (!czl_sys_hash_insert(gp, CZL_STRING,
                                 (void*)(key+i),
                                 &gp->tmp->keyword_hash))
            return 0;

    return 1;
}
///////////////////////////////////////////////////////////////
czl_fun* czl_fun_node_create
(
    czl_gp *gp,
    char *name,
    char state,
    char type,
    char ret_ot,
    void *sys_fun
)
{
	unsigned long len = 0;
    czl_fun *p = (czl_fun*)CZL_RT_MALLOC(gp, CZL_GFNS(type));
    if (!p)
        return NULL;

    if (!name)
        p->name = NULL;
    else
    {
        len = strlen(name);
        if (!(p->name=(char*)CZL_RT_MALLOC(gp, len+1)))
        {
            CZL_RT_FREE(gp, p, sizeof(czl_fun));
            return NULL;
        }
        strcpy(p->name, name);
    }

    p->state = state;
    p->type = type;
    p->enter_vars_count = 0;
    p->vars = NULL;
    p->dynamic_vars_cnt = 0;
    p->static_vars_cnt = 0;
    p->backup_vars = NULL;
    p->backup_cnt = 0;
    p->backup_size = 0;
    p->para_explain = NULL;
    p->sys_fun = (char(*)(void*, void*))sys_fun;
    p->file = gp->error_file;
    p->next = NULL;

    p->ret.name = NULL;
    p->ret.quality = CZL_FUNRET_VAR;
    p->ret.ot = ret_ot;
    p->ret.type = CZL_INT;
    p->ret.val.inum = 0;

    if (type != CZL_SYS_FUN)
    {
        p->try_flag = 0;
        p->goto_flag = 0;
        p->yeild_flag = 0;
        p->switch_flag = 0;
        p->loc_vars = NULL;
        p->store_device = NULL;
        p->store_device_head = NULL;
        p->goto_flags = NULL;
        p->sentences_head = NULL;
        p->class_vars = NULL;
        p->cur_ins = NULL;
        p->pc = NULL;
        p->foreachs = NULL;
        p->foreach_cnt = 0;
        p->foreach_sum = 0;
        p->reg = NULL;
        p->reg_cnt = 0;
        p->opcode = NULL;
        p->opcode_cnt= 0;
        if (CZL_USR_CLASS_FUN == type)
        {
            p->permission = gp->tmp->permission;
            p->hash = czl_bkdr_hash(name, len);
        }
    }

    return p;
}

void czl_fun_node_insert(czl_fun **head, czl_fun *node)
{
    node->next = *head;
    *head = node;
}

char czl_fun_insert(czl_gp *gp, czl_fun *node)
{
    switch (gp->tmp->analysis_field)
    {
    case CZL_IN_GLOBAL: case CZL_IN_GLOBAL_FUN:
        czl_fun_node_insert(&gp->funs_head, node);
        return czl_sys_hash_insert(gp, CZL_STRING, node,
                                   &gp->tmp->funs_hash);
    default: //CZL_IN_CLASS、CZL_IN_CLASS_FUN
        czl_fun_node_insert(&gp->tmp->cur_class->funs, node);
        if (czl_hash_repeat_check(node->hash,
                                  &gp->tmp->cur_class->funs_hash,
                                  CZL_FUNCTION))
        {
            sprintf(gp->log_buf, "hash repeat of class function %s , ", node->name);
            return 0;
        }
        return czl_sys_hash_insert(gp, CZL_STRING, node,
                                   &gp->tmp->cur_class->funs_hash);
    }
}

czl_fun* czl_fun_node_find(char *name, const czl_sys_hash *table)
{
    return (czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, table);
}

czl_fun* czl_fun_in_class_parents_find
(
    char *name,
    const czl_class_parent *p,
    char flag
)
{
    czl_fun *node;

    while (p)
    {
        if (flag || p->permission != CZL_PRIVATE)
        {
            node = czl_fun_node_find(name, &p->pclass->funs_hash);
            if (node && node->permission != CZL_PRIVATE)
                return node;
            if (p->permission != CZL_PRIVATE)
            {
                node = czl_fun_in_class_parents_find
                        (name, p->pclass->parents, 0);
                if (node)
                    return node;
            }
        }
        p = p->next;
    }

    return NULL;
}

czl_fun* czl_fun_find(czl_gp *gp, char *name, char mode)
{
    czl_fun *node;

    switch (gp->tmp->analysis_field)
    {
    case CZL_IN_GLOBAL: case CZL_IN_GLOBAL_FUN:
        return (czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, &gp->tmp->funs_hash);
    default: //CZL_IN_CLASS、CZL_IN_CLASS_FUN
        node = czl_fun_node_find(name, &gp->tmp->cur_class->funs_hash);
        if (node || CZL_LOCAL_FIND == mode)
            return node;
        //CZL_GLOBAL_FIND
        node = czl_fun_in_class_parents_find
                (name, gp->tmp->cur_class->parents, 1);
        if (node)
            return node;
        return (czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, &gp->tmp->funs_hash);
    }
}

czl_fun* czl_fun_find_in_exp
(
    czl_gp *gp,
    char *name,
    char global_flag,
    char this_flag
)
{
    czl_fun *node;

    if (global_flag)
        return (czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL, name, &gp->tmp->funs_hash);

    if (this_flag)
    {
        node = czl_fun_node_find(name, &gp->tmp->cur_class->funs_hash);
        if (node)
            return node;
        return czl_fun_in_class_parents_find
                    (name, gp->tmp->cur_class->parents, 1);
    }

    return czl_fun_find(gp, name, CZL_GLOBAL_FIND);
}
///////////////////////////////////////////////////////////////
czl_goto_flag* czl_goto_node_create(czl_gp *gp, const char *name)
{
	czl_code_block *block = gp->tmp->code_blocks +
                            gp->tmp->code_blocks_count - 1;
    czl_goto_flag *p = (czl_goto_flag*)CZL_TMP_MALLOC(gp, sizeof(czl_goto_flag));
    if (!p)
        return NULL;

    if (!(p->name=(char*)CZL_TMP_MALLOC(gp, strlen(name)+1)))
    {
        CZL_TMP_FREE(gp, p, sizeof(czl_goto_flag));
        return NULL;
    }
    strcpy(p->name, name);
    p->next = NULL;

    p->block = block->block.fun;
    switch (block->type)
    {
    case CZL_BRANCH_BLOCK:
        if (!block->block.branch->childs_tail)
        {
            p->sentence = block->block.branch->sentences_tail;
            block->block.branch->goto_flag = 1;
        }
        else
        {
            p->block = block->block.branch->childs_tail;
            p->sentence = block->block.branch->childs_tail->sentences_tail;
            block->block.branch->childs_tail->goto_flag = 1;
        }
        break;
    case CZL_LOOP_BLOCK:
        p->sentence = block->block.loop->sentences_tail;
        block->block.loop->goto_flag = 1;
        break;
    case CZL_TASK_BLOCK:
        p->sentence = block->block.task->sentences_tail;
        block->block.task->goto_flag = 1;
        break;
    case CZL_TRY_BLOCK:
        p->sentence = block->block.Try->sentences_tail;
        block->block.Try->goto_flag = 1;
        break;
    default: //CZL_FUN_BLOCK
        p->sentence = gp->tmp->sentences_tail;
        block->block.fun->goto_flag = 1;
        break;
    }

    return p;
}

void czl_goto_node_insert(czl_goto_flag **head, czl_goto_flag *node)
{
    node->next = *head;
    *head = node;
}

void czl_goto_list_delete(czl_gp *gp, czl_goto_flag *p)
{
    czl_goto_flag *q;

    while (p)
    {
        q = p->next;
        CZL_TMP_FREE(gp, p->name, strlen(p->name)+1);
        CZL_TMP_FREE(gp, p, sizeof(czl_goto_flag));
        p = q;
    }
}

czl_branch_child* czl_branch_child_node_create(czl_gp *gp)
{
    czl_branch_child *p = (czl_branch_child*)CZL_TMP_MALLOC(gp, sizeof(czl_branch_child));
    if (!p)
        return NULL;

    p->type = gp->tmp->current_branch_type;
    p->goto_flag = 0;
    p->conditions = gp->tmp->branch_child_paras;
    p->sentences_head = NULL;
    p->sentences_tail = NULL;
    p->store_device = NULL;
    p->next = NULL;

    gp->tmp->branch_child_paras = NULL;

    return p;
}

czl_branch* czl_branch_node_create(czl_gp *gp)
{
    czl_branch *p = (czl_branch*)CZL_TMP_MALLOC(gp, sizeof(czl_branch));
    if (!p)
        return NULL;

    if (CZL_SWITCH_BRANCH == gp->tmp->current_branch_type)
        gp->cur_fun->switch_flag = 1;

    p->type = gp->tmp->current_branch_type;
    p->goto_flag = 0;
    p->condition = gp->tmp->exp_head;
    p->sentences_head = NULL;
    p->sentences_tail = NULL;
    p->store_device = NULL;
    p->childs_head = NULL;
    p->childs_tail = NULL;

    gp->tmp->exp_head = NULL;

    return p;
}

czl_loop* czl_loop_node_create(czl_gp *gp)
{
    czl_loop *p = (czl_loop*)CZL_TMP_MALLOC(gp, sizeof(czl_loop));
    if (!p)
        return NULL;

    p->type = gp->tmp->current_loop_type;
    p->goto_flag = 0;
    p->sentences_head = NULL;
    p->sentences_tail = NULL;
    p->store_device = NULL;

    switch (gp->tmp->current_loop_type)
    {
    case CZL_WHILE_LOOP: case CZL_DO_WHILE_LOOP: case CZL_TIMER_LOOP:
        p->condition = gp->tmp->exp_head;
        p->paras_start = NULL;
        p->paras_end = NULL;
        p->task_cnt = 0;
        gp->tmp->exp_head = NULL;
        break;
    default: //CZL_FOR_LOOP/CZL_FOREACH_LOOP
        if (CZL_FOREACH_LOOP == gp->tmp->current_loop_type)
        {
            if (4 == gp->tmp->foreach_type || 5 == gp->tmp->foreach_type)
            {
                p->foreach_type = 1;
                p->flag = (4 == gp->tmp->foreach_type ? 1 : 0);
            }
            else
            {
                p->foreach_type = 0;
                p->flag = gp->tmp->foreach_type;
            }
        }
        p->condition = gp->tmp->for_condition;
        p->paras_start = gp->tmp->for_paras_start;
        p->paras_end = gp->tmp->for_paras_end;
        gp->tmp->for_condition = NULL;
        gp->tmp->for_paras_start = NULL;
        gp->tmp->for_paras_end = NULL;
        break;
    }

    return p;
}

czl_task* czl_task_node_create(czl_gp *gp)
{
    czl_task *p = (czl_task*)CZL_TMP_MALLOC(gp, sizeof(czl_task));
    if (!p)
        return NULL;

    p->goto_flag = 0;
    p->condition = gp->tmp->exp_head;
    p->sentences_head = NULL;
    p->sentences_tail = NULL;
    p->store_device = NULL;

    gp->tmp->exp_head = NULL;

    return p;
}

czl_try* czl_try_node_create(czl_gp *gp)
{
    czl_try *p = (czl_try*)CZL_TMP_MALLOC(gp, sizeof(czl_try));
    if (!p)
        return NULL;

    gp->cur_fun->try_flag = 1;

    p->type = gp->tmp->try_type;
    p->goto_flag = 0;
    p->name = gp->tmp->goto_flag_name;
    p->err_file = gp->error_file;
    p->err_line = gp->error_line;
    p->paras = gp->tmp->try_paras;
    p->sentences_head = NULL;
    p->sentences_tail = NULL;
    p->store_device = NULL;
    p->next = NULL;

    gp->tmp->try_paras = NULL;
    gp->tmp->goto_flag_name = NULL;

    return p;
}

czl_goto* czl_goto_create(czl_gp *gp)
{
    czl_goto *node = (czl_goto*)CZL_TMP_MALLOC(gp, sizeof(czl_goto));
    if (!node)
        return NULL;

    node->name = gp->tmp->goto_flag_name;
    node->err_file = gp->error_file;
    node->err_line = gp->error_line;

    gp->tmp->goto_flag_name = NULL;

    return node;
}

czl_sentence* czl_sentence_create(czl_gp *gp, char type)
{
    czl_sentence *p = (czl_sentence*)CZL_TMP_MALLOC(gp, sizeof(czl_sentence));
    if (!p)
        return NULL;

    switch (type)
    {
    case CZL_EXP_SENTENCE:
    case CZL_RETURN_SENTENCE: case CZL_YEILD_SENTENCE:
        if (CZL_YEILD_SENTENCE == type)
            gp->cur_fun->yeild_flag = 1;
        p->sentence.exp = gp->tmp->exp_head;
        gp->tmp->exp_head = NULL;
        break;
    case CZL_BRANCH_BLOCK:
        p->sentence.branch = czl_branch_node_create(gp);
        if (!p->sentence.branch)
            goto CZL_ERROR_END;
        break;
    case CZL_LOOP_BLOCK:
        p->sentence.loop = czl_loop_node_create(gp);
        if (!p->sentence.loop)
            goto CZL_ERROR_END;
        if (CZL_FOREACH_LOOP == p->sentence.loop->type)
        {
            ++gp->cur_fun->foreach_sum;
            if (0 == p->sentence.loop->foreach_type)
                ++gp->cur_fun->foreach_cnt;
        }
        break;
    case CZL_TASK_BLOCK:
        p->sentence.task = czl_task_node_create(gp);
        if (!p->sentence.task)
            goto CZL_ERROR_END;
        break;
    case CZL_TRY_BLOCK:
        p->sentence.Try = czl_try_node_create(gp);
        if (!p->sentence.Try)
            goto CZL_ERROR_END;
        break;
    case CZL_GOTO_SENTENCE:
        p->sentence.Goto = czl_goto_create(gp);
        if (!p->sentence.Goto)
            goto CZL_ERROR_END;
        break;
    default: //CZL_BREAK_SENTENCE、CZL_CONTINUE_SENTENCE
        p->sentence.exp = NULL;
        break;
    }

    p->type = type;
    p->next = NULL;
    return p;

CZL_ERROR_END:
    CZL_TMP_FREE(gp, p, sizeof(czl_sentence));
    return NULL;
}

void czl_sentence_node_insert
(
    czl_sentence **head,
    czl_sentence **tail,
    czl_sentence *node
)
{
    if (NULL == *head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

void czl_sentence_insert(czl_gp *gp, czl_sentence *node)
{
    czl_sentence **head;
    czl_sentence **tail;
    czl_code_block *current = &gp->tmp->code_blocks[gp->tmp->code_blocks_count-1];

    switch (current->type)
    {
    case CZL_FUN_BLOCK:
        head = &current->block.fun->sentences_head;
        tail = &gp->tmp->sentences_tail;
        break;
    case CZL_LOOP_BLOCK:
        head = &current->block.loop->sentences_head;
        tail = &current->block.loop->sentences_tail;
        break;
    case CZL_TASK_BLOCK:
        head = &current->block.task->sentences_head;
        tail = &current->block.task->sentences_tail;
        break;
    case CZL_TRY_BLOCK:
        head = &current->block.Try->sentences_head;
        tail = &current->block.Try->sentences_tail;
        break;
    default: //CZL_BRANCH_BLOCK
        if (!current->block.branch->childs_tail)
        {
            head = &current->block.branch->sentences_head;
            tail = &current->block.branch->sentences_tail;
        }
        else
        {
            head = &current->block.branch->
                    childs_tail->sentences_head;
            tail = &current->block.branch->
                    childs_tail->sentences_tail;
        }
        break;
    }

    czl_sentence_node_insert(head, tail, node);
}

char czl_branch_child_node_insert(czl_gp *gp)
{
	czl_branch *branch = gp->tmp->code_blocks[gp->tmp->code_blocks_count-1].block.branch;
    czl_branch_child *branch_child = czl_branch_child_node_create(gp);
    if (!branch_child)
        return 0;

    if (!branch->childs_head)
        branch->childs_head = branch_child;
    else
        branch->childs_tail->next = branch_child;

    branch->childs_tail = branch_child;

    return 1;
}

char czl_sentence_block_create_in_fun(czl_gp *gp, char type)
{
    czl_sentence *node;

    switch (type)
    {
    case CZL_BRANCH_BLOCK: case CZL_LOOP_BLOCK: case CZL_TASK_BLOCK: case CZL_TRY_BLOCK:
        if (!(node=czl_sentence_create(gp, type)))
            return 0;
        czl_sentence_insert(gp, node);
        gp->tmp->code_blocks[gp->tmp->code_blocks_count].type = type;
        gp->tmp->code_blocks[gp->tmp->code_blocks_count].block.branch = node->sentence.branch;
        gp->tmp->code_blocks_count++;
        break;
    case CZL_BRANCH_CHILD_BLOCK:
        if (!czl_branch_child_node_insert(gp))
            return 0;
        break;
    default:
        if (!(node=czl_sentence_create(gp, type)))
            return 0;
        czl_sentence_insert(gp, node);
    }

    return 1;
}

char czl_global_vars_init_sentence_insert(czl_gp *gp)
{
    //全局、类成员变量的初始化必须等到解释完脚本才进行，这里先将语句存起来
    czl_glo_sentence *p = (czl_glo_sentence*)CZL_TMP_MALLOC(gp, sizeof(czl_glo_sentence));
    if (!p)
        return 0;

    p->sentence.exp = gp->tmp->exp_head;
    gp->tmp->exp_head = NULL;

    p->type = CZL_EXP_SENTENCE;
    p->next = NULL;
    p->file = gp->error_file;

    if (NULL == gp->glo_vars_init_head)
        gp->glo_vars_init_head = p;
    else
        gp->tmp->glo_vars_init_tail->next = p;

    gp->tmp->glo_vars_init_tail = p;

    return 1;
}

char czl_sentence_block_create(czl_gp *gp, char block_type)
{
    switch (gp->tmp->analysis_field)
    {
    case CZL_IN_GLOBAL:
        return czl_global_vars_init_sentence_insert(gp);
    default: //CZL_IN_GLOBAL_FUN、CZL_IN_CLASS_FUN
        return czl_sentence_block_create_in_fun(gp, block_type);
    }
}

void czl_else_block_to_elif(czl_gp *gp)
{
    czl_branch *if_node = gp->tmp->code_blocks[gp->tmp->code_blocks_count-1].block.branch;
    if_node->childs_tail->conditions = gp->tmp->branch_child_paras;
    gp->tmp->branch_child_paras = NULL;
}
///////////////////////////////////////////////////////////////
static char czl_foreach_range(czl_gp *gp, const czl_foreach *loop, char flag)
{
    czl_var *lo, *ro;
    if (CZL_REG_VAR == loop->a->kind && loop->a->res->type != CZL_OBJ_REF)
        lo = loop->a->res;
    else if (!(lo=czl_get_opr(gp, loop->a->kind, loop->a->res)))
        goto CZL_END;

    if (flag)
    {
        if (lo->type != CZL_INT && lo->type != CZL_FLOAT)
            goto CZL_END;
        if (!(ro=czl_exp_stack_cac(gp, loop->b)) || !czl_ass_cac(gp, lo, ro))
        {
            czl_runtime_error_report(gp, loop->b);
            return -1;
        }
    }
    else
    {
        if (loop->para.d)
        {
            if (CZL_EIO(loop->para.d) &&
                CZL_REG_VAR == loop->para.d->kind &&
                loop->para.d->res->type != CZL_OBJ_REF)
            {
                ro = loop->para.d->res;
            }
            else if (!(ro=czl_exp_stack_cac(gp, loop->para.d)))
            {
                czl_runtime_error_report(gp, loop->para.d);
                return -1;
            }
            switch (lo->type)
            {
            case CZL_INT:
                switch (ro->type)
                {
                case CZL_INT: lo->val.inum += ro->val.inum; break;
                case CZL_FLOAT: lo->val.inum += ro->val.fnum; break;
                default: czl_runtime_error_report(gp, loop->para.d); return -1;
                }
                break;
            case CZL_FLOAT:
                switch (ro->type)
                {
                case CZL_FLOAT: lo->val.fnum += ro->val.fnum; break;
                case CZL_INT:  lo->val.fnum += ro->val.inum; break;
                default: czl_runtime_error_report(gp, loop->para.d); return -1;
                }
                break;
            default: goto CZL_END;
            }
        }
        else
        {
            switch (lo->type) // ++i
            {
            case CZL_INT: loop->flag ? ++lo->val.inum : --lo->val.inum; break;
            case CZL_FLOAT: loop->flag ? ++lo->val.fnum : --lo->val.fnum; break;
            default: goto CZL_END;
            }
        }
    }

    if (CZL_EIO(loop->c) &&
        CZL_REG_VAR == loop->c->kind &&
        loop->c->res->type != CZL_OBJ_REF)
    {
        ro = loop->c->res;
    }
    else if (!(ro=czl_exp_stack_cac(gp, loop->c)))
    {
        czl_runtime_error_report(gp, loop->c);
        return -1;
    }

    if (CZL_INT == lo->type)
        switch (ro->type)
        {
        case CZL_INT:
            return loop->flag ? lo->val.inum <= ro->val.inum :
                                lo->val.inum >= ro->val.inum;
        case CZL_FLOAT:
            return loop->flag ? lo->val.inum <= ro->val.fnum :
                                lo->val.inum >= ro->val.fnum;
        default: czl_runtime_error_report(gp, loop->c); return -1;
        }
    else
        switch (ro->type)
        {
        case CZL_INT:
            return loop->flag ? lo->val.fnum <= ro->val.inum :
                                lo->val.fnum >= ro->val.inum;
        case CZL_FLOAT:
            return loop->flag ? lo->val.fnum <= ro->val.fnum :
                                lo->val.fnum >= ro->val.fnum;
        default: czl_runtime_error_report(gp, loop->c); return -1;
        }

CZL_END:
    czl_runtime_error_report(gp, loop->a);
    return -1;
}

static char czl_foreach_string
(
    czl_gp *gp,
    czl_foreach *loop,
    czl_var *lo,
    char *str,
    char flag
)
{
	czl_var ro;

    if (flag)
        loop->para.inx = 0;

    if ('\0' == str[loop->para.inx])
        return 0;

    ro.type = CZL_INT;
    ro.val.inum = (unsigned char)str[loop->para.inx++];

    return czl_ass_cac(gp, lo, &ro) ? 1 : -1;
}

static char czl_foreach_array_list
(
    czl_gp *gp,
    czl_foreach *loop,
    czl_var *lo,
    const czl_array_list *list,
    char flag
)
{
	czl_var *ro;
    czl_para *para;

    if (flag)
    {
        if (!(para=list->paras))
            return 0;
    }
    else
    {
        if (!loop->para.ele)
            return 0;
        para = (czl_para*)loop->para.ele;
    }

    if (!(ro=czl_exp_stack_cac(gp, para->para)))
        return -1;
    loop->para.ele = para->next;

    return czl_ass_cac(gp, lo, ro) ? 1 : -1;
}

static char czl_foreach_table
(
    czl_gp *gp,
    czl_foreach *loop,
    czl_var *lo,
    czl_var *ro,
    char flag
)
{
    czl_table *tab = CZL_TAB(ro->val.Obj);
    czl_tabkv *ele = (flag ? tab->eles_head : tab->foreach_inx);

    if (!ele)
    {
        if (2 == loop->flag)
        {
            if (!czl_val_del(gp, lo))
                return -1;
            lo->type = CZL_INT;
            lo->val.inum = 0;
        }
        return 0;
    }

    tab->foreach_inx = ele->next;

    if (1 == loop->flag)
    {
        czl_var tmp;
        tmp.type = ele->kt;
        tmp.val = ele->key;
        return czl_ass_cac(gp, lo, &tmp) ? 1 : -1;
    }
    else if (2 == loop->flag)
    {
		czl_var tmp;
        if (!ele->name && !czl_ref_obj_create(gp, ro, (czl_var*)ele, &ro, 1))
            return -1;
        tmp.type = CZL_OBJ_REF;
        tmp.val.ref.inx = -1;
        tmp.val.ref.var = (czl_var*)ele;
        return czl_ass_cac(gp, lo, &tmp) ? 1 : -1;
    }
    else if (loop->a1)
    {
		czl_var *lo1;
        czl_var tmp;
        tmp.type = ele->kt;
        tmp.val = ele->key;
        if (!czl_ass_cac(gp, lo, &tmp))
            return -1;

        if (CZL_REG_VAR == loop->a1->kind &&
            loop->a1->res->type != CZL_OBJ_REF)
            lo1 = loop->a1->res;
        else if (!(lo1=czl_get_opr(gp, loop->a1->kind, loop->a1->res)))
        {
            czl_runtime_error_report(gp, loop->a1);
            return -1;
        }
        return czl_ass_cac(gp, lo1, (czl_var*)ele) ? 1 : -1;
    }
    else
        return czl_ass_cac(gp, lo, (czl_var*)ele) ? 1 : -1;
}

static char czl_foreach_array
(
    czl_gp *gp,
    czl_foreach *loop,
    czl_var *lo,
    czl_var *ro,
    char flag
)
{
	czl_var *var;
    czl_array *arr = CZL_ARR(ro->val.Obj);

    if (flag)
        loop->para.inx = 0;

    if (loop->para.inx == arr->cnt)
    {
        if (2 == loop->flag)
        {
            if (!czl_val_del(gp, lo))
                return -1;
            lo->type = CZL_INT;
            lo->val.inum = 0;
        }
        return 0;
    }

    var = arr->vars + loop->para.inx++;

    if (2 == loop->flag)
    {
		czl_var tmp;
        if (!var->name && !czl_ref_obj_create(gp, ro, var, &ro, 1))
            return -1;
        tmp.type = CZL_OBJ_REF;
        tmp.val.ref.inx = loop->para.inx - 1;
        tmp.val.ref.var = (czl_var*)arr;
        return czl_ass_cac(gp, lo, &tmp) ? 1 : -1;
    }
    else
        return czl_ass_cac(gp, lo, var) ? 1 : -1;
}

static char czl_foreach_sq
(
    czl_gp *gp,
    czl_foreach *loop,
    czl_var *lo,
    czl_var *ro,
    char flag
)
{
    czl_sq *sq = CZL_SQ(ro->val.Obj);
    czl_glo_var *ele = (flag ? sq->eles_head : sq->foreach_inx);

    if (!ele)
    {
        if (2 == loop->flag)
        {
            if (!czl_val_del(gp, lo))
                return -1;
            lo->type = CZL_INT;
            lo->val.inum = 0;
        }
        return 0;
    }

    sq->foreach_inx = ele->next;

    if (2 == loop->flag)
    {
		czl_var tmp;
        if (!ele->name && !czl_ref_obj_create(gp, ro, (czl_var*)ele, &ro, 1))
            return -1;
        tmp.type = CZL_OBJ_REF;
        tmp.val.ref.inx = -1;
        tmp.val.ref.var = (czl_var*)ele;
        return czl_ass_cac(gp, lo, &tmp) ? 1 : -1;
    }
    else
        return czl_ass_cac(gp, lo, (czl_var*)ele) ? 1 : -1;
}

static char czl_foreach_file
(
    czl_gp *gp,
    czl_foreach *loop,
    czl_var *lo,
    czl_file *f,
    char flag
)
{
    char ret;
    long inx = (flag ? 0 : loop->para.inx);
    char exceptionCode = gp->exceptionCode;

    if (fseek(f->fp, inx, SEEK_SET))
        return 0;

    if (!czl_val_del(gp, lo))
        return -1;
    lo->type = CZL_INT;

    ret = (1 == f->mode ? czl_obj_read(gp, f->fp, lo) :
                          czl_line_read(gp, f->fp, lo));

    if (EOF == (inx=ftell(f->fp)))
        ret = 0;
    else
        loop->para.inx = inx;

    if (!ret)
        gp->exceptionCode = exceptionCode;

    return ret;
}

static char czl_obj_check(czl_gp *gp, czl_exp_ele *ele)
{
    czl_var *ro;
    if (CZL_REG_VAR == ele->kind && ele->res->type != CZL_OBJ_REF)
        ro = ele->res;
    else if (!(ro=czl_get_opr(gp, ele->kind, ele->res)))
    {
        czl_runtime_error_report(gp, ele);
        return 0;
    }

    switch (ro->type)
    {
    case CZL_TABLE:
        if (CZL_TAB(ro->val.Obj)->rc > 1)
            return czl_table_fork(gp, CZL_TAB(ro->val.Obj), &ro->val, 1);
        break;
    case CZL_ARRAY:
        if (CZL_ARR(ro->val.Obj)->rc > 1)
            return czl_array_fork(gp, CZL_ARR(ro->val.Obj), &ro->val, 1);
        break;
    case CZL_STACK: case CZL_QUEUE:
        if (CZL_SQ(ro->val.Obj)->rc > 1)
            return czl_sq_fork(gp, CZL_SQ(ro->val.Obj), &ro->val, 1);
        break;
    default:
        break;
    }

    return 1;
}

static char czl_foreach_object
(
    czl_gp *gp,
    czl_foreach *loop,
    char flag
)
{
	char ret;
    czl_var *lo, *ro;

    if (2 == loop->flag)
    {
        if (flag && !czl_obj_check(gp, loop->b))
        {
            ret = -1;
            goto CZL_END;
        }
        if (CZL_INS_VAR == loop->a->kind)
            lo = ((czl_ins_var*)loop->a->res)->var;
        else
            lo = loop->a->res;
    }
    else
    {
        if (CZL_REG_VAR == loop->a->kind &&
            loop->a->res->type != CZL_OBJ_REF)
            lo = loop->a->res;
        else if (!(lo=czl_get_opr(gp, loop->a->kind, loop->a->res)))
        {
            czl_runtime_error_report(gp, loop->a);
            return -1;
        }
    }

    if (CZL_REG_VAR == loop->b->kind && loop->b->res->type != CZL_OBJ_REF)
        ro = loop->b->res;
    else if (!(ro=czl_get_opr(gp, loop->b->kind, loop->b->res)))
    {
        czl_runtime_error_report(gp, loop->b);
        return -1;
    }

    if (3 == loop->flag)
    {
        if (!czl_ass_cac(gp, lo, ro))
            ret = -1;
        else if (gp->yeild_end)
            ret = 0;
        else
            ret = 1;
        goto CZL_END;
    }

    if (loop->type != ro->type || loop->c != (czl_exp_ele*)ro->val.Obj)
        flag = 1;
    if (flag)
    {
        loop->type = ro->type;
        loop->c = (czl_exp_ele*)ro->val.Obj;
    }

    switch (ro->type)
    {
    case CZL_STRING:
        ret = czl_foreach_string(gp, loop, lo, CZL_STR(ro->val.str.Obj)->str, flag);
        break;
    case CZL_FILE:
        ret = czl_foreach_file(gp, loop, lo, CZL_FIL(ro->val.Obj), flag);
        break;
    case CZL_ARRAY_LIST:
        ret = czl_foreach_array_list(gp, loop, lo, CZL_ARR_LIST(ro->val.Obj), flag);
        break;
    case CZL_TABLE:
        ret = czl_foreach_table(gp, loop, lo, ro, flag);
        break;
    case CZL_ARRAY:
        ret = czl_foreach_array(gp, loop, lo, ro, flag);
        break;
    case CZL_STACK: case CZL_QUEUE:
        ret = czl_foreach_sq(gp, loop, lo, ro, flag);
        break;
    default:
        return 0;
    }

CZL_END:
    if (-1 == ret)
        czl_runtime_error_report(gp, loop->b);

    return ret;
}

static czl_exp_ele* czl_try_find(czl_gp *gp, czl_exp_ele *pc)
{
    czl_exp_ele *bp = pc;

    for (;;)
    {
        if (CZL_LOGIC_JUMP == pc->flag && pc->lt)
        {
            if (CZL_TRY_CONTINUE == pc->kind)
            {
                czl_exp_ele *p;
                if (CZL_BLOCK_BEGIN == bp->next->flag && !(bp->next-1)->res)
                    (bp->next-1)->res = (bp->next-1)->lo;
                if (!(p=pc->next))
                    return bp->next;
                while (p->flag != CZL_TRY_BLOCK)
                    p = p->next;
                p->pl.pc = bp->next;
            }
            return pc->next;
        }
        else if (CZL_RETURN_SENTENCE == pc->flag)
        {
            if (gp->exceptionFuns[gp->exceptionCode-1])
            {
                gp->exceptionCode = CZL_EXCEPTION_NO;
                return bp;
            }
        }
        pc = pc->next;
    }
}

static czl_exp_ele* czl_exception_handle
(
    czl_gp *gp,
    czl_fun *fun,
    czl_exp_ele *pc,
    czl_usrfun_stack *stack,
    unsigned long *index
)
{
    unsigned long cnt = *index;

    if (!gp->error_file)
        gp->error_file = (cnt ? stack[cnt-1].fun->file : fun->file);

    for (;;)
    {
		czl_usrfun_stack *cur;
        if (CZL_EXIT_ABNORMAL == gp->exit_code &&
            gp->exceptionCode != CZL_EXCEPTION_DEAD &&
            ((0 == cnt && fun->try_flag) || (cnt > 0 && stack[cnt-1].fun->try_flag)) &&
            (pc=czl_try_find(gp, pc)))
        {
            *index = cnt;
            return pc;
        }
        if (0 == cnt)
        {
            gp->yeild_pc = pc + 1;
            return NULL;
        }
        cur = stack + --cnt;
        if (cur->fun->yeild_flag)
        {
            cur->fun->pc = pc + 1;
            cur->fun->state = CZL_IN_IDLE;
            czl_coroutine_paras_backup(cur->fun, (czl_var*)cur->fun->backup_vars);
            if (cur->fun->cur_ins)
                cur->fun->cur_ins = NULL;
        }
        else
            czl_fun_local_vars_clean(gp, cur->fun);
        pc = cur->pc;
    }
}

static czl_exp_ele* czl_switch_case_cmp(const czl_exp_ele *pc)
{
    switch (pc->lo->type)
    {
    case CZL_INT:
        switch ((pc-1)->res->type)
        {
        case CZL_INT:
            return pc->lo->val.inum == (pc-1)->res->val.inum ? pc->pl.pc : pc->next;
        case CZL_FLOAT:
            return pc->lo->val.inum == (pc-1)->res->val.fnum ? pc->pl.pc : pc->next;
        default:
            return pc->next;
        }
    case CZL_FLOAT:
        switch ((pc-1)->res->type)
        {
        case CZL_INT:
            return pc->lo->val.fnum == (pc-1)->res->val.inum ? pc->pl.pc : pc->next;
        case CZL_FLOAT:
            return pc->lo->val.fnum == (pc-1)->res->val.fnum ? pc->pl.pc : pc->next;
        default:
            return pc->next;
        }
    case CZL_STRING:
        return CZL_STRING == (pc-1)->res->type &&
               !strcmp(CZL_STR(pc->lo->val.str.Obj)->str,
                       CZL_STR((pc-1)->res->val.str.Obj)->str) ? pc->pl.pc : pc->next;
    default:
        return pc->lo->val.Obj == (pc-1)->res->val.Obj ? pc->pl.pc : pc->next;
    }
}

unsigned long czl_get_time(const czl_var *t)
{
    switch (t->type)
    {
    case CZL_INT: return t->val.inum >= 0 ? t->val.inum : 1;
    case CZL_FLOAT: return t->val.fnum >= 0 ? t->val.fnum : 1;
    default: return 1;
    }
}

static czl_exp_ele* czl_task_begin(const czl_var *t, czl_exp_ele *pc)
{
    unsigned long time = CZL_CLOCK;
    unsigned long cycle = czl_get_time(t);

    if (time - (unsigned long)pc->res >= cycle)
    {
        pc->res = (czl_var*)time;
        return pc->pl.pc;
    }

    return pc->next;
}

static char czl_fun_run(czl_gp *gp, czl_exp_ele *pc, czl_fun *fun)
{
    czl_var *lo = NULL, *ro = NULL;
    //
    czl_usrfun_stack *stack = NULL, *cur;
    unsigned long index = 0, size = 0;

    for (;;)
    {
    CZL_BEGIN:
        switch (pc->flag)
        {
        case CZL_BINARY_OPT:
            CZL_RBO(gp, ro, pc);
            break;
        case CZL_BINARY2_OPT:
            CZL_RB2O(gp, lo, ro, pc);
            break;
        case CZL_ASS_OPT:
            CZL_RAO(gp, ro, pc);
            break;
        case CZL_UNARY_OPT:
            CZL_RUO(gp, pc);
            break;
        case CZL_UNARY2_OPT:
            CZL_RU2O(gp, pc);
            break;
        case CZL_AND_AND:
            CZL_AA_CJ(gp, (pc-1)->res, pc);
            break;
        case CZL_OR_OR:
            CZL_OO_CJ(gp, (pc-1)->res, pc);
            break;
        case CZL_OPERAND:
            CZL_GOR(gp, pc);
            CZL_COT(gp, pc);
            break;
        case CZL_FOREACH_BLOCK:
            CZL_RFS(gp, pc);
            break;
        case CZL_BLOCK_BEGIN:
            pc = (CZL_EIT((pc-1)->res) ? pc->pl.pc : pc->next);
            break;
        case CZL_LOGIC_JUMP:
            pc = pc->pl.pc;
            break;
        case CZL_CASE_SENTENCE:
            CZL_RCS(pc);
            break;
        case CZL_SWITCH_SENTENCE:
            CZL_RSS(gp, pc);
            break;
        case CZL_RETURN_SENTENCE: case CZL_YEILD_SENTENCE:
            CZL_RRYS(gp, pc);
        case CZL_THREE_OPT:
            CZL_RTO((pc-1)->res, pc);
            break;
        case CZL_THREE_END:
            CZL_TORM(gp, pc->lo, (pc-1)->res, pc);
            break;
        case CZL_TRY_BLOCK:
            CZL_RTS(gp, pc, stack, size);
            break;
        //
        case CZL_ADD_SELF:
            CZL_ADD_SELF_CAC(pc);
            break;
        case CZL_DEC_SELF:
            CZL_DEC_SELF_CAC(pc);
            break;
        case CZL_NUMBER_NOT:
            CZL_NUMBER_NOT_CAC(pc);
            break;
        case CZL_LOGIC_NOT:
            CZL_LOGIC_NOT_CAC(pc);
            break;
        case CZL_LOGIC_FLIP:
            CZL_LOGIC_FLIP_CAC(pc);
            break;
        case CZL_SELF_ADD:
            CZL_SELF_ADD_CAC(pc);
            break;
        case CZL_SELF_DEC:
            CZL_SELF_DEC_CAC(pc);
            break;
        //
        case CZL_ASS:
            CZL_ASS_CAC(pc);
            break;
        case CZL_ADD_A:
            CZL_ADD_A_CAC(pc);
            break;
        case CZL_DEC_A:
            CZL_DEC_A_CAC(pc);
            break;
        case CZL_MUL_A:
            CZL_MUL_A_CAC(pc);
            break;
        case CZL_DIV_A:
            CZL_DIV_A_CAC(pc);
            break;
        case CZL_MOD_A:
            CZL_MOD_A_CAC(pc);
            break;
        case CZL_OR_A:
            CZL_OR_A_CAC(pc);
            break;
        case CZL_XOR_A:
            CZL_XOR_A_CAC(pc);
            break;
        case CZL_AND_A:
            CZL_AND_A_CAC(pc);
            break;
        case CZL_L_SHIFT_A:
            CZL_L_SHIFT_A_CAC(pc);
            break;
        case CZL_R_SHIFT_A:
            CZL_R_SHIFT_A_CAC(pc);
            break;
        //
        case CZL_MORE:
            CZL_MORE_CAC(pc);
            break;
        case CZL_MORE_EQU:
            CZL_MORE_EQU_CAC(pc);
            break;
        case CZL_LESS:
            CZL_LESS_CAC(pc);
            break;
        case CZL_LESS_EQU:
            CZL_LESS_EQU_CAC(pc);
            break;
        case CZL_EQU_EQU:
            CZL_EQU_EQU_CAC(pc);
            break;
        case CZL_NOT_EQU:
            CZL_NOT_EQU_CAC(pc);
            break;
        case CZL_EQU_3:
            CZL_EQU_3_CAC(pc);
            break;
        case CZL_XOR_XOR:
            CZL_XOR_XOR_CAC(pc);
            break;
        case CZL_ADD:
            CZL_ADD_CAC(pc);
            break;
        case CZL_DEC:
            CZL_DEC_CAC(pc);
            break;
        case CZL_MUL:
            CZL_MUL_CAC(pc);
            break;
        case CZL_DIV:
            CZL_DIV_CAC(pc);
            break;
        case CZL_MOD:
            CZL_MOD_CAC(pc);
            break;
        case CZL_OR:
            CZL_OR_CAC(pc);
            break;
        case CZL_XOR:
            CZL_XOR_CAC(pc);
            break;
        case CZL_AND:
            CZL_AND_CAC(pc);
            break;
        case CZL_L_SHIFT:
            CZL_L_SHIFT_CAC(pc);
            break;
        case CZL_R_SHIFT:
            CZL_R_SHIFT_CAC(pc);
            break;
        //
        case CZL_TASK_BEGIN:
            pc = czl_task_begin((pc-1)->res, pc);
            break;
        case CZL_TIMER_SLEEP:
            CZL_SLEEP(czl_get_time((pc-1)->res));
            pc = pc->pl.pc;
            break;
        case CZL_TIMER_INIT:
            pc->pl.pc->res = (czl_var*)CZL_CLOCK;
            ++pc;
            break;
        default:
            break;
        }
    }

CZL_FUN_RETURN:
    CZL_UFR(gp, index, size, stack, cur, pc, ro);

CZL_RUN_FUN:
    CZL_RUF(gp, index, size, stack, cur, pc, lo);

CZL_EXCEPTION_CATCH:
    czl_fun_ret_check_free(gp, pc->res, lo, ro);
    czl_runtime_error_report(gp, pc);
    if ((pc=czl_exception_handle(gp, fun, pc, stack, &index)))
    {
        gp->exit_flag = 0;
        goto CZL_BEGIN;
    }
    CZL_STACK_FREE(gp, stack, size*sizeof(czl_usrfun_stack));
    return 0;
}
///////////////////////////////////////////////////////////////
static void czl_set_exp_fun_reg(czl_gp *gp, czl_exp_fun *exp_fun)
{
	czl_para *i;

    if (!exp_fun) return;

    for (i = exp_fun->paras; i; i = i->next)
        czl_set_exp_reg(gp, i->para, 0);
}

static void czl_set_array_list_reg(czl_gp *gp, czl_array_list *list)
{
	czl_para *i;

    if (!list) return;

    for (i = list->paras; i; i = i->next)
        czl_set_exp_reg(gp, i->para, 0);
}

static void czl_set_table_list_reg(czl_gp *gp, czl_table_list *list)
{
    czl_table_node *i;

    if (!list) return;

    for (i = list->paras; i; i = i->next)
    {
        czl_set_exp_reg(gp, i->key, 0);
        czl_set_exp_reg(gp, i->val, 0);
    }
}

static void czl_set_member_reg(czl_gp *gp, czl_obj_member *obj)
{
	czl_member_index *inx = obj->index;

    if (CZL_LG_VAR == obj->type &&
        CZL_NIL == ((czl_loc_var*)obj->obj)->flag)
        obj->obj = ((czl_loc_var*)obj->obj)->var;

    while (inx)
    {
        if (CZL_ARRAY_INDEX == inx->type)
        {
            czl_set_exp_reg(gp, inx->index.arr.exp, 0);
            czl_set_exp_fun_reg(gp, inx->index.arr.exp_fun);
        }
        else //CZL_INSTANCE_INDEX
        {
            czl_set_exp_fun_reg(gp, inx->index.ins.exp_fun);
        }
        inx = inx->next;
    }
}

static void czl_set_new_reg(czl_gp *gp, czl_new_sentence *New)
{
    switch (New->type)
    {
    case CZL_INSTANCE:
        czl_set_exp_reg(gp, New->new_obj.ins.len, 0);
        czl_set_exp_fun_reg(gp, New->new_obj.ins.init_fun);
        break;
    case CZL_STRING:
        czl_set_exp_reg(gp, New->new_obj.arr.len, 0);
        break;
    case CZL_TABLE:
        czl_set_table_list_reg(gp, New->new_obj.tab);
        break;
    default: //CZL_ARRAY、CZL_STACK、CZL_QUEUE、CZL_LIST
        czl_set_exp_reg(gp, New->new_obj.arr.len, 0);
        czl_set_array_list_reg(gp, New->new_obj.arr.init_list);
        break;
    }
}

static czl_var* czl_set_opr_reg(czl_gp *gp, unsigned char *kind, czl_var *obj, char flag)
{
    czl_exp_fun *ef;

    switch (*kind)
    {
    case CZL_FUNCTION:
        czl_set_exp_fun_reg(gp, (czl_exp_fun*)obj);
        ef = (czl_exp_fun*)obj;
        if (1 == flag &&
            CZL_STATIC_FUN == ef->quality &&
            ef->fun->type != CZL_SYS_FUN)
        {
            *kind = CZL_USR_FUN;
        }
        if (CZL_LG_VAR_DYNAMIC_FUN == ef->quality &&
            CZL_NIL == ((czl_loc_var*)(ef->fun))->flag)
        {
            ef->fun = (czl_fun*)((czl_loc_var*)ef->fun)->var;
        }
        break;
    case CZL_MEMBER:
        czl_set_member_reg(gp, (czl_obj_member*)obj);
        break;
    case CZL_ARRAY_LIST:
        *kind = CZL_REG_VAR;
        czl_set_array_list_reg(gp, CZL_ARR_LIST(((czl_var*)obj)->val.Obj));
        break;
    case CZL_TABLE_LIST:
        *kind = CZL_REG_VAR;
        czl_set_table_list_reg(gp, CZL_TAB_LIST(((czl_var*)obj)->val.Obj));
        break;
    case CZL_NEW:
        *kind = CZL_REG_VAR;
        czl_set_new_reg(gp, (czl_new_sentence*)((czl_var*)obj)->val.Obj);
        break;
    case CZL_INS_VAR:
        break;
    case CZL_LG_VAR:
        *kind = CZL_REG_VAR;
        if (CZL_NIL == ((czl_loc_var*)obj)->flag)
            obj = (czl_var*)((czl_loc_var*)obj)->var;
        break;
    default: //CZL_REG_VAR/CZL_ENUM/CZL_FUN_REF/CZL_NIL
        *kind = CZL_REG_VAR;
        break;
    }

    return obj;
}

static void czl_check_opt_regs
(
    czl_exp_ele *pc,
    czl_var *res,
    czl_var *reg
)
{
    while (pc)
    {
        switch (pc->flag)
        {
        case CZL_UNARY_OPT: case CZL_BINARY_OPT:
            if (res == pc->lo)
            {
                pc->lo = reg;
                return;
            }
            if (res == pc->ro)
            {
                pc->ro = reg;
                return;
            }
        default:
            break;
        }
        pc = pc->next;
    }
}

static czl_var* czl_set_opt_reg(czl_gp *gp, czl_var *res, czl_exp_ele *pc)
{
    czl_var *reg = gp->cur_fun->reg + (res - gp->exp_reg);
    czl_check_opt_regs(pc->next, res, reg);
    return reg;
}

static void czl_set_quick_opt_reg(czl_exp_ele *pc, czl_var *reg, unsigned char *cnt)
{
    *cnt = !*cnt;
    czl_check_opt_regs(pc->next, pc->res, reg+*cnt);
    pc->res = reg+*cnt;
}

static char czl_is_quick_unary_opt(czl_gp *gp, czl_exp_ele *pc, unsigned char tof)
{
    if (pc->lt != CZL_REG_VAR || !pc->lo->permission ||
        (pc->lo->ot != CZL_INT && pc->lo->ot != CZL_FLOAT))
        return 0;

    if (tof ||
        (pc->kind >= CZL_REF_VAR && pc->kind <= CZL_AND_AND) ||
        (CZL_LOGIC_FLIP == pc->kind && pc->lo->ot != CZL_INT))
        return 0;

    if (CZL_ADD_SELF == pc->kind || CZL_DEC_SELF == pc->kind)
    {
        pc->res = pc->lo;
        goto CZL_END;
    }

    if (CZL_FLOAT == pc->lo->ot &&
        (CZL_NUMBER_NOT == pc->kind ||
         CZL_SELF_ADD == pc->kind || CZL_SELF_DEC == pc->kind))
        czl_set_quick_opt_reg(pc, gp->tmp->float_reg, &gp->tmp->float_reg_cnt);
    else
        czl_set_quick_opt_reg(pc, gp->tmp->int_reg, &gp->tmp->int_reg_cnt);

CZL_END:
    pc->flag = pc->kind;
    pc->lt = pc->lo->ot;
    return 1;
}

static char czl_is_quick_binary_opt(czl_gp *gp, czl_exp_ele *pc, unsigned char tof)
{
    if (pc->lt != CZL_REG_VAR || !pc->lo->permission ||
        (pc->lo->ot != CZL_INT && pc->lo->ot != CZL_FLOAT) ||
        pc->rt != CZL_REG_VAR || !pc->ro->permission ||
        (pc->ro->ot != CZL_INT && pc->ro->ot != CZL_FLOAT))
        return 0;

    if (tof || CZL_SWAP == pc->kind || CZL_CMP == pc->kind ||
        (pc->kind >= CZL_ELE_DEL && pc->kind <= CZL_ELE_INX) ||
        (((pc->kind >= CZL_MOD_A && pc->kind <= CZL_R_SHIFT_A) ||
          (pc->kind >= CZL_MOD && pc->kind <= CZL_R_SHIFT)) &&
         (pc->lo->ot != CZL_INT || pc->ro->ot != CZL_INT)))
        return 0;

    if (pc->kind >= CZL_ASS && pc->kind <= CZL_R_SHIFT_A)
    {
        pc->res = pc->lo;
        goto CZL_END;
    }

    if (CZL_FLOAT == pc->lo->ot &&
        (CZL_ADD == pc->kind || CZL_DEC == pc->kind ||
         CZL_MUL == pc->kind || CZL_DIV == pc->kind))
        czl_set_quick_opt_reg(pc, gp->tmp->float_reg, &gp->tmp->float_reg_cnt);
    else
        czl_set_quick_opt_reg(pc, gp->tmp->int_reg, &gp->tmp->int_reg_cnt);

CZL_END:
    pc->flag = pc->kind;
    pc->lt = pc->lo->ot;
    pc->rt = pc->ro->ot;
    return 1;
}

static char czl_three_opt_check(czl_exp_ele *pc)
{
    while (pc && pc->flag != CZL_THREE_OPT)
        pc = pc->next;
    return pc ? 1 : 0;
}

static void czl_set_exp_reg(czl_gp *gp, czl_exp_ele *pc, char flag)
{
    unsigned char tof = 0; //three opt flag

    if (!pc) return;

    if (CZL_EIO(pc))
    {
        pc->res = pc->ro = czl_set_opr_reg(gp, &pc->kind, pc->res, flag);
        return;
    }

    if (flag != 2 && (gp->tmp->int_reg_cnt || gp->tmp->float_reg_cnt))
        tof = czl_three_opt_check(pc); //含有三目运算符的表达式不能优化强数值类型需缓存结果的指令

    do {
        switch (pc->flag)
        {
        case CZL_OPERAND:
            pc->res = pc->ro = czl_set_opr_reg(gp, &pc->kind, pc->res, flag);
            if (flag != 2 && CZL_THREE_END == pc->lt && pc->lo)
                pc->lo = czl_set_opt_reg(gp, pc->lo, pc);
            if (CZL_CONDITION == pc->lt)
                pc->lt = (pc->lo ? CZL_OR_OR : CZL_AND_AND);
            break;
        case CZL_UNARY_OPT:
            pc->lo = czl_set_opr_reg(gp, &pc->lt, pc->lo, flag);
            if (flag != 1 || !czl_is_quick_unary_opt(gp, pc, tof))
            {
                if (pc->kind >= CZL_NUMBER_NOT)
                {
                    pc->flag = CZL_UNARY2_OPT;
                    pc->ro = pc->lo;
                }
                else
                {
                    pc->res = pc->lo;
                }
                if (pc->kind >= CZL_NUMBER_NOT)
                    pc->kind -= CZL_NUMBER_NOT;
                if (flag != 2 && pc->res)
                    pc->res = czl_set_opt_reg(gp, pc->res, pc);
            }
            break;
        case CZL_BINARY_OPT:
            pc->lo = czl_set_opr_reg(gp, &pc->lt, pc->lo, flag);
            pc->ro = czl_set_opr_reg(gp, &pc->rt, pc->ro, flag);
            if (flag != 1 || !czl_is_quick_binary_opt(gp, pc, tof))
            {
                if (CZL_ASS == pc->kind) //把赋值运算符独立处理
                {
                    pc->flag = CZL_ASS_OPT;
                    pc->res = pc->lo;
                }
                else if (pc->res) //把有中间结果的双目运算符独立处理
                {
                    pc->flag = CZL_BINARY2_OPT;
                    if (pc->kind >= CZL_ADD)
                        pc->kind = CZL_ADD_A + (pc->kind - CZL_ADD);
                }
                else
                {
                    pc->res = pc->lo;
                }
                if (pc->kind >= CZL_NUMBER_NOT)
                    pc->kind -= CZL_NUMBER_NOT;
                if (flag != 2 && pc->res)
                    pc->res = czl_set_opt_reg(gp, pc->res, pc);
            }
            break;
        case CZL_THREE_END:
            if (flag != 2 && pc->res)
                pc->res = czl_set_opt_reg(gp, pc->res, pc);
            pc->lo = pc->res;
            break;
        case CZL_CONDITION:
            pc->flag = (pc->lo ? CZL_OR_OR : CZL_AND_AND);
            break;
        case CZL_UNARY2_OPT: //fun(&v)
            pc->ro = pc->lo = czl_set_opr_reg(gp, &pc->lt, pc->lo, flag);
            break;
        default: //CZL_THREE_OPT
            break;
        }
    } while ((pc=pc->next));
}

static void czl_set_reg(czl_gp *gp, czl_sentence *p, char flag)
{
    czl_para *i;
    czl_branch_child *c;

    while (p)
    {
        switch (p->type)
        {
        case CZL_EXP_SENTENCE:
            czl_set_exp_reg(gp, p->sentence.exp, flag);
            break;
        case CZL_BRANCH_BLOCK:
            czl_set_exp_reg(gp, p->sentence.branch->condition, flag);
            czl_set_reg(gp, p->sentence.branch->sentences_head, flag);
            for (c = p->sentence.branch->childs_head; c; c = c->next)
            {
                for (i = c->conditions; i; i = i->next)
                    czl_set_exp_reg(gp, i->para, flag);
                czl_set_reg(gp, c->sentences_head, flag);
            }
            break;
        case CZL_LOOP_BLOCK:
            if (CZL_FOREACH_LOOP == p->sentence.loop->type)
            {
                czl_exp_ele *opr =
                        (czl_exp_ele*)p->sentence.loop->paras_start;
                opr->res = czl_set_opr_reg(gp, &opr->kind, opr->res, 0);
                if (0 == p->sentence.loop->foreach_type)
                {
                    opr = (czl_exp_ele*)p->sentence.loop->paras_end;
                    opr->res = czl_set_opr_reg(gp, &opr->kind, opr->res, 0);
                }
                else //1
                {
                    czl_para *para = p->sentence.loop->paras_end;
                    czl_set_exp_reg(gp, para->para, 0);
                    czl_set_exp_reg(gp, para->next->para, 0);
                    if (para->next->next)
                        czl_set_exp_reg(gp, para->next->next->para, 0);
                }
                if (p->sentence.loop->condition)
                    czl_set_exp_reg(gp, p->sentence.loop->condition, 0);
            }
            else
            {   //CZL_FOR_LOOP/CZL_WHILE_LOOP/CZL_DO_WHILE_LOOP/CZL_TIMER_LOOP
                czl_set_exp_reg(gp, p->sentence.loop->condition, flag);
                for (i = p->sentence.loop->paras_start; i; i = i->next)
                    czl_set_exp_reg(gp, i->para, flag);
                for (i = p->sentence.loop->paras_end; i; i = i->next)
                    czl_set_exp_reg(gp, i->para, flag);
            }
            czl_set_reg(gp, p->sentence.loop->sentences_head, flag);
            break;
        case CZL_TASK_BLOCK:
            czl_set_exp_reg(gp, p->sentence.task->condition, flag);
            czl_set_reg(gp, p->sentence.task->sentences_head, flag);
            break;
        case CZL_TRY_BLOCK:
            for (i = p->sentence.Try->paras; i; i = i->next)
                czl_set_exp_reg(gp, i->para, flag);
            czl_set_reg(gp, p->sentence.Try->sentences_head, flag);
            break;
        case CZL_RETURN_SENTENCE: case CZL_YEILD_SENTENCE:
            czl_set_exp_reg(gp, p->sentence.exp, flag);
            break;
        default:
            //CZL_BREAK_SENTENCE/CZL_CONTINUE_SENTENCE/CZL_GOTO_SENTENCE
            break;
        }
        p = p->next;
    }
}
///////////////////////////////////////////////////////////////
unsigned long czl_get_ast_size
(
    czl_sentence *p,
    unsigned long size
)
{
    czl_para *i;
    czl_branch_child *c;

    while (p)
    {
        switch (p->type)
        {
        case CZL_EXP_SENTENCE:
            size += CZL_EL(p->sentence.exp);
            break;
        case CZL_BRANCH_BLOCK:
            size += CZL_EL(p->sentence.branch->condition);
            ++size; //CZL_BLOCK_BEGIN/CZL_SWITCH_CASE
            if (CZL_IF_BRANCH == p->sentence.branch->type)
            {
                size = czl_get_ast_size(p->sentence.branch->sentences_head, size);
                ++size; //CZL_LOGIC_JUMP
            }
            for (c = p->sentence.branch->childs_head; c; c = c->next)
            {
                i = c->conditions;
                do {
                    if (i) {
                        size += CZL_EL(i->para);
                        ++size; //CZL_BLOCK_BEGIN/CZL_SWITCH_CASE
                        i = i->next;
                    }
                } while (i);
                size = czl_get_ast_size(c->sentences_head, size);
                ++size; //CZL_LOGIC_JUMP
            }
            break;
        case CZL_LOOP_BLOCK:
            if (CZL_FOREACH_LOOP == p->sentence.loop->type)
                size += 2; //CZL_FOREACH_BLOCK
            else if (CZL_TIMER_LOOP == p->sentence.loop->type)
            {
                if (!p->sentence.loop->condition && !p->sentence.loop->task_cnt)
                {
                    size += 2; //CZL_LOGIC_JUMP
                    p->sentence.loop->type = CZL_WHILE_LOOP;
                }
                else
                {
                    size += p->sentence.loop->task_cnt; //CZL_TIMER_INIT
                    size += CZL_EL(p->sentence.loop->condition);
                    ++size; //CZL_TIMER_SLEEP/CZL_LOGIC_JUMP
                }
            }
            else //CZL_FOR_LOOP/CZL_WHILE_LOOP/CZL_DO_WHILE_LOOP
            {
                ++size; //CZL_BLOCK_BEGIN/CZL_LOGIC_JUMP
                size += CZL_EL(p->sentence.loop->condition);
                if (p->sentence.loop->type != CZL_DO_WHILE_LOOP)
                {
                    ++size; //CZL_BLOCK_BEGIN/CZL_LOGIC_JUMP
                    size += CZL_EL(p->sentence.loop->condition);
                }
                for (i = p->sentence.loop->paras_start; i; i = i->next)
                    size += CZL_EL(i->para);
                for (i = p->sentence.loop->paras_end; i; i = i->next)
                    size += CZL_EL(i->para);
            }
            size = czl_get_ast_size(p->sentence.loop->sentences_head, size);
            break;
        case CZL_TASK_BLOCK:
            size += CZL_EL(p->sentence.task->condition);
            ++size; //CZL_TASK_BLOCK
            size = czl_get_ast_size(p->sentence.task->sentences_head, size);
            ++size; //CZL_LOGIC_JUMP
            break;
        case CZL_TRY_BLOCK:
            if (CZL_TRY_CONTINUE == p->sentence.Try->type && !p->sentence.Try->paras)
                ++size; //CZL_LOGIC_JUMP
            else
            {
                for (i = p->sentence.Try->paras; i; i = i->next)
                    size += CZL_EL(i->para);
                size += 2; //CZL_TRY_BLOCK/CZL_LOGIC_JUMP
            }
            size = czl_get_ast_size(p->sentence.Try->sentences_head, size);
            break;
        case CZL_RETURN_SENTENCE: case CZL_YEILD_SENTENCE:
            size += CZL_EL(p->sentence.exp);
            ++size; //CZL_RETURN_SENTENCE/CZL_YEILD_SENTENCE
            break;
        default: //CZL_BREAK_SENTENCE/CZL_CONTINUE_SENTENCE/CZL_GOTO_SENTENCE
            ++size; //CZL_LOGIC_JUMP
            break;
        }
        p = p->next;
    }

    return size;
}
///////////////////////////////////////////////////////////////
void czl_goto_flag_check
(
    czl_goto_flag *flags,
    void *block,
    czl_sentence *sentence,
    czl_exp_ele *head,
    czl_exp_ele *pc
)
{
    while (flags)
    {
        if (block == flags->block)
        {
            if (!sentence || !flags->sentence)
            {
                flags->pc = head;
                return;
            }
            else if (sentence == flags->sentence)
            {
                flags->pc = pc;
                return;
            }
        }
        flags = flags->next;
    }
}

char czl_goto_init(czl_gp *gp, czl_goto *p, czl_goto_flag *flags)
{
    while (p)
    {
        czl_goto_flag *q = flags;
        while (q)
        {
            if (strcmp(p->name, q->name) == 0)
            {
                if (CZL_IS_OJ(q->pc))
                    q->pc = q->pc->pl.pc;
                p->pc->flag = CZL_LOGIC_JUMP; //goto语句必须放这里设置标志位
                p->pc->pl.pc = q->pc;
                if (p->pc->res)
                    ((czl_exp_ele*)p->pc->res)->pl.pc = q->pc;
                break;
            }
            q = q->next;
        }
        if (!q)
        {
            sprintf(gp->log_buf, "undeclared goto_flag %s, ", p->name);
            gp->error_file = p->err_file;
            gp->error_line = p->err_line;
            return 0;
        }
        p = p->next;
    }

    return 1;
}

char czl_try_goto_init(czl_gp *gp, czl_try *p, czl_goto_flag *flags)
{
    while (p)
    {
        czl_goto_flag *q = flags;
        while (q)
        {
            if (strcmp(p->name, q->name) == 0)
            {
                if (CZL_IS_OJ(q->pc))
                    q->pc = q->pc->pl.pc;
                p->pc->pl.pc = q->pc;
                break;
            }
            q = q->next;
        }
        if (!q)
        {
            sprintf(gp->log_buf, "undeclared goto_flag %s, ", p->name);
            gp->error_file = p->err_file;
            gp->error_line = p->err_line;
            return 0;
        }
        p = p->next;
    }

    return 1;
}
///////////////////////////////////////////////////////////////
void czl_set_timer_init(czl_stack_block *b, czl_exp_ele *pc)
{
    for (;;)
    {
        if (CZL_LOOP_BLOCK == b->flag && CZL_TIMER_LOOP == b->type)
        {
            czl_exp_ele *task = ((czl_loop*)b->block)->tasks +
                                ((czl_loop*)b->block)->task_cnt++;
            task->pl.pc = pc;
            task->flag = CZL_TIMER_INIT;
            return;
        }
        --b;
    }
}

void czl_loop_jump_delete(czl_gp *gp, czl_loop_jump *p)
{
    czl_loop_jump *q;
    while (p)
    {
        q = p->next;
        CZL_TMP_FREE(gp, p, sizeof(czl_loop_jump));
        p = q;
    }
}

void czl_set_loop_jump(czl_gp *gp, czl_loop_jump *head, czl_stack_block *b)
{
    czl_loop_jump *p;

    for (;;)
    {
        if (CZL_LOOP_BLOCK == b->flag)
            break;
        --b;
    }

    for (p = head; p; p = p->next)
    {
        czl_exp_ele *pc = (CZL_BREAK_SENTENCE == p->buf->kind ? b->next : b->condition);
        if (p->last)
        {
            if (0XFF == p->last->flag) p->last->pl.pc = pc;
            else p->last->next = p->buf;
        }
        p->buf->pl.pc = pc;
    }

    czl_loop_jump_delete(gp, head);
}

czl_exp_ele* czl_compile_block
(
    czl_gp *gp,
    czl_sentence *p,
    czl_tmp_block *b
)
{
    czl_exp_ele *first = b->buf;
    czl_exp_ele *buf = b->buf;
    czl_stack_block *cur = b->block+b->i;
    czl_exp_ele *save = NULL;
    czl_loop_jump *head = NULL, *node;
    czl_para *i;
    czl_branch_child *c;

    if (!p && cur->goto_flag)
        czl_goto_flag_check(gp->cur_fun->goto_flags, cur->block, p, first, buf);

    while (p)
    {
        switch (p->type)
        {
        case CZL_EXP_SENTENCE:
            CZL_SOCN(b->last, buf);
            CZL_CE(p->sentence.exp, buf);
            b->last = buf-1;
            break;
        case CZL_BRANCH_BLOCK:
            CZL_SOCN(b->last, buf);
            CZL_CE(p->sentence.branch->condition, buf);
            (buf-1)->next = buf;
            p->sentence.branch->block_condition = buf;
            ++buf; //CZL_BLOCK_BEGIN/CZL_SWITCH_CASE
            (buf-1)->next = buf;
            for (c = p->sentence.branch->childs_head; c; c = c->next)
            {
                i = c->conditions;
                do {
                    if (i) {
                        CZL_CE(i->para, buf);
                        (buf-1)->next = buf;
                        ++buf; //CZL_BLOCK_BEGIN/CZL_SWITCH_CASE
                        (buf-1)->next = buf;
                        i = i->next;
                    }
                } while (i);
            }
            p->sentence.branch->block_next = buf;
            b->last = buf-1;
            break;
        case CZL_LOOP_BLOCK:
            if (CZL_TIMER_LOOP == p->sentence.loop->type ||
                CZL_DO_WHILE_LOOP == p->sentence.loop->type)
            {
                if (CZL_TIMER_LOOP == p->sentence.loop->type && p->sentence.loop->task_cnt)
                {
                    unsigned long i;
                    p->sentence.loop->tasks = buf;
                    CZL_SOCN(b->last, buf);
                    for (i = 0; i < p->sentence.loop->task_cnt; ++i)
                    {
                        ++buf; //CZL_TIMER_INIT
                        (buf-1)->next = buf;
                    }
                    p->sentence.loop->task_cnt = 0;
                    b->last = buf-1;
                }
                ++b->i;
                b->block[b->i].flag = CZL_LOOP_BLOCK;
                b->block[b->i].type = p->sentence.loop->type;
                b->block[b->i].goto_flag = p->sentence.loop->goto_flag;
                b->block[b->i].block = p->sentence.loop;
                b->buf = buf;
                if (!(buf=czl_compile_block(gp, p->sentence.loop->sentences_head, b)))
                {
                    czl_loop_jump_delete(gp, head);
                    return NULL;
                }
                --b->i;
            }
            else
            {
                CZL_SOCN(b->last, buf);
                if (CZL_FOREACH_LOOP == p->sentence.loop->type)
                {
                    p->sentence.loop->block_condition = buf;
                    ++buf; //CZL_FOREACH_BLOCK
                }
                else //CZL_FOR_LOOP/CZL_WHILE_LOOP
                {
                    for (i = p->sentence.loop->paras_start; i; i = i->next)
                    {
                        CZL_SOCN(b->last, buf);
                        CZL_CE(i->para, buf);
                        b->last = buf-1;
                    }
                    CZL_SOCN(b->last, buf);
                    if (p->sentence.loop->condition)
                    {
                        CZL_CE(p->sentence.loop->condition, buf);
                        (buf-1)->next = buf;
                    }
                    p->sentence.loop->block_condition = buf;
                    ++buf; //CZL_BLOCK_BEGIN/CZL_LOGIC_JUMP
                }
            }
            p->sentence.loop->block_next = buf;
            b->last = buf-1;
            break;
        case CZL_TASK_BLOCK:
            CZL_SOCN(b->last, buf);
            CZL_CE(p->sentence.task->condition, buf);
            (buf-1)->next = buf;
            p->sentence.task->block_condition = buf;
            czl_set_timer_init(cur, buf);
            ++buf; //CZL_TASK_BEGIN
            (buf-1)->next = buf;
            p->sentence.task->block_next = buf;
            b->last = buf-1;
            break;
        case CZL_TRY_BLOCK:
            ++b->i;
            b->block[b->i].flag = CZL_TRY_BLOCK;
            b->block[b->i].type = p->sentence.Try->type;
            b->block[b->i].goto_flag = p->sentence.Try->goto_flag;
            b->block[b->i].block = p->sentence.Try;
            b->buf = buf;
            if (!(buf=czl_compile_block(gp, p->sentence.Try->sentences_head, b)))
            {
                czl_loop_jump_delete(gp, head);
                return NULL;
            }
            --b->i;
            p->sentence.Try->block_next = buf;
            b->last = NULL;
            break;
        case CZL_RETURN_SENTENCE: case CZL_YEILD_SENTENCE:
            CZL_SOCN(b->last, buf);
            if (!p->sentence.exp)
                buf->res = NULL;
            else
            {
                CZL_CE(p->sentence.exp, buf);
                (buf-1)->next = buf;
                buf->res = &gp->cur_fun->ret;
            }
            buf->flag = p->type;
            buf->ro = &gp->cur_fun->ret;
            b->last = buf;
            ++buf; //CZL_RETURN_SENTENCE/CZL_YEILD_SENTENCE
            break;
        case CZL_BREAK_SENTENCE: case CZL_CONTINUE_SENTENCE:
            if (!(node=CZL_TMP_MALLOC(gp, sizeof(czl_loop_jump))))
            {
                czl_loop_jump_delete(gp, head);
                return NULL;
            }
            node->next = head;
            head = node;
            node->last = b->last;
            node->buf = buf;
            buf->flag = CZL_LOGIC_JUMP;
            buf->kind = p->type;
            buf->rt = 1; //标记为不可优化的跳转指令
            b->last = buf;
            ++buf; //CZL_LOGIC_JUMP
            break;
        default: //CZL_GOTO_SENTENCE
            p->sentence.Goto->pc = buf;
            p->sentence.Goto->next = b->goto_head;
            b->goto_head = p->sentence.Goto;
            CZL_SOCN(b->last, buf);
            if (b->last && 0XFF == b->last->flag)
                buf->res = (czl_var*)b->last;
            b->last = buf;
            ++buf; //CZL_LOGIC_JUMP
            break;
        }
        if (cur->goto_flag)
            czl_goto_flag_check(gp->cur_fun->goto_flags, cur->block, p, first, buf);
        p = p->next;
    }

    if (CZL_LOOP_BLOCK == cur->flag)
    {
        if (CZL_FOREACH_LOOP == cur->type)
        {
            save = cur->condition;
            cur->condition = buf;
        }
        else if (CZL_DO_WHILE_LOOP == cur->type)
        {
            cur->condition = buf;
            CZL_SOCN(b->last, buf);
            CZL_CE(((czl_loop*)cur->block)->condition, buf);
            (buf-1)->next = buf;
            cur->next = buf+1;
        }
        else if (CZL_TIMER_LOOP == cur->type)
        {
            cur->condition = buf;
            CZL_SOCN(b->last, buf);
            if (((czl_loop*)cur->block)->condition)
            {
                CZL_CE(((czl_loop*)cur->block)->condition, buf);
                (buf-1)->next = buf;
            }
            cur->next = buf+1;
        }
        else if (CZL_FOR_LOOP == cur->type)
        {
            save = cur->condition;
            cur->condition = buf;
            for (i = ((czl_loop*)cur->block)->paras_end; i; i = i->next)
            {
                CZL_SOCN(b->last, buf);
                CZL_CE(i->para, buf);
                b->last = buf-1;
            }
        }
        else
            save = cur->condition;
    }

    if (head)
        czl_set_loop_jump(gp, head, cur);

    switch (cur->flag)
    {
    case CZL_FUN_BLOCK:
        if (!gp->cur_fun->return_flag)
        {
            buf->flag = CZL_RETURN_SENTENCE;
            buf->res = NULL;
            buf->ro = &gp->cur_fun->ret;
            CZL_SOCN(b->last, buf);
            ++buf;
        }
        break;
    case CZL_TRY_BLOCK:
        ((czl_try*)cur->block)->block_end = buf;
        buf->flag = CZL_LOGIC_JUMP;
        buf->kind = cur->type;
        buf->lt = 1; //用于异常查找识别
        CZL_SOCN(b->last, buf);
        if (CZL_TRY_CONTINUE == cur->type && !((czl_try*)cur->block)->paras)
            buf->next = NULL;
        else
        {
            buf->next = buf+1;
            ++buf;
            for (i = ((czl_try*)cur->block)->paras; i; i = i->next)
            {
                CZL_CE(i->para, buf);
                (buf-1)->next = buf;
            }
            buf->flag = CZL_TRY_BLOCK;
            buf->kind = cur->type;
            ((czl_try*)cur->block)->block_condition = buf;
        }
        if (CZL_TRY_GOTO == cur->type)
        {
            ((czl_try*)cur->block)->pc = buf;
            ((czl_try*)cur->block)->next = b->try_head;
            b->try_head = ((czl_try*)cur->block);
        }
        ++buf;
        break;
    case CZL_LOOP_BLOCK:
        if (CZL_FOREACH_LOOP == cur->type)
        {
            CZL_SOCN(b->last, buf);
            *buf = *save;
            buf->flag = CZL_FOREACH_BLOCK;
            buf->lt = 0;
        }
        else if (CZL_DO_WHILE_LOOP == cur->type)
        {
            buf->pl.pc = first;
            buf->flag = CZL_BLOCK_BEGIN;
            ((czl_loop*)cur->block)->block_condition = buf;
        }
        else if (CZL_TIMER_LOOP == cur->type)
        {
            buf->pl.pc = first;
            buf->flag = (((czl_loop*)cur->block)->condition ?
                         CZL_TIMER_SLEEP : CZL_LOGIC_JUMP);
            ((czl_loop*)cur->block)->block_condition = buf;
        }
        else
        {
            CZL_SOCN(b->last, save);
            if (CZL_LOGIC_JUMP == save->kind)
                *buf = *save;
            else
            {
                CZL_CE(save, buf);
                *buf = *(save+CZL_EL(save));
            }
            buf->flag = buf->kind;
        }
        ++buf;
        break;
    default:
        if (CZL_TASK_BLOCK == cur->flag || CZL_IF_BRANCH == cur->type || cur->case_end)
            CZL_SOCN(b->last, cur->next)
        else
        {
            CZL_SOCN(b->last, buf);
            buf->rt = 1; //标记case结尾是不可优化的跳转指令
        }
        buf->flag = CZL_LOGIC_JUMP;
        buf->pl.pc = cur->next;
        ++buf;
        break;
    }

    return buf;
}
///////////////////////////////////////////////////////////////
czl_foreach* czl_build_foreach(czl_tmp_block *b, czl_loop *loop)
{
    czl_foreach *f;

    if (0 == loop->foreach_type)
    {
        f = b->foreachs + b->l++;
        f->a = (czl_exp_ele*)loop->paras_start;
        f->a1 = loop->condition;
        f->b = (czl_exp_ele*)loop->paras_end;
        f->c = NULL;
        f->flag = loop->flag;
    }
    else //1
    {
        f = b->foreachs + b->m++;
        f->a = (czl_exp_ele*)loop->paras_start;
        f->a1 = loop->condition;
        f->b = loop->paras_end->para;
        f->c = loop->paras_end->next->para;
        f->para.d = (loop->paras_end->next->next ?
                     loop->paras_end->next->next->para : NULL);
        f->flag = loop->flag;
    }

    return f;
}

czl_exp_ele* czl_opcode_create
(
    czl_gp *gp,
    czl_sentence *p,
    czl_tmp_block *b,
    unsigned char flag
)
{
    czl_exp_ele *s;
    czl_exp_ele *end;
    czl_exp_ele *last;
    czl_branch_child *c;
    czl_stack_block *cur;

    if (flag && !(b->buf=czl_compile_block(gp, p, b)))
        return NULL;

    while (p)
    {
        switch (p->type)
        {
        case CZL_BRANCH_BLOCK:
            s = last = b->last = p->sentence.branch->block_condition;
            if (CZL_IF_BRANCH == p->sentence.branch->type)
            {
                cur = b->block + (++b->i);
                cur->flag = CZL_BRANCH_BLOCK;
                cur->type = CZL_IF_BRANCH;
                cur->goto_flag = p->sentence.branch->goto_flag;
                cur->block = p->sentence.branch;
                cur->next = p->sentence.branch->block_next;
                if (CZL_IS_OJ(cur->next))
                    cur->next = cur->next->pl.pc;
                if (!p->sentence.branch->childs_head)
                    s->next = cur->next;
                //
                s->flag = 0xFF;
                if (!czl_opcode_create(gp, p->sentence.branch->sentences_head, b, 1))
                    return NULL;
                s->flag = CZL_BLOCK_BEGIN;
                --b->i;
            }
            else //CZL_SWITCH_BRANCH
            {
                s->flag = CZL_SWITCH_SENTENCE;
                s->res = gp->cur_fun->reg + gp->cur_fun->reg_cnt - 1;
            }
            //
            end = NULL;
            for (c = p->sentence.branch->childs_head; c; c = c->next)
            {
                czl_para *i;
                //
                cur = b->block + (++b->i);
                cur->flag = CZL_BRANCH_BLOCK;
                cur->type = c->type;
                cur->goto_flag = c->goto_flag;
                cur->case_end = (c->next ? 0 : 1);
                cur->block = c;
                cur->next = p->sentence.branch->block_next;
                if (CZL_IS_OJ(cur->next))
                    cur->next = cur->next->pl.pc;
                //
                if (!c->conditions)
                    b->last = last;
                else
                {
                    s += (CZL_EL(c->conditions->para)+1);
                    last = b->last = s;
                    s->flag = 0xFF;
                }
                if (!czl_opcode_create(gp, c->sentences_head, b, 1))
                    return NULL;
                --b->i;
                i = c->conditions;
                do {
                    if (i)
                    {
                        if (CZL_IF_BRANCH == c->type)
                            s->flag = CZL_BLOCK_BEGIN;
                        else //CZL_SWITCH_BRANCH
                        {
                            s->flag = CZL_CASE_SENTENCE;
                            s->lo = gp->cur_fun->reg + gp->cur_fun->reg_cnt - 1;
                            if (!i->next)
                            {
                                if (end)
                                {
                                    if (CZL_IS_OJ(end+1))
                                        end->pl.pc = (end+1)->pl.pc;
                                    else
                                        end->pl.pc = (end+1);
                                }
                                end = b->buf-1;
                            }
                        }
                        i = i->next;
                        if (i)
                        {
                            s += (CZL_EL(i->para)+1);
                            s->pl.pc = last->pl.pc;
                        }
                        else
                        {
                            last = s;
                            if (!c->next)
                                s->next = cur->next;
                        }
                    }
                } while (i);
            }
            break;
        case CZL_LOOP_BLOCK:
            cur = b->block + (++b->i);
            cur->flag = CZL_LOOP_BLOCK;
            cur->type = p->sentence.loop->type;
            cur->goto_flag = p->sentence.loop->goto_flag;
            cur->block = p->sentence.loop;
            cur->next = p->sentence.loop->block_next;
            if (CZL_IS_OJ(cur->next))
                cur->next = cur->next->pl.pc;
            //
            p->sentence.loop->block_condition->next = cur->next;
            if (CZL_DO_WHILE_LOOP == cur->type || CZL_TIMER_LOOP == cur->type)
            {
                if (!czl_opcode_create(gp, p->sentence.loop->sentences_head, b, 0))
                    return NULL;
            }
            else
            {
                if (CZL_FOREACH_LOOP == cur->type)
                {
                    s = b->last = p->sentence.loop->block_condition;
                    cur->condition = s;
                    s->flag = CZL_FOREACH_BLOCK;
                    s->kind = p->sentence.loop->foreach_type;
                    s->res = (czl_var*)czl_build_foreach(b, p->sentence.loop);
                    s->lt = 1;
                }
                else //CZL_FOR_LOOP/CZL_WHILE_LOOP
                {
                    s = b->last = p->sentence.loop->block_condition;
                    cur->condition = s - CZL_EL(p->sentence.loop->condition);
                    s->flag = (p->sentence.loop->condition ? CZL_BLOCK_BEGIN : CZL_LOGIC_JUMP);
                    s->kind = s->flag;
                    if (!p->sentence.loop->paras_end)
                        cur->type = CZL_WHILE_LOOP;
                }
                flag = s->flag;
                s->flag = 0XFF;
                if (!czl_opcode_create(gp, p->sentence.loop->sentences_head, b, 1))
                    return NULL;
                s->flag = flag;
            }
            --b->i;
            break;
        case CZL_TASK_BLOCK:
            s = b->last = p->sentence.task->block_condition;
            cur = b->block + (++b->i);
            cur->flag = CZL_TASK_BLOCK;
            cur->goto_flag = p->sentence.task->goto_flag;
            cur->block = p->sentence.task;
            cur->next = p->sentence.task->block_next;
            if (CZL_IS_OJ(cur->next))
                cur->next = cur->next->pl.pc;
            //
            p->sentence.task->block_condition->next = cur->next;
            s->flag = 0xFF;
            if (!czl_opcode_create(gp, p->sentence.task->sentences_head, b, 1))
                return NULL;
            s->flag = CZL_TASK_BEGIN;
            --b->i;
            break;
        case CZL_TRY_BLOCK:
            cur = b->block + (++b->i);
            cur->flag = CZL_TRY_BLOCK;
            cur->type = p->sentence.Try->type;
            cur->goto_flag = p->sentence.Try->goto_flag;
            cur->block = p->sentence.Try;
            cur->next = p->sentence.Try->block_next;
            if (CZL_IS_OJ(cur->next))
                cur->next = cur->next->pl.pc;
            //
            p->sentence.Try->block_end->pl.pc = cur->next;
            if (cur->type != CZL_TRY_CONTINUE || p->sentence.Try->paras)
                p->sentence.Try->block_condition->pl.pc = cur->next;
            if (!czl_opcode_create(gp, p->sentence.Try->sentences_head, b, 0))
                return NULL;
            --b->i;
            break;
        default:
            break;
        }
        p = p->next;
    }

    return b->buf;
}
///////////////////////////////////////////////////////////////
char czl_reg_create(czl_gp *gp, czl_fun *fun)
{
    char ret = 1;
	czl_store_device *s;
    czl_loc_var *p;
    czl_var *a = fun->vars;
    czl_var *b = fun->vars + fun->dynamic_vars_cnt;
    for (p = fun->loc_vars; p; p = p->next)
    {
        CZL_TMP_FREE(gp, p->var, strlen((char*)p->var)+1);
        if (CZL_DYNAMIC_VAR == p->quality)
        {
            p->var = a;
            czl_init_var_type(a, p->ot);
            a->quality = p->quality;
            a->permission = p->optimizable;
            (a++)->ot = p->ot;
        }
        else //CZL_STATIC_VAR
        {
            p->var = b;
            b->type = p->type;
            b->val = p->val;
            b->quality = p->quality;
            b->permission = p->optimizable;
            (b++)->ot = p->ot;
        }
    }

    for (s = fun->store_device_head; s; s = s->next)
        for (p = s->vars; p; p = p->next)
        {
            CZL_TMP_FREE(gp, p->var, strlen((char*)p->var)+1);
            if (CZL_DYNAMIC_VAR == p->quality)
            {
                p->var = a;
                czl_init_var_type(a, p->ot);
                a->quality = p->quality;
                a->permission = p->optimizable;
                (a++)->ot = p->ot;
            }
            else //CZL_STATIC_VAR
            {
                p->var = b;
                b->type = p->type;
                b->val = p->val;
                b->quality = p->quality;
                b->permission = p->optimizable;
                (b++)->ot = p->ot;
            }
        }

    return ret;
}

void czl_vars_init(czl_var *p, czl_var *q, unsigned long cnt)
{
    unsigned long i;
    for (i = 0; i < cnt; ++i, ++p, ++q)
    {
        p->ot = q->ot;
        p->quality = q->quality;
    }
}

void czl_save_fun_enum_list(czl_gp *gp, czl_store_device *p)
{
    while (p)
    {
        if (p->enums)
        {
            czl_enum *q = p->enums;
            while (q->next)
                q = q->next;
            q->next = gp->enums_head;
            gp->enums_head = p->enums;
            p->enums = NULL;
        }
        p = p->next;
    }
}

unsigned char czl_check_fun_return(czl_sentence *p)
{
    if (!p) return 0;

    while (p->next)
        p = p->next;

    return CZL_RETURN_SENTENCE == p->type ? 1 : 0;
}

void czl_set_num_reg(czl_gp *gp, czl_fun *fun)
{
    czl_var *reg = fun->reg+fun->reg_cnt-gp->tmp->int_reg_cnt-gp->tmp->float_reg_cnt;

    if (0 == fun->reg_cnt) return;

    if (gp->tmp->int_reg_cnt)
    {
        gp->tmp->int_reg = reg;
        reg->type = reg->ot = CZL_INT;
        (reg++)->permission = 1;
        reg->type = reg->ot = CZL_INT;
        reg->permission = 1;
    }
    if (gp->tmp->float_reg_cnt)
    {
        gp->tmp->float_reg = reg;
        reg->type = reg->ot = CZL_FLOAT;
        (reg++)->permission = 1;
        reg->type = reg->ot = CZL_FLOAT;
        reg->permission = 1;
    }
}

char czl_ast_serialize(czl_gp *gp, czl_fun *fun)
{
	czl_tmp_block b;
	char *opcode;
    unsigned long order_cnt = sizeof(czl_exp_ele) *
                              czl_get_ast_size(fun->sentences_head, 0);
    unsigned long foreach_cnt = fun->foreach_sum*sizeof(czl_foreach);
    unsigned long reg_cnt;

    if (!(fun->return_flag=czl_check_fun_return(fun->sentences_head)))
        order_cnt += sizeof(czl_exp_ele);

    fun->reg_cnt += (fun->switch_flag + gp->tmp->int_reg_cnt + gp->tmp->float_reg_cnt);

    reg_cnt = sizeof(czl_var) * (fun->reg_cnt +
                                 fun->dynamic_vars_cnt +
                                 fun->static_vars_cnt);

    if (fun->yeild_flag)
        reg_cnt += sizeof(czl_var) * (fun->dynamic_vars_cnt +
                                      fun->foreach_cnt);

    fun->opcode_cnt = order_cnt + foreach_cnt + reg_cnt;
    if (!(opcode=(char*)CZL_RT_MALLOC(gp, fun->opcode_cnt)))
        return 0;

    memset(opcode, 0, fun->opcode_cnt); //必须初始化为0
    fun->opcode = (czl_exp_ele*)opcode;

    if (fun->foreach_sum)
        fun->foreachs = (czl_foreach*)(opcode + order_cnt);
    if (fun->reg_cnt)
        fun->reg = (czl_var*)(opcode + order_cnt + foreach_cnt);
    if (fun->dynamic_vars_cnt || fun->static_vars_cnt)
        fun->vars = (czl_var*)(opcode + order_cnt + foreach_cnt) + fun->reg_cnt;

    if (!czl_reg_create(gp, fun))
        return 0;
    czl_set_num_reg(gp, fun); //设置强数值类型临时结果寄存器
    czl_set_reg(gp, fun->sentences_head, 1); //设置函数里所有运算符和变量的reg地址

    if (fun->yeild_flag)
    {
        fun->backup_vars = (czl_var**)((czl_var*)(opcode + order_cnt + foreach_cnt) +
                            fun->reg_cnt + fun->dynamic_vars_cnt + fun->static_vars_cnt);
        czl_vars_init((czl_var*)fun->backup_vars, fun->vars, fun->dynamic_vars_cnt);
    }

    ///////////////////////////////////////////////////////////////

    b.buf = (czl_exp_ele*)opcode;
    b.last = NULL;
    b.goto_head = NULL;
    b.try_head = NULL;
    b.block[0].flag = CZL_FUN_BLOCK;
    b.block[0].goto_flag = fun->goto_flag;
    b.block[0].block = fun;
    b.i = b.l = 0;
    b.m = fun->foreach_cnt;
    b.foreachs = fun->foreachs;

    b.buf = czl_opcode_create(gp, fun->sentences_head, &b, 1);
    if ((char*)b.buf != opcode+order_cnt)
    {
        if (b.buf)
            sprintf(gp->log_buf, "complie function %s error, ", fun->name);
        return 0;
    }

    if (!czl_goto_init(gp, b.goto_head, fun->goto_flags) ||
        !czl_try_goto_init(gp, b.try_head, fun->goto_flags)) //必须先czl_goto_init
        return 0;

    czl_block_delete(gp, CZL_FUN_BLOCK, fun, 1);
    return 1;
}
///////////////////////////////////////////////////////////////
void czl_glo_vars_name_free(czl_gp *gp, czl_glo_var *p)
{
    while (p)
    {
        CZL_TMP_FREE(gp, p->name, strlen(p->name)+1);
        p->name = NULL;
        p = p->next;
    }
}

void czl_enum_names_free(czl_gp *gp, czl_enum *p)
{
    while (p)
    {
        czl_glo_vars_name_free(gp, p->constants_head);
        p = p->next;
    }
}

void czl_vars_name_free(czl_gp *gp)
{
	czl_class *q;

    czl_glo_vars_name_free(gp, gp->vars_head);
    czl_enum_names_free(gp, gp->enums_head);

	for (q = gp->class_head; q; q = q->next)
    {
        czl_class_var *var;
        for (var = q->vars; var; var = var->next)
        {
            var->Name = var->name;
            var->name = NULL;
        }
        czl_enum_names_free(gp, q->enums);
    }
}
///////////////////////////////////////////////////////////////
void czl_nsef_delete(czl_gp *gp, czl_nsef *p)
{
    czl_nsef *q;

    while (p)
    {
        q = p->next;
        CZL_TMP_FREE(gp, p, sizeof(czl_nsef));
        p = q;
    }
}

char czl_nsef_check(czl_gp *gp)
{
    czl_nsef *p = gp->tmp->nsef_head;

    while (p)
    {
        if (p->flag)
        {
            if (CZL_IN_STATEMENT == p->ef->fun->state ||
                !czl_fun_paras_check(gp, p->ef, p->ef->fun))
            {
                sprintf(gp->log_buf, "undefined function %s, ", p->ef->fun->name);
                gp->error_file = p->err_file;
                gp->error_line = p->err_line;
                return 0;
            }
        }
        else
        {
            if (CZL_IN_STATEMENT == ((czl_fun*)p->ef)->state)
            {
                sprintf(gp->log_buf, "undefined function %s, ", ((czl_fun*)p->ef)->name);
                gp->error_file = p->err_file;
                gp->error_line = p->err_line;
                return 0;
            }
        }
        p = p->next;
    }

    return 1;
}

static void czl_for_paras_delete
(
    czl_gp *gp,
    char loop_type,
    char foreach_type,
    czl_exp_stack condition,
    czl_para_list start,
    czl_para_list end,
    char flag
)
{
    if (CZL_FOREACH_LOOP == loop_type)
    {
        if (flag)
        {
            if (foreach_type)
                czl_paras_list_free(gp, end);
        }
        else
        {
            CZL_RT_FREE(gp, start, sizeof(czl_exp_ele));
            CZL_RT_FREE(gp, condition, sizeof(czl_exp_ele));
            if (foreach_type)
                czl_paras_list_delete(gp, end, 0);
            else
                CZL_RT_FREE(gp, end, sizeof(czl_exp_ele));
        }
    }
    else
    {
        czl_exp_stack_delete(gp, condition);
        czl_paras_list_delete(gp, start, 0);
        czl_paras_list_delete(gp, end, 0);
    }
}

char czl_integrity_check(czl_gp *gp, char flag)
{
    if (flag)
    {
        if (!(gp->cur_fun=czl_fun_node_find(CZL_MAIN_FUN_NAME,
                                            &gp->tmp->funs_hash)))
        {
            flag = 0;
            sprintf(gp->log_buf, "less main function, ");
        }
        else
        {
            flag = czl_nsef_check(gp);
        }
    }
    else
    {
        czl_exp_stack_delete(gp, gp->tmp->exp_head);
        czl_paras_list_delete(gp, gp->tmp->try_paras, 0);
        czl_paras_list_delete(gp, gp->tmp->branch_child_paras, 0);
        czl_class_ptr_vars_delete(gp, gp->tmp->class_ptr_vars_head);
        czl_for_paras_delete(gp, gp->tmp->current_loop_type,
                             gp->tmp->foreach_type,
                             gp->tmp->for_condition,
                             gp->tmp->for_paras_start,
                             gp->tmp->for_paras_end, 0);
        if (gp->tmp->goto_flag_name)
            CZL_TMP_FREE(gp, gp->tmp->goto_flag_name,
                         strlen(gp->tmp->goto_flag_name)+1);
    }

    if (!flag)
    {
        char tmp[8];
        switch (gp->exceptionCode)
        {
        case CZL_EXCEPTION_OUT_OF_MEMORY:
            sprintf(gp->log_buf, "out of memory, ");
            break;
        case CZL_EXCEPTION_ORDER_TYPE_NOT_MATCH:
            sprintf(gp->log_buf, "order type not match, ");
            break;
        default:
            break;
        }
        strcat(gp->log_buf, gp->error_file);
        strcat(gp->log_buf, ": ");
        czl_itoa(gp->error_line, tmp);
        strcat(gp->log_buf, tmp);
        strcat(gp->log_buf, "\n");
        czl_log(gp, gp->log_buf);
    }

    //释放运行前无用内存
    czl_vars_name_free(gp);
    czl_nsef_delete(gp, gp->tmp->nsef_head);
    czl_hash_table_delete(gp, &gp->tmp->consts_hash);
    czl_hash_table_delete(gp, &gp->tmp->vars_hash);
    czl_hash_table_delete(gp, &gp->tmp->funs_hash);
    czl_hash_table_delete(gp, &gp->tmp->sn_hash);
    czl_syslib_delete(gp, gp->tmp->syslibs);
    czl_hash_table_delete(gp, &gp->tmp->syslibs_hash);
    czl_hash_table_delete(gp, &gp->tmp->keyword_hash);
    free(gp->tmp); gp->tmp = NULL;

    return flag;
}

char czl_global_vars_init(czl_gp *gp)
{
	char ret = 1;
    czl_glo_sentence *p;

    czl_set_reg(gp, (czl_sentence*)gp->glo_vars_init_head, 2);
    for (p = gp->glo_vars_init_head; p; p = p->next)
    {
        if (p->sentence.exp && !czl_exp_stack_cac(gp, p->sentence.exp))
        {
            ret = 0;
            gp->error_file = p->file;
            break;
        }
    }

    czl_reg_check(gp, gp->exp_reg, CZL_MAX_REG_CNT);
    return ret;
}

char czl_main_run(czl_gp *gp, czl_fun *fun, czl_exp_ele *pc)
{
    if (!czl_fun_run(gp, pc, fun))
        return 0;

    if (gp->yeild_pc)
    {
        fun->pc = gp->yeild_pc;
        gp->yeild_pc = NULL;
        return 2;
    }
    else
        return 1;
}

char czl_first_main_run(czl_gp *gp, czl_fun *fun)
{
    if (fun->enter_vars_count)
    {
    #ifdef CZL_MULT_THREAD
        if (gp->thread_pipe)
        {
            fun->vars->type = gp->enter_var.type;
            fun->vars->val = gp->enter_var.val;
        }
        else
    #endif //#ifdef CZL_MULT_THREAD
        if (!czl_val_copy(gp, fun->vars, &gp->enter_var))
        {
            gp->exit_flag = 1;
            gp->error_line = gp->main_err_line;
            gp->error_file = gp->main_err_file;
            return 0;
        }
        gp->enter_var.type = CZL_INT;
    }

    fun->state = CZL_IN_BUSY;
    return czl_main_run(gp, fun, fun->opcode);
}

char czl_run(czl_gp *gp)
{
	char ret = 0;

#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)
    gp->runtime = CZL_CLOCK; //标记系统开始运行时的时间戳
#endif //#if (defined CZL_SYSTEM_LINUX || defined CZL_SYSTEM_WINDOWS)

    gp->error_file = NULL;
    gp->exceptionCode = CZL_EXCEPTION_NO;

    if (czl_global_vars_init(gp))
        ret = czl_first_main_run(gp, gp->cur_fun);

    if (gp->exit_flag)
    {
        sprintf(gp->log_buf, "exit(%d-%d), %s: %ld\n",
                              gp->exit_code,
                              gp->exceptionCode,
                              gp->error_file,
                              gp->error_line);
        czl_log(gp, gp->log_buf);
    }

    return ret;
}

#ifndef CZL_CONSOLE
char czl_resume_shell(czl_gp *gp, czl_fun *fun)
{
	char ret;

    if (fun->ret.type != CZL_INT)
    {
        czl_val_del(gp, &fun->ret);
        fun->ret.type = CZL_INT;
    }

    ret = czl_main_run(gp, fun, fun->pc);

    if (gp->exit_flag)
    {
        sprintf(gp->log_buf, "exit(%d-%d), %s: %ld\n",
                              gp->exit_code,
                              gp->exceptionCode,
                              gp->error_file,
                              gp->error_line);
        czl_log(gp, gp->log_buf);
    }

    return ret;
}

czl_tabkv* czl_tabkv_create
(
    czl_gp *gp,
    czl_table *tab,
    int key
)
{
    if (tab->count+1 > tab->size &&
        !czl_hash_size_update(gp, tab->size<<1, tab))
        return NULL;

    return czl_create_key_num_tabkv(gp, tab, key, NULL);
}

czl_tabkv* czl_tabkv_find
(
    czl_table *tab,
    int key
)
{
    return czl_find_num_tabkv(tab, key);
}

char czl_tabkv_delete
(
    czl_gp *gp,
    czl_table *tab,
    int key
)
{
    czl_tabkv *p;
    long index; //必须为有符号Long型
    CZL_INDEX_CAC(index,
                  czl_num_hash(key, tab->key, tab->attack_cnt),
                  tab->mask);

    if (!(p=czl_get_num_tabkv(tab->buckets[index], key)))
        return 0;

    return czl_delete_tabkv_node(gp, tab, p, index);
}
#endif //#ifndef CZL_CONSOLE
///////////////////////////////////////////////////////////////
#if ((defined CZL_MULT_THREAD) && (defined CZL_CONSOLE))
unsigned char czl_print_lock_init; //控制台输出锁初始化标志位
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION czl_print_cs; //控制台输出锁
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t czl_print_mutex; //控制台输出锁
#endif

void czl_print_lock(void)
{
#ifdef CZL_SYSTEM_WINDOWS
    EnterCriticalSection(&czl_print_cs);
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_lock(&czl_print_mutex);
#endif
}

void czl_print_unlock(void)
{
#ifdef CZL_SYSTEM_WINDOWS
    LeaveCriticalSection(&czl_print_cs);
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_unlock(&czl_print_mutex);
#endif
}
#endif //#if ((defined CZL_MULT_THREAD) && (defined CZL_CONSOLE))

void czl_log(czl_gp *gp, char *log)
{
#ifdef CZL_CONSOLE
    #ifdef CZL_MULT_THREAD
        czl_print_lock();
    #endif
    fprintf(stdout, "%s", log);
    #ifdef CZL_MULT_THREAD
        czl_print_unlock();
    #endif
#else
	FILE *fout;
    if (!gp->log_path)
        return;
    if ((fout=fopen(gp->log_path, "a")))
    {
        time_t now;
        struct tm *tm_now;
        time(&now);
        tm_now = localtime(&now);
        fprintf(fout, "[%d-%02d-%02d %02d:%02d:%02d] %s",
                tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday,
                tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, log);
        fclose(fout);
    }
#endif
}
///////////////////////////////////////////////////////////////
void czl_block_delete(czl_gp *gp, char type, void *block, char flag)
{
    czl_sentence *p, *q, *head;
    czl_fun *fun;
    czl_branch *branch;
    czl_branch_child *branch_child, *next_child;
    czl_loop *loop;
    czl_task *task;
    czl_try *Try;
    unsigned int block_size;

	if (!block) return;

    switch (type)
    {
    case CZL_FUN_BLOCK:
        fun = (czl_fun*)block;
        if (CZL_SYS_FUN == fun->type)
        {
            czl_para_explain_list_delete(gp, fun->para_explain);
            if (fun->dynamic_vars_cnt >= 0)
                CZL_RT_FREE(gp, fun->vars,
                            fun->dynamic_vars_cnt*sizeof(czl_var));
            CZL_RT_FREE(gp, block, CZL_GFNS(fun->type));
            return;
        }
        //
        czl_goto_list_delete(gp, fun->goto_flags);
        czl_store_device_delete(gp, fun);
        czl_loc_var_list_delete(gp, fun->loc_vars, fun->vars);
        //
        block_size = CZL_GFNS(fun->type);
        head = fun->sentences_head;
        break;
    case CZL_BRANCH_BLOCK:
        block_size = sizeof(czl_branch);
        branch = (czl_branch*)block;
        head = branch->sentences_head;
        czl_exp_stack_delete(gp, branch->condition);
        break;
    case CZL_BRANCH_CHILD_BLOCK:
        block_size = sizeof(czl_branch_child);
        branch_child = (czl_branch_child*)block;
        head = branch_child->sentences_head;
        czl_paras_list_delete(gp, branch_child->conditions, 0);
        break;
    case CZL_LOOP_BLOCK:
        block_size = sizeof(czl_loop);
        loop = (czl_loop*)block;
        head = loop->sentences_head;
        czl_for_paras_delete(gp, loop->type, loop->foreach_type,
                             loop->condition, loop->paras_start,
                             loop->paras_end, flag);
        break;
    case CZL_TASK_BLOCK:
        block_size = sizeof(czl_task);
        task = (czl_task*)block;
        head = task->sentences_head;
        czl_exp_stack_delete(gp, task->condition);
        break;
    default: //CZL_TRY_BLOCK
        block_size = sizeof(czl_try);
        Try = (czl_try*)block;
        head = Try->sentences_head;
        czl_paras_list_delete(gp, Try->paras, 0);
        if (CZL_TRY_GOTO == Try->type)
            CZL_TMP_FREE(gp, Try->name, strlen(Try->name)+1);
        break;
    }

    p = head;
    while (p)
    {
        switch (p->type)
        {
        case CZL_EXP_SENTENCE:
        case CZL_RETURN_SENTENCE: case CZL_YEILD_SENTENCE:
            czl_exp_stack_delete(gp, p->sentence.exp);
            break;
        case CZL_BRANCH_BLOCK:
            branch_child = p->sentence.branch->childs_head;
            czl_block_delete(gp, CZL_BRANCH_BLOCK, p->sentence.branch, flag);
            while (branch_child)
            {
                next_child = branch_child->next;
                czl_block_delete(gp, CZL_BRANCH_CHILD_BLOCK, branch_child, flag);
                branch_child = next_child;
            }
            break;
        case CZL_LOOP_BLOCK:
            czl_block_delete(gp, CZL_LOOP_BLOCK, p->sentence.loop, flag);
            break;
        case CZL_TASK_BLOCK:
            czl_block_delete(gp, CZL_TASK_BLOCK, p->sentence.task, flag);
            break;
        case CZL_TRY_BLOCK:
            czl_block_delete(gp, CZL_TRY_BLOCK, p->sentence.Try, flag);
            break;
        case CZL_GOTO_SENTENCE:
            CZL_TMP_FREE(gp, p->sentence.Goto->name,
                         strlen(p->sentence.Goto->name)+1);
            CZL_TMP_FREE(gp, p->sentence.Goto, sizeof(czl_goto));
            break;
        default: //CZL_BREAK_SENTENCE、CZL_CONTINUE_SENTENCE
            break;
        }
        p = p->next;
    }

    p = head;
    while (p)
    {
        q = p->next;
        CZL_TMP_FREE(gp, p, sizeof(czl_sentence));
        p = q;
    }

    if (type != CZL_FUN_BLOCK)
        CZL_TMP_FREE(gp, block, block_size);
}

static void czl_foreach_paras_delete
(
    czl_gp *gp,
    czl_foreach *f,
    unsigned long cnt,
    unsigned long sum
)
{
    unsigned long i = 0;
    while (i < cnt)
    {
        CZL_RT_FREE(gp, f->a, sizeof(czl_exp_ele));
        CZL_RT_FREE(gp, f->a1, sizeof(czl_exp_ele));
        CZL_RT_FREE(gp, f->b, sizeof(czl_exp_ele));
        ++f; ++i;
    }
    while (i < sum)
    {
        CZL_RT_FREE(gp, f->a, sizeof(czl_exp_ele));
        CZL_RT_FREE(gp, f->a1, sizeof(czl_exp_ele));
        czl_exp_stack_delete(gp, f->b);
        czl_exp_stack_delete(gp, f->c);
        czl_exp_stack_delete(gp, f->para.d);
        ++f; ++i;
    }
}

void czl_fun_list_delete(czl_gp *gp, czl_fun *p)
{
    czl_fun *q;

    while (p)
    {
        q = p->next;

        czl_val_del(gp, &p->ret);
        czl_backup_vars_delete(gp, p);
        czl_para_explain_list_delete(gp, p->para_explain);
        if (CZL_SYS_FUN == p->type)
        {
            if (p->dynamic_vars_cnt >= 0)
                czl_vars_delete(gp, p->vars, p->dynamic_vars_cnt, 1);
        }
        else
        {
            CZL_RT_FREE(gp, p->name, strlen(p->name)+1);
            czl_class_ptr_vars_delete(gp, p->class_vars);
            if (p->opcode)
            {
                if (p->yeild_flag)
                {
                    czl_vars_delete(gp, (czl_var*)p->backup_vars,
                                    p->dynamic_vars_cnt, 0);
                }
                czl_foreach_paras_delete(gp, p->foreachs,
                                         p->foreach_cnt, p->foreach_sum);
                czl_vars_delete(gp, p->vars,
                                p->dynamic_vars_cnt+p->static_vars_cnt, 0);
                czl_reg_check(gp, p->reg, p->reg_cnt);
                CZL_RT_FREE(gp, p->opcode, p->opcode_cnt);
            }
            else
                czl_block_delete(gp, CZL_FUN_BLOCK, p, 0);
        }
        CZL_RT_FREE(gp, p, CZL_GFNS(p->type));

        p = q;
    }
}

void czl_class_list_delete(czl_gp *gp, czl_class *p)
{
    czl_class *q;

    while (p)
    {
        q = p->next;
        CZL_RT_FREE(gp, p->name, strlen(p->name)+1);
        czl_class_var_list_delete(gp, p->vars);
        czl_enum_list_delete(gp, p->enums);
        czl_fun_list_delete(gp, p->funs);
        czl_class_parent_list_delete(gp, p->parents);
        czl_hash_table_delete(gp, &p->vars_hash);
        czl_hash_table_delete(gp, &p->funs_hash);
        CZL_RT_FREE(gp, p, sizeof(czl_class));
        p = q;
    }
}

void czl_coroutine_list_delete(czl_gp *gp, void **p)
{
    void **q;

    while (p)
    {
        czl_coroutine *c = CZL_COR(p);
        q = c->next;
        if (c->vars)
        {
            czl_vars_delete(gp, c->vars, c->fun->dynamic_vars_cnt, 0);
            CZL_STACK_FREE(gp, c->vars, (c->fun->dynamic_vars_cnt+
                                         c->fun->foreach_cnt)*sizeof(czl_var));
        }
        CZL_COR_FREE(gp, p);
        p = q;
    }
}

#ifdef CZL_MULT_THREAD
void czl_thread_list_delete(czl_gp *gp, czl_thread *p)
{
    czl_thread *q;
    while (p)
    {
        q = p->next;
    #ifdef CZL_SYSTEM_WINDOWS
        CloseHandle(p->id);
    #endif
        if (p->pipe->alive)
        {
            p->pipe->alive = 0;
            czl_event_send(&p->pipe->notify_event);
        }
        else
        {
            czl_thread_pipe_delete(p->pipe);
        }
        CZL_THREAD_FREE(gp, p);
        p = q;
    }
}
#endif //#ifdef CZL_MULT_THREAD

void czl_glo_vars_init_sentences_delete(czl_gp *gp, czl_glo_sentence *p)
{
    czl_glo_sentence *q;

    while (p)
    {
        q = p->next;
        czl_exp_stack_delete(gp, p->sentence.exp);
        CZL_TMP_FREE(gp, p, sizeof(czl_glo_sentence));
        p = q;
    }
}

void czl_shell_free(czl_gp *gp)
{
    free(gp->tmp);
    czl_shell_name_delete(gp, gp->sn_head);
    czl_glo_vars_init_sentences_delete(gp, gp->glo_vars_init_head);
    czl_expfun_delete(gp, gp->expfun_head);
    czl_member_delete(gp, gp->member_head);
    czl_eles_delete(gp, gp->eles_head);
    czl_var_list_delete(gp, gp->consts_head);
    czl_var_list_delete(gp, gp->vars_head);
    czl_enum_list_delete(gp, gp->enums_head);
    czl_fun_list_delete(gp, gp->funs_head);
    czl_class_list_delete(gp, gp->class_head);
    czl_hash_table_delete(gp, &gp->class_hash);
    czl_hash_table_delete(gp, &gp->coroutines_hash);
    czl_coroutine_list_delete(gp, gp->coroutines_head);
#ifdef CZL_MULT_THREAD
    czl_hash_table_delete(gp, &gp->threads_hash);
    czl_thread_list_delete(gp, gp->threads_head);
#endif //#ifdef CZL_MULT_THREAD
}

#ifdef CZL_MM_MODULE
void czl_mm_free(czl_gp *gp)
{
    char *tmp;
    czl_class *c;
    czl_mm_sp *t;
    unsigned long i;
    unsigned char id;
    czl_mm_sys_heap *p, *q;

    for (c = gp->class_head; c; c = c->next)
        czl_mm_sp_destroy(gp, &c->pool);

    for (t = gp->mmp_tab.head; t; t = t->next)
        for (id = t->useHead; id; id = tmp[2])
        {
			czl_table *tab;
        #ifdef CZL_MM_RT_GC
            tmp = CZL_MM_OBJ_HEAP_GET(t->heap, id, gp->mmp_tab.max);
            tab = (czl_table*)(tmp+CZL_MM_OBJ_HEAP_MSG_SIZE);
        #else
            tmp = CZL_MM_BUF_HEAP_GET(t->heap, id, gp->mmp_tab.max);
            tab = (czl_table*)(tmp+CZL_MM_BUF_HEAP_MSG_SIZE);
        #endif //#ifdef CZL_MM_RT_GC
            czl_mm_sp_destroy(gp, &tab->pool);
        }

    for (t = gp->mmp_sq.head; t; t = t->next)
        for (id = t->useHead; id; id = tmp[2])
        {
			czl_sq *sq;
        #ifdef CZL_MM_RT_GC
            tmp = CZL_MM_OBJ_HEAP_GET(t->heap, id, gp->mmp_sq.max);
            sq = (czl_sq*)(tmp+CZL_MM_OBJ_HEAP_MSG_SIZE);
        #else
            tmp = CZL_MM_BUF_HEAP_GET(t->heap, id, gp->mmp_sq.max);
            sq = (czl_sq*)(tmp+CZL_MM_BUF_HEAP_MSG_SIZE);
        #endif //#ifdef CZL_MM_RT_GC
            czl_mm_sp_destroy(gp, &sq->pool);
        }

    for (t = gp->mmp_file.head; t; t = t->next)
        for (id = t->useHead; id; id = tmp[2])
        {
            czl_file *file;
        #ifdef CZL_MM_RT_GC
            tmp = CZL_MM_OBJ_HEAP_GET(t->heap, id, gp->mmp_file.max);
            file = (czl_file*)(tmp+CZL_MM_OBJ_HEAP_MSG_SIZE);
        #else
            tmp = CZL_MM_BUF_HEAP_GET(t->heap, id, gp->mmp_file.max);
            file = (czl_file*)(tmp+CZL_MM_BUF_HEAP_MSG_SIZE);
        #endif //#ifdef CZL_MM_RT_GC
            fclose(file->fp);
        }

    for (i = 0; i < CZL_MM_SP_RANGE; ++i)
    {
        czl_mm_sp_destroy(gp, gp->mmp_tmp+i);
        czl_mm_sp_destroy(gp, gp->mmp_rt+i);
        czl_mm_sp_destroy(gp, gp->mmp_stack+i);
        czl_mm_sp_destroy(gp, gp->mmp_str+i);
    }

    czl_mm_sp_destroy(gp, &gp->mmp_obj);
    czl_mm_sp_destroy(gp, &gp->mmp_tab);
    czl_mm_sp_destroy(gp, &gp->mmp_arr);
    czl_mm_sp_destroy(gp, &gp->mmp_sq);
    czl_mm_sp_destroy(gp, &gp->mmp_cor);
    czl_mm_sp_destroy(gp, &gp->mmp_file);
    czl_mm_sp_destroy(gp, &gp->mmp_ref);

#ifdef CZL_MULT_THREAD
    czl_mm_sp_destroy(gp, &gp->mmp_thread);
#endif //#ifdef CZL_MULT_THREAD

#ifdef CZL_MM_CACHE
    czl_mm_cache_pools_destroy(gp, &gp->mm_cache);
#endif //#ifdef CZL_MM_CACHE

    for (p = gp->mmp_sh_head; p; p = q)
    {
        gp->mm_cnt -= p->size;
        q = p->next;
        free(p);
    }
}
#endif //#ifdef CZL_MM_MODULE

void czl_memory_free(czl_gp *gp)
{
#ifdef CZL_MM_MODULE
    free(gp->tmp);
    czl_mm_free(gp);
    #ifndef CZL_CONSOLE
        gp->table = NULL;
    #endif //#ifndef CZL_CONSOLE
#else //用于测试内存是否泄漏
    czl_shell_free(gp);
    czl_val_del(gp, &gp->enter_var);
    CZL_STACK_FREE(gp, gp->ch_head, sizeof(czl_char_var));
    #ifndef CZL_CONSOLE
        czl_table_delete(gp, gp->table);
        gp->table = NULL;
    #endif //#ifndef CZL_CONSOLE
#endif //#ifdef CZL_MM_MODULE

    czl_mm_print(gp); //打印内存统计信息用于检查是否发生内存泄露
}

void czl_init_free(czl_gp *gp, char flag)
{
    if (gp->tmp)
    {
        czl_syslib_delete(gp, gp->tmp->syslibs);
        czl_hash_table_delete(gp, &gp->tmp->syslibs_hash);
        czl_hash_table_delete(gp, &gp->tmp->keyword_hash);
        free(gp->tmp);
    }
    if (flag)
    {
        CZL_STACK_FREE(gp, gp->ch_head, sizeof(czl_char_var));
    #ifndef CZL_CONSOLE
        if (gp->table)
        {
            czl_table_delete(gp, gp->table);
            gp->table = NULL;
        }
    #endif //#ifndef CZL_CONSOLE
    }
}

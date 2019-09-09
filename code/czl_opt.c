#include "czl_opt.h"
#include "czl_lib.h"

#ifdef CZL_LIB_TCP
#include "czl_tcp.h" //TCP库
#endif //CZL_LIB_TCP

//////////////////////////////////////////////////////////////////
char czl_number_not_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    switch ((res->type=opr->type)) // -
    {
    case CZL_INT: res->val.inum = -opr->val.inum; return 1;
    case CZL_FLOAT: res->val.fnum = -opr->val.fnum; return 1;
    default: return 0;
    }
}

char czl_logic_not_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    res->type = CZL_INT; // !
    res->val.inum = !CZL_EIT(opr);
    return 1;
}

char czl_logic_flit_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    if (opr->type != CZL_INT) // ~
        return 0;

    res->type = CZL_INT;
    res->val.inum = ~opr->val.inum;
    return 1;
}

char czl_self_add_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    switch ((res->type=opr->type)) // i++
    {
    case CZL_INT: res->val.inum = opr->val.inum++; return 1;
    case CZL_FLOAT: res->val.fnum = opr->val.fnum++; return 1;
    default: return 0;
    }
}

char czl_self_dec_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    switch ((res->type=opr->type)) // i--
    {
    case CZL_INT: res->val.inum = opr->val.inum--; return 1;
    case CZL_FLOAT: res->val.fnum = opr->val.fnum--; return 1;
    default: return 0;
    }
}

char czl_addr_obj_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    res->type = CZL_OBJ_REF; // &i
    res->quality = CZL_REF_ELE;
    if (CZL_REF_ELE == opr->quality)
    {
        res->val.ref.inx = opr->val.inum;
        res->val.ref.var = (czl_var*)opr->name;
    }
    else
    {
        res->val.ref.inx = -1;
        res->val.ref.var = opr;
    }
    return 1;
}

char czl_obj_cnt_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    res->type = CZL_INT;
    switch (opr->type) // #
    {
    case CZL_STRING:
        res->val.inum = CZL_STR(opr->val.str.Obj)->len;
        break;
    case CZL_TABLE:
        res->val.inum = CZL_TAB(opr->val.Obj)->count;
        break;
    case CZL_ARRAY:
        res->val.inum = CZL_ARR(opr->val.Obj)->cnt;
        break;
    case CZL_STACK: case CZL_QUEUE:
        res->val.inum = CZL_SQ(opr->val.Obj)->count;
        break;
    case CZL_FILE:
        res->val.inum = czl_get_file_size(CZL_FIL(opr->val.Obj)->fp);
        break;
    case CZL_SOURCE:
    #ifdef CZL_LIB_TCP
        if (czl_lib_tcp == CZL_SRC(opr->val.Obj)->lib)
        {
            czl_tcp_handle *h = CZL_SRC(opr->val.Obj)->src;
            if (CZL_TCP_SRV_MASTER == h->type || CZL_TCP_SRV_WORKER == h->type)
                res->val.inum = CZL_TAB(h->obj)->count;
            else
                res->val.inum = 1;
        }
        else
    #endif
            res->val.inum = 1;
        break;
    default:
        res->val.inum = 1;
        break;
    }
    return 1;
}
//////////////////////////////////////////////////////////////////
char czl_swap_cac_num_type(czl_var *left, czl_var *right)
{
    czl_value tmp;

    switch (left->type)
    {
    case CZL_INT:
        if (right->type != CZL_FLOAT)
            return 0;
        if (CZL_NIL == left->ot)
        {
            left->type = CZL_FLOAT;
            tmp.fnum = right->val.fnum;
            right->val.fnum = left->val.inum;
            left->val.inum = tmp.fnum;
        }
        else if (CZL_NIL == right->ot)
        {
            right->type = CZL_INT;
            tmp.inum = right->val.fnum;
            right->val.inum = left->val.inum;
            left->val.inum= tmp.inum;
        }
        else
        {
            tmp.inum = right->val.fnum;
            right->val.fnum = left->val.inum;
            left->val.inum = tmp.inum;
        }
        return 1;
    case CZL_FLOAT:
        if (right->type != CZL_INT)
            return 0;
        if (CZL_NIL == left->ot)
        {
            left->type = CZL_INT;
            tmp.inum = right->val.inum;
            right->val.inum = left->val.fnum;
            left->val.inum = tmp.inum;
        }
        else if (CZL_NIL == right->ot)
        {
            right->type = CZL_FLOAT;
            tmp.fnum = right->val.inum;
            right->val.fnum = left->val.fnum;
            left->val.fnum = tmp.fnum;
        }
        else
        {
            tmp.inum = right->val.fnum;
            right->val.fnum = left->val.inum;
            left->val.inum = tmp.inum;
        }
        return 1;
    default:
        return 0;
    }
}

char czl_swap_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    // ><
    if (left->type == right->type ||
        (CZL_NIL == left->ot && CZL_NIL == right->ot))
    {
        if ((CZL_INT == left->type ||
             CZL_FLOAT == left->type ||
             CZL_STRING == left->type) &&
            (CZL_INT == right->type ||
             CZL_FLOAT == right->type ||
             CZL_STRING == right->type))
        {
			czl_value tmp;
            CZL_VAL_SWAP(left->val, right->val, tmp);
            if (left->type != right->type)
                CZL_INT_SWAP(left->type, right->type);
            return 1;
        }
        return 0;
    }

    return czl_swap_cac_num_type(left, right);
}

char czl_new_handle(czl_gp *gp, czl_var *left, czl_new_sentence *New)
{
	czl_var res;

    if (left->ot != CZL_NIL && left->ot != New->type)
        return 0;

    if (!czl_val_del(gp, left))
        return 0;
    left->type = CZL_INT;

    if (!czl_obj_new(gp, New, &res))
        return 0;

    left->type = res.type;
    left->val = res.val;
    return 1;
}

char czl_addr_obj_ass_handle(czl_gp *gp, czl_var *left, czl_var *right)
{
	czl_var *var;

    right->quality = CZL_DYNAMIC_VAR;

    if (CZL_NULL == right->ot) // = null
    {
        if (!czl_val_del(gp, left))
            return 0;
        left->type = CZL_INT;
        left->val.inum = 0;
        return 1;
    }

    var = CZL_GRV(right);
    if (left == var) //避免自引用问题: a = &a
        return 1;

    switch (left->ot)
    {
    case CZL_NIL:
        break;
    case CZL_NUM:
        switch (var->type) {
        case CZL_INT: case CZL_FLOAT: break;
        default: return 0;
        }
        break;
    default:
        if (left->ot > CZL_NIL)
        {
            if (var->type != CZL_INSTANCE ||
                left->ot != CZL_INS(var->val.Obj)->pclass->ot_num)
                return 0;
        }
        else if (left->ot != var->type)
            return 0;
        break;
    }

    if (left->type != CZL_OBJ_REF)
    {
		char ret;
        if (left->name && ((czl_ref_obj*)left->name)->cnt <= CZL_MAX_MEMBER_INDEX_LAYER)
            czl_ref_obj_delete(gp, left);
        if (var->quality != CZL_OBJ_ELE)
            ret = czl_val_del(gp, left);
        else //处理 a = &a[0] 问题
        {
            CZL_LOCK_OBJ(var);
            ret = czl_val_del(gp, left);
            CZL_UNLOCK_OBJ(var);
        }
        if (!ret)
            return 0;
    }

    if (!czl_ref_copy(gp, left, var))
        return 0;

    left->val.ref = right->val.ref;
    if (left->type != CZL_OBJ_REF)
        left->type = CZL_OBJ_REF;

    return 1;
}

char czl_arr_tab_list_ass(czl_gp *gp, czl_var *left, czl_var *right)
{
    void **obj;

    if (!czl_val_del(gp, left))
        return 0;
    left->type = CZL_INT;

    if (CZL_ARRAY_LIST == right->type)
    {
        switch (left->ot)
        {
        case CZL_NIL: case CZL_ARRAY:
            return czl_array_new(gp, NULL, CZL_ARR_LIST(right->val.Obj), left);
        case CZL_STACK: case CZL_QUEUE:
            if (!(obj=czl_sq_new(gp, NULL, CZL_ARR_LIST(right->val.Obj))))
                return 0;
            left->type = left->ot;
            left->val.Obj = obj;
            return 1;
        default:
            return 0;
        }
    }
    else
    {
        if (left->ot != CZL_NIL && left->ot != CZL_TABLE)
            return 0;
        return czl_table_new(gp, CZL_TAB_LIST(right->val.Obj), left);
    }
}

char czl_ass_cac_diff_type(czl_gp *gp, czl_var *left, czl_var *right)
{
	czl_var tmp;

    switch (right->type)
    {
    case CZL_INT:
        switch (left->ot)
        {
        case CZL_NIL:
            break;
        case CZL_NUM:
            left->type = right->type;
            left->val = right->val;
            return 1;
        case CZL_INT:
            left->val = right->val;
            return 1;
        case CZL_FLOAT:
            left->type = CZL_FLOAT;
            left->val.fnum = right->val.inum;
            return 1;
        default:
            return 0;
        }
        break;
    case CZL_FLOAT:
        switch (left->ot)
        {
        case CZL_NIL:
            break;
        case CZL_NUM:
            left->type = right->type;
            left->val = right->val;
            return 1;
        case CZL_INT:
            left->val.inum = right->val.fnum;
            return 1;
        case CZL_FLOAT:
            left->type = CZL_FLOAT;
            left->val = right->val;
            return 1;
        default:
            return 0;
        }
        break;
    case CZL_NIL:
        if (!czl_val_del(gp, left))
            return 0;
        left->type = CZL_INT;
        left->val.inum = 0;
        return 1;
    case CZL_NEW:
        return czl_new_handle(gp, left, (czl_new_sentence*)right->val.Obj);
    case CZL_OBJ_REF:
        return czl_addr_obj_ass_handle(gp, left, right);
    case CZL_ARRAY_LIST: case CZL_TABLE_LIST:
        return czl_arr_tab_list_ass(gp, left, right);
    case CZL_FUN_REF:
        if (left->ot != CZL_NIL && left->ot != right->type)
            return 0;
        break;
    default:
        if (left->ot != CZL_NIL && left->ot != right->type)
        {
            if (CZL_CIRCLE_REF_VAR == right->quality)
            {
                czl_val_del(gp, right);
                right->quality = CZL_DYNAMIC_VAR;
            }
            return 0;
        }
        switch (right->quality)
        {
        case CZL_FUNRET_VAR:
            if (!czl_val_del(gp, left))
                return 0;
            left->type = right->type;
            left->val = right->val;
            right->type = CZL_INT;
            return 1;
        case CZL_ARRBUF_VAR:
            right->quality = CZL_DYNAMIC_VAR;
            if (CZL_STRING == right->type)
                return czl_strbuf_copy(gp, left);
            else
            {
                if (!czl_val_del(gp, left))
                {
                    CZL_SF(gp, right->val.str);
                    return 0;
                }
                left->type = right->type;
                left->val = right->val;
                return 1;
            }
        case CZL_CIRCLE_REF_VAR:
            right->quality = CZL_DYNAMIC_VAR;
            if (!czl_val_del(gp, left))
            {
                czl_val_del(gp, right);
                return 0;
            }
            left->type = right->type;
            left->val = right->val;
            return 1;
        default:
            if (CZL_STRING == right->type)
            {
                czl_var tmp = *right; //tmp 用于处理 a = a.v 情况
                CZL_SRCA1(tmp.val.str);
                if (!czl_val_del(gp, left))
                {
                    CZL_SRCD1(gp, tmp.val.str);
                    return 0;
                }
                left->type = tmp.type;
                left->val = tmp.val;
                return 1;
            }
            else if (right->type != CZL_FILE &&
                     right->type != CZL_SOURCE &&
                     CZL_INS(right->val.Obj)->rf)
                return czl_obj_fork(gp, left, right);
            else
            {
                czl_var tmp = *right; //tmp 用于处理 a = a.v 情况
                CZL_ORCA1(tmp.val.Obj);
                if (!czl_val_del(gp, left))
                {
                    CZL_ORCD1(tmp.val.Obj);
                    return 0;
                }
                left->type = tmp.type;
                left->val = tmp.val;
                return 1;
            }
        }
    }

    if (CZL_INT == left->type || CZL_FLOAT == left->type)
    {
        left->type = right->type;
        left->val = right->val;
        return 1;
    }

    tmp = *right; //tmp 用于处理 a = a.v 情况
    if (!czl_val_del(gp, left))
        return 0;
    left->type = tmp.type;
    left->val = tmp.val;
    return 1;
}

char czl_ass_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    if (left->type != right->type || (left->ot != CZL_NIL && left->ot != left->type)) // =
        return czl_ass_cac_diff_type(gp, left, right);

    switch (left->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_FUN_REF:
        break;
    case CZL_OBJ_REF:
        return czl_addr_obj_ass_handle(gp, left, right);
    case CZL_STRING:
        if (CZL_FUNRET_VAR == right->quality)
        {
            CZL_SRCD1(gp, left->val.str);
            right->type = CZL_NIL;
        }
        else if (CZL_ARRBUF_VAR == right->quality)
        {
            right->quality = CZL_DYNAMIC_VAR;
            return czl_strbuf_copy(gp, left);
        }
        else
        {
            czl_string *l = CZL_STR(left->val.str.Obj);
            czl_string *r = CZL_STR(right->val.str.Obj);
            if (1 == l->rc &&
                (0 == r->len ||
                 (left->val.str.size > r->len && left->val.str.size <= 2*r->len)))
            {
                memcpy(l->str, r->str, r->len+1);
                l->len = r->len;
                return 1;
            }
            else
            {
                ++r->rc;
                if (0 == --l->rc)
                    CZL_SF(gp, left->val.str);
            }
        }
        break;
    default:
        switch (right->quality)
        {
        case CZL_FUNRET_VAR:
            if (!czl_val_del(gp, left))
                return 0;
            right->type = CZL_NIL;
            break;
        case CZL_ARRBUF_VAR:
            if (!czl_val_del(gp, left))
                return 0;
            right->quality = CZL_DYNAMIC_VAR;
            break;
        case CZL_CIRCLE_REF_VAR:
            right->quality = CZL_DYNAMIC_VAR;
            if (!czl_val_del(gp, left))
            {
                czl_val_del(gp, right);
                return 0;
            }
            break;
        default:
            if (CZL_FILE == right->type || CZL_SOURCE == right->type)
            {
                CZL_ORCA1(right->val.Obj);
                if (0 == CZL_ORCD1(left->val.Obj))
                {
                    if (CZL_FILE == right->type)
                        czl_file_delete(gp, left->val.Obj);
                    else
                        czl_extsrc_delete(gp, left->val.Obj);
                }
            }
            else if (CZL_INS(right->val.Obj)->rf)
                return czl_obj_fork(gp, left, right);
            else
            {
                czl_value tmp = right->val; //tmp 用于处理 a = a.v 情况
                CZL_ORCA1(tmp.Obj);
                if (!czl_val_del(gp, left))
                {
                    CZL_ORCD1(tmp.Obj);
                    return 0;
                }
                left->val = tmp;
                return 1;
            }
            break;
        }
        break;
    }

    left->val = right->val;
    return 1;
}

char czl_arr_link(czl_gp *gp, czl_var *left, czl_var *right)
{
    void **obj;
	czl_array *arr;

    if (left->quality != CZL_FUNRET_VAR &&
        left->quality != CZL_ARRLINK_VAR)
        return 0;

    if (CZL_ARRAY == right->type)
    {
        czl_array *array = CZL_ARR(right->val.Obj);
        unsigned long sum = CZL_TB_SZIE(gp->add_pc, left, array->cnt, 0);
        unsigned long cnt = array->cnt + 1;
        if (!(obj=czl_array_create(gp, sum, cnt)))
            return 0;
        arr = CZL_ARR(obj);
        if (!czl_array_vars_new_by_array(gp, arr->vars+1,
                                         array->vars, array->cnt))
        {
            czl_array_delete(gp, obj);
            return 0;
        }
    }
    else //CZL_ARRAY_LIST
    {
        czl_array_list *list = CZL_ARR_LIST(right->val.Obj);
        unsigned long sum = CZL_TB_SZIE(gp->add_pc, left, list->paras_count, 0);
        unsigned long cnt = list->paras_count + 1;
        if (!(obj=czl_array_create(gp, sum, cnt)))
            return 0;
        arr = CZL_ARR(obj);
        if (!czl_array_vars_new_by_list(gp, arr->vars+1, list->paras))
        {
            czl_array_delete(gp, obj);
            return 0;
        }
    }

    arr->vars->type = left->type;
    arr->vars->val = left->val;
    if (CZL_STRING == left->type)
        CZL_SRCA1(left->val.str);

    left->val.Obj = obj;
    left->type = CZL_ARRAY;
    left->quality = CZL_ARRBUF_VAR;

    return 1;
}

char czl_arr_add(czl_gp *gp, czl_var *left, czl_var *right)
{
    void **obj;
	czl_array *arr;
    czl_array *array;
    czl_array_list *list;
    unsigned long count;
    unsigned long cnt;
    unsigned long sum;

    switch (right->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_STRING:
        cnt = 1;
        break;
    case CZL_ARRAY:
        cnt = CZL_ARR(right->val.Obj)->cnt;
        break;
    case CZL_ARRAY_LIST:
        cnt = CZL_ARR_LIST(right->val.Obj)->paras_count;
        break;
    default:
        return 0;
    }

    if (CZL_ARRAY == left->type)
    {
        array = CZL_ARR(left->val.Obj);
        count = array->cnt;
        if (CZL_ARRBUF_VAR == left->quality ||
            CZL_FUNRET_VAR == left->quality ||
            (left->quality != CZL_ARRLINK_VAR && 1 == array->rc))
        {
            sum = array->cnt + cnt;
            if (sum <= array->sum)
                array->cnt = sum;
            else if (!czl_array_resize(gp, array, sum))
                return 0;
            obj = left->val.Obj;
            arr = array;
        }
        else
        {
            sum = CZL_TB_SZIE(gp->add_pc, left, array->cnt, cnt-1);
            if (!(obj=czl_array_create(gp, sum, array->cnt+cnt)))
                return 0;
            arr = CZL_ARR(obj);
            if (!czl_array_vars_new_by_array(gp, arr->vars, array->vars, array->cnt))
            {
                czl_array_delete(gp, obj);
                return 0;
            }
        }
    }
    else //CZL_ARRAY_LIST
    {
        list = CZL_ARR_LIST(left->val.Obj);
        count = list->paras_count;
        sum = CZL_TB_SZIE(gp->add_pc, left, list->paras_count, cnt-1);
        if (!(obj=czl_array_create(gp, sum, list->paras_count+cnt)))
            return 0;
        arr = CZL_ARR(obj);
        if (!czl_array_vars_new_by_list(gp, arr->vars, list->paras))
        {
            czl_array_delete(gp, obj);
            return 0;
        }
    }

    switch (right->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_STRING:
        arr->vars[count].type = right->type;
        arr->vars[count].val = right->val;
        if (CZL_STRING == right->type)
            CZL_SRCA1(right->val.str);
        break;
    case CZL_ARRAY:
        array = CZL_ARR(right->val.Obj);
        if (!czl_array_vars_new_by_array(gp, arr->vars+count,
                                         array->vars, array->cnt))
            return 0;
        break;
    default: //CZL_ARRAY_LIST
        list = CZL_ARR_LIST(right->val.Obj);
        if (!czl_array_vars_new_by_list(gp, arr->vars+count, list->paras))
            return 0;
        break;
    }

    if (CZL_ARRLINK_VAR == left->quality || CZL_FUNRET_VAR == left->quality)
        left->quality = CZL_ARRBUF_VAR;

    left->val.Obj = obj;
    left->type = CZL_ARRAY;

    return 1;
}

char czl_tab_cac(czl_gp *gp, czl_var *left, czl_var *right, unsigned char add)
{
    char ret = 1;
    char flag = 1;
    void **obj;

    if (add)
    {
        if (right->type != CZL_TABLE && right->type != CZL_TABLE_LIST)
            return 0;
    }
    else
    {
        switch (right->type)
        {
        case CZL_INT: case CZL_FLOAT: case CZL_STRING:
        case CZL_TABLE: case CZL_ARRAY:
        case CZL_STACK: case CZL_QUEUE:
        case CZL_ARRAY_LIST:
            break;
        default:
            return 0;
        }
    }

    if (CZL_TABLE_LIST == left->type)
    {
        czl_var tmp;
        if (!czl_table_new(gp, CZL_TAB_LIST(left->val.Obj), &tmp))
            return 0;
        obj = tmp.val.Obj;
    }
    else if (CZL_ARRLINK_VAR == left->quality)
    {
        if (!(obj=czl_table_new_by_table(gp, CZL_TAB(left->val.Obj))))
            return 0;
    }
    else
    {
        if (CZL_TAB(left->val.Obj)->rc > 1 && !czl_table_fork(gp, left))
            return 0;
        obj = left->val.Obj;
        flag = 0;
    }

    if (add)
    {
        if (CZL_TABLE == right->type)
            ret = czl_table_union_by_table(gp, CZL_TAB(obj),
                                           CZL_TAB(right->val.Obj)->eles_head);
        else
            ret = czl_table_union_by_list(gp, CZL_TAB(obj),
                                          CZL_TAB_LIST(right->val.Obj)->paras);
    }
    else
    {
        left->val.Obj = obj;
        switch (right->type)
        {
        case CZL_INT: case CZL_FLOAT: case CZL_STRING:
            czl_delete_tabkv(gp, left, right);
            break;
        case CZL_TABLE:
            czl_table_mix_by_table(gp, left, CZL_TAB(right->val.Obj)->eles_head);
            break;
        case CZL_ARRAY:
            czl_table_mix_by_array(gp, left, CZL_ARR(right->val.Obj));
            break;
        case CZL_STACK: case CZL_QUEUE:
            czl_table_mix_by_sq(gp, left, CZL_SQ(right->val.Obj)->eles_head);
            break;
        case CZL_ARRAY_LIST:
            ret = czl_table_mix_by_list(gp, left, CZL_ARR_LIST(right->val.Obj)->paras);
            break;
        default:
            return 0;
        }
    }

    if (!ret)
    {
        if (flag)
            czl_table_delete(gp, obj);
        return 0;
    }

    if (flag)
    {
        if (CZL_ARRLINK_VAR == left->quality || CZL_FUNRET_VAR == left->quality)
            left->quality = CZL_ARRBUF_VAR;
        left->val.Obj = obj;
        left->type = CZL_TABLE;
    }

    return 1;
}

char czl_add_buf_resize(czl_gp *gp, unsigned long sum)
{    char *buf = CZL_TMP_REALLOC(gp, gp->add_buf, sum, gp->add_sum);
    if (!buf)
        return 0;
    gp->add_buf = buf;
    gp->add_sum = sum;
    return 1;
}

char czl_strbuf_copy(czl_gp *gp, czl_var *left)
{
    unsigned long len;

    if (0 == gp->add_cnt) return 1;

    len = *((unsigned long*)(gp->add_buf+gp->add_cnt-4));

    if (CZL_STRING == left->type && 1 == CZL_STR(left->val.str.Obj)->rc &&
        left->val.str.size > len && left->val.str.size <= 2*len)
    {
        czl_string *l = CZL_STR(left->val.str.Obj);
        memcpy(l->str, gp->add_buf+gp->add_cnt-len-5, len);
        l->str[len] = '\0';
        l->len = len;
    }
    else if (left->type != CZL_INT && !czl_val_del(gp, left))
        return 0;
    else if (!czl_str_create(gp, &left->val.str, len+1, len, gp->add_buf+gp->add_cnt-len-5))
    {
        left->type = CZL_INT;
        return 0;
    }
    else
        left->type = CZL_STRING;

#ifdef CZL_MM_RT_GC
    gp->add_cnt -= (len+1+4+8+4);
#else
    gp->add_cnt -= (len+1+8+4);
#endif

    if (gp->add_sum > CZL_ADD_BUF_SIZE)
        czl_add_buf_resize(gp, CZL_ADD_BUF_SIZE);

    return 1;
}

char czl_add_var_create
(
    czl_gp *gp,
    const char *lstr,
    const unsigned long llen,
    const char *rstr,
    const unsigned rlen,
    czl_var *var
)
{
    czl_string *s;

#ifdef CZL_MM_RT_GC
    void **obj;
    unsigned long sum = gp->add_cnt + llen + rlen + 1 + 4 + 8 + 4;
#else
    unsigned long sum = gp->add_cnt + rlen + rlen + 1 + 8 + 4;
#endif

    if (sum > gp->add_sum && !czl_add_buf_resize(gp, sum*2))
        return 0;

#ifdef CZL_MM_RT_GC
    obj = (void**)(gp->add_buf+gp->add_cnt);
    gp->add_cnt += 4;
    *obj = gp->add_buf+gp->add_cnt;
#endif

    s = (czl_string*)(gp->add_buf+gp->add_cnt);
    s->rc = 1;
    s->len = llen + rlen;
    gp->add_cnt += 8;

    memcpy(gp->add_buf+gp->add_cnt, lstr, llen);
    gp->add_cnt += llen;
    memcpy(gp->add_buf+gp->add_cnt, rstr, rlen);
    gp->add_cnt += rlen;
    gp->add_buf[gp->add_cnt++] = '\0';
    *((unsigned long*)(gp->add_buf+gp->add_cnt)) = s->len;
    gp->add_cnt += 4;

    if (CZL_FUNRET_VAR == var->quality && CZL_STRING == var->type)
        CZL_SRCD1(gp, var->val.str);

    var->quality = CZL_ARRBUF_VAR;
    var->type = CZL_STRING;
    var->val.str.size = s->len + 1;

#ifdef CZL_MM_RT_GC
    var->val.str.Obj = obj;
#else
    var->val.str.Obj = (void**)s;
#endif

    return 1;
}

char czl_str_link(czl_gp *gp, czl_var *left, czl_var *right)
{
    char tmp[32];
    unsigned long llen;
    czl_string *r = CZL_STR(right->val.Obj);

    if (left->quality != CZL_FUNRET_VAR && left->quality != CZL_ARRLINK_VAR)
        return 0;

    llen = (CZL_INT == left->type ?
            czl_itoa(left->val.inum, tmp) :
            czl_itoa(left->val.fnum, tmp));

    return czl_add_var_create(gp, tmp, llen, r->str, r->len, left);
}

char czl_str_add(czl_gp *gp, czl_var *left, czl_var *right)
{
    char tmp[32];
    char *rstr;
    unsigned long rlen;

    switch (right->type)
    {
    case CZL_STRING:
        rstr = CZL_STR(right->val.str.Obj)->str;
        rlen = CZL_STR(right->val.str.Obj)->len;
        break;
    case CZL_INT:
        rstr = tmp;
        rlen = czl_itoa(right->val.inum, tmp);
        break;
    case CZL_FLOAT:
        rstr = tmp;
        rlen = czl_itoa(right->val.fnum, tmp);
        break;
    case CZL_ARRAY: case CZL_ARRAY_LIST:
        return czl_arr_link(gp, left, right);
    default:
        return 0;
    }

    if (CZL_ARRBUF_VAR == left->quality)
    {
        unsigned long sum = gp->add_cnt + rlen;
        if (sum > gp->add_sum && !czl_add_buf_resize(gp, sum*2))
            return 0;
        if (gp->add_cnt)
        {
            czl_string *s = CZL_STR(left->val.str.Obj);
            sum = *((unsigned long*)(gp->add_buf+gp->add_cnt-4));
            memcpy(s->str+s->len, rstr, rlen);
            s->len += rlen;
            s->str[s->len] = '\0';
            left->val.str.size += rlen;
            gp->add_cnt += rlen;
            *((unsigned long*)(gp->add_buf+gp->add_cnt-4)) = sum + rlen;
        }
    }
    else if (CZL_ARRLINK_VAR == left->quality || CZL_FUNRET_VAR == left->quality)
    {
        czl_string *l = CZL_STR(left->val.Obj);
        return czl_add_var_create(gp, l->str, l->len, rstr, rlen, left);
    }
    else
    {
        czl_string *l = CZL_STR(left->val.Obj);
        if (l->rc > 1)
        {
            czl_str str;
            if (!czl_str_create(gp, &str, 2*l->len+rlen, l->len, l->str))
                return 0;
            --l->rc;
            left->val.str = str;
            l = CZL_STR(str.Obj);
        }
        else if (l->len+rlen >= left->val.str.size)
        {
            unsigned long size = 2*l->len+rlen;
            void **obj = CZL_SR(gp, left->val.str, size);
            if (!obj)
                return 0;
            left->val.str.Obj = obj;
            left->val.str.size = size;
            l = CZL_STR(obj);
        }
        memcpy(l->str+l->len, rstr, rlen);
        l->len += rlen;
        l->str[l->len] = '\0';
    }

    return 1;
}

char czl_add_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // +=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum += right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum += right->val.fnum;
            return 1;
        case CZL_STRING:
            return czl_str_link(gp, left, right);
        case CZL_ARRAY: case CZL_ARRAY_LIST:
            return czl_arr_link(gp, left, right);
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_FLOAT:
            left->val.fnum += right->val.fnum;
            return 1;
        case CZL_INT:
            left->val.fnum += right->val.inum;
            return 1;
        case CZL_STRING:
            return czl_str_link(gp, left, right);
        case CZL_ARRAY: case CZL_ARRAY_LIST:
            return czl_arr_link(gp, left, right);
        default:
            return 0;
        }
    case CZL_STRING:
        return czl_str_add(gp, left, right);
    case CZL_ARRAY: case CZL_ARRAY_LIST:
        return czl_arr_add(gp, left, right);
    case CZL_TABLE: case CZL_TABLE_LIST:
        return czl_tab_cac(gp, left, right, 1);
    default:
        return 0;
    }
}

char czl_dec_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // -=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum -= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum -= right->val.fnum;
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_FLOAT:
            left->val.fnum -= right->val.fnum;
            return 1;
        case CZL_INT:
            left->val.fnum -= right->val.inum;
            return 1;
        default:
            return 0;
        }
    case CZL_TABLE: case CZL_TABLE_LIST:
        return czl_tab_cac(gp, left, right, 0);
    default:
        return 0;
    }
}

char czl_mul_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // *=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum *= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum *= right->val.fnum;
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_FLOAT:
            left->val.fnum *= right->val.fnum;
            return 1;
        case CZL_INT:
            left->val.fnum *= right->val.inum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_div_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // /=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            if (0 == right->val.inum)
                return 0;
            left->val.inum /= right->val.inum;
            return 1;
        case CZL_FLOAT:
            if (0 == right->val.fnum)
                return 0;
            left->val.inum /= right->val.fnum;
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_FLOAT:
            if (0 == right->val.fnum)
                return 0;
            left->val.fnum /= right->val.fnum;
            return 1;
        case CZL_INT:
            if (0 == right->val.inum)
                return 0;
            left->val.fnum /= right->val.inum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_mod_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    // %=
    if (left->type != CZL_INT || right->type != CZL_INT || 0 == right->val.inum)
        return 0;

    left->val.inum %= right->val.inum;
    return 1;
}

char czl_or_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    // |=
    if (left->type != CZL_INT || right->type != CZL_INT)
        return 0;

    left->val.inum |= right->val.inum;
    return 1;
}

char czl_xor_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    // ^=
    if (left->type != CZL_INT || right->type != CZL_INT)
        return 0;

    left->val.inum ^= right->val.inum;
    return 1;
}

char czl_and_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    // &=
    if (left->type != CZL_INT || right->type != CZL_INT)
        return 0;

    left->val.inum &= right->val.inum;
    return 1;
}

char czl_l_shift_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    // <<=
    if (left->type != CZL_INT || right->type != CZL_INT)
        return 0;

    left->val.inum <<= right->val.inum;
    return 1;
}

char czl_r_shift_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    // >>=
    if (left->type != CZL_INT || right->type != CZL_INT)
        return 0;

    left->val.inum >>= right->val.inum;
    return 1;
}
//////////////////////////////////////////////////////////////////
char czl_num_str_cmp(const czl_var *var, char *str)
{
    czl_var res;

    if (!czl_get_number_from_str(str, &res))
        return -1;
    else if (CZL_INT == res.type)
    {
        if (CZL_INT == var->type)
        {
            if (res.val.inum == var->val.inum)
                return 0;
            else if (res.val.inum < var->val.inum)
                return -1;
            else
                return 1;
        }
        else //CZL_FLOAT
        {
            if (res.val.inum == var->val.fnum)
                return 0;
            else if (res.val.inum < var->val.fnum)
                return -1;
            else
                return 1;
        }
    }
    else //CZL_FLOAT
    {
        if (CZL_INT == var->type)
        {
            if (res.val.fnum == var->val.inum)
                return 0;
            else if (res.val.fnum < var->val.inum)
                return -1;
            else
                return 1;
        }
        else //CZL_FLOAT
        {
            if (res.val.fnum == var->val.fnum)
                return 0;
            else if (res.val.fnum < var->val.fnum)
                return -1;
            else
                return 1;
        }
    }
}

char czl_str_cmp(czl_gp *gp, czl_var *left, const czl_var *right)
{
    if (CZL_STRING == left->type)
    {
        char ret;
        if (CZL_STRING == right->type)
        {
            czl_string *l = CZL_STR(left->val.str.Obj);
            czl_string *r = CZL_STR(right->val.str.Obj);
            if (l->len == r->len)
                ret = memcmp(l->str, r->str, l->len);
            else
                ret = (l->len < r->len ? -1 : 1);
        }
        else
            ret = czl_num_str_cmp(right, CZL_STR(left->val.str.Obj)->str);
        CZL_TB_CF(gp, left);
        return ret;
    }
    else
        return -czl_num_str_cmp(left, CZL_STR(right->val.str.Obj)->str);
}

char czl_more_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // >
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.inum > right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum = left->val.inum > right->val.fnum;
            return 1;
        case CZL_STRING:
            left->val.inum = (1==czl_str_cmp(gp, left, right) ? 1 : 0);
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.fnum > right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.fnum > right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (1==czl_str_cmp(gp, left, right) ? 1 : 0);
            break;
        default:
            return 0;
        }
        left->type = CZL_INT;
        return 1;
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING: case CZL_INT: case CZL_FLOAT:
            left->val.inum = (1==czl_str_cmp(gp, left, right) ? 1 : 0);
            left->type = CZL_INT;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_more_equ_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // >=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.inum >= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum = left->val.inum >= right->val.fnum;
            return 1;
        case CZL_STRING:
            left->val.inum = (-1==czl_str_cmp(gp, left, right) ? 0 : 1);
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.fnum >= right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.fnum >= right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (-1==czl_str_cmp(gp, left, right) ? 0 : 1);
            break;
        default:
            return 0;
        }
        left->type = CZL_INT;
        return 1;
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING: case CZL_INT: case CZL_FLOAT:
            left->val.inum = (-1==czl_str_cmp(gp, left, right) ? 0 : 1);
            left->type = CZL_INT;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_less_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // <
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.inum < right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum = left->val.inum < right->val.fnum;
            return 1;
        case CZL_STRING:
            left->val.inum = (-1==czl_str_cmp(gp, left, right) ? 1 : 0);
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.fnum < right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.fnum < right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (-1==czl_str_cmp(gp, left, right) ? 1 : 0);
            break;
        default:
            return 0;
        }
        left->type = CZL_INT;
        return 1;
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING: case CZL_INT: case CZL_FLOAT:
            left->val.inum = (-1==czl_str_cmp(gp, left, right) ? 1 : 0);
            left->type = CZL_INT;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_less_equ_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // <=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.inum <= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum = left->val.inum <= right->val.fnum;
            return 1;
        case CZL_STRING:
            left->val.inum = (1==czl_str_cmp(gp, left, right) ? 0 : 1);
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.fnum <= right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.fnum <= right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (1==czl_str_cmp(gp, left, right) ? 0 : 1);
            break;
        default:
            return 0;
        }
        left->type = CZL_INT;
        return 1;
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING: case CZL_INT: case CZL_FLOAT:
            left->val.inum = (1==czl_str_cmp(gp, left, right) ? 0 : 1);
            left->type = CZL_INT;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_equ_equ_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // ==
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.inum == right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.inum == right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (0==czl_str_cmp(gp, left, right) ? 1 : 0);
            break;
        default: goto CZL_END;
        }
        return 1;
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.fnum == right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.fnum == right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (0==czl_str_cmp(gp, left, right) ? 1 : 0);
            break;
        default: goto CZL_END;
        }
        left->type = CZL_INT;
        return 1;
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING: case CZL_INT: case CZL_FLOAT:
            left->val.inum = (0==czl_str_cmp(gp, left, right) ? 1 : 0);
            left->type = CZL_INT;
            return 1;
        default: goto CZL_END;
        }
    default:
        CZL_TB_CF(gp, left);
        left->val.inum = left->val.Obj == right->val.Obj;
        left->type = CZL_INT;
        return 1;
    }

CZL_END:
    CZL_TB_CF(gp, left);
    left->val.inum = 0;
    left->type = CZL_INT;
    return 1;
}

char czl_not_equ_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // !=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.inum != right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.inum != right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (czl_str_cmp(gp, left, right) ? 1 : 0);
            break;
        default: goto CZL_END;
        }
        return 1;
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum = left->val.fnum != right->val.inum;
            break;
        case CZL_FLOAT:
            left->val.inum = left->val.fnum != right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (czl_str_cmp(gp, left, right) ? 1 : 0);
            break;
        default: goto CZL_END;
        }
        left->type = CZL_INT;
        return 1;
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING: case CZL_INT: case CZL_FLOAT:
            left->val.inum = (czl_str_cmp(gp, left, right) ? 1 : 0);
            left->type = CZL_INT;
            return 1;
        default: goto CZL_END;
        }
    default:
        CZL_TB_CF(gp, left);
        left->val.inum = left->val.Obj != right->val.Obj;
        left->type = CZL_INT;
        return 1;
    }

CZL_END:
    CZL_TB_CF(gp, left);
    left->val.inum = 1;
    left->type = CZL_INT;
    return 1;
}

char czl_equ_3_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    if (left->type != right->type)
    {
        CZL_TB_CF(gp, left);
        left->val.inum = 0;
    }
    else // ===
    {
        switch (left->type)
        {
        case CZL_INT:
            left->val.inum = left->val.inum == right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum = left->val.fnum == right->val.fnum;
            break;
        case CZL_STRING:
            left->val.inum = (0==czl_str_cmp(gp, left, right) ? 1 : 0);
            break;
        default:
            CZL_TB_CF(gp, left);
            left->val.inum = left->val.Obj == right->val.Obj;
            break;
        }
    }

    left->type = CZL_INT;
    return 1;
}

char czl_xor_xor_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // ^^
    {
    case CZL_INT: case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT: case CZL_FLOAT:
            left->val.inum = (left->val.inum ?
                              (right->val.inum ? 0 : 1) :
                              (right->val.inum ? 1 : 0));
            break;
        default:
            left->val.inum = (left->val.inum ? 1 : 0);
            break;
        }
        break;
    default:
        switch (right->type)
        {
        case CZL_INT: case CZL_FLOAT:
            left->val.inum = (right->val.inum ? 1 : 0);
            break;
        default:
            left->val.inum = 0;
            break;
        }
        break;
    }

    left->type = CZL_INT;
    return 1;
}

char czl_cmp_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // ??
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            if (left->val.inum == right->val.inum)
                left->val.inum = 0;
            else if (left->val.inum < right->val.inum)
                left->val.inum = -1;
            else
                left->val.inum = 1;
            return 1;
        case CZL_FLOAT:
            if (left->val.inum == right->val.fnum)
                left->val.inum = 0;
            else if (left->val.inum < right->val.fnum)
                left->val.inum = -1;
            else
                left->val.inum = 1;
            return 1;
        case CZL_STRING:
            left->val.inum = czl_str_cmp(gp, left, right);
            return 1;
        default:
            return 0;
        }
    case CZL_FLOAT:
        switch (right->type)
        {
        case CZL_INT:
            if (left->val.fnum == right->val.inum)
                left->val.inum = 0;
            else if (left->val.fnum < right->val.inum)
                left->val.inum = -1;
            else
                left->val.inum = 1;
            break;
        case CZL_FLOAT:
            if (left->val.fnum == right->val.fnum)
                left->val.inum = 0;
            else if (left->val.fnum < right->val.fnum)
                left->val.inum = -1;
            else
                left->val.inum = 1;
            break;
        case CZL_STRING:
            left->val.inum = czl_str_cmp(gp, left, right);
            break;
        default:
            return 0;
        }
        left->type = CZL_INT;
        return 1;
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING: case CZL_INT: case CZL_FLOAT:
            left->val.inum = czl_str_cmp(gp, left, right);
            left->type = CZL_INT;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_ele_del(czl_gp *gp, czl_var *left, czl_var *right)
{
    char ret; // =>

    if (right->type != CZL_TABLE)
        return 0;

    //对象必须是right，因为left是临时变量，否则在引用计数>1时删除操作不成功
    ret = czl_delete_tabkv(gp, right, left);

    CZL_TB_CF(gp, left);
    left->type = CZL_INT;
    left->val.inum = ret;
    return 1;
}

czl_long czl_obj_inx(czl_var *inx)
{
    switch (inx->type)
    {
    case CZL_INT: return inx->val.inum;
    case CZL_FLOAT: return inx->val.fnum;
    default: return -1;
    }
}

char czl_ele_inx(czl_gp *gp, czl_var *left, czl_var *right)
{
    char ret = 0; // ->
    czl_long inx;

    switch (left->type)
    {
    case CZL_TABLE:
        ret = (czl_find_tabkv(CZL_TAB(left->val.Obj), right) ? 1 : 0);
        break;
    case CZL_ARRAY:
        inx = czl_obj_inx(right);
        if (inx >= 0 && inx < CZL_ARR(left->val.Obj)->cnt)
            ret = 1;
        break;
    case CZL_STRING:
        inx = czl_obj_inx(right);
        if (inx >= 0 && inx < CZL_STR(left->val.Obj)->len)
            ret = 1;
        break;
    default:
        return 0;
    }

    CZL_TB_CF(gp, left);
    left->type = CZL_INT;
    left->val.inum = ret;
    return 1;
}
//////////////////////////////////////////////////////////////////
//双参运算符计算函数集合
char (*const czl_opt_cac_funs[])(czl_gp*, czl_var*, czl_var*) =
{
    //双参单目运算符函数
    czl_number_not_cac,
    czl_logic_not_cac,
    czl_logic_flit_cac,
    czl_self_add_cac,
    czl_self_dec_cac,
    czl_addr_obj_cac,
    czl_obj_cnt_cac,
    //双参双目运算符函数
    czl_swap_cac,
    czl_ass_cac,
    czl_add_a_cac,
    czl_dec_a_cac,
    czl_mul_a_cac,
    czl_div_a_cac,
    czl_mod_a_cac,
    czl_or_a_cac,
    czl_xor_a_cac,
    czl_and_a_cac,
    czl_l_shift_a_cac,
    czl_r_shift_a_cac,
    czl_more_cac,
    czl_more_equ_cac,
    czl_less_cac,
    czl_less_equ_cac,
    czl_equ_equ_cac,
    czl_not_equ_cac,
    czl_equ_3_cac,
    czl_xor_xor_cac,
    czl_cmp_cac,
    czl_ele_del,
    czl_ele_inx,
};

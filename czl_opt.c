#include "czl_opt.h"
#include "czl_lib.h"
//////////////////////////////////////////////////////////////////
void czl_str_resize(czl_gp *gp, czl_str *str)
{
    if (str->s->len+1 == str->size)
        return;
    str->s = CZL_SR(gp, *str, str->s->len+1); //内存缩小不会导致分配失败
    str->size = str->s->len + 1;
}

char czl_ref_copy(czl_gp *gp, czl_var *left, czl_var *var)
{
    czl_ref_var *ref = (czl_ref_var*)left->name;
    czl_ref_obj *obj = (czl_ref_obj*)var->name;
    if (!left->name)
    {
        if (!(left->name=(char*)CZL_REF_MALLOC(gp)))
            return 0;
        ((czl_ref_var*)left->name)->var = left;
    }
    if (!obj)
    {
        if (!(obj=(czl_ref_obj*)CZL_TMP_MALLOC(gp, CZL_RL(0))))
        {
            if (!ref)
                CZL_REF_FREE(gp, left->name);
            return 0;
        }
        obj->cnt = 0;
        obj->head = NULL;
        var->name = (char*)obj;
    }
    if (!ref)
        ref = (czl_ref_var*)left->name;

    if (left->type == CZL_OBJ_REF)
    {
        czl_var *tmp = CZL_GRV(left);
        if (tmp == var)
            return 1;
        czl_ref_break(gp, ref, tmp);
    }

    ref->last = NULL;
    if (NULL == obj->head)
        ref->next = NULL;
    else
    {
        ref->next = obj->head;
        obj->head->last = ref;
    }
    obj->head = ref;

    if (obj->cnt)
        CZL_ORFA1(obj);

    return 1;
}

char czl_val_copy(czl_gp *gp, czl_var *left, czl_var *right)
{
    if (!right) return 0;

    switch (right->type)
    {
    case CZL_INT:
        switch (left->ot)
        {
        case CZL_NIL: case CZL_NUM:
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
            goto CZL_TYPE_ERROR;
        }
    case CZL_FLOAT:
        switch (left->ot)
        {
        case CZL_NIL: case CZL_NUM:
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
            goto CZL_TYPE_ERROR;
        }
    case CZL_OBJ_REF:
        if (left->ot != CZL_NIL && left->ot != CZL_GRV(right)->type)
            goto CZL_TYPE_ERROR;
        if (right->quality != CZL_FUNRET_VAR &&
            !czl_ref_copy(gp, left, CZL_GRV(right)))
            return 0;
        left->type = CZL_OBJ_REF;
        left->val = right->val;
        return 1;
    case CZL_FILE: case CZL_FUN_REF:
        if (left->ot != CZL_NIL && left->ot != right->type)
            goto CZL_TYPE_ERROR;
        left->type = right->type;
        left->val = right->val;
        return 1;
    case CZL_NEW:
        if (left->ot != CZL_NIL &&
            left->ot != ((czl_new_sentence*)right->val.obj)->type)
            goto CZL_TYPE_ERROR;
        return czl_obj_new(gp, (czl_new_sentence*)right->val.obj, left);
    case CZL_ARRAY_LIST:
        switch (left->ot)
        {
        case CZL_NIL: case CZL_ARRAY:
            if (!(left->val.obj=czl_array_new(gp, NULL, (czl_array_list*)right->val.obj)))
                return 0;
            left->type = CZL_ARRAY;
            return 1;
        case CZL_STACK: case CZL_QUEUE: case CZL_LIST:
            if (!(left->val.obj=czl_sql_new(gp, NULL, (czl_array_list*)right->val.obj)))
                return 0;
            left->type = left->ot;
            return 1;
        default:
            goto CZL_TYPE_ERROR;
        }
    case CZL_TABLE_LIST:
        if (left->ot != CZL_NIL && left->ot != CZL_TABLE)
            goto CZL_TYPE_ERROR;
        if (!(left->val.obj=czl_table_new(gp, (czl_table_list*)right->val.obj)))
            return 0;
        left->type = CZL_TABLE;
        return 1;
    default:
        if (left->ot != CZL_NIL && left->ot != right->type)
            goto CZL_TYPE_ERROR;
        break;
    }

    switch (right->quality)
    {
    case CZL_FUNRET_VAR:
        left->type = right->type;
        left->val = right->val;
        right->type = CZL_INT;
        return 1;
    case CZL_ARRBUF_VAR:
        right->quality = CZL_DYNAMIC_VAR;
        if (CZL_STRING == right->type)
            czl_str_resize(gp, &right->val.str);
        break;
    default:
        if (CZL_STRING == right->type)
            CZL_SRCA1(right->val.str);
        else if (!((czl_ins*)right->val.obj)->rf)
            CZL_ORCA1(right->val.obj);
        else
            return czl_obj_fork(gp, left, right);
        break;
    }

    left->type = right->type;
    left->val = right->val;
    return 1;

CZL_TYPE_ERROR:
    gp->exceptionCode = CZL_EXCEPTION_COPY_TYPE_NOT_MATCH;
    return 0;
}
//////////////////////////////////////////////////////////////////
char czl_add_self_cac(czl_var *opr)
{
    switch (opr->type) // ++i
    {
    case CZL_INT: ++opr->val.inum; return 1;
    case CZL_FLOAT: ++opr->val.fnum; return 1;
    default: return 0;
    }
}

char czl_dec_self_cac(czl_var *opr)
{
    switch (opr->type) // --i
    {
    case CZL_INT: --opr->val.inum; return 1;
    case CZL_FLOAT: --opr->val.fnum; return 1;
    default: return 0;
    }
}
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
    res->type = CZL_INT;
    switch (opr->type) // !
    {
    case CZL_INT: res->val.inum = !opr->val.inum; return 1;
    case CZL_FLOAT: res->val.fnum = !opr->val.fnum; return 1;
    default: return 0;
    }
}

char czl_logic_flit_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    res->type = CZL_INT;
    switch (opr->type) // ~
    {
    case CZL_INT: res->val.inum = ~opr->val.inum; return 1;
    case CZL_FLOAT: res->val.fnum = ~(czl_long)opr->val.fnum; return 1;
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

char czl_or_or_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    res->type = CZL_INT;
    switch (opr->type) // ||
    {
    case CZL_INT: res->val.inum = opr->val.inum || 0; return 1;
    case CZL_FLOAT: res->val.inum = opr->val.fnum || 0; return 1;
    default: res->val.inum = 0; return 1;
    }
}

char czl_and_and_cac(czl_gp *gp, czl_var *res, czl_var *opr)
{
    res->type = CZL_INT;
    switch (opr->type) // &&
    {
    case CZL_INT: res->val.inum = opr->val.inum && 1; return 1;
    case CZL_FLOAT: res->val.inum = opr->val.fnum && 1; return 1;
    default: res->val.inum = 0; return 1;
    }
}

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
            CZL_INT_SWAP(left->type, right->type);
            CZL_VAL_SWAP(left->val, right->val, tmp);
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

char czl_nil_obj(czl_gp *gp, czl_var *var)
{
    czl_ins *ins = NULL;
    czl_class *pclass = NULL;
    if (CZL_INSTANCE == var->type)
        pclass = ((czl_ins*)var->val.obj)->pclass;

    if (!czl_val_del(gp, var))
        return 0;
    var->type = CZL_INT;

    switch (var->ot)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_NUM:
    case CZL_FILE: case CZL_FUN_REF:
        var->val.inum = 0;
        break;
    case CZL_STRING:
        if (!czl_str_create(gp, &var->val.str, 1, 0, NULL))
            return 0;
        break;
    case CZL_INSTANCE:
        if (!czl_instance_fork(gp, &ins, pclass, 1))
            return 0;
        var->val.obj = ins;
        break;
    case CZL_TABLE:
        if (!(var->val.obj=czl_table_create(gp, 0, 0)))
            return 0;
        break;
    case CZL_ARRAY:
        if (!(var->val.obj=czl_array_create(gp, 0, 0)))
            return 0;
        break;
    default: //CZL_STACK/CZL_QUEUE/CZL_LIST
        if (!(var->val.obj=czl_sql_create(gp, 0)))
            return 0;
        break;
    }

    var->type = var->ot;
    return 1;
}

char czl_nil_handle(czl_gp *gp, czl_var *var)
{
    if (var->ot != CZL_NIL)
        return czl_nil_obj(gp, var);

    if (!czl_val_del(gp, var))
        return 0;
    var->type = CZL_INT;
    var->val.inum = 0;

    return 1;
}

char czl_addr_obj_ass_handle(czl_gp *gp, czl_var *left, czl_var *right)
{
	czl_var *var;

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
        if (left->ot != var->type)
            return 0;
        break;
    }

    if (left->type != CZL_OBJ_REF)
    {
		char ret;
		const unsigned char quality = var->quality;
        if (CZL_VAR_EXIST_REF(left))
            czl_ref_obj_delete(gp, left);
        if (CZL_OBJ_ELE == quality) //处理 a = &a[0] 问题
            CZL_LOCK_OBJ(var);
        ret = czl_val_del(gp, left);
        if (quality != CZL_LOCK_ELE && CZL_OBJ_IS_LOCK(var))
            CZL_UNLOCK_OBJ(var, quality);
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
	void *ret;
    unsigned char type;

    if (!czl_val_del(gp, left))
        return 0;
    left->type = CZL_INT;

    if (CZL_ARRAY_LIST == right->type)
    {
        switch (left->ot)
        {
        case CZL_NIL: case CZL_ARRAY:
            if (!(ret=czl_array_new(gp, NULL, (czl_array_list*)right->val.obj)))
                return 0;
            type = CZL_ARRAY;
            break;
        case CZL_STACK: case CZL_QUEUE: case CZL_LIST:
            if (!(ret=czl_sql_new(gp, NULL, (czl_array_list*)right->val.obj)))
                return 0;
            type = left->ot;
            break;
        default:
            return 0;
        }
    }
    else
    {
        if (left->ot != CZL_NIL && left->ot != CZL_TABLE)
            return 0;
        if (!(ret=czl_table_new(gp, (czl_table_list*)right->val.obj)))
            return 0;
        type = CZL_TABLE;
    }

    left->type = type;
    left->val.obj = ret;
    return 1;
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
    case CZL_NEW:
        return czl_new_handle(gp, left, (czl_new_sentence*)right->val.obj);
    case CZL_NIL:
        return czl_nil_handle(gp, left);
    case CZL_OBJ_REF:
        return czl_addr_obj_ass_handle(gp, left, right);
    case CZL_ARRAY_LIST: case CZL_TABLE_LIST:
        return czl_arr_tab_list_ass(gp, left, right);
    case CZL_FILE: case CZL_FUN_REF:
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
            if (!czl_val_del(gp, left))
            {
                CZL_SF(gp, right->val.str);
                return 0;
            }
            if (CZL_STRING == right->type)
                czl_str_resize(gp, &right->val.str);
            left->type = right->type;
            left->val = right->val;
            return 1;
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
            else if (((czl_ins*)right->val.obj)->rf)
                return czl_obj_fork(gp, left, right);
            else
            {
                czl_var tmp = *right; //tmp 用于处理 a = a.v 情况
                CZL_ORCA1(tmp.val.obj);
                if (!czl_val_del(gp, left))
                {
                    CZL_ORCD1(tmp.val.obj);
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
    if (left->type != right->type ||
        (left->ot != CZL_NIL && left->ot != left->type)) // =
        return czl_ass_cac_diff_type(gp, left, right);

    switch (left->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_FILE: case CZL_FUN_REF:
        break;
    case CZL_OBJ_REF:
        return czl_addr_obj_ass_handle(gp, left, right);
    case CZL_STRING:
        switch (right->quality)
        {
        case CZL_FUNRET_VAR:
            CZL_SRCD1(gp, left->val.str);
            right->type = CZL_INT;
            break;
        case CZL_ARRBUF_VAR:
            CZL_SRCD1(gp, left->val.str);
            right->quality = CZL_DYNAMIC_VAR;
            czl_str_resize(gp, &right->val.str);
            break;
        default:
            if (left->val.str.s != right->val.str.s) //避免产生循环引用
            {
                CZL_SRCA1(right->val.str);
                CZL_SRCD1(gp, left->val.str);
            }
            break;
        }
        break;
    default:
        switch (right->quality)
        {
        case CZL_FUNRET_VAR:
            if (!czl_val_del(gp, left))
                return 0;
            right->type = CZL_INT;
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
            if (left->val.obj != right->val.obj) //避免产生循环引用
            {
                if (((czl_ins*)right->val.obj)->rf)
                    return czl_obj_fork(gp, left, right);
                else
                {
                    czl_value tmp = right->val; //tmp 用于处理 a = a.v 情况
                    CZL_ORCA1(tmp.obj);
                    if (!czl_val_del(gp, left))
                    {
                        CZL_ORCD1(tmp.obj);
                        return 0;
                    }
                    left->val = tmp;
                    return 1;
                }
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
	czl_array *arr;

    if (left->quality != CZL_FUNRET_VAR &&
        left->quality != CZL_ARRLINK_VAR)
        return 0;

    if (CZL_ARRAY == right->type)
    {
        czl_array *array = (czl_array*)right->val.obj;
        unsigned long cnt = array->cnt + 1;
        if (!(arr=czl_array_create(gp, cnt, cnt)))
            return 0;
        if (!czl_array_vars_new_by_array(gp, arr->vars+1,
                                         array->vars, array->cnt))
        {
            czl_array_delete(gp, arr);
            return 0;
        }
    }
    else //CZL_ARRAY_LIST
    {
        czl_array_list *list = (czl_array_list*)right->val.obj;
        unsigned long cnt = list->paras_count + 1;
        if (!(arr=czl_array_create(gp, cnt, cnt)))
            return 0;
        if (!czl_array_vars_new_by_list(gp, arr->vars+1, list->paras))
        {
            czl_array_delete(gp, arr);
            return 0;
        }
    }

    arr->vars->type = left->type;
    arr->vars->val = left->val;
    if (CZL_STRING == left->type)
        CZL_SRCA1(left->val.str);

    left->val.obj = arr;
    left->type = CZL_ARRAY;
    left->quality = CZL_ARRBUF_VAR;

    return 1;
}

char czl_arr_add(czl_gp *gp, czl_var *left, czl_var *right)
{
	czl_array *arr;
    czl_array *array;
    czl_array_list *list;
    unsigned long count;
    unsigned long cnt;

    switch (right->type)
    {
    case CZL_INT: case CZL_FLOAT: case CZL_STRING:
        cnt = 1;
        break;
    case CZL_ARRAY:
        cnt = ((czl_array*)(right->val.obj))->cnt;
        break;
    case CZL_ARRAY_LIST:
        cnt = ((czl_array_list*)right->val.obj)->paras_count;
        break;
    default:
        return 0;
    }

    if (CZL_ARRAY == left->type)
    {
        array = (czl_array*)left->val.obj;
        count = array->cnt;
        cnt += array->cnt;
        if (CZL_ARRBUF_VAR == left->quality ||
            CZL_FUNRET_VAR == left->quality ||
            (left->quality != CZL_ARRLINK_VAR && 1 == array->rc))
        {
            if (!czl_array_resize(gp, array, cnt))
                return 0;
            arr = array;
        }
        else
        {
            if (!(arr=czl_array_create(gp, cnt, cnt)))
                return 0;
            if (!czl_array_vars_new_by_array(gp, arr->vars,
                                             array->vars, array->cnt))
            {
                czl_array_delete(gp, arr);
                return 0;
            }
        }
    }
    else //CZL_ARRAY_LIST
    {
        list = (czl_array_list*)left->val.obj;
        count = list->paras_count;
        cnt += list->paras_count;
        if (!(arr=czl_array_create(gp, cnt, cnt)))
            return 0;
        if (!czl_array_vars_new_by_list(gp, arr->vars, list->paras))
        {
            czl_array_delete(gp, arr);
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
        array = (czl_array*)right->val.obj;
        if (!czl_array_vars_new_by_array(gp, arr->vars+count,
                                         array->vars, array->cnt))
            return 0;
        break;
    default: //CZL_ARRAY_LIST
        list = (czl_array_list*)right->val.obj;
        if (!czl_array_vars_new_by_list(gp, arr->vars+count, list->paras))
            return 0;
        break;
    }

    switch (left->quality)
    {
    case CZL_ARRLINK_VAR: case CZL_FUNRET_VAR:
        left->quality = CZL_ARRBUF_VAR;
        break;
    default:
        break;
    }

    left->val.obj = arr;
    left->type = CZL_ARRAY;

    return 1;
}

char czl_str_link(czl_gp *gp, czl_var *left, czl_var *right)
{
	char tmp[128];
	unsigned long llen;


    if (left->quality != CZL_FUNRET_VAR &&
        left->quality != CZL_ARRLINK_VAR)
        return 0;

    
    llen = (CZL_INT == left->type ?
            czl_itoa(left->val.inum, tmp) :
            czl_itoa(left->val.fnum, tmp));

    if (!czl_str_create(gp, &left->val.str,
                        right->val.str.s->len+llen+1,
                        llen, tmp))
        return 0;

    memcpy(left->val.str.s->str+llen,
           right->val.str.s->str,
           right->val.str.s->len);
    left->val.str.s->str[llen+right->val.str.s->len] = '\0';
    left->val.str.s->len += right->val.str.s->len;
    left->type = CZL_STRING;
    left->quality = CZL_ARRBUF_VAR;

    return 1;
}

char czl_str_add(czl_gp *gp, czl_var *left, czl_var *right)
{
    char tmp[128];
    char *rstr;
    unsigned long rlen;

    switch (right->type)
    {
    case CZL_STRING:
        rstr = right->val.str.s->str;
        rlen = right->val.str.s->len;
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

    switch (left->quality)
    {
    case CZL_ARRLINK_VAR:
        ++left->val.str.s->rc;
        left->quality = CZL_ARRBUF_VAR;
        break;
    case CZL_FUNRET_VAR:
        left->quality = CZL_ARRBUF_VAR;
        break;
    default:
        break;
    }

    if (left->val.str.s->rc > 1)
    {
        czl_str str;
        if (!czl_str_create(gp, &str,
                            left->val.str.s->len+rlen+1,
                            left->val.str.s->len,
                            left->val.str.s->str))
            return 0;
        --left->val.str.s->rc;
        left->val.str = str;
    }
    else if (left->val.str.s->len+rlen >= left->val.str.size)
    {
        czl_string *s;
        unsigned long size = (left->val.str.s->len+rlen)*2;
        if (!(s=CZL_SR(gp, left->val.str, size)))
            return 0;
        left->val.str.s = s;
        left->val.str.size = size;
    }

    memcpy(left->val.str.s->str+left->val.str.s->len, rstr, rlen);
    left->val.str.s->str[left->val.str.s->len+rlen] = '\0';
    left->val.str.s->len += rlen;

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
    switch (left->type) // %=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            if (0 == right->val.inum)
                return 0;
            left->val.inum %= right->val.inum;
            return 1;
        case CZL_FLOAT:
            if (0 == right->val.fnum)
                return 0;
            left->val.inum %= (czl_long)right->val.fnum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_or_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // |=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum |= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum |= (czl_long)right->val.fnum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_xor_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // ^=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum ^= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum ^= (czl_long)right->val.fnum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_and_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // &=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum &= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum &= (czl_long)right->val.fnum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_l_shift_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // <<=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum <<= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum <<= (czl_long)right->val.fnum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

char czl_r_shift_a_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    switch (left->type) // >>=
    {
    case CZL_INT:
        switch (right->type)
        {
        case CZL_INT:
            left->val.inum >>= right->val.inum;
            return 1;
        case CZL_FLOAT:
            left->val.inum >>= (czl_long)right->val.fnum;
            return 1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}
//////////////////////////////////////////////////////////////////
char czl_str_cmp(czl_gp *gp, czl_var *left, const czl_var *right)
{
    char ret;
    char tmp[128];

    switch (left->type)
    {
    case CZL_STRING:
        switch (right->type)
        {
        case CZL_STRING:
            ret = strcmp(left->val.str.s->str, right->val.str.s->str);
            break;
        case CZL_INT:
            czl_itoa(right->val.inum, tmp);
            ret = strcmp(left->val.str.s->str, tmp);
            break;
        default: //CZL_FLOAT
            czl_itoa(right->val.fnum, tmp);
            ret = strcmp(left->val.str.s->str, tmp);
            break;
        }
        if (CZL_ARRBUF_VAR == left->quality ||
            CZL_FUNRET_VAR == left->quality)
        {
            CZL_SRCD1(gp, left->val.str);
            left->quality = CZL_DYNAMIC_VAR;
        }
        return ret;
    case CZL_INT:
        czl_itoa(left->val.inum, tmp);
        return strcmp(tmp, right->val.str.s->str);
    default: //CZL_FLOAT
        czl_itoa(left->val.fnum, tmp);
        return strcmp(tmp, right->val.str.s->str);
    }
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
    case CZL_FILE:
        switch (right->type)
        {
        case CZL_FILE:
            left->val.inum = left->val.file.fp == right->val.file.fp;
            left->type = CZL_INT;
            return 1;
        default: goto CZL_END;
        }
    default:
        if (CZL_FUNRET_VAR == left->quality)
        {
            czl_val_del(gp, left);
            left->quality = CZL_DYNAMIC_VAR;
        }
        left->val.inum = left->val.obj == right->val.obj;
        left->type = CZL_INT;
        return 1;
    }

CZL_END:
    if (CZL_FUNRET_VAR == left->quality)
    {
        czl_val_del(gp, left);
        left->quality = CZL_DYNAMIC_VAR;
    }
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
    case CZL_FILE:
        switch (right->type)
        {
        case CZL_FILE:
            left->val.inum = left->val.file.fp != right->val.file.fp;
            left->type = CZL_INT;
            return 1;
        default: goto CZL_END;
        }
    default:
        if (CZL_FUNRET_VAR == left->quality)
        {
            czl_val_del(gp, left);
            left->quality = CZL_DYNAMIC_VAR;
        }
        left->val.inum = left->val.obj != right->val.obj;
        left->type = CZL_INT;
        return 1;
    }

CZL_END:
    if (CZL_FUNRET_VAR == left->quality)
    {
        czl_val_del(gp, left);
        left->quality = CZL_DYNAMIC_VAR;
    }
    left->val.inum = 1;
    left->type = CZL_INT;
    return 1;
}

char czl_equ_3_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    if (left->type != right->type)
    {
        if (CZL_FUNRET_VAR == left->quality)
        {
            czl_val_del(gp, left);
            left->quality = CZL_DYNAMIC_VAR;
        }
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
        case CZL_FILE:
            left->val.inum = left->val.file.fp == right->val.file.fp;
            break;
        default:
            if (CZL_FUNRET_VAR == left->quality)
            {
                czl_val_del(gp, left);
                left->quality = CZL_DYNAMIC_VAR;
            }
            left->val.inum = left->val.obj == right->val.obj;
            break;
        }
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

char czl_tab_cac(czl_gp *gp, czl_var *left, czl_var *right)
{
    char ret = 0; // =>

    //删除操作table必须是右操作数，因为左操作数会被临时变量覆盖
    if (CZL_TABLE == right->type)
        ret = czl_delete_tabkv(gp, &right->val, left);
    else if (CZL_TABLE == left->type)
        ret = czl_find_res_tabkv((czl_table*)left->val.obj, right);

    if (CZL_FUNRET_VAR == left->quality ||
        CZL_ARRBUF_VAR == left->quality)
    {
        czl_val_del(gp, left);
        left->quality = CZL_DYNAMIC_VAR;
    }

    left->type = CZL_INT;
    left->val.inum = ret;
    return 1;
}
//////////////////////////////////////////////////////////////////
//单参运算符计算函数集合
char (*const czl_1p_opt_cac_funs[])(czl_var*) =
{
    //单参单目运算符函数
    czl_add_self_cac,
    czl_dec_self_cac,
};

//双参运算符计算函数集合
char (*const czl_2p_opt_cac_funs[])(czl_gp*, czl_var*, czl_var*) =
{
    //双参单目运算符函数
    czl_number_not_cac,
    czl_logic_not_cac,
    czl_logic_flit_cac,
    czl_addr_obj_cac,
    czl_self_add_cac,
    czl_self_dec_cac,
    czl_or_or_cac,
    czl_and_and_cac,
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
    czl_cmp_cac,
    czl_tab_cac,
};

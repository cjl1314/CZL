#include "czl_paser.h"
#include "czl_opt.h"
#include "czl_lib.h"

///////////////////////////////////////////////////////////////
enum czl_keyword_index_enum
{
    CZL_IF_INDEX = 1, //必须从1开始
    CZL_ELIF_INDEX,
    CZL_ELSE_INDEX,
    CZL_FOR_INDEX,
    CZL_WHILE_INDEX,
    CZL_DO_INDEX,
    CZL_SWITCH_INDEX,
    CZL_CASE_INDEX,
    CZL_DEFAULT_INDEX,
    CZL_BREAK_INDEX,
    CZL_CONTINUE_INDEX,
    CZL_GOTO_INDEX,
    CZL_RETURN_INDEX,
    CZL_YEILD_INDEX,
    CZL_TRY_INDEX,
    CZL_LOAD_INDEX,
    CZL_FUN_INDEX,
    CZL_VAR_INDEX,
    CZL_INT_INDEX,
    CZL_FLOAT_INDEX,
    CZL_NUM_INDEX,
    CZL_STR_INDEX,
    CZL_FILE_INDEX,
    CZL_ARR_INDEX,
    CZL_TABLE_INDEX,
    CZL_STACK_INDEX,
    CZL_QUEUE_INDEX,
    CZL_STATIC_INDEX,
    CZL_CONST_INDEX,
    CZL_CLASS_INDEX,
    CZL_FINAL_INDEX,
    CZL_THIS_INDEX,
    CZL_FUNC_INDEX,
    CZL_PUB_INDEX,
    CZL_PRO_INDEX,
    CZL_PRI_INDEX,
    CZL_NEW_INDEX,
    CZL_NIL_INDEX,
    CZL_NULL_INDEX,
    CZL_TRUE_INDEX,
    CZL_FALSE_INDEX,
    CZL_LBRACKET_INDEX,
    CZL_RBRACKET_INDEX,
    //
    CZL_INS_INDEX //仅起到标记作用
};

//系统关键字列表
const czl_keyword czl_keyword_table[] =
{
    {"if",          CZL_IF_INDEX},
    {"elif",        CZL_ELIF_INDEX},
    {"else",        CZL_ELSE_INDEX},
    {"for",         CZL_FOR_INDEX},
    {"while",       CZL_WHILE_INDEX},
    {"do",          CZL_DO_INDEX},
    {"switch",      CZL_SWITCH_INDEX},
    {"case",        CZL_CASE_INDEX},
    {"default",     CZL_DEFAULT_INDEX},
    {"break",       CZL_BREAK_INDEX},
    {"continue",    CZL_CONTINUE_INDEX},
    {"goto",        CZL_GOTO_INDEX},
    {"return",      CZL_RETURN_INDEX},
    {"yeild",       CZL_YEILD_INDEX},
    {"try",         CZL_TRY_INDEX},
    {"load",        CZL_LOAD_INDEX},
    {"fun",         CZL_FUN_INDEX},
    {"var",         CZL_VAR_INDEX},
    {"int",         CZL_INT_INDEX},
    {"float",       CZL_FLOAT_INDEX},
    {"num",         CZL_NUM_INDEX},
    {"str",         CZL_STR_INDEX},
    {"file",        CZL_FILE_INDEX},
    {"arr",         CZL_ARR_INDEX},
    {"table",       CZL_TABLE_INDEX},
    {"stack",       CZL_STACK_INDEX},
    {"queue",       CZL_QUEUE_INDEX},
    {"static",      CZL_STATIC_INDEX},
    {"const",       CZL_CONST_INDEX},
    {"class",       CZL_CLASS_INDEX},
    {"final",       CZL_FINAL_INDEX},
    {"this",        CZL_THIS_INDEX},
    {"func",        CZL_FUNC_INDEX},
    {"pub",         CZL_PUB_INDEX},
    {"pro",         CZL_PRO_INDEX},
    {"pri",         CZL_PRI_INDEX},
    {"new",         CZL_NEW_INDEX},
    {"nil",         CZL_NIL_INDEX},
    {"null",        CZL_NULL_INDEX},
    {"true",        CZL_TRUE_INDEX},
    {"false",       CZL_FALSE_INDEX},
    {"{",           CZL_LBRACKET_INDEX},
    {"}",           CZL_RBRACKET_INDEX}
};
const unsigned long czl_keyword_table_num =
        sizeof(czl_keyword_table) / sizeof(czl_keyword);
///////////////////////////////////////////////////////////////
czl_var czl_true = {NULL, CZL_INT, CZL_CONST_VAR, 0, 0, {1}};
czl_var czl_false = {NULL, CZL_INT, CZL_CONST_VAR, 0, 0, {0}};
///////////////////////////////////////////////////////////////
char* czl_exp_analysis(czl_gp*, char*, czl_exp_handle*);
char* czl_expression_analysis(czl_gp*, char*);
char czl_shell_analysis(czl_gp*, char*);
char czl_condition_block_check(czl_gp*, int, char**, char*);
char* czl_class_def(czl_gp*, char*, char, char, char, czl_class*);
char* czl_fun_def(czl_gp*, char*, char);
///////////////////////////////////////////////////////////////
char* czl_annotate_check(czl_gp *gp, char *code)
{
    if ('/' == *code++)
    {
        while (*code != '\0')
        {
            if ('\n' == *code)
                return code;
            code++;
        }
    }
    else
    {
        char flag = 1;
        while (*code != '\0')
        {
            if ('\n' == *code)
                gp->error_line++;
            else if ('\"' == *code && *(code-1) != '\\')
                flag = !flag;
            else if ('/' == *code && '*' == *(code+1))
            {
                code = czl_annotate_check(gp, code+1);
                continue;
            }
            code++;
            if ('*' == *code && '/' == *(code+1) && flag)
                return code+2;
        }
    }

    return code;
}

//忽略不可见字符(czl_ignore_sign)和注释
char* czl_ignore_sign_filt(czl_gp *gp, char *code)
{
    while (*code != '\0')
    {
        if ('\n' == *code)
            gp->error_line++;
        if (' ' == *code || '\t' == *code || '\n' == *code || '\r' == *code)
            code++;
        else if ('/' == *code && ('/' == *(code+1) || '*' == *(code+1)))
            code = czl_annotate_check(gp, code+1);
        else
            break;
    }

    return code;
}

int czl_is_keyword(czl_gp *gp, char *name)
{
    czl_keyword *key = (czl_keyword*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
													   name, &gp->tmp->keyword_hash);
    if (!key)
        return 0;

    return key->index;
}

char* czl_name_match(czl_gp *gp, char *code, char *name)
{
    int len = 0;
    char *value;

    code = czl_ignore_sign_filt(gp, code);

    value = code;

    if (!((*code >= 'a' && *code <= 'z') ||
          (*code >= 'A' && *code <= 'Z') ||
          ('_' == *code)))
        return NULL;

    while (*code != '\0' && len < CZL_NAME_MAX_SIZE)
    {
        len++;
        code++;
        if (!((*code >= '0' && *code <= '9') ||
              (*code >= 'a' && *code <= 'z') ||
              (*code >= 'A' && *code <= 'Z') ||
              ('_' == *code)))
            break;
    }

    if (CZL_NAME_MAX_SIZE == len)
        return NULL;

    strncpy(name, value, len);
    name[len] = '\0';

    return code;
}

czl_goto_flag* czl_goto_node_find(const char *name, czl_goto_flag *p)
{
    while (p)
    {
        if (strcmp(name, p->name) == 0)
            return p;
        p = p->next;
    }

    return NULL;
}

char czl_goto_flag_insert(czl_gp *gp, const char *name)
{
	czl_goto_flag *node;
    if (czl_goto_node_find(name, gp->cur_fun->goto_flags))
    {
        sprintf(gp->log_buf, "redeclared goto_flag %s, ", name);
        return 0;
    }
    if (!(node=czl_goto_node_create(gp, name)))
        return 0;
    czl_goto_node_insert(&gp->cur_fun->goto_flags, node);

    return 1;
}

//系统关键字查找
char* czl_keyword_find(czl_gp *gp, char *code, int *index, char flag)
{
    char *tmp;
    char name[CZL_NAME_MAX_SIZE];
	unsigned long error_line_backup;

    *index = 0;

    code = czl_ignore_sign_filt(gp, code);
    tmp = code;

    if (flag)
    {
        if ('{' == *code)
        {
            *index = CZL_LBRACKET_INDEX;
            return code+1;
        }
        else if ('}' == *code)
        {
            *index = CZL_RBRACKET_INDEX;
            return code+1;
        }
    }

    if (!(code=czl_name_match(gp, code, name)))
        return tmp;

    if ((*index=czl_is_keyword(gp, name)))
    {
        if (1 == flag &&
            (CZL_INT_INDEX == *index ||
             CZL_FLOAT_INDEX == *index ||
             CZL_STR_INDEX == *index))
        {
            error_line_backup = gp->error_line;
            code = czl_ignore_sign_filt(gp, code);
            if ('(' == *code)
            {
                gp->error_line = error_line_backup;
                *index = 0;
                return tmp;
            }
        }
        return code;
    }

    error_line_backup = gp->error_line;
    code = czl_ignore_sign_filt(gp, code);
    if ((*code >= 'a' && *code <= 'z') ||
        (*code >= 'A' && *code <= 'Z') ||
        ('_' == *code))
    {
        gp->error_line = error_line_backup;
        *index = CZL_INS_INDEX;
        return tmp;
    }
    gp->error_line = error_line_backup;

    if (1 == flag)
    {
        error_line_backup = gp->error_line;
        code = czl_ignore_sign_filt(gp, code);
        if (':' == *code && *(code+1) != ':')
        {
            if (!czl_goto_flag_insert(gp, name))
                return NULL;
            return czl_keyword_find(gp, code+1, index, 1);
        }
        gp->error_line = error_line_backup;
    }

    return tmp;
}

char czl_loop_find(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
        if (CZL_FOR_INDEX == gp->tmp->block_stack[i].index ||
            CZL_WHILE_INDEX == gp->tmp->block_stack[i].index ||
            CZL_DO_INDEX == gp->tmp->block_stack[i].index)
            return 1;

    return 0;
}

char czl_switch_pop(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
        if (CZL_LBRACKET_INDEX == gp->tmp->block_stack[i].index)
        {
            if (i > 0 && CZL_SWITCH_INDEX == gp->tmp->block_stack[i-1].index)
            {
                gp->tmp->block_count = i-1;
                return 1;
            }
            return 0;
        }

    return 0;
}

char czl_switch_case_find(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
    {
        switch (gp->tmp->block_stack[i].index)
        {
        case CZL_SWITCH_INDEX: case CZL_CASE_INDEX:
            return 1;
        case CZL_LBRACKET_INDEX:
            break;
        default:
            return 0;
        }
    }

    return 0;
}

char czl_switch_find(czl_gp *gp)
{
    int i;

    for (i = gp->tmp->block_count-1; i >= 0; i--)
        if (CZL_SWITCH_INDEX == gp->tmp->block_stack[i].index)
            return 1;

    return 0;
}

char czl_block_context_check(czl_gp *gp, int index)
{
    int last_index = (0 == gp->tmp->block_count ? -1 :
                      gp->tmp->block_stack[gp->tmp->block_count-1].index);

    switch (index)
    {
    case CZL_ELIF_INDEX: case CZL_ELSE_INDEX:
        if (last_index < 0 || (last_index != CZL_IF_INDEX &&
                               last_index != CZL_ELIF_INDEX))
        {
            sprintf(gp->log_buf, "missed if for elif/else, ");
            return 0;
        }
        break;
    case CZL_CASE_INDEX: case CZL_DEFAULT_INDEX:
        if (!czl_switch_case_find(gp))
        {
            sprintf(gp->log_buf, "missed switch for case/default, ");
            return 0;
        }
        break;
    case CZL_BREAK_INDEX:
        if (!czl_loop_find(gp) && !czl_switch_find(gp))
        {
            sprintf(gp->log_buf, "missed loop/switch for break, ");
            return 0;
        }
        break;
    case CZL_CONTINUE_INDEX:
        if (!czl_loop_find(gp))
        {
            sprintf(gp->log_buf, "missed loop for continue, ");
            return 0;
        }
        break;
    case CZL_RBRACKET_INDEX:
        if (last_index == CZL_LBRACKET_INDEX)
        {
            gp->tmp->block_count--;
            //检查是否是空switch语句
            if (gp->tmp->block_count > 0 &&
                    CZL_SWITCH_INDEX ==
                    gp->tmp->block_stack[gp->tmp->block_count-1].index)
                gp->tmp->block_count--;
        }
        else
        {
            //检查switch语句是否正确结束
            if (!czl_switch_pop(gp))
            {
                sprintf(gp->log_buf, "missed '{' for '}', ");
                return 0;
            }
            //退出switch/case/default语句块
            gp->tmp->code_blocks_count--;
        }
        break;
    default:
        break;
    }

    return 1;
}

char czl_code_blocks_count(czl_gp *gp, int index)
{
    if (gp->tmp->block_count >= CZL_MAX_CODE_NEST_LAYER)
    {
        sprintf(gp->log_buf, "nest layer of block should not be much %d, ", CZL_MAX_CODE_NEST_LAYER);
        return 0;
    }

    gp->tmp->block_stack[gp->tmp->block_count].index = index;

    if (CZL_LBRACKET_INDEX == index)
        gp->tmp->block_stack[gp->tmp->block_count].flag = CZL_SURE;
    else
        gp->tmp->block_stack[gp->tmp->block_count].flag = CZL_NOT_SURE;

    gp->tmp->block_count++;

    return 1;
}

void czl_if_elif_else_pop(czl_gp *gp)
{
    while (gp->tmp->block_stack[--gp->tmp->block_count].index != CZL_IF_INDEX)
        ;
}

char* czl_end_sign_check(czl_gp *gp, char *code)
{
    code = czl_ignore_sign_filt(gp, code);

    if (';' == *code)
        return code+1;

    if ((*code >= 'a' && *code <= 'z') ||
        (*code >= 'A' && *code <= 'Z') || (*code >= '0' && *code <= '9'))
        return code;

    switch (*code)
    {
    case '_': case '{': case '}': case '.': case '\"': case '\'':
    case '!': case '~': case '-': case '$':
        return code;
    default:
        return NULL;
    }
}

char* czl_number_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //number
    czl_var res;

    if (!(code=czl_get_number_from_str(code, &res)))
    {
        sprintf(gp->log_buf, "number structure error, ");
        return NULL;
    }

    *node = czl_opr_node_create(gp, res.type, &res.val);

    return code;
}

char czl_is_esc_sign(const char ch)
{
    switch (ch)
    {
    case '\'' :
        return '\'';
    case '\"' :
        return '\"';
    case '\\' :
        return '\\';
    case '0' :
        return '\0';
    case 'a' :
        return '\a';
    case 'b' :
        return '\b';
    case 'f' :
        return '\f';
    case 'n' :
        return '\n';
    case 'r' :
        return '\r';
    case 't' :
        return '\t';
    case 'v' :
        return '\v';
    default :
        return -1;
    }
}

char* czl_char_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //'char'
    char ch;
    czl_value val;

    code++;

    if ('\\' == *code)
    {
        ch = czl_is_esc_sign(*(code+1));
        if (ch < 0)
        {
            sprintf(gp->log_buf, "escape character should be within [\' \" \\ 0 a b f n r t v], ");
            return code;
        }
        code++;
    }
    else
    {
        ch = *code;
        if ('\'' == ch)
        {
            sprintf(gp->log_buf, "character should not be empty, ");
            return code;
        }
    }

    if (*(code+1) != '\'')
    {
        sprintf(gp->log_buf, "missed '\'' for end defined a character, ");
        return code;
    }

    val.inum = ch;

    *node = czl_opr_node_create(gp, CZL_INT, &val);

    return code+2;
}

int czl_get_str_len(czl_gp *gp, char *code)
{
    int len = 0;

    while (*code != '\n' && *code != '\"')
    {
        if ('\\' == *code)
        {
            if (czl_is_esc_sign(*(code+1)) < 0)
                return -1;
            len++;
            code += 2;
        }
        else
        {
            len++;
            code++;
        }
    }

    if ('\n' == *code)
    {
        sprintf(gp->log_buf, "missed '\"' for end defined string, ");
        return -1;
    }

    return len;
}

char* czl_get_str(char *code, char *val)
{
    while (*code != '\"')
    {
        if ('\\' == *code)
        {
            *val++ = czl_is_esc_sign(*(code+1));
            code += 2;
        }
        else
            *val++ = *code++;
    }

    return code+1;
}

char* czl_string_filt(czl_gp *gp, char *code, char **val)
{
    long len = czl_get_str_len(gp, ++code);
    if (len < 0)
        return NULL;

    if (!(*val=(char*)CZL_TMP_MALLOC(gp, len+1)))
        return NULL;

    (*val)[len] = '\0';

    return czl_get_str(code, *val);
}

char* czl_str_match(czl_gp *gp, char *code, czl_var **ret)
{
    //"string"
	czl_value val;
	czl_var *node;

    long size = czl_get_str_len(gp, ++code);
    if (size < 0)
        return NULL;

    if (!czl_str_create(gp, &val.str, size+1, size, NULL))
        return NULL;
    code = czl_get_str(code, CZL_STR(val.str.Obj)->str);
    if (!(node=czl_const_create(gp, CZL_STRING, &val)))
        return NULL;
    *ret = node;

    return code;
}

char* czl_string_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //"string"
	czl_value val;
    long size = czl_get_str_len(gp, ++code);
    if (size < 0)
        return NULL;

    if (!czl_str_create(gp, &val.str, size+1, size, NULL))
        return NULL;
    code = czl_get_str(code, CZL_STR(val.str.Obj)->str);

    if (!(*node=czl_opr_node_create(gp, CZL_STRING, &val)))
        return NULL;

    return code;
}

char* czl_numstr_ignore_sign_filt(char *code)
{
    while (' ' == *code || '\t' == *code || '\n' == *code || '\r' == *code)
        code++;

    return code;
}

int czl_get_numstr_len(czl_gp *gp, char *code)
{
    int len = 0;
    int cnt;

    code = czl_numstr_ignore_sign_filt(code);

    do {
        if (*(code++) != '0')
            goto CZL_SYNTAX_ERROR;
        cnt = 0;
        switch (*(code++))
        {
        case 'x': case 'X':
            if ('\'' == *code)
                goto CZL_SYNTAX_ERROR;
            while (((*code >= '0' && *code <= '9') ||
                    (*code >= 'a' && *code <= 'f') ||
                    (*code >= 'A' && *code <= 'F')) &&
                   ((*(code+1) >= '0' && *(code+1) <= '9') ||
                    (*(code+1) >= 'a' && *(code+1) <= 'f') ||
                    (*(code+1) >= 'A' && *(code+1) <= 'F')))
            {
                cnt++;
                code += 2;
            }
            if (0 == cnt)
                goto CZL_SYNTAX_ERROR;
            len += cnt;
            break;
        case 'b': case 'B':
            if ('\'' == *code)
                goto CZL_SYNTAX_ERROR;
            while ('0' == *code || '1' == *code)
            {
                cnt++;
                code++;
            }
            if (cnt%8 != 0)
                goto CZL_SYNTAX_ERROR;
            len += cnt/8;
            break;
        default:
            goto CZL_SYNTAX_ERROR;
        }

        code = czl_numstr_ignore_sign_filt(code);
    } while (*code != '\'');

    if ('\n' == *code)
    {
        sprintf(gp->log_buf, "missed '\'' for end defined numstr, ");
        return -1;
    }

    return len;

CZL_SYNTAX_ERROR:
    sprintf(gp->log_buf, "numstr error, ");
    return -1;
}

char* czl_get_numstr(char *code, char *val)
{
    int cnt = 0;
    unsigned char h, l;
    const unsigned char bTable[] = {128, 64, 32, 16, 8, 4, 2, 1};

    code = czl_numstr_ignore_sign_filt(code);

    do {
        code++;
        switch (*(code++))
        {
        case 'x': case 'X':
            while (!(' ' == *code || '\t' == *code ||
                     '\n' == *code || '\r' == *code || '\'' == *code))
            {
                if (*code >= '0' && *code <= '9')
                    h = *code - '0';
                else if (*code >= 'a' && *code <= 'z')
                    h = *code - 'a' + 10;
                else
                    h = *code - 'A' + 10;
                code++;
                if (*code >= '0' && *code <= '9')
                    l = *code - '0';
                else if (*code >= 'a' && *code <= 'z')
                    l = *code - 'a' + 10;
                else
                    l = *code - 'A' + 10;
                code++;
                *(val++) = h*16 + l;
            }
            break;
        case 'b': case 'B':
            cnt = 0;
            while ('0' == *code || '1' == *code)
            {
                *val += ('0' == *code ? 0 : bTable[cnt]);
                if (++cnt == 8)
                {
                    val++;
                    cnt = 0;
                }
                code++;
            }
            break;
        default:
            break;
        }

        code = czl_numstr_ignore_sign_filt(code);
    } while (*code != '\'');

    return code+1;
}

char* czl_numstr_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    //'0x0a 0b11110000'
    czl_value val;
    long size = czl_get_numstr_len(gp, ++code);
    if (size < 0)
        return NULL;

    if (!czl_str_create(gp, &val.str, size+1, size, NULL))
        return NULL;
    code = czl_get_numstr(code, CZL_STR(val.str.Obj)->str);

    if (!(*node=czl_opr_node_create(gp, CZL_STRING, &val)))
        return NULL;

    return code;
}

char czl_is_constant(czl_gp *gp, char **code, czl_exp_node **node)
{
    if ('\'' == **code)
    {
        if ('\'' == *(*code+2) || ('\\' == *(*code+1) && '\'' == *(*code+3)))
            *code = czl_char_match(gp, *code, node);
        else
            *code = czl_numstr_match(gp, *code, node);
    }
    else if ('\"' == **code)
        *code = czl_string_match(gp, *code, node);
    else if ((**code >= '0' && **code <= '9') ||
             ('.' == **code && (*(*code+1) >= '0' && *(*code+1) <= '9')))
        *code = czl_number_match(gp, *code, node);
    else
        return 0;

    return 1;
}

char* czl_tabkv_list_match
(
    czl_gp *gp,
    char *code,
    czl_table_list **list
)
{
    czl_exp_ele *key, *val;
    czl_table_node *para;
    czl_table_node *paras_tail;

    if (!((*list)=czl_table_list_create(gp)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if ('}' == *code)
        return code+1;

    for (;;)
    {
        ++(*list)->paras_count;
        key = val = NULL;
        //key match
        if (!(code=czl_expression_analysis(gp, code)))
            goto CZL_SYNTAX_ERROR;
        if (!gp->tmp->exp_head)
        {
            sprintf(gp->log_buf, "para should not be empty, ");
            goto CZL_SYNTAX_ERROR;
        }
        if (CZL_IS_REF_VAR(gp->tmp->exp_head))
        {
            sprintf(gp->log_buf, "para should not be &, ");
            goto CZL_SYNTAX_ERROR;
        }
        key = gp->tmp->exp_head;
        gp->tmp->exp_head = NULL;
        // ':' check
        code = czl_ignore_sign_filt(gp, code);
        if (*code != ':')
        {
            sprintf(gp->log_buf, "missed ':' for key-value of table, ");
            goto CZL_SYNTAX_ERROR;
        }
        //val match
        if (!(code=czl_expression_analysis(gp, code+1)))
            goto CZL_SYNTAX_ERROR;
        if (!gp->tmp->exp_head)
        {
            sprintf(gp->log_buf, "para should not be empty, ");
            goto CZL_SYNTAX_ERROR;
        }
        if (CZL_IS_REF_VAR(gp->tmp->exp_head))
        {
            sprintf(gp->log_buf, "para should not be &, ");
            goto CZL_SYNTAX_ERROR;
        }
        val = gp->tmp->exp_head;
        gp->tmp->exp_head = NULL;
        //insert key_val
        if (!(para=czl_table_node_create(gp, key, val)))
            goto CZL_SYNTAX_ERROR;
        czl_table_node_insert(&(*list)->paras, &paras_tail, para);
        if (',' == *code)
            code++;
        else if ('}' == *code)
        {
            code++;
            break;
        }
        else
        {
            key = val = NULL;
            sprintf(gp->log_buf, "missed '}' for end a table, ");
            goto CZL_SYNTAX_ERROR;
        }
    }

    return code;

CZL_SYNTAX_ERROR:
    czl_exp_stack_delete(gp, key);
    czl_exp_stack_delete(gp, val);
    czl_table_list_delete(gp, *list);
    return NULL;
}

char* czl_paras_match
(
    czl_gp *gp,
    char *code,
    czl_para_list *paras_head,
    unsigned long *paras_count,
    char exp_null_flag,
    char alloc_flag,
    char end_sign
)
{
    czl_para *para;
    czl_para_list paras_tail;
    unsigned long cnt = 0;

    *paras_head = NULL;

    //过滤参数列表
    for (;;)
    {
        cnt++;
        if (!(code=czl_expression_analysis(gp, code)))
            return NULL;
        if (gp->tmp->exp_head || 1 == exp_null_flag)
        {
            if (']' == end_sign && gp->tmp->exp_head && CZL_IS_REF_VAR(gp->tmp->exp_head))
            {
                sprintf(gp->log_buf, "para should not be &, ");
                return NULL;
            }
            if (!(para=czl_para_node_create(gp, alloc_flag)))
                return NULL;
            czl_para_node_insert(paras_head, &paras_tail, para);
        }
        else if (0 == exp_null_flag)
        {
            sprintf(gp->log_buf, "para should not be empty, ");
            return NULL;
        }
        if (',' == *code)
            ++code;
        else if (end_sign == *code++)
            break;
        else
        {
            sprintf(gp->log_buf, "missed '%c' for end a paras-list, ", end_sign);
            return NULL;
        }
    }

    if (paras_count)
        *paras_count = cnt;

    return code;
}

char* czl_exp_fun_build
(
    czl_gp *gp,
    char *code,
    czl_exp_fun **ret_fun,
    czl_fun *fun,
    char fun_type
)
{
    czl_exp_fun *exp_fun = czl_exp_fun_create(gp, fun, fun_type);
    if (!exp_fun)
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (')' == *code)
        code++;
    else
    {
        unsigned long paras_count;
        if (!(code=czl_paras_match(gp, code, &exp_fun->paras,
                                   &paras_count, 1, 1, ')')))
            return NULL;
        exp_fun->paras_count = paras_count;
    }

    if (CZL_STATIC_FUN == exp_fun->quality &&
        fun->state != CZL_IN_STATEMENT &&
        !czl_fun_paras_check(gp, exp_fun, fun))
    {
        return NULL;
    }

    *ret_fun = exp_fun;
    return code;
}

czl_fun* czl_undef_static_fun_create(czl_gp *gp, char *name)
{
    char type = (CZL_IN_GLOBAL == gp->tmp->analysis_field ||
                 CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field ?
                 CZL_USR_GLOBAL_FUN : CZL_USR_CLASS_FUN);

    czl_fun *fun = czl_fun_node_create(gp, name, CZL_IN_STATEMENT,
                                       type, CZL_NIL, NULL);
    if (!fun || !czl_fun_insert(gp, fun))
        return NULL;

    return fun;
}

char* czl_fun_match
(
    czl_gp *gp,
    char *code,
    char *name,
    czl_exp_node **node,
    char global_flag,
    char this_flag
)
{
    char tmp[CZL_NAME_MAX_SIZE];
	char fun_type = CZL_STATIC_FUN;
    czl_fun *fun;
	czl_sysfun *sys_fun;
	czl_exp_fun *exp_fun;

    if (2 == global_flag)
    {
        //静态系统函数查询
        czl_sys_hash *hash = czl_sys_fun_hash_create(gp, name);
        if (!hash)
            return NULL;
        if (!(code=czl_name_match(gp, code, tmp)))
        {
            sprintf(gp->log_buf, "missed function after library %s, ", name);
            return NULL;
        }
        if (!(sys_fun=czl_sys_fun_find(tmp, hash)))
        {
            sprintf(gp->log_buf, "undefined function %s in library %s, ", tmp, name);
            return NULL;
        }
        if (!(fun=czl_sys_fun_create(gp, sys_fun)))
            return NULL;
        code = czl_ignore_sign_filt(gp, code);
        if (*(code++) != '(')
        {
            sprintf(gp->log_buf, "missed '(' for calling function %s, ", tmp);
            return NULL;
        }
    }
    //静态用户函数查询
    else if (!(fun=czl_fun_find_in_exp(gp, name, global_flag, this_flag)))
    {
        //静态系统函数查询
        if (!gp->tmp->osfun_hash &&
            !(gp->tmp->osfun_hash=czl_sys_fun_hash_create(gp, CZL_SYS_LIB_NAME)))
            return NULL;
        if ((sys_fun=czl_sys_fun_find(name, gp->tmp->osfun_hash)))
        {
            if (!(fun=czl_sys_fun_create(gp, sys_fun)))
                return NULL;
        }
        else
        {
            czl_var *var = czl_var_find_in_exp(gp, name, global_flag, this_flag);
            if (var) //变量调用方式动态函数查询
            {
                fun = (czl_fun*)var;
                if (gp->tmp->current_class_var)
                    fun_type = CZL_INS_VAR_DYNAMIC_FUN;
                else
                    fun_type = CZL_LG_VAR_DYNAMIC_FUN;
            }
            else //未定义的静态函数调用
            {
                if (!(fun=czl_undef_static_fun_create(gp, name)))
                    return NULL;
            }
        }
    }

    if (!(code=czl_exp_fun_build(gp, code, &exp_fun, fun, fun_type)))
        return NULL;

    if (!(*node=czl_opr_node_create(gp, CZL_FUNCTION, exp_fun)))
        return NULL;

    return code;
}

czl_var* czl_dynamic_var_create(czl_gp *gp, char *name)
{
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    if (!czl_store_device_check(gp))
        return NULL;

    return czl_var_create_by_field(gp, name, CZL_DYNAMIC_VAR, CZL_NIL);
}

char* czl_array_list_build(czl_gp *gp, char *code, czl_array_list **list)
{
    if (!(*list=czl_array_list_create(gp)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (']' == *code)
        return code+1;

    if (!(code=czl_paras_match(gp, code, &(*list)->paras,
                               &(*list)->paras_count, 0, 1, ']')))
    {
        czl_array_list_delete(gp, *list);
        return NULL;
    }

    return code;
}

char* czl_is_exp_right(czl_gp *gp, char *code)
{
    return czl_expression_analysis(gp, code);
}

char* czl_get_arr_len(czl_gp *gp, char *code, czl_exp_ele **len)
{
    code = czl_ignore_sign_filt(gp, code);
    if (*code != '[')
    {
        *len = NULL;
        return code;
    }

    if (!(code=czl_is_exp_right(gp, code+1)))
        return NULL;
    if (!gp->tmp->exp_head)
    {
        sprintf(gp->log_buf, "[] length should not be empty, ");
        return NULL;
    }
    if (*(code++) != ']')
    {
        sprintf(gp->log_buf, "missed ']' for end declared array length, ");
        return NULL;
    }

    if (CZL_EIO(gp->tmp->exp_head) && CZL_ENUM == gp->tmp->exp_head->kind)
    {
        czl_var *var = gp->tmp->exp_head->res;
        if ((CZL_INT == var->type && var->val.inum < 0) ||
            (CZL_FLOAT == var->type && var->val.fnum < 0))
        {
            sprintf(gp->log_buf, "[] length should not be less 0, ");
            return NULL;
        }
        else if (var->type != CZL_INT && var->type != CZL_FLOAT)
        {
            sprintf(gp->log_buf, "[] length should be a number, ");
            return NULL;
        }
    }

    *len = gp->tmp->exp_head;
    gp->tmp->exp_head = NULL;

    return code;
}

char* czl_str_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
    if (!(*newObj=czl_new_sentence_create(gp, CZL_STRING)))
        return NULL;

    (*newObj)->new_obj.arr.init_list = NULL;
    if (!(code=czl_get_arr_len(gp, code, &(*newObj)->new_obj.arr.len)))
        goto CZL_SYNTAX_ERROR;

    code = czl_ignore_sign_filt(gp, code);
    if ('\"' == *code)
    {
        czl_var *str;
        if (!(code=czl_str_match(gp, code, &str)))
            goto CZL_SYNTAX_ERROR;
        (*newObj)->new_obj.arr.init_list = (czl_array_list*)str;
    }

    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, *newObj);
    return NULL;
}

char* czl_arr_def(czl_gp *gp, char *code, czl_new_sentence **newObj, char type)
{
    if (!(*newObj=czl_new_sentence_create(gp, type)))
        return NULL;

    (*newObj)->new_obj.arr.init_list = NULL;
    if (!(code=czl_get_arr_len(gp, code, &(*newObj)->new_obj.arr.len)))
        goto CZL_SYNTAX_ERROR;

    code = czl_ignore_sign_filt(gp, code);
    if ('[' == *code && !(code=czl_array_list_build
                          (gp, code+1, &(*newObj)->new_obj.arr.init_list)))
        goto CZL_SYNTAX_ERROR;

    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, *newObj);
    return NULL;
}

char* czl_tab_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
    if (!(*newObj=czl_new_sentence_create(gp, CZL_TABLE)))
        return NULL;

    (*newObj)->new_obj.tab = NULL;
    code = czl_ignore_sign_filt(gp, code);
    if ('{' == *code)
    {
        if (!(code=czl_tabkv_list_match(gp, code+1, &(*newObj)->new_obj.tab)))
        {
            czl_new_sentence_delete(gp, *newObj);
            return NULL;
        }
    }

    return code;
}

char* czl_ins_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
	char name[CZL_NAME_MAX_SIZE];

    if (!(*newObj=czl_new_sentence_create(gp, CZL_INSTANCE)))
        return NULL;

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "after new should be a class name, ");
        goto CZL_SYNTAX_ERROR;
    }
    if (!((*newObj)->new_obj.ins.pclass=czl_class_find(gp, name)))
    {
        sprintf(gp->log_buf, "undefined class %s, ", name);
        goto CZL_SYNTAX_ERROR;
    }
    (*newObj)->new_obj.ins.init_fun = NULL;
    if (!(code=czl_get_arr_len(gp, code, &(*newObj)->new_obj.ins.len)))
        goto CZL_SYNTAX_ERROR;
    code = czl_ignore_sign_filt(gp, code);
    if ('(' == *code)
    {
        czl_fun *fun = czl_fun_node_find
                            ((*newObj)->new_obj.ins.pclass->name,
                            &(*newObj)->new_obj.ins.pclass->funs_hash);
        if (!fun)
        {
            sprintf(gp->log_buf, "undefined structure function in class %s, ", name);
            goto CZL_SYNTAX_ERROR;
        }
        if (!(code=czl_exp_fun_build(gp, code+1,
                                     &(*newObj)->new_obj.ins.init_fun,
                                     fun, CZL_STATIC_FUN)))
            goto CZL_SYNTAX_ERROR;
    }
    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, *newObj);
    return NULL;
}

char* czl_new_sentence_def(czl_gp *gp, char *code, czl_new_sentence **newObj)
{
    int index;

    code = czl_keyword_find(gp, code, &index, 0);
    switch (index)
    {
    case CZL_TABLE_INDEX:
        return czl_tab_def(gp, code, newObj);
    case CZL_STR_INDEX:
        return czl_str_def(gp, code, newObj);
    case CZL_ARR_INDEX:
        return czl_arr_def(gp, code, newObj, CZL_ARRAY);
    case CZL_STACK_INDEX:
        return czl_arr_def(gp, code, newObj, CZL_STACK);
    case CZL_QUEUE_INDEX:
        return czl_arr_def(gp, code, newObj, CZL_QUEUE);
    default:
        return czl_ins_def(gp, code, newObj);
    }
}

char* czl_var_match
(
    czl_gp *gp,
    char *code,
    char *name,
    czl_exp_node **node,
    char global_flag,
    char this_flag
)
{
    char type;
    czl_var *var = czl_var_find_in_exp(gp, name, global_flag, this_flag);
    if (var)
    {
        if (gp->tmp->current_class_var)
            type = CZL_INS_VAR;
        else
            type = CZL_CONST_VAR == var->quality ?
                   CZL_ENUM : CZL_LG_VAR;
    }
    else
    {
        if (global_flag || this_flag)
        {
            sprintf(gp->log_buf, "undefined variable %s, ", name);
            return NULL;
        }
        if (CZL_IN_CONSTANT == gp->tmp->variable_field)
        {
            sprintf(gp->log_buf, "undefined constant %s, ", name);
            return NULL;
        }
        code = czl_ignore_sign_filt(gp, code);
        if ('=' == *code)
        {
            //函数内部可以不用"var"关键字定义变量，全局或类成员变量必须用"var"声明
            if (!(var=czl_dynamic_var_create(gp, name)))
                return NULL;
            type = CZL_LG_VAR;
        }
        else if ('.' == *code)
        {
            //系统库函数调用
            return czl_fun_match(gp, code+1, name, node, 2, 0);
        }
        else if (strcmp("new", name) == 0)
        {
			czl_new_sentence *newObj;
            type = CZL_NEW;
            if (!(code=czl_new_sentence_def(gp, code, &newObj)))
                return NULL;
            var = (czl_var*)newObj;
        }
        else if (strcmp("nil", name) == 0)
            type = CZL_NIL;
        else if (strcmp("null", name) == 0)
            type = CZL_OBJ_REF;
        else if (strcmp("true", name) == 0)
        {
            type = CZL_ENUM;
            var = &czl_true;
        }
        else if (strcmp("false", name) == 0)
        {
            type = CZL_ENUM;
            var = &czl_false;
        }
        else
        {
            sprintf(gp->log_buf, "undefined variable %s, ", name);
            return NULL;
        }
    }

    if (!(*node=czl_opr_node_create(gp, type, var)))
        return NULL;

    return code;
}

char* czl_instance_index_match
(
    czl_gp *gp,
    char *code,
    char *var_name,
    char *class_name
)
{
    if (!(code=czl_name_match(gp, code, var_name)))
    {
        sprintf(gp->log_buf, "missed name after '.', ");
        return NULL;
    }
    code = czl_ignore_sign_filt(gp, code);
    if (':' == *code && ':' == *(code+1))
    {
        strcpy(class_name, var_name);
        if (!(code=czl_name_match(gp, code+2, var_name)))
        {
            sprintf(gp->log_buf, "missed name after ::, ");
            return NULL;
        }
    }
    else
        class_name[0] = '\0';

    return code;
}

char* czl_array_index_match
(
    czl_gp *gp,
    char *code,
    czl_exp_ele **index
)
{
    if (!(code=czl_is_exp_right(gp, code)))
        return NULL;
    if (!gp->tmp->exp_head)
    {
        sprintf(gp->log_buf, "array index [] should not be empty, ");
        return NULL;
    }
    if (*(code++) != ']')
    {
        sprintf(gp->log_buf, "missed ']' for end an array index, ");
        return NULL;
    }

    *index = gp->tmp->exp_head;
    gp->tmp->exp_head = NULL;

    return code;
}

czl_obj_member* czl_obj_member_create(czl_gp *gp, czl_exp_node *node)
{
    czl_obj_member *member = czl_obj_member_node_create
                                (gp, node->type, node->op.obj);
    if (!member)
        return NULL;

    node->type = CZL_MEMBER;
    node->op.obj = member;

    return member;
}

char* czl_object_member_match(czl_gp *gp, char *code, czl_exp_node *node)
{
    unsigned long cnt = 0;
    czl_exp_fun *exp_fun;
    czl_member_index *index;
    czl_member_index *index_tail;
    czl_obj_member *member = czl_obj_member_create(gp, node);
    if (!member)
        return NULL;

    for (;;)
    {
        code = czl_ignore_sign_filt(gp, code);
        if ('.' == *code)
        {
            char var_name[CZL_NAME_MAX_SIZE];
            char class_name[CZL_NAME_MAX_SIZE];
            if (!(code=czl_instance_index_match(gp, code+1, var_name, class_name)))
                return NULL;
            code = czl_ignore_sign_filt(gp, code);
            if (*code != '(')
                exp_fun = NULL;
            else if (!(code=czl_exp_fun_build(gp, code+1, &exp_fun, NULL,
                                              CZL_LG_VAR_DYNAMIC_FUN)))
                return NULL;
            if (!(index=czl_ins_index_create(gp, var_name, class_name, exp_fun)))
                return NULL;
        }
        else if ('[' == *code)
        {
            czl_exp_ele *exp;
            if (!(code=czl_array_index_match(gp, code+1, &exp)))
                return NULL;
            code = czl_ignore_sign_filt(gp, code);
            if (*code != '(')
                exp_fun = NULL;
            else if (!(code=czl_exp_fun_build(gp, code+1, &exp_fun, NULL,
                                              CZL_LG_VAR_DYNAMIC_FUN)))
            {
                czl_exp_stack_delete(gp, exp);
                return NULL;
            }
            if (!(index=czl_arr_index_create(gp, exp, exp_fun)))
                return NULL;
        }
        else
            return code;
        czl_member_index_node_insert(&member->index, &index_tail, index);
        if (exp_fun) //不允许 a.fun().b 这种情况，函数在.和[]索引必须是终点
            return code;
        if (++cnt > CZL_MAX_MEMBER_INDEX_LAYER)
        {
            sprintf(gp->log_buf, "object index number should be less %d, ", CZL_MAX_MEMBER_INDEX_LAYER);
            return 0;
        }
    }
}

char* czl_class_member_match
(
    czl_gp *gp,
    char *code,
    czl_exp_node **node,
    char *name
)
{
	char type;
	czl_class_var *var;
    czl_class *pclass = czl_class_find(gp, name);
    if (!pclass)
    {
        sprintf(gp->log_buf, "undefined class %s, ", name);
        return NULL;
    }
    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed variable after :: , ");
        return NULL;
    }
    code = czl_ignore_sign_filt(gp, code);
    if (!(var=czl_class_var_find(name, pclass)))
    {
        sprintf(gp->log_buf, "undefined variable %s in class %s, ", name, pclass->name);
        return NULL;
    }
    if (var->quality != CZL_CONST_VAR &&
        CZL_IN_CONSTANT == gp->tmp->variable_field)
    {
        sprintf(gp->log_buf, "variable %s is not a constant in class %s, ", name, pclass->name);
        return NULL;
    }
    type = (CZL_CONST_VAR == var->quality ? CZL_ENUM : CZL_LG_VAR);
    if (!(*node=czl_opr_node_create(gp, type, var)))
        return NULL;
    code = czl_ignore_sign_filt(gp, code);
    if ('.' == *code || '[' == *code)
    {
        if (!(code=czl_object_member_match(gp, code, *node)))
        {
            *node = NULL;
            return NULL;
        }
    }

    return code;
}

char czl_is_var_or_fun(czl_gp *gp, char **code, czl_exp_node **node, char *flag)
{
    char name[CZL_NAME_MAX_SIZE];
    char *s = *code;
    char global_flag = 0;
    char this_flag = 0;
    unsigned long line;

    //用 $ 指定引用全局变量或常量
    if ('$' == *s)
    {
        s++;
        global_flag = 1;
    }
    else if (!((*s >= 'a' && *s <= 'z') ||
               (*s >= 'A' && *s <= 'Z') ||
               ('_' == *s)))
        return 0;

    if (!(s=czl_name_match(gp, s, name)))
        return 1;

    line = gp->error_line;
    s = czl_ignore_sign_filt(gp, s);

    if (flag && !global_flag)
    {
        if (0 == strcmp("up", name))
            *flag = 1;
        else if (0 == strcmp("down", name))
            *flag = 2;
        if (*flag)
        {
            *code = s;
            return 0;
        }
    }

    if ('.' == *s)
    {
        if (strcmp("this", name) == 0)
        {
            if (global_flag)
            {
                sprintf(gp->log_buf, "$ should not use with this, ");
                goto CZL_END;
            }
            if (gp->tmp->analysis_field != CZL_IN_CLASS &&
                gp->tmp->analysis_field != CZL_IN_CLASS_FUN)
            {
                sprintf(gp->log_buf, "this should use in class-scope, ");
                goto CZL_END;
            }
            if (!(s=czl_name_match(gp, s+1, name)))
            {
                sprintf(gp->log_buf, "missed variable or function after this, ");
                goto CZL_END;
            }
            this_flag = 1;
        }
    }
    else if (':' == *s && ':' == *(s+1))
    {
        if (global_flag)
        {
            sprintf(gp->log_buf, "$ should not use with ::, ");
            goto CZL_END;
        }
        *code = czl_class_member_match(gp, s+2, node, name);
        return 1;
    }

    s = czl_ignore_sign_filt(gp, s);
    if ('(' == *s)
    {
        if (CZL_IN_CONSTANT == gp->tmp->variable_field)
        {
            sprintf(gp->log_buf, "should not use function in constant init, ");
            goto CZL_END;
        }
        *code = czl_fun_match(gp, s+1, name, node, global_flag, this_flag);
        return 1;
    }

    if (!(s=czl_var_match(gp, s, name, node, global_flag, this_flag)))
        goto CZL_END;

    s = czl_ignore_sign_filt(gp, s);
    if (('.' == *s || '[' == *s) && !(s=czl_object_member_match(gp, s, *node)))
        *node = NULL;

    *code = s;
    return 1;

CZL_END:
    gp->error_line = line;
    return 1;
}

char* czl_array_list_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    czl_array_list *list;

    if (!(code=czl_array_list_build(gp, code+1, &list)))
        return NULL;

    if (!(*node=czl_opr_node_create(gp, CZL_ARRAY_LIST, list)))
    {
        czl_array_list_delete(gp, list);
        return NULL;
    }

    return code;
}

char* czl_table_list_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    czl_table_list *list;

    if (!(code=czl_tabkv_list_match(gp, code+1, &list)))
        return NULL;

    if (!(*node=czl_opr_node_create(gp, CZL_TABLE_LIST, list)))
    {
        czl_table_list_delete(gp, list);
        return NULL;
    }

    return code;
}

char czl_is_array_or_table(czl_gp *gp, char **code, czl_exp_node **node)
{
    char *s = *code;

    if (*s != '[' && *s != '{')
        return 0;

    if ('[' == *s)
        *code = czl_array_list_match(gp, s, node);
    else // '{'
        *code = czl_table_list_match(gp, s, node);

    return 1;
}

char* czl_fun_addr_match(czl_gp *gp, char *code, czl_exp_node **node)
{
    char name[CZL_NAME_MAX_SIZE];
    char tmp[CZL_NAME_MAX_SIZE];
    char flag = 0;
	czl_fun *fun;
    czl_sysfun *sys_fun;
    czl_sys_hash *hash;

    *node = NULL;

    code = czl_ignore_sign_filt(gp, code);
    if ('$' == *code)
    {
        flag = 1;
        code++;
    }

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed function or library name after @, ");
        return NULL;
    }

    if (0 == flag)
    {
        code = czl_ignore_sign_filt(gp, code);
        if ('.' == *code)
        {
            flag = 2;
            if (!(code=czl_name_match(gp, code+1, tmp)))
            {
                sprintf(gp->log_buf, "missed function name after library %s, ", name);
                return NULL;
            }
        }
    }

    switch (flag)
    {
    case 0:
        if (!(fun=czl_fun_find(gp, name, CZL_GLOBAL_FIND)))
        {
            if (!gp->tmp->osfun_hash &&
                !(gp->tmp->osfun_hash=czl_sys_fun_hash_create(gp, CZL_SYS_LIB_NAME)))
                return NULL;
            if ((sys_fun=czl_sys_fun_find(name, gp->tmp->osfun_hash)))
            {
                if (!(fun=czl_sys_fun_create(gp, sys_fun)))
                    return NULL;
            }
            else if (!(fun=czl_undef_static_fun_create(gp, name)) ||
                     !czl_nsef_create(gp, fun, 0))
                return NULL;
        }
        break;
    case 1:
        if (!(fun = (czl_fun*)czl_sys_hash_find(CZL_STRING, CZL_NIL,
                                          name, &gp->tmp->funs_hash)))
        {
            if (!(fun=czl_undef_static_fun_create(gp, name)) ||
                !czl_nsef_create(gp, fun, 0))
                return NULL;
        }
        break;
    default: //2
        if (!(hash=czl_sys_fun_hash_create(gp, name)))
            return NULL;
        if (!(sys_fun=czl_sys_fun_find(tmp, hash)))
        {
            sprintf(gp->log_buf, "undefined function %s in library %s, ", tmp, name);
            return NULL;
        }
        if (!(fun=czl_sys_fun_create(gp, sys_fun)))
            return NULL;
        break;
    }

    if (!(*node=czl_opr_node_create(gp, CZL_FUN_REF, fun)))
        return NULL;

    return code;
}

char czl_is_unary_opt(czl_gp *gp, char **code, czl_exp_node **node)
{
    char *s = *code;
    unsigned long i, j, len;

    // '-' 优先考虑作为单目运算符
    for (i = 0; i < czl_unary_opt_table_num; i++)
    {
        len = strlen(czl_unary_opt_table[i].name);
        for (j = 0; j < len; j++)
        {
            if (s[j] != czl_unary_opt_table[i].name[j])
                break;
        }
        if (j == len) //匹配上
        {
            //- ! 与 -= != 冲突检测
            if ((CZL_NUMBER_NOT == czl_unary_opt_table[i].macro ||
                 CZL_LOGIC_NOT == czl_unary_opt_table[i].macro) && '=' == s[j])
                return 0;
            //- 与 -> 冲突检测
            if (CZL_NUMBER_NOT == czl_unary_opt_table[i].macro && '>' == s[j])
                return 0;
            //& 与 &= && 冲突检测
            if (CZL_REF_VAR == czl_unary_opt_table[i].macro &&
                ('=' == s[j] || '&' == s[j]))
                return 0;
            *code = s+len;
            *node = czl_unary_opt_node_create
                    (gp, czl_unary_opt_table[i].macro,
                     czl_unary_opt_table[i].position);
            return 1;
        }
    }

    return 0;
}

char czl_is_binary_opt(czl_gp *gp, char **code, czl_exp_node **node)
{
    char *s = *code;
    unsigned long i, j, len;

    for (i = 0; i < czl_binary_opt_table_num; i++)
    {
        len = strlen(czl_binary_opt_table[i].name);
        for (j = 0; j < len; j++)
        {
            if (s[j] != czl_binary_opt_table[i].name[j])
                break;
        }
        if (j == len) //匹配上
        {
            if (CZL_COL == czl_binary_opt_table[i].macro && 0 == gp->tmp->colon_flag)
                return 0;
            *code = s+len;
            *node = czl_binary_opt_node_create
                    (gp, czl_binary_opt_table[i].macro,
                     czl_binary_opt_table[i].priority,
                     czl_binary_opt_table[i].associativity);
            return 1;
        }
    }

    return 0;
}

char* czl_child_exp_analysis_start
(
    czl_gp *gp,
    char *code,
    czl_exp_handle *h
)
{
    h->bc++;
    if (!h->root)
    {
        if (!(code=czl_exp_analysis(gp, code+1, h)))
            return NULL;
        if (h->root)
            h->root->bracket = CZL_BRACKET_YES;
        h->cur = h->root;
    }
    else
    {
        czl_exp_node *node;
        czl_exp_node *root = h->root;
        czl_exp_node *current = h->cur;
        unsigned long cnt = h->mtoc;
        h->root = h->cur = NULL;
        h->mtoc = 0;
        code = czl_exp_analysis(gp, code+1, h);
        if ((node=h->root))
            node->bracket = CZL_BRACKET_YES;
        h->root = root;
        h->cur = current;
        h->mtoc = cnt;
        if (!code || !czl_exp_node_insert(gp, node, h))
            return NULL;
    }

    return code;
}

char* czl_child_exp_analysis_end
(
    czl_gp *gp,
    char *code,
    czl_exp_handle *h
)
{
    h->bc--;

    //表达式树完整新检查
    if (!czl_exp_integrity_check(gp, h))
        return NULL;

    return code+1;
}

char* czl_exp_analysis(czl_gp *gp, char *code, czl_exp_handle *h)
{
    czl_exp_node *node;

    code = czl_ignore_sign_filt(gp, code);

    for (node = NULL; *code != '\0'; node = NULL)
    {
        //括号
        if ('(' == *code)
        {
            if (!(code=czl_child_exp_analysis_start(gp, code, h)))
                return NULL;
            continue;
        }
        else if (')' == *code)
        {
            if (0 == h->bc)
                return czl_exp_integrity_check(gp, h) ? code : NULL;
            return czl_child_exp_analysis_end(gp, code, h);
        }
        //单目运算符，注意：必须在双目运算符前判断
        else if (czl_is_unary_opt(gp, &code, &node)) {}
        //双目运算符
        else if (czl_is_binary_opt(gp, &code, &node)) {}
        else
        {
            //检查表达式是否已经完整
            if (czl_exp_is_integrity(gp, h, *code))
                return czl_exp_integrity_check(gp, h) ? code : NULL;
            //函数指针
            if ('@' == *code)
            {
                if (!(code=czl_fun_addr_match(gp, code+1, &node)))
                    return NULL;
            }
            //常量: 数字或字符串
            else if (czl_is_constant(gp, &code, &node)) {}
            //变量或函数
            else if (czl_is_var_or_fun(gp, &code, &node, NULL)) {}
            //数组list或表list [ {
            else if (czl_is_array_or_table(gp, &code, &node)) {}
            else //表达式树完整性检查
                return czl_exp_integrity_check(gp, h) ? code : NULL;
        }
        if (!czl_exp_node_insert(gp, node, h))
            return NULL;
        code = czl_ignore_sign_filt(gp, code);
    }

    return NULL;
}

char* czl_expression_analysis
(
    czl_gp *gp,
    char *code
)
{
	czl_exp_handle h = {NULL, NULL, 0, 0};
    unsigned long reg_cnt_backup = gp->reg_cnt;
    unsigned char reg_flag_backup = gp->tmp->reg_flag;
    gp->tmp->reg_flag = 0;
    gp->tmp->reg_sign = 0;
    ++gp->tmp->exp_flag;

    if (!(code=czl_exp_analysis(gp, code, &h)))
        return NULL;

    gp->reg_cnt = reg_cnt_backup;
    gp->tmp->reg_flag = reg_flag_backup;
    --gp->tmp->exp_flag;

    //树转栈，目的是为了计算表达式时避免递归耗时和
    //减少表达式节点里没价值的指针域占用的内存。
    if (!czl_exp_tree_to_stack(gp, h.root))
        return NULL;

    return code;
}

char* czl_exp_sentence_match(czl_gp *gp, char *code)
{
    if (!(code=czl_is_exp_right(gp, code)))
        return NULL;

    if (!(code=czl_end_sign_check(gp, code)))
        return NULL;

    return code;
}

char* czl_sentence_match(czl_gp *gp, char *code)
{
    if (!(code=czl_exp_sentence_match(gp, code)))
        return NULL;

    if (gp->tmp->exp_head) //不存空语句
    {
        //创建表达式语句块
        if (!czl_sentence_block_create(gp, CZL_EXP_SENTENCE))
            return NULL;
    }

    return code;
}

char* czl_var_create
(
    czl_gp *gp,
    char *code,
    czl_var **node,
    char quality,
    char ot
)
{
    char name[CZL_NAME_MAX_SIZE];
    unsigned char var_field_backup;

    //过滤变量名
    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "variable name should be within _/a~z/A~Z/0~9 and can not be 0~9 firstly, ");
        return NULL;
    }
    //变量名不能与系统关键字同名
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    //变量与常量在同一作用域不能同名
    var_field_backup = gp->tmp->variable_field;
    gp->tmp->variable_field = CZL_IN_ANY;
    //变量是否存在
    if (czl_var_find(gp, name, CZL_LOCAL_FIND))
    {
        sprintf(gp->log_buf, "redefined variable %s, ", name);
        return NULL;
    }
    //检查是否已有存储器
    if (!czl_store_device_check(gp))
        return NULL;
    //创建变量节点
    if (!(*node=czl_var_create_by_field(gp, name, quality, ot)))
        return NULL;
    gp->tmp->variable_field = var_field_backup;

    return code;
}

char czl_default_para_type_check(czl_gp *gp, char ot)
{
    if (CZL_NIL == ot)
        return 1;

    switch (gp->tmp->exp_head->kind)
    {
    case CZL_FUN_REF:
        return (CZL_FUN_REF == ot ? 1 : 0);
    case CZL_ARRAY_LIST:
        return (CZL_ARRAY == ot ? 1 : 0);
    case CZL_TABLE_LIST:
        return (CZL_TABLE == ot ? 1 : 0);
    default:
        switch (gp->tmp->exp_head->res->type)
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
}

char* czl_var_def(czl_gp *gp, char *code, char quality, char ot)
{
    char const_init_flag = 0;
	czl_var *last_num = NULL;

    if (quality != CZL_DYNAMIC_VAR || CZL_IN_CLASS == gp->tmp->analysis_field)
    {
        const_init_flag = 1;
        gp->tmp->variable_field = CZL_IN_CONSTANT;
    }

    for (;;)
    {
        char *tmp = code;
        int error_line_backup = gp->error_line;
        czl_var *var;
        if (!(code=czl_var_create(gp, code, &var, quality, ot)))
            return NULL;

        if (const_init_flag)
        {
            code = czl_ignore_sign_filt(gp, code);
            if ('=' == *code)
            {
                if (!(code=czl_is_exp_right(gp, code+1)))
                    return NULL;
                if (!gp->tmp->exp_head)
                {
                    sprintf(gp->log_buf, "init para of %s should not be empty, ", var->name);
                    return NULL;
                }
                if (!(czl_default_para_type_check(gp, ot)))
                {
                    sprintf(gp->log_buf, "type of init para not matched %s, ", var->name);
                    return NULL;
                }
                if (!czl_const_exp_init(gp, var, quality))
                    return NULL;
            }
            else if (CZL_CONST_VAR == quality && last_num)
            {
                //没初始化枚举常量默认是前一个数值常量加一，如果没有则为零
                var->type = last_num->type;
                if (CZL_INT == last_num->type)
                    var->val.inum = last_num->val.inum + 1;
                else //CZL_FLOAT
                    var->val.fnum = last_num->val.fnum + 1;
            }
            if (CZL_CONST_VAR == quality &&
                (CZL_INT == var->type || CZL_FLOAT == var->type))
                last_num = var;
        }
        else
        {
            code = czl_ignore_sign_filt(gp, code);
            if ('=' == *code)
            {
                gp->error_line = error_line_backup;
                if (!(code=czl_is_exp_right(gp, tmp)))
                    return NULL;
                //创建变量定义语句块
                if (!czl_sentence_block_create(gp, CZL_EXP_SENTENCE))
                    return NULL;
            }
        }

        code = czl_ignore_sign_filt(gp, code);
        if (',' == *code)
            ++code;
        else
        {
            gp->tmp->variable_field = CZL_IN_ANY;
            return ';' == *code ? code+1 : code;
        }
    }
}

char* czl_const_def(czl_gp *gp, char *code)
{
    czl_enum *node;

    if (!czl_store_device_check(gp))
        return NULL;
    if (!(node=czl_enum_node_create(gp)))
        return NULL;
    czl_enum_insert(gp, node);
    return czl_var_def(gp, code+1, CZL_CONST_VAR, CZL_NIL);
}

char czl_create_new_sentence
(
    czl_gp *gp,
    czl_var *var,
    czl_new_sentence *newObj,
    char const_init_flag
)
{
	czl_exp_ele *exp;
    czl_var *ele = czl_ele_create(gp, CZL_NEW, newObj);
    if (!ele)
    {
        czl_new_sentence_delete(gp, newObj);
        return 0;
    }

    if (const_init_flag)
    {
        if (!(exp=(czl_exp_ele*)CZL_TMP_MALLOC(gp, sizeof(czl_exp_ele))))
            return 0;
        exp->res = ele;
        exp->flag = CZL_OPERAND;
        exp->lt = CZL_OPERAND;
        exp->rt = 0; //标记内存的分配来源
    }
    else
    {
        if (!(exp=(czl_exp_ele*)CZL_TMP_CALLOC(gp, 2, sizeof(czl_exp_ele))))
            return 0;
        exp->msg.em.err_file = gp->error_file;
        exp->msg.em.err_line = gp->error_line;
        exp->flag = CZL_BINARY_OPT;
        exp->kind = CZL_ASS;
        exp->lt = CZL_LG_VAR;
        exp->lo = var;
        exp->rt = CZL_NEW;
        exp->ro = ele;
        exp[1].flag = CZL_OP_END;
        exp[1].rt = 0; //标记内存的分配来源
    }

    gp->tmp->exp_head = exp;
    return 1;
}

char* czl_obj_paras_def
(
    czl_gp *gp,
    char *code,
    czl_var *var,
    char type,
    char const_init_flag,
    czl_class *pclass
)
{
	czl_exp_ele *len = NULL;
    czl_new_sentence *newObj = czl_new_sentence_create(gp, type);
    if (!newObj)
        return NULL;

	++gp->tmp->exp_flag;

    if (type != CZL_TABLE)
    {
        if (!(code=czl_get_arr_len(gp, code, &len)))
            goto CZL_SYNTAX_ERROR;
        code = czl_ignore_sign_filt(gp, code);
    }

    switch (type)
    {
    case CZL_INSTANCE:
        newObj->new_obj.ins.pclass = pclass;
        newObj->new_obj.ins.init_fun = NULL;
        newObj->new_obj.ins.len = len;
        if ('(' == *code)
        {
			czl_fun *fun;
            if (const_init_flag)
            {
                sprintf(gp->log_buf, "init static instance should not be with structure function, ");
                goto CZL_SYNTAX_ERROR;
            }
            if (!(fun=czl_fun_node_find(newObj->new_obj.ins.pclass->name,
									    &newObj->new_obj.ins.pclass->funs_hash)))
            {
                sprintf(gp->log_buf, "undefined structure function in class %s, ", pclass->name);
                goto CZL_SYNTAX_ERROR;
            }
            if (!(code=czl_exp_fun_build(gp, code+1,
                                         &newObj->new_obj.ins.init_fun,
                                         fun, CZL_STATIC_FUN)))
                goto CZL_SYNTAX_ERROR;
        }
        break;
    case CZL_ARRAY: case CZL_STACK: case CZL_QUEUE:
        newObj->new_obj.arr.init_list = NULL;
        newObj->new_obj.arr.len = len;
        if ('=' == *code)
        {
            code = czl_ignore_sign_filt(gp, code+1);
            if (*code != '[')
            {
                sprintf(gp->log_buf, "init para of variable %s should be an array, ", var->name);
                goto CZL_SYNTAX_ERROR;
            }
            if (!(code=czl_array_list_build
                  (gp, code+1, &newObj->new_obj.arr.init_list)))
                goto CZL_SYNTAX_ERROR;
        }
        break;
    case CZL_STRING:
        newObj->new_obj.arr.init_list = NULL;
        newObj->new_obj.arr.len = len;
        if ('=' == *code)
        {
			czl_var *str;
            code = czl_ignore_sign_filt(gp, code+1);
            if (*code != '\"')
            {
                sprintf(gp->log_buf, "init para of variable %s should be a string, ", var->name);
                goto CZL_SYNTAX_ERROR;
            }
            if (!(code=czl_str_match(gp, code, &str)))
                goto CZL_SYNTAX_ERROR;
            newObj->new_obj.arr.init_list = (czl_array_list*)str;
        }
        break;
    default: //CZL_TABLE
        break;
    }

    if (!czl_create_new_sentence(gp, var, newObj, const_init_flag))
        return NULL;

    --gp->tmp->exp_flag;
    return code;

CZL_SYNTAX_ERROR:
    czl_new_sentence_delete(gp, newObj);
    return NULL;
}

char* czl_objs_def
(
    czl_gp *gp,
    char *code,
    char quality,
    char type,
    czl_class *pclass
)
{
    char const_init_flag = 0;

    if (CZL_STATIC_VAR == quality ||
            CZL_IN_CLASS == gp->tmp->analysis_field)
    {
        const_init_flag = 1;
        gp->tmp->variable_field = CZL_IN_CONSTANT;
    }

    for (;;)
    {
		czl_var *var;
        char *tmp = code = czl_ignore_sign_filt(gp, code);
		unsigned long error_line_backup = gp->error_line;
        if (!(code=czl_var_create(gp, code, &var, quality, type)))
            return NULL;
        code = czl_ignore_sign_filt(gp, code);
        if ('=' == *code)
        {
            if (const_init_flag)
            {
                if (!(code=czl_is_exp_right(gp, code+1)))
                    return NULL;
            }
            else
            {
                gp->error_line = error_line_backup;
                if (!(code=czl_is_exp_right(gp, tmp)))
                    return NULL;
            }
        }
        else if (!(code=czl_obj_paras_def(gp, code, var, type,
                                          const_init_flag, pclass)))
            return NULL;

        if (!(const_init_flag ? czl_const_exp_init(gp, var, quality) :
            czl_sentence_block_create(gp, CZL_EXP_SENTENCE)))
            return NULL;

        code = czl_ignore_sign_filt(gp, code);
        if (',' == *code)
            ++code;
        else
        {
            gp->tmp->variable_field = CZL_IN_ANY;
            return ';' == *code ? code+1 : code;
        }
    }
}

char* czl_instance_def(czl_gp *gp, char *code, char quality)
{
	czl_class *pclass;
    char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "instance name should be within _/a~z/A~Z/0~9 and can not be 0~9 firstly, ");
        return NULL;
    }

    if (!(pclass=czl_class_find(gp, name)))
    {
        sprintf(gp->log_buf, "undefined class %s, ", name);
        return NULL;
    }
    if (CZL_NOT_SURE == pclass->flag)
    {
        sprintf(gp->log_buf, "unsure class %s, ", name);
        return NULL;
    }

    return czl_objs_def(gp, code, quality, CZL_INSTANCE, pclass);
}

char* czl_obj_def(czl_gp *gp, char *code, int index, char quality)
{
    switch (index)
    {
    case CZL_VAR_INDEX:
        return czl_var_def(gp, code, quality, CZL_NIL);
    case CZL_INT_INDEX:
        return czl_var_def(gp, code, quality, CZL_INT);
    case CZL_FLOAT_INDEX:
        return czl_var_def(gp, code, quality, CZL_FLOAT);
    case CZL_NUM_INDEX:
        return czl_var_def(gp, code, quality, CZL_NUM);
    case CZL_FILE_INDEX:
        return czl_var_def(gp, code, quality, CZL_FILE);
    case CZL_FUN_INDEX:
        return czl_var_def(gp, code, quality, CZL_FUN_REF);
    case CZL_INS_INDEX:
        return czl_instance_def(gp, code, quality);
    case CZL_TABLE_INDEX:
        return czl_objs_def(gp, code, quality, CZL_TABLE, NULL);
    case CZL_STR_INDEX:
        return czl_objs_def(gp, code, quality, CZL_STRING, NULL);
    case CZL_ARR_INDEX:
        return czl_objs_def(gp, code, quality, CZL_ARRAY, NULL);
    case CZL_STACK_INDEX:
        return czl_objs_def(gp, code, quality, CZL_STACK, NULL);
    case CZL_QUEUE_INDEX:
        return czl_objs_def(gp, code, quality, CZL_QUEUE, NULL);
    case CZL_CONST_INDEX:
        return czl_const_def(gp, code);
    default:
        if (CZL_STATIC_VAR == quality)
            return czl_var_def(gp, code, quality, CZL_NIL);
        sprintf(gp->log_buf, "%s used error in this scope, ", czl_keyword_table[index-1].name);
        return NULL;
    }
}

char* czl_foreach_obj_end(czl_gp *gp, char *code)
{
	unsigned long cur_line;
    czl_exp_node *node = NULL;
    char flag = 0;

    code = czl_ignore_sign_filt(gp, code);
    cur_line = gp->error_line;

    if (!czl_is_var_or_fun(gp, &code, &node, &flag) || !node)
    {
        if (gp->tmp->foreach_type)
        {
            sprintf(gp->log_buf, "& should be with a variable, ");
            return NULL;
        }
        if ('\"' == *code && !flag)
        {
            if (!(code=czl_string_match(gp, code, &node)))
                return NULL;
        }
        else
        {
            ++gp->tmp->exp_flag;
            if ('[' == *code && !flag)
            {
                if (!(code=czl_array_list_match(gp, code, &node)))
                    return NULL;
            }
            else if ('(' == *code)
            {
				unsigned long paras_count;
                if (0 == flag || 1 == flag)
                    gp->tmp->foreach_type = 4;
                else
                    gp->tmp->foreach_type = 5;
                if (!(code=czl_paras_match(gp, code+1,
                                           &gp->tmp->for_paras_end,
                                           &paras_count, 0, 0, ')')))
                    return NULL;
                if (paras_count != 2 && paras_count != 3)
                {
                    sprintf(gp->log_buf, "paras number of for range should be 2/3, ");
                    return NULL;
                }
            }
            else
                return NULL;
            --gp->tmp->exp_flag;
        }
        if (gp->tmp->for_paras_end)
            return code;
    }
    else if (CZL_IS_NOT_VAR(node))
    {
        if (CZL_FUNCTION == node->type || CZL_MEMBER == node->type)
            gp->tmp->foreach_type = 3;
        else if (2 == gp->tmp->foreach_type)
        {
            sprintf(gp->log_buf, "end para of for should be a variable or function, ");
            return NULL;
        }
    }

    if (!(gp->tmp->for_paras_end=(czl_para*)czl_opr_create(gp, node, 1)))
        return NULL;
    ((czl_exp_ele*)gp->tmp->for_paras_end)->msg.em.err_line = cur_line;
    --gp->tmp->exp_node_cnt;

    return code;
}

char* czl_foreach_dynamic_var_create
(
    czl_gp *gp,
    char *code,
    czl_exp_node **node
)
{
	czl_var *var;
    char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed variable after for, ");
        return NULL;
    }
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    if (!(var=czl_dynamic_var_create(gp, name)))
        return NULL;
    if (!(*node=czl_opr_node_create(gp, CZL_LG_VAR, var)))
        return NULL;

    return code;
}

char* czl_foreach_obj_start_para(czl_gp *gp, char *code, char flag)
{
	czl_exp_ele *opr;
    czl_exp_node *node = NULL;

    if (!czl_is_var_or_fun(gp, &code, &node, NULL) || !node)
    {
        sprintf(gp->log_buf, "syntax error, ");
        if (!(code=czl_foreach_dynamic_var_create(gp, code, &node)))
            return NULL;
    }
    else if (CZL_IS_NOT_VAR(node))
    {
        sprintf(gp->log_buf, "start para of for should be a variable, ");
        return NULL;
    }

    if (!(opr=czl_opr_create(gp, node, 1)))
        return NULL;
    --gp->tmp->exp_node_cnt;

    if (flag)
        gp->tmp->for_paras_start = (czl_para*)opr;
    else
        gp->tmp->for_condition = opr;

    return code;
}

char* czl_foreach_obj_start(czl_gp *gp, char *code)
{
    if (!(code=czl_foreach_obj_start_para(gp, code, 1)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (',' == *code && !(code=czl_foreach_obj_start_para(gp, code+1, 0)))
        return NULL;

    return code;
}

char* czl_foreach_paras_match(czl_gp *gp, char *code)
{
	char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_foreach_obj_start(gp, code)))
        return NULL;
    
    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed in/key after for, ");
        return NULL;
    }

    if (strcmp("in", name) == 0)
    {
        code = czl_ignore_sign_filt(gp, code);
        if ('&' == *code)
        {
            if (CZL_MEMBER == ((czl_exp_ele*)gp->tmp->for_paras_start)->kind)
            {
                sprintf(gp->log_buf, "member of object should not be &, ");
                return NULL;
            }
            ++code;
            gp->tmp->foreach_type = 2;
        }
        else
            gp->tmp->foreach_type = 0;
    }
    else if (strcmp("key", name) == 0)
        gp->tmp->foreach_type = 1;
    else
    {
        sprintf(gp->log_buf, "keyword of for should be in/key, ");
        return NULL;
    }

    return czl_foreach_obj_end(gp, code);
}

char* czl_for_condition_match(czl_gp *gp, char *code)
{
    code = czl_ignore_sign_filt(gp, code);
    if (*code != '(')
    {
        gp->tmp->current_loop_type = CZL_FOREACH_LOOP;
        return czl_foreach_paras_match(gp, code);
    }

    gp->tmp->current_loop_type = CZL_FOR_LOOP;
    if (!(code=czl_paras_match(gp, code+1,
                               &gp->tmp->for_paras_start,
                               NULL, 2, 0, ';')))
        return NULL;

    if (!(code=czl_exp_sentence_match(gp, code)))
        return NULL;
    gp->tmp->for_condition = gp->tmp->exp_head;
    gp->tmp->exp_head = NULL;

    return czl_paras_match(gp, code,
                           &gp->tmp->for_paras_end,
                           NULL, 2, 0, ')');
}

char* czl_try_flag_match(czl_gp *gp, char *code)
{
    char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "try missed exit/break/continue, ");
        return NULL;
    }

    if (strcmp("exit", name) == 0)
        gp->tmp->try_type = CZL_TRY_EXIT;
    else if (strcmp("break", name) == 0)
        gp->tmp->try_type = CZL_TRY_BREAK;
    else if (strcmp("continue", name) == 0)
        gp->tmp->try_type = CZL_TRY_CONTINUE;
    else
    {
        gp->tmp->try_type = CZL_TRY_GOTO;
        if (!(gp->tmp->goto_flag_name=(char*)CZL_TMP_MALLOC(gp, strlen(name)+1)))
            return NULL;
        strcpy(gp->tmp->goto_flag_name, name);
    }

    return code;
}

char* czl_try_condition_match(czl_gp *gp, char *code)
{
    gp->tmp->try_type = CZL_TRY_CONTINUE;

    code = czl_ignore_sign_filt(gp, code);
    if (*code != '(')
        return code;

    if (!(code=czl_try_flag_match(gp, code+1)))
        return NULL;

    code = czl_ignore_sign_filt(gp, code);
    if (')' == *code)
        return code+1;
    else if (',' == *code)
        code++;

    return czl_paras_match(gp, code,
                           &gp->tmp->try_paras,
                           NULL, 0, 0, ')');
}

char* czl_condition_match(czl_gp *gp, char *code, char while_flag)
{
    gp->tmp->condition_flag = 1;
    if (!(code=czl_expression_analysis(gp, code)))
        return NULL;
    gp->tmp->condition_flag = 0;

    if (!gp->tmp->exp_head && !while_flag)
    {
        sprintf(gp->log_buf, "logic condition should not be empty, ");
        return NULL;
    }

    return code;
}

char* czl_cases_condition_match(czl_gp *gp, char *code)
{
    return czl_paras_match(gp, code, &gp->tmp->branch_child_paras, NULL, 0, 0, ':');
}

char czl_elif_condition_create(czl_gp *gp)
{
    //之所以将elif独立处理，是为了适配 elif(else if) 与 case 数据结构格式
    gp->tmp->branch_child_paras = czl_para_node_create(gp, 0);
    return (gp->tmp->branch_child_paras ? 1 : 0);
}

char czl_condition_block_check
(
    czl_gp *gp,
    int index,
    char **code,
    char *do_while_end
)
{
	czl_block_struct *p;

    if (!gp->tmp->block_count)
        return 0;

    p = gp->tmp->block_stack + gp->tmp->block_count - 1;
    if (CZL_NOT_SURE == p->flag)
    {
        if ((CZL_IF_INDEX == index ||
             CZL_ELIF_INDEX == index ||
             CZL_ELSE_INDEX == index) &&
            (CZL_IF_INDEX == p->index ||
             CZL_ELIF_INDEX == p->index ||
             CZL_ELSE_INDEX == p->index))
        {
            if (CZL_IF_INDEX == index)
            {
                //else if 等效 elif 处理
                if (CZL_ELSE_INDEX == p->index)
                {
                    p->index = CZL_ELIF_INDEX;
                    return 1;
                }
                //if if 或 elif if 第一个 if/elif 标记不为空
                else
                    p->flag = CZL_SURE;
            }
            else
            {
                //if elif else 语句均不能为空
                sprintf(gp->log_buf, "block of branch should bot be empty, ");
                return -1;
            }
        }
        else
        {
            p->flag = CZL_SURE;
        }
    }
    else
    {
        if (CZL_FOR_INDEX == p->index ||
            CZL_WHILE_INDEX == p->index ||
            CZL_DO_INDEX == p->index ||
            CZL_TRY_INDEX == p->index)
        {
            //do while 语句结束
            if (CZL_DO_INDEX == p->index)
            {
                if (index != CZL_WHILE_INDEX)
                {
                    sprintf(gp->log_buf, "missed while for do, ");
                    return -1;
                }
                if (!(*code=czl_condition_match(gp, *code, 0)))
                    return -1;
                gp->tmp->code_blocks[gp->tmp->code_blocks_count-1].
                        block.loop->condition = gp->tmp->exp_head;
                gp->tmp->exp_head = NULL;
                *do_while_end = 1;
            }
            gp->tmp->block_count--;
            //退出for while语句块
            gp->tmp->code_blocks_count--;
            return czl_condition_block_check(gp, index, code, do_while_end);
        }
        else if ((CZL_ELSE_INDEX == p->index) ||
                  ((CZL_IF_INDEX == p->index ||
                    CZL_ELIF_INDEX == p->index) &&
                   (index != CZL_ELIF_INDEX &&
                    index != CZL_ELSE_INDEX)))
        {
            czl_if_elif_else_pop(gp);
            //退出if/elif/else语句块
            gp->tmp->code_blocks_count--;
            return czl_condition_block_check(gp, index, code, do_while_end);
        }
        //上面递归调用 czl_condition_block_check(gp, index);
        //是为了纠正以下解释错误情况：
        /*
        fun ()
        {
            if (1) //或 while
                if (1) //或 while
                    ;
        }
        */
    }

    return 0;
}

char* czl_goto_flag_match(czl_gp *gp, char *code)
{
    char name[CZL_NAME_MAX_SIZE];

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed goto_flag for goto, ");
        return NULL;
    }

    if (!(gp->tmp->goto_flag_name=(char*)CZL_TMP_MALLOC(gp, strlen(name)+1)))
        return NULL;
    strcpy(gp->tmp->goto_flag_name, name);

    if (!(code=czl_end_sign_check(gp, code)))
        return NULL;

    return code;
}

char czl_code_block_create(czl_gp *gp, int index)
{
    char block_type;

    switch (index)
    {
    case CZL_IF_INDEX:
        block_type = CZL_BRANCH_BLOCK;
        gp->tmp->current_branch_type = CZL_IF_BRANCH;
        break;
    case CZL_ELIF_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_ELSE_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_TRY_INDEX:
        block_type = CZL_TRY_BLOCK;
        break;
    case CZL_FOR_INDEX:
        block_type = CZL_LOOP_BLOCK;
        break;
    case CZL_WHILE_INDEX:
        block_type = CZL_LOOP_BLOCK;
        gp->tmp->current_loop_type = CZL_WHILE_LOOP;
        break;
    case CZL_DO_INDEX:
        block_type = CZL_LOOP_BLOCK;
        gp->tmp->current_loop_type = CZL_DO_WHILE_LOOP;
        break;
    case CZL_SWITCH_INDEX:
        block_type = CZL_BRANCH_BLOCK;
        gp->tmp->current_branch_type = CZL_SWITCH_BRANCH;
        break;
    case CZL_CASE_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_DEFAULT_INDEX:
        block_type = CZL_BRANCH_CHILD_BLOCK;
        break;
    case CZL_BREAK_INDEX:
        block_type = CZL_BREAK_SENTENCE;
        break;
    case CZL_CONTINUE_INDEX:
        block_type = CZL_CONTINUE_SENTENCE;
        break;
    case CZL_RETURN_INDEX:
        block_type = CZL_RETURN_SENTENCE;
        break;
    case CZL_YEILD_INDEX:
        block_type = CZL_YEILD_SENTENCE;
        break;
    default: //CZL_GOTO_INDEX
        block_type = CZL_GOTO_SENTENCE;
        break;
    }

    return czl_sentence_block_create(gp, block_type);
}

char czl_switch_check(czl_gp *gp, char *code)
{
    int index;
    int error_line_backup = gp->error_line;

    code = czl_ignore_sign_filt(gp, code);
    if (*code != '{')
    {
        sprintf(gp->log_buf, "missed '{' for switch, ");
        return 0;
    }

    czl_keyword_find(gp, code+1, &index, 0);
    if (index != CZL_CASE_INDEX)
    {
        sprintf(gp->log_buf, "missed case for switch, ");
        return 0;
    }

    gp->error_line = error_line_backup;

    return 1;
}

char* czl_code_block_check(czl_gp *gp, char *code, int index)
{
    if (!czl_block_context_check(gp, index))
        return NULL;

    if (!czl_code_blocks_count(gp, index))
        return NULL;

    switch (index)
    {
    case CZL_DO_INDEX: case CZL_ELSE_INDEX:
        break;
    case CZL_DEFAULT_INDEX:
        code = czl_ignore_sign_filt(gp, code);
        if (':' == *code)
            ++code;
        break;
    case CZL_FOR_INDEX:
        if (!(code=czl_for_condition_match(gp, code)))
            return NULL;
        break;
    case CZL_TRY_INDEX:
        if (!(code=czl_try_condition_match(gp, code)))
            return NULL;
        break;
    case CZL_WHILE_INDEX:
        if (!(code=czl_condition_match(gp, code, 1)))
            return NULL;
        break;
    case CZL_CASE_INDEX:
        if (!(code=czl_cases_condition_match(gp, code)))
            return NULL;
        break;
    //IF_INDEX/ELIF_INDEX/SWITCH_INDEX
    default:
        if (!(code=czl_condition_match(gp, code, 0)))
            return NULL;
        if (CZL_ELIF_INDEX == index)
        {
            if (!czl_elif_condition_create(gp))
                return NULL;
        }
        else if (CZL_SWITCH_INDEX == index)
        {
            if (!czl_switch_check(gp, code))
                return NULL;
        }
        break;
    }

    //创建条件语句块
    if (!czl_code_block_create(gp, index))
        return NULL;

    return code;
}

char* czl_context_analysis(czl_gp *gp, char *code, int index)
{
    int ret;
    char do_while_end = 0;

    //以下1、2、3的步骤不能换

    //1
    ret = czl_condition_block_check(gp, index, &code, &do_while_end);
    if (1 == ret)
    {
        //修正 else 语句块为 elif 语句块
        if (!(code=czl_condition_match(gp, code, 0)) ||
            !czl_elif_condition_create(gp))
            return NULL;
        czl_else_block_to_elif(gp);
        return code;
    }
    else if (-1 == ret)
        return NULL;

    if (do_while_end)
        return code;

    //2
    if (!index)
    {
        //表达式语句
        return czl_sentence_match(gp, code);
    }

    //3
    switch (index)
    {
    case CZL_IF_INDEX: case CZL_ELIF_INDEX: case CZL_ELSE_INDEX:
    case CZL_FOR_INDEX: case CZL_WHILE_INDEX: case CZL_DO_INDEX:
    case CZL_SWITCH_INDEX: case CZL_CASE_INDEX: case CZL_DEFAULT_INDEX:
    case CZL_TRY_INDEX:
        return czl_code_block_check(gp, code, index);
    case CZL_LBRACKET_INDEX:
        if (!czl_code_blocks_count(gp, index))
            return NULL;
        break;
    case CZL_RBRACKET_INDEX:
        if (!czl_block_context_check(gp, index))
            return NULL;
        break;
    case CZL_BREAK_INDEX: case CZL_CONTINUE_INDEX:
        if (!czl_block_context_check(gp, index))
            return NULL;
        if (!(code=czl_end_sign_check(gp, code)))
            return NULL;
        if (!czl_code_block_create(gp, index))
            return NULL;
        break;
    case CZL_RETURN_INDEX: case CZL_YEILD_INDEX:
        if (CZL_YEILD_INDEX == index && gp->cur_fun->para_explain)
        {
            sprintf(gp->log_buf, "coroutine %s should not have default paras, ", gp->cur_fun->name);
            return NULL;
        }
        if (!(code=czl_exp_sentence_match(gp, code)))
            return NULL;
        if (!czl_code_block_create(gp, index))
            return NULL;
        break;
    case CZL_GOTO_INDEX:
        if (!(code=czl_goto_flag_match(gp, code)))
            return NULL;
        if (!czl_code_block_create(gp, index))
            return NULL;
        break;
    case CZL_THIS_INDEX:
        return czl_sentence_match(gp, code-4);
    case CZL_STATIC_INDEX:
        code = czl_keyword_find(gp, code, &index, 0);
        return czl_obj_def(gp, code, index, CZL_STATIC_VAR);
    default:
        return czl_obj_def(gp, code, index, CZL_DYNAMIC_VAR);
    }

    return code;
}

char* czl_fun_body_analysis(czl_gp *gp, char *code)
{
    int index;

    while (*code != '\0')
    {
        if (!(code=czl_keyword_find(gp, code, &index, 1)))
            return NULL;
        if (!(code=czl_context_analysis(gp, code, index)))
            return NULL;
        code = czl_ignore_sign_filt(gp, code);
        if (0 == gp->tmp->block_count)
            break;
    }

    if (gp->tmp->block_count)
    {
        sprintf(gp->log_buf, "missed '}' for end defined a function, ");
        return NULL;
    }

    return code;
}

czl_fun* czl_usr_fun_create(czl_gp *gp, char *name, char state, char ot)
{
	char fun_type;
	czl_fun *node;

    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    if (CZL_IN_GLOBAL == gp->tmp->analysis_field ||
            CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field)
        fun_type = CZL_USR_GLOBAL_FUN;
    else
        fun_type = CZL_USR_CLASS_FUN;
    node = czl_fun_node_create(gp, name, state, fun_type, ot, NULL);
    if (!node || !czl_fun_insert(gp, node))
        return NULL;

    return node;
}

char czl_get_para_ot(czl_gp *gp, char *name)
{
    switch (czl_is_keyword(gp, name))
    {
    case CZL_INT_INDEX: return CZL_INT;
    case CZL_FLOAT_INDEX: return CZL_FLOAT;
    case CZL_NUM_INDEX: return CZL_NUM;
    case CZL_STR_INDEX: return CZL_STRING;
    case CZL_INS_INDEX: return CZL_INSTANCE;
    case CZL_TABLE_INDEX: return CZL_TABLE;
    case CZL_ARR_INDEX: return CZL_ARRAY;
    case CZL_STACK_INDEX: return CZL_STACK;
    case CZL_QUEUE_INDEX: return CZL_QUEUE;
    case CZL_FILE_INDEX: return CZL_FILE;
    case CZL_FUN_INDEX: return CZL_FUN_REF;
    default: return CZL_NIL;
    }
}

char* czl_para_dec_check(czl_gp *gp, char *code, unsigned char *ot, unsigned char *ref_flag)
{
	char name[CZL_NAME_MAX_SIZE];
    char *tmp = code = czl_ignore_sign_filt(gp, code);

    *ot = CZL_NIL;
    *ref_flag = 0;

    if ((code=czl_name_match(gp, code, name)))
    {
        *ot = czl_get_para_ot(gp, name);
        if (CZL_NIL == *ot)
            return tmp;
        code = czl_ignore_sign_filt(gp, code);
    }
    else
        code = tmp;

    if ('&' == *code)
        *ref_flag = *code++;

    return code;
}

char* czl_fun_paras_match(czl_gp *gp, char *code, czl_fun *fun)
{
    unsigned long paras_count = 0;

	gp->tmp->variable_field = CZL_IN_CONSTANT; //函数默认参数只能是常量

    code = czl_ignore_sign_filt(gp, code);
    if ('(' == *code)
    {
        char name[CZL_NAME_MAX_SIZE];
        czl_loc_var *tail;
        czl_para_explain *para_explain_tail = NULL;
        czl_para_explain *para;
        unsigned char ref_flag;
        unsigned char ot;
        code = czl_ignore_sign_filt(gp, code+1);
        if (')' == *code)
        {
            ++code;
            goto CZL_END;
        }
        for (;;)
        {
            code = czl_para_dec_check(gp, code, &ot, &ref_flag);
            if (!(code=czl_name_match(gp, code, name)))
            {
                if (0 == paras_count && CZL_NIL == ot)
                    sprintf(gp->log_buf, "missed ')' for end defined function paras, ");
                else
                    sprintf(gp->log_buf, "missed variable after ',', ");
                return NULL;
            }
            if (czl_is_keyword(gp, name))
            {
                sprintf(gp->log_buf, "%s is a keyword, ", name);
                return NULL;
            }
            if (czl_loc_var_find(name, (czl_loc_var*)fun->loc_vars))
            {
                sprintf(gp->log_buf, "redefined variable %s, ", name);
                return NULL;
            }
            if (!czl_loc_var_create(gp, &fun->loc_vars, &tail,
                                    name, ot, CZL_DYNAMIC_VAR))
                return NULL;

            code = czl_ignore_sign_filt(gp, code);
            if ('=' == *code)
            {
                if (ref_flag)
                    return NULL;
                ++gp->tmp->exp_flag;
                code = czl_expression_analysis(gp, code+1);
                if (!code || !gp->tmp->exp_head)
                    return NULL;
                --gp->tmp->exp_flag;
                if (!czl_default_para_type_check(gp, ot))
                {
                    sprintf(gp->log_buf, "type of default para not matched %s, ", name);
                    return NULL;
                }
            }
            else
            {
                gp->tmp->exp_head = NULL;
            }

            if (ref_flag || gp->tmp->exp_head)
            {
                if (!(para=czl_para_explain_create(gp, paras_count, ref_flag)))
                    return NULL;
                czl_para_explain_insert(&fun->para_explain,
                                        &para_explain_tail, para);
            }

            paras_count++;

            code = czl_ignore_sign_filt(gp, code);
            if (',' == *code)
                ++code;
            else if (')' == *code++)
                break;
            else
            {
                sprintf(gp->log_buf, "missed ')' for end defined function paras, ");
                return NULL;
            }
        }
    }

CZL_END:

    //全局main函数规定入参不得多于一个
    if (CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field &&
        0 == strcmp(CZL_MAIN_FUN_NAME, fun->name))
    {
        if (paras_count > 1)
        {
            sprintf(gp->log_buf, "paras number of function main should not be much 1, ");
            return NULL;
        }
        gp->main_err_line = gp->error_line;
        gp->main_err_file = gp->error_file;
    }

    code = czl_ignore_sign_filt(gp, code);
    if (*code != '{')
    {
        sprintf(gp->log_buf, "missed '{' for begin defined function body, ");
        return NULL;
    }

    fun->enter_vars_count = paras_count;

    gp->tmp->variable_field = CZL_IN_ANY;

    return code;
}

char* czl_fun_def(czl_gp *gp, char *code, char ot)
{
    char name[CZL_NAME_MAX_SIZE];
    czl_fun *fun;
    czl_class *pclass = NULL;

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "function name should be within _/a~z/A~Z/0~9 and can not be 0~9 firstly, ");
        return NULL;
    }
    code = czl_ignore_sign_filt(gp, code);
    if (':' == *code)
    {
        if (CZL_IN_CLASS == gp->tmp->analysis_field || *(code+1) != ':')
            return NULL;
        if (!(pclass=czl_class_find(gp, name)))
        {
            sprintf(gp->log_buf, "undefined class %s, ", name);
            return NULL;
        }
        if (!(code=czl_name_match(gp, code+2, name)))
        {
            sprintf(gp->log_buf, "missed function name after ':', ");
            return NULL;
        }
        gp->tmp->cur_class = pclass;
        gp->tmp->analysis_field = CZL_IN_CLASS_FUN;
    }
    else
    {
        if (CZL_IN_GLOBAL == gp->tmp->analysis_field)
            gp->tmp->analysis_field = CZL_IN_GLOBAL_FUN;
        else //CZL_IN_CLASS
            gp->tmp->analysis_field = CZL_IN_CLASS_FUN;
    }

    if ((fun=czl_fun_find(gp, name, CZL_LOCAL_FIND)))
    {
        if (fun->state != CZL_IN_STATEMENT)
        {
            sprintf(gp->log_buf, "redefined function %s, ", name);
            return NULL;
        }
        fun->state = CZL_IN_IDLE;
        fun->ret.ot = ot;
    }
    else
    {
        //类成员函数必须要声明，因为需要标记函数的访问权限
        if (pclass)
        {
            sprintf(gp->log_buf,
                    "undeclared function %s in class %s, ",
                    name, pclass->name);
            return NULL;
        }
        if (!(fun=czl_usr_fun_create(gp, name, CZL_IN_IDLE, ot)))
            return NULL;
    }

    //初始化函数解析作用域参数
    gp->cur_fun = fun;
    gp->tmp->code_blocks[0].type = CZL_FUN_BLOCK;
    gp->tmp->code_blocks[0].block.fun = fun;
    gp->tmp->code_blocks_count = 1;

    if (!(code=czl_fun_paras_match(gp, code, fun)))
        return NULL;

    code = czl_fun_body_analysis(gp, code);
    czl_save_fun_enum_list(gp, fun->store_device_head);
    if (!code)
        return NULL;

    //排序类函数中用到的this变量对应的类，加速类实例函数调用时this参数的传递
    if (CZL_IN_CLASS_FUN == gp->tmp->analysis_field &&
        !czl_this_vars_class_sort(gp, fun))
        return NULL;

    //编译抽象语法树ast为opcode
    if (!czl_ast_serialize(gp, fun))
        return NULL;

    return code;
}

char* czl_fun_dec(czl_gp *gp, char *code)
{
    char name[CZL_NAME_MAX_SIZE];

    for (;;)
    {
		czl_fun *fun;
        if (!(code=czl_name_match(gp, code, name)))
        {
            sprintf(gp->log_buf, "missed function name after func, ");
            return NULL;
        }
        if (czl_is_keyword(gp, name))
        {
            sprintf(gp->log_buf, "%s is a keyword, ", name);
            return NULL;
        }
        if (czl_sys_hash_find(CZL_STRING, CZL_NIL,
                              name, &gp->tmp->cur_class->funs_hash))
        {
            sprintf(gp->log_buf, "redeclared function %s, ", name);
            return NULL;
        }
		fun = czl_undef_static_fun_create(gp, name);
        if (!fun || !czl_nsef_create(gp, fun, 0))
            return NULL;
        code = czl_ignore_sign_filt(gp, code);
        if (',' == *code)
            ++code;
        else
            return ';' == *code ? code+1 : code;
    }
}

//读取脚本
char* czl_read_shell(czl_gp *gp, const char *path, unsigned long *code_size)
{
	int i;
    char ch;
	char *code;
    FILE *out = fopen(path, "r");
    if (!out)
    {
        sprintf(gp->log_buf, "Can't open file %s, ", path);
        return NULL;
    }

    if (fseek(out, 0, SEEK_END))
        goto CZL_END;

    *code_size = ftell(out)+1;
    if (!(code=(char*)CZL_TMP_MALLOC(gp, *code_size)))
        goto CZL_END;

    rewind(out);

    for (i = 0; (ch=fgetc(out)) != EOF; i++)
        code[i] = ch;
    code[i] = '\0';

    fclose(out);
    return code;

CZL_END:
    fclose(out);
    return NULL;
}

//跳转到指定文件名进行解析
char* czl_load_shell(czl_gp *gp, char *old_code)
{
	char *path;
	char *new_code;
	char *error_file_backup;
	unsigned long error_line_backup;
	unsigned long code_size;

    old_code = czl_ignore_sign_filt(gp, old_code);
    if (*old_code != '\"')
    {
        sprintf(gp->log_buf, "missed file path after load, ");
        return NULL;
    }

    if (!(old_code=czl_string_filt(gp, old_code, &path)))
        return NULL;

    if (czl_shell_name_find(gp, path))
    {
        CZL_TMP_FREE(gp, path, strlen(path));
        return old_code;
    }

    new_code = czl_read_shell(gp, path, &code_size);
    if (!new_code || !czl_shell_name_save(gp, path, 1))
    {
        CZL_TMP_FREE(gp, path, strlen(path));
        return NULL;
    }

    error_line_backup = gp->error_line;
    error_file_backup = gp->error_file;
    gp->error_line = 1;
    gp->error_file = gp->tmp->sn_head->name;

    if (!czl_shell_analysis(gp, new_code))
        old_code = NULL;
    else
    {
        gp->error_line = error_line_backup;
        gp->error_file = error_file_backup;
    }

    CZL_TMP_FREE(gp, new_code, code_size);

    return old_code;
}

char* czl_permission_dec(czl_gp *gp, char *code, int index)
{
    code = czl_ignore_sign_filt(gp, code);
    if (*code != ':')
        return NULL;

    switch (index)
    {
    case CZL_PUB_INDEX:
        gp->tmp->permission = CZL_PUBLIC;
        break;
    case CZL_PRO_INDEX:
        gp->tmp->permission = CZL_PROTECTED;
        break;
    default: //CZL_PRI_INDEX
        gp->tmp->permission = CZL_PRIVATE;
        break;
    }

    return code+1;
}

char* czl_is_fun_def(czl_gp *gp, char *code, char ot, char *flag)
{
    int error_line_backup = gp->error_line;
    char *tmp = code;
    char name[CZL_NAME_MAX_SIZE];

	if (CZL_IN_GLOBAL_FUN == gp->tmp->analysis_field ||
        CZL_IN_CLASS_FUN == gp->tmp->analysis_field)
        return NULL;

    if ((code=czl_name_match(gp, code, name)))
    {
        code = czl_ignore_sign_filt(gp, code);
        if ('(' == *code || '{' == *code || ':' == *code)
        {
            gp->error_line = error_line_backup;
            if (!(code=czl_fun_def(gp, tmp, ot)))
            {
                *flag = 0;
                return tmp;
            }
            return code;
        }
    }

    gp->error_line = error_line_backup;
    code = tmp;
    return NULL;
}

char* czl_is_fun_or_obj_def(czl_gp *gp, char *code, int index)
{
    char ot, flag = 1;
	char *tmp;

    switch (index)
    {
    case CZL_INT_INDEX: ot = CZL_INT; break;
    case CZL_FLOAT_INDEX: ot = CZL_FLOAT; break;
    case CZL_NUM_INDEX: ot = CZL_NUM; break;
    case CZL_FILE_INDEX: ot = CZL_FILE; break;
    case CZL_FUN_INDEX: ot = CZL_FUN_REF; break;
    case CZL_INS_INDEX: ot = CZL_INSTANCE; break;
    case CZL_TABLE_INDEX: ot = CZL_TABLE; break;
    case CZL_STR_INDEX: ot = CZL_STRING; break;
    case CZL_ARR_INDEX: ot = CZL_ARRAY; break;
    case CZL_STACK_INDEX: ot = CZL_STACK; break;
    case CZL_QUEUE_INDEX: ot = CZL_QUEUE; break;
    case CZL_VAR_INDEX: return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_NIL);
    case CZL_CONST_INDEX: return czl_const_def(gp, code);
    default: return czl_fun_def(gp, code, CZL_NIL);
    }

    if ((tmp=czl_is_fun_def(gp, code, ot, &flag)))
		return flag ? tmp : NULL;

    switch (index)
    {
    case CZL_INT_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_INT);
    case CZL_FLOAT_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_FLOAT);
    case CZL_NUM_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_NUM);
    case CZL_FILE_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_FILE);
    case CZL_FUN_INDEX:
        return czl_var_def(gp, code, CZL_DYNAMIC_VAR, CZL_FUN_REF);
    case CZL_INS_INDEX:
        return czl_instance_def(gp, code, CZL_DYNAMIC_VAR);
    case CZL_TABLE_INDEX:
        return czl_objs_def(gp, code, CZL_DYNAMIC_VAR, CZL_TABLE, NULL);
    default: //CZL_STR_INDEX/CZL_ARR_INDEX/
             //CZL_STACK_INDEX/CZL_QUEUE_INDEX/CZL_LIS_INDEX
        return czl_objs_def(gp, code, CZL_DYNAMIC_VAR, ot, NULL);
    }
}

char* czl_class_body_match
(
    czl_gp *gp,
    char *code,
    czl_class *pclass,
    char analysis_field,
    char permission,
    czl_class *parent
)
{
    int index;
    czl_class_var_list var_list_backup = gp->tmp->class_vars_tail;

    gp->tmp->cur_class = pclass;
    gp->tmp->permission = CZL_PUBLIC;

    for (;;)
    {
        code = czl_keyword_find(gp, code, &index, 2);
        gp->tmp->analysis_field = CZL_IN_CLASS;
        switch (index)
        {
        case CZL_RBRACKET_INDEX:    //类定义结束
        #ifdef CZL_MM_MODULE
            czl_mm_pool_init(&pclass->pool,
                             CZL_IL(pclass->parents_count,
                                    pclass->vars_count), CZL_MM_OBJ_SP);
        #endif
            pclass->flag = CZL_SURE;
            gp->tmp->analysis_field = analysis_field;
            gp->tmp->permission = permission;
            gp->tmp->cur_class = parent;
            gp->tmp->class_vars_tail = var_list_backup;
            return code;
        case CZL_PUB_INDEX:         //访问权限声明
        case CZL_PRO_INDEX: case CZL_PRI_INDEX:
            code = czl_permission_dec(gp, code, index);
            break;
        case CZL_STATIC_INDEX:      //静态变量、实例定义
            code = czl_keyword_find(gp, code, &index, 0);
            code = czl_obj_def(gp, code, index, CZL_STATIC_VAR);
            break;
        case CZL_CLASS_INDEX: //类定义
            code = czl_class_def(gp, code, CZL_IN_CLASS, 0,
                                 gp->tmp->permission, pclass);
            break;
        case CZL_FINAL_INDEX:
            code = czl_keyword_find(gp, code, &index, 0);
            if (index != CZL_CLASS_INDEX)
                return NULL;
            code = czl_class_def(gp, code, CZL_IN_CLASS, 1,
                                 gp->tmp->permission, pclass);
            break;
        case CZL_FUNC_INDEX:
            code = czl_fun_dec(gp, code);
            break;
        default:
            code = czl_is_fun_or_obj_def(gp, code, index);
            break;
        }
        if (!code)
            return NULL;
    }
}

char* czl_class_inherit_match(czl_gp *gp, char *code, czl_class *pclass)
{
    int index;
    char name[CZL_NAME_MAX_SIZE];
    czl_class *parent;
    czl_class_parent *node;
    czl_class_parent *parents_tail;
    char permission;

    for (;;)
    {
        code = czl_keyword_find(gp, code, &index, 0);
        switch (index)
        {
        case CZL_PUB_INDEX:
            permission = CZL_PUBLIC;
            break;
        case CZL_PRO_INDEX:
            permission = CZL_PROTECTED;
            break;
        case CZL_PRI_INDEX:
            permission = CZL_PRIVATE;
            break;
        default:
            permission = CZL_PUBLIC;
            break;
        }
        if (!(code=czl_name_match(gp, code, name)))
        {
            sprintf(gp->log_buf, "missed parent name aftern ':', ");
            return NULL;
        }
        if (!(parent=czl_class_find(gp, name)))
        {
            sprintf(gp->log_buf, "undefined class %s, ", name);
            return NULL;
        }
        if (parent->final_flag)
        {
            sprintf(gp->log_buf, "class %s is final, ", name);
            return NULL;
        }
        if (czl_if_class_parent_repeat(name, pclass))
        {
            sprintf(gp->log_buf, "redeclared parent %s, ", name);
            return NULL;
        }
        if (!(node=czl_class_parent_node_create(gp, parent, permission)))
            return NULL;
        czl_class_parent_node_insert(&pclass->parents,
                                     &parents_tail,
                                     node);
        pclass->parents_count++;
        code = czl_ignore_sign_filt(gp, code);
        switch (*code)
        {
        case ',':
            code++;
            break;
        case '{':
            return code;
        default:
            sprintf(gp->log_buf, "missed '{' for begin define class body, ");
            return NULL;
        }
    }
}

char* czl_class_def
(
    czl_gp *gp,
    char *code,
    char analysis_field,
    char final_flag,
    char permission,
    czl_class *parent
)
{
    char name[CZL_NAME_MAX_SIZE];
    czl_class *node;

    if (!(code=czl_name_match(gp, code, name)))
    {
        sprintf(gp->log_buf, "missed class name after class, ");
        return NULL;
    }
    if (czl_is_keyword(gp, name))
    {
        sprintf(gp->log_buf, "%s is a keyword, ", name);
        return NULL;
    }
    if (czl_class_find(gp, name))
    {
        sprintf(gp->log_buf, "redefined class %s, ", name);
        return NULL;
    }
    code = czl_ignore_sign_filt(gp, code);
    if (*code != '{' && *code != ':')
    {
        sprintf(gp->log_buf, "missed '{' or ':' after class, ");
        return NULL;
    }
    if (!(node=czl_class_create(gp, name, final_flag)))
        return NULL;
    if (!czl_class_insert(gp, node))
        return NULL;
    if (':' == *code && !(code=czl_class_inherit_match(gp, code+1, node)))
        return NULL;

    return czl_class_body_match(gp, code+1, node, analysis_field, permission, parent);
}

//解析脚本生成可执行逻辑结构
char czl_shell_analysis(czl_gp *gp, char *code)
{
    int index;

    while (*code != '\0')
    {
        code = czl_keyword_find(gp, code, &index, 0);
        gp->tmp->analysis_field = CZL_IN_GLOBAL;
        switch (index)
        {
        case CZL_LOAD_INDEX: //加载脚本文件
            code = czl_load_shell(gp, code);
            break;
        case CZL_CLASS_INDEX: //类定义
            code = czl_class_def(gp, code, CZL_IN_GLOBAL, 0, CZL_PUBLIC, NULL);
            break;
        case CZL_FINAL_INDEX:
            code = czl_keyword_find(gp, code, &index, 0);
            if (index != CZL_CLASS_INDEX)
                return 0;
            code = czl_class_def(gp, code, CZL_IN_GLOBAL, 1, CZL_PUBLIC, NULL);
            break;
        default:
            code = czl_is_fun_or_obj_def(gp, code, index);
            break;
        }
        if (!code)
            return 0;
        code = czl_ignore_sign_filt(gp, code);
    }

    return 1;
}

char czl_exec_shell(czl_gp *gp, char *path)
{
    char ret = 1;

    //读取脚本
    unsigned long code_size;
    char *code = czl_read_shell(gp, path, &code_size);
    if (!code)
        ret = 0;

    //保存脚本名
    if (ret && !czl_shell_name_save(gp, path, 0))
        ret = 0;
    gp->error_file = path;

    if (ret) //解析脚本生成抽象语法树ast
        ret = czl_shell_analysis(gp, code);

    CZL_TMP_FREE(gp, code, code_size);

    //脚本完整性检查
    if ((ret=czl_integrity_check(gp, ret)))
        ret = czl_run(gp); //执行脚本

    gp->exceptionCode = CZL_EXCEPTION_NO;
    return ret;
}

void czl_mm_print(czl_gp *gp)
{
    char mode[8] = "%d";
    char modify[64];

//    czl_get_int_mode(mode);
//    strcpy(modify, "gp->mm_max: ");
//    strcat(modify, mode);
//    strcat(modify, "\n");
//    sprintf(gp->log_buf, modify, gp->mm_max);
//    czl_log(gp, gp->log_buf);

    if (gp->mm_cnt != 0)
    {
        mode[1] = 'd';
        czl_get_int_mode(mode);
        strcpy(modify, "memory error, ");
        strcat(modify, mode);
        strcat(modify, "\n");
        sprintf(gp->log_buf, modify, gp->mm_cnt);
        czl_log(gp, gp->log_buf);
    }
}

char czl_global_paras_init(czl_gp *gp)
{
    //初始化log缓存
    strcpy(gp->log_buf, "syntax error, ");

    //初始化脚本语法错误行号、错误提示
    gp->error_line = 1;

    //脚本中断退出标志位初始化
    gp->exit_code = CZL_EXIT_ABNORMAL;

    //初始化异常码
    gp->exceptionCode = CZL_EXCEPTION_NO;

    //变量区域限制标志位初始化
    gp->tmp->variable_field = CZL_IN_ANY;

    //系统关键字哈希表创建
    if (!czl_sys_keyword_hash_create(gp, czl_keyword_table,
                                     czl_keyword_table_num))
        return 0;

    //系统函数哈希表创建
    if (!czl_sys_lib_hash_create(gp))
        return 0;

    return 1;
}

char czl_sys_init(czl_gp *gp, char flag)
{
    if (flag)
    {
    #ifndef CZL_CONSOLE
        char shell_path[CZL_MAX_SHELL_PATH_SIZE];
        char log_path[CZL_MAX_LOG_PATH_SIZE];
        strcpy(shell_path, gp->shell_path);
        strcpy(log_path, gp->log_path);
    #endif //#ifndef CZL_CONSOLE
        memset(gp, 0, sizeof(czl_gp));
    #ifndef CZL_CONSOLE
        strcpy(gp->shell_path, shell_path);
        strcpy(gp->log_path, log_path);
    #endif //#ifndef CZL_CONSOLE
    }

    if (!(gp->tmp=(czl_analysis_gp*)malloc(sizeof(czl_analysis_gp))))
        return 0;
    memset(gp->tmp, 0, sizeof(czl_analysis_gp));

#ifdef CZL_MM_MODULE
    //初始化内存池： 这个必须先执行
    czl_mm_module_init(gp, CZL_MM_SP_HEAP_NUM_MAX/2, 1, CZL_MM_4MB
                       #ifdef CZL_MM_CACHE
                       , 0
                       #endif
                       );
#endif //#ifdef CZL_MM_MODULE

    gp->mm_limit = CZL_MM_3GB;

#ifndef CZL_CONSOLE
    if (!(gp->stack=czl_sq_create(gp, 0)))
        return 0;
#endif //#ifndef CZL_CONSOLE

#ifdef CZL_MULT_THREAD
    if (!flag)
    {
        char ret = 1;
        if (gp->thread_pipe->notify_buf)
            ret = czl_notify_paras_create(gp, &gp->enter_var, gp->thread_pipe);
        gp->thread_pipe->ready = 1;
        if (!ret)
            return 0;
    }
    #ifdef CZL_CONSOLE
    if (!czl_print_lock_init)
    {
        czl_print_lock_init = 1;
    #ifdef CZL_SYSTEM_WINDOWS
        InitializeCriticalSection(&czl_print_cs);
    #elif defined CZL_SYSTEM_LINUX
        pthread_mutex_init(&czl_print_mutex, NULL);
    #endif
    }
    #endif //#ifdef CZL_CONSOLE
#endif //#ifdef CZL_MULT_THREAD

    //字符串元素缓冲区链表头初始化
    if (!(gp->ch_head=(czl_char_var*)CZL_STACK_MALLOC(gp, sizeof(czl_char_var))))
        return 0;
    gp->ch_head->quality = CZL_DYNAMIC_VAR;
    gp->ch_head->next = NULL;

    //初始化czl_global_paras结构
    if (!czl_global_paras_init(gp))
        return 0;

    return 1;
}

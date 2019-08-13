#ifndef CZL_VM_H
#define CZL_VM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include "czl_mm.h"

///////////////////////////////////////////////////////////////
//变量名、函数名、类名等最大长度
#define CZL_NAME_MAX_SIZE           128
//表达式节点最大个数
#define CZL_EXP_NODE_MAX_SIZE       128
//代码嵌套最大层数
#define CZL_MAX_CODE_NEST_LAYER     64
//默认用户函数保存临时结果最大寄存器个数
#define CZL_MAX_REG_CNT             16
//哈希冲突碰撞最大个数限制
#define CZL_MAX_HASH_COLLISION_CNT  64
//对象成员索引最大层数限制
#define CZL_MAX_MEMBER_INDEX_LAYER  16
//栈空间大小限制: 具体大小与系统平台有关
#define CZL_MAX_STACK_SIZE          3000
//异常码个数
#define CZL_EXCEPTION_CODE_NUM      14
//log缓存大小
#define CZL_LOG_BUF_SIZE 128
//脚本路径最大长度
#define CZL_MAX_SHELL_PATH_SIZE 128
//log路径最大长度
#define CZL_MAX_LOG_PATH_SIZE 128
//主函数名
#define CZL_MAIN_FUN_NAME	"main"
//系统库名
#define CZL_LIB_OS_NAME    "os"
///////////////////////////////////////////////////////////////
//变量是否存在&引用: var exist ref
#define CZL_VAR_EXIST_REF(var) \
(var->name && var->type != CZL_OBJ_REF && \
 ((czl_ref_obj*)var->name)->cnt <= CZL_MAX_MEMBER_INDEX_LAYER)
///////////////////////////////////////////////////////////////
//字符串长度: string length
#define CZL_SL(size) (8+size)

//实例长度: instance length
#define CZL_IL(parents_cnt, vars_cnt) \
(12+(parents_cnt)*sizeof(czl_ins*) + (vars_cnt)*sizeof(czl_var))

//获取实例变量数组: get instance vars
#define CZL_GIV(ins) \
((czl_var*)(ins->parents + ins->pclass->parents_count))

//获取对象成员指针长度
#define CZL_RL(cnt) (8+(cnt)*sizeof(void*))
///////////////////////////////////////////////////////////////
//释放字符串: string free
#define CZL_SF(gp, str) CZL_STR_FREE(gp, str.Obj, CZL_SL(str.size))

//重新调整字符串长度: string resize
#define CZL_SR(gp, str, new_size) \
(void**)CZL_STR_REALLOC(gp, str.Obj, CZL_SL(new_size), CZL_SL(str.size))
///////////////////////////////////////////////////////////////
//字符串引用计数加一: str ref_cnt add 1
#define CZL_SRCA1(str) ++CZL_STR(str.Obj)->rc

//字符串引用计数减一: string ref_cnt dec 1
#define CZL_SRCD1(gp, str) \
{ if (0 == --CZL_STR(str.Obj)->rc) CZL_SF(gp, str); }

//哈希表键字符串引用计数减一: key string ref_cnt dec 1
#define CZL_KEY_SRCD1(gp, s) \
{ if (0 == --CZL_STR(s)->rc) CZL_STR_FREE(gp, s, CZL_SL(CZL_STR(s)->len+1)); }
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
//对象是否被锁: obj is lock
#define CZL_OBJ_IS_LOCK(obj) (CZL_LOCK_ELE == (obj)->quality)

//锁定对象: lock obj
#define CZL_LOCK_OBJ(obj) ((obj)->quality = CZL_LOCK_ELE)

//解锁对象: unlock obj
#define CZL_UNLOCK_OBJ(obj, Quality) ((obj)->quality = Quality)

//解锁对象集合: unlock objs
#define CZL_UNLOCK_OBJS(objs, quality, i, j) \
do { objs[i]->quality = quality[i]; } while (++i < j);
///////////////////////////////////////////////////////////////
#ifdef CZL_MM_RT_GC
    #define CZL_STR(Obj) ((czl_string*)(*Obj))
#else
    #define CZL_STR(Obj) ((czl_string*)Obj)
#endif

#ifdef CZL_MM_RT_GC
    #define CZL_INS(Obj) ((czl_ins*)(*Obj))
#else
    #define CZL_INS(Obj) ((czl_ins*)Obj)
#endif

#ifdef CZL_MM_RT_GC
    #define CZL_TAB(Obj) ((czl_table*)(*Obj))
#else
    #define CZL_TAB(Obj) ((czl_table*)Obj)
#endif

#ifdef CZL_MM_RT_GC
    #define CZL_ARR(Obj) ((czl_array*)(*Obj))
#else
    #define CZL_ARR(Obj) ((czl_array*)Obj)
#endif

#ifdef CZL_MM_RT_GC
    #define CZL_SQ(Obj) ((czl_sq*)(*Obj))
#else
    #define CZL_SQ(Obj) ((czl_sq*)Obj)
#endif

#ifdef CZL_MM_RT_GC
    #define CZL_COR(Obj) ((czl_coroutine*)(*Obj))
#else
    #define CZL_COR(Obj) ((czl_coroutine*)Obj)
#endif

#ifdef CZL_MM_RT_GC
    #define CZL_FIL(Obj) ((czl_file*)(*Obj))
#else
    #define CZL_FIL(Obj) ((czl_file*)Obj)
#endif

#ifdef CZL_MM_RT_GC
    #define CZL_SRC(Obj) ((czl_extsrc*)(*Obj))
#else
    #define CZL_SRC(Obj) ((czl_extsrc*)Obj)
#endif

#define CZL_TAB_LIST(Obj) ((czl_table_list*)(Obj))
#define CZL_ARR_LIST(Obj) ((czl_array_list*)(Obj))
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
  CZL_NEW == node->type))

//检查AST节点是否是变量:
#define CZL_IS_NOT_VAR(node) \
(node->type != CZL_LG_VAR && \
 node->type != CZL_INS_VAR && \
 !czl_is_member_var(node->type, (czl_obj_member*)node->op.obj))
///////////////////////////////////////////////////////////////
//表达式是否是操作数: exp is opr
#define CZL_EIO(exp) (CZL_OPERAND == exp->flag && CZL_OPERAND == exp->lt)

//结果是否为真: res is true
#define CZL_EIT(res) (res)->val.inum
///////////////////////////////////////////////////////////////
//表达式长度: exp length
#define CZL_EL(exp) (exp ? exp->pl.msg.cnt : 0)

//编译表达式: compile exp
#define CZL_CE(exp, buf)  \
if (exp) { \
    memcpy(buf, exp, CZL_EL(exp)*sizeof(czl_exp_ele)); \
    czl_set_order_next(buf, CZL_EL(exp)); \
    buf += CZL_EL(exp); \
}

//设置字节码next: set opcode next
#define CZL_SOCN(last, opcode) { \
    if (last) { \
        if (last->flag != 0XFF) last->next = opcode; \
        else last->pl.pc = opcode; \
    } \
}

//是否为可被结构语句next指针优化的跳转指令: is optimizable jmp
#define CZL_IS_OJ(order) (CZL_LOGIC_JUMP == (order)->flag && !(order)->rt)
///////////////////////////////////////////////////////////////
#define CZL_INT_SWAP(a, b) { a=a^b; b=b^a; a=a^b; }
#define CZL_VAL_SWAP(a, b, t) { t=a; a=b; b=t; }
///////////////////////////////////////////////////////////////
#ifdef CZL_SYSTEM_WINDOWS
    #define CZL_CLOCK clock()
    #define CZL_SLEEP(t) Sleep(t)
#elif defined CZL_SYSTEM_LINUX
    extern long czl_linux_clock(void);
    #define CZL_CLOCK czl_linux_clock()
    #define CZL_SLEEP(t) usleep(t*1000)
#else
    #define CZL_CLOCK (0)
    #define CZL_SLEEP(t) (t)
#endif
///////////////////////////////////////////////////////////////
//运算符
//添加或删除元素后注意修改宏: CZL_UNARY_OPT_NUMBER 和 CZL_BINARY_OPT_NUMBER
typedef enum czl_op_enum
{
    //单目运算符
    CZL_NUMBER_NOT,	// - 必须从0开始映射到czl_opt_cac_funs函数指针数组下标
    CZL_LOGIC_NOT,	// !
    CZL_LOGIC_FLIP,	// ~
    CZL_SELF_ADD,	// i++
    CZL_SELF_DEC,	// i--
    CZL_REF_VAR,	// &
    CZL_OBJ_CNT,    // #
    //双目运算符
    CZL_SWAP,       // ><
    CZL_ASS,		// =
    //
    CZL_ADD_A,		// +=
    CZL_DEC_A,		// -=
    CZL_MUL_A,		// *=
    CZL_DIV_A,		// /=
    CZL_MOD_A,		// %=
    CZL_OR_A,		// |=
    CZL_XOR_A,		// ^=
    CZL_AND_A,		// &=
    CZL_L_SHIFT_A,	// <<=
    CZL_R_SHIFT_A,	// >>=
    //
    CZL_MORE,		// >
    CZL_MORE_EQU,	// >=
    CZL_LESS,		// <
    CZL_LESS_EQU,	// <=
    CZL_EQU_EQU,	// ==
    CZL_NOT_EQU,	// !=
    CZL_EQU_3,      // ===
    CZL_XOR_XOR,    // ^^
    CZL_CMP,        // ??
    CZL_ELE_DEL,    // =>
    CZL_ELE_INX,    // ->
    //后面的带临时结果的双目运算符运行时自动转为 += -= ... 运算符计算
    CZL_ADD,		// +
    CZL_DEC,		// -
    CZL_MUL,		// *
    CZL_DIV,		// /
    CZL_MOD,		// %
    CZL_OR,			// |
    CZL_XOR,		// ^
    CZL_AND,		// &
    CZL_L_SHIFT,	// <<
    CZL_R_SHIFT,	// >>
    //? : 三目运算符没有处理函数所以需要放在最后
    CZL_QUE,        // ?
    CZL_COL,        // :
    //
    CZL_ADD_SELF,	// ++i
    CZL_DEC_SELF,	// --i
    //
    CZL_OBJ_TYPE,   // $
    //
    CZL_CONDITION,  //&&、||条件判断
    //
    CZL_ASS_OPT,            //赋值运算符
    CZL_OPERAND,            //操作数
    CZL_UNARY_OPT,          //单目运算符
    CZL_BINARY_OPT,         //双目运算符
    CZL_UNARY2_OPT,         //产生临时结果的单目运算符
    CZL_BINARY2_OPT,        //产生临时结果的双目运算符
    CZL_THREE_OPT,          //三目运算符
    CZL_THREE_END,          //三目运算符子表达式结束符
    CZL_OR_OR,              // ||
    CZL_AND_AND,            // &&
    //
    CZL_FOREACH_BLOCK,      //foreach语句
    CZL_BLOCK_BEGIN,        //逻辑块开始
    CZL_LOGIC_JUMP,         //跳转指令
    CZL_CASE_SENTENCE,      //case/default语句
    CZL_SWITCH_SENTENCE,    //switch语句
    CZL_RETURN_SENTENCE,    //return;
    CZL_YEILD_SENTENCE,     //yeild;
    CZL_TRY_BLOCK,          //try(exit/break/continue)
    CZL_TASK_BEGIN,         //task
    CZL_TIMER_INIT,         //task计时初始化
    CZL_TIMER_SLEEP,        //timer
} czl_op_enum;

//单目运算符位置性: 左、右
typedef enum czl_unary_opt_pos_enum
{
    CZL_POS_LEFT,
    CZL_POS_RIGHT,
    CZL_POS_LR
} czl_unary_opt_pos_enum;

//双目运算符结合性
typedef enum czl_binary_opt_ass_enum
{
    CZL_LEFT_TO_RIGHT,
    CZL_RIGHT_TO_LEFT
} czl_binary_opt_ass_enum;

//操作数类型
typedef enum czl_opr_type_enum
{
    CZL_INT,            // 32/64bit整形， CZL_INT 必须为 0
    CZL_FLOAT,          // 32/64bit浮点型
    CZL_STRING,         // 任意长度的堆字符串
    CZL_ARRAY,          // 数组
    CZL_TABLE,          // 哈希表
    CZL_INSTANCE,       // 类实例
    CZL_STACK,          // 栈
    CZL_QUEUE,          // 队列
    CZL_FILE,           // 文件
    CZL_SOURCE,         // 资源
    CZL_FUN_REF,        // 函数指针 32bit
    CZL_OBJ_REF,        // 对象指针 32bit
    CZL_ARRAY_LIST,     // 数组[]列表
    CZL_TABLE_LIST,     // 表{}列表
    //
    CZL_LG_VAR,         // 局部变量和全局变量
    CZL_INS_VAR,        // 实例变量
    CZL_MEMBER,         // 类、类实例、数组、表 成员
    CZL_FUNCTION,       // 函数调用
    //
    CZL_NEW,            // 新建实例
    CZL_NIL,            // 清零作用，同时可用于指示变量没有强类型声明
    //
    //以下标记位只起到标记作用
    CZL_USR_FUN,        // 用户自定义函数，用于实现函数递归无层数限制
    CZL_REG_VAR,        // 寄存器变量
    CZL_ENUM,           // 枚举常量
    CZL_NUM,            // 数值类型
    CZL_CHAR,           // 字符类型
    CZL_NULL,           // 空指针
} czl_opr_type_enum;

//圆括号()标志位
typedef enum czl_bracket_flag_enum
{
    CZL_BRACKET_NO,
    CZL_BRACKET_YES
} czl_bracket_flag_enum;

//查找模式
typedef enum czl_find_mode_enum
{
    CZL_LOCAL_FIND,
    CZL_GLOBAL_FIND
} czl_find_mode_enum;

//变量属性
typedef enum czl_var_quality_enum
{
    CZL_DYNAMIC_VAR,    //动态变量，必须是0
    CZL_CONST_VAR,      //常量
    CZL_STATIC_VAR,     //静态变量
    CZL_FUNRET_VAR,     //函数返回值变量，不能等于0
    CZL_ARRBUF_VAR,     //数组连接缓存变量，不能等于0
    CZL_ARRLINK_VAR,    //数组连接开始变量
    CZL_STR_ELE,        //字符串中的字符
    CZL_OBJ_ELE,        //对象成员变量
    CZL_LOCK_ELE,       //被锁定的对象成员变量
    CZL_REF_ELE,        //&运算符结果变量
    CZL_CIRCLE_REF_VAR, //循环引用触发对象写时复制数据变量
} czl_var_quality_enum;

//权限
typedef enum czl_permission_enum
{
    CZL_PUBLIC,
    CZL_PROTECTED,
    CZL_PRIVATE
} czl_permission_enum;

//当前解析变量域、常量域标志
typedef enum czl_var_field_enum
{
    CZL_IN_ANY,
    CZL_IN_CONSTANT
} czl_var_field_enum;

//当前解析域
typedef enum czl_analysis_field_enum
{
    CZL_IN_GLOBAL,
    CZL_IN_CLASS,
    CZL_IN_GLOBAL_FUN,
    CZL_IN_CLASS_FUN
} czl_analysis_field_enum;

//语句类型
typedef enum czl_sentence_type_enum
{
    CZL_FUN_BLOCK,          //func() {}
    CZL_LOOP_BLOCK,         //while/for
    CZL_BRANCH_BLOCK,       //if/switch
    CZL_BRANCH_CHILD_BLOCK, //elif/else/case/default
    CZL_TASK_BLOCK,         //task
    CZL_BREAK_SENTENCE,     //break;
    CZL_CONTINUE_SENTENCE,  //continue;
    CZL_GOTO_SENTENCE,      //goto flag;
    CZL_EXP_SENTENCE,       //1+1;
} czl_sentence_type_enum;

//分支语句类型
typedef enum czl_branch_type_enum
{
    CZL_IF_BRANCH,
    CZL_SWITCH_BRANCH,
    //
    CZL_SWITCH,
    CZL_CASE
} czl_branch_type_enum;

//循环类型
typedef enum czl_loop_type_enum
{
    CZL_WHILE_LOOP,
    CZL_DO_WHILE_LOOP,
    CZL_FOR_LOOP,
    CZL_FOREACH_LOOP,
    CZL_TIMER_LOOP,
} czl_loop_type_enum;

//函数状态
typedef enum czl_fun_state_enum
{
    CZL_IN_STATEMENT,	//声明状态
    CZL_IN_IDLE,        //函数没被调用
    CZL_IN_BUSY,        //函数被第一次调用
    CZL_IN_RECURSION    //函数被递归调用，即大于一次调用
} czl_fun_state_enum;

//函数属性
typedef enum czl_fun_quality_enum
{
    CZL_STATIC_FUN,
    CZL_INS_STATIC_FUN,
    CZL_SRC_STATIC_FUN,
    CZL_LG_VAR_DYNAMIC_FUN,
    CZL_INS_VAR_DYNAMIC_FUN,
} czl_fun_quality_enum;

//函数类型
typedef enum czl_fun_type_enum
{
    CZL_SYS_FUN,            //系统函数
    CZL_USR_GLOBAL_FUN,     //用户自定义全局函数
    CZL_USR_CLASS_FUN,      //用户自定义类成员函数
} czl_fun_type_enum;

//状态确认枚举
typedef enum czl_sure_enum
{
    CZL_SURE,
    CZL_NOT_SURE
} czl_sure_enum;

//类、实例控制类型
typedef enum czl_ctrl_type_enum
{
    CZL_INSTANCE_CTRL,
    CZL_CLASS_CTRL
} czl_ctrl_type_enum;

//索引类型
typedef enum czl_index_type_enum
{
    CZL_ARRAY_INDEX,
    CZL_INSTANCE_INDEX
} czl_index_type_enum;

//try异常触发动作类型
typedef enum czl_try_type_enum
{
    CZL_TRY_EXIT,
    CZL_TRY_BREAK,
    CZL_TRY_CONTINUE,
    CZL_TRY_GOTO
} czl_try_type_enum;

//修改程序退出原因码后需要同步更新 czl_exit_code_table
typedef enum czl_exit_code_enum
{
    CZL_EXIT_ABNORMAL,
    CZL_EXIT_TRY,
    CZL_EXIT_ASSERT,
#ifdef CZL_MULT_THREAD
    CZL_EXIT_KILL,
#endif //#ifdef CZL_MULT_THREAD
} czl_exit_code_enum;

//修改异常码后需要同步更新 CZL_EXCEPTION_CODE_NUM 和 czl_exception_code_table
enum czl_exception_code_enum
{
    CZL_EXCEPTION_NO = 0,
    CZL_EXCEPTION_OBJECT_LOCK = 1,
    CZL_EXCEPTION_OUT_OF_MEMORY = 2,
    CZL_EXCEPTION_STACK_OVERFLOW = 3,
    CZL_EXCEPTION_SYSFUN_RUN_ERROR = 4,
    CZL_EXCEPTION_CLASS_FUNCTION_GRAB = 5,
    CZL_EXCEPTION_YEILD_FUNCTION_GRAB = 6,
    CZL_EXCEPTION_COPY_TYPE_NOT_MATCH = 7,
    CZL_EXCEPTION_ORDER_TYPE_NOT_MATCH = 8,
    CZL_EXCEPTION_OBJECT_TYPE_NOT_MATCH = 9,
    CZL_EXCEPTION_OBJECT_MEMBER_NOT_FIND = 10,
    CZL_EXCEPTION_ARRAY_LENGTH_LESS_ZERO = 11,
    CZL_EXCEPTION_FUNCTION_CALL_NO_FUN_PTR = 12,
    CZL_EXCEPTION_FUNCTION_CALL_NO_INSTANCE = 13,
    CZL_EXCEPTION_FUNCTION_CALL_PARAS_NOT_MATCH = 14,
    CZL_EXCEPTION_DEAD //malloc失败导致线程彻底死亡，无法处理该异常
};
///////////////////////////////////////////////////////////////
//字符串结构: 以'\0'结尾(为了兼容C/C++)，
//通过长度变量确定字符串范围，因此可以存二进制数据
typedef struct czl_string
{
    unsigned long rc;   //引用计数
    unsigned long len;  //字符串实际长度
    char str[4];        //字符串数据地址
} czl_string;

typedef struct czl_str
{
    void **Obj;         //对象
    unsigned long size; //字符串内存分配实际长度，用于字符串 += 运算缓冲
} czl_str;

typedef struct czl_ref
{
    struct czl_var *var;
    long inx;
} czl_ref;

typedef struct czl_ext
{
    union czl_ext_v1
    {
        int i32;
        float f32;
        void *ptr;
    } v1;
    union czl_ext_v2
    {
        int i32;
        unsigned long u32;
        void *ptr;
    } v2;
} czl_ext;

//操作数值结构
typedef union czl_value
{
    czl_long inum;      //整型数值
    czl_float fnum;     //浮点型数值
    czl_str str;        //字符串
    czl_ref ref;        //引用
    czl_ext ext;        //扩展两个4字节变量使用
    struct czl_fun *fun;//函数指针
    void **Obj;         //对象
} czl_value;

//单目运算符
typedef struct czl_unary_operator
{
    char *name;				//操作符字符串名称
    char macro;             //操作符枚举: czl_op_enum
    char position;          //运算符位置性: czl_unary_opt_pos_enum
} czl_unary_operator;

//双目运算符
typedef struct czl_binary_operator
{
    char *name;				//操作符字符串名称
    char macro;         	//操作符枚举: czl_op_enum
    char priority;			//优先级: 见 czl_binary_opt_table
    char associativity;     //结合性: czl_binary_opt_ass_enum
} czl_binary_operator;

//操作数或运算符
typedef union czl_exp_op
{
    //操作数
    void *obj;
    //操作符
    struct czl_exp_opt
    {
        char position;		//单目运算符位置性: czl_unary_opt_pos_enum
        char priority;		//双目运算符优先级: 见 czl_binary_opt_table
        char associativity;	//双目运算符结合性: czl_binary_opt_ass_enum
    } opt;
} czl_exp_op;

//操作数、操作符树节点
typedef struct czl_exp_node
{
    czl_exp_op op;              //操作数或运算符
    unsigned char bracket;      //括号标识符: czl_bracket_flag_enum
    unsigned char flag;         //操作对象标识符: czl_op_enum
    unsigned char type;         //操作数或运算符类型: czl_opr_type_enum/czl_op_enum
    struct czl_exp_node *left;
    struct czl_exp_node *right;
    struct czl_exp_node *parent;
    //
    char *err_file;
    unsigned long err_line;
} czl_exp_node, *czl_exp_tree;

//表达式树解析操作结构
typedef struct czl_exp_handle
{
    czl_exp_node *root;     //指向表达式树根节点
    czl_exp_node *cur;      //指向当前操作符节点
    int bc;                 //用于匹配表达式括号计数
    int mtoc;               //不完整三目运算符计数
} czl_exp_handle;

//操作数、操作符栈元素: czl_opcode
typedef struct czl_exp_ele
{
    union czl_exp_pl
    {
        struct czl_exp_ele *pc; //块内部第一条指令地址
        struct czl_exp_msg
        {
            unsigned short line;//指令行号
            unsigned short cnt; //一条表达式指令个数
        } msg;
    } pl;
    struct czl_exp_ele *next;   //下一条指令地址
    //
    struct czl_var *lo;    //左操作数
    struct czl_var *ro;    //右操作数
    struct czl_var *res;   //临时变量用于存运算符结果
    unsigned char flag;    //元素对象标识符: czl_op_enum
    unsigned char kind;    //操作数或运算符指令类型: czl_opr_type_enum/czl_op_enum
    unsigned char lt;      //左操作数类型: czl_opr_type_enum
    unsigned char rt;      //右操作数类型: czl_opr_type_enum
} czl_exp_ele, *czl_exp_stack;

typedef struct czl_ref_var
{
    struct czl_var *var;
    struct czl_ref_var *next;
    struct czl_ref_var *last;
} czl_ref_var;

typedef struct czl_ref_obj
{
    unsigned long cnt; //必须放在第一个，与 czl_ref_var 里的 var 左区别
    czl_ref_var *head;
    void **objs[1];
} czl_ref_obj;

//全局变量节点，链表结构，解释时用到，
//运行时会将函数局部变量、实例成员变量链表转为 czl_var 数组结构，
//枚举常量、全局变量、静态变量运行时保持 czl_glo_var 链表结构存储。
typedef struct czl_glo_var
{
    char *name;                 //变量名
    unsigned char type;         //变量类型: czl_opr_type_enum
    unsigned char quality;      //变量属性: czl_var_quality_enum
    unsigned char optimizable;  //标记变量是否可优化，注意与czl_loc_var的optimizable内存对其
    unsigned char ot;           //强制类型: czl_opr_type_enum
    czl_value val;              //变量值
    unsigned long hash;         //缓存常量字符串哈希值
    struct czl_glo_var *next;
} czl_glo_var, *czl_glo_var_list;

typedef struct czl_class_var
{
    char *name;                 //变量名
    unsigned char type;         //变量类型: czl_opr_type_enum
    unsigned char quality;      //变量属性: czl_var_quality_enum
    unsigned char permission;   //变量权限: czl_permission_enum
    unsigned char ot;           //强制类型: czl_opr_type_enum
    czl_value val;              //变量值
    struct czl_class_var *next;
    char *Name;
    unsigned long index;//实例变量索引
    unsigned long hash; //实例变量哈希值
} czl_class_var, *czl_class_var_list;

typedef struct czl_char_var
{
    char *name;                 //变量名
    unsigned char type;         //变量类型: czl_opr_type_enum
    unsigned char quality;      //变量属性: czl_var_quality_enum
    unsigned char permission;   //变量权限: czl_permission_enum
    unsigned char ot;           //强制类型: czl_opr_type_enum
    czl_value val;              //变量值
    //
    char *ch;
    struct czl_char_var *next;
} czl_char_var;

//动态变量节点
typedef struct czl_var
{
    char *name;                 //变量名
    unsigned char type;         //变量类型: czl_opr_type_enum
    unsigned char quality;      //变量属性: czl_var_quality_enum
    unsigned char permission;   //变量权限: czl_permission_enum
    unsigned char ot;           //强制类型: czl_opr_type_enum
    czl_value val;              //变量值
} czl_var, *czl_var_arr;

//局部变量的表达式形式节点
typedef struct czl_loc_var
{
    void *var;                  //局部变量地址
    unsigned char flag;         //与struct var结构做区别，这里主要用于与全局变量区别
    unsigned char quality;      //czl_var_quality_enum
    unsigned char optimizable;  //标记变量是否可优化，注意与czl_glo_var的optimizable内存对其
    unsigned char ot;           //类型声明
    czl_value val;
    //静态局部变量初始化参数
    unsigned char type;
    struct czl_loc_var *next;
} czl_loc_var, *czl_loc_var_list;

//枚举节点
typedef struct czl_enum
{
    czl_glo_var_list constants_head;
    czl_glo_var_list constants_tail;
    struct czl_enum *next;
} czl_enum, *czl_enum_list;

//逻辑块
typedef union czl_logic_block
{
    struct czl_fun *fun;
    struct czl_loop *loop;
    struct czl_branch *branch;
    struct czl_branch_child *branch_child;
    struct czl_task *task;
    struct czl_try *Try;
} czl_logic_block;

//代码块
typedef struct czl_code_block
{
    unsigned char type;       //代码块类型
    czl_logic_block block;    //代码块地址
} czl_code_block;

//语句共用体
typedef union czl_sentence_union
{
    struct czl_exp_ele *exp;
    struct czl_loop *loop;
    struct czl_branch *branch;
    struct czl_task *task;
    struct czl_goto *Goto;
    struct czl_try *Try;
} czl_sentence_union;

//语句节点
typedef struct czl_sentence
{
    unsigned long type;			 //语句类型: czl_op_enum
    czl_sentence_union sentence; //语句
    struct czl_sentence *next;
} czl_sentence, *czl_sentence_list;

//全局变量初始化语句节点
typedef struct czl_glo_sentence
{
    unsigned long type;			 //语句类型: czl_op_enum
    czl_sentence_union sentence; //语句
    struct czl_glo_sentence *next;
    char *file;
} czl_glo_sentence, *czl_glo_sentence_list;

//
typedef struct czl_block_jump
{
    czl_exp_ele *last;
    czl_exp_ele *buf;
    struct czl_block_jump *next;
} czl_block_jump;

//
typedef struct czl_stack_block
{
    unsigned char flag;
    unsigned char type;
    unsigned char goto_flag;
    unsigned char case_end;
    void *block;
    czl_exp_ele *next;      //块结束跳转地址
    czl_exp_ele *condition; //块条件地址
} czl_stack_block;

//
typedef struct czl_tmp_block
{
    czl_stack_block block[CZL_MAX_CODE_NEST_LAYER];
    unsigned long i;
    //
    czl_exp_ele *buf;
    //
    czl_exp_ele *last;
    //
    struct czl_try *try_head;
    //
    struct czl_goto *goto_head;
    //
    struct czl_foreach *foreachs; //foreach 数组
    unsigned long l, m;
} czl_tmp_block;

//goto语句节点
typedef struct czl_goto
{
    char *name;
    char *err_file;
    unsigned long err_line;
    czl_exp_ele *pc;
    struct czl_goto *next;
} czl_goto;

//goto标志节点
typedef struct czl_goto_flag
{
    char *name;
    void *block;
    czl_sentence *sentence;
    czl_exp_ele *pc;
    struct czl_goto_flag *next;
} czl_goto_flag, *czl_goto_flag_list;

//表达式参数节点
typedef struct czl_para
{
    czl_exp_ele *para;	 //表达式参数
    struct czl_para *next;
} czl_para, *czl_para_list;

//数据存储器结构
typedef struct czl_store_device
{
    czl_loc_var_list vars;
    czl_enum_list enums;
    struct czl_store_device *next;
} czl_store_device;

//分支语句子句节点
typedef struct czl_branch_child
{
    char type;                          //分支类型: czl_branch_type_enum
    char goto_flag;                     //是否有goto_flag标记位
    czl_para_list conditions;			//条件表达式
    czl_sentence_list sentences_head;   //语句列表头
    czl_sentence_list sentences_tail;   //语句列表尾
    czl_store_device *store_device;     //数据存储器
    struct czl_branch_child *next;      //下一个分支子句
} czl_branch_child;

//分支语句节点
typedef struct czl_branch
{
    char type;                          //分支类型: czl_branch_type_enum
    char goto_flag;                     //是否有goto_flag标记位
    czl_exp_ele *condition;		   	    //条件表达式
    czl_sentence_list sentences_head;	//语句列表头
    czl_sentence_list sentences_tail;	//语句列表尾
    czl_store_device *store_device;     //数据存储器
    czl_branch_child *childs_head;      //分支子句列表头
    czl_branch_child *childs_tail;      //分支子句列表尾
    //
    czl_exp_ele *block_next;            //编译字节码时用到
    czl_exp_ele *block_condition;       //编译字节码时用到
} czl_branch;

//foreach语句节点
typedef struct czl_foreach
{
    czl_exp_ele *a;
    czl_exp_ele *b;
    czl_exp_ele *c;
    czl_exp_ele *d;
    unsigned long cnt;
    unsigned char flag; //0: 默认对象, 1: key, 2: &, 3: 协程; 0: down, 1: up
} czl_foreach;

//循环节点
typedef struct czl_loop
{
    char type;                          //循环类型: czl_loop_type_enum
    char goto_flag;                     //是否有goto_flag标记位
    char foreach_type;                  //foreach类型, 0:object, 1:up/down
    char flag;                          //0: 默认对象, 1: key, 2: &, 3: 协程, 1: up, 0: down
    czl_exp_ele *condition;             //条件表达式
    czl_sentence_list sentences_head;   //语句列表头
    czl_sentence_list sentences_tail;   //语句列表尾
    czl_store_device *store_device;     //数据存储器
    czl_para_list paras_start;          //开始参数列表
    czl_para_list paras_end;            //结束参数列表
    czl_exp_ele *foreach_cnt;           //循环次数
    unsigned long task_cnt;             //仅timer循环有
    //
    czl_exp_ele *block_next;            //编译字节码时用到
    czl_exp_ele *block_condition;       //编译字节码时用到
    czl_exp_ele *tasks;                 //编译字节码时用到
} czl_loop;

//task语句节点
typedef struct czl_task
{
    char goto_flag;                     //是否有goto_flag标记位
    czl_exp_ele *condition;		   	    //条件表达式
    czl_sentence_list sentences_head;	//语句列表头
    czl_sentence_list sentences_tail;	//语句列表尾
    czl_store_device *store_device;     //数据存储器
    //
    czl_exp_ele *block_next;            //编译字节码时用到
    czl_exp_ele *block_condition;       //编译字节码时用到
} czl_task;

//try语句节点
typedef struct czl_try
{
    char type;                          //czl_try_type_enum
    char goto_flag;                     //是否有goto_flag标记位
    czl_para_list paras;                //异常触发执行序列
    czl_sentence_list sentences_head;   //语句列表头
    czl_sentence_list sentences_tail;   //语句列表尾
    czl_store_device *store_device;     //数据存储器
    char *name;                         //异常跳转goto_flag名
    //以下try goto_flag 专用
    char *err_file;
    unsigned long err_line;
    czl_exp_ele *pc;
    struct czl_try *next;
    //
    czl_exp_ele *block_next;            //编译字节码时用到
    czl_exp_ele *block_condition;       //编译字节码时用到
    czl_exp_ele *block_end;             //编译字节码时用到
} czl_try;

//函数入参说明结构
typedef struct czl_para_explain
{
    unsigned short index;        //参数下标
    unsigned char ref_flag;      //引用标志位
    czl_exp_stack para; //默认参数表达式
    struct czl_para_explain *next;
} czl_para_explain;

//函数节点
typedef struct czl_fun
{
    char *name;                             //函数名
    unsigned char state;                    //函数状态: czl_fun_state_enum
    unsigned char type;                     //函数类型: czl_fun_type_enum
    short enter_vars_cnt;                   //函数传入变量个数
    czl_var *vars;                          //函数动态变量
    short dynamic_vars_cnt;                 //动态变量个数
    unsigned short static_vars_cnt;         //静态变量个数
    czl_var_arr *backup_vars;               //备份变量: 动态变量+reg+foreach_index
    unsigned long backup_cnt;               //递归备份次数计数
    unsigned long backup_size;              //备份缓冲区大小
    czl_para_explain *para_explain;         //函数传入变量说明信息
    unsigned long paras_cnt;                //参数个数不确定系统函数实时参数个数
    char (*sys_fun)(void*, void*);          //系统函数指针
    char *file;                             //函数所在的脚本文件名
    struct czl_fun *next;
    czl_var ret;                            //返回值
    //上面64个字节为用户函数和系统函数共有，下面64个字节的为用户函数私有
    unsigned char try_flag;                 //是否有try语句标记位
    unsigned char goto_flag;                //是否有goto_flag标记位
    unsigned char yeild_flag;               //是否有yeild语句标记位
    unsigned char switch_flag;              //是否有switch语句标记位
    unsigned char return_flag;              //是否函数末尾有return语句标记位
    unsigned char permission;               //函数权限: czl_permission_enum
    unsigned short reg_cnt;                 //寄存器个数
    czl_var_arr reg;                        //寄存器
    czl_loc_var_list loc_vars;              //表达式形式入参变量
    czl_store_device *store_device;         //数据存储器
    czl_store_device *store_device_head;    //数据存储器列表头
    czl_goto_flag_list goto_flags;          //goto语句标志列表
    czl_sentence_list sentences_head;       //语句列表头
    struct czl_class_ptr_vars *class_vars;  //类成员my变量入参列表
    void **cur_ins;                         //当前类函数所属的实例
    unsigned long hash;                     //函数名哈希值
    czl_exp_ele *pc;                        //yeild语句的下一条pc地址
    czl_foreach *foreachs;                  //foreach语句指针数组
    unsigned short foreach_cnt;             //foreach object语句个数
    unsigned short foreach_sum;             //foreach语句个数
    czl_exp_ele *opcode;                    //字节码: 指令 + 临时变量 + 局部变量
    unsigned long opcode_cnt;               //字节码大小
} czl_fun, *czl_fun_list;

//文件对象结构
typedef struct czl_file
{
    unsigned long rc;   //引用计数
    FILE *fp;           //文件指针
    unsigned char mode; //1:字节流 2:行读写 3:结构化
    unsigned char sign; //行写间隔符号
#ifdef CZL_SYSTEM_WINDOWS
    unsigned char txt;  //是否是文本读写方式
#endif
    long addr;          //文件偏移地址
} czl_file;

//缓存文件结构
typedef struct czl_buf_file
{
    char *path;
    FILE *fp;
    long date;
    unsigned long time;
    void **buf;
    struct czl_buf_file *next;
    struct czl_buf_file *last;
} czl_buf_file;

//哈希表桶链节点
typedef struct czl_bucket
{
    void *key;
    struct czl_bucket *next;
} czl_bucket;

//哈希表结构
typedef struct czl_sys_hash
{
    long mask;                  //-size
    unsigned long size;         //bucket个数: 2的幂
    unsigned long count;        //总数据节点个数: 与size比较进行扩容和回收
    czl_bucket **buckets;       //bucket数组
} czl_sys_hash;

//扩展库资源结构
typedef struct czl_extsrc
{
    unsigned long rc;   //引用计数
    void *src;
    void (*src_free)(void*);
    char *name;
    czl_sys_hash *hash;
    unsigned char type;
    unsigned char engine;
    unsigned char state;
#ifdef CZL_SYSTEM_WINDOWS
    unsigned char stdcall_flag;
#endif
} czl_extsrc;

//父类节点
typedef struct czl_class_parent
{
    unsigned char permission; //继承方式: czl_permission_enum
    struct czl_class *pclass;
    struct czl_class_parent *next;
} czl_class_parent, *czl_class_parent_list;

//类节点
typedef struct czl_class
{
    char *name;                         //类名
    unsigned char flag;                 //标记类是否定义结束
    unsigned char final_flag;           //终节点类标志位
    unsigned char ot_num;               //用于强类型声明
    unsigned short parents_count;       //父类继承个数
    unsigned short vars_count;          //类成员动态变量个数
    czl_class_var_list vars;            //类成员变量列表头
    czl_enum_list enums;                //类成员枚举列表头
    czl_fun_list funs;                  //类成员函数列表头
    czl_class_parent_list parents;      //父类列表头
    struct czl_class *next;
    czl_sys_hash vars_hash;             //类成员变量索引
    czl_sys_hash funs_hash;             //类成员函数索引
#ifdef CZL_MM_MODULE
    czl_mm_sp_pool pool;                //本类专用内存池
#endif
} czl_class, *czl_class_list;

//类实例节点: 继承树结构
typedef struct czl_ins
{
    unsigned long rc;               //引用计数
    unsigned long rf;               //引用标记
    //
    czl_class *pclass;              //实例所属的类指针
    void **parents[1];              //父类数组，其后紧跟czl_var数组内存
} czl_ins;

//实例成员变量的表达式形式节点
typedef struct czl_ins_var
{
    czl_var *var;       //实例成员变量地址
    unsigned long index;//实例变量索引
    struct czl_ins_var *next;
} czl_ins_var, *czl_ins_var_list;

//类成员函数my成员变量变量节点
typedef struct czl_class_ptr_vars
{
    czl_class *pclass;           //变量所属的类指针
    czl_ins_var_list vars;       //实例成员变量列表
    struct czl_class_ptr_vars *next;    //父类
} czl_class_ptr_vars;

typedef struct czl_instance_index
{
    //注意： 当类名有哈希冲突且哈希值相等时，索引会出错，但这种概率几乎是0，不考虑
    unsigned long hash;             //缓存哈希值
    struct czl_exp_fun *exp_fun;    //成员函数
    char *class_name;               //指定类前缀名
} czl_instance_index;

typedef struct czl_array_index
{
    czl_exp_ele *exp;               //表达式
    struct czl_exp_fun *exp_fun;    //函数
} czl_array_index;

//成员索引共用体
typedef union czl_member_index_union
{
    czl_array_index arr;    //数组索引
    czl_instance_index ins; //实例索引
} czl_member_index_union;

//类实例、数组、表、字符串 成员索引节点
typedef struct czl_member_index
{
    unsigned char type;            //索引类型: czl_index_type_enum
    czl_member_index_union index;  //索引
    struct czl_member_index *next; //下一级索引
} czl_member_index;

//类、类实例、数组、表 成员节点
typedef struct czl_obj_member
{
    unsigned char type;         //原始对象类型: czl_opr_type_enum
    //flag说明: CZL_ASS: 成员赋值， CZL_SWAP: 成员交换， CZL_REF_VAR: 取成员引用
    unsigned char flag;
    czl_var *obj;               //原始对象值: 实例、数组、表、字符串
    czl_member_index *index;    //成员索引
    struct czl_obj_member *next;
} czl_obj_member;

//数组列表节点
typedef struct czl_array_list
{
    unsigned char quality; //czl_var_quality_enum
    unsigned long paras_count;
    czl_para_list paras;
} czl_array_list;

//元素节点: 存 CZL_FUN_REF/CZL_NEW/CZL_NIL/CZL_ARRAY_LIST/CZL_TABLE_LIST
typedef struct czl_ele
{
    struct czl_ele *next;
    unsigned char type;     //元素类型: czl_opr_type_enum
    unsigned char quality;  //变量属性: czl_var_quality_enum
    char null;              //仅用于内存对其
    unsigned char ot;       //强制类型: czl_opr_type_enum
    czl_value val;          //元素值
} czl_ele;

//数组节点
typedef struct czl_array
{
    unsigned long rc;           //引用计数
    unsigned long rf;           //引用标记
    //
    czl_var *vars;              //元素集合
    unsigned long sum;          //数组实际元素个数
    unsigned long cnt;          //实际使用元素个数
} czl_array;

//栈、队列节点
typedef struct czl_sq
{
    unsigned long rc;           //引用计数
    unsigned long rf;           //引用标记
    //
    czl_glo_var *eles_head;     //元素集合头
    czl_glo_var *eles_tail;     //元素集合尾
    czl_glo_var *foreach_inx;   //记录foreach语句索引，方便gc
    unsigned long count;        //元素个数
#ifdef CZL_MM_MODULE
    czl_mm_sp_pool pool;        //本sq专用内存池
#endif
} czl_sq;

//键值对节点
typedef struct czl_table_node
{
    czl_exp_stack key;
    czl_exp_stack val;
    struct czl_table_node *next;
} czl_table_node;

//字典列表
typedef struct czl_table_list
{
    unsigned char quality; //czl_var_quality_enum
    unsigned long paras_count;
    czl_table_node *paras;
} czl_table_list;

//表键值节点
typedef struct czl_tabkv
{
    char *name;
    unsigned char type;     //值类型: czl_opr_type_enum
    unsigned char quality;  //变量属性: czl_var_quality_enum
    unsigned char kt;       //键类型: 只能是整形或字符串类型
    unsigned char ot;       //强制类型: czl_opr_type_enum
    czl_value val;          //值
    czl_value key;          //键
    struct czl_tabkv *next;
    struct czl_tabkv *last;
    struct czl_tabkv *clsNext;
    struct czl_tabkv *clsLast;
} czl_tabkv;

//哈希表结构
typedef struct czl_table
{
    unsigned long rc;           //引用计数
    unsigned long rf;           //引用标记
    //
    czl_tabkv **buckets;        //bucket数组
    long mask;                  //-size, 注意不能为0
    unsigned long size;         //bucket个数: 2的幂
    unsigned long count;        //总数据节点个数: 与size比较进行扩容和回收
    unsigned long key;          //防止哈希碰撞攻击密钥
    unsigned long attack_cnt;   //哈希碰撞攻击次数计数
    czl_tabkv *eles_head;       //元素链表头
    czl_tabkv *eles_tail;       //元素链表尾
    czl_tabkv *foreach_inx;     //记录foreach语句索引，方便gc
#ifdef CZL_MM_MODULE
    czl_mm_sp_pool pool;        //本表专用内存池
#endif
} czl_table;

//新建类实例语句节点
typedef struct czl_new_ins
{
    czl_class *pclass;             //实例所属类
    czl_exp_ele *len;              //数组长度表达式
    struct czl_exp_fun *init_fun;  //构造函数
} czl_new_ins;

//新建数组语句节点
typedef struct czl_new_array
{
    czl_exp_ele *len;           //数组长度表达式
    czl_array_list *init_list;  //初始化数组列表
} czl_new_array;

//新建对象节点
typedef union czl_new_obj_union
{
    czl_new_ins ins;        //新建实例
    czl_new_array arr;      //新建数组
    czl_table_list *tab;    //新建表
} czl_new_obj_union;

//new语句节点
typedef struct czl_new_sentence
{
    czl_new_obj_union new_obj;  //新建对象
    char type;                  //新建对象类型: czl_opr_type_enum
} czl_new_sentence;

//表达式中函数调用节点
typedef struct czl_exp_fun
{
    czl_fun *fun;                 //函数指针
    czl_para_list paras;          //参数列表
    short paras_count;            //参数个数
    unsigned char quality;        //函数属性: czl_fun_quality_enum
    unsigned char flag;           //1: 无空参数； 0: 有空参数
    struct czl_exp_fun *next;
} czl_exp_fun;

//not sure fun or exp_fun
typedef struct czl_nsef
{
    unsigned char flag;  //1: fun(a,b) 0: @fun
    unsigned short err_line;
    char *err_file;
    czl_exp_fun *ef;
    struct czl_nsef *next;
} czl_nsef;

//脚本文件名链表节点结构
typedef struct czl_name
{
    char *name;
    struct czl_name *next;
} czl_name;

//别名链表节点结构
typedef struct czl_as
{
    char *new_name;
    char *old_name;
    struct czl_as *next;
} czl_as;

//系统关键字索引结构
typedef struct czl_keyword
{
    char *name;
    int index;
} czl_keyword;

//局部变量对象缓存
typedef struct czl_obj_cache
{
    void **obj;
    struct czl_obj_cache *next;
} czl_obj_cache;

//用户函数调用栈
typedef struct czl_usrfun_stack
{
    czl_fun *fun;
    czl_exp_ele *pc;
} czl_usrfun_stack;

typedef struct czl_block_struct
{
    //if elif else switch for while do {
    char index;	//关键字索引
    char flag;	//块确定标志位
} czl_block_struct;

//系统函数声明结构
typedef struct czl_sys_fun
{
    char *name;             //函数名
    //系统函数指针: char (*sys_fun)(czl_gp*, czl_fun*);
    void *sys_fun;
    short enter_vars_cnt;   //入参个数
    char *paras_explain;    //入参声明
} czl_sys_fun;

//系统函数表结构
typedef struct czl_sys_lib
{
    char *name;
    const czl_sys_fun *funs;
    unsigned long num;
} czl_sys_lib;

typedef struct czl_sysfun
{
    char *name;
    const czl_sys_fun *sysfun;
    czl_fun *fun;
    unsigned long hash;
} czl_sysfun;

typedef struct czl_syslib
{
    char *name;
    const czl_sys_lib *lib;
    czl_sysfun *funs;
    czl_sys_hash hash;
} czl_syslib;

typedef struct czl_usrlib
{
    char *name;
    struct czl_usrlib *next;
    czl_nsef *nsef_head;
    czl_nsef *nsef_tail;
    czl_sys_hash vars_hash;
    czl_sys_hash funs_hash;
    czl_sys_hash as_hash;       //别名哈希索引
    czl_as *as_head;            //别名链表头
} czl_usrlib;

//协程节点结构
typedef struct czl_coroutine
{
    unsigned char type; //0: 临时, 1: 永久
    czl_fun *fun;
    czl_var *vars;
    czl_exp_ele *pc;
    void **next;
    void **last;
} czl_coroutine;

#ifdef CZL_MULT_THREAD
typedef struct czl_pipe_buf
{
    struct czl_pipe_buf *next;
    char buf[1];
} czl_pipe_buf;

typedef struct czl_thread_pipe
{
#ifdef CZL_SYSTEM_WINDOWS
    CRITICAL_SECTION report_lock;
    CRITICAL_SECTION notify_lock;
    HANDLE report_event;
    HANDLE notify_event;
#elif defined CZL_SYSTEM_LINUX
    pthread_mutex_t report_lock;
    pthread_mutex_t notify_lock;
    sem_t report_event;
    sem_t notify_event;
#endif
    //
    char *para;
    //
    czl_pipe_buf *rb_head;
    czl_pipe_buf *rb_tail;
    unsigned long rb_cnt;
    //
    czl_pipe_buf *nb_head;
    czl_pipe_buf *nb_tail;
    unsigned long nb_cnt;
    //线程同步标志位必须加volatile声明，否则会被编译器优化无法检测到真实的状态
    volatile unsigned char alive;
    volatile unsigned char kill;
    volatile unsigned char suspend;
} czl_thread_pipe;

//线程节点结构
typedef struct czl_thread
{
#ifdef CZL_SYSTEM_WINDOWS
    HANDLE id;
#elif defined CZL_SYSTEM_LINUX
    pthread_t id;
#endif
    czl_thread_pipe *pipe;
    struct czl_thread *next;
    struct czl_thread *last;
} czl_thread;
#endif //#ifdef CZL_MULT_THREAD

#ifdef CZL_TIMER
typedef struct czl_timer
{
#ifdef CZL_SYSTEM_WINDOWS
    unsigned long id;
    WINAPI int (*timer_delete)(unsigned long);
#else //CZL_SYSTEM_LINUX
    timer_t id;
    int (*timer_delete)(timer_t);
#endif
    unsigned long period;
    struct czl_gp *gp;
    czl_fun *cb_fun;
    unsigned char state;
    struct czl_timer *next;
    struct czl_timer *last;
} czl_timer;
#endif //#ifdef CZL_TIMER

//热更新数据
typedef struct czl_hot_update_datas
{
    czl_name *sn_head;
    czl_exp_fun *expfun_head;
    czl_obj_member *member_head;
    czl_ele *eles_head;
    czl_glo_var_list vars_head;
    czl_enum_list enums_head;
    czl_fun_list funs_head;
    czl_class_list class_head;
    czl_sys_hash class_hash;
} czl_hot_update_datas;

//热更新数据结构
typedef struct czl_hot_update
{
    char *path;
    FILE *fp;
    long date;
    unsigned long time;
    czl_fun *main_fun;
    unsigned long main_err_line;
    struct czl_hot_update *next;
    struct czl_hot_update *last;
    czl_hot_update_datas huds;
} czl_hot_update;

//解释时全局参数结构
typedef struct czl_analysis_gp
{
    czl_sys_hash *osfun_hash;   //os系统库函数哈希索引
    czl_sys_hash usrlibs_hash;  //用户库哈希索引
    //
    czl_block_struct block_stack[CZL_MAX_CODE_NEST_LAYER];
    unsigned long block_count;
    //
    //表达式树节点内存供应池
    czl_exp_node exp_node_pool[CZL_EXP_NODE_MAX_SIZE];
    unsigned long exp_node_cnt;
    //
    //存放一个函数内的嵌套代码块地址
    czl_code_block code_blocks[CZL_MAX_CODE_NEST_LAYER];
    unsigned long code_blocks_count;
    //
    czl_exp_ele *exp_head;      //exp_root对应的表达式栈首元素
    //
    czl_sys_hash sn_hash;       //脚本文件名哈希索引
    //
    czl_usrlib *glo_lib;        //全局域
    czl_usrlib *cur_usrlib;     //当前用户库
    czl_usrlib *usrlib_head;    //用户库链表头节点
    //
    czl_glo_var_list global_vars_tail;          //指向全局变量尾节点
    //
    czl_sentence_list sentences_tail;           //当前函数语句列表尾
    czl_glo_sentence_list glo_vars_init_tail;   //全局变量初始化表达式链表尾
    //
    czl_class_var_list class_vars_tail;         //指向类变量链表尾节点
    czl_class *cur_class;                       //当前类
    unsigned char permission;					//当前变量、函数访问权限
    unsigned char class_ot_num;                 //类强类型声明编号
    //
    czl_class_ptr_vars *class_ptr_vars_head;     //引用变量头
    czl_class_ptr_vars *class_ptr_vars_tail;     //引用变量尾
    czl_class_var *cur_class_var;                //当前类函数中的成员变量
    czl_class *cur_var_class;                    //czl_cur_class_var所属的类
    czl_ins_var *ins_vars_tail;                  //引用实例变量尾
    //
    unsigned char variable_field;           //当前解析的是变量还是常量
    unsigned char analysis_field;			//当前解析域在全局还是函数内
    //
    unsigned char cur_loop_type;    	//当前解释循环语句块类型: for/while
    unsigned char foreach_type;         //foreach类型
    czl_para_list for_paras_start;		//for循环参数开始列表
    czl_para_list for_paras_end;		//for循环参数结尾列表
    czl_exp_stack for_condition;		//for循环循环条件表达式
    czl_exp_stack for_cnt;              //for循环次数表达式
    //
    czl_try_type_enum try_type; //try语句异常触发行为处理类型
    czl_para_list try_paras;    //try语句异常触发执行序列
    //
    czl_branch_type_enum cur_branch_type;   //当前解释分支语句块类型: if/switch
    czl_para_list branch_child_paras;       //elif条件参数或switch语句连续case参数列表
    //
    char *goto_flag_name;   //goto语句标志名称
    //
    int colon_flag;  //用于:运算符在?:和{key:val}和case xxx:的冲突处理
    //
    unsigned char reg_flag;
    unsigned char reg_sign;
    //
    unsigned char condition_flag; //解决 while {} 条件可为空问题
    //
    czl_var *int_reg;
    czl_var *float_reg;
    unsigned char int_reg_cnt;
    unsigned char float_reg_cnt;
} czl_analysis_gp;

//全局参数结构
typedef struct czl_gp
{
    czl_analysis_gp *tmp; //解释时全局参数结构
#ifdef CZL_MM_MODULE
    czl_mm_sp_pool mmp_obj;         //对象住址内存池
    czl_mm_sp_pool mmp_tab;         //table结构内存池
    czl_mm_sp_pool mmp_arr;         //array结构内存池
    czl_mm_sp_pool mmp_sq;          //sq结构内存池
    czl_mm_sp_pool mmp_ref;         //ref_var结构内存池
    czl_mm_sp_pool mmp_cor;         //coroutine结构内存池
    czl_mm_sp_pool mmp_file;        //file结构内存池
    czl_mm_sp_pool mmp_buf_file;    //buf_file结构内存池
    czl_mm_sp_pool mmp_hot_update;  //hot_update结构内存池
    czl_mm_sp_pool mmp_extsrc;      //extsrc结构内存池
#ifdef CZL_MULT_THREAD
    czl_mm_sp_pool mmp_thread;      //thread结构内存池
#endif //#ifdef CZL_MULT_THREAD
#ifdef CZL_TIMER
    czl_mm_sp_pool mmp_timer;       //timer结构内存池
#endif //#ifdef CZL_TIMER
    //内存池: 运行时全局函数、类、变量、常量、代码等，静态
    czl_mm_sp_pool mmp_rt[CZL_MM_SP_RANGE];
    //内存池: 解释时临时AST内存、各个哈希表内存、全局语句，动态
    czl_mm_sp_pool mmp_tmp[CZL_MM_SP_RANGE];
    //内存池: 运行时调用栈，函数调用、字符串元素缓存，动态
    czl_mm_sp_pool mmp_stack[CZL_MM_SP_RANGE];
    //内存池: 字符串，动态
    czl_mm_sp_pool mmp_str[CZL_MM_SP_RANGE];
#ifdef CZL_MM_CACHE
    czl_ulong mm_cache_size;        //cache缓存大小
    czl_mm_cache_handle mm_cache;   //cache
#endif //#ifdef CZL_MM_CACHE
    czl_mm_sys_heap *mmp_sh_head;   //操作系统管理的堆链表头
    czl_ulong mmp_gc_size;          //内存分配失败时执行批量回收垃圾大小
    unsigned char mmp_rank;         //内存池缓存等级: 0~CZL_MM_SP_HEAP_NUM_MAX
    unsigned char mmp_selfAdapt;    //内存池缓存等级自适应开关
#endif //#ifdef CZL_MM_MODULE
    //
    czl_ulong mm_limit_backup;
    czl_ulong mm_limit; //允许的总内存大小
    czl_ulong mm_cnt;   //当前内存总字节数
    czl_ulong mm_max;   //内存使用峰值统计
    //
#ifdef CZL_MULT_THREAD
    czl_sys_hash threads_hash;
    czl_thread *threads_head;
    czl_thread_pipe *thread_pipe;
    czl_fun *killFun; //子线程被kill回调函数
#endif //#ifdef CZL_MULT_THREAD
#ifdef CZL_TIMER
    czl_sys_hash timers_hash;
    czl_timer *timers_head;
    czl_timer *timer_next;
    unsigned long timerEventCnt;
    #ifdef CZL_SYSTEM_WINDOWS
        CRITICAL_SECTION timer_cs;
    #elif defined CZL_SYSTEM_LINUX
        pthread_mutex_t timer_mutex;
    #endif
#endif //#ifdef CZL_TIMER
    //
    czl_sys_hash coroutines_hash;
    void **coroutines_head;
    //
    czl_sys_hash syslibs_hash; //系统库哈希索表
    czl_syslib *syslibs; //系统库
    //
    czl_sys_hash file_hash; //缓存文件哈希表
    czl_buf_file *file_head; //缓存文件链表头
    //
    czl_sys_hash hot_update_hash; //热更新哈希表
    czl_hot_update *hot_update_head; //热更新链表头
    //
    czl_sys_hash consts_hash; //常量哈希表
    czl_glo_var_list consts_head;
    //
    czl_sys_hash keyword_hash;  //系统关键字哈希索引
    //
#ifndef CZL_CONSOLE
    void **table; //与C/C++数据交互表
    char *shell_path;
    char *log_path;
    char *class_name;
    czl_class *pclass;
    czl_fun *main_fun;
#endif //#ifndef CZL_CONSOLE
    //
#ifdef CZL_SYSTEM_LINUX
    int kdpfd;  //用于tcp/udp库的epoll_create
#endif //#ifdef CZL_SYSTEM_LINUX
    //
    czl_char_var *ch_head;      //字符串元素缓冲区链表头
    //
    unsigned long fun_deep;     //栈空间上函数调用深度
    //
    czl_hot_update_datas huds; //热更新数据
    //
    czl_glo_sentence_list glo_vars_init_head;
    //
    czl_fun *cur_fun;   //当前函数
    //
    unsigned long runtime; //系统执行时间
    //
    czl_var exp_reg[CZL_MAX_REG_CNT]; //全局表达式计算寄存器数组
    unsigned long reg_cnt; //寄存器使用计数
    //
    //全局临时变量，循环引用fork结果、数组元素&结果
    czl_var tmp_var;
    //
    czl_var enter_var; //用于main函数传参
    //
    //全局临时变量，用于对象类型成员赋值时循环引用检测 和 资源函数对象传参
    czl_var *cur_var;
    //
    void **cur_ins;             //当前实例指针
    //
    unsigned long error_line;   //脚本错误行号
    char *error_file;           //脚本错误文件名
    //
    unsigned long main_err_line;//main函数错误行号
    //
    char error_flag;            //运行时错误标记
    //
    char exit_flag;             //脚本中断退出标志位
    char exit_code;             //脚本中断退出方式码: czl_exit_code_enum
    //
    char yeild_end;             //用于foreach协程标记其是否结束
    czl_exp_ele *yeild_pc;      //用于栈空间上函数yeild返回PC地址
    //
    czl_exp_ele *next_pc;       //用于字符串连接判断是否是最后一个+指令
    //
    czl_var *fun_ret;           //函数返回值
    //
    char log_buf[CZL_LOG_BUF_SIZE]; //log缓冲区
    char exceptionCode;             //运行时异常码: czl_exception_code_enum
    czl_fun *exceptionFuns[CZL_EXCEPTION_CODE_NUM]; //运行时异常回调函数
    //
    czl_exp_fun ef0, ef1, ef2; //运行时构造函数
    czl_para efp1, efp2;
    czl_exp_ele efe1, efe2;
    //
    czl_exp_fun tcp_ef; //运行时tcp事件回调构造函数
    czl_para tcp_efp1, tcp_efp2;
    czl_exp_ele tcp_efe1, tcp_efe2;
    czl_var tcp_v1, tcp_v2;
} czl_gp;
///////////////////////////////////////////////////////////////
//单目运算符列表
extern const czl_unary_operator czl_unary_opt_table[];
extern const unsigned long czl_unary_opt_table_num;
//双目运算符列表
extern const czl_binary_operator czl_binary_opt_table[];
extern const unsigned long czl_binary_opt_table_num;
///////////////////////////////////////////////////////////////
char czl_as_save(czl_gp*, char*, char*);
char czl_shell_name_save(czl_gp*, char*, char);
czl_name* czl_shell_name_find(czl_gp*, char*);
char czl_fun_paras_check(czl_gp*, czl_exp_fun*, const czl_fun*);
char czl_exp_tree_to_stack(czl_gp*, czl_exp_node*);
char czl_exp_integrity_check(czl_gp*, czl_exp_handle*);
char czl_exp_is_integrity(czl_gp*, czl_exp_handle*, char);
czl_exp_node* czl_opr_node_create(czl_gp*, char, void*);
czl_var* czl_const_create(czl_gp*, char, const czl_value*);
czl_exp_node* czl_unary_opt_node_create(czl_gp*, char, char);
czl_exp_node* czl_binary_opt_node_create(czl_gp*, char, char, char);
char czl_exp_node_insert(czl_gp*, czl_exp_node*, czl_exp_handle*);
void czl_exp_stack_delete(czl_gp*, czl_exp_ele*);
char czl_const_exp_init(czl_gp*, czl_var*, char);
czl_enum* czl_enum_node_create(czl_gp*);
void czl_enum_insert(czl_gp*, czl_enum*);
czl_var* czl_var_create_by_field(czl_gp*, char*, char, char);
void* czl_loc_var_find(const char*, czl_loc_var*);
czl_var* czl_var_in_class_find(const char*, char);
czl_var* czl_var_find(czl_gp*, char*, char);
czl_var* czl_var_find_in_exp(czl_gp*, char*, char);
char czl_file_delete(czl_gp*, void**);
czl_class* czl_class_create(czl_gp*, char*, char);
czl_class* czl_class_find_in_local(czl_gp*, char*);
czl_class* czl_class_find(czl_gp*, char*, char);
char czl_class_insert(czl_gp*, czl_class*);
czl_class_parent* czl_class_parent_node_create(czl_gp*, czl_class*, char);
void czl_class_parent_node_insert(czl_class_parent**,
                                  czl_class_parent**,
                                  czl_class_parent*);
char czl_instance_delete(czl_gp*, void**);
void** czl_instance_fork(czl_gp*, czl_class*, char);
czl_array* czl_array_fork(czl_gp*, czl_var*);
czl_table* czl_table_fork(czl_gp*, czl_var*);
czl_sq* czl_sq_fork(czl_gp*, czl_var*);
czl_string* czl_string_fork(czl_gp*, czl_var*);
czl_file* czl_file_fork(czl_gp*, czl_var*);
czl_glo_var* czl_sqele_node_create
(
    czl_gp*
#ifdef CZL_MM_MODULE
    , czl_mm_sp_pool*
#endif
);
void czl_array_list_delete(czl_gp*, czl_array_list*);
czl_array_list* czl_array_list_create(czl_gp*);
char czl_array_vars_new_by_list(czl_gp*, czl_var*, const czl_para*);
char czl_array_vars_new_by_array(czl_gp*, czl_var*, czl_var*, int);
void** czl_array_create(czl_gp*, unsigned long, unsigned long);
char czl_array_resize(czl_gp*, czl_array*, unsigned long);
char czl_array_delete(czl_gp*, void**);
char czl_str_create(czl_gp*, czl_str*, unsigned long, unsigned long, const void*);
void** czl_sq_new(czl_gp*, czl_exp_stack len, const czl_array_list*);
char czl_array_new(czl_gp*, czl_exp_stack len, const czl_array_list*, czl_var*);

void** czl_sq_create(czl_gp*, unsigned long);
char czl_sq_delete(czl_gp*, void**);
void** czl_sq_new_by_sq(czl_gp*, const czl_sq*);
czl_new_sentence* czl_new_sentence_create(czl_gp*, char);
void czl_new_sentence_delete(czl_gp*, czl_new_sentence*);
char czl_obj_new(czl_gp*, czl_new_sentence*, czl_var*);
char czl_obj_fork(czl_gp*, czl_var *left, czl_var *right);
char czl_my_vars_class_sort(czl_gp*, czl_fun*);
czl_var* czl_class_ref_var_node_create(czl_gp*);
char czl_is_member_var(char, const czl_obj_member*);
czl_obj_member* czl_obj_member_node_create(czl_gp*, char, void*);
czl_member_index* czl_ins_index_create(czl_gp*, char*, char*, czl_exp_fun*);
czl_member_index* czl_arr_index_create(czl_gp*, czl_exp_ele*, czl_exp_fun*);
void czl_member_index_node_insert(czl_member_index**,
                                  czl_member_index**,
                                  czl_member_index*);
void czl_obj_member_delete(czl_obj_member*);
czl_class_var* czl_class_var_find(char*, const czl_class*);
czl_fun* czl_member_fun_find(unsigned long, const czl_class*);
czl_class* czl_if_class_parent_repeat(const char*, czl_class*);
char czl_sys_lib_hash_create(czl_gp*);
char czl_sys_keyword_hash_create(czl_gp*, const czl_keyword*, unsigned long);
czl_sys_hash* czl_sys_fun_hash_create(czl_gp*, char*);
czl_sysfun* czl_sys_fun_find(char*, czl_sys_hash*);
czl_fun* czl_sys_fun_create(czl_gp*, czl_sysfun*);
czl_fun* czl_fun_node_create(czl_gp*, char*, char, char, char, void*);
void czl_fun_node_insert(czl_fun**, czl_fun*);
char czl_fun_insert(czl_gp*, czl_fun*);
czl_fun* czl_fun_node_find(char*, const czl_sys_hash*);
void* czl_sys_hash_find(char, char, void*, const czl_sys_hash*);
char czl_sys_hash_insert(czl_gp*, char, void*, czl_sys_hash*);
void* czl_sys_hash_delete(czl_gp*, char, void*, czl_sys_hash*);
czl_fun* czl_fun_find(czl_gp*, char*, char);
czl_fun* czl_fun_find_in_exp(czl_gp*, char*, char);
char czl_integrity_check(czl_gp*, char);
char czl_nsef_create(czl_gp*, void*, char);
czl_exp_fun* czl_exp_fun_create(czl_gp*, czl_fun*, char);
czl_para* czl_para_node_create(czl_gp*, char);
void czl_para_node_insert(czl_para**, czl_para**, czl_para*);
czl_para_explain* czl_para_explain_create(czl_gp*, unsigned short,
                                          unsigned char, czl_exp_ele*);
void czl_para_explain_insert(czl_para_explain**,
                             czl_para_explain**,
                             czl_para_explain*);
void czl_hash_table_delete(czl_gp*, czl_sys_hash*);
czl_table_list* czl_table_list_create(czl_gp*);
czl_table_node* czl_table_node_create(czl_gp*, czl_exp_stack, czl_exp_stack);
void czl_table_node_insert(czl_table_node**,
                           czl_table_node**,
                           czl_table_node*);
void czl_table_list_delete(czl_gp*, czl_table_list*);
char czl_hash_init(czl_gp*, czl_table*, unsigned long);
void** czl_table_create(czl_gp*, unsigned long, unsigned long, unsigned long);
char czl_delete_tabkv(czl_gp*, czl_var*, czl_var*);
char czl_delete_num_tabkv(czl_gp*, czl_var*, czl_long);
czl_tabkv* czl_create_tabkv(czl_gp*, czl_table*, czl_var*, czl_tabkv*, unsigned char);
czl_tabkv* czl_create_key_num_tabkv(czl_gp*, czl_table*, czl_long, czl_tabkv*, unsigned char);
czl_tabkv* czl_create_key_str_tabkv(czl_gp*, czl_table*, czl_var*, czl_tabkv*, unsigned char);
czl_tabkv* czl_find_tabkv(czl_table*, czl_var*);
czl_tabkv* czl_find_num_tabkv(czl_table*, czl_long);
char czl_table_delete(czl_gp*, void**);
czl_tabkv* czl_create_str_tabkv(czl_gp*, czl_table*, unsigned long, long, char*);
czl_tabkv* czl_create_num_tabkv(czl_gp*, czl_table*, czl_long);
char czl_table_new(czl_gp*, const czl_table_list*, czl_var*);
void** czl_table_new_by_table(czl_gp*, czl_table*);
char czl_table_union_by_table(czl_gp*, czl_table*, czl_tabkv*);
char czl_table_union_by_list(czl_gp*, czl_table*, czl_table_node*);
void czl_table_mix_by_table(czl_gp*, czl_var*, czl_tabkv*);
void czl_table_mix_by_array(czl_gp*, czl_var*, czl_array*);
void czl_table_mix_by_sq(czl_gp*, czl_var*, czl_glo_var*);
char czl_table_mix_by_list(czl_gp*, czl_var*, czl_para*);
czl_goto_flag* czl_goto_node_create(czl_gp*, const char*);
void czl_goto_node_insert(czl_goto_flag**, czl_goto_flag*);
char czl_sentence_block_create(czl_gp*, char);
void czl_else_block_to_elif(czl_gp*);
void czl_res_check(czl_var*);
czl_var* czl_exp_cac(czl_gp*, czl_exp_stack);
char czl_store_device_check(czl_gp*);
czl_loc_var* czl_loc_var_create(czl_gp*, czl_loc_var**, czl_loc_var**,
                                char*, char, char, char);
void czl_save_fun_enum_list(czl_gp*, czl_store_device*);
char czl_ast_serialize(czl_gp*, czl_fun*);
czl_fun* czl_run(czl_gp*);
#ifndef CZL_CONSOLE
char czl_resume_shell(czl_gp*, czl_fun*);
czl_tabkv* czl_tabkv_create(czl_gp*, czl_table*, int);
czl_tabkv* czl_tabkv_find(czl_table*, int);
char czl_tabkv_delete(czl_gp*, czl_table*, int);
#endif //#ifndef CZL_CONSOLE
void czl_hot_update_create(czl_gp*, const char*, czl_fun*);
void czl_hot_update_delete(czl_gp*, czl_hot_update*);
void czl_run_again(czl_gp*, czl_fun*, czl_var*, unsigned long);
void czl_run_clean(czl_gp*, czl_fun*);
char czl_val_del(czl_gp*, czl_var*);
void czl_hot_update_datas_free(czl_gp *gp, czl_hot_update_datas*);
void czl_memory_free(czl_gp*);
void czl_init_free(czl_gp*, char);
void czl_enum_list_delete(czl_gp*, czl_enum*);
void czl_block_delete(czl_gp*, char, void*, char);
unsigned long czl_hash_key_create(unsigned long, unsigned long);
unsigned long czl_str_hash(char*, unsigned long,
                           unsigned long, unsigned long);
czl_exp_ele* czl_opr_create(czl_gp*, czl_exp_node*);
void czl_fun_node_delete(czl_gp*, czl_fun*);
int czl_is_keyword(czl_gp*, char*);
char czl_get_para_ot(czl_gp*, char*);
czl_var* czl_ele_create(czl_gp*, char, void*);
void czl_ref_break(czl_gp*, czl_ref_var*, czl_var*);
char czl_ref_obj_create(czl_gp*, czl_var*, czl_var*, czl_var**, unsigned long);
void czl_ref_obj_delete(czl_gp*, czl_var*);
void czl_ref_obj_update(czl_var*);
void czl_vars_init(czl_var*, czl_var*, unsigned long);
void czl_coroutine_delete(czl_gp*, czl_var*, void**);
void czl_coroutine_paras_reset(czl_gp*, czl_var*, unsigned long);
czl_var* czl_coroutine_run(czl_gp*, czl_para*, unsigned long, void**);
void czl_log(czl_gp*, char*);
czl_usrlib* czl_usrlib_create(czl_gp*, char*);
char czl_sort_cmp_fun_ret(czl_gp*, czl_var*, czl_var*);
void czl_buf_file_delete(czl_gp*, czl_buf_file*);
unsigned long czl_bkdr_hash(char*, unsigned long);
void* czl_extsrc_create(czl_gp*, void*, void*, char*);
czl_extsrc* czl_extsrc_get(void**, char*);
void czl_extsrc_delete(czl_gp*, void**);
char czl_tcp_event_handle(czl_gp*, czl_fun*, czl_var*, unsigned long);
///////////////////////////////////////////////////////////////
#ifdef CZL_TIMER
void czl_timer_lock(czl_gp*);
void czl_timer_unlock(czl_gp*);
unsigned long czl_timer_create(czl_gp*, czl_timer*,
                           #ifdef CZL_SYSTEM_WINDOWS
                               unsigned long,
                               WINAPI int (*)(unsigned long),
                           #else //CZL_SYSTEM_LINUX
                               timer_t,
                               int (*)(timer_t),
                           #endif
                               unsigned long, czl_fun*);
char czl_timer_delete(czl_gp*, unsigned long);
char czl_timer_cb_fun_run(czl_gp*);
#endif //#ifdef CZL_TIMER
///////////////////////////////////////////////////////////////
#if (defined CZL_MULT_THREAD && defined CZL_CONSOLE)
    #ifdef CZL_SYSTEM_WINDOWS
        extern CRITICAL_SECTION czl_global_cs;
    #elif defined CZL_SYSTEM_LINUX
        extern pthread_mutex_t czl_global_mutex;
    #endif
    void czl_global_lock(void);
    void czl_global_unlock(void);
#endif //#if (defined CZL_MULT_THREAD && defined CZL_CONSOLE)
///////////////////////////////////////////////////////////////
void* czl_malloc(czl_gp*, czl_ulong
                 #ifdef CZL_MM_MODULE
                 , czl_mm_sp_pool*
                 #endif
                 );
void* czl_calloc(czl_gp*, czl_ulong, czl_ulong
                 #ifdef CZL_MM_MODULE
                 , czl_mm_sp_pool*
                 #endif
                 );
void* czl_realloc(czl_gp*, void*, czl_ulong, czl_ulong
                  #ifdef CZL_MM_MODULE
                  , czl_mm_sp_pool*
                  #endif
                  );
void czl_free(czl_gp*, void*, czl_ulong
              #ifdef CZL_MM_MODULE
              , czl_mm_sp_pool*
              #endif
              );

#ifdef CZL_MM_MODULE
    #ifdef CZL_MM_CACHE
    void czl_mm_cache_pools_destroy(czl_gp*, czl_mm_cache_handle*);
    #endif

    void czl_mm_module_init
    (
        czl_gp*,
        unsigned char,
        unsigned char,
        czl_ulong
    #ifdef CZL_MM_CACHE
        , czl_ulong
    #endif
    );

    void czl_mm_sp_destroy(czl_gp*, czl_mm_sp_pool*);
    czl_ulong czl_mm_gc(czl_gp*);

    #define CZL_RT_MALLOC(gp, size) czl_malloc(gp, size, gp->mmp_rt)
    #define CZL_RT_CALLOC(gp, num, size) czl_calloc(gp, num, size, gp->mmp_rt)
    #define CZL_RT_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size, gp->mmp_rt)
    #define CZL_RT_FREE(gp, buf, size) czl_free(gp, buf, size, gp->mmp_rt)

    #define CZL_TMP_MALLOC(gp, size) czl_malloc(gp, size, gp->mmp_tmp)
    #define CZL_TMP_CALLOC(gp, num, size) czl_calloc(gp, num, size, gp->mmp_tmp)
    #define CZL_TMP_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size, gp->mmp_tmp)
    #define CZL_TMP_FREE(gp, buf, size) czl_free(gp, buf, size, gp->mmp_tmp)

    #define CZL_STACK_MALLOC(gp, size) czl_malloc(gp, size, gp->mmp_stack)
    #define CZL_STACK_CALLOC(gp, num, size) czl_calloc(gp, num, size, gp->mmp_stack)
    #define CZL_STACK_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size, gp->mmp_stack)
    #define CZL_STACK_FREE(gp, buf, size) czl_free(gp, buf, size, gp->mmp_stack)

    #define CZL_STR_MALLOC(gp, size) (czl_string*)czl_malloc(gp, size, gp->mmp_str)
    #define CZL_STR_CALLOC(gp, num, size) czl_calloc(gp, num, size, gp->mmp_str)
    #define CZL_STR_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size, gp->mmp_str)
    #define CZL_STR_FREE(gp, buf, size) czl_free(gp, buf, size, gp->mmp_str)

    #define CZL_TAB_MALLOC(gp) czl_malloc(gp, sizeof(czl_table), &gp->mmp_tab)
    #define CZL_TAB_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_table), &gp->mmp_tab)

    #define CZL_ARR_MALLOC(gp) czl_malloc(gp, sizeof(czl_array), &gp->mmp_arr)
    #define CZL_ARR_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_array), &gp->mmp_arr)

    #define CZL_SQ_MALLOC(gp) czl_malloc(gp, sizeof(czl_sq), &gp->mmp_sq)
    #define CZL_SQ_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_sq), &gp->mmp_sq)

    #define CZL_REF_MALLOC(gp) czl_malloc(gp, sizeof(czl_ref_var), &gp->mmp_ref)
    #define CZL_REF_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_ref_var), &gp->mmp_ref)

    #define CZL_COR_MALLOC(gp) czl_malloc(gp, sizeof(czl_coroutine), &gp->mmp_cor)
    #define CZL_COR_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_coroutine), &gp->mmp_cor)

    #define CZL_FILE_MALLOC(gp) czl_malloc(gp, sizeof(czl_file), &gp->mmp_file)
    #define CZL_FILE_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_file), &gp->mmp_file)

    #define CZL_BUF_FILE_MALLOC(gp) (czl_buf_file*)czl_malloc(gp, sizeof(czl_buf_file), &gp->mmp_buf_file)
    #define CZL_BUF_FILE_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_buf_file), &gp->mmp_buf_file)

    #define CZL_HOT_UPDATE_MALLOC(gp) (czl_hot_update*)czl_malloc(gp, sizeof(czl_hot_update), &gp->mmp_hot_update)
    #define CZL_HOT_UPDATE_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_hot_update), &gp->mmp_hot_update)

    #define CZL_EXTSRC_MALLOC(gp) (czl_extsrc*)czl_malloc(gp, sizeof(czl_extsrc), &gp->mmp_extsrc)
    #define CZL_EXTSRC_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_extsrc), &gp->mmp_extsrc)

#ifdef CZL_MULT_THREAD
    #define CZL_THREAD_MALLOC(gp) (czl_thread*)czl_malloc(gp, sizeof(czl_thread), &gp->mmp_thread)
    #define CZL_THREAD_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_thread), &gp->mmp_thread)
#endif //#ifdef CZL_MULT_THREAD

#ifdef CZL_TIMER
    #define CZL_TIMER_MALLOC(gp) (czl_timer*)czl_malloc(gp, sizeof(czl_timer), &gp->mmp_timer)
    #define CZL_TIMER_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_timer), &gp->mmp_timer)
#endif //#ifdef CZL_TIMER

#else
    #define CZL_RT_MALLOC(gp, size) czl_malloc(gp, size)
    #define CZL_RT_CALLOC(gp, num, size) czl_calloc(gp, num, size)
    #define CZL_RT_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size)
    #define CZL_RT_FREE(gp, buf, size) czl_free(gp, buf, size)

    #define CZL_TMP_MALLOC(gp, size) czl_malloc(gp, size)
    #define CZL_TMP_CALLOC(gp, num, size) czl_calloc(gp, num, size)
    #define CZL_TMP_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size)
    #define CZL_TMP_FREE(gp, buf, size) czl_free(gp, buf, size)

    #define CZL_STACK_MALLOC(gp, size) czl_malloc(gp, size)
    #define CZL_STACK_CALLOC(gp, num, size) czl_calloc(gp, num, size)
    #define CZL_STACK_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size)
    #define CZL_STACK_FREE(gp, buf, size) czl_free(gp, buf, size)

    #define CZL_STR_MALLOC(gp, size) (czl_string*)czl_malloc(gp, size)
    #define CZL_STR_CALLOC(gp, num, size) czl_calloc(gp, num, size)
    #define CZL_STR_REALLOC(gp, buf, new_size, old_size) czl_realloc(gp, buf, new_size, old_size)
    #define CZL_STR_FREE(gp, buf, size) czl_free(gp, buf, size)

    #define CZL_TAB_MALLOC(gp) czl_malloc(gp, sizeof(czl_table))
    #define CZL_TAB_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_table))

    #define CZL_ARR_MALLOC(gp) czl_malloc(gp, sizeof(czl_array))
    #define CZL_ARR_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_array))

    #define CZL_SQ_MALLOC(gp) czl_malloc(gp, sizeof(czl_sq))
    #define CZL_SQ_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_sq))

    #define CZL_REF_MALLOC(gp) czl_malloc(gp, sizeof(czl_ref_var))
    #define CZL_REF_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_ref_var))

    #define CZL_COR_MALLOC(gp) czl_malloc(gp, sizeof(czl_coroutine))
    #define CZL_COR_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_coroutine))

    #define CZL_FILE_MALLOC(gp) czl_malloc(gp, sizeof(czl_file))
    #define CZL_FILE_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_file))

    #define CZL_BUF_FILE_MALLOC(gp) (czl_buf_file*)czl_malloc(gp, sizeof(czl_buf_file))
    #define CZL_BUF_FILE_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_buf_file))

    #define CZL_HOT_UPDATE_MALLOC(gp) (czl_hot_update*)czl_malloc(gp, sizeof(czl_hot_update))
    #define CZL_HOT_UPDATE_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_hot_update))

    #define CZL_EXTSRC_MALLOC(gp) (czl_extsrc*)czl_malloc(gp, sizeof(czl_extsrc))
    #define CZL_EXTSRC_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_extsrc))

#ifdef CZL_MULT_THREAD
    #define CZL_THREAD_MALLOC(gp) (czl_thread*)czl_malloc(gp, sizeof(czl_thread))
    #define CZL_THREAD_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_thread))
#endif //#ifdef CZL_MULT_THREAD

#ifdef CZL_TIMER
    #define CZL_TIMER_MALLOC(gp) (czl_timer*)czl_malloc(gp, sizeof(czl_timer))
    #define CZL_TIMER_FREE(gp, buf) czl_free(gp, buf, sizeof(czl_timer))
#endif //#ifdef CZL_TIMER

#endif //#ifdef CZL_MM_MODULE
///////////////////////////////////////////////////////////////
#endif //CZL_VM_H

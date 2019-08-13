#include "czl_ws.h"

#ifdef CZL_LIB_WS

//库函数声明，其中gp是CZL运行时用到的全局参数，fun是函数。
char czl_ws_res(czl_gp *gp, czl_fun *fun);    //响应websocket连接请求
char czl_ws_doc(czl_gp *gp, czl_fun *fun);    //解包websocket数据
char czl_ws_mask(czl_gp *gp, czl_fun *fun);   //对数据进行掩码计算
char czl_ws_send(czl_gp *gp, czl_fun *fun);   //发送websocket数据
char czl_ws_close(czl_gp *gp, czl_fun *fun);  //发送websocket关闭请求
char czl_ws_ping(czl_gp *gp, czl_fun *fun);   //发送websocket心跳ping
char czl_ws_pong(czl_gp *gp, czl_fun *fun);   //发送websocket心跳pong

//库函数表定义
const czl_sys_fun czl_lib_ws[] =
{
    //函数名,     函数指针,        参数个数,  参数声明
    {"res",      czl_ws_res,     2,        "int_v1,str_v2"},
    {"doc",      czl_ws_doc,     1,        "str_v1"},
    {"mask",     czl_ws_mask,    3,        "&str_v1,int_v2,int_v3"},
    {"send",     czl_ws_send,    3,        "int_v1,str_v2,int_v3=0"},
    {"close",    czl_ws_close,   1,        "int_v1"},
    {"ping",     czl_ws_ping,    1,        "int_v1"},
    {"pong",     czl_ws_pong,    1,        "int_v1"},
};

///////////////////////////////////////////////////////////////
#define CZL_SHA1_HASH_SIZE 20

typedef struct czl_sha1
{
    unsigned long Intermediate_Hash[CZL_SHA1_HASH_SIZE/4];
    unsigned long Length_Low;
    unsigned long Length_High;
    unsigned long Message_Block_Index;
    unsigned char Message_Block[64];
} czl_sha1;

#define CZL_SHA1_SHIFT(bits, word) (((word) << (bits)) | ((word) >> (32-(bits))))

void czl_sha1_process_message_block(czl_sha1 *context)
{
    const unsigned long K[] =
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
    int t;
    unsigned long temp;
    unsigned long W[80];
    unsigned long A, B, C, D, E;

    for(t = 0; t < 16; t++)
    {
        W[t] = context->Message_Block[t * 4] << 24;
        W[t] |= context->Message_Block[t * 4 + 1] << 16;
        W[t] |= context->Message_Block[t * 4 + 2] << 8;
        W[t] |= context->Message_Block[t * 4 + 3];
    }
    for(t = 16; t < 80; t++)
    {
        W[t] = CZL_SHA1_SHIFT(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }
    A = context->Intermediate_Hash[0];
    B = context->Intermediate_Hash[1];
    C = context->Intermediate_Hash[2];
    D = context->Intermediate_Hash[3];
    E = context->Intermediate_Hash[4];
    for(t = 0; t < 20; t++)
    {
                temp = CZL_SHA1_SHIFT(5,A) +
                        ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        E = D;
        D = C;
        C = CZL_SHA1_SHIFT(30,B);
        B = A;
        A = temp;
    }
    for(t = 20; t < 40; t++)
    {
        temp = CZL_SHA1_SHIFT(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = CZL_SHA1_SHIFT(30,B);
        B = A;
        A = temp;
    }
    for(t = 40; t < 60; t++)
    {
        temp = CZL_SHA1_SHIFT(5,A) +
            ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = CZL_SHA1_SHIFT(30,B);
        B = A;
        A = temp;
    }
    for(t = 60; t < 80; t++)
    {
        temp = CZL_SHA1_SHIFT(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = CZL_SHA1_SHIFT(30,B);
        B = A;
        A = temp;
    }
    context->Intermediate_Hash[0] += A;
    context->Intermediate_Hash[1] += B;
    context->Intermediate_Hash[2] += C;
    context->Intermediate_Hash[3] += D;
    context->Intermediate_Hash[4] += E;
    context->Message_Block_Index = 0;
}

void czl_sha1_pad_message(czl_sha1 *context)
{
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 64)
            context->Message_Block[context->Message_Block_Index++] = 0;
        czl_sha1_process_message_block(context);
        while(context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = 0;
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = 0;
    }

    context->Message_Block[56] = context->Length_High >> 24;
    context->Message_Block[57] = context->Length_High >> 16;
    context->Message_Block[58] = context->Length_High >> 8;
    context->Message_Block[59] = context->Length_High;
    context->Message_Block[60] = context->Length_Low >> 24;
    context->Message_Block[61] = context->Length_Low >> 16;
    context->Message_Block[62] = context->Length_Low >> 8;
    context->Message_Block[63] = context->Length_Low;
    czl_sha1_process_message_block(context);
}

void czl_sha1_reset(czl_sha1 *context)
{
    context->Length_Low = 0;
    context->Length_High = 0;
    context->Message_Block_Index = 0;
    context->Intermediate_Hash[0] = 0x67452301;
    context->Intermediate_Hash[1] = 0xEFCDAB89;
    context->Intermediate_Hash[2] = 0x98BADCFE;
    context->Intermediate_Hash[3] = 0x10325476;
    context->Intermediate_Hash[4] = 0xC3D2E1F0;
}

void czl_sha1_result(czl_sha1 *context, unsigned char Message_Digest[CZL_SHA1_HASH_SIZE])
{
    int i;

    czl_sha1_pad_message(context);
    for(i=0; i<64; ++i)
        context->Message_Block[i] = 0;
    context->Length_Low = 0;
    context->Length_High = 0;

    for(i = 0; i < CZL_SHA1_HASH_SIZE; ++i)
        Message_Digest[i] = context->Intermediate_Hash[i>>2] >> 8 * ( 3 - ( i & 0x03 ) );
}

void czl_sha1_compute(czl_sha1 *context, const unsigned char *message_array, unsigned length)
{
    unsigned char corrupted = 0;

    if (!length)
        return;

    while(length-- && !corrupted)
    {
        context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);
        context->Length_Low += 8;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            if (context->Length_High == 0)
                corrupted = 1;
        }
        if (context->Message_Block_Index == 64)
            czl_sha1_process_message_block(context);
        message_array++;
    }
}
///////////////////////////////////////////////////////////////
void czl_base64(const unsigned char *a, unsigned long len, char *b)
{
    static const char base64[64] =
    {
        'A','B','C','D','E','F','G','H','I','J','K','L','M',
        'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
        'a','b','c','d','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w','x','y','z',
        '0','1','2','3','4','5','6','7','8','9','+','/'
    };
    unsigned long i, j = len/3*3, k = len-j;

    for (i = 0; i < j; i += 3)
    {
        *(b++) = base64[*a>>2];
        *(b++) = base64[((*(a++)&0x03)<<4) | (*a>>4)];
        *(b++) = base64[((*(a++)&0x0f)<<2) | (*a&0xc0)>>6];
        *(b++) = base64[*(a++)&0x3f];
    }

    if (1 == k)
    {
        *(b++) = base64[*a>>2];
        *(b++) = base64[(*a&0x03)<<4];
        *(b++) = *(b++) = '=';
    }
    else if (2 == k)
    {
        *(b++) = base64[*a>>2];
        *(b++) = base64[((*(a++)&0x03)<<4) | (*a>>4)];
        *(b++) = base64[(*a&0x0f)<<2];
        *(b++) = '=';
    }

    *b = '\0';
}
///////////////////////////////////////////////////////////////
char czl_ws_res(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_string *key = CZL_STR(fun->vars[1].val.str.Obj);
    char res[128] = "HTTP/1.1 101 Switching Protocols\r\nConnection:Upgrade\r\nUpgrade:websocket\r\nSec-WebSocket-Accept:";
    unsigned long res_len = 94; //strlen(res)
    const char *code = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char magic[128];
    unsigned long magic_len = key->len + 36; //strlen(code)
    czl_sha1 sha;
    unsigned char msg_digest[CZL_SHA1_HASH_SIZE];
    unsigned long base64_len = CZL_SHA1_HASH_SIZE/3*4 + (CZL_SHA1_HASH_SIZE%3 ? 4 : 0);

    if (key->len > 64)
    {
        fun->ret.val.inum = 0;
        return 1;
    }

    strcpy(magic, key->str);
    strcat(magic, code);

    czl_sha1_reset(&sha);
    czl_sha1_compute(&sha, magic, magic_len);
    czl_sha1_result(&sha, msg_digest);

    czl_base64(msg_digest, CZL_SHA1_HASH_SIZE, res+res_len);
    res_len += base64_len;

    res[res_len++] = '\r';
    res[res_len++] = '\n';
    res[res_len++] = '\r';
    res[res_len++] = '\n';

    fun->ret.val.inum = czl_net_send(gp, sock, res, res_len);
    return 1;
}

void czl_ws_mask_compute
(
    unsigned char *s,
    unsigned long size,
    unsigned long mask
)
{
    unsigned long cnt, len = size;

    size = size/4*4;
    len -= size;

    for (cnt = 0; cnt < size; cnt += 4)
    {
        *(s++) ^= (mask&0xff);
        *(s++) ^= ((mask&0xff00)>>8);
        *(s++) ^= ((mask&0xff0000)>>16);
        *(s++) ^= ((mask&0xff000000)>>24);
    }

    if (1 == len)
        *(s++) ^= (mask&0xff);
    else if (2 == len)
    {
        *(s++) ^= (mask&0xff);
        *(s++) ^= ((mask&0xff00)>>8);
    }
    else if (3 == len)
    {
        *(s++) ^= (mask&0xff);
        *(s++) ^= ((mask&0xff00)>>8);
        *(s++) ^= ((mask&0xff0000)>>16);
    }
}

char czl_ws_doc(czl_gp *gp, czl_fun *fun)
{
    czl_string *str = CZL_STR(fun->vars->val.str.Obj);
    void **obj = czl_array_create(gp, 4, 0);
    czl_array *arr;
    czl_var *vars;
    unsigned char *s, *e;

    if (!obj)
        return 0;
    arr = CZL_ARR(obj);

    s = str->str;
    e = s + str->len;

    while (s < e)
    {
        unsigned long size, cnt;
        unsigned char maskFlag, opcode = *s&0x0f;

        if (arr->cnt == arr->sum && !czl_array_resize(gp, arr, arr->cnt))
            goto CZL_OOM;

        if (!(arr->vars[arr->cnt].val.Obj=czl_array_create(gp, 6, 6)))
            return 0;
        vars = CZL_ARR(arr->vars[arr->cnt].val.Obj)->vars;
        arr->vars[arr->cnt++].type = CZL_ARRAY;

        vars[0].val.inum = (*s&0x80)>>4;
        vars[1].val.inum = opcode;
        if (opcode >= 0x08 && opcode <= 0x0a && s+1 == e)
        {
            ++s;
            break;
        }

        size = s[1]&0x7f;
        if (size < 126)
            cnt = 2;
        else if (126 == size)
        {
            if (e-s < 4)
                break;
            size = s[2]<<8 | s[3];
            cnt = 4;
        }
        else
        {
            if (e-s < 10)
                break;
            size = s[6]<<24 | s[7]<<16 | s[8]<<8 | s[9];
            cnt = 10;
        }
        maskFlag = s[1]&0x80;
        if (maskFlag)
            cnt += 4;
        if (s+cnt > e)
            break;

        vars[2].val.inum = size;
        if (size > e-s-cnt)
            size = e - s - cnt;

        if (maskFlag)
        {
            unsigned long mask = *((unsigned long*)(s+cnt-4));
            vars[3].val.inum = mask;
            if (!czl_set_ret_str(gp, vars+4, s+cnt, size))
                goto CZL_OOM;
            if (vars[2].val.inum == size)
                czl_ws_mask_compute(CZL_STR(vars[4].val.str.Obj)->str, size, mask);
        }
        else if (!czl_set_ret_str(gp, vars+4, s+cnt, size))
            goto CZL_OOM;

        s += (cnt+size);
    }

    if (s < e && !czl_set_ret_str(gp, vars+4, s, e-s))
        goto CZL_OOM;

    fun->ret.type = CZL_ARRAY;
    fun->ret.val.Obj = obj;
    return 1;

CZL_OOM:
    czl_array_delete(gp, obj);
    return 0;
}

char czl_ws_mask(czl_gp *gp, czl_fun *fun)
{
    czl_var *data = CZL_GRV(fun->vars);
    czl_string *str = CZL_STR(data->val.str.Obj);
    unsigned long mask = fun->vars[1].val.inum;
    unsigned long offset = fun->vars[2].val.inum;

    if (0 == mask || offset > str->len)
        return 1;

    if (str->rc > 1 && !(str=czl_string_fork(gp, data)))
        return 0;

    czl_ws_mask_compute(str->str+offset, str->len-offset, mask);
    return 1;
}

char czl_ws_send(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    czl_string *str = CZL_STR(fun->vars[1].val.str.Obj);
    unsigned long mask = fun->vars[2].val.inum;
    unsigned char *buf = CZL_TMP_MALLOC(gp, 14+str->len);
    unsigned long len;

    if (!buf)
        return 0;

    buf[0] = 0x81;
    buf[1] = (mask ? 0x80 : 0x00);

    if (str->len < 126)
    {
        buf[1] |= str->len;
        len = 2;
    }
    else if (str->len < 65536)
    {
        buf[1] |= 126;
        buf[2] = (str->len&0xff00) >> 8;
        buf[3] = str->len&0xff;
        len = 4;
    }
    else
    {
        buf[1] |= 127;
        buf[2] = buf[3] = buf[4] = buf[5] = 0;
        buf[6] = (str->len&0xff000000) >> 24;
        buf[7] = (str->len&0xff0000) >> 16;
        buf[8] = (str->len&0xff00) >> 8;
        buf[9] = str->len&0xff;
        len = 10;
    }

    if (mask)
    {
        *((unsigned long*)(buf+len)) = mask;
        len += 4;
        memcpy(buf+len, str->str, str->len);
        czl_ws_mask_compute(buf+len, str->len, mask);
    }
    else
        memcpy(buf+len, str->str, str->len);

    if (czl_net_send(gp, sock, buf, len+str->len) < 0)
        fun->ret.val.inum = -1;
    else
        fun->ret.val.inum = str->len;

    CZL_TMP_FREE(gp, buf, 14+str->len);
    return 1;
}

char czl_ws_close(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    char order = 0x88;
    fun->ret.val.inum = czl_net_send(gp, sock, &order, 1);
    return 1;
}

char czl_ws_ping(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    char order = 0x89;
    fun->ret.val.inum = czl_net_send(gp, sock, &order, 1);
    return 1;
}

char czl_ws_pong(czl_gp *gp, czl_fun *fun)
{
    SOCKET sock = fun->vars[0].val.inum;
    char order = 0x8a;
    fun->ret.val.inum = czl_net_send(gp, sock, &order, 1);
    return 1;
}
#endif //CZL_LIB_WS

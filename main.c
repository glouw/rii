#include <stdbool.h>
#include <stdio.h>

#include <str.h>

#define T str
#include <vec.h>

#define P
#define T char
#include <deq.h>

static int line;

static str namespace;

#define quit(...) \
    printf("error: line %d: ", line), printf(__VA_ARGS__), exit(1)

#define TYPES \
    X(I8) X(U8) X(I16) X(U16) X(I32) X(U32) X(I64) X(U64) X(F32) X(F64) \
    X(STR) X(FUN) X(REF) X(OBJ) X(ARR) X(NUL) X(BLN)

#define X(A) A,
typedef enum { TYPES } Type;
#undef X

#define X(A) #A,
static const char* TypeStr[] = { TYPES };
#undef X

typedef struct Val* Elem;

static void Elem_free(Elem*);
static Elem Elem_copy(Elem*);
#define T Elem
#include <deq.h>

typedef struct
{
    str str;
    Elem elem;
    bool constant;
}
Memb;

static void Memb_free(Memb*);
static Memb Memb_copy(Memb*);
#define T Memb
#include <set.h>

static set_Memb db;

typedef union
{
    set_Memb obj;
    deq_Elem arr;
    Memb* ref;
    str str;
    vec_str fun;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;
    bool bln;
}
Poly;

typedef struct Val
{
    Type type;
    Poly poly;
}
Val;

static void
PromoteAdd(Elem a, Elem b)
{
    const char* operator = "+";
#define OP +
#include "promote.c"
}

static void
PromoteSub(Elem a, Elem b)
{
    const char* operator = "-";
#define OP -
#include "promote.c"
}

static void
PromoteDiv(Elem a, Elem b)
{
    const char* operator = "/";
#define OP /
#include "promote.c"
}

static void
PromoteMul(Elem a, Elem b)
{
    const char* operator = "*";
#define OP *
#include "promote.c"
}

static void
PromoteEquals(Elem a, Elem b)
{
    const char* operator = "==";
#define OP ==
#include "compare.c"
}

static void
PromoteGT(Elem a, Elem b)
{
    const char* operator = ">";
#define OP >
#include "compare.c"
}

static void
PromoteLT(Elem a, Elem b)
{
    const char* operator = "<";
#define OP <
#include "compare.c"
}

static void
PromoteGTE(Elem a, Elem b)
{
    const char* operator = ">=";
#define OP >=
#include "compare.c"
}

static void
PromoteLTE(Elem a, Elem b)
{
    const char* operator = "<=";
#define OP <=
#include "compare.c"
}

static void
Cast(Elem e, Type type)
{
#include "cast.c"
}

static bool
Equal(str* a, const char* b)
{
    return str_compare(a, b) == 0;
}

static Elem
Elem_init(Type type, Poly poly)
{
    Elem elem = malloc(sizeof(*elem));
    elem->type = type;
    elem->poly = poly;
    return elem;
}

static void
Elem_free(Elem* e)
{
    Type type = (*e)->type;
    Poly poly = (*e)->poly;
    switch(type)
    {
    case OBJ:
        set_Memb_free(&poly.obj);
        break;
    case ARR:
        deq_Elem_free(&poly.arr);
        break;
    case STR:
        str_free(&poly.str);
        break;
    case FUN:
        vec_str_free(&poly.fun);
        break;
    default:
        break;
    }
    free(*e);
}

static Elem
Elem_copy(Elem* e)
{
    Type type = (*e)->type;
    Poly poly = (*e)->poly;
    Elem copy = malloc(sizeof(*copy));
    copy->type = type;
    switch(type)
    {
    case OBJ:
        copy->poly.obj = set_Memb_copy(&poly.obj);
        break;
    case ARR:
        copy->poly.arr = deq_Elem_copy(&poly.arr);
        break;
    case STR:
        copy->poly.str = str_copy(&poly.str);
        break;
    case FUN:
        copy->poly.fun = vec_str_copy(&poly.fun);
        break;
    default:
        copy->poly = poly;
        break;
    }
    return copy;
}

static void
Memb_free(Memb* m)
{
    str_free(&m->str);
    Elem_free(&m->elem);
}

static Memb
Memb_init(bool c, str s, Elem e)
{
    return (Memb) { s, e, c };
}

static Memb
Memb_copy(Memb* m)
{
    Elem e = Elem_copy(&m->elem);
    return Memb_init(m->constant, str_copy(&m->str), e);
}

static int
Memb_compare(Memb* a, Memb* b)
{
    return str_key_compare(&a->str, &b->str);
}

static deq_char
Queue(const char* code)
{
    deq_char q = deq_char_init();
    for(const char* c = code; *c; c++)
        deq_char_push_back(&q, *c);
    return q;
}

static void
Unpop(deq_char* q, char c)
{
    if(c == '\n')
        line -= 1;
    deq_char_push_front(q, c);
}

static void
Requeue(deq_char* q, str* s)
{
    for(int i = s->size - 1; i >= 0; i--)
        Unpop(q, s->value[i]);
}

static bool
IsSpace(char c)
{
    return c == ' '
        || c == '\n';
}

static bool
IsLower(char c)
{
    return c >= 'a'
        && c <= 'z';
}

static bool
IsUpper(char c)
{
    return c >= 'A'
        && c <= 'Z';
}

static bool
IsDigit(char c)
{
    return c >= '0'
        && c <= '9';
}

static bool
IsNum(char c)
{
    return IsDigit(c)
        || c == '.'
        || c == '-'
        || c == '+';
}

static bool
IsString(char c)
{
    return IsLower(c)
        || IsUpper(c)
        || IsSpace(c)
        || IsNum(c);
}

static bool
IsIdent(char c)
{
    return IsLower(c)
        || IsUpper(c)
        || IsDigit(c)
        || c == '_';
}

static void
Pop(deq_char* q)
{
    if(*deq_char_front(q) == '\n')
        line += 1;
    deq_char_pop_front(q);
}

static void
Comment(deq_char* q)
{
    while(!deq_char_empty(q))
    {
        char c = *deq_char_front(q);
        Pop(q);
        if(c == '\n')
            break;
    }
}

static void
Spin(deq_char* q)
{
    while(!deq_char_empty(q))
    {
        char c = *deq_char_front(q);
        if(IsSpace(c))
            Pop(q);
        else
        if(c == '#')
            Comment(q);
        else
            break;
    }
}

static char
Next(deq_char* q)
{
    Spin(q);
    return *deq_char_front(q);
}

static void
Match(deq_char* q, char c)
{
    char n = Next(q);
    if(n != c)
        quit("got '%c'; expected '%c'\n", n, c);
    Pop(q);
}

static str
Read(deq_char* q, bool clause(char))
{
    Spin(q);
    str s = str_init("");
    while(!deq_char_empty(q))
    {
        char c = *deq_char_front(q);
        if(clause(c))
        {
            str_push_back(&s, c);
            Pop(q);
        }
        else
            break;
    }
    return s;
}

static str
Prefix(str* s, str* with)
{
    str n = str_copy(with);
    if(n.size > 0)
        str_append(&n, ".");
    str_append(&n, s->value);
    return n;
}

static set_Memb_node*
Find(str* s)
{
    // FIRST, FIND LOCAL.
    str n = Prefix(s, &namespace);
    set_Memb_node* local = set_Memb_find(&db, (Memb) { .str = n });
    str_free(&n);
    if(local)
        return local;
    // FAILING, FIND GLOBAL.
    return set_Memb_find(&db, (Memb) { .str = *s });
}

static void
Erase(str* s)
{
    set_Memb_node* node = Find(s);
    set_Memb_erase_node(&db, node);
}

static set_Memb_node*
Exists(str* s)
{
    set_Memb_node* node = Find(s);
    if(node == NULL)
        quit("identifier '%s' not defined\n", s->value);
    return node;
}

static void
Insert(str* s, Elem e, bool c)
{
    str n = Prefix(s, &namespace);
    Memb m = Memb_init(c, n, e);
    set_Memb_insert(&db, m);
}

static str
ReadBlock(deq_char* q)
{
    if(Next(q) != '{')
        quit("expected block `{`\n");
    int open = 0;
    str b = str_init("");
    do
    {   // MUST READ SPACE.
        char n = *deq_char_front(q);
        if(n == '{') open++;
        if(n == '}') open--;
        str_push_back(&b, n);
        Pop(q);
    }
    while(open > 0);
    return b;
}

static vec_str
Params(deq_char* q, bool check)
{
    vec_str p = vec_str_init();
    Match(q, '(');
    while(Next(q) != ')')
    {
        str s = Read(q, IsIdent);
        if(check)
            if(Find(&s))
                quit("`%s` already defined", s.value);
        vec_str_push_back(&p, s);
        char n = Next(q);
        if(IsIdent(n))
            quit("expected ','\n");
        if(n == ',')
            Match(q, ',');
    }
    Match(q, ')');
    return p;
}

static void
Fun(deq_char* q, str* l)
{
    str label = str_copy(l);
    vec_str p = Params(q, true);
    vec_str_push_back(&p, label);
    str code = ReadBlock(q);
    vec_str_push_back(&p, code);
    Insert(&label, Elem_init(FUN, (Poly) { .fun = p }), true);
}

static Memb*
Deref(Memb* m)
{
    Memb* other = m->elem->poly.ref;
    while(other->elem->type == REF)
        other = other->elem->poly.ref;
    return other;
}

static Memb*
Resolve(str* s)
{
    Memb* m = &Exists(s)->key;
    return (m->elem->type == REF) ? Deref(m) : m;
}

static Elem
Call(Memb*, vec_str*);

static Elem
Ident(deq_char* q)
{
    str s = Read(q, IsIdent);
    Memb* m = Resolve(&s);
    Elem e;
    if(Next(q) == '(')
    {
        vec_str p = Params(q, false);
        e = Call(m, &p);
        vec_str_free(&p);
    }
    else
        e = Elem_copy(&m->elem);
    str_free(&s);
    return e;
}

static str
String(deq_char* q)
{
    Match(q, '"');
    str s = Read(q, IsString);
    Match(q, '"');
    return s;
}

static Poly
LoadValue(deq_char* q, Type t)
{
    Poly p = { 0 };
    str s = Read(q, IsNum);
    switch(t)
    {
    case  I8: p.i8  = strtol  (s.value, NULL, 10); break;
    case  U8: p.u8  = strtoul (s.value, NULL, 10); break;
    case I16: p.i16 = strtol  (s.value, NULL, 10); break;
    case U16: p.u16 = strtoul (s.value, NULL, 10); break;
    case I32: p.i32 = strtol  (s.value, NULL, 10); break;
    case U32: p.u32 = strtoul (s.value, NULL, 10); break;
    case I64: p.i64 = strtoll (s.value, NULL, 10); break;
    case U64: p.u64 = strtoull(s.value, NULL, 10); break;
    case F32: p.f32 = strtof  (s.value, NULL); break;
    default:
    case F64: p.f64 = strtod  (s.value, NULL); break;
    }
    str_free(&s);
    return p;
}

static Type
LoadType(deq_char* q, char l, char r)
{
    Match(q, l);
    str t = Read(q, IsIdent);
    Type type;
    if(Equal(&t,  "i8")) type =  I8; else
    if(Equal(&t,  "u8")) type =  U8; else
    if(Equal(&t, "i16")) type = I16; else
    if(Equal(&t, "u16")) type = U16; else
    if(Equal(&t, "i32")) type = I32; else
    if(Equal(&t, "u32")) type = U32; else
    if(Equal(&t, "i64")) type = I64; else
    if(Equal(&t, "u64")) type = U64; else
    if(Equal(&t, "f32")) type = F32; else
    if(Equal(&t, "f64")) type = F64; else quit("direct type load '%s' not supported\n", t.value);
    Match(q, r);
    str_free(&t);
    return type;
}

static Elem
Value(deq_char* q)
{
    Type t = (Next(q) == '|') ? LoadType(q, '|', '|') : F64;
    Poly p = LoadValue(q, t);
    return Elem_init(t, p);
}

static Elem
Expression(deq_char*);

static deq_Elem
Array(deq_char* q)
{
    deq_Elem es = deq_Elem_init();
    Match(q, '[');
    if(Next(q) != ']')
        while(true)
        {
            Elem e = Expression(q);
            deq_Elem_push_back(&es, e);
            if(Next(q) == ',')
                Match(q, ',');
            else
                break;
        }
    Match(q, ']');
    return es;
}

static void
Members(deq_char* q, set_Memb* m)
{
    while(true)
    {
        Elem s = Expression(q);
        if(s->type != STR)
            quit("Key value must be strings");
        Match(q, ':');
        Elem e = Expression(q);
        set_Memb_insert(m, Memb_init(true, str_copy(&s->poly.str), e));
        Elem_free(&s);
        if(Next(q) == ',')
            Match(q, ',');
        else
            break;
    }
}

static set_Memb
Object(deq_char* q)
{
    set_Memb m = set_Memb_init(Memb_compare);
    Match(q, '{');
    if(Next(q) != '}')
        Members(q, &m);
    Match(q, '}');
    return m;
}

static void
deq_Elem_zip(deq_Elem* a, deq_Elem* b, void Op(Elem, Elem))
{
    if(a->size != b->size)
        quit("array size mismatch - sizes were `%lu` and `%lu`", a->size, b->size);
    deq_Elem_it ia = deq_Elem_it_each(a);
    deq_Elem_it ib = deq_Elem_it_each(b);
    while(!ia.done && !ib.done)
    {
        Op(*ia.ref, *ib.ref);
        ia.step(&ia);
        ib.step(&ib);
    }
}

static void
set_Memb_zip(set_Memb* a, set_Memb* b, void Op(Elem, Elem))
{
    foreach(set_Memb, b, it)
    {
        Memb m = { .str = it.ref->str };
        set_Memb_node* node = set_Memb_find(a, m);
        if(node)
            Op(node->key.elem, it.ref->elem);
    }
}

static char*
Label(Elem e)
{
    vec_str* v = &e->poly.fun;
    return v->value[v->size - 2].value;
}

static char*
Code(Elem e)
{
    vec_str* v = &e->poly.fun;
    return v->value[v->size - 1].value;
}

static size_t
Arguments(Elem e)
{
    vec_str* v = &e->poly.fun;
    return v->size - 2; // LABEL AND CODE ARE LAST TWO ELEMS.
}

static void
StrSub(str* a, str* b)
{
    if(a->size == 0)
        return;
    size_t n = str_find_last_of(a, b->value);
    if(n == a->size - 1)
    {
        size_t len = b->size;
        while(len--)
            str_pop_back(a); 
    }
}

static bool
Basic(Elem a)
{
    return a->type >= I8 && a->type <= F64;
}

static void
Check(Elem a, Elem b)
{
    if(Basic(a) && Basic(b))
        return;
    if(a->type != b->type)
        quit("type mismatch - types were '%s' and '%s'\n",
            TypeStr[a->type],
            TypeStr[b->type]);
}

static void
Mul(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Mul); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Mul); break; 
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
        PromoteMul(a, b);
        break;
    case STR: case FUN: case NUL: case BLN: case REF:
        quit("mul (*) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
Div(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Div); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Div); break; 
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
        PromoteDiv(a, b);
        break;
    case STR: case FUN: case NUL: case BLN: case REF: 
        quit("div (/) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
Add(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: str_append(&a->poly.str, str_c_str(&b->poly.str)); break; 
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
        PromoteAdd(a, b);
        break;
    case FUN: case NUL: case BLN: case REF:
        quit("add (+) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
Sub(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Sub); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Sub); break; 
    case STR: StrSub(&a->poly.str, &b->poly.str); break;
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
        PromoteSub(a, b);
        break;
    case FUN: case NUL: case BLN: case REF:
        quit("sub (-) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static bool StrET(char* a, char* b) { return strcmp(a, b) == 0; }
static bool StrLT(char* a, char* b) { return strcmp(a, b) < 0; }
static bool StrGT(char* a, char* b) { return strcmp(a, b) > 0; }
static bool StrLTE(char* a, char* b) { return StrLT(a, b) || StrET(a, b); }
static bool StrGTE(char* a, char* b) { return StrGT(a, b) || StrET(a, b); }

static void
StrToBoolOp(Elem e, bool compare(char*, char*), char* a, char* b)
{
    bool bln = compare(a, b);
    str_free(&e->poly.str);
    e->poly.bln = bln;
    e->type = BLN;
}

static void
FunToBool(Elem a, Elem b)
{
    bool bln = strcmp(Label(a), Label(b)) == 0;
    vec_str_free(&a->poly.fun);
    a->poly.bln = bln;
    a->type = BLN;
}

static void
BoolEqual(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: StrToBoolOp(a, StrET, a->poly.str.value, b->poly.str.value); break;
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
    case BLN:
        PromoteEquals(a, b);
        break;
    case FUN: FunToBool(a, b); break;
    case NUL: case REF:
        quit("equals (==) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
BoolGT(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: StrToBoolOp(a, StrGT, a->poly.str.value, b->poly.str.value); break;
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
    case BLN:
        PromoteGT(a, b);
        break;
    case FUN: case NUL: case REF:
        quit("greater than (>) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
BoolLT(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: StrToBoolOp(a, StrLT, a->poly.str.value, b->poly.str.value); break;
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
    case BLN:
        PromoteLT(a, b);
        break;
    case FUN: case NUL: case REF:
        quit("less than (<) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
BoolLTE(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: StrToBoolOp(a, StrLTE, a->poly.str.value, b->poly.str.value); break;
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
    case BLN:
        PromoteLTE(a, b);
        break;
    case FUN: case NUL: case REF:
        quit("less than or equal to (<=) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
BoolGTE(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR: deq_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: StrToBoolOp(a, StrGTE, a->poly.str.value, b->poly.str.value); break;
    case  I8: case  U8: case I16: case U16: case I32:
    case U32: case I64: case U64: case F32: case F64:
    case BLN:
        PromoteGTE(a, b);
        break;
    case FUN: case NUL: case REF:
        quit("greater than or equal to (>=) not supported for types `%s` and `%s`\n",
            TypeStr[a->type],
            TypeStr[b->type]); 
        break; 
    }
}

static void
BoolNot(Elem a)
{
    a->poly.bln = !a->poly.bln;
}

static Elem
Element(deq_char* q)
{
    char n = Next(q);
    return (n == '"') ? Elem_init(STR, (Poly) { .str = String(q) })
         : (n == '{') ? Elem_init(OBJ, (Poly) { .obj = Object(q) })
         : (n == '[') ? Elem_init(ARR, (Poly) { .arr = Array (q) })
         : (n == '|') || IsNum(n) ? Value(q)
         : Ident(q);
}

static Elem
Parn(deq_char* q)
{
    Match(q, '(');
    Elem a = Expression(q);
    Match(q, ')');
    return a;
}

static Elem
Factor(deq_char* q)
{
    Type t = (Next(q) == '<') ? LoadType(q, '<', '>') : NUL;
    Elem e = (Next(q) == '(') ? Parn(q) : Element(q);
    if(t != NUL)
        Cast(e, t);
    return e;
}

static Elem
Term(deq_char* q)
{
    Elem a = Factor(q);
    while(true)
    {
        char n = Next(q);
        if(n == '*')
        {
            Match(q, '*');
            Elem b = Factor(q);
            Mul(a, b);
            Elem_free(&b);
        }
        else
        if(n == '/')
        {
            Match(q, '/');
            Elem b = Factor(q);
            Div(a, b);
            Elem_free(&b);
        }
        else
            break;
    }
    return a;
}

static Elem
Expression(deq_char* q)
{
    Elem a = Term(q);
    while(true)
    {
        char n = Next(q);
        if(n == '+')
        {
            Match(q, '+');
            Elem b = Term(q);
            Add(a, b);
            Elem_free(&b);
        }
        else
        if(n == '-')
        {
            Match(q, '-');
            Elem b = Term(q);
            Sub(a, b);
            Elem_free(&b);
        }
        else
        if(n == '!')
        {
            Match(q, '!');
            Match(q, '=');
            Elem b = Expression(q);
            BoolEqual(a, b);
            BoolNot(a);
            Elem_free(&b);
        }
        else
        if(n == '=')
        {
            Match(q, '=');
            Match(q, '=');
            Elem b = Expression(q);
            BoolEqual(a, b);
            Elem_free(&b);
        }
        else
        if(n == '>')
        {
            Match(q, '>');
            if(Next(q) == '=')
            {
                Match(q, '=');
                Elem b = Expression(q);
                BoolGTE(a, b);
                Elem_free(&b);
            }
            else
            {
                Elem b = Expression(q);
                BoolGT(a, b);
                Elem_free(&b);
            }
        }
        else
        if(n == '<')
        {
            Match(q, '<');
            if(Next(q) == '=')
            {
                Match(q, '=');
                Elem b = Expression(q);
                BoolLTE(a, b);
                Elem_free(&b);
            }
            else
            {
                Elem b = Expression(q);
                BoolLT(a, b);
                Elem_free(&b);
            }
        }
        else
            break;
    }
    return a;
}

static void
Tabs(int tabs)
{
    int width = 4;
    while(tabs--)
        for(int i = 0; i < width; i++)
            putchar(' ');
}

static void
Write(Elem e, int tabs);

static void
Obj_write(Elem e, int tabs)
{
    size_t index = 0;
    putchar('{');
    putchar('\n');
    foreach(set_Memb, &e->poly.obj, it)
    {
        Tabs(tabs);
        printf("\"%s\" : ", it.ref->str.value);
        Write(it.ref->elem, tabs);
        index += 1;
        if(index < e->poly.obj.size)
            putchar(',');
        putchar('\n');
    }
    Tabs(tabs - 1);
    putchar('}');
}

static void
Arr_write(Elem e, int tabs)
{
    size_t index = 0;
    putchar('[');
    putchar('\n');
    foreach(deq_Elem, &e->poly.arr, it)
    {
        Tabs(tabs);
        Write(*it.ref, tabs);
        index += 1;
        if(index < e->poly.arr.size)
            putchar(',');
        putchar('\n');
    }
    Tabs(tabs - 1);
    putchar(']');
}

static void
Write(Elem e, int tabs)
{
    switch(e->type)
    {
    case OBJ: Obj_write(e, tabs + 1); break;
    case ARR: Arr_write(e, tabs + 1); break;
    case STR: printf("|str| \"%s\"", e->poly.str.value); break;
    case REF: printf("|ref| %p ", (void*) e->poly.ref->elem); Write(e->poly.ref->elem, 0); break;
    case NUL: printf("|null| null"); break;
    case I8:  printf("|i8| %d", e->poly.i8); break;
    case U8:  printf("|u8| %u", e->poly.u8); break;
    case I16: printf("|i16| %d", e->poly.i16); break;
    case U16: printf("|u16| %u", e->poly.u16); break;
    case I32: printf("|i32| %d", e->poly.i32); break;
    case U32: printf("|u32| %u", e->poly.u32); break;
    case I64: printf("|i64| %ld", e->poly.i64); break;
    case U64: printf("|u64| %lu", e->poly.u64); break;
    case F32: printf("|f32| %f", e->poly.f32); break;
    case F64: printf("|f64| %lf", e->poly.f64); break;
    case FUN: printf("|fun| %s", Label(e)); break;
    case BLN: printf("|bln| %s", e->poly.bln ? "true" : "false"); break;
    }
}

static void
Print(Elem e)
{
    Write(e, 0);
    putchar('\n');
}

static void
Define(deq_char* q, str* n, bool mut)
{
    Match(q, ':');
    Match(q, '=');
    if(Find(n))
        quit("`%s` already defined", n->value);
    Elem e = Expression(q);
    Insert(n, e, mut ? false : true);
}

static void
Update(deq_char* q, str* n)
{
    Match(q, '=');
    Exists(n);
    Memb* m = Resolve(n);
    if(m->constant)
        quit("%s `%s` is constant", TypeStr[m->elem->type], m->str.value);
    Elem e = Expression(q);
    Elem_free(&m->elem);
    m->elem = e;
}

static bool
Mutable(deq_char* q, str* s)
{
    bool mut = Equal(s, "mut");
    if(mut)
    {
        str_free(s);
        *s = Read(q, IsIdent);
    }
    return mut;
}

static Elem
Cond(deq_char* q)
{
    Match(q, '(');
    Elem e = Expression(q);
    if(e->type != BLN)
        quit("expression must evaluate to boolean\n");
    Match(q, ')');
    return e;
}

static Elem
Block(deq_char*);

static void
CondBlock(const char* code, bool* exec, Elem* ret)
{
    deq_char b = Queue(code);
    Elem e = Block(&b);
    deq_char_free(&b);
    Elem_free(ret);
    *ret = e;
    *exec = true;
}

static void
If(deq_char* q, bool* exec, Elem* ret)
{
    Elem e = Cond(q);
    str code = ReadBlock(q);
    if(e->poly.bln == true)
        if(*exec == false)
            CondBlock(code.value, exec, ret);
    str_free(&code);
    Elem_free(&e);
}

static void
Else(deq_char* q, bool* exec, Elem* ret)
{
    str code = ReadBlock(q);
    if(*exec == false)
        CondBlock(code.value, exec, ret);
    str_free(&code);
}

static void
IfChain(deq_char* q, Elem* ret)
{
    bool exec = false;
    bool done = false;
    If(q, &exec, ret);
    while(!done)
    {
        str n = Read(q, IsIdent);
        if(Equal(&n, "elif"))
            If(q, &exec, ret);
        else
        if(Equal(&n, "else"))
            Else(q, &exec, ret);
        else
        {
            Requeue(q, &n);
            done = true;
        }
        str_free(&n);
    }
}

static Elem
Block(deq_char* q)
{
    Match(q, '{');
    static Poly zero;
    Elem ret = Elem_init(NUL, zero);
    set_Memb old = set_Memb_copy(&db);
    bool abort = false;
    while(Next(q) != '}')
    {
        str s = Read(q, IsIdent);
        if(Equal(&s, "ret"))
        {
            Elem_free(&ret);
            ret = Expression(q);
            Match(q, ';');
            abort = true;
        }
        else
        if(Equal(&s, "if"))
        {
            IfChain(q, &ret);
            if(ret->type != NUL)
                abort = true;
        }
        else
        {
            bool mut = Mutable(q, &s);
            char c = Next(q);
            if(c == ':')
                Define(q, &s, mut);
            else
            if(c == '=')
                Update(q, &s);
            else
            {
                Requeue(q, &s);
                Elem e = Expression(q);
                Elem_free(&e);
            }
            Match(q, ';');
        }
        str_free(&s);
    }
    if(abort == false)
        Match(q, '}');
    set_Memb diff = set_Memb_difference(&db, &old);
    foreach(set_Memb, &diff, it)
    {
        printf(">> FREE %s: ", it.ref->str.value);
        Print(it.ref->elem);
    }
    foreach(set_Memb, &diff, it)
        Erase(&it.ref->str);
    set_Memb_free(&old);
    set_Memb_free(&diff);
    return ret;
}

static Elem
Call(Memb* m, vec_str* args)
{
    if(m->elem->type != FUN)
        quit("expected function\n");
    int exp = Arguments(m->elem);
    int got = args->size;
    if(got != exp)
        quit("`%s()` got %d args but expected %d\n", m->str.value, got, exp);
    char* code = Code(m->elem);
    deq_char q = Queue(code);
    str new = str_copy(&m->str);
    Memb* membs[got];
    int index = 0;
    foreach(vec_str, args, it)
        membs[index++] = &Exists(it.ref)->key;
    str_swap(&namespace, &new);
    for(int i = 0; i < got; i++)
    {
        Elem e = Elem_init(REF, (Poly) { .ref = membs[i] });
        Insert(&m->elem->poly.fun.value[i], e, true);
    }
    Elem ret = Block(&q);
    str_swap(&namespace, &new);
    deq_char_free(&q);
    str_free(&new);
    return ret;
}

static Elem
Command(int argc, char* argv[])
{
    str s = str_init("");
    str_append(&s, "[");
    for(int i = 1; i < argc; i++)
    {
        str_append(&s, "\"");
        str_append(&s, argv[i]);
        str_append(&s, "\"");
        if(i < argc - 1)
            str_append(&s, ",");
    }
    str_append(&s, "]");
    deq_char q = Queue(s.value);
    str_free(&s);
    Elem e = Element(&q);
    deq_char_free(&q);
    return e;
}

static void
Program(const char* code)
{
    deq_char q = Queue(code);
    while(!deq_char_empty(&q))
    {
        str f = Read(&q, IsIdent);
        char n = Next(&q);
        if(n == '(')
            Fun(&q, &f);
        else
        {
            bool mut = Mutable(&q, &f);
            Define(&q, &f, mut);
            Match(&q, ';');
        }
        Spin(&q);
        str_free(&f);
    }
    deq_char_free(&q);
}

static void
Setup(void)
{
    line = 1;
    namespace = str_init("");
    db = set_Memb_init(Memb_compare);
    static Poly zero;
    Memb members[] = {
        Memb_init(true, str_init("true"),  Elem_init(BLN, (Poly) { .bln = true  })),
        Memb_init(true, str_init("false"), Elem_init(BLN, (Poly) { .bln = false })),
        Memb_init(true, str_init("null"),  Elem_init(NUL, zero)),
    };
    for(size_t i = 0; i < len(members); i++)
        set_Memb_insert(&db, members[i]);
}

static void
Teardown(void)
{
    str_free(&namespace);
    set_Memb_free(&db);
}

static int
Run(int argc, char* argv[], const char* code)
{
    Setup();
    Program(code);
    Elem args = Command(argc, argv);
    str entry = str_init("Main");
    vec_str params = vec_str_init();
    str a = str_init("argv");
    Insert(&a, args, true);
    set_Memb_node* node = Exists(&entry);
    Elem e = Call(&node->key, &params);
    Type t = I32;
    if(e->type != t)
        quit("entry `%s` expected return type `%s`\n", entry.value, TypeStr[t]);
    int32_t ret = e->poly.i32;
    Teardown();
    Elem_free(&e);
    str_free(&a);
    str_free(&entry);
    str_free(&namespace);
    vec_str_free(&params);
    return ret;
}

int
main(int argc, char* argv[])
{
    return Run(argc, argv,
        "Join(a, b)"
        "{"
            "if(false)"
            "{"
                "ret a + b;"
            "}"
            "else"
            "{"
                "if(false)"
                "{"
                    "ret a + a + b;"
                "}"
                "else"
                "{"
                    "ret a + a + a + b;"
                "}"
            "}"
        "}"

        "F()"
        "{"
        "}"

        "Test(g)"
        "{"
            "same := (((g == F)));"
        "}"

        "Main()"
        "{"
            "a := |i8| -1.3;"
            "b := |u8| 255 == <u8> a;"
            "Test(F);"
            "if(false)"
            "{"
                "ret |i32| 1;"
            "}"
            "else"
            "{"
                "ret |i32| 0;"
            "}"
        "}"
    );
}

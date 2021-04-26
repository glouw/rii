#include <stdio.h>
#include <stdbool.h>

#define P
#define T char
#include <que.h>

#include <str.h>

#define T str
#include <vec.h>

int line = 1;

#define quit(...) \
    printf("error: line %d: ", line), printf(__VA_ARGS__), exit(1)

#define TYPES X(I8) X(U8) \
    X(I16) X(U16) X(I32) X(U32) \
    X(I64) X(U64) X(F32) X(F64) \
    X(OBJ) X(ARR) X(STR) X(BLN) \
    X(BLK) X(REF) X(NUL) // ORDER DETERMINES CASTING CONVERSIONS.

#define TABLE_CAST(A, TYPE) \
    case  I8: A->poly.i8  = A->poly.TYPE; break; \
    case  U8: A->poly.u8  = A->poly.TYPE; break; \
    case I16: A->poly.i16 = A->poly.TYPE; break; \
    case U16: A->poly.u16 = A->poly.TYPE; break; \
    case I32: A->poly.i32 = A->poly.TYPE; break; \
    case U32: A->poly.u32 = A->poly.TYPE; break; \
    case I64: A->poly.i64 = A->poly.TYPE; break; \
    case U64: A->poly.u64 = A->poly.TYPE; break; \
    case F32: A->poly.f32 = A->poly.TYPE; break; \
    case F64: A->poly.f64 = A->poly.TYPE; break; \
    default: break;

#define TABLE_OPER(A, B, OPER) \
    case  U8: A->poly.u8  OPER B->poly.u8;  break; \
    case U16: A->poly.u16 OPER B->poly.u16; break; \
    case U32: A->poly.u32 OPER B->poly.u32; break; \
    case U64: A->poly.u64 OPER B->poly.u64; break; \
    case  I8: A->poly.i8  OPER B->poly.i8;  break; \
    case I16: A->poly.i16 OPER B->poly.i16; break; \
    case I32: A->poly.i32 OPER B->poly.i32; break; \
    case I64: A->poly.i64 OPER B->poly.i64; break; \
    case F32: A->poly.f32 OPER B->poly.f32; break; \
    case F64: A->poly.f64 OPER B->poly.f64; break;

#define X(A) A,
typedef enum { TYPES } Type;
#undef X

#define X(A) #A,
const char* TypeStr[] = { TYPES };
#undef X

typedef struct Val* Elem;

void Elem_free(Elem*);
Elem Elem_copy(Elem*);
#define T Elem
#include <vec.h>

typedef struct
{
    str str;
    Elem elem;
}
Memb;

void Memb_free(Memb*);
Memb Memb_copy(Memb*);
#define T Memb
#include <set.h>

Elem
Element(que_char*, set_Memb*);

Elem
Expression(que_char*, set_Memb*);

Elem
Term(que_char*, set_Memb*);

set_Memb
Object(que_char*, set_Memb*);

void
Eval(const char*, set_Memb*);

typedef struct
{
    str text;
    vec_str params;
    set_Memb captured;
}
Blk;

typedef union
{
    set_Memb obj;
    vec_Elem arr;
    str ref;
    str str;
    Blk blk;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
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

bool
Equal(str* a, const char* b)
{
    return str_compare(a, b) == 0;
}

void
Blk_Free(Blk* b)
{
    str_free(&b->text);
    vec_str_free(&b->params);
    set_Memb_free(&b->captured);
}

Blk
Blk_Copy(Blk* b)
{
    return (Blk) {
        str_copy(&b->text),
        vec_str_copy(&b->params),
        set_Memb_copy(&b->captured),
    };
}

Blk
Blk_Init(str text, vec_str params, set_Memb captured)
{
    return (Blk) { text, params, captured };
}

Elem
Elem_Init(Type type, Poly poly)
{
    Elem elem = malloc(sizeof(*elem));
    elem->type = type;
    elem->poly = poly;
    return elem;
}

void
Elem_free(Elem* e)
{
    Type type = (*e)->type;
    Poly poly = (*e)->poly;
    switch(type)
    {
    case OBJ: set_Memb_free(&poly.obj); break;
    case ARR: vec_Elem_free(&poly.arr); break;
    case STR: str_free(&poly.str); break;
    case BLK: Blk_Free(&poly.blk); break;
    case REF: str_free(&poly.ref); break;
    default:
        break;
    }
    free(*e);
}

Elem
Elem_copy(Elem* e)
{
    Type type = (*e)->type;
    Poly poly = (*e)->poly;
    Elem copy = malloc(sizeof(*copy));
    copy->type = type;
    switch(type)
    {
    case OBJ: copy->poly.obj = set_Memb_copy(&poly.obj); break;
    case ARR: copy->poly.arr = vec_Elem_copy(&poly.arr); break;
    case STR: copy->poly.str = str_copy(&poly.str); break;
    case BLK: copy->poly.blk = Blk_Copy(&poly.blk); break;
    case REF: copy->poly.ref = str_copy(&poly.ref); break;
    default:
        copy->poly = poly;
        break;
    }
    return copy;
}

void
Tabs(int tabs)
{
    int width = 4;
    while(tabs--)
        for(int i = 0; i < width; i++)
            putchar(' ');
}

void
Elem_write(Elem e, int tabs)
{
    int indent = tabs + 1;
    size_t index = 0;
    switch(e->type)
    {
    case OBJ:
        putchar('{');
        putchar('\n');
        foreach(set_Memb, &e->poly.obj, it)
        {
            Tabs(indent);
            printf("\"%s\" : ", it.ref->str.value);
            Elem_write(it.ref->elem, indent);
            index += 1;
            if(index < e->poly.obj.size)
                putchar(',');
            putchar('\n');
        }
        Tabs(tabs);
        putchar('}');
        break;
    case ARR:
        putchar('[');
        putchar('\n');
        foreach(vec_Elem, &e->poly.arr, it)
        {
            Tabs(indent);
            Elem_write(*it.ref, indent);
            index += 1;
            if(index < e->poly.arr.size)
                putchar(',');
            putchar('\n');
        }
        Tabs(tabs);
        putchar(']');
        break;
    case STR: printf("|str| %s", e->poly.str.value); break;
    case REF: printf("|ref| %s", e->poly.ref.value); break;
    case BLN: printf("|bln| %s", e->poly.bln ? "true" : "false"); break;
    case NUL: printf("|null| null"); break;
    case F64: printf("|f64| %lf", e->poly.f64); break;
    case F32: printf("|f32| %f",  e->poly.f32); break;
    case I64: printf("|i64| %ld", e->poly.i64); break;
    case I32: printf("|i32| %d",  e->poly.i32); break;
    case I16: printf("|i16| %d",  e->poly.i16); break;
    case  I8: printf("|i8 | %d",  e->poly.i8);  break;
    case U64: printf("|u64| %lu", e->poly.u64); break;
    case U32: printf("|u32| %u",  e->poly.u32); break;
    case U16: printf("|u16| %u",  e->poly.u16); break;
    case  U8: printf("|u8 | %u",  e->poly.u8);  break;
    case BLK: break;
    }
}

void
Elem_print(Elem e)
{
    Elem_write(e, 0);
    putchar('\n');
}

void
Memb_free(Memb* m)
{
    str_free(&m->str);
    Elem_free(&m->elem);
}


Memb
Memb_Init(str s, Elem e)
{
    return (Memb) { s, e };
}

Memb
Memb_copy(Memb* m)
{
    str s = str_copy(&m->str);
    Elem e = Elem_copy(&m->elem);
    return Memb_Init(s, e);
}

int
Memb_Compare(Memb* a, Memb* b)
{
    return str_key_compare(&a->str, &b->str);
}

que_char
Queue(const char* code)
{
    que_char q = que_char_init();
    for(const char* c = code; *c; c++)
        que_char_push(&q, *c);
    return q;
}

bool IsSpace(char c) { return c == ' ' || c == '\n'; }
bool IsLower(char c) { return c >= 'a' && c <= 'z'; }
bool IsUpper(char c) { return c >= 'A' && c <= 'Z'; }

bool
IsString(char c)
{
    return IsLower(c) || IsUpper(c) || IsSpace(c);
}

bool
IsDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool
IsNum(char c)
{
    return IsDigit(c) || c == '.' || c == '-' || c == '+';
}

bool
IsValidIdent(char c)
{
    return IsLower(c) || IsUpper(c) || c == '_';
}

bool
IsIdent(char c)
{
    return IsValidIdent(c) || IsDigit(c);
}

void
Pop(que_char* q)
{
    if(*que_char_front(q) == '\n')
        line += 1;
    que_char_pop(q);
}

void
Spin(que_char* q)
{
    while(que_char_empty(q) == false)
        if(IsSpace(*que_char_front(q)))
            Pop(q);
        else
            break;
}

char
Next(que_char* q)
{
    Spin(q);
    return *que_char_front(q);
}

void
Match(que_char* q, char c)
{
    char n = Next(q);
    if(n != c)
        quit("got '%c'; expected '%c'\n", n, c);
    Pop(q);
}

str
Read(que_char* q, bool clause(char))
{
    Spin(q);
    str s = str_init("");
    while(que_char_empty(q) == false)
    {
        char c = *que_char_front(q);
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

str
String(que_char* q)
{
    Match(q, '"');
    str s = Read(q, IsString);
    Match(q, '"');
    return s;
}

vec_Elem
Array(que_char* q, set_Memb* idents)
{
    vec_Elem es = vec_Elem_init();
    Match(q, '[');
    if(Next(q) != ']')
        while(true)
        {
            vec_Elem_push_back(&es, Expression(q, idents));
            if(Next(q) == ',')
                Match(q, ',');
            else
                break;
        }
    Match(q, ']');
    return es;
}

Type
CastType(que_char* q, char l, char r)
{
    Type type = { 0 };
    Match(q, l);
    str t = Read(q, IsIdent);
    if(Equal(&t, "u8"))  type =  U8; else
    if(Equal(&t, "u16")) type = U16; else
    if(Equal(&t, "u32")) type = U32; else
    if(Equal(&t, "u64")) type = U64; else
    if(Equal(&t, "i8"))  type =  I8; else
    if(Equal(&t, "i16")) type = I16; else
    if(Equal(&t, "i32")) type = I32; else
    if(Equal(&t, "i64")) type = I64; else
    if(Equal(&t, "f32")) type = F32; else
    if(Equal(&t, "f64")) type = F64; else quit("casting to type '%s' not supported\n", t.value);
    Match(q, r);
    str_free(&t);
    return type;
}

Poly
Poly_Basic(que_char* q, Type t)
{
    Poly p = { 0 };
    str s = Read(q, IsNum);
    switch(t)
    {
    default:
    case F64: p.f64 = strtod   (s.value, NULL); break;
    case F32: p.f32 = strtof   (s.value, NULL); break;
    case  U8: p.u8  = strtoul  (s.value, NULL, 10); break;
    case U16: p.u16 = strtoul  (s.value, NULL, 10); break;
    case U32: p.u32 = strtoul  (s.value, NULL, 10); break;
    case U64: p.u64 = strtoull (s.value, NULL, 10); break;
    case  I8: p.i8  = strtol   (s.value, NULL, 10); break;
    case I16: p.i16 = strtol   (s.value, NULL, 10); break;
    case I32: p.i32 = strtol   (s.value, NULL, 10); break;
    case I64: p.i64 = strtoll  (s.value, NULL, 10); break;
    }
    str_free(&s);
    return p;
}

Elem
Value(que_char* q)
{
    Type t = (Next(q) == '|') ? CastType(q, '|', '|') : F64;
    Poly p = Poly_Basic(q, t);
    return Elem_Init(t, p);
}

set_Memb_node*
Node(set_Memb* idents, str* s)
{
    return set_Memb_find(idents, (Memb) { .str = *s });
}

set_Memb_node*
Find(set_Memb* idents, str* s)
{
    set_Memb_node* node = Node(idents, s);
    if(node == NULL)
        quit("identifier '%s' not defined\n", s->value);
    return node;
}

void
Erase(set_Memb* idents, str* s)
{
    set_Memb_erase_node(idents, Node(idents, s));
}

void
Dupe(set_Memb* idents, str* s)
{
    if(Node(idents, s))
        quit("identifier '%s' already defined\n", s->value);
}

Elem
GetByRef(set_Memb* idents, str* s)
{
    set_Memb_node* node = Find(idents, s);
    return node->key.elem;
}

Elem
GetByVal(set_Memb* idents, str* s)
{
    Elem e = GetByRef(idents, s);
    return Elem_copy(&e);
}

Elem
Ident(que_char* q, set_Memb* idents)
{
    str s = Read(q, IsIdent);
    Elem e = GetByVal(idents, &s);
    str_free(&s);
    return e;
}

Elem
Element(que_char* q, set_Memb* idents)
{
    char n = Next(q);
    return (n == '"') ? Elem_Init(STR, (Poly) {.str = String(q)})
         : (n == '{') ? Elem_Init(OBJ, (Poly) {.obj = Object(q, idents)})
         : (n == '[') ? Elem_Init(ARR, (Poly) {.arr = Array(q, idents)})
         : IsValidIdent(n) ? Ident(q, idents)
         : Value(q);
}

void
Members(que_char* q, set_Memb* idents, set_Memb* m)
{
    while(true)
    {
        str s = String(q);
        Match(q, ':');
        Elem e = Expression(q, idents);
        set_Memb_insert(m, Memb_Init(s, e));
        if(Next(q) == ',')
            Match(q, ',');
        else
            break;
    }
}

set_Memb
Object(que_char* q, set_Memb* idents)
{
    set_Memb m = set_Memb_init(Memb_Compare);
    Match(q, '{');
    if(Next(q) != '}')
        Members(q, idents, &m);
    Match(q, '}');
    return m;
}

Elem
Parn(que_char* q, set_Memb* idents)
{
    Match(q, '(');
    Elem a = Expression(q, idents);
    Match(q, ')');
    return a;
}

bool
IsBasic(Elem e)
{
    return e->type >= I8 && e->type <= F64;
}

void
Cast(Elem e, Type t)
{
    if(IsBasic(e))
    {
        switch(e->type)
        {
        case  I8: switch(t) { TABLE_CAST(e,  i8) } break;
        case  U8: switch(t) { TABLE_CAST(e,  u8) } break;
        case I16: switch(t) { TABLE_CAST(e, i16) } break;
        case U16: switch(t) { TABLE_CAST(e, u16) } break;
        case I32: switch(t) { TABLE_CAST(e, i32) } break;
        case U32: switch(t) { TABLE_CAST(e, u32) } break;
        case I64: switch(t) { TABLE_CAST(e, i64) } break;
        case U64: switch(t) { TABLE_CAST(e, u64) } break;
        case F32: switch(t) { TABLE_CAST(e, f32) } break;
        case F64: switch(t) { TABLE_CAST(e, f64) } break;
        default: break;
        }
        e->type = t;
    }
}

Elem
Factor(que_char* q, set_Memb* idents)
{
    Type t = (Next(q) == '<') ? CastType(q, '<', '>') : NUL;
    Elem e = (Next(q) == '(') ? Parn(q, idents) : Term(q, idents);
    if(t != NUL)
        Cast(e, t);
    return e;
}

void
vec_Elem_zip(vec_Elem* a, vec_Elem* b, void Op(Elem, Elem))
{
    if(a->size != b->size)
        quit("array size mismatch - sizes were '%lu' and '%lu'", a->size, b->size);
    vec_Elem_it ia = vec_Elem_it_each(a);
    vec_Elem_it ib = vec_Elem_it_each(b);
    while(!ia.done && !ib.done)
    {
        Op(*ia.ref, *ib.ref);
        ia.step(&ia);
        ib.step(&ib);
    }
}

void
set_Memb_zip(set_Memb* a, set_Memb* b, void Op(Elem, Elem))
{
    foreach(set_Memb, b, it)
    {
        set_Memb_node* node = Node(a, &it.ref->str);
        if(node)
            Op(node->key.elem, it.ref->elem);
    }
}

void
Check(Elem a, Elem b)
{
    if(!IsBasic(a) || !IsBasic(b))
        if(a->type != b->type)
            quit("type mismatch - types were '%s' and '%s'\n",
                    TypeStr[a->type],
                    TypeStr[b->type]);
}

void
OperCast(Elem a, Elem b)
{
    if(b->type > a->type) Cast(a, b->type); else
    if(a->type > b->type) Cast(b, a->type);
}

void
OperPrep(Elem a, Elem b)
{
    Check(a, b);
    OperCast(a, b);
}

void
Mul(Elem a, Elem b)
{
    OperPrep(a, b);
    switch(a->type)
    {
    TABLE_OPER(a, b, *=)
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Mul); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Mul); break; 
    case STR:
    case REF:
    case BLN:
    case BLK:
    case NUL:
        quit("mul (*) not supported for '%s'\n", TypeStr[a->type]); 
        break; 
    }
}

void
Div(Elem a, Elem b)
{
    OperPrep(a, b);
    switch(a->type)
    {
    TABLE_OPER(a, b, /=)
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Div); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Div); break; 
    case STR:
    case REF: 
    case BLN: 
    case BLK: 
    case NUL:
        quit("div (/) not supported for '%s'\n", TypeStr[a->type]); 
        break; 
    }
}

void
Add(Elem a, Elem b)
{
    OperPrep(a, b);
    switch(a->type)
    {
    TABLE_OPER(a, b, +=)
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: str_append(&a->poly.str, str_c_str(&b->poly.str)); break; 
    case REF:
    case BLN:
    case BLK:
    case NUL:
        quit("add (+) not supported for '%s'\n", TypeStr[a->type]); 
        break; 
    }
}

void
Sub(Elem a, Elem b)
{
    OperPrep(a, b);
    switch(a->type)
    {
    TABLE_OPER(a, b, -=)
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Sub); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Sub); break; 
    case STR:
    case REF:
    case BLN:
    case BLK:
    case NUL:
        quit("sub (-) not supported for '%s'\n", TypeStr[a->type]); 
        break; 
    }
}

void
Deref(Elem* e, set_Memb* idents)
{
    while((*e)->type == REF) // DEPENDS HOW MANY FUNCTIONS DEEP.
    {
        Elem n = GetByVal(idents, &(*e)->poly.ref);
        Elem_free(e);
        *e = n;
    }
}

Elem
Term(que_char* q, set_Memb* idents)
{
    Elem a = Element(q, idents);
    Deref(&a, idents);
    while(true)
    {
        char n = Next(q);
        if(n == '*')
        {
            Match(q, '*');
            Elem b = Factor(q, idents);
            Deref(&b, idents);
            Mul(a, b);
            Elem_free(&b);
        }
        else
        if(n == '/')
        {
            Match(q, '/');
            Elem b = Factor(q, idents);
            Deref(&b, idents);
            Div(a, b);
            Elem_free(&b);
        }
        else
            break;
    }
    return a;
}

Elem
Expression(que_char* q, set_Memb* idents)
{
    Elem a = Factor(q, idents);
    Deref(&a, idents);
    while(true)
    {
        char n = Next(q);
        if(n == '+')
        {
            Match(q, '+');
            Elem b = Element(q, idents);
            Deref(&b, idents);
            Add(a, b);
            Elem_free(&b);
        }
        else
        if(n == '-')
        {
            Match(q, '-');
            Elem b = Element(q, idents);
            Deref(&b, idents);
            Sub(a, b);
            Elem_free(&b);
        }
        else
            break;
    }
    return a;
}

str
ReadBlock(que_char* q)
{
    int open = 1;
    str s = str_init("");
    Match(q, '{');
    while(open > 0)
    {
        char n = *que_char_front(q);
        if(n == '{')
            open += 1;
        else
        if(n == '}')
            open -= 1;
        str_push_back(&s, n);
        Pop(q);
    }
    str_pop_back(&s); // POP '}'
    return s;
}

vec_str
DeclParams(que_char* q)
{
    vec_str params = vec_str_init();
    Match(q, '(');
    if(Next(q) != ')')
        while(true)
        {
            str ident = Read(q, IsIdent);
            vec_str_push_back(&params, ident);
            if(Next(q) == ',')
                Match(q, ',');
            else
                break;
        }
    Match(q, ')');
    return params;
}

Elem
Lambda(que_char* q, set_Memb* idents)
{
    vec_str params = DeclParams(q);
    Match(q, '=');
    str b = ReadBlock(q);
    set_Memb c = set_Memb_copy(idents);
    Blk blk = Blk_Init(b, params, c);
    return Elem_Init(BLK, (Poly) { .blk = blk });
}

set_Memb
Setup(void)
{
    static Poly zero;
    set_Memb idents = set_Memb_init(Memb_Compare);
    Memb members[] = {
        Memb_Init(str_init("true"),  Elem_Init(BLN, (Poly) {.bln = true  })),
        Memb_Init(str_init("false"), Elem_Init(BLN, (Poly) {.bln = false })),
        Memb_Init(str_init("null"),  Elem_Init(NUL, zero)),
    };
    for(size_t i = 0; i < len(members); i++)
        set_Memb_insert(&idents, members[i]);
    return idents;
}

void
Args(que_char* q, set_Memb* idents, vec_str* params)
{
    Match(q, '(');
    int count = 0;
    if(Next(q) != ')')
        foreach(vec_str, params, it)
        {
            str real = Read(q, IsIdent);
            count += 1;
            Find(idents, &real);
            Elem e = Elem_Init(REF, (Poly) { .ref = real });
            str a = str_copy(it.ref);
            Memb m = Memb_Init(a, e);
            set_Memb_insert(idents, m);
            if(Next(q) == ',')
                Match(q, ',');
            else
                break;
        }
    int expect = params->size;
    if(count != expect)
        quit("expected '%d' args, got '%d' args", expect, count);
    Match(q, ')');
}

void
Block(const char* code, set_Memb* idents, set_Memb* old)
{
    Eval(code, idents);
    set_Memb diff = set_Memb_difference(idents, old);
    foreach(set_Memb, &diff, it)
    {
        printf("%s = ", it.ref->str.value);
        Elem_print(it.ref->elem);
        Erase(idents, &it.ref->str);
    }
    set_Memb_free(&diff);
}

void
Call(que_char* q, set_Memb* idents, str* s)
{
    set_Memb old = set_Memb_copy(idents);
    Elem e = GetByVal(idents, s);
    if(e->type != BLK)
        quit("type '%s' cannot be called\n", TypeStr[e->type]);
    Args(q, idents, &e->poly.blk.params);
    Block(e->poly.blk.text.value, idents, &old);
    set_Memb_free(&old);
    Elem_free(&e);
    // DISCARDS VALUE FOR NOW - MUST RETURN COMPUTED VALUE ONE DAY.
}

Elem
Assign(que_char* q, set_Memb* idents)
{
    Elem elem = { 0 };
    char n = Next(q);
    if(n == '(')
        elem = Lambda(q, idents);
    else
    if(n == '=')
    {
        Match(q, '=');
        elem = Expression(q, idents);
    }
    return elem;
}

void
Let(que_char* q, set_Memb* idents)
{
    str s = Read(q, IsIdent);
    Dupe(idents, &s);
    Elem e = Assign(q, idents);
    set_Memb_insert(idents, Memb_Init(s, e));
}

str
Alias(Memb* m, set_Memb* idents)
{
    while(m->elem->type == REF)
        m = &Find(idents, &m->elem->poly.ref)->key;
    return str_copy(&m->str);
}

void
Set(que_char* q, set_Memb* idents, str* s)
{
    Elem e = Assign(q, idents);
    Memb* m = &Find(idents, s)->key;
    str a = Alias(m, idents);
    Erase(idents, &a);
    set_Memb_insert(idents, Memb_Init(a, e));
}

void
Statement(que_char* q, set_Memb* idents)
{
    str action = Read(q, IsIdent);
    if(Equal(&action, "let"))
        Let(q, idents);
    else
    if(Next(q) == '(')
        Call(q, idents, &action);
    else
        Set(q, idents, &action);
    Match(q, ';');
    str_free(&action);
}

void
Body(que_char* q, set_Memb* idents)
{
    while(que_char_empty(q) == false)
    {
        Statement(q, idents);
        Spin(q);
    }
}

void
Eval(const char* code, set_Memb* idents)
{
    que_char q = Queue(code);
    Body(&q, idents);
    que_char_free(&q);
}

void
Execute(const char* code)
{
    set_Memb idents = Setup();
    Eval(code, &idents);
    set_Memb_free(&idents);
}

int
main(void)
{
    Execute(
        "let global = 1.0;"
        "let main() = {"
            "let b = 1.0;"
        "};"
        "let a = 1.0;"
        "main();"
    );
}

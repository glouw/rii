#include <stdio.h>
#include <stdbool.h>

#define P
#define T char
#include <que.h>

#include <str.h>

int line = 0;

#define quit(...) printf("error: line %d: ", line), printf(__VA_ARGS__), exit(1)

// ORDER DICTATES TYPE CONVERSION
#define TYPES \
    X(I8)  \
    X(U8)  \
    X(I16) \
    X(U16) \
    X(I32) \
    X(U32) \
    X(I64) \
    X(U64) \
    X(F32) \
    X(F64) \
    X(OBJ) \
    X(ARR) \
    X(STR) \
    X(BLN) \
    X(NUL)

#define X(A) A,
typedef enum { TYPES } Type;
#undef X

#define X(A) #A,
const char* TypeStr[] = { TYPES };
#undef X

typedef struct Val* Elem;

bool
Equal(str* a, const char* b)
{
    return str_compare(a, b) == 0;
}

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

typedef union
{
    set_Memb obj;
    vec_Elem arr;
    str str;
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
    default: // OKAY, TYPES ARE POD.
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
    default: // OKAY, TYPES ARE POD.
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
    case STR: printf("%s", e->poly.str.value); break;
    case BLN: printf("%s", e->poly.bln ? "true" : "false"); break;
    case NUL: printf("null"); break;
    case F64: printf("%lf", e->poly.f64); break;
    case F32: printf("%f",  e->poly.f32); break;
    case I64: printf("%ld", e->poly.i64); break;
    case I32: printf("%d",  e->poly.i32); break;
    case I16: printf("%d",  e->poly.i16); break;
    case  I8: printf("%d",  e->poly.i8);  break;
    case U64: printf("%lu", e->poly.u64); break;
    case U32: printf("%u",  e->poly.u32); break;
    case U16: printf("%u",  e->poly.u16); break;
    case  U8: printf("%u",  e->poly.u8);  break;
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
Memb_copy(Memb* m)
{
    return (Memb) { str_copy(&m->str), Elem_copy(&m->elem) };
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

Elem Element(que_char*, set_Memb*);

vec_Elem
Array(que_char* q, set_Memb* idents)
{
    vec_Elem es = vec_Elem_init();
    Match(q, '[');
    if(Next(q) != ']')
        while(true)
        {
            vec_Elem_push_back(&es, Element(q, idents));
            if(Next(q) == ',')
                Match(q, ',');
            else
                break;
        }
    Match(q, ']');
    return es;
}

Elem
Number(que_char* q)
{
    char n = Next(q);
    Type type = F64;
    if(n == '<')
    {
        Match(q, '<');
        str t = Read(q, IsIdent);
        if(Equal(&t, "u8"))  type =  U8;
        if(Equal(&t, "u16")) type = U16;
        if(Equal(&t, "u32")) type = U32;
        if(Equal(&t, "u64")) type = U64;
        if(Equal(&t, "i8"))  type =  I8;
        if(Equal(&t, "i16")) type = I16;
        if(Equal(&t, "i32")) type = I32;
        if(Equal(&t, "i64")) type = I64;
        if(Equal(&t, "f32")) type = F32;
        if(Equal(&t, "f64")) type = F64;
        Match(q, '>');
        str_free(&t);
    }
    str s = Read(q, IsNum);
    Poly p;
    switch(type)
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
    return Elem_Init(type, p);
}

Elem
Ident(que_char* q, set_Memb* idents)
{
    str s = Read(q, IsIdent);
    set_Memb_node* node = set_Memb_find(idents, (Memb) { .str = s });
    if(node == NULL)
        quit("identifier '%s' not defined\n", s.value);
    str_free(&s);
    return Elem_copy(&node->key.elem);
}

set_Memb Object(que_char*, set_Memb* idents);

Elem
Element(que_char* q, set_Memb* idents)
{
    char n = Next(q);
    return (n == '"') ? Elem_Init(STR, (Poly) {.str = String(q)})
         : (n == '{') ? Elem_Init(OBJ, (Poly) {.obj = Object(q, idents)})
         : (n == '[') ? Elem_Init(ARR, (Poly) {.arr = Array(q, idents)})
         : IsValidIdent(n) ? Ident(q, idents)
         : Number(q);
}

void
Members(que_char* q, set_Memb* idents, set_Memb* m)
{
    while(true)
    {
        str s = String(q);
        Match(q, ':');
        set_Memb_insert(m, (Memb) { s, Element(q, idents) });
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


Elem Expression(que_char*, set_Memb*);

Elem Term(que_char*, set_Memb*);

Elem
Parn(que_char* q, set_Memb* idents)
{
    Match(q, '(');
    Elem a = Expression(q, idents);
    Match(q, ')');
    return a;
}

Elem
Factor(que_char* q, set_Memb* idents)
{
    return (Next(q) == '(')
        ? Parn(q, idents)
        : Term(q, idents);
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
        set_Memb_node* node = set_Memb_find(a, (Memb) { .str = it.ref->str });
        if(node)
            Op(node->key.elem, it.ref->elem);
    }
}

void
Mul(Elem a, Elem b)
{
    switch(a->type)
    {
    case  U8: a->poly.u8  *= b->poly.u8;  break; 
    case U16: a->poly.u16 *= b->poly.u16; break; 
    case U32: a->poly.u32 *= b->poly.u32; break; 
    case U64: a->poly.u64 *= b->poly.u64; break; 
    case  I8: a->poly.i8  *= b->poly.i8;  break; 
    case I16: a->poly.i16 *= b->poly.i16; break; 
    case I32: a->poly.i32 *= b->poly.i32; break; 
    case I64: a->poly.i64 *= b->poly.i64; break; 
    case F32: a->poly.f32 *= b->poly.f32; break; 
    case F64: a->poly.f64 *= b->poly.f64; break; 
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Mul); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Mul); break; 
    case STR: quit("mul (*) not supported for strings\n"); break; 
    case BLN: quit("mul (*) not supported for booleans\n"); break; 
    case NUL: quit("mul (*) not supported for null types\n"); break; 
    }
}

void
Div(Elem a, Elem b)
{
    switch(a->type)
    {
    case  U8: a->poly.u8  /= b->poly.u8;  break; 
    case U16: a->poly.u16 /= b->poly.u16; break; 
    case U32: a->poly.u32 /= b->poly.u32; break; 
    case U64: a->poly.u64 /= b->poly.u64; break; 
    case  I8: a->poly.i8  /= b->poly.i8;  break; 
    case I16: a->poly.i16 /= b->poly.i16; break; 
    case I32: a->poly.i32 /= b->poly.i32; break; 
    case I64: a->poly.i64 /= b->poly.i64; break; 
    case F32: a->poly.f32 /= b->poly.f32; break; 
    case F64: a->poly.f64 /= b->poly.f64; break; 
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Div); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Div); break; 
    case STR: quit("div (/) not supported for strings\n"); break; 
    case BLN: quit("div (/) not supported for booleans\n"); break; 
    case NUL: quit("div (/) not supported for null types\n"); break; 
    }
}

void
Convert(Elem a, Type t)
{
#define TABLE(X) \
    switch(t) { \
    case  I8: a->poly.i8  = a->poly.X; break; \
    case  U8: a->poly.u8  = a->poly.X; break; \
    case I16: a->poly.i16 = a->poly.X; break; \
    case U16: a->poly.u16 = a->poly.X; break; \
    case I32: a->poly.i32 = a->poly.X; break; \
    case U32: a->poly.u32 = a->poly.X; break; \
    case I64: a->poly.i64 = a->poly.X; break; \
    case U64: a->poly.u64 = a->poly.X; break; \
    case F32: a->poly.f32 = a->poly.X; break; \
    case F64: a->poly.f64 = a->poly.X; break; \
    default: break; \
    }
    switch(a->type)
    {
    case  I8: TABLE(i8); break;
    case  U8: TABLE(u8); break;
    case I16: TABLE(i16); break;
    case U16: TABLE(u16); break;
    case I32: TABLE(i32); break;
    case U32: TABLE(u32); break;
    case I64: TABLE(i64); break;
    case U64: TABLE(u64); break;
    case F32: TABLE(f32); break;
    case F64: TABLE(f64); break;
    default: break;
    }
#undef TABLE
}

void
Add(Elem a, Elem b)
{
    switch(a->type)
    {
    case  U8: case U16: case U32: case U64:
    case  I8: case I16: case I32: case I64:
    case F32: case F64:
        break;
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Add); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Add); break; 
    case STR: str_append(&a->poly.str, str_c_str(&b->poly.str)); break; 
    case BLN: quit("add (+) not supported for booleans\n"); break; 
    case NUL: quit("add (+) not supported for null types\n"); break; 
    }
}

void
Sub(Elem a, Elem b)
{
    switch(a->type)
    {
    case  U8: a->poly.u8  -= b->poly.u8;  break; 
    case U16: a->poly.u16 -= b->poly.u16; break; 
    case U32: a->poly.u32 -= b->poly.u32; break; 
    case U64: a->poly.u64 -= b->poly.u64; break; 
    case  I8: a->poly.i8  -= b->poly.i8;  break; 
    case I16: a->poly.i16 -= b->poly.i16; break; 
    case I32: a->poly.i32 -= b->poly.i32; break; 
    case I64: a->poly.i64 -= b->poly.i64; break; 
    case F32: a->poly.f32 -= b->poly.f32; break; 
    case F64: a->poly.f64 -= b->poly.f64; break; 
    case ARR: vec_Elem_zip(&a->poly.arr, &b->poly.arr, Sub); break; 
    case OBJ: set_Memb_zip(&a->poly.obj, &b->poly.obj, Sub); break; 
    case STR: quit("sub (-) not supported for strings\n"); break; 
    case BLN: quit("sub (-) not supported for booleans\n"); break; 
    case NUL: quit("sub (-) not supported for null types\n"); break; 
    }
}

bool
IsBasic(Elem e)
{
    return e->type ==  I8 || e->type == I16 || e->type == I32 || e->type == I64
        || e->type ==  U8 || e->type == U16 || e->type == U32 || e->type == U64
        || e->type == F32 || e->type == F64;
}

void
Check(Elem a, Elem b)
{
    if(IsBasic(a) && IsBasic(b)) // Two basic types are compatible with math operations.
        return;
    if(a->type != b->type)
        quit("type mismatch - types were '%s' and '%s'\n",
            TypeStr[a->type],
            TypeStr[b->type]);
}

Elem
Term(que_char* q, set_Memb* idents)
{
    Elem a = Element(q, idents);
    while(true)
    {
        char n = Next(q);
        if(n == '*')
        {
            Match(q, '*');
            Elem b = Factor(q, idents);
            Check(a, b);
            Mul(a, b);
            Elem_free(&b);
        }
        else
        if(n == '/')
        {
            Match(q, '/');
            Elem b = Factor(q, idents);
            Check(a, b);
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
    while(true)
    {
        char n = Next(q);
        if(n == '+')
        {
            Match(q, '+');
            Elem b = Element(q, idents);
            Check(a, b);
            Add(a, b);
            Elem_free(&b);
        }
        else
        if(n == '-')
        {
            Match(q, '-');
            Elem b = Element(q, idents);
            Check(a, b);
            Sub(a, b);
            Elem_free(&b);
        }
        else
            break;
    }
    return a;
}

void
Let(que_char* q, set_Memb* idents)
{
    str s = Read(q, IsIdent);
    Match(q, '=');
    Elem e = Expression(q, idents);
    set_Memb_insert(idents, (Memb) { s, e });
    Match(q, ';');
}

set_Memb
Setup(void)
{
    static Poly zero;
    set_Memb idents = set_Memb_init(Memb_Compare);
    Memb members[] = {
        { str_init("true"),  Elem_Init(BLN, (Poly) {.bln = true  }) },
        { str_init("false"), Elem_Init(BLN, (Poly) {.bln = false }) },
        { str_init("null"),  Elem_Init(NUL, zero) },
    };
    for(size_t i = 0; i < len(members); i++)
        set_Memb_insert(&idents, members[i]);
    return idents;
}

void
Execute(const char* code)
{
    set_Memb idents = Setup();
    que_char q = Queue(code);
    while(que_char_empty(&q) == false)
    {
        str action = Read(&q, IsIdent);
        if(Equal(&action, "let"))
            Let(&q, &idents);
        str_free(&action);
        Spin(&q);
    }
    foreach(set_Memb, &idents, it)
        Elem_print(it.ref->elem);
    que_char_free(&q);
    set_Memb_free(&idents);
}

int
main(void)
{
    Execute(
        "let Z = <u8> 255 - 1.0;"
    );
}

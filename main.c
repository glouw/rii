#include <stdbool.h>
#include <stdio.h>

#include <str.h>
#define T str
#include <vec.h>

#define P
#define T char
#include <deq.h>

int line = 1;

str namespace;

#define quit(...) \
    printf("error: line %d: ", line), printf(__VA_ARGS__), exit(1)

#define TYPES X(F64) X(STR) X(FUN) X(REF) X(OBJ) X(ARR) X(NUL)

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
    bool constant;
}
Memb;

void Memb_free(Memb*);
Memb Memb_copy(Memb*);
#define T Memb
#include <set.h>

set_Memb db;

Elem
Call(Memb*, vec_str*);

Elem
Expression(deq_char*);

typedef union
{
    set_Memb obj;
    vec_Elem arr;
    Memb* ref;
    str str;
    vec_str fun;
    double f64;
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
    case OBJ:
        set_Memb_free(&poly.obj);
        break;
    case ARR:
        vec_Elem_free(&poly.arr);
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

Elem
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
        copy->poly.arr = vec_Elem_copy(&poly.arr);
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

void
Memb_free(Memb* m)
{
    str_free(&m->str);
    Elem_free(&m->elem);
}

Memb
Memb_Init(str s, Elem e, bool constant)
{
    return (Memb) { s, e, constant };
}

Memb
Memb_copy(Memb* m)
{
    Elem e = Elem_copy(&m->elem);
    return Memb_Init(str_copy(&m->str), e, m->constant);
}

int
Memb_Compare(Memb* a, Memb* b)
{
    return str_key_compare(&a->str, &b->str);
}

deq_char
Queue(const char* code)
{
    deq_char q = deq_char_init();
    for(const char* c = code; *c; c++)
        deq_char_push_back(&q, *c);
    return q;
}

void
Unpop(deq_char* q, char c)
{
    if(c == '\n')
        line -= 1;
    deq_char_push_front(q, c);
}

void
Requeue(deq_char* q, str* s)
{
    for(int i = s->size - 1; i >= 0; i--)
        Unpop(q, s->value[i]);
}

bool
IsSpace(char c)
{
    return c == ' '
        || c == '\n';
}

bool
IsLower(char c)
{
    return c >= 'a'
        && c <= 'z';
}

bool
IsUpper(char c)
{
    return c >= 'A'
        && c <= 'Z';
}

bool
IsDigit(char c)
{
    return c >= '0'
        && c <= '9';
}

bool
IsNum(char c)
{
    return IsDigit(c)
        || c == '.'
        || c == '-'
        || c == '+';
}

bool
IsString(char c) // WHAT IS A STRING?
{
    return IsLower(c)
        || IsUpper(c)
        || IsSpace(c)
        || IsDigit(c);
}

bool
IsIdent(char c)
{
    return IsLower(c)
        || IsUpper(c)
        || IsDigit(c)
        || c == '_';
}

void
Pop(deq_char* q)
{
    if(*deq_char_front(q) == '\n')
        line += 1;
    deq_char_pop_front(q);
}

void
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

void
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

char
Next(deq_char* q)
{
    Spin(q);
    return *deq_char_front(q);
}

void
Match(deq_char* q, char c)
{
    char n = Next(q);
    if(n != c)
        quit("got '%c'; expected '%c'\n", n, c);
    Pop(q);
}

str
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

str
DefBlock(deq_char* q)
{
    int open = 0;
    str b = str_init("");
    do {
        char n = *deq_char_front(q);
        if(n == '{') open++;
        if(n == '}') open--;
        str_push_back(&b, n);
        Pop(q);
    }
    while(open > 0);
    return b;
}

vec_str
Params(deq_char* q)
{
    vec_str p = vec_str_init();
    Match(q, '(');
    while(Next(q) != ')')
    {
        str s = Read(q, IsIdent);
        vec_str_push_back(&p, s);
        if(Next(q) == ',')
            Match(q, ',');
    }
    Match(q, ')');
    return p;
}

str
Prefix(str* s, str* with)
{
    str n = str_copy(with);
    if(n.size > 0)
        str_append(&n, ".");
    str_append(&n, s->value);
    return n;
}

void
Insert(str* s, Elem e, bool constant)
{
    str n = Prefix(s, &namespace);
    Memb m = Memb_Init(n, e, constant);
    set_Memb_insert(&db, m);
}

void
Fun(deq_char* q)
{
    str f = Read(q, IsIdent);
    vec_str p = Params(q);
    vec_str_push_back(&p, f);
    vec_str_push_back(&p, DefBlock(q));
    Insert(&f, Elem_Init(FUN, (Poly) { .fun = p }), true);
}

set_Memb_node*
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

void
Erase(str* s)
{
    set_Memb_node* node = Find(s);
    set_Memb_erase_node(&db, node);
}

set_Memb_node*
Exists(str* s)
{
    set_Memb_node* node = Find(s);
    if(node == NULL)
        quit("identifier '%s' not defined\n", s->value);
    return node;
}

Memb*
Deref(Memb* m)
{
    Memb* other = m->elem->poly.ref;
    while(other->elem->type == REF)
        other = other->elem->poly.ref;
    return other;
}

Memb*
Resolve(str* s)
{
    Memb* m = &Exists(s)->key;
    return (m->elem->type == REF) ? Deref(m) : m;
}

Elem
Ident(deq_char* q)
{
    str s = Read(q, IsIdent);
    Memb* m = Resolve(&s);
    Elem e;
    if(Next(q) == '(')
    {
        vec_str p = Params(q);
        e = Call(m, &p);
        vec_str_free(&p);
    }
    else
        e = Elem_copy(&m->elem);
    str_free(&s);
    return e;
}

str
String(deq_char* q)
{
    Match(q, '"');
    str s = Read(q, IsString);
    Match(q, '"');
    return s;
}

Type
LoadType(deq_char* q, char l, char r)
{
    Type type = { 0 };
    Match(q, l);
    str t = Read(q, IsIdent);
    // TODO: ADD MORE TYPES.
    if(Equal(&t, "f64"))
        type = F64;
    else
        quit("casting to type '%s' not supported\n", t.value);
    Match(q, r);
    str_free(&t);
    return type;
}

Poly
Load(deq_char* q, Type t)
{
    Poly p = { 0 };
    str s = Read(q, IsNum);
    // TODO: ADD MORE TYPES.
    switch(t)
    {
    default:
    case F64:
        p.f64 = strtod(s.value, NULL);
        break;
    }
    str_free(&s);
    return p;
}

Elem
Value(deq_char* q)
{
    Type t = (Next(q) == '|') ? LoadType(q, '|', '|') : F64;
    Poly p = Load(q, t);
    return Elem_Init(t, p);
}

vec_Elem
Array(deq_char* q)
{
    vec_Elem es = vec_Elem_init();
    Match(q, '[');
    if(Next(q) != ']')
        while(true)
        {
            Elem e = Expression(q);
            vec_Elem_push_back(&es, e);
            if(Next(q) == ',')
                Match(q, ',');
            else
                break;
        }
    Match(q, ']');
    return es;
}

void
Members(deq_char* q, set_Memb* m)
{
    while(true)
    {
        str s = String(q);
        Match(q, ':');
        Elem e = Expression(q);
        set_Memb_insert(m, Memb_Init(s, e, true));
        if(Next(q) == ',')
            Match(q, ',');
        else
            break;
    }
}

set_Memb
Object(deq_char* q)
{
    set_Memb m = set_Memb_init(Memb_Compare);
    Match(q, '{');
    if(Next(q) != '}')
        Members(q, &m);
    Match(q, '}');
    return m;
}

void
vec_Elem_zip(vec_Elem* a, vec_Elem* b, void Op(Elem, Elem))
{
    if(a->size != b->size)
        quit("array size mismatch - sizes were `%lu` and `%lu`", a->size, b->size);
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
        Memb m = { .str = it.ref->str };
        set_Memb_node* node = set_Memb_find(a, m);
        if(node)
            Op(node->key.elem, it.ref->elem);
    }
}

void
Check(Elem a, Elem b)
{
    if(a->type != b->type)
        quit("type mismatch - types were '%s' and '%s'\n",
                TypeStr[a->type],
                TypeStr[b->type]);
}

void
Mul(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR:
        vec_Elem_zip(&a->poly.arr, &b->poly.arr, Mul);
        break; 
    case OBJ:
        set_Memb_zip(&a->poly.obj, &b->poly.obj, Mul);
        break; 
    case F64:
        a->poly.f64 *= b->poly.f64;
        break;
    case STR:
    case FUN:
    case NUL:
    case REF:
        quit("mul (*) not supported for types `%s` and `%s`\n",
                TypeStr[a->type], TypeStr[b->type]); 
        break; 
    }
}

void
Div(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR:
        vec_Elem_zip(&a->poly.arr, &b->poly.arr, Div);
        break; 
    case OBJ:
        set_Memb_zip(&a->poly.obj, &b->poly.obj, Div);
        break; 
    case F64:
        a->poly.f64 /= b->poly.f64;
        break;
    case STR:
    case FUN:
    case NUL:
    case REF: 
        quit("div (/) not supported for types `%s` and `%s`\n",
                TypeStr[a->type], TypeStr[b->type]); 
        break; 
    }
}

void
Add(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR:
        vec_Elem_zip(&a->poly.arr, &b->poly.arr, Add);
        break; 
    case OBJ:
        set_Memb_zip(&a->poly.obj, &b->poly.obj, Add);
        break; 
    case STR:
        str_append(&a->poly.str, str_c_str(&b->poly.str));
        break; 
    case F64:
        a->poly.f64 += b->poly.f64;
        break;
    case FUN:
    case NUL:
    case REF:
        quit("add (+) not supported for types `%s` and `%s`\n",
                TypeStr[a->type], TypeStr[b->type]); 
        break; 
    }
}

void
Sub(Elem a, Elem b)
{
    Check(a, b);
    switch(a->type)
    {
    case ARR:
        vec_Elem_zip(&a->poly.arr, &b->poly.arr, Sub);
        break; 
    case OBJ:
        set_Memb_zip(&a->poly.obj, &b->poly.obj, Sub);
        break; 
    case F64:
        a->poly.f64 -= b->poly.f64;
        break;
    case STR:
    case FUN:
    case NUL:
    case REF:
        quit("sub (-) not supported for types `%s` and `%s`\n",
                TypeStr[a->type], TypeStr[b->type]); 
        break; 
    }
}

Elem
Element(deq_char* q)
{
    char n = Next(q);
    return (n == '"') ? Elem_Init(STR, (Poly) {.str = String(q)})
         : (n == '{') ? Elem_Init(OBJ, (Poly) {.obj = Object(q)})
         : (n == '[') ? Elem_Init(ARR, (Poly) {.arr = Array(q)})
         : IsDigit(n) ? Value(q)
         : Ident(q);
}

Elem
Parn(deq_char* q)
{
    Match(q, '(');
    Elem a = Expression(q);
    Match(q, ')');
    return a;
}

Elem
Factor(deq_char* q)
{
    return (Next(q) == '(') ? Parn(q) : Element(q);
}

Elem
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

Elem
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
            break;
    }
    return a;
}

void
Tabs(int tabs)
{
    int width = 4;
    while(tabs--)
        for(int i = 0; i < width; i++)
            putchar(' ');
}

char*
Code(Elem e)
{
    vec_str* v = &e->poly.fun;
    return vec_str_back(v)->value;
}

size_t
Arguments(Elem e)
{
    vec_str* v = &e->poly.fun;
    return v->size - 2; // CODE AND LABEL.
}

str*
Label(Elem e)
{
    vec_str* v = &e->poly.fun;
    return &v->value[Arguments(e)];
}

void
Elem_write(Elem e, int tabs);

void
Obj_write(Elem e, int indent)
{
    size_t index = 0;
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
    Tabs(indent - 1);
    putchar('}');
}

void
Arr_write(Elem e, int indent)
{
    size_t index = 0;
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
    Tabs(indent - 1);
    putchar(']');
}

void
Elem_write(Elem e, int tabs)
{
    int indent = tabs + 1;
    switch(e->type)
    {
    case OBJ:
        Obj_write(e, indent);
        break;
    case ARR:
        Arr_write(e, indent);
        break;
    case STR:
        printf("|str| \"%s\"", e->poly.str.value);
        break;
    case REF:
        printf("|ref| %p ", (void*) e->poly.ref->elem);
        Elem_write(e->poly.ref->elem, 0);
        break;
    case NUL:
        printf("|null| null");
        break;
    case F64:
        printf("|f64| %lf", e->poly.f64);
        break;
    case FUN:
        printf("|fun| %s", Label(e)->value);
        break;
    }
}

void
Elem_print(Elem e)
{
    Elem_write(e, 0);
    putchar('\n');
}

void
Define(deq_char* q, str* n, bool mut)
{
    Match(q, ':');
    Match(q, '=');
    if(Find(n))
        quit("`%s` already defined", n->value);
    Elem e = Expression(q);
    Insert(n, e, mut ? false : true);
}

void
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

void
Block(deq_char* q)
{
    set_Memb old = set_Memb_copy(&db);
    Match(q, '{');
    while(Next(q) != '}')
    {
        str s = Read(q, IsIdent);
        bool mut = Equal(&s, "mut");
        if(mut)
        {
            str_free(&s);
            s = Read(q, IsIdent);
        }
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
        str_free(&s);
    }
    Match(q, '}');
    set_Memb diff = set_Memb_difference(&db, &old);
    foreach(set_Memb, &diff, it)
    {
        printf("FREE %s: ", it.ref->str.value);
        Elem_print(it.ref->elem);
    }
    foreach(set_Memb, &diff, it)
        Erase(&it.ref->str);
    set_Memb_free(&old);
    set_Memb_free(&diff);
}

Elem
Call(Memb* m, vec_str* args)
{
    if(m->elem->type != FUN)
        quit("expected function\n");
    int exp = Arguments(m->elem);
    int got = args->size;
    if(got != exp)
        quit("`%s()` got %d args but expected %d\n", m->str.value, got, exp);
    deq_char q = Queue(Code(m->elem));
    str new = str_copy(&m->str);
    Memb* membs[got];
    int index = 0;
    foreach(vec_str, args, it)
        membs[index++] = &Exists(it.ref)->key;
    str_swap(&namespace, &new);
    for(int i = 0; i < got; i++)
    {
        Elem e = Elem_Init(REF, (Poly) { .ref = membs[i] });
        Insert(&m->elem->poly.fun.value[i], e, true);
    }
    Block(&q);
    str_swap(&namespace, &new);
    deq_char_free(&q);
    str_free(&new);
    static Poly zero;
    return Elem_Init(NUL, zero);
}

Elem
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

int
main(int argc, char* argv[])
{
    namespace = str_init("");
    const char* code =
        "foo(A)"
        "{"
        "}"

        "inf()"
        "{"
            "inf();"
        "}"

        "bar(A)"
        "{"
            "foo(A);"
        "}"

        "g(A, a)"
        "{"
            "A(a);"
        "}"

        "# This is a comment\n"
        "main()"
        "{"
            "mut b := 1;"
        "}";
    deq_char q = Queue(code);
    db = set_Memb_init(Memb_Compare);
    while(!deq_char_empty(&q))
    {
        Fun(&q);
        Spin(&q);
    }
    str entry = str_init("main");
    vec_str params = vec_str_init();
    str a = str_init("argv");
    Elem cmd = Command(argc, argv);
    Insert(&a, cmd, true);
    set_Memb_node* node = Exists(&entry);
    Elem e = Call(&node->key, &params);
    str_free(&a);
    str_free(&entry);
    set_Memb_free(&db);
    deq_char_free(&q);
    str_free(&namespace);
    vec_str_free(&params);
    Elem_free(&e); // CAN RETURN TO OS, BE NICE FOR TESTING.
}

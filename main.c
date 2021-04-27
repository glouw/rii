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
}
Memb;

void Memb_free(Memb*);
Memb Memb_copy(Memb*);
#define T Memb
#include <set.h>

set_Memb db;

Elem
Call(str*, vec_str*);

Elem
Element(deq_char*);

Elem
Expression(deq_char*);

Elem
Term(deq_char*);

set_Memb
Object(deq_char*);

typedef union
{
    set_Memb obj;
    vec_Elem arr;
    Elem* ref;
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
Memb_Init(str s, Elem e)
{
    return (Memb) { s, e };
}

Memb
Memb_copy(Memb* m)
{
    return Memb_Init(str_copy(&m->str), Elem_copy(&m->elem));
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
Requeue(deq_char* q, str* s)
{
    for(int i = s->size - 1; i >= 0; i--)
        deq_char_push_front(q, s->value[i]);
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
IsString(char c)
{
    return IsLower(c)
        || IsUpper(c)
        || IsSpace(c);
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
IsValidIdent(char c)
{
    return IsLower(c)
        || IsUpper(c)
        || c == '_';
}

bool
IsIdent(char c)
{
    return IsValidIdent(c)
        || IsDigit(c);
}

void
Pop(deq_char* q)
{
    if(*deq_char_front(q) == '\n')
        line += 1;
    deq_char_pop_front(q);
}

void
Unpop(deq_char* q, char c)
{
    if(c  == '\n')
        line -= 1;
    deq_char_push_front(q, c);
}

void
Spin(deq_char* q)
{
    while(!deq_char_empty(q))
        if(IsSpace(*deq_char_front(q)))
            Pop(q);
        else
            break;
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
    str_append(&n, ".");
    str_append(&n, s->value);
    return n;
}

void
Insert(str* s, Elem e)
{
    str n = Prefix(s, &namespace);
    set_Memb_insert(&db, Memb_Init(n, e));
}

void
Fun(deq_char* q)
{
    str f = Read(q, IsIdent);
    vec_str p = Params(q);
    vec_str_push_back(&p, DefBlock(q));
    set_Memb_insert(&db, Memb_Init(f, Elem_Init(FUN, (Poly) { .fun = p }))); // COULD USE INSERT?
}

set_Memb_node*
Find(str* s)
{
    str n = Prefix(s, &namespace);
    set_Memb_node* local = set_Memb_find(&db, (Memb) { .str = n });
    str_free(&n);
    if(local)
        return local;
    return set_Memb_find(&db, (Memb) { .str = *s });
}

void
Erase(str* s)
{
    set_Memb_erase_node(&db, Find(s));
}

set_Memb_node*
Exists(str* s)
{
    set_Memb_node* node = Find(s);
    if(node == NULL)
        quit("identifier '%s' not defined\n", s->value);
    return node;
}

Elem*
Deref(Elem e)
{
    Elem* other = e->poly.ref;
    while((*other)->type == REF)
        other = (*other)->poly.ref;
    return other;
}

Elem
Ident(deq_char* q)
{
    str s = Read(q, IsIdent);
    if(Next(q) == '(')
    {
        vec_str p = Params(q);
        Elem e = Call(&s, &p);
        vec_str_free(&p);
        str_free(&s);
        return e;
    }
    else
    {
        Elem* ee = &Exists(&s)->key.elem;
        if((*ee)->type == REF)
            ee = Deref(*ee);
        Elem e = Elem_copy(ee);
        str_free(&s);
        return e;
    }
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
            vec_Elem_push_back(&es, Expression(q));
            if(Next(q) == ',')
                Match(q, ',');
            else
                break;
        }
    Match(q, ']');
    return es;
}

Elem
Element(deq_char* q)
{
    char n = Next(q);
    return (n == '"') ? Elem_Init(STR, (Poly) {.str = String(q)})
         : (n == '{') ? Elem_Init(OBJ, (Poly) {.obj = Object(q)})
         : (n == '[') ? Elem_Init(ARR, (Poly) {.arr = Array(q)})
         : IsValidIdent(n) ? Ident(q)
         : Value(q);
}

void
Members(deq_char* q, set_Memb* m)
{
    while(true)
    {
        str s = String(q);
        Match(q, ':');
        Elem e = Expression(q);
        set_Memb_insert(m, Memb_Init(s, e));
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
    return (Next(q) == '(') ? Parn(q) : Term(q);
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
Term(deq_char* q)
{
    Elem a = Element(q);
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
    Elem a = Factor(q);
    while(true)
    {
        char n = Next(q);
        if(n == '+')
        {
            Match(q, '+');
            Elem b = Element(q);
            Add(a, b);
            Elem_free(&b);
        }
        else
        if(n == '-')
        {
            Match(q, '-');
            Elem b = Element(q);
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
    case STR:
        printf("|str| \"%s\"", e->poly.str.value);
        break;
    case REF:
        printf("|ref| %p ", (void*) *e->poly.ref);
        Elem_write(*e->poly.ref, 0);
        break;
    case NUL:
        printf("|null| null");
        break;
    case F64:
        printf("|f64| %lf", e->poly.f64);
        break;
    case FUN:
        printf("|fun|");
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
Assign(deq_char* q, str* n)
{
    Match(q, '=');
    Elem e = Expression(q);
    set_Memb_node* node = Find(n);
    if(node && node->key.elem->type == REF)
    {
        // DEREFERENCE.
        //
        // foo(A)
        // {
        //     A = 0;
        // }
        //
        // main()
        // {
        //     a = 1;
        //     foo(a);
        // }

        Elem* d = Deref(node->key.elem);
        Elem_free(d);
        *d = e;
    }
    else
    {
        // ASSIGN OR UPDATE.
        //
        // main()
        // {
        //     a = 88; # SEE POINT 1.
        //     a = 99; # SEE POINT 1 AND 2.
        // }

        // POINT 1.
        if(node)
            Erase(n);

        // POINT 2.
        Insert(n, e);
    }
}

void
Block(deq_char* q)
{
    set_Memb old = set_Memb_copy(&db);
    Match(q, '{');
    while(Next(q) != '}')
    {
        str n = Read(q, IsIdent);
        if(Next(q) == '=')
            Assign(q, &n);
        else
        {
            Requeue(q, &n);
            Elem e = Expression(q);
            Elem_free(&e);
        }
        Match(q, ';');
        str_free(&n);
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
    return v->size - 1;
}

Elem
Call(str* s, vec_str* args)
{
    set_Memb_node* node = Exists(s);
    Elem elem = node->key.elem;
    if(elem->type != FUN)
        quit("expected function\n");
    int exp = Arguments(elem);
    int got = args->size;
    if(got != exp)
        quit("function `%s()` got %d args but expected %d\n", s->value, got, exp);
    deq_char q = Queue(Code(elem));
    str new = str_copy(s);
    Elem* elems[got];
    {
        int index = 0;
        foreach(vec_str, args, it)
            elems[index++] = &Exists(it.ref)->key.elem;
    }{
        str_swap(&namespace, &new);
        for(int i = 0; i < got; i++)
            Insert(&elem->poly.fun.value[i], Elem_Init(REF, (Poly) { .ref = elems[i] }));
        Block(&q);
        str_swap(&namespace, &new);
    }
    deq_char_free(&q);
    str_free(&new);
    static Poly zero;
    return Elem_Init(NUL, zero);
}

int
main(void)
{
    namespace = str_init("");
    const char* code =
        "baz(A, B, C)"
        "{"
            "A = A + A + [2, 3, 4];"
        "}"

        "foo(A, B, C)"
        "{"
            "baz(A, B, C);"
        "}"

        "bar(A, B, C)"
        "{"
            "foo(A, B, C);"
        "}"

        "main()"
        "{"
            "a = [1, 2, 3];"
            "b = 2;"
            "c = 3;"
            "bar(a, b, c);"
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
    Elem e = Call(&entry, &params);
    Elem_free(&e);
    str_free(&entry);
    set_Memb_free(&db);
    deq_char_free(&q);
    str_free(&namespace);
    vec_str_free(&params);
}

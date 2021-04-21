#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define P
#define T char
#include <que.h>

#include <str.h>

int line = 0;

#define quit(...) printf("error: line %d:", line), printf(__VA_ARGS__), exit(1)

typedef enum { OBJ, ARR, STR, NUM, TRU, FLS, NUL } Type;

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

typedef union
{
    set_Memb obj;
    vec_Elem arr;
    str str;
    double number;
    bool boolean;
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
    default:
        break;
    }
    free(*e);
};

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
    default:
        copy->poly = poly;
        break;
    }
    return copy;
};

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

int
main(void)
{
}

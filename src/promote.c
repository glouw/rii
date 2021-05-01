#ifndef OP
#error "Must define OP as math operator (+ - / *), etc..."
#endif

#define BAIL quit("math operator `%s` not supported for `%s: `%s`", operator, Types[a->type], Types[b->type])

switch(a->type)
{
case I8: 
    switch(b->type)
    {
        case  I8: a->poly. i8 = a->poly.i8 OP b->poly.i8;  a->type =  I8; break;
        case  U8: a->poly. u8 = a->poly.i8 OP b->poly.u8;  a->type =  U8; break;
        case I16: a->poly.i16 = a->poly.i8 OP b->poly.i16; a->type = I16; break;
        case U16: a->poly.u16 = a->poly.i8 OP b->poly.u16; a->type = U16; break;
        case I32: a->poly.i32 = a->poly.i8 OP b->poly.i32; a->type = I32; break;
        case U32: a->poly.u32 = a->poly.i8 OP b->poly.u32; a->type = U32; break;
        case I64: a->poly.i64 = a->poly.i8 OP b->poly.i64; a->type = I64; break;
        case U64: a->poly.u64 = a->poly.i8 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.i8 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.i8 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case U8: 
    switch(b->type)
    {
        case  I8: a->poly. u8 = a->poly.u8 OP b->poly.i8;  a->type =  U8; break;
        case  U8: a->poly. u8 = a->poly.u8 OP b->poly.u8;  a->type =  U8; break;
        case I16: a->poly.i16 = a->poly.u8 OP b->poly.i16; a->type = I16; break;
        case U16: a->poly.u16 = a->poly.u8 OP b->poly.u16; a->type = U16; break;
        case I32: a->poly.i32 = a->poly.u8 OP b->poly.i32; a->type = I32; break;
        case U32: a->poly.u32 = a->poly.u8 OP b->poly.u32; a->type = U32; break;
        case I64: a->poly.i64 = a->poly.u8 OP b->poly.i64; a->type = I64; break;
        case U64: a->poly.u64 = a->poly.u8 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.u8 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.u8 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case I16:
    switch(b->type)
    {
        case  I8: a->poly.i16 = a->poly.i16 OP b->poly.i8;  a->type = I16; break;
        case  U8: a->poly.i16 = a->poly.i16 OP b->poly.u8;  a->type = I16; break;
        case I16: a->poly.i16 = a->poly.i16 OP b->poly.i16; a->type = I16; break;
        case U16: a->poly.u16 = a->poly.i16 OP b->poly.u16; a->type = U16; break;
        case I32: a->poly.i32 = a->poly.i16 OP b->poly.i32; a->type = I32; break;
        case U32: a->poly.u32 = a->poly.i16 OP b->poly.u32; a->type = U32; break;
        case I64: a->poly.i64 = a->poly.i16 OP b->poly.i64; a->type = I64; break;
        case U64: a->poly.u64 = a->poly.i16 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.i16 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.i16 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case U16:
    switch(b->type)
    {
        case  I8: a->poly.u16 = a->poly.u16 OP b->poly.i8;  a->type = U16; break;
        case  U8: a->poly.u16 = a->poly.u16 OP b->poly.u8;  a->type = U16; break;
        case I16: a->poly.u16 = a->poly.u16 OP b->poly.i16; a->type = U16; break;
        case U16: a->poly.u16 = a->poly.u16 OP b->poly.u16; a->type = U16; break;
        case I32: a->poly.i32 = a->poly.u16 OP b->poly.i32; a->type = I32; break;
        case U32: a->poly.u32 = a->poly.u16 OP b->poly.u32; a->type = U32; break;
        case I64: a->poly.i64 = a->poly.u16 OP b->poly.i64; a->type = I64; break;
        case U64: a->poly.u64 = a->poly.u16 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.u16 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.u16 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case I32:
    switch(b->type)
    {
        case  I8: a->poly.i32 = a->poly.i32 OP b->poly.i8;  a->type = I32; break;
        case  U8: a->poly.i32 = a->poly.i32 OP b->poly.u8;  a->type = I32; break;
        case I16: a->poly.i32 = a->poly.i32 OP b->poly.i16; a->type = I32; break;
        case U16: a->poly.i32 = a->poly.i32 OP b->poly.u16; a->type = I32; break;
        case I32: a->poly.i32 = a->poly.i32 OP b->poly.i32; a->type = I32; break;
        case U32: a->poly.u32 = a->poly.i32 OP b->poly.u32; a->type = U32; break;
        case I64: a->poly.i64 = a->poly.i32 OP b->poly.i64; a->type = I64; break;
        case U64: a->poly.u64 = a->poly.i32 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.i32 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.i32 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case U32:
    switch(b->type)
    {
        case  I8: a->poly.u32 = a->poly.u32 OP b->poly.i8;  a->type = U32; break;
        case  U8: a->poly.u32 = a->poly.u32 OP b->poly.u8;  a->type = U32; break;
        case I16: a->poly.u32 = a->poly.u32 OP b->poly.i16; a->type = U32; break;
        case U16: a->poly.u32 = a->poly.u32 OP b->poly.u16; a->type = U32; break;
        case I32: a->poly.u32 = a->poly.u32 OP b->poly.i32; a->type = U32; break;
        case U32: a->poly.u32 = a->poly.u32 OP b->poly.u32; a->type = U32; break;
        case I64: a->poly.i64 = a->poly.u32 OP b->poly.i64; a->type = I64; break;
        case U64: a->poly.u64 = a->poly.u32 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.u32 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.u32 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case I64:
    switch(b->type)
    {
        case  I8: a->poly.i64 = a->poly.i64 OP b->poly.i8;  a->type = I64; break;
        case  U8: a->poly.i64 = a->poly.i64 OP b->poly.u8;  a->type = I64; break;
        case I16: a->poly.i64 = a->poly.i64 OP b->poly.i16; a->type = I64; break;
        case U16: a->poly.i64 = a->poly.i64 OP b->poly.u16; a->type = I64; break;
        case I32: a->poly.i64 = a->poly.i64 OP b->poly.i32; a->type = I64; break;
        case U32: a->poly.i64 = a->poly.i64 OP b->poly.u32; a->type = I64; break;
        case I64: a->poly.i64 = a->poly.i64 OP b->poly.i64; a->type = I64; break;
        case U64: a->poly.u64 = a->poly.i64 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.i64 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.i64 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case U64:
    switch(b->type)
    {
        case  I8: a->poly.u64 = a->poly.u64 OP b->poly.i8;  a->type = U64; break;
        case  U8: a->poly.u64 = a->poly.u64 OP b->poly.u8;  a->type = U64; break;
        case I16: a->poly.u64 = a->poly.u64 OP b->poly.i16; a->type = U64; break;
        case U16: a->poly.u64 = a->poly.u64 OP b->poly.u16; a->type = U64; break;
        case I32: a->poly.u64 = a->poly.u64 OP b->poly.i32; a->type = U64; break;
        case U32: a->poly.u64 = a->poly.u64 OP b->poly.u32; a->type = U64; break;
        case I64: a->poly.u64 = a->poly.u64 OP b->poly.i64; a->type = U64; break;
        case U64: a->poly.u64 = a->poly.u64 OP b->poly.u64; a->type = U64; break;
        case F32: a->poly.f32 = a->poly.u64 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.u64 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case F32:
    switch(b->type)
    {
        case  I8: a->poly.f32 = a->poly.f32 OP b->poly.i8;  a->type = F32; break;
        case  U8: a->poly.f32 = a->poly.f32 OP b->poly.u8;  a->type = F32; break;
        case I16: a->poly.f32 = a->poly.f32 OP b->poly.i16; a->type = F32; break;
        case U16: a->poly.f32 = a->poly.f32 OP b->poly.u16; a->type = F32; break;
        case I32: a->poly.f32 = a->poly.f32 OP b->poly.i32; a->type = F32; break;
        case U32: a->poly.f32 = a->poly.f32 OP b->poly.u32; a->type = F32; break;
        case I64: a->poly.f32 = a->poly.f32 OP b->poly.i64; a->type = F32; break;
        case U64: a->poly.f32 = a->poly.f32 OP b->poly.u64; a->type = F32; break;
        case F32: a->poly.f32 = a->poly.f32 OP b->poly.f32; a->type = F32; break;
        case F64: a->poly.f64 = a->poly.f32 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
case F64:
    switch(b->type)
    {
        case  I8: a->poly.f64 = a->poly.f64 OP b->poly.i8;  a->type = F64; break;
        case  U8: a->poly.f64 = a->poly.f64 OP b->poly.u8;  a->type = F64; break;
        case I16: a->poly.f64 = a->poly.f64 OP b->poly.i16; a->type = F64; break;
        case U16: a->poly.f64 = a->poly.f64 OP b->poly.u16; a->type = F64; break;
        case I32: a->poly.f64 = a->poly.f64 OP b->poly.i32; a->type = F64; break;
        case U32: a->poly.f64 = a->poly.f64 OP b->poly.u32; a->type = F64; break;
        case I64: a->poly.f64 = a->poly.f64 OP b->poly.i64; a->type = F64; break;
        case U64: a->poly.f64 = a->poly.f64 OP b->poly.u64; a->type = F64; break;
        case F32: a->poly.f64 = a->poly.f64 OP b->poly.f32; a->type = F64; break;
        case F64: a->poly.f64 = a->poly.f64 OP b->poly.f64; a->type = F64; break;
        default: BAIL; break;
    }
    break;
default: BAIL; break;
}

#undef OP
#undef BAIL

#define BAIL quit("cast `%s: to `%s` not supported\n", TypeStr[type], TypeStr[e->type]);

switch(type)
{
case I8: 
    switch(e->type)
    {
    case  I8: e->type = I8; e->poly.i8 = e->poly.i8; break;
    case  U8: e->type = I8; e->poly.i8 = e->poly.u8; break;
    case I16: e->type = I8; e->poly.i8 = e->poly.i16; break;
    case U16: e->type = I8; e->poly.i8 = e->poly.u16; break;
    case I32: e->type = I8; e->poly.i8 = e->poly.i32; break;
    case U32: e->type = I8; e->poly.i8 = e->poly.u32; break;
    case I64: e->type = I8; e->poly.i8 = e->poly.i64; break;
    case U64: e->type = I8; e->poly.i8 = e->poly.u64; break;
    case F32: e->type = I8; e->poly.i8 = e->poly.f32; break;
    case F64: e->type = I8; e->poly.i8 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case U8: 
    switch(e->type)
    {
    case  I8: e->type = U8; e->poly.u8 = e->poly.i8; break;
    case  U8: e->type = U8; e->poly.u8 = e->poly.u8; break;
    case I16: e->type = U8; e->poly.u8 = e->poly.i16; break;
    case U16: e->type = U8; e->poly.u8 = e->poly.u16; break;
    case I32: e->type = U8; e->poly.u8 = e->poly.i32; break;
    case U32: e->type = U8; e->poly.u8 = e->poly.u32; break;
    case I64: e->type = U8; e->poly.u8 = e->poly.i64; break;
    case U64: e->type = U8; e->poly.u8 = e->poly.u64; break;
    case F32: e->type = U8; e->poly.u8 = e->poly.f32; break;
    case F64: e->type = U8; e->poly.u8 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case I16:
    switch(e->type)
    {
    case  I8: e->type = I16; e->poly.i16 = e->poly.i8; break;
    case  U8: e->type = I16; e->poly.i16 = e->poly.u8; break;
    case I16: e->type = I16; e->poly.i16 = e->poly.i16; break;
    case U16: e->type = I16; e->poly.i16 = e->poly.u16; break;
    case I32: e->type = I16; e->poly.i16 = e->poly.i32; break;
    case U32: e->type = I16; e->poly.i16 = e->poly.u32; break;
    case I64: e->type = I16; e->poly.i16 = e->poly.i64; break;
    case U64: e->type = I16; e->poly.i16 = e->poly.u64; break;
    case F32: e->type = I16; e->poly.i16 = e->poly.f32; break;
    case F64: e->type = I16; e->poly.i16 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case U16:
    switch(e->type)
    {
    case  I8: e->type = U16; e->poly.u16 = e->poly.i8; break;
    case  U8: e->type = U16; e->poly.u16 = e->poly.u8; break;
    case I16: e->type = U16; e->poly.u16 = e->poly.i16; break;
    case U16: e->type = U16; e->poly.u16 = e->poly.u16; break;
    case I32: e->type = U16; e->poly.u16 = e->poly.i32; break;
    case U32: e->type = U16; e->poly.u16 = e->poly.u32; break;
    case I64: e->type = U16; e->poly.u16 = e->poly.i64; break;
    case U64: e->type = U16; e->poly.u16 = e->poly.u64; break;
    case F32: e->type = U16; e->poly.u16 = e->poly.f32; break;
    case F64: e->type = U16; e->poly.u16 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case I32:
    switch(e->type)
    {
    case  I8: e->type = I32; e->poly.i32 = e->poly.i8; break;
    case  U8: e->type = I32; e->poly.i32 = e->poly.u8; break;
    case I16: e->type = I32; e->poly.i32 = e->poly.i16; break;
    case U16: e->type = I32; e->poly.i32 = e->poly.u16; break;
    case I32: e->type = I32; e->poly.i32 = e->poly.i32; break;
    case U32: e->type = I32; e->poly.i32 = e->poly.u32; break;
    case I64: e->type = I32; e->poly.i32 = e->poly.i64; break;
    case U64: e->type = I32; e->poly.i32 = e->poly.u64; break;
    case F32: e->type = I32; e->poly.i32 = e->poly.f32; break;
    case F64: e->type = I32; e->poly.i32 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case U32:
    switch(e->type)
    {
    case  I8: e->type = U32; e->poly.u32 = e->poly.i8; break;
    case  U8: e->type = U32; e->poly.u32 = e->poly.u8; break;
    case I16: e->type = U32; e->poly.u32 = e->poly.i16; break;
    case U16: e->type = U32; e->poly.u32 = e->poly.u16; break;
    case I32: e->type = U32; e->poly.u32 = e->poly.i32; break;
    case U32: e->type = U32; e->poly.u32 = e->poly.u32; break;
    case I64: e->type = U32; e->poly.u32 = e->poly.i64; break;
    case U64: e->type = U32; e->poly.u32 = e->poly.u64; break;
    case F32: e->type = U32; e->poly.u32 = e->poly.f32; break;
    case F64: e->type = U32; e->poly.u32 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case I64:
    switch(e->type)
    {
    case  I8: e->type = I64; e->poly.i64 = e->poly.i8; break;
    case  U8: e->type = I64; e->poly.i64 = e->poly.u8; break;
    case I16: e->type = I64; e->poly.i64 = e->poly.i16; break;
    case U16: e->type = I64; e->poly.i64 = e->poly.u16; break;
    case I32: e->type = I64; e->poly.i64 = e->poly.i32; break;
    case U32: e->type = I64; e->poly.i64 = e->poly.u32; break;
    case I64: e->type = I64; e->poly.i64 = e->poly.i64; break;
    case U64: e->type = I64; e->poly.i64 = e->poly.u64; break;
    case F32: e->type = I64; e->poly.i64 = e->poly.f32; break;
    case F64: e->type = I64; e->poly.i64 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case U64:
    switch(e->type)
    {
    case  I8: e->type = U64; e->poly.u64 = e->poly.i8; break;
    case  U8: e->type = U64; e->poly.u64 = e->poly.u8; break;
    case I16: e->type = U64; e->poly.u64 = e->poly.i16; break;
    case U16: e->type = U64; e->poly.u64 = e->poly.u16; break;
    case I32: e->type = U64; e->poly.u64 = e->poly.i32; break;
    case U32: e->type = U64; e->poly.u64 = e->poly.u32; break;
    case I64: e->type = U64; e->poly.u64 = e->poly.i64; break;
    case U64: e->type = U64; e->poly.u64 = e->poly.u64; break;
    case F32: e->type = U64; e->poly.u64 = e->poly.f32; break;
    case F64: e->type = U64; e->poly.u64 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case F32:
    switch(e->type)
    {
    case  I8: e->type = F32; e->poly.f32 = e->poly.i8; break;
    case  U8: e->type = F32; e->poly.f32 = e->poly.u8; break;
    case I16: e->type = F32; e->poly.f32 = e->poly.i16; break;
    case U16: e->type = F32; e->poly.f32 = e->poly.u16; break;
    case I32: e->type = F32; e->poly.f32 = e->poly.i32; break;
    case U32: e->type = F32; e->poly.f32 = e->poly.u32; break;
    case I64: e->type = F32; e->poly.f32 = e->poly.i64; break;
    case U64: e->type = F32; e->poly.f32 = e->poly.u64; break;
    case F32: e->type = F32; e->poly.f32 = e->poly.f32; break;
    case F64: e->type = F32; e->poly.f32 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
case F64:
    switch(e->type)
    {
    case  I8: e->type = F64; e->poly.f64 = e->poly.i8; break;
    case  U8: e->type = F64; e->poly.f64 = e->poly.u8; break;
    case I16: e->type = F64; e->poly.f64 = e->poly.i16; break;
    case U16: e->type = F64; e->poly.f64 = e->poly.u16; break;
    case I32: e->type = F64; e->poly.f64 = e->poly.i32; break;
    case U32: e->type = F64; e->poly.f64 = e->poly.u32; break;
    case I64: e->type = F64; e->poly.f64 = e->poly.i64; break;
    case U64: e->type = F64; e->poly.f64 = e->poly.u64; break;
    case F32: e->type = F64; e->poly.f64 = e->poly.f32; break;
    case F64: e->type = F64; e->poly.f64 = e->poly.f64; break;
    default: BAIL; break;
    }
    break;
default: BAIL; break;
}

#undef BAIL

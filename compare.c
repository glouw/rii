#ifndef OP
#error "Must define OP as a comparison operator (< <= >= == !=), etc..."
#endif

#define BAIL quit("compare operator `%s` not supported for `%s: `%s`\n", operator, TypeStr[a->type], TypeStr[b->type]);

switch(a->type)
{
case BLN: 
    switch(b->type)
    {
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; break;
        default: BAIL; break;
    }
    break;
case I8: 
    switch(b->type)
    {
        case  U8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case  I8: a->poly.bln = a->poly.i8  OP b->poly.i8;  a->type = BLN; break;
        case I16: a->poly.bln = a->poly.i8  OP b->poly.i16; a->type = BLN; break;
        case I32: a->poly.bln = a->poly.i8  OP b->poly.i32; a->type = BLN; break;
        case I64: a->poly.bln = a->poly.i8  OP b->poly.i64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.i8  OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.i8  OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case U8: 
    switch(b->type)
    {
        case  I8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U16: a->poly.bln = a->poly.u8 OP b->poly.u16; a->type = BLN; break;
        case  U8: a->poly.bln = a->poly.u8 OP b->poly.u8;  a->type = BLN; break;
        case U32: a->poly.bln = a->poly.u8 OP b->poly.u32; a->type = BLN; break;
        case U64: a->poly.bln = a->poly.u8 OP b->poly.u64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.u8 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.u8 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case I16:
    switch(b->type)
    {
        case  U8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case  I8: a->poly.bln = a->poly.i16 OP b->poly.i8;  a->type = BLN; break;
        case I16: a->poly.bln = a->poly.i16 OP b->poly.i16; a->type = BLN; break;
        case I32: a->poly.bln = a->poly.i16 OP b->poly.i32; a->type = BLN; break;
        case I64: a->poly.bln = a->poly.i16 OP b->poly.i64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.i16 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.i16 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case U16:
    switch(b->type)
    {
        case  I8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case  U8: a->poly.bln = a->poly.u16 OP b->poly.u8;  a->type = BLN; break;
        case U16: a->poly.bln = a->poly.u16 OP b->poly.u16; a->type = BLN; break;
        case U32: a->poly.bln = a->poly.u16 OP b->poly.u32; a->type = BLN; break;
        case U64: a->poly.bln = a->poly.u16 OP b->poly.u64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.u16 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.u16 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case I32:
    switch(b->type)
    {
        case  U8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case  I8: a->poly.bln = a->poly.i32 OP b->poly.i8;  a->type = BLN; break;
        case I16: a->poly.bln = a->poly.i32 OP b->poly.i16; a->type = BLN; break;
        case I32: a->poly.bln = a->poly.i32 OP b->poly.i32; a->type = BLN; break;
        case I64: a->poly.bln = a->poly.i32 OP b->poly.i64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.i32 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.i32 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case U32:
    switch(b->type)
    {
        case  I8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case  U8: a->poly.bln = a->poly.u32 OP b->poly.u8;  a->type = BLN; break;
        case U16: a->poly.bln = a->poly.u32 OP b->poly.u32; a->type = BLN; break;
        case U32: a->poly.bln = a->poly.u32 OP b->poly.u32; a->type = BLN; break;
        case U64: a->poly.bln = a->poly.u32 OP b->poly.u64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.u32 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.u32 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case I64:
    switch(b->type)
    {
        case  U8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case U64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case  I8: a->poly.bln = a->poly.i64 OP b->poly.i8;  a->type = BLN; break;
        case I16: a->poly.bln = a->poly.i64 OP b->poly.i16; a->type = BLN; break;
        case I32: a->poly.bln = a->poly.i64 OP b->poly.i32; a->type = BLN; break;
        case I64: a->poly.bln = a->poly.i64 OP b->poly.i64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.i64 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.i64 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case U64:
    switch(b->type)
    {
        case  I8: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I16: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I32: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case I64: quit("sign mismatch: `%s` `%s`", TypeStr[a->type], TypeStr[b->type]); break;
        case  U8: a->poly.bln = a->poly.u64 OP b->poly.u8;  a->type = BLN; break;
        case U16: a->poly.bln = a->poly.u64 OP b->poly.u16; a->type = BLN; break;
        case U32: a->poly.bln = a->poly.u64 OP b->poly.u32; a->type = BLN; break;
        case U64: a->poly.bln = a->poly.u64 OP b->poly.u64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.u64 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.u64 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case F32:
    switch(b->type)
    {
        case  I8: a->poly.bln = a->poly.f32 OP b->poly.i8;  a->type = BLN; break;
        case  U8: a->poly.bln = a->poly.f32 OP b->poly.u8;  a->type = BLN; break;
        case I16: a->poly.bln = a->poly.f32 OP b->poly.i16; a->type = BLN; break;
        case U16: a->poly.bln = a->poly.f32 OP b->poly.u16; a->type = BLN; break;
        case I32: a->poly.bln = a->poly.f32 OP b->poly.i32; a->type = BLN; break;
        case U32: a->poly.bln = a->poly.f32 OP b->poly.u32; a->type = BLN; break;
        case I64: a->poly.bln = a->poly.f32 OP b->poly.i64; a->type = BLN; break;
        case U64: a->poly.bln = a->poly.f32 OP b->poly.u64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.f32 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.f32 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
case F64:
    switch(b->type)
    {
        case  I8: a->poly.bln = a->poly.f64 OP b->poly.i8;  a->type = BLN; break;
        case  U8: a->poly.bln = a->poly.f64 OP b->poly.u8;  a->type = BLN; break;
        case I16: a->poly.bln = a->poly.f64 OP b->poly.i16; a->type = BLN; break;
        case U16: a->poly.bln = a->poly.f64 OP b->poly.u16; a->type = BLN; break;
        case I32: a->poly.bln = a->poly.f64 OP b->poly.i32; a->type = BLN; break;
        case U32: a->poly.bln = a->poly.f64 OP b->poly.u32; a->type = BLN; break;
        case I64: a->poly.bln = a->poly.f64 OP b->poly.i64; a->type = BLN; break;
        case U64: a->poly.bln = a->poly.f64 OP b->poly.u64; a->type = BLN; break;
        case F32: a->poly.bln = a->poly.f64 OP b->poly.f32; a->type = BLN; break;
        case F64: a->poly.bln = a->poly.f64 OP b->poly.f64; a->type = BLN; break;
        case BLN: a->poly.bln = a->poly.bln OP b->poly.bln; a->type = BLN; break;
        default: BAIL; break;
    }
    break;
default: BAIL; break;
}

#undef OP
#undef BAIL

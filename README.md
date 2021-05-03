# The Roman II Programming Language

Roman II is a Python and C hybrid scripting language supporting no more than dictionaries,
lists, built in CPU data-types, looping, and control flow.

```
Fact(n)
{
    if(n == 0)
    {
        ret 1;
    }
    next := n - 1;
    ret n * Fact(next);
}

Main()
{
    n := 10;
    f := Fact(n);
    if(f == 3628800)
    {
        ret 0;
    }
    else
    {
        ret 1;
    }
}
```

Roman II conforms to ANSI-C99 and is built with the C Template Library (CTL).

## Build

```
git clone ssh://git@github.com/glouw/rr
git submodule update --init --recursive
make
```

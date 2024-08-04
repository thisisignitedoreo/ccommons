
# ccommons
a simple set of header-only stb-style libraries

## quick
to include a library:
1. download header
2. define `{header_name.upper()}_IMPL`
3. include the header

note: if you are including this header in other files, don't do the define step.

api is documented inside the headers.

## libs
- base64.h

  my implementation of base64 in plain c99<br/>
  dependencies: libc

- da.h

  simple dynamic array library, inspired by [dynarray.h](https://github.com/eignnx/dynarray)<br/>
  dependencies: libc


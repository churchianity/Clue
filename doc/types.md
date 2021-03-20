
# Types/Primitives

| symbol       | description or standard | C/C++ equivalent |
| :----------: | :---------------------: | :--------------: |
| byte         | signed 8 bit integer    | int8_t           |
| short        | signed 16 bit integer   | int16_t          |
| int          | signed 32 bit integer   | int32_t          |
| long         | signed 64 bit integer   | int64_t          |
|              |                         |                  |
| s8           | signed 8 bit integer    | int8_t           |
| s16          | signed 16 bit integer   | int16_t          |
| s32          | signed 32 bit integer   | int32_t          |
| s64          | signed 64 bit integer   | int64_t          |
|              |                         |                  |
| u8           | unsigned 8 bit integer  | uint8_t          |
| u16          | unsigned 16 bit integer | uint16_t         |
| u32          | unsigned 32 bit integer | uint32_t         |
| u64          | unsigned 64 bit integer | uint64_t         |
|              |                         |                  |
| f32          | IEEE-754-2008 binary32  | float            |
| f64          | IEEE-754-2008 binary64  | double           |
| float        | IEEE-754-2008 binary32  | float            |
| double       | IEEE-754-2008 binary64  | double           |
|              |                         |                  |
| extended     | x87 extended 80 bit     | extended         |
|              |                         |                  |
| bool         | boolean; true or false  | _Bool/bool       |
|              |                         |                  |
| char         |                         | char8_t          |


# Special values

| symbol       | description                                                                                    |
| :----------: | :--------------------------------------------------------------------------------------------: |
| nil          | the address that contains all zeroes. can be assigned to any pointer variable without casting  |
|              |                                                                                                |
| false        | boolean false                                                                                  |
| true         | boolean true                                                                                   |

## Booleans

The values nil and false are the only two values that make a conditional expression false. All other values are true.

Booleans (bool) will be a byte in size, unless they are inside of a struct which has other booleans - in which case they are packed into the bits of X bytes, where X is the smallest number of bytes that can accomodate the number of boolean present. The actual amount of space available depends on struct packing.

You can perform logical operations on booleans, such as `true == true` or `true and false` or `not false`, but no other operations, even though while being shuttled around, they are often equivalent to a value of type `u8`. Casting a `bool` to another type is not allowed.

## Escape Characters

In any string, these sequences of characters have special meaning. There is heavy overlap between how this works and how it works in C, and many, many other languages.

| symbol       | description    |
| :----------: | :------------: |
| \r           |                |
| \n           |                |
| \t           |                |
|              |                |




## @TODO

others:
how to handle characters greater than 8 bits is a big unknown at the moment.
various extended floating point formats - x87 extended 80 bit being the first.
f80?
extended?


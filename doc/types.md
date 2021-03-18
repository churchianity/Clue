
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
| bool         | boolean; true or false**| _Bool/bool       |
|              |                         |                  |
| char         |                         | char8_t**         |

* booleans will be a byte in size, unless they are inside of a struct which has other booleans - in which case they are packed into the bits of X bytes, where X is the smallest number of bytes that can accomodate the number of boolean present. The actual amount of space available depends on struct packing.

# Special values

| symbol       | description                         |
| :----------: | :---------------------------------: |
| nil          | a pointer which contains all zeroes |
| false        | a pointer which contains all zeroes |
| true         | 1                                   |

All variables if not explicitly initialized are set to nil.

The values nil and false are the only two values that make a conditional expression false. All other values are true.
Some boolean expression operators return true or false, and some return the other values which may resolve to true or false.

`x == y` returns `true` if `x` is equal to `y`, and `false` otherwise. But, `x and y` returns `false` if `x` is `false` or `nil`, and returns `y` otherwise (which may be any value, including nil and false)


others:
how to handle characters greater than 8 bits is a big unknown at the moment.
various extended floating point formats - x87 extended 80 bit being the first.
f80?
extended?


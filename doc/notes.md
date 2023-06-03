
# Functions


# Macros



# Anonymous Functions / Lambdas




# ( [ { } ] )
Curly Braces (`{}`) are for grouping things in a key-value manner.

Square Brackets (`[]`) are for grouping things in an indexed or contiguous manner.

Parentheses are for grouping things


# 'special' keywords

## typecasting via 'cast' keyword
The "cast" keyword tells the compiler to treat a variable of one type as a variable of another type.
It's not clear if this is a good idea.
It's probably error prone, and the situations in which it is useful are few and far between.

Maybe the most common case is to cast an integer to a float to get a floating point result from a division, however:
### floor/integer division vs. floating point division
Different languages interpret the expression `x / y` based on the types of the operands differently.
Some languages, like Python, assume you always want a floating point result, and provide a separate operator `//` for integer division.

We'll probably do the same.
Use `x / y` when you want to garuntee you receive a floating point value of some kind back.
Use `x // y` when you want to garuntee you receive a (probably signed?) integer value of some kind back.

In both cases, Clue should probably do the following:
- if x or y are not one of the following types, throw an error:
    - any integer type
    - any floating point type
    - any pointer type (this does not include strings or arrays)
- if the left hand side of the expression is a pointer, and the right hand side is a floating point number, produce an error
- if the right hand side of the expression ('y', in this case) is a pointer type, produce a warning - maybe
- continue

## instanceof
If runtime type information is enabled, this keyword will return a boolean indicating whether or not its left hand operand is an instance of the class (provided by its name) on its right hand side.

`foo instanceof MyClassBar`

## argsof
If runtime type information is enabled, this keyword will return a type that is the minimal type that matches the argument list for the function name on its right hand side.

```
pow := (x: Int): Int {
    return x ** 2;
}

pow := (x: Int, y: Int): Int {
    return x ** y;
}

argsof pow;
```
The expression `argsof pow` should return a type which matches:
- an Int
- an object with exactly one property `x` that is of type Int

## typeof
If runtime type information is enabled, this keyword will return a string representing the type or list of types matching the variable on its right hand side.
The string returned should have the same spelling and capitalization as the type it represents does.

```
x := 4.0;
typeof x;
```
The above example would print 'float'.


clue's operators are somewhat c like in precedence and associativity.

binary operators are operators that take two operands, and unary operators are operators that take one operand.

there are debatably other operators, some which take any number of operands, and some which take no operands, but precendence and associativity are not relevant for them, so they won't be covered in the next few sections, which are on precedence and associativity.

there are even more binary operators not listed below, which aren't relevant for precedence or associativity, because they never get mixed with other operators in any potentially ambiguous way.
*assignment operators fall under this category*

# Precedence
precedence determines which operator is evaluated first when multiple operators share an operand.

for example, the expression `4 + 1 * 7` evaluates to `35` if you evaluate `4 + 1` first, or `11` if you evaluate `1 * 7` first.

# Associativity
associativity determines the order in which operators are evaluated when multiple operators share an operand and have the same precedence.

for example the expression `4 - 1 + 5 + 3` evaluates to `11` if you evaluate it left-to-right, or `-5` if you evaluate it right-to-left.

binary operators have left-to-right associativity.
*we almost made exponentiation right-to-left, but it would have been the only exception, so we decided against it. There is not an agreed upon way to do associativity for exponentiation, though some math folks argue heavily in favor of right-to-left, many others think it doesn't matter. we're closer to the latter, and voted in favor of consistency.*

unary operators have right-to-left associativity.

# Precedence and Associativity Table
effort was made to make the precedence and associativity be such that you can write your expressions without extra parentheses as much as possible, if you so wish - even if you might prefer to use extra parens for perceived clarity or 'just in case'.

| operator | precedence  | associativity   | notes                  |
|----------|-------------|-----------------|------------------------|
| &&       | 2           | l-r             | logical and            |
| \|\|     | 2           | l-r             | logical or             |
| ^^       | 2           | l-r             | logical xor            |
|          |             |                 |                        |
| ==       | 3           | none            | equality               |
| !=       | 3           | none            | not equals             |
| <        | 3           | none            | less than              |
| >        | 3           | none            | greater than           |
| <=       | 3           | none            | less than or equal     |
| >=       | 3           | none            | greater than or equal  |
|          |             |                 |                        |
| +        | 4 (unary 6) | l-r (unary r-l) | binary/unary addition  |
| -        | 4 (unary 6) | l-r (unary r-l) | binary/unary minus     |
|          |             |                 |                        |
| *        | 5           | l-r             | multiplication         |
| /        | 5           | l-r             | fp division            |
| //       | 5           | l-r             | floor/integer division |
| %        | 5           | l-r             | remainder              |
| &        | 5           | l-r             | bitwise and            |
| \|       | 5           | l-r             | bitwise or             |
| ^        | 5           | l-r             | bitwise xor            |
| >>       | 5           | l-r             | bitwise right shift    |
| <<       | 5           | l-r             | bitwise left shift     |
|          |             |                 |                        |
| ~        | 6           | r-l             | bitwise not            |
| !        | 6           | r-l             | logical not            |
|          |             |                 |                        |
| **       | 7           | l-r             | exponentiation         |
|          |             |                 |                        |
| @        | 8           | r-l             | address-of, pointer decl |
| $        | 8           | r-l             | value-at (p deref)     |
| .        | 8           | l-r             | ownership operator     |
|          |             |                 |                        |

# Please ignore the following table.
### Likely x86_64 instructions various operators will resolve to
| operator | x86_64 instruction(s)                                  |
|----------|--------------------------------------------------------|
| &&       |                                                        |
| ||       |                                                        |
| ^^       |                                                        |
|          |                                                        |
| ==       | cmp                                                    |
| !=       | cmp                                                    |
| <        | cmp                                                    |
| >        | cmp                                                    |
| <=       | cmp                                                    |
| >=       | cmp                                                    |
|          |                                                        |
| +        | add/lea                                                |
| -        | sub/lea                                                |
|          |                                                        |
| *        | mul/imul/lea                                           |
| /        | div/idiv                                               |
| //       | div/idiv                                               |
| %        |                                                        |
| &        | and                                                    |
| \|       | or                                                     |
| ^        | xor                                                    |
| >>       | shr/sar                                                |
| <<       | shl/sal                                                |
|          |                                                        |
| ~        | not                                                    |
| !        |                                                        |
|          |                                                        |
| **       |                                                        |
|          |                                                        |
| @        |                                                        |
| $        |                                                        |
| .        |                                                        |
|          |                                                        |

# Structs

# Namespaces


# Types/Primitives

| symbol       | description or standard | C/C++ equivalent | notes            |
| :----------: | :---------------------: | :--------------: | :--------------: |
| byte         | signed 8 bit integer    | char             | these types may  |
| short        | signed 16 bit integer   | short            | resolve to sizes |
| int          | signed 32 bit integer   | int              | other than what  |
| long         | signed 64 bit integer   | long             | is listed, like  |
|              |                         |                  | C.               |
|              |                         |                  |                  |
| s8           | signed 8 bit integer    | int8_t           |                  |
| s16          | signed 16 bit integer   | int16_t          |                  |
| s32          | signed 32 bit integer   | int32_t          |
| s64          | signed 64 bit integer   | int64_t          |
|              |                         |                  |
| u8           | unsigned 8 bit integer  | uint8_t          |
| u16          | unsigned 16 bit integer | uint16_t         |
| u32          | unsigned 32 bit integer | uint32_t         |
| u64          | unsigned 64 bit integer | uint64_t         |
|              |                         |                  |
| f8           | IEEE-754-2008 binary8   | n/a              |
| f16          | IEEE-754-2008 binary16  | n/a              |
| f32          | IEEE-754-2008 binary32  | float            |
| f64          | IEEE-754-2008 binary64  | double           |
|              |                         |                  |
| f80          | x87 extended 80 bit     | extended         | only if supported.|
|              |                         |                  |
| bool         | boolean; true or false  | _Bool/bool       |
|              |                         |                  |
| char         |                         | char8_t          |

# Built-in Composite Types
| symbol       | description                                                                                              |
| :----------: | :------------------------------------------------------------------------------------------------------: |
|              | VECTORS & MATRICES                                                                                       |
|              |                                                                                                          |
| vec2_byte    | You can construct a vector or matrix type that is comprised of any primitive type from the table above,  |
| vec2_short   | in any quantity from 2-4 for vectors, or any (square) dimensions from 2x2 to 4x4 for matrices.           |
| vec2_int     |                                                                                                          |
| vec2_long    | We don't have 'generics' or 'template metaprogramming', so the construction of these types is fairly     |
|              | manual. You simply prepend 'vecN_', or 'matN_' to the primitive type name, where N is 2-4.               |
|              |                                                                                                          |
|              | The complete list is on the left. Notable exceptions:                                                    |
|              |   - 'f80' primitive does not have a vector or matrix equivalent                                          |
|              |   - 'bool' primitive does not have a vector or matrix equivalent. you probably want bitfields or an      |
|              |     Array, or a vector or matrix of bytes.                                                               |
|              |                                                                                                          |
|              | These types don't have any runtime overhead compared to a raw packed array of the appropriate values.    |
|              | Their memory layout is what you would expect.                                                            |
|              |                                                                                                          |
|              | Certain operators are overloaded to make programming with vectors and matrices more pleasing. Underneath |
|              | these operators is always a function call at worst, a single SIMD instruction at best. Either way, it    |
|              | will be clear what your operators get transformed into by the compiler.                                  |
|              |                                                                                                          |
|              |                                                                                                          |
|              |                                                                                                          |
| Array        |                                                                                                          |
|              |                                                                                                          |
|              |                                                                                                          |
|              |                                                                                                          |
|              |                                                                                                          |
| Table        |                                                                                                          |
|              |                                                                                                          |
|              |                                                                                                          |
|              |                                                                                                          |
|              |                                                                                                          |


# Special values

| symbol       | description                                                                                              |
| :----------: | :------------------------------------------------------------------------------------------------------: |
| nil          | the (virtual) address that contains all zeroes. can be assigned to any pointer variable without casting  |
|              |                                                                                                          |
| false        | boolean false                                                                                            |
| true         | boolean true                                                                                             |


# Type modifiers


## Pointers

If you prepend the '@' operator to any type name, this denotes a pointer type. The type is an address, the value at which is expected to be of the type following the '@'.
This can be done to any type, including another pointer type - so '@@int' is a valid type (the address of a variable which holds a value which is the address of an int).

Pointer types can have arithmetic done on them. This is called 'pointer math' or 'pointer arithmetic'. @TODO It works more or less the same as it does in C.


## Booleans

The values nil and false are the only two values that make a conditional expression false. All other values are true.

Booleans (bool) will be a byte in size, unless they are inside of a struct which has other booleans - in which case they are packed into the bits of X bytes, where X is the smallest number of bytes that can accomodate the number of boolean present. The actual amount of space available depends on struct packing.

You can perform logical operations on booleans, such as `true == true` or `true and false` or `not false`, but no other operations, even though while being shuttled around, they are often equivalent to a value of type `u8`. Casting a `bool` to another type is not allowed.


## Escape Characters, Special Characters, and Strings

In *any* string, these sequences of characters have special meaning. There is heavy overlap between how this works and how it works in C, and many, many other languages.
The main difference, is that we have way fewer escape sequences.

If you wish to interleave lesser-used values into your string, like form-feed (\\f in C), or write out a short network payload with mixed data, you can do so with the `\\x` escape code, followed by a hexadecimal number of arbitrary length, padded to the nearest byte (if your string is: "hi! \xF", then the byte value written will actually be hex 0F).

The byte sequence is terminated when a non-hexadecimal character is encountered. If an 'x' is present in the sequence, it will not be included in the actual string data, but will still cause termination. This allows you to terminate the sequence even if the next character would have been a valid hexadeimcal character.

Additionally, spaces will not terminate the sequence or be included in the string output, so you can lay your hexadecimal out in an easy to read format, like '\\xDE AD B3 3Fx

| symbol       | description                                      |
| :----------: | :----------------------------------------------: |
| \r           | carriage return                                  |
| \n           | newline                                          |
| \t           | tab character                                    |
| \\           | literal backslash                                |
| \%           | literal percent symbol                           |
| \x{byte(s)}  | arbitrary byte length hexadecimal number literal |

In Clue, all String constants are format strings. The '%' operator is overloaded to do String formatting.


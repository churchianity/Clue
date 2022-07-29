
# ( [ { } ] )
Curly Braces (`{}`) are for grouping things in a key-value manner.

Square Brackets (`[]`) are for grouping things in an indexed or contiguous manner.

Parentheses are for grouping things 


# 'normal' keywords
Keywords that you might see in just about every language that probably don't differ wildly go here.

## while
## do

## not
## and
## or







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


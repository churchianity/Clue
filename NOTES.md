

# TOODOOS
Token is struct that contains a pointer to the actual content, but currently this only points to string.
In the case of numeric literals, it would likely be better to store a pointer to the constant as a 64-(or 32)bit floating point number.

# Terminal & Non-Terminal Symbols

evaluating a:
    - SYMBOL
    Terminal
    returns the value of the symbol from the available namespace, if previously declared.

    - NUMERIC:
    Terminal
    returns the literal value of the numeric. this is just converting a string to a floating point 64-bit number.

    - STRING:
    Terminal
    returns the literal value of the string, except in cases of formatted strings, which are a big @TODO.

    - OPERATORS:
    Non-Terminal
    returns the value of the the operation, using its children as operands, which can be terminal or non-terminal symbols.

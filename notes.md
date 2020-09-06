
### Clue Program Flow Diagram

```
 -1.-------------.-------------------.
   | CLA Handler  /  Config Loader   |
   `-------------|-------------------.
                 |                   |
      if sandbox mode enabled and    |
     no source code files provided   |
                 |                   |
                 v        when source code files
  0.-------------.            are provided
.->| Input       |                   |
|  `-------------|<------------------'
|                |
|                v
| 1.-------------.      :   goal - reduce noise in input
|  | Lexer       |
|  `-------------|
|                |
|                v
| 2.-------------.      :  produce initial Abstract Syntax Tree
|  | Parser      |      :  resolve operators and operands into nodes, resolve precedence and associativity
|  `-------------|      :
|                |
|                v
| 3.-------------.
|  | Typing      |
|  `-------------|
|                |
|                v



```

-1. CLA Handler / Config Loader

    clue [args*] [?main.clue]

    -s  --sandbox       enters sandbox mode, where
    -i  --interactive   alias for --sandbox
    -v  --version       prints version information
    -h  --help [?arg]   prints generalized help text, or specialized help if you provide another option name after


0. Input

    If a source code file is provided, its source (and the source of its dependencies, via the `import` statement) is read immediately.

    In Clue, you should only need to provide one source code file per program.

    If the --sandbox or --interactive mode is used, after reading any source files, you are dropped into an interactive prompt. Code you type is read in and injected into the program, evaluated immediately as if specified to be compile-time executed.


1. Lexer
    The lexer's purpose is to reduce noise in the input. Most whitespace, for example, is mostly irrelevant when parsing, so we eliminate it.
    Given that the range of possible inputs to the compiler is basically only limited by:
        - memory/max file size
        - the number of possible/probable text encodings + how many characters exist in that encoding

    We *could*, be given a gigantic bag of garbage, so cleaning that up seems reasonable as a first step.
    We can use this stage to identify early problems, like bad file encoding, or sequences of characters which are either never valid,
    or 'ugly' from the perspective of the linter.

2. Parser
    The parser's purpose is produce the initial AST.

3. Typing
    Perform a pass on the AST and for each operation, verify the types of the operands against the valid sets of types for said operation.


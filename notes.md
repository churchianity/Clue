
### Clue Program Flow Diagram

```
 -1.-------------.
   | CLA Handler |
   `-------------|-------------------.
                 |                   |
      if sandbox mode enabled and    |
     no source code files provided   |
                 |                   |
                 v                   |
  0.-------------.         when source code files
.->| Input       |              are provided
|  `-------------|                   |
|                |                   |
|                v                   |
| 1.-------------.<------------------'
|  | Lexer       |
|  `-------------|
|                |
|                v
| 2.-------------.
|  | Parser      |
|  `-------------|
|                |
|                v
| 3.-------------.
|  | Typing      |
|  `-------------|
|                |
|                v



















```

-1. CLA Handler

    clue [args*] [?main.clue]

    -s  --sandbox       enters sandbox mode, where
    -i  --interactive   alias for --sandbox
    -v  --version       prints version information
    -h  --help [?arg]   prints generalized help text, or specialized help if you provide another option name after


0. Input

    If a source code file is provided, its source (and the source of its dependencies, via the `import` statement) is read immediately.
    If the --sandbox or --interactive mode is used, after reading any source files, you are dropped into an interactive prompt. Code you type is read in and injected into the program, evaluated immediately as if specified to be compile-time executed.


1. Lexer



2. Parser
    The parser's purpose is to resolve operator precedence and associativity, and produce an abstract syntax tree.


3. Typing



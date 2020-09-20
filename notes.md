
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
|  `-------------|<------------------' :  identify file encoding, we don't support non-ascii
|                |
|                v
| 1.-------------.      :  produce array of Tokens - reducing noise in input
|  | Lexer       |
|  `-------------|
|                |
|                v
| 2.-------------.      :  produce initial Abstract Syntax Tree
|  | Parser      |      :  resolve operator precedence, resolve operator associativity
|  `-------------|
|                |
|                v
| 3.-------------.      :  this and subsequent stages perform optimization passes on the AST,
|  | Typing      |      :  re-arranging the structure of the program/appending information or reporting structural issues
|  `-------------|      :  specifically, this one should append static-type information to all operands
|                |
|                v
| 4..?-----------.      :  there will probably be some number of stages here that are just optimization passes
|  | ...         |      :  like removing operations on constants... a node of (4, 2, addition) can be reduced to just (6)
|  `-------------|      :  i don't know what to call these stages at the moment
|                |
|                |
|                |
|                |--------------------------. :  if code is to be compile-time executed...
|                |                          |
| ".-------------------------. $.-------------------------.
|  | Machine Code Generation | |         Runtime          |
|  `-------------------------' `--------------------------'
|                |                              |
|                |                              |
|                |                              |
|                |                              |
`<-----<-----<---|--< if in interactive mode <--'
                 |
                 |
                 |


```

    clue [args*] [?main.clue]

    -s  --sandbox       enters sandbox mode, where
    -i  --interactive   alias for --sandbox
    -v  --version       prints version information
    -h  --help [?arg]   prints generalized help text, or specialized help if you provide another option name after



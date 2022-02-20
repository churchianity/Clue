
clue's operators are somewhat c like in precedence and associativity.
effort was made to make the precedence be such that you can write your expressions without extra parentheses as much as possible, if you so wish - even if you might prefer to use extra parens for perceived clarity or 'just in case'.

one example of this is that casting is done via the 'cast' keyword, and the type comes first:
`float cast myDouble`

this prevents you from having to wrap the expression in parens when desiring to use the '.' operator on the result of a typecast, which is somewhat common to do with languages that support inheritance and typecasting:
`MySubclass cast myParentObject.subclassProperty`

| operator | precedence  | associativity   | notes              |
|----------|-------------|-----------------|--------------------|
| and      | 2           | l-r             | logical operators. |
| or       | 2           | l-r             | we don't use &&, \|\||
| xor      | 2           | l-r             | or ^^              |
|          |             |                 |                    |
| ==       | 3           | none            | comparison.        |
| !=       | 3           | none            |                    |
| <        | 3           | none            |                    |
| >        | 3           | none            |                    |
| <=       | 3           | none            |                    |
| >=       | 3           | none            |                    |
|          |             |                 |                    |
| +        | 4 (unary 6) | l-r (unary r-l) | when unary, prec   |
| -        | 4 (unary 6) | l-r (unary r-l) | and assoc changes. |
|          |             |                 |                    |
| *        | 5           | l-r             |                    |
| /        | 5           | l-r             |                    |
| %        | 5           | l-r             |                    |
| &        | 5           | l-r             |                    |
| \|       | 5           | l-r             |                    |
| &        | 5           | l-r             |                    |
| >>       | 5           | l-r             |                    |
| <<       | 5           | l-r             |                    |
|          |             |                 |                    |
| ~        | 6           | r-l             |                    |
| !        | 6           | r-l             |                    |
! not      | 6           | r-l             |                    |
|          |             |                 |                    |
| **       | 7           | r-l             | exponentiation.    |
|          |             |                 |                    |
| @        | 8           | r-l             | address-of         |
| $        | 8           | r-l             | value-at (p deref) |
| .        | 8           | l-r             |                    |
|          |             |                 |                    |
| cast     | 9           | none            |                    |
|          |             |                 |                    |


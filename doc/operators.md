
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
| ||       | 2           | l-r             | logical or             |
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

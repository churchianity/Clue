import "./samples/long.clue";

`   Simple program to calculate fibonnaci numbers
fib(n: Int) {
    if n == 1 || n == 2 { ` end of line comment
        return 1;
    }

    return fib(n - 1) + fib(n - 2);
}






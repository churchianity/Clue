
`   Simple program to calculate fibonnaci numbers
`   Demonstrates recursion
fib(n: Int): Int {
    if n == 1 || n == 2 {
        return 1;
    }

    return fib(n - 1) + fib(n - 2);
}

fib(3);


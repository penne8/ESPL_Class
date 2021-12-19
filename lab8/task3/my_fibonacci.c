int fib(int limit)
{
    int a = 0, b = 1, temp = 1, last_a = -1, i;

    for (i = 0; a < limit; i++) {
        last_a = a;
        temp += a;
        a = b;
        b = temp;
    }

    return last_a;
}

int main(int argc, char **argv)
{
    int last_a = fib(100);

    return 0;
}
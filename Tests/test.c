extern void printd(int i);

int calc(int a, int b, int c, int d, int e, int f, int g)
{
    int result;
    int result2;

    result = ((a + b) * - (c + d) / (a - c)) * ((b + d) * e - (c / a)) + (d - e) * (f + g) / - (b + e);
    result2 = ((a + b) * - (c + d) / (a - c)) * ((b + d) * e - (c / a)) + (d - e) * (f + g) / - (b + e);

    return result + result2;
}

int main()
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int i;
    void *s;
    int result;

    for (i = 0; i < 10; i = i + 1)
    {
        int a;
        printd(i);
    }

    a = 2 + (1 + 2 + 3);

    a = 1;
    b = 2;
    c = 3;
    d = 4;
    e = 5;
    f = 6;
    g = 7;

    h = - 1 + 2;

    result = calc(a, b, c, d, e, f, s);

    printd(result + a + b);

    for ( i = 0; i < - 1000; i = i + 1)
    {
        printd(i);
    }

    return 0;
}
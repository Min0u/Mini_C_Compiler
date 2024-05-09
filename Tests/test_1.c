extern int printd( int i );

int add(int x, int y)
{
    return x + y;
}

int sub(int x, int y)
{
    return x - y;
}

int mul(int x, int y)
{
    return x * y;
}

int main()
{
    int i;
    int j;
    int k;
    int l;
    i = 2;
    j = 3;

    k = add(i, j);
    l = sub(mul(i, j), add(i, sub(i, k)));
    printd(add(i, j));
    printd(sub(mul(i, j), add(i, sub(i, k))));
    printd(mul(0, add(i, l)));

    /* Error: too few arguments to function 'mul' */
    k = mul();

    return 0;
}
extern int printd(int i);

int (*fact)(int n);

int foo(int n)
{
	if (n > 1) goto false_0;
		return 1;
false_0:
	return (n * (*fact)(n - 1));
}

int main()
{
	int _temp_0;
	fact = &foo;
	_temp_0 = 10;
	_temp_0 = (*fact)(_temp_0);
	printd(_temp_0);
	return 0;
}


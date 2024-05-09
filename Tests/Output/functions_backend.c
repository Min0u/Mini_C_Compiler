extern int printd(int i);

void *fact;

int foo(int n)
{
	int _temp_0;
	if (n > 1) goto false_0;
		return 1;
false_0:
	_temp_0 = n - 1;
	_temp_0 = (*fact)(_temp_0);
	_temp_0 = n * _temp_0;
	return (_temp_0);
}

int main()
{
	int _temp_1;
	fact = &foo;
	_temp_1 = 10;
	_temp_1 = (*fact)(_temp_1);
	printd(_temp_1);
	return 0;
}


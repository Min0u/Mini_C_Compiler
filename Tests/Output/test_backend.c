extern void printd(int i);

int calc(int a, int b, int c, int d, int e, int f, int g)
{
	int result;
	int result2;
	int _temp_0;
	int _temp_1;
	int _temp_2;
	_temp_0 = a + b;
	_temp_1 = c + d;
	_temp_1 = -_temp_1;
	_temp_0 = _temp_0 * _temp_1;
	_temp_1 = a - c;
	_temp_0 = _temp_0 / _temp_1;
	_temp_1 = b + d;
	_temp_1 = _temp_1 * e;
	_temp_2 = c / a;
	_temp_1 = _temp_1 - _temp_2;
	_temp_0 = _temp_0 * _temp_1;
	_temp_1 = d - e;
	_temp_2 = f + g;
	_temp_1 = _temp_1 * _temp_2;
	_temp_2 = b + e;
	_temp_2 = -_temp_2;
	_temp_1 = _temp_1 / _temp_2;
	_temp_0 = _temp_0 + _temp_1;
	result = _temp_0;
	_temp_0 = a + b;
	_temp_1 = c + d;
	_temp_1 = -_temp_1;
	_temp_0 = _temp_0 * _temp_1;
	_temp_1 = a - c;
	_temp_0 = _temp_0 / _temp_1;
	_temp_1 = b + d;
	_temp_1 = _temp_1 * e;
	_temp_2 = c / a;
	_temp_1 = _temp_1 - _temp_2;
	_temp_0 = _temp_0 * _temp_1;
	_temp_1 = d - e;
	_temp_2 = f + g;
	_temp_1 = _temp_1 * _temp_2;
	_temp_2 = b + e;
	_temp_2 = -_temp_2;
	_temp_1 = _temp_1 / _temp_2;
	_temp_0 = _temp_0 + _temp_1;
	result2 = _temp_0;
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
	int _temp_3;
	i = 0;
test_0:
	if (i >= 10) goto false_0;
	{
		int a;
		printd(i);
	}
	i = i + 1;
	goto test_0;
false_0:
	_temp_3 = 1 + 2;
	_temp_3 = _temp_3 + 3;
	_temp_3 = 2 + _temp_3;
	a = _temp_3;
	a = 1;
	b = 2;
	c = 3;
	d = 4;
	e = 5;
	f = 6;
	g = 7;
	_temp_3 = -1;
	_temp_3 = _temp_3 + 2;
	h = _temp_3;
	result = calc(a, b, c, d, e, f, s);
	_temp_3 = result + a;
	_temp_3 = _temp_3 + b;
	printd(_temp_3);
	_temp_3 = -1000;
	i = 0;
test_1:
	if (i >= _temp_3) goto false_1;
	{
		printd(i);
	}
	i = i + 1;
	goto test_1;
false_1:
	return 0;
}


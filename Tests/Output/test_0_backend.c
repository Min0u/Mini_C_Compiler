extern void printd(int i);

extern void *malloc(int size);

void print_point(void *p)
{
	int *_temp_0;
	int _temp_1;
	_temp_0 = p + 0;
	_temp_1 = *_temp_0;
	printd(_temp_1);
	_temp_0 = p + 4;
	_temp_1 = *_temp_0;
	printd(_temp_1);
	_temp_0 = p + 20;
	_temp_1 = *_temp_0;
	printd(_temp_1);
}

int print_1()
{
	int _temp_2;
	_temp_2 = 1;
	printd(_temp_2);
	return 0;
}

int add(int a, int b)
{
	return a + b;
}

int main(int a)
{
	void *p;
	void *p2;
	int x;
	int y;
	int z;
	void s;
	int _temp_3;
	int _temp_4;
	int *_temp_5;
	void *_temp_6;
	_temp_3 = -1;
	_temp_3 = _temp_3 + 2;
	_temp_4 = 3 + 4;
	_temp_4 = _temp_4 + 5;
	_temp_3 = _temp_3 + _temp_4;
	_temp_4 = 2 + 3;
	_temp_3 = _temp_3 + _temp_4;
	a = _temp_3;
	_temp_3 = 12;
	p = malloc(_temp_3);
	_temp_5 = p + 0;
	*_temp_5 = 1;
	_temp_5 = p + 4;
	*_temp_5 = 2;
	_temp_5 = p + 20;
	*_temp_5 = 3;
	_temp_3 = 12;
	p2 = malloc(_temp_3);
	_temp_5 = p2 + 0;
	*_temp_5 = 4;
	_temp_5 = p2 + 4;
	*_temp_5 = 5;
	_temp_5 = p2 + 20;
	*_temp_5 = 6;
	_temp_6 = p + 8;
	*_temp_6 = p2;
	print_point(p);
	_temp_6 = p + 8;
	_temp_3 = *_temp_6;
	print_point(_temp_3);
	_temp_3 = 1;
	_temp_4 = 2;
	_temp_3 = add(_temp_3, _temp_4);
	printd(_temp_3);
	return 0;
}


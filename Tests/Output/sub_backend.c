extern int printd(int i);

int main()
{
	int i;
	int j;
	int _temp_0;
	int _temp_1;
	i = 45000;
	j = -123;
	_temp_0 = i - j;
	printd(_temp_0);
	_temp_0 = 45000 - j;
	printd(_temp_0);
	_temp_0 = i - 123;
	printd(_temp_0);
	_temp_0 = 45000 - 123;
	printd(_temp_0);
	_temp_0 = j + 0;
	_temp_0 = i - _temp_0;
	printd(_temp_0);
	_temp_0 = i + 0;
	_temp_0 = _temp_0 - j;
	printd(_temp_0);
	_temp_0 = i + 0;
	_temp_1 = j + 0;
	_temp_0 = _temp_0 - _temp_1;
	printd(_temp_0);
	_temp_0 = i + 0;
	_temp_0 = _temp_0 - 123;
	printd(_temp_0);
	_temp_0 = j + 0;
	_temp_0 = 45000 - _temp_0;
	printd(_temp_0);
	return 0;
}


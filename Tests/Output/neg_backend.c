extern int printd(int i);

int main()
{
	int j;
	int _temp_0;
	j = 123;
	_temp_0 = -j;
	printd(_temp_0);
	_temp_0 = -123;
	printd(_temp_0);
	_temp_0 = -(123 + 0);
	printd(_temp_0);
	_temp_0 = -(j + 0);
	printd(_temp_0);
	return 0;
}


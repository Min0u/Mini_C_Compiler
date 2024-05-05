extern int printd(int i);

int main()
{
	int i;
	i = 0;
test_0:
	if (i >= 1000) goto false_0;
	{
		printd(i);
	}
	i = i + 1;
	goto test_0;
false_0:
	return 0;
}


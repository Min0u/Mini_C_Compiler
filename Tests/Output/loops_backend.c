void printd(int i){
	printf("%d\n", i);
}

int main(){
	int i;
	i = 0;
test_0:
	if (i >= 10) goto false_0;
		{
		printd(i);
		i = i + 2;
	};
	goto test_0;

false_0:
	i = -10;
test_1:
	if (i > 10) goto false_1;
		printd(i);
	i = i + 1;
	goto test_1;
false_1:
	i = 0;
test_2:
	if (i < -20) goto false_2;
		{
		printd(i);
		i = i - 1;
	};
	goto test_2;

false_2:
	return 0;
}


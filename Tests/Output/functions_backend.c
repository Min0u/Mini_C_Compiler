extern int printd(int i);

int (*fact)(int n);

int foo(int n){
	if (n > 1) goto false_0;
		return 1;
false_0:
	return (n * (*fact)(n - 1));
}

int main(){
	fact = &foo;
	printd((*fact)(10));
	return 0;
}


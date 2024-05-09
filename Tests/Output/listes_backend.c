extern int printd(int i);

void *affiche(void *p)
{
	int *_temp_0;
	int _temp_1;
	_temp_0 = p + 0;
	_temp_1 = *_temp_0;
	if (p == 0) goto false_0;
		printd(_temp_1);
false_0:
	return p;
}

extern void *malloc(int size);

extern void free(void *ptr);

void *allouer(void *p)
{
	if (p == 0) goto false_1;
	{
		void *_temp_2;
		int _temp_3;
		_temp_2 = p + 4;
		_temp_3 = 4;
		_temp_3 = malloc(_temp_3);
		*_temp_2 = _temp_3;
		_temp_2 = p + 4;
		return *_temp_2;
	}
	goto end_1;
false_1:
	{
		int _temp_4;
		_temp_4 = 4;
		p = malloc(_temp_4);
		return p;
	}
end_1:;
}

void *desallouer(void *p)
{
	void *q;
	void *_temp_5;
	_temp_5 = p + 4;
	q = *_temp_5;
	free(p);
	return q;
}

void *parcours(void *l, void *f)
{
	int i;
	void *p;
	void *tete;
	p = f(l);
	tete = p;
	i = 0;
test_2:
	if (i >= 100) goto false_2;
	{
		int *_temp_6;
		p = f(p);
		_temp_6 = p + 0;
		if (p == 0) goto false_3;
			*_temp_6 = i;
false_3:;
	}
	i = i + 1;
	goto test_2;
false_2:
	return tete;
}

int main()
{
	void *tete;
	void *t;
	int _temp_7;
	int _temp_8;
	_temp_7 = 0;
	_temp_8 = &allouer;
	tete = parcours(_temp_7, _temp_8);
	t = tete;
	_temp_7 = &affiche;
	parcours(t, _temp_7);
	_temp_7 = &desallouer;
	parcours(tete, _temp_7);
	return 0;
}


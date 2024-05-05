extern int printd(int i);

struct liste{
	int valeur;
	struct liste *suivant;
};

struct liste *affiche(struct liste *p)
{
	int _temp_0;
	_temp_0 = p->valeur;
	if (p == 0) goto false_0;
		printd(_temp_0);
false_0:
	return p;
}

extern void *malloc(int size);

extern void free(void *ptr);

struct liste *allouer(struct liste *p)
{
	if (p == 0) goto false_1;
	{
		int _temp_1;
		_temp_1 = sizeof(p);
		p->suivant = malloc(_temp_1);
		return p->suivant;
	}
	goto end_1;
false_1:
	{
		int _temp_2;
		_temp_2 = sizeof(p);
		p = malloc(_temp_2);
		return p;
	}
end_1:;
}

struct liste *desallouer(struct liste *p)
{
	struct liste *q;
	q = p->suivant;
	free(p);
	return q;
}

struct liste *parcours(struct liste *l, struct liste *(*f)(struct liste *p))
{
	int i;
	struct liste *p;
	struct liste *tete;
	p = f(l);
	tete = p;
	i = 0;
test_2:
	if (i >= 100) goto false_2;
	{
		p = f(p);
		if (p == 0) goto false_3;
			p->valeur = i;
false_3:;
	}
	i = i + 1;
	goto test_2;
false_2:
	return tete;
}

int main()
{
	struct liste *tete;
	struct liste *t;
	int _temp_3;
	int _temp_4;
	_temp_3 = 0;
	_temp_4 = &allouer;
	tete = parcours(_temp_3, _temp_4);
	t = tete;
	_temp_3 = &affiche;
	parcours(t, _temp_3);
	_temp_3 = &desallouer;
	parcours(tete, _temp_3);
	return 0;
}


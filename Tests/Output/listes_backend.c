extern int printd(int i);

struct liste{
	int valeur;
	struct liste *suivant;
};

struct liste *affiche(struct liste *p){
	if (p == 0) goto false_0;
		printd(p->valeur);
false_0:
	return p;
}

extern void *malloc(int size);

extern void free(void *ptr);

struct liste *allouer(struct liste *p){
	if (p == 0) goto false_1;
;	{
		p->suivant = malloc(sizeof(p));
		return p->suivant;
	}
	goto end_1;
false_1:
	{
		p = malloc(sizeof(p));
		return p;
	}
end_1:;
}

struct liste *desallouer(struct liste *p){
	struct liste *q;
	q = p->suivant;
	free(p);
	return q;
}

struct liste *parcours(struct liste *l, struct liste *(*f)(struct liste *p)){
	int i;
	struct liste *p;
	struct liste *tete;
	tete = p = f(l);
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

int main(){
	struct liste *tete;
	struct liste *t;
	t = tete = parcours(0, &allouer);
	parcours(t, &affiche);
	parcours(tete, &desallouer);
	return 0;
}


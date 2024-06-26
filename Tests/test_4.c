extern void printd(int i);
extern void *malloc(int size);

struct Point {
  int x;
  int y;
  
  struct Point *next;
  int z;
};

void print_point(struct Point *p) {
  printd(p->x);
  printd(p->y);
  printd(p->z);
}

int main(int a) {
  struct Point *p;
  struct Point *p2;
  int x;
  int y;
  int z;
  void s;

  p = malloc(sizeof(struct Point));

  p->x = 1;
  p->y = 2;
  p->z = 3;

  p2 = malloc(sizeof(struct Point));

  p2->x = 4;
  p2->y = 5;
  p2->z = 6;

  p->next = p2;

  print_point(p);
  print_point(p->next);
  
  /* Error: 'y' is not a struct */
  print_point(y->p); 

  return 0;
}

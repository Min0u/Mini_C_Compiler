extern void printf(int i);

void one() {
    printf(1);
}

int main() {
    int i;
    i = 0;

    /* Error: one() takes no arguments */ 
    one(i);
    return 0;
}
extern void printd(int i);

int main(){
    int i;
    void j;

    i = 0;

    while (i < 10) {
        /* Error: 'k' is not declared */
        printd(k);
        i = i + 2;
    }
}
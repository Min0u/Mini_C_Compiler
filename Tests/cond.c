extern int printd( int i );

int main() {
  int i;
  int j;
  i = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;
  j = -123;

  /*-------------------- Conditional Statements --------------------*/

  if ( 1 > 1 + 1 && 2 + 1 != 2 + 2 + 2 ) printd(i); else printd(j);
  if ( 1 <= 3 - 1 && 3 >= 4 ) 
  {
    if (4 <= 3) 
    {
      printd(i);
    } else {
      printd(j);
    }
  }

  if ( 1 == 2 || 3 >= 4 ) printd(i); else printd(j);
  if ( 1 > 2 || 3 < 4 ) 
  {
    if (4 > 3) 
    {
      printd(i);
    } else {
      printd(j);
    }
  }

  /*----------------------------------------------------------------*/

  if ( i+1 < j+0 ) printd(i); else printd(j);
  if ( i+1 < 123 ) printd(i); else printd(j);
  if ( i+1 < j ) printd(i); else printd(j);
  if ( 45 < j+0 ) printd(i); else printd(j);
  if ( 45 < 123 ) printd(i); else printd(j);
  if ( 45 < j ) printd(i); else printd(j);
  if ( i < j+0 ) printd(i); else printd(j);
  if ( i < 123 ) printd(i); else printd(j);
  if ( i < j ) printd(i); else printd(j);

  if ( i+1 > j+0 ) printd(i); else printd(j);
  if ( i+1 > 123 ) printd(i); else printd(j);
  if ( i+1 > j ) printd(i); else printd(j);
  if ( 45 > j+0 ) printd(i); else printd(j);
  if ( 45 > 123 ) printd(i); else printd(j);
  if ( 45 > j ) printd(i); else printd(j);
  if ( i > j+0 ) printd(i); else printd(j);
  if ( i > 123 ) printd(i); else printd(j);
  if ( i > j ) printd(i); else printd(j);

  if ( i+1 >= j+0 ) printd(i); else printd(j);
  if ( i+1 >= 123 ) printd(i); else printd(j);
  if ( i+1 >= j ) printd(i); else printd(j);
  if ( 45 >= j+0 ) printd(i); else printd(j);
  if ( 45 >= 123 ) printd(i); else printd(j);
  if ( 45 >= j ) printd(i); else printd(j);
  if ( i >= j+0 ) printd(i); else printd(j);
  if ( i >= 123 ) printd(i); else printd(j);
  if ( i >= j ) printd(i); else printd(j);

  if ( i+1 <= j+0 ) printd(i); else printd(j);
  if ( i+1 <= 123 ) printd(i); else printd(j);
  if ( i+1 <= j ) printd(i); else printd(j);
  if ( 45 <= j+0 ) printd(i); else printd(j);
  if ( 45 <= 123 ) printd(i); else printd(j);
  if ( 45 <= j ) printd(i); else printd(j);
  if ( i <= j+0 ) printd(i); else printd(j);
  if ( i <= 123 ) printd(i); else printd(j);
  if ( i <= j ) printd(i); else printd(j);

  if ( i+1 == j+0 ) printd(i); else printd(j);
  if ( i+1 == 123 ) printd(i); else printd(j);
  if ( i+1 == j ) printd(i); else printd(j);
  if ( 45 == j+0 ) printd(i); else printd(j);
  if ( 45 == 123 ) printd(i); else printd(j);
  if ( 45 == j ) printd(i); else printd(j);
  if ( i == j+0 ) printd(i); else printd(j);
  if ( i == 123 ) printd(i); else printd(j);
  if ( i == j ) printd(i); else printd(j);

  if ( i+1 != j+0 ) printd(i); else printd(j);
  if ( i+1 != 123 ) printd(i); else printd(j);
  if ( i+1 != j ) printd(i); else printd(j);
  if ( 45 != j+0 ) printd(i); else printd(j);
  if ( 45 != 123 ) printd(i); else printd(j);
  if ( 45 != j ) printd(i); else printd(j);
  if ( i != j+0 ) printd(i); else printd(j);
  if ( i != 123 ) printd(i); else printd(j);
  if ( i != j ) printd(i); else printd(j);

  return 0;
}

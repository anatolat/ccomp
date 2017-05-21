#include <stdio.h>
 
int main()
{
  char is_open[100];
  int pass; int door;

  for (door = 0; door < 100; ++door)
    is_open[door] = 0;
 
  /* do the 100 passes */
  for (pass = 0; pass < 100; ++pass)
    for (door = pass; door < 100; door = door + pass+1)
      is_open[door] = !is_open[door];
 
  /* output the result */
  for (door = 0; door < 100; ++door) {
    char *s;
    if (is_open) {
      s = "open";
    } else {
      s = "closed";
    }
 
    printf("door #%d is %s.\n", door+1, s);
  }
 
  return 0;
}
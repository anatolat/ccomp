
void test_simple(int i) {
 switch (i) {
   case 1: 
   case 3: 
          printf("1 or 3\n"); 
          break;

   case 4: 
   case 5: 
          printf("4 or 5\n"); 
          break;

  default:
          printf("x %d\n", i);
          break;
  }
}

void test_default(int i) {
 switch (i) {
  default:
          printf("x %d\n", i);
  }
}
 

int main() {
 for (int i = 0; i < 10; ++i) {
	test_simple(i);
 }
 printf("\n");
 for (int i = 0; i < 10; ++i) {
	test_default(i);
 }
 printf("\n");
}
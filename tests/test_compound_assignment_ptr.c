

int glob_int_value;
int* glob_int;
char glob_char_value;
char* glob_char;

void test_local_int() {
	printf("test_local_int\n");
	int x = 10;
	int* a = &x;
	printf("%d ", *a += 1);
	printf("%d ", *a -= 1);
	printf("%d ", *a /= 2);
	printf("%d ", *a *= 2);
	printf("%d ", *a %= 3);
	
	printf("\n");
}

void test_local_char() {
	printf("test_local_char\n");
	char b = 11; // unused
	char x = 10;
	char *a = &x;
	printf("%d ", *a += 1);
	printf("%d ", *a -= 1);
	printf("%d ", *a /= 2);
	printf("%d ", *a *= 2);
	printf("%d ", *a %= 3);
	
	printf("\n");
}

void test_glob_int() {
	printf("test_glob_int\n");
	*glob_int = 10;
	printf("%d ", *glob_int += 1);
	printf("%d ", *glob_int -= 1);
	printf("%d ", *glob_int /= 2);
	printf("%d ", *glob_int *= 2);
	printf("%d ", *glob_int %= 3);
	
	printf("\n");
}


void test_glob_char() {
	printf("test_glob_char\n");
	*glob_char = 10;
	printf("%d ", *glob_char += 1);
	printf("%d ", *glob_char -= 1);
	printf("%d ", *glob_char /= 2);
	printf("%d ", *glob_char *= 2);
	printf("%d ", *glob_char %= 3);
	
	printf("\n"); 
}

int main() {
	glob_int = &glob_int_value;
	glob_char = &glob_char_value;

	test_local_int();
	test_local_char();
	test_glob_int();
	test_glob_char();
}


int glob_int;
char glob_char;

void test_local_int() {
	printf("test_local_int\n");
	int a = 10;
	printf("%d ", a += 1);
	printf("%d ", a -= 1);
	printf("%d ", a /= 2);
	printf("%d ", a *= 2);
	printf("%d ", a %= 3);
	
	printf("\n");
}

void test_local_char() {
	printf("test_local_char\n");
	char x = 13; // unused
	char a = 10;
	char b = 11; // unused
	printf("%d ", a += 1);
	printf("%d ", a -= 1);
	printf("%d ", a /= 2);
	printf("%d ", a *= 2);
	printf("%d ", a %= 3);
	
	printf("\n");
}

void test_glob_int() {
	printf("test_glob_int\n");
	glob_int = 10;
	printf("%d ", glob_int += 1);
	printf("%d ", glob_int -= 1);
	printf("%d ", glob_int /= 2);
	printf("%d ", glob_int *= 2);
	printf("%d ", glob_int %= 3);
	
	printf("\n");
}


void test_glob_char() {
	printf("test_glob_char\n");
	glob_char = 10;
	printf("%d ", glob_char += 1);
	printf("%d ", glob_char -= 1);
	printf("%d ", glob_char /= 2);
	printf("%d ", glob_char *= 2);
	printf("%d ", glob_char %= 3);
	
	printf("\n"); 
}

int main() {
	test_local_int();
	test_local_char();
	test_glob_int();
	test_glob_char();
}
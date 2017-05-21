
int test_i() {
	printf("test_i\n");
	return 42;
	printf("test_i\n");
}

void test_void() {
	printf("test_void\n");
	return;
	printf("test_void\n");
}

int main() {
	printf("%d\n", test_i());
	test_void();
}
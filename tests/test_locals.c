
void test_params(int a, int b) {
	printf("%d %d\n", a, b);
}


int main() {
	int local = 42;
	printf("%d\n", local);

	int a;
	a = 100;
	printf("%d\n", a);

	test_params(5, 7);
}
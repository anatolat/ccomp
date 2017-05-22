
// Test chars
// Test local
// Test global
// Test dereference
// Test array
// Test params

void test_local() {
	char a = 10;
	char* p = &a;

	//*p += 1;
	printf("%d\n", *p);
}

int main() {
	test_local();
}
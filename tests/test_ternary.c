
void test(int e) {
	int b = e >= 0 ? 1 : -1;
	printf("%d\n", b);
}

void test2(int e) {
	char* s = e == 0 ? "0" :
		  e == 1 ? "1" :
		  e == 2 ? "2" : "> 2";
	printf("%s\n", s);

}

int main() {
	test(5);
	test(-5);

	test2(0);
	test2(1);
	test2(2);
	test2(3);
}
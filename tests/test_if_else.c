void test_if(int i) {
	if (i == 1) printf("one");
	else if (i == 2) printf("two");
	else if (i == 3) printf("three");
	else printf("xxx");

	printf("\n");
}


int main() {
	test_if(1);
	test_if(2);
	test_if(3);
	test_if(4);
}

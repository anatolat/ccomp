
int main() {
	char b;
	char* ptr = &b;
	*ptr = 1;

	printf("%d %d\n", b, *ptr);
}

int main() {
	char ch;
	int i;
	char *ptr;

	printf("%d %d %d %d %d\n", sizeof(i), sizeof(ch), sizeof(ptr), sizeof(*ptr), sizeof(ptr[0]));

	char str[256];
	printf("%d %d\n", sizeof(str), sizeof(str[0]));
}
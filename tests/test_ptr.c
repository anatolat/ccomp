
int main() {
	int b;
	int *ptr = &b;
	*ptr = 1;

	printf("%d %d\n", b, *ptr);
	
}
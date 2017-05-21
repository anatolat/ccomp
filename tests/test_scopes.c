
int main() {
	int i = 11;
	{
		int i = 13;
		printf("%d\n", i);
	}
	printf("%d\n", i);
	{
		int i = 13;
		printf("%d\n", i);
	}
	printf("%d\n", i);

	for(int i = 15; 0;) printf("%d\n", i);

	printf("%d\n", i);
}
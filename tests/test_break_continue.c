

int main()  {
	for (int i = 0; i < 10; ++i) {
		if (i == 5) break;
		if (i == 3) continue;
		printf("%d ", i);
		continue;
	}
	printf("\n");

	int i = 0;
	while (i < 10) {
		++i;

		if (i == 5) break;
		if (i == 3) continue;
		printf("%d ", i);
		continue;
	}
	printf("\n");

	i = 0;
	do {
		++i;

		if (i == 5) break;
		if (i == 3) continue;
		printf("%d ", i);
		continue;
	} while (i < 10);
	printf("\n");

	i = 0;
	do {
		++i;
		if (i == 1) continue;
		else printf("!!!");
	} while (0);

}
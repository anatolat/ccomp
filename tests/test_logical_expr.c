int t(int a) {
	printf("%d ", a);
	return a;
}


void tt(int a, int b) {
	int r = t(a) && t(b);
	printf("\na && b = %d\n", r);
	r = t(a) || t(b);
	printf("\na || b = %d\n", r);
}

int main() {
	tt(0, 0);
	tt(0, 1);
	tt(1, 0);
	tt(1, 1);


	printf("\n\n");
	t(1) && t(1) || t(0) && t(0); printf("\n");
	t(0) && t(0) || t(1) && t(1); printf("\n");
}
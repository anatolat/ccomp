
int main() {
	int b;
	int *ptr = &b;
	*ptr = 1;

	printf("%d %d\n", b, *ptr);
	
	int xs[2]; 
	xs[0] = 1; xs[1] = 2;

	ptr = xs;
        printf("%d %d\n", ptr[0], ptr[1]);

}
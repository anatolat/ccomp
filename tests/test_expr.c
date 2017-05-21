
int main() {
	int a = 10;
	int b = 5;
	
	// arithmetic
	printf("a+b=%d a-b=%d a*b=%d a/b=%d a%%b=%d\n",
		a + b,
		a - b,	
		a * b,
		a / b,
		a % b);	

	// unary not
	printf("!0=%d !1=%d\n", !0, !1);

	// (expression)
	printf("%d\n", (100));
	
	// unary -/+
	printf("%d %d\n", -1, +1);
}
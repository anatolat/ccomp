
int global;

int main() {
	int local;

	global = 0;
	local = 0;

	printf("%d %d\n", ++local, ++global);
	printf("%d %d\n", --local, --global);
	printf("%d %d\n", local++, global++);
	printf("%d %d\n", local--, global--);
	printf("%d %d\n", local, global);
}
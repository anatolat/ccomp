char str_glob[27];


int main() {
	char str_local[27];
	for (int i = 0; i < 26; ++i) { 
		char ch = 'a' + i;
		str_glob[i] = ch;
		str_local[i] = ch;
	}

	printf("%s\n", str_glob);
	printf("%s\n", str_local);
	
}
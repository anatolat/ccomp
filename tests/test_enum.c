
// empty enum
enum {
};

enum {
  A_0,
  A_1,
  A_2
};
enum {
  A_3 = 3,
  A_4,
  A_10 = 10
};


int main() {
	printf("%d%d%d\n", A_0, A_1, A_2);
	printf("%d%d%d\n", A_3, A_4, A_10);
}
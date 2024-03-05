int add(int a, int b) {
    return a + b;
}

// void fun(int a) {
//     write(a);
//     fun(a + 1);
//     return;
// }

int add_many(int a, int b, int c, int d, int e, int f) {
    int res = a + b + c + d + add(e, add(f, 0));
    return res;
}

int main()
{
    int a = read();
    write(add_many(1,2,3,4,5,a));
	return 0;
}

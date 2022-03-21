int a;
double b = 10.0;

void add_int_and_double(int _a, double _b);

int main()
{
    add_int_and_double(a, b);

    return 0;
}

void add_int_and_double(int _a, double _b)
{
    printf("%d", _a + _b);
}
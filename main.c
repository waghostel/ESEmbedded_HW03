int add(int a, int b);
int a=1; //Glbal varable
void reset_handler(void)
{
	int b=2;
	int c=3;
	int dL=add(b,c);
	int d=7; //End of the the rest_handler()
	while (1)
		;
}

int add(int a, int b){

	return a+b;
}
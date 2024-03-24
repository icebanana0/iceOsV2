void main()
{
	char* memory = (char*) 0xb8000;
	*memory = 'A';
	memory += 2;

}

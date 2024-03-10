void main() 
{
    printc('A');
    return;
}

void printc(char c) 
{
    *(char*)0xb8000 = c;
}

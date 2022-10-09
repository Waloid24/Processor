#include <stdio.h>
#include <ctype.h>

int my_strcmp (const char * string1, const char * string2);

int main (void)
{
    char * s1 = "CP 2 13";
    char * s2 = "CP";
    int v2 = 1;
    int coef = -1;
    printf ("answer = %d\n", my_strcmp(s1, s2)); 
    sscanf (s2, "%d", &coef);
	FILE * p = fopen ("prog", "r");
	printf ("p = %p\n", p);
    printf ("coef = %d\n", coef); //1752397168
}

int my_strcmp (const char * string1, const char * string2)
{
	for (int i = 0, j = 0; string1[i] != '\0' && string2[j] != '\0'; i++, j++)
	{
		while (!isalpha(string1[i]) && string1[i] != '\0') //заменить на while 
			i++;
			
		while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;
		
		if (tolower(string1[i]) == tolower(string2[j]))
			continue;
		return (tolower(string1[i]) - tolower(string2[j]));
	}
	return (tolower(string1[0]) - tolower(string2[0]));
}
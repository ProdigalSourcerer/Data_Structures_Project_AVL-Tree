#include "team.h"

#define MAXMYINT   500000
#define  K     80
#define  DIM   K * 1024LU
#define  NMAX  8LU * DIM


unsigned long sieve(unsigned long num);
	





/*
	 * Function to return an index into the hash table.  The function simply adds up
	 * the values of the char's in the string.
	 *
	 * Parameter:
	 * key:	pointer to a character string
	 * size:	size of the hash table
	 *
	 * Return value: integer hash value of the string
	 *                 */

int getHashKey(void* record, int size)
{ 
	char* key = ((PRISONER*)record)->id;
	int hashval;
	for (hashval = 0 ; *key != '\0' ; hashval += *key++);
	return (hashval % size); 
}

/***********************************************
 * Function to write to file and destroy all dynamically
 * allocated memory at time of call.
 *
 * Parameters: two level pointers to hash table, nameTree and idTree
 * Pre: params allocated and populated (though Null will not cause runtime
 * error.
 * Post: all records written to file and all memory freed
 * ********************************************/

void cleanUp(HASH** hash, TREE** nameTree, TREE** idTree)
{
	HASH_SaveFile(*hash, getName("output file", 1), writeFile);
	*hash = HASH_Destroy(*hash, NULL);
	*nameTree = DestroyTree(*nameTree, PRESERVE);
	*idTree = DestroyTree(*idTree, DESTROY);
}


/* ================ sieve ===================
 * This function will find the last prime
 * Pre: unsigned long number
 * Post: return the last prime number
 */
unsigned long sieve(unsigned long num)
{
    char bit[DIM];
    char bit0[] = { (char)0x7F, (char)0xBF, (char)0xDF, (char)0xEF, (char)0xF7, (char)0xFB, (char)0xFD, (char)0xFE };

    #define  is_prime(x)  ((bit[(x - 1) / 8]) >> (7 - (x - 1) % 8))  & 1
    #define  delete_m(mx)   bit[(mx - 1) / 8] &= bit0[(int)((mx - 1) % 8)]

    unsigned long n, i, max_prime;

	for ( i = 0; i < DIM; bit[i++] = (char)0xFF )
        ;
    
	for ( n = 2; n <= num; n++ ) 
		if ( is_prime( n ) ) {
			max_prime = n;   
			for ( i = 2 * n; i <= num; i += n ) 
				delete_m( i );                  
		}
    
    return max_prime;
}
/* ============== getPrime ================
 * This function will find the next prime for numbers up to 500,000
 * Pre: one integer
 * Post: return the next prime 
 */
int getPrime(int x)
{
	int lastPrime;
	int myPrime;

	if (x < MAXMYINT)
	{
		lastPrime = (int)sieve(x);
		while (x < MAXMYINT && lastPrime == (myPrime = (int)sieve(x++)));
	}
	else
		myPrime = 500009;

	return myPrime;
}


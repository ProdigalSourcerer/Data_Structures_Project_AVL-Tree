#include "team.h"

/****** yesNo *****************************************************************************
 * 	Ask the user a yes/no question and return the response
 * 			PRE		nothing
 * 					RETURN	yes (1) -or- no (0)
 * 					*******************************************************************************************/
int yesNo(char *prompt)
{
	//Local Declarations
	int response = 0;

	//Statements
	printf(prompt);
	printf(" (Y/N): ");
	do  {
		response = toupper(getchar());
		FLUSH;
	}while (response != 'Y' && response != 'N');

	return response == 'Y';
}//yesNo

#include "team.h"


int main(int argc, char* argv[])
{
	HASH* hash;
	TREE *nameTree, *idTree;
	int choice;

	printWelcome();
	setup(&hash, &nameTree, &idTree, argv[1]);
	while( (choice = getMenuChoice(9, "Add prisoner(s)", "Delete prisoner", "Search for prisoner",
					"Print Hash Table", "Print prisoners in ID order", "Print Indented Name Tree",
					"Save all records to file", "Print efficiency report", "Quit")) != 9 ){
		switch(choice){
		case 1: addManager(&hash, nameTree, idTree);
				break;
		case 2: deleteManager(hash, nameTree, idTree);
				break;
		case 3: searchManager(hash, nameTree);
				break;
		case 4: HASH_Testing(hash, printIndex, printPrisonerBrief);
				break;
		case 5: Traverse(idTree, printPrisonerBrief);
				break;
		case 6: PrintNested(nameTree, printPrisonerBrief, 0);
			   	break;
		case 7: HASH_SaveFile(hash, getName("ouput file", 1), writeFile);
				break;
		case 8:	printEfficiency(hash);
				break;
		default: printf("WTF this isn't supposed to be able to happen\n");
		}
	}
	cleanUp(&hash, &nameTree, &idTree);

	return 0;
}

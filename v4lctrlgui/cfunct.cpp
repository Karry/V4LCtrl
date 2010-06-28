
#include "cfunct.h"

char *qStringToChar(QString str){
	int len = str.length();
	char *result = new char[ len+1 ];
	result[ len ] = 0;
	for (int i=0; i< len; i++)
		result[i] = str[i].cell();
	
	return result;
}



/*
 *  string_utils.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#include "string_utils.h"

const char* strtolower(const char *cstr){
	char *str = (char*)malloc(sizeof(char)*strlen(cstr)+1);
	memset(str, '\0', sizeof(char)*strlen(cstr)+1);
	for(size_t i = 0; i < strlen(cstr); i ++){
		str[i] = tolower(cstr[i]);
	}
	return (const char*)str;
}

void reverse_str(char *str){
	char *res = (char*)malloc(sizeof(char)*strlen(str)+1);
	memset(res, '\0', sizeof(char)*strlen(str)+1);
	int i;
	int j;
	for(i = strlen(str)-1, j = 0; i >= 0; i--, j++){
		res[j] = str[i];
	}
	strncpy(str, res, strlen(str));
}

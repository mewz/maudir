/*
 *  base62.c
 *
 *  Created by Jason Hullinger on 5/27/10.
 *
 */

#include "base62.h"

const char *base_vals = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

char* encode_base62(u_int64_t val){
	char *str = (char*)malloc(1);
	memset(str, '\0', 1);
	do{
		int i = val % 62;
		str = (char*)realloc(str, sizeof(char)*strlen(str)+2);
		str[strlen(str)+1] = '\0';
		str[strlen(str)] = base_vals[i];
		val = (val - i) / 62;
	}while(val > 0);
	reverse_str(str);
	return str;	
}

u_int64_t decode_base62(char *str){
	int len = strlen(str);
	u_int64_t val = 0;
	for(int i = 0; i < len; i ++){
		char c = str[i];
		if(!isalnum(c)){
			return -1;
		}
		int j;
		for(j = 0; j < strlen(base_vals); j++){
			if(base_vals[j] == c) break;
		}
		val += j * pow(62, len-i-1);
	}
	return val;	
}

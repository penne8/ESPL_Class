int cmpstr(char *s1, char *s2){

	int curr = 0;
	while(s1[curr]!='\0' || s2[curr]!='\0'){
		int diff = s1[curr]-s2[curr];
		if(diff != 0)
			return diff;
		curr++;
	}

	if (s1[curr]==s2[curr])
		return 0;

	else if(s2[curr]=='\0')
		return s1[curr];

	else
		return s2[curr];
}
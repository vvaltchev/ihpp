#include "util.h"

vector<string> *splitString(string s, char sep) {

	vector<string> *res = new vector<string>();
	size_t len = s.length();
	size_t j;

	char *buf; 

	if (!len)
		return res;

	buf = new char[len+1];
	memset(buf, 0, len+1);

	j=0;

	for (size_t i=0; i < len; i++) {

		if (s[i] == sep) {
		
			if (!j) {
			
				delete [] buf;
				delete res;
				return 0;
			}
				
			buf[j]=0;
			res->push_back(string(buf));
			j=0;
			continue;
		}

		buf[j++]=s[i];
	}

	if (j) {
	
		buf[j]=0;
		res->push_back(string(buf));
	}

	delete [] buf;
	return res;
}


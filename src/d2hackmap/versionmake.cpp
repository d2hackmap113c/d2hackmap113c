#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc,char *argv[]) {
	struct stat st;
	time_t latest=0;
	for (int i=1;i<argc;i++) {
		if (stat(argv[i],&st)!=0) continue;
		if (st.st_mtime>latest) latest=st.st_mtime;
	}
	struct tm *m=localtime(&latest);if (!m) return -1;
	printf("compile time: %d-%d-%d %02d:%02d:%02d\n",1900+m->tm_year,m->tm_mon+1,m->tm_mday,
		m->tm_hour,m->tm_min,m->tm_sec);
	FILE *fp=fopen("version.cpp","w+");if (!fp) return -1;
	fprintf(fp,"#include \"stdafx.h\"\n");
	fprintf(fp,"char *szVersion=\"%04d%02d%02d\";\n",1900+m->tm_year,m->tm_mon+1,m->tm_mday);
	fprintf(fp,"char *compileTime=\"%04d-%02d-%02d %02d:%02d:%02d\";\n",
		1900+m->tm_year,m->tm_mon+1,m->tm_mday,m->tm_hour,m->tm_min,m->tm_sec);
	fclose(fp);
	return 0;
}


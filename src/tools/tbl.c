#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <assert.h>
int gen_armor=1;
int split(char *line,char *ss[],int dup) {
	char *s=line;int n=0;ss[n++]=s;
	while (1) {
		char *e=strchr(s,'\t');if (!e) break;
		*e=0;s=e+1;ss[n++]=s;
	}
	if (dup) {
		for (int i=0;i<n;i++) ss[i]=strdup(ss[i]);
	}
	return n;
}
int split2(char *line,char *ss[],int dup) {
	char *s=line;int n=0;ss[n++]=s;
	while (1) {
		char *e=strchr(s,'-');if (!e) break;
		char c=e[1];
		if ('a'<=c&&c<='z'||'A'<=c&&c<='Z') {s=e+1;continue;}
		*e=0;s=e+1;
		while (*s=='-') s++;
		ss[n++]=s;
	}
	if (dup) {
		for (int i=0;i<n;i++) ss[i]=strdup(ss[i]);
	}
	return n;
}
char buf[8192];
char *hh[8192];
char *ss[8192];
int flag[512]={0};
struct W {
	char *code;
	char *min1,*max1,*min2,*max2,*speed;
	char *str,*dex,*ilvl,*Lvl,*ns;
} weapons[512];
int nweapons=0;
void load_weapons() {
	nweapons=0;
	char *path="atma/Weapons.txt";
	if (gen_armor) path="atma/Armor.txt";
	FILE *fp=fopen(path,"r");assert(fp);
	int n=0;
	while (1) {
		char *line=fgets(buf,8192,fp);if (!line) break;
		int len=strlen(line);assert(len<8000);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[len-1]=='\r') {len--;line[len]=0;}
		if (n) {
			int nss=split(line,ss,0);
			struct W *p=&weapons[nweapons++];assert(nweapons<512);
			if (!gen_armor) {
				p->code=strdup(ss[3]);
				p->min1=strdup(ss[10]);
				p->max1=strdup(ss[11]);
				p->min2=strdup(ss[14]);
				p->max2=strdup(ss[15]);
				p->speed=strdup(ss[20]);
				p->str=strdup(ss[23]);
				p->dex=strdup(ss[24]);
				p->ilvl=strdup(ss[27]);
				p->Lvl=strdup(ss[28]);
				p->ns=strdup(ss[52]);
			} else {
				p->code=strdup(ss[17]);
				p->min1=strdup(ss[5]);
				p->max1=strdup(ss[6]);
				p->speed=strdup(ss[8]);
				p->str=strdup(ss[9]);
				p->ilvl=strdup(ss[13]);
				p->Lvl=strdup(ss[14]);
				p->ns=strdup(ss[31]);
			}
			//printf("%s,",p->code);
		} 
		n++;
	}
	fclose(fp);
}
void gen_weapons() {
	load_weapons();
	char *path="atma/ws.txt";
	char *outpath="atma/gen_ws.txt";
	if (gen_armor) {
		path="atma/as.txt";
		outpath="atma/gen_as.txt";
	}
	FILE *fp=fopen(path,"r");assert(fp);
	FILE *out=fopen(outpath,"w+");assert(out);
	int n=0;
	while (1) {
		char *line=fgets(buf,8192,fp);if (!line) break;
		int len=strlen(line);assert(len<8000);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[len-1]=='\r') {len--;line[len]=0;}
		char c=line[0];
		if ('a'<=c&&c<='z'||'A'<=c&&c<='Z') {
			//printf("%s\n",line);
			int nss=split2(line,ss,0);
			assert(nss==4);
			char *code=ss[2];
			struct W *p=NULL;
			for (int i=0;i<nweapons;i++) if (strcmp(weapons[i].code,code)==0) {p=&weapons[i];break;}
			if (!p) printf("can't find '%s'\n",code);
			fprintf(out,"%4s\t%s\t",ss[3],code);
			if (p) {
				if (p->ns&&p->ns[0]) fprintf(out,"%s\xbf\xd7",p->ns);
				fprintf(out,"\t");
				fprintf(out,"\xc1\xa6%s",p->str);
				fprintf(out,"\t");
				if (!gen_armor||p->dex&&p->dex[0]) fprintf(out,"\xc3\xf4%s",p->dex);
				fprintf(out,"\t");
				if (gen_armor) {
					if (p->min1&&p->min1[0]) fprintf(out,"\xb7\xc0%s-%s",p->min1,p->max1);
				} else {
					if (p->min1&&p->min1[0]) fprintf(out,"\xb5\xa5%s-%s",p->min1,p->max1);
					if (p->min2&&p->min2[0]) fprintf(out,"\xcb\xab%s-%s",p->min2,p->max2);
				}
				fprintf(out,"\t");
				if (p->speed&&p->speed[0]) fprintf(out,"\xcb\xd9%s",p->speed);
				fprintf(out,"\t");
				fprintf(out,"i%s L%s",p->ilvl,p->Lvl);
				fprintf(out,"\t");
			}
			fprintf(out,"%s\t%s\n",ss[0],ss[1]);
		} else {
			fprintf(out,"%s\n",line);
		}
	}
	fclose(fp);
	fclose(out);
}
void process_tbl() {
	char path[256],outpath[256];
	char namepath[256];
	int nhh=0,n=0;
	char *name;
		name="Misc";
		name="Skills";
		name="ATMA_quests";
		name="ATMA_queststates";
		name="Runes";
		name="ATMA_properties";
		name="MonStats";
		name="ItemStatCost";
		name="Hireling";
		name="monstats";
		name="sounds";
		name="charstats";
		name="states";
		name="Armor";
		name="ATMA_quests";
		name="ATMA_queststates";
		name="objects";
		name="ATMA_properties";
		name="Weapons";
	sprintf(path,"t/txt/%s.txt",name);
	sprintf(outpath,"t/txt/zout_%s.txt",name);
	sprintf(namepath,"t/txt/zname_%s.txt",name);
	FILE *fp=fopen(path,"r");assert(fp);
	FILE *out=fopen(outpath,"w+");assert(out);
	FILE *out2=fopen(namepath,"w+");assert(out2);
	printf("%s -> %s\n",path,outpath);
	while (1) {
		char *line=fgets(buf,8192,fp);if (!line) break;
		int len=strlen(line);assert(len<8000);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[len-1]=='\r') {len--;line[len]=0;}
		if (nhh==0) nhh=split(line,hh,1);
		else {
			int nss=split(line,ss,0);
			if (out2) {
				if (0) {
					for (int i=3;i<=3;i++) if (ss[i]&&ss[i][0]) fprintf(out2,"%s\n",ss[i]);
				} else if (0) { //armor
					fprintf(out2,"%s (%s)",ss[17],ss[0]);
					if (ss[14][0]) fprintf(out2," L%s",ss[14]);
					if (ss[31][0]) fprintf(out2," socket%s",ss[31]);
					if (ss[9][0]) fprintf(out2," STR%s",ss[9]);
					if (ss[5][0]) fprintf(out2," ac%s-%s",ss[5],ss[6]);
					if (ss[63][0]) fprintf(out2," dam%s-%s",ss[63],ss[64]);
					fputc('\n',out2);
				} else if (1) { //weapon
					fprintf(out2,"%s (%s)",ss[3],ss[0]);
					if (ss[28][0]) fprintf(out2," L%s",ss[28]);
					if (ss[52][0]) fprintf(out2," socket%s",ss[52]);
					if (ss[20][0]) fprintf(out2," speed%s",ss[20]);
					if (ss[23][0]) fprintf(out2," STR%s",ss[23]);
					if (ss[24][0]) fprintf(out2," DEX%s",ss[24]);
					if (ss[10][0]) fprintf(out2," (1hand)%s-%s",ss[10],ss[11]);
					if (ss[14][0]) fprintf(out2," (2hand)%s-%s",ss[14],ss[15]);
					fputc('\n',out2);
				} else {
				}
			}
			fprintf(out,"------- %d ------\n",n);
			for (int i=0;i<nhh;i++) {
				if (i>=nss) break;
				char *s=ss[i];
				if (s[0]==0) continue;
				fprintf(out,"%d %s:%s\n",i,hh[i],ss[i]);
			}
			for (int i=nhh;i<nss;i++) {
				fprintf(out,"%d ???:%s\n",i,ss[i]);
			}
			n++;
		}
	}
	fclose(fp);
	fclose(out);
	if (out2) fclose(out2);
}
int main( void ) {
	process_tbl();return 0;
	gen_armor=0;
	gen_weapons();
	//gen_armors();
	return 0;
}

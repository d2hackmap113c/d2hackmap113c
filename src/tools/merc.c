#include <windows.h>
#include <stdio.h>
#include <assert.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");assert(fp);
	fseek(fp,0,2);int size=ftell(fp);*psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
void save(char *path,char *buf,int size) {
	FILE *fp=fopen(path,"wb+");assert(fp);
	int n=fwrite(buf,1,size,fp);assert(n==size);fclose(fp);
}
/*
type:
0 normal fire,1	normal ice,2 nightmare fire,3 nightmare ice,4 hell fire,5 hell ice
6 normal combat,7 normal defense,8 normal offense
	9 nightmare combat,a nightmare defense,b nightmare offense
	c hell combat,d hell defense,e hell offense
f normal A3 fire,10	normal A3 ice,11	normal A3 light
	12 	nightmare A3 fire,13	nightmare A3 ice,14	nightmare A3 light
	15 	hell A3 fire,16	hell A3 ice,17	hell A3 light
18 	normal bar,19 	normal bar
	1a 	nightmare bar,1b 	nightmare bar
	1c 	hell bar,1d 	hell bar
*/
struct MercInfo {
	int expansion; //+0, 0:classic 0x64:expansion,
	int type; //+4
	int txt; //+8 A1:10f A2:152 A3:167 A5:231
	int act; //+C, 1-5
	int difficulty; //+10, 1:normal 2:nightmare 3:hell
	int seller; //+14
	int gold; //+18
	int level; //+1C
	int expPerLevel; //+20
	//experience_function(level) = (Exp/Lvl) * (level+1) * level ^ 2
	int life; //+24
	int lifePerLevel; //+28
	int defense; //+2C
	int defensePerLevel; //+30
	int strength; //+34
	int strengthPer8Level; //+38
	int dexterity; //+3C
	int dexterityPer8Level; //+40
	int AR; //+44
	int ARPerLevel; //+48
	int off4C; //+4C
	int minDamage; //+50
	int maxDamage; //+54
	int damagePerLevel; //+58
	int resist; //+5C
	int resistPer4Level; //+60
	int defaultChance; //+64
	int off68;
	int off6C;
	int off70;
	int off74;
	int skills[2]; //+78
	int off80[17];
	char offc4[2];
	char skillLevel[2]; //+C6
	char offc8[4];
	char skillPer32Level[2]; //+CC
	char offCE[2];
	char offD0[3],idstrStart[32],idstrEnd[33]; //size 0x118
	short nameIdStart,nameIdEnd;
};

void dump() {
	struct MercInfo info;
	assert(sizeof(struct MercInfo)==0x118);
	int size;char *buf=load("t/mercData.bin",&size);
	int n=size/0x118;
	printf("n=%d\n",n);
	for (int i=0;i<n;i++) {
		memcpy(&info,buf+i*0x118,0x118);
		printf("%d name=%x-%x",i,info.nameIdStart,info.nameIdEnd);
		printf(" exp=%x",info.expansion);
		printf(" type=%x",info.type);
		//printf(" txt=%x",info.txt);
		int t=info.off64;
		printf(" (%d %x)",t);
		t=info.off68;
		printf(" (%d %x)",t);
		t=info.off6C;
		printf(" (%d %x)",t);
		//printf(" act=%d",info.act);
		//printf(" str=%s-%s",info.idstrStart,info.idstrEnd);
		printf("\n");
	}
}
int main(int argc, char* argv[]) {
	dump();
	//Lv90 Nightmare act2 str195 dex157
	return 0;
}


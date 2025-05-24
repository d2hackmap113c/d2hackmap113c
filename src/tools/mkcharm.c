// #include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <assert.h>

#define MAX_BITS 8192
FILE *fplog;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
void hex(int addr,u8 *buf,int n) {
	for (int i=0;i<n;i+=16) {
		printf("%06x: ",addr+i);
		for (int j=0;j<16;j++) {
			if (i+j>=n) printf("   ");
			else printf("%02x ",buf[i+j]);
			if (j==3||j==7||j==11) printf("- ");
		}
		printf("|");
		for (int j=0;j<16;j++) {
			if (i+j>=n) {printf(" ");continue;}
			u8 c=buf[i+j];
			if (c<' '||c>127) printf(" ");
			else printf("%c",c);
		}
		printf("\n");
	}
	printf("\n");
}
void diff(u8 *b1,u8 *b2,int size) {
	for (int i=0;i<size;i+=20) {
		int n=size-i;if (n>20) n=20;
		if (memcmp(b1+i,b2+i,n)==0) continue;
		printf("%3d: ",i);
		for (int j=0;j<n;j++) {
			if (j==5||j==10||j==15) printf(" ");
			printf("%d",b1[i+j]);
		}
		printf("\n     ");
		for (int j=0;j<n;j++) {
			if (j==5||j==10||j==15) printf(" ");
			if (b1[i+j]!=b2[i+j]) printf("%d",b2[i+j]);else printf("_");
		}
		printf("\n");
	}
}
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
char *loadbits(char *bs,int size) {
	if (!bs) return NULL;
	assert(size*8<MAX_BITS);
	char *b=malloc(MAX_BITS);assert(b);
	memset(b,0,MAX_BITS);
	char *s=b;
	for (int i=0;i<size;i++) {
		u8 v=*bs++;
		for (int j=0;j<8;j++) {
			*s++=v&1;v>>=1;
		}
	}
	return b;
}
char *loadbitfile(char *path,int *psize) {
	int size;char *bs=load(path,&size);
	*psize=size*8;
	return loadbits(bs,size);
}
int rbits(char *bs,int pos,int n) {
	assert(n<=32);int v=0;
	for (int i=0;i<n;i++) {
		if (bs[pos+i]) v|=(1<<i);
	}
	return v;
}
int wbits(char *bs,int pos,int n,int value) {
	assert(n<=32);for (int i=0;i<n;i++) {bs[pos+i]=value&1;value>>=1;}
}
void savebits(char *path,char *b,int size) {
	int s2=size/8;if (size&7) s2++;
	char *bs=malloc(s2);assert(bs);
	char *s=b,*end=b+size,*bs2=bs;
	for (int i=0;i<s2;i++) {
		u8 v=0;
		for (int j=0;j<8;j++) {
			v>>=1;
			if (s<end&&*s) v|=0x80;
			s++;
		}
		*bs2++=v;
	}
	save(path,bs,s2);
}

char *qualityStr[]={0,"Inferior","Normal","superior","Magic","Set","Rare","Unique","Craft","Tempered"};
char *memmem(char *text,int size,char *ptn,int n) {
	char *s=text,*end=text+size-n;
	while (s<=end) if (memcmp(s,ptn,n)==0) return s;else s++;
}
void search(char *text,int size,int n,int value) {
	char ptn[256];
	wbits(ptn,0,n,value);
	char *end=text+size-n;
	for (char *s=text;s<=end;s++) {
		if (memcmp(s,ptn,n)==0) {
			printf("match %d 0x%x at %d\n",value,value,s-text);
		}
	}
}
void pbits(u8 *b,int size) {
	for (int i=0;i<size;i++) {
		int p=i%20;
		if (p==0) printf("%3d: ",i);
		printf("%d",b[i]);
		if (p==19) printf("\n");
		else if (p==4||p==9||p==14) printf(" ");
	}
	printf("\n%d bits\n",size);
}
int add_prop1(char *b,int pos,int id,int v,int bits) {
	wbits(b,pos,9,id);pos+=9;
	wbits(b,pos,bits,v);pos+=bits;
	return pos;
}
int add_prop2(char *b,int pos,int id,int v1,int v2,int bits1,int bits2) {
	wbits(b,pos,9,id);pos+=9;
	wbits(b,pos,bits1,v1);pos+=bits1;
	wbits(b,pos,bits2,v2);pos+=bits2;
	return pos;
}
int add_prop3(char *b,int pos,int id,int v1,int v2,int v3,int bits1,int bits2,int bits3) {
	wbits(b,pos,9,id);pos+=9;
	wbits(b,pos,bits1,v1);pos+=bits1;
	wbits(b,pos,bits2,v2);pos+=bits2;
	wbits(b,pos,bits3,v3);pos+=bits3;
	return pos;
}
void make_charm() {
	char charm[]={
		0x4a,0x4d,0x10,0x00,0x80,0x00,0x64,0x00,0x,0x60,0x32,0xd6,0x16,0x03,0x02,0xee,0xda,
		0xaa,0xf2,0x0b,0x05,0xad,0x00,0xc0,0x16,0x,0x80,0x4b,0x7b,0xfe,0x7f,
	};
	int size;
	char *outpath="t/t2.d2i";
	char *b=loadbit(charm,sizeof(charm));
	int pos=182;
	//pos=add_prop1(b,pos,127,1,3);//"%+d 所有技能"
	//pos=add_prop2(b,pos,97,70,3,9,6);//+3 to 80
//pos=add_prop2(b,pos,97,149,63,9,6);//+63 to BO
	//wbits(b,pos,9,0x1ff);pos+=9;savebits(outpath,b,pos);return;
	/*
	pos=add_prop1(b,pos,127,7,3);//"%+d 所有技能"
	pos=add_prop1(b,pos,85,511,9);//"%+d%% 经验获得"
pos=add_prop2(b,pos,97,52,1,9,6);//+1 to Enchant
pos=add_prop2(b,pos,97,149,1,9,6);//+1 battle order
pos=add_prop2(b,pos,97,226,1,9,6);//+1 oak sage
pos=add_prop2(b,pos,97,52,63,9,6);//+63 to Enchant
pos=add_prop2(b,pos,97,131,63,9,6);//+63 to 
pos=add_prop2(b,pos,97,142,63,9,6);//+63 to
pos=add_prop2(b,pos,97,240,63,9,6);//+63 to
pos=add_prop2(b,pos,97,241,63,9,6);//+63 to
*/
	pos=add_prop2(b,pos,97,54,63,9,6);//+63 to Teleport
	pos=add_prop2(b,pos,97,58,63,9,6);//+63 to Energy Shield
	pos=add_prop1(b,pos,27,255,8);//"加快恢复法力 %d%%" "Regenerate Mana %d%% - %d%%"
	pos=add_prop1(b,pos,32,511,9);//"%+d 近身防御" "From %d - %d Defense vs. Missile"
	pos=add_prop1(b,pos,33,255,8);//"%+d 远程防御" "From %d - %d Defense vs. Melee"
	pos=add_prop1(b,pos,34,63,6);//"物理伤害减少 %d" "Damage Reduced by %d - %d"
	pos=add_prop1(b,pos,35,63,6);//"魔法伤害减少 %d" "Magic Damage Reduced by %d - %d"
	pos=add_prop1(b,pos,36,255,8);//"物理伤害减少 %d%%" "Damage Reduced by %d%% - %d%%"
	pos=add_prop1(b,pos,37,255,8);//"魔法抵抗 %+d%%" "Magic Resist %d%% - %d%%"
	pos=add_prop1(b,pos,38,31,5);//"%+d%% 最大魔法抵抗" "From %d%% - %d%% to Maximum Magic Resist"
	pos=add_prop1(b,pos,39,255,8);//"抗火焰 %+d%%" "Fire Resist %d%% - %d%%"
	pos=add_prop1(b,pos,40,31,5);//"%+d%% 最大火焰抵抗" "From %d%% - %d%% to Maximum Fire Resist"
	pos=add_prop1(b,pos,41,255,8);//"抗闪电 %+d%%" "Lightning Resist %d%% - %d%%"
	pos=add_prop1(b,pos,42,31,5);//"%+d%% 最大闪电抵抗" "From %d%% - %d%% to Maximum Lightning Resist"
	pos=add_prop1(b,pos,43,255,8);//"抗冰冷 %+d%%" "Cold Resist %d%% - %d%%"
	pos=add_prop1(b,pos,44,31,5);//"%+d%% 最大冰冷抵抗" "From %d%% - %d%% to Maximum Cold Resist"
	pos=add_prop1(b,pos,45,255,8);//"抗毒素 %+d%%" "Poison Resist %d%% - %d%%"
	pos=add_prop1(b,pos,46,31,5);//"%+d%% 最大毒素抵抗" "From %d%% - %d%% to Maximum Poison Resist"
	pos=add_prop1(b,pos,67,127,7);//"%d%% Faster Run/Walk"
	pos=add_prop1(b,pos,74,63,6);//"Drain Life %+d"
	pos=add_prop1(b,pos,76,63,6);//"增加生命最大值 %d%%"
	pos=add_prop1(b,pos,77,63,6);//"增加法力最大值 %d%%"
	pos=add_prop1(b,pos,80,255,8);//"%d%% 获得魔法装备"
	pos=add_prop1(b,pos,87,127,7);//"减少交易花费 %d%%"
	pos=add_prop1(b,pos,89,15,4);//"%+d 照亮范围"
	pos=add_prop1(b,pos,91,0,8);//"需求 %+d%%"
	pos=add_prop1(b,pos,93,127,7);//"%d%% 增加攻击速度"
	pos=add_prop1(b,pos,96,40,7);//"%d%% Faster Run/Walk"
	pos=add_prop1(b,pos,99,127,7);//"%d%% 快速打击恢复"
	pos=add_prop1(b,pos,105,127,7);//"%d%% 快速施法速度"
	pos=add_prop1(b,pos,142,127,7);//"%d%% 火焰吸收" "From %d%% - %d%% Fire Absorb"
	pos=add_prop1(b,pos,143,127,7);//"%+d 火焰吸收" "From %d - %d Fire Absorb"
	pos=add_prop1(b,pos,144,127,7);//"%d%% 闪电吸收" "From %d%% - %d%% Lightning Absorb"
	pos=add_prop1(b,pos,145,127,7);//"%+d 闪电吸收" "From %d - %d Lightning Absorb"
	pos=add_prop1(b,pos,146,127,7);//"%d%% 魔法吸收" "From %d%% - %d%% Magic Absorb"
	pos=add_prop1(b,pos,147,127,7);//"%+d 魔法吸收" "From %d - %d Magic Absorb"
	pos=add_prop1(b,pos,148,127,7);//"%d%% 冰冻吸收" "From %d%% - %d%% Cold Absorb"
	pos=add_prop1(b,pos,149,127,7);//"%+d 冰冻吸收" "From %d - %d Cold Absorb"
	pos=add_prop1(b,pos,153,1,1);//"无法被冻结" 
	/*
pos=add_prop1(b,pos,0,255,8);//"%+d 力量" "From %d - %d Strength"
pos=add_prop1(b,pos,1,127,7);//"%+d 精力" "From %d - %d Energy"
pos=add_prop1(b,pos,2,127,7);//"%+d 敏捷" "From %d - %d Dexterity"
pos=add_prop1(b,pos,3,127,7);//"%+d 体力" "From %d - %d Vitality"
pos=add_prop1(b,pos,7,511,9);//"%+d 生命" "From %d - %d Life"
pos=add_prop1(b,pos,9,255,8);//"%+d 法力" "From %d - %d Mana"
pos=add_prop1(b,pos,11,255,8);//"%+d 最大耐力" "From %d - %d Maximum Stamina"
pos=add_prop1(b,pos,16,511,9);//"%+d%% 增强防御" "From %d%% - %d%% Enhanced Defense"
pos=add_prop2(b,pos,17,511,511,9,9);//"%+d%% 增强伤害" "From %d%% - %d%% Enhanced Damage"
pos=add_prop1(b,pos,19,1023,10);//"%+d 命中率" "From %d - %d Attack Rating"
pos=add_prop1(b,pos,28,255,8);//"加快恢复耐力 %d%%" "Heal Stamina Plus %d%% - %d%%"
pos=add_prop1(b,pos,31,2047,11);//"%+d 防御" "From %d - %d Defense"
pos=add_prop1(b,pos,49,511,9);//"%+d 最大火焰伤害" "From %d - %d to Maximum Fire Damage"
*/
/*
pos=add_prop1(b,pos,141,127,7);//"%d%% 双倍打击" "From %d%% - %d%% Deadly Strike"
pos=add_prop1(b,pos,152,1,1);//"无法破坏的" 
pos=add_prop1(b,pos,154,127,7);//"%d%% 减少耐力消耗" "From %d%% - %d%% Slower Stamina Drain"
pos=add_prop1(b,pos,156,127,7);//"穿透攻击 [%d]" 
pos=add_prop1(b,pos,157,127,7);//"射出魔法箭矢" 
pos=add_prop1(b,pos,158,127,7);//"箭矢或弓弹火焰爆炸" 
pos=add_prop1(b,pos,252,63,6);//"每秒恢复 %d 耐久度" "Repairs %d durability per second"
pos=add_prop1(b,pos,253,63,6);//"自动恢复数量" 
pos=add_prop1(b,pos,305,255,8);//"%+d%% 使敌人减少冰冻抵抗" "%d%%-%d%% to Enemy Cold Resistance"
pos=add_prop1(b,pos,306,255,8);//"%+d%% 使敌人减少火焰抵抗" "%d%%-%d%% to Enemy Fire Resistance"
pos=add_prop1(b,pos,307,255,8);//"%+d%% 使敌人减少闪电抵抗" "%d%%-%d%% to Enemy Lightning Resistance"
pos=add_prop1(b,pos,308,255,8);//"%+d%% 使敌人减少毒素抵抗" "%d%%-%d%% to Enemy Poison Resistance"
pos=add_prop1(b,pos,308,255,8);//"%+d%% 使敌人减少毒素抵抗" "%d%%-%d%% to Enemy Poison Resistance"
pos=add_prop1(b,pos,329,511,9);//"%+d%% 火焰技能伤害" 
pos=add_prop1(b,pos,330,511,9);//"%+d%% 闪电技能伤害" 
pos=add_prop1(b,pos,331,511,9);//"%+d%% 冰冻技能伤害" 
pos=add_prop1(b,pos,332,511,9);//"%+d%% 毒素技能伤害" 
pos=add_prop1(b,pos,332,511,9);//"%+d%% 毒素技能伤害" 
pos=add_prop1(b,pos,333,255,8);//"-%d%% 敌人火焰抵抗" "%d%%-%d%% to Enemy Fire Resistance"
pos=add_prop1(b,pos,334,255,8);//"-%d%% 敌人闪电抵抗" "%d%%-%d%% to Enemy Lightning Resistance"
pos=add_prop1(b,pos,335,255,8);//"-%d%% 敌人冰冻抵抗" "%d%%-%d%% to Enemy Cold Resistance"
pos=add_prop1(b,pos,336,255,8);//"-%d%% 敌人毒素抵抗" "%d%%-%d%% to Enemy Poison Resistance"
pos=add_prop1(b,pos,336,255,8);//"-%d%% 敌人毒素抵抗" "%d%%-%d%% to Enemy Poison Resistance"
*/
	/*
	pos=add_prop2(b,pos,17,511,511,9,9);//"From %d%% - %d%% Enhanced Damage"
	pos=add_prop2(b,pos,48,255,511,8,9);//"增加 %d-%d 火焰伤害"
	pos=add_prop3(b,pos,57,1023,1023,25,10,10,9);//"增加 %d-%d 毒素伤害 %d 秒"
	pos=add_prop1(b,pos,60,127,7);//"%d%% 生命于击中时偷取"
	pos=add_prop1(b,pos,68,127,7);//"%d%% 增加攻击速度"
	pos=add_prop1(b,pos,89,15,4);//"%+d 照亮范围"
pos=add_prop2(b,pos,97,9,63,9,6);//+63 to Critical Strike
pos=add_prop2(b,pos,97,23,63,9,6);//+63 to Penetrate
pos=add_prop2(b,pos,97,24,63,9,6);//+63 to Charged Strike
pos=add_prop2(b,pos,97,26,63,9,6);//+63 to Strafe
pos=add_prop2(b,pos,97,35,63,9,6);//+63 to Lightning Fury
pos=add_prop2(b,pos,97,37,63,9,6);//+63 to Warmth
pos=add_prop2(b,pos,97,44,63,9,6);//+63 to Frost Nova
pos=add_prop2(b,pos,97,48,63,9,6);//+63 to Nova
pos=add_prop2(b,pos,97,52,63,9,6);//+63 to Enchant
pos=add_prop2(b,pos,97,53,63,9,6);//+63 to Chain Lightning
pos=add_prop2(b,pos,97,54,63,9,6);//+63 to Teleport
pos=add_prop2(b,pos,97,55,63,9,6);//+63 to Glacial Spike
pos=add_prop2(b,pos,97,57,63,9,6);//+63 to Thunder Storm
pos=add_prop2(b,pos,97,58,63,9,6);//+63 to Energy Shield
pos=add_prop2(b,pos,97,59,63,9,6);//+63 to Blizzard
pos=add_prop2(b,pos,97,61,63,9,6);//+63 to Fire Mastery
pos=add_prop2(b,pos,97,63,63,9,6);//+63 to Lightning Mastery
pos=add_prop2(b,pos,97,65,63,9,6);//+63 to Cold Mastery
pos=add_prop2(b,pos,97,66,63,9,6);//+63 to Amplify Damage
pos=add_prop2(b,pos,97,69,63,9,6);//+63 to Skeleton Mastery
pos=add_prop2(b,pos,97,70,63,9,6);//+63 to Raise Skeleton
pos=add_prop2(b,pos,97,71,63,9,6);//+63 to Dim Vision
pos=add_prop2(b,pos,97,80,63,9,6);//+63 to Raise Skeletal Mage
pos=add_prop2(b,pos,97,86,63,9,6);//+63 to Attract
pos=add_prop2(b,pos,97,89,63,9,6);//+63 to Summon Resist
pos=add_prop2(b,pos,97,91,63,9,6);//+63 to Lower Resist
pos=add_prop2(b,pos,97,92,63,9,6);//+63 to Poison Nova
pos=add_prop2(b,pos,97,87,63,9,6);//+63 to Decrepify
pos=add_prop2(b,pos,97,95,63,9,6);//+63 to Revive
pos=add_prop2(b,pos,97,143,63,9,6);//+63 to Leap Attack
pos=add_prop2(b,pos,97,149,63,9,6);//+63 to Battle Orders
	*/
//pos=add_prop2(b,pos,97,316,63,9,6);//+63 to Baal Nova
	wbits(b,pos,9,0x1ff);pos+=9;
	savebits(outpath,b,pos);
}
int main() {
	//make_charm();
}

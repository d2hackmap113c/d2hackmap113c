#include <stdio.h>
#include <string.h>
#include <assert.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
static char *qNames[]={0,"low","normal","superior","magic","set","rare","unique","crafted","tampered"};
struct bitstream {
	unsigned char *bufstart,*buf,*bufend;
	unsigned int v;
	int n,pos;
};
struct Stat {int id,value_bits,param_bits,add,shift;};
struct Stat stats[359]={
{0,8,0,32,0},{1,7,0,32,0},{2,7,0,32,0},{3,7,0,32,0},{4,0,0,0,0},
{5,0,0,0,0},{6,0,0,0,8},{7,9,0,32,8},{8,0,0,0,8},{9,8,0,32,8},
{10,0,0,0,8},{11,8,0,32,8},{12,0,0,0,0},{13,0,0,0,0},{14,0,0,0,0},
{15,0,0,0,0},{16,9,0,0,0},{17,9,0,0,0},{18,9,0,0,0},{19,10,0,0,0},
{20,6,0,0,0},{21,6,0,0,0},{22,7,0,0,0},{23,6,0,0,0},{24,7,0,0,0},
{25,8,0,0,0},{26,8,0,0,0},{27,8,0,0,0},{28,8,0,0,0},{29,0,0,0,0},
{30,0,0,0,0},{31,11,0,10,0},{32,9,0,0,0},{33,8,0,0,0},{34,6,0,0,0},
{35,6,0,0,0},{36,8,0,0,0},{37,8,0,0,0},{38,5,0,0,0},{39,8,0,50,0},
{40,5,0,0,0},{41,8,0,50,0},{42,5,0,0,0},{43,8,0,50,0},{44,5,0,0,0},
{45,8,0,50,0},{46,5,0,0,0},{47,0,0,0,0},{48,8,0,0,0},{49,9,0,0,0},
{50,6,0,0,0},{51,10,0,0,0},{52,8,0,0,0},{53,9,0,0,0},{54,8,0,0,0},
{55,9,0,0,0},{56,8,0,0,0},{57,10,0,0,0},{58,10,0,0,0},{59,9,0,0,0},
{60,7,0,0,0},{61,0,0,0,0},{62,7,0,0,0},{63,0,0,0,0},{64,0,0,0,0},
{65,0,0,0,0},{66,0,0,0,0},{67,7,0,30,0},{68,7,0,30,0},{69,0,0,0,0},
{70,0,0,0,0},{71,8,0,100,0},{72,9,0,0,0},{73,8,0,0,0},{74,6,0,30,0},
{75,7,0,20,0},{76,6,0,10,0},{77,6,0,10,0},{78,7,0,0,0},{79,9,0,100,0},
{80,8,0,100,0},{81,7,0,0,0},{82,9,0,20,0},{83,3,3,0,0},{84,0,0,0,0},
{85,9,0,50,0},{86,7,0,0,0},{87,7,0,0,0},{88,1,0,0,0},{89,4,0,4,0},
{90,24,0,0,0},{91,8,0,100,0},{92,7,0,0,0},{93,7,0,20,0},{94,7,0,64,0},
{95,0,0,0,0},{96,7,0,20,0},{97,6,9,0,0},{98,1,8,0,0},{99,7,0,20,0},
{100,0,0,0,0},{101,0,0,0,0},{102,7,0,20,0},{103,0,0,0,0},{104,0,0,0,0},
{105,7,0,20,0},{106,0,0,0,0},{107,3,9,0,0},{108,1,0,0,0},{109,9,0,0,0},
{110,8,0,20,0},{111,9,0,20,0},{112,7,0,-1,0},{113,7,0,0,0},{114,6,0,0,0},
{115,1,0,0,0},{116,7,0,0,0},{117,7,0,0,0},{118,1,0,0,0},{119,9,0,20,0},
{120,7,0,128,0},{121,9,0,20,0},{122,9,0,20,0},{123,10,0,128,0},{124,10,0,128,0},
{125,1,0,0,0},{126,3,3,0,0},{127,3,0,0,0},{128,5,0,0,0},{129,0,0,0,0},
{130,0,0,0,0},{131,0,0,0,0},{132,0,0,0,0},{133,0,0,0,0},{134,5,0,0,0},
{135,7,0,0,0},{136,7,0,0,0},{137,7,0,0,0},{138,7,0,0,0},{139,7,0,0,0},
{140,7,0,0,0},{141,7,0,0,0},{142,7,0,0,0},{143,7,0,0,0},{144,7,0,0,0},
{145,7,0,0,0},{146,7,0,0,0},{147,7,0,0,0},{148,7,0,0,0},{149,7,0,0,0},
{150,7,0,0,0},{151,5,9,0,0},{152,1,0,0,0},{153,1,0,0,0},{154,7,0,20,0},
{155,7,10,0,0},{156,7,0,0,0},{157,7,0,0,0},{158,7,0,0,0},{159,6,0,0,0},
{160,7,0,0,0},{161,0,0,0,0},{162,0,0,0,0},{163,0,0,0,0},{164,0,0,0,0},
{165,0,0,0,0},{166,0,0,0,0},{167,0,0,0,0},{168,0,0,0,0},{169,0,0,0,0},
{170,0,0,0,0},{171,0,0,0,0},{172,0,0,0,0},{173,0,0,0,0},{174,0,0,0,0},
{175,0,0,0,0},{176,0,0,0,0},{177,0,0,0,0},{178,0,0,0,0},{179,9,10,0,0},
{180,9,10,0,0},{181,3,0,0,0},{182,0,0,0,0},{183,0,0,0,0},{184,0,0,0,0},
{185,0,0,0,0},{186,0,0,0,0},{187,0,0,0,0},{188,3,16,0,0},{189,0,0,0,0},
{190,0,0,0,0},{191,0,0,0,0},{192,0,0,0,0},{193,0,0,0,0},{194,4,0,0,0},
{195,7,16,0,0},{196,7,16,0,0},{197,7,16,0,0},{198,7,16,0,0},{199,7,16,0,0},
{200,0,0,0,0},{201,7,16,0,0},{202,0,0,0,0},{203,0,0,0,0},{204,16,16,0,0},
{205,0,0,0,0},{206,0,0,0,0},{207,0,0,0,0},{208,0,0,0,0},{209,0,0,0,0},
{210,0,0,0,0},{211,0,0,0,0},{212,0,0,0,0},{213,0,0,0,0},{214,6,0,0,0},
{215,6,0,0,0},{216,6,0,0,8},{217,6,0,0,8},{218,6,0,0,0},{219,6,0,0,0},
{220,6,0,0,0},{221,6,0,0,0},{222,6,0,0,0},{223,6,0,0,0},{224,6,0,0,0},
{225,6,0,0,0},{226,6,0,0,0},{227,6,0,0,0},{228,6,0,0,0},{229,6,0,0,0},
{230,6,0,0,0},{231,6,0,0,0},{232,6,0,0,0},{233,6,0,0,0},{234,6,0,0,0},
{235,6,0,0,0},{236,6,0,0,0},{237,6,0,0,0},{238,5,0,0,0},{239,6,0,0,0},
{240,6,0,0,0},{241,6,0,0,0},{242,6,0,0,0},{243,6,0,0,0},{244,6,0,0,0},
{245,6,0,0,0},{246,6,0,0,0},{247,6,0,0,0},{248,6,0,0,0},{249,6,0,0,0},
{250,6,0,0,0},{251,0,0,0,0},{252,6,0,0,0},{253,6,0,0,0},{254,8,0,0,0},
{255,0,0,0,0},{256,0,0,0,0},{257,0,0,0,0},{258,0,0,0,0},{259,0,0,0,0},
{260,0,0,0,0},{261,0,0,0,0},{262,0,0,0,0},{263,0,0,0,0},{264,0,0,0,0},
{265,0,0,0,0},{266,0,0,0,0},{267,0,0,0,0},{268,22,0,0,0},{269,22,0,0,0},
{270,22,0,0,0},{271,22,0,0,0},{272,22,0,0,0},{273,22,0,0,0},{274,22,0,0,0},
{275,22,0,0,0},{276,22,0,0,0},{277,22,0,0,0},{278,22,0,0,0},{279,22,0,0,0},
{280,22,0,0,0},{281,22,0,0,0},{282,22,0,0,0},{283,22,0,0,0},{284,22,0,0,0},
{285,22,0,0,0},{286,22,0,0,0},{287,22,0,0,0},{288,22,0,0,0},{289,22,0,0,0},
{290,22,0,0,0},{291,22,0,0,0},{292,22,0,0,0},{293,22,0,0,0},{294,22,0,0,0},
{295,22,0,0,0},{296,22,0,0,0},{297,22,0,0,0},{298,22,0,0,0},{299,22,0,0,0},
{300,22,0,0,0},{301,22,0,0,0},{302,22,0,0,0},{303,22,0,0,0},{304,0,0,0,0},
{305,8,0,50,0},{306,8,0,50,0},{307,8,0,50,0},{308,8,0,50,0},{309,0,0,0,0},
{310,0,0,0,0},{311,0,0,0,0},{312,0,0,0,0},{313,0,0,0,0},{314,0,0,0,0},
{315,0,0,0,0},{316,0,0,0,0},{317,0,0,0,0},{318,0,0,0,0},{319,0,0,0,0},
{320,0,0,0,0},{321,0,0,0,0},{322,0,0,0,0},{323,0,0,0,0},{324,6,0,0,0},
{325,0,0,0,0},{326,0,0,0,0},{327,0,0,0,0},{328,0,0,0,0},{329,9,0,50,0},
{330,9,0,50,0},{331,9,0,50,0},{332,9,0,50,0},{333,8,0,0,0},{334,8,0,0,0},
{335,8,0,0,0},{336,8,0,0,0},{337,8,0,0,0},{338,7,0,0,0},{339,7,0,0,0},
{340,7,0,0,0},{341,8,0,0,0},{342,8,0,0,0},{343,8,0,0,0},{344,8,0,0,0},
{345,8,0,0,0},{346,8,0,0,0},{347,8,0,0,0},{348,8,0,0,0},{349,8,0,0,0},
{350,0,0,0,0},{351,0,0,0,0},{352,0,0,0,0},{353,0,0,0,0},{354,0,0,0,0},
{355,0,0,0,0},{356,2,0,0,0},{357,9,0,50,0},{358,8,0,0,0},};
void bitstream_init(struct bitstream *bs,void *buf,void *end) {
	bs->bufstart=(unsigned char *)buf;
	bs->bufend=(unsigned char *)end;
	bs->buf=bs->bufstart;
	bs->v=0;bs->n=0;bs->pos=0;
}
int rbit(struct bitstream *bs,int n) {
	if (n<=0) return 0;
	if (n>16) {
		int low=rbit(bs,16);
		int high=rbit(bs,n-16);
		return (high<<16)|low;
	}
	while (bs->n < n) {
		if (bs->buf>=bs->bufend) return 0;
		bs->v|=((*bs->buf++)&0xFF)<<bs->n;
		bs->n+=8;
	}
	int r=bs->v & ((1<<n)-1);
	bs->v >>= n;bs->n -= n;bs->pos+=n;
	return r;
}
void bitstream_seek(struct bitstream *bs,int pos) {
	bs->buf=bs->bufstart+(pos>>3);
	bs->v=0;bs->n=0;
	rbit(bs,pos&7);bs->pos=pos;
}
char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");assert(fp);
	fseek(fp,0,2);int size=ftell(fp);*psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
enum ItemFlag{
	ITEMFLAG_IDENTIFIED  =	0x00000010,
	ITEMFLAG_SWITCHIN  =	0x00000040,
	ITEMFLAG_SWITCHOUT =	0x00000080,
	ITEMFLAG_BROKEN =		0x00000100,
	ITEMFLAG_SOCKETED =		0x00000800,
	ITEMFLAG_INSTORE =		0x00002000,
	ITEMFLAG_NAMED =		0x00008000,//for ears, personalized items etc
	ITEMFLAG_ISEAR =		0x00010000,
	ITEMFLAG_INEXPENSIVE =	0x00020000,//always costs 1 for repair /	sell
	ITEMFLAG_COMPACTSAVE =	0x00200000,
	ITEMFLAG_ETHEREAL =		0x00400000,
	ITEM_PERSONALIZED =		0x01000000,
	ITEMFLAG_RUNEWORD =		0x04000000
};
void show_item() {
	int complete=1;
	int size;
	char *path;
	path="/n/t/items/add3fire.d2i";
	path="/n/t/charm.d2i";
	path="/n/t/t2.d2i";
	char *buf=load(path,&size);
	struct bitstream bs={0};
	bitstream_init(&bs,buf,buf+size);
	if (complete) {
		int magic=rbit(&bs,16);assert(magic==0x4d4a); //d2common_28DD0
	}
	int flags=rbit(&bs,32); //d2common_28DE6
	printf("flag=%X ",flags);
	if (flags&ITEMFLAG_IDENTIFIED) printf("identified,");
	if (flags&ITEMFLAG_SWITCHIN) printf("switchIn,");
	if (flags&ITEMFLAG_SWITCHOUT) printf("switchOut,");
	if (flags&ITEMFLAG_BROKEN) printf("broken,");
	if (flags&ITEMFLAG_SOCKETED) printf("socketed,");
	if (flags&ITEMFLAG_INSTORE) printf("instore,");
	if (flags&ITEMFLAG_NAMED) printf("named,");
	if (flags&ITEMFLAG_ISEAR) printf("ear,");
	if (flags&ITEMFLAG_INEXPENSIVE) printf("startgear,");
	if (flags&ITEMFLAG_COMPACTSAVE) printf("compact,");
	if (flags&ITEMFLAG_ETHEREAL) printf("ethereal,");
	if (flags&ITEM_PERSONALIZED) printf("personalized,");
	if (flags&ITEMFLAG_RUNEWORD) printf("runeword,");
	printf("\n");
	int format=rbit(&bs,10); //d2common_271ED
	int mode=rbit(&bs,3); //d2common_2720B
	printf(" format=0x%x mode=%d",format,mode);
	if (mode==3||mode==5) {
		int x=rbit(&bs,16); //d2common_27363
		int y=rbit(&bs,16); //d2common_273A0
		printf(" pos=(%d,%d)",x,y);
	} else {
		int bodyLocation=rbit(&bs,4); //d2common_27251
/*|  1    | Helmet                  |
|  2    | Amulet                  |
|  3    | Armor                   |
|  4    | Weapon (Right)          |
|  5    | Weapon (Left)           |
|  6    | Ring (Right)            |
|  7    | Ring (Left)             |
|  8    | Belt                    |
|  9    | Boots                   |
| 10    | Gloves                  |
| 11    | Alternate Weapon (Right)|
| 12    | Alternate Weapon (Left) |
*/
		int x=rbit(&bs,4); //d2common_272A0
		int y=rbit(&bs,4); //d2common_272EF
		int itemLocation=rbit(&bs,3)-1; //d2common_27323
		//-1 equiped 0 Inventory , 3 cube , 4 stash  
		printf(" body=%d x=%d y=%d item=%d",bodyLocation,x,y,itemLocation);
	}
	int code=rbit(&bs,32);char *codeS=(char *)&code; //d2common_273D1
	codeS[3]=0;printf(" code=%s",codeS);
	int usedSocket=rbit(&bs,3); //d2common_273F3
	printf(" usedSocket=%d",usedSocket);
	if (complete) {
		int seed=rbit(&bs,32);printf(" seed=%X",seed); //d2common_27406
	}
	int lv=rbit(&bs,7);printf(" lv=%d",lv); //d2common_27450
	//1:low 2:normal 3:superior 4:magic 5:set 6:rare 7:unique 8:crafted 9:tampered
	int q=rbit(&bs,4);printf(" q=%d %s",q,q>9?"":qNames[q]); //d2common_27474
	int customGraph=rbit(&bs,1); //d2common_274D8 not sure? ItemTypeTxt+23
	printf(" graph=%d",customGraph);
	if (customGraph) {
		int invGfxIdx=rbit(&bs,3); //d2common_2753F pItemData->nInvGfxIdx
		printf(":%d",invGfxIdx);
	}
	printf("\n");
	int classSpecific=rbit(&bs,1); //d2common_275BC
	printf(" classSpec=%d",classSpecific); //d2common_27544
	if (classSpecific) {
		int bits=rbit(&bs,11);printf(":%d",bits); //d2common_275D6
	}
	//1:low 2:normal 3:superior 4:magic 5:set 6:rare 7:unique 8:crafted 9:tampered
	switch (q) {
		case 1:case 3: { //low & superior
			int fileindex=rbit(&bs,3);printf(" fileindex=%d",fileindex); //d2common_27646
/*
| 0     | Crude       |
| 1     | Cracked     |
| 2     | Damaged     |
| 3     | Low Quality |
*/
			break;
		}
		case 6:case 8: { //rare & crafted
			int rarePrefix=rbit(&bs,16);printf(" rarePrefix=%d",rarePrefix); //d2common_276B4
			int rareSuffix=rbit(&bs,16);printf(" rareSuffix=%d",rareSuffix); //d2common_276DC
			
			break;
		}
		case 5:case 7: {//set & unique
			if (!complete&&!(flags&ITEMFLAG_IDENTIFIED)) break;
			int fileindex=rbit(&bs,12);printf(" fileindex=%d",fileindex); //d2common_278D8
			break;
		}
		case 4: { //magic
			int prefix=rbit(&bs,11);printf(" prefix=%d",prefix); //d2common_2787C
			int suffix=rbit(&bs,11);printf(" suffix=%d",suffix); //d2common_27888
			break;
		}
	}
	if (flags&ITEMFLAG_RUNEWORD) {
		int wLocaleTxtNo=rbit(&bs,16);printf(" wLocaleTxtNo=%d",wLocaleTxtNo); //d2common_27A8C
	}
	if (complete) {
		int has20=rbit(&bs,1);
		if (has20) {
			//if (itemData+20) 1bit:1 32bit:itemData+1C 32bit:itemData+20 32bit:0
		}
	}
	//if (armor) 11bits of DEF
	printf("\n");
	printf("bit=%d\n",bs.pos);
}
int main() {
	show_item();
}

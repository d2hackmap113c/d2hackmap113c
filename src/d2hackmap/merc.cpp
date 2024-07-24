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
	int expansion; //offset0, 0:classic 0x64:expansion,
	int type; //offset4
	int txt; //offset8 A1:10f A2:152 A3:167 A5:230,231
	int act; //offsetC, 1-5
	int off10,off14,off18;
	int level; //offset1C
	int off20;
	int life; //offset24
	int lifePerLevel; //offset28
	int defense; //offset2C
	int defensePerLevel; //offset30
	int strength; //offset34
	int strengthPer8Level; //offset38
	int dexterity; //offset3C
	int dexterityPer8Level; //offset40
	int off44[6];
	int resist; //offset5C
	int resistPer4Level; //offset60
	int off64[5];
	int skills[2]; //offset78
	int off80[17];
	char offc4[2];
	char skillLevel[2]; //offsetC6
	char offc8[4];
	char skillPer32Level[2]; //offsetCC
	char offCE[2];
	char offD0[3],idstrStart[32],idstrEnd[33]; //size 0x118
	short nameIdStart,nameIdEnd;
};



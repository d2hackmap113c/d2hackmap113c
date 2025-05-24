#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
int getFileSize(const char *path) {struct stat st;if (stat(path,&st)!=0) return -1;return st.st_size;}
int fileExist(const char *path) {return getFileSize(path)!=-1;}
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
void save(char *path,char *buf,int size) {
	FILE *fp=fopen(path,"wb+");assert(fp);
	int n=fwrite(buf,1,size,fp);assert(n==size);fclose(fp);
}
char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");if (!fp) printf("can't open %s\n",path);assert(fp);
	fseek(fp,0,2);int size=ftell(fp);*psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
static uint32_t mpq_crypt_table[0x500];
void init_crypt_table() {
	uint32_t seed=0x00100001;
	for(int i = 0; i < 0x100; i++) {
		for(int j = i; j < 0x500; j += 0x100) {
			seed=(seed*125+3)%0x2AAAAB;uint32_t high=(seed&0xFFFF)<<16;
			seed=(seed*125+3)%0x2AAAAB;mpq_crypt_table[j]=high|(seed&0xFFFF);
		}
	}
	//for (int i=32;i<127;i++) printf("%d %c %x\n",i,i,mpq_crypt_table[i]);exit(0);
}
uint32_t mpq_hashstring(const char *s, uint32_t offset) {
	uint32_t a=0x7FED7FED,b=0xEEEEEEEE,c;
	if (s) while (*s) {c=toupper(*s++);a=mpq_crypt_table[offset+c]^(a+b);b=c+a+b+(b<<5)+3;}
	return a;
}
void encrypt_block(uint32_t *in_buf, uint32_t in_size, uint32_t seed) {
	for (uint32_t seed2=0xEEEEEEEE;in_size>=4;in_size-=4) {
		seed2+=mpq_crypt_table[0x400+(seed&0xFF)];
		uint32_t ch=*in_buf^(seed+seed2);
		seed=((~seed<<0x15)+0x11111111)|(seed>>0x0B);seed2=*in_buf+seed2+(seed2<<5)+3;
		*in_buf++=ch;
	}
}
void decrypt_block(uint32_t *in_buf, int in_size, uint32_t seed) {
	for (uint32_t seed2=0xEEEEEEEE;in_size>=4;in_size-=4) {
		seed2+=mpq_crypt_table[0x400+(seed&0xFF)];
		uint32_t ch=*in_buf^(seed+seed2);
		seed=((~seed<<0x15)+0x11111111)|(seed>>0x0B);seed2=ch+seed2+(seed2<<5)+3;
		*in_buf++=ch;
	}
}
int search_key(uint32_t *in_buf, uint32_t coffset0, uint32_t block_size, uint32_t *key) {
	uint32_t eoffset0=in_buf[0],eoffset1=in_buf[1];
	uint32_t temp = (eoffset0^coffset0)-0xEEEEEEEE;
	for (int i = 0; i < 0x100; i++) {
		uint32_t seed  = temp - mpq_crypt_table[0x400 + i];
		if ((seed&0xFF)!=i) continue;
		uint32_t seed2=0xEEEEEEEEU+mpq_crypt_table[0x400+i];
		uint32_t offset0=eoffset0^(seed+seed2);
		if (offset0 != coffset0) continue;
		uint32_t saveseed=seed+1;
		seed=((~seed<<0x15)+0x11111111)|(seed>>0x0B);
		seed2=offset0+seed2+(seed2<<5)+3;
		seed2+=mpq_crypt_table[0x400+(seed&0xFF)];
		uint32_t offset1=eoffset1^(seed+seed2);
		if ((offset1-offset0)<=block_size) {*key=saveseed;return 1;}
	}
	return 0;
}

typedef BOOL (WINAPI* funcSCompCompress)(LPVOID lpvDestinationMem, LPDWORD lpdwCompressedSize, LPVOID lpvSourceMem, DWORD dwDecompressedSize, DWORD dwCompressionType, DWORD dwCompressionSubType, DWORD dwWAVQuality);
typedef BOOL (WINAPI* funcSCompDecompress)(LPVOID lpvDestinationMem, LPDWORD lpdwDecompressedSize, LPVOID lpvSourceMem, DWORD dwCompressedSize);
funcSCompCompress stormSCompCompress = 0;
funcSCompDecompress stormSCompDecompress = 0;
#define MPQ_FLAG_EXISTS			0x80000000	
#define MPQ_FLAG_SINGLE			0x01000000	
#define MPQ_FLAG_MODENCRYPTED			0x00020000	
#define MPQ_FLAG_ENCRYPTED			0x00010000	
#define MPQ_FLAG_COMPRESSED			0x0000FF00
#define MPQ_FLAG_COMPRESS_PKZIP		0x00000100
#define MPQ_FLAG_COMPRESS_MULTI		0x00000200
#define MPQ_FLAG_CRC				0x04000000	
struct mpq_header {
	int magic,header_size,archive_size;short version,block_size_shift512;
	int hash_offset,block_offset,nhash,nblock;
};
struct mpq_hash {int hash_a,hash_b,locale_platform,block_id;};
struct mpq_block {int offset,packed_size,unpacked_size,flags;};
struct mpq_block_info {int hid;char *name;char type[4];};
struct mpq {
	FILE *fp;char *path,*listpath;
	int block_size,modified;
	struct mpq_header mheader;
	struct mpq_hash *mhash;
	struct mpq_block *mblock;
	struct mpq_block_info *mblock_info;
};
struct mpq mpqs[32]={0};int nmpq=0;
char *globalListPath=NULL;
FILE *logfp=NULL;

void extract_block(struct mpq *pmpq,char *name,int block_id,FILE *outfp) {
	struct mpq_block *p=&pmpq->mblock[block_id];
	char in_buf1[4096+16],out_buf1[4096],packed_offset_buf[4096];
	assert(pmpq->block_size<=4096);
	char *in_buf=in_buf1+8;
	char *out_ptr=out_buf1;
	int out_left=p->unpacked_size;
	fprintf(logfp,"extracting %s block%d flags%x off=%d %d/%d\n",
		name?name:"",block_id,p->flags,p->offset,p->packed_size,p->unpacked_size);
	fseek(pmpq->fp,p->offset,0);
	if (p->packed_size==p->unpacked_size) {
		assert(p->flags==0||p->flags==0x80000000);
		//if (p->flags==0) assert(out_left==0);
		//else assert(p->packed_size==p->unpacked_size);
		for (int left=p->packed_size;left>0;) {
			int b=left>4096?4096:left;fread(in_buf,1,b,pmpq->fp);fwrite(in_buf,1,b,outfp);left-=b;
		}
		return;
	}
	int seed=0,ret;
	int nblocks=p->unpacked_size>>(pmpq->mheader.block_size_shift512+9);
	if (p->unpacked_size&(pmpq->block_size-1)) nblocks++;
	if (p->flags&MPQ_FLAG_SINGLE) nblocks=1;
	int noffsets=nblocks+1;if (p->flags&MPQ_FLAG_CRC) noffsets++;
	int offsets_size=noffsets*sizeof(int);
	uint32_t *packed_offset=packed_offset_buf;
	if (offsets_size>4096) packed_offset=malloc(offsets_size);
	if (p->flags&MPQ_FLAG_SINGLE) {
		packed_offset[0]=0;packed_offset[1]=p->packed_size;
	} else if (p->flags&MPQ_FLAG_COMPRESSED) {
		fread(packed_offset,1,offsets_size,pmpq->fp);
		if (packed_offset[0]!=offsets_size&&packed_offset[0]!=offsets_size+4) p->flags|=MPQ_FLAG_ENCRYPTED;
		if (p->flags&MPQ_FLAG_ENCRYPTED) {
			if (!search_key(packed_offset, offsets_size, pmpq->block_size, &seed)) assert(0);
			decrypt_block(packed_offset, offsets_size, seed - 1);
			//printf("off[0]=%d off[%d]=%d %d\n",packed_offset[0],noffsets-1,packed_offset[noffsets-1],offsets_size);
			assert(packed_offset[0]==offsets_size);
			assert(packed_offset[noffsets-1]==p->packed_size);
		}
	} else {
		for (int i=0,off=0;i<nblocks;i++,off+=pmpq->block_size) packed_offset[i]=off;
		packed_offset[nblocks]=p->unpacked_size;
	}
	if (name&&p->flags&MPQ_FLAG_ENCRYPTED) {
		int key2=0;
		if (p->flags&MPQ_FLAG_ENCRYPTED) key2 = mpq_hashstring(name,0x300);
		if (p->flags&MPQ_FLAG_MODENCRYPTED) key2 = (key2 + p->offset) ^ p->unpacked_size;
		assert(seed==key2);
	}
	for (int i = 0; i < nblocks; i++) {
		int out_size=i<nblocks-1?pmpq->block_size:p->unpacked_size-pmpq->block_size*i;
		assert(out_size<=4096);
		int block_offset=p->offset+packed_offset[i];
		int in_size=packed_offset[i+1]-packed_offset[i];assert(in_size<=pmpq->block_size);
		fread(in_buf,1,in_size,pmpq->fp);
		if (p->flags&MPQ_FLAG_ENCRYPTED) decrypt_block((uint32_t *)in_buf, in_size, seed+i);
		int compression=p->flags&MPQ_FLAG_COMPRESSED;
		if (in_size==out_size) compression=0;
		switch (compression) {
			case 0:fwrite(in_buf,1,in_size,outfp);out_left-=in_size;break;
			case MPQ_FLAG_COMPRESS_PKZIP:
				in_buf[-1]=8;
				ret=stormSCompDecompress(out_ptr, &out_size, in_buf-1, in_size+1);assert(ret);
				fwrite(out_ptr,1,out_size,outfp);
				out_left-=out_size;
				break;
			case MPQ_FLAG_COMPRESS_MULTI:
				ret=stormSCompDecompress(out_ptr, &out_size, in_buf, in_size);assert(ret);
				fwrite(out_ptr,1,out_size,outfp);
				out_left-=out_size;
				break;
			default:
				assert(0);
		}
	}
	assert(out_left==0);
	if (packed_offset!=packed_offset_buf) free(packed_offset);
}
void extract_block_to_file(struct mpq *pmpq,char *name,int block_id,char *out_path) {
	FILE *fp_out=fopen(out_path,"wb+");assert(fp_out);
	extract_block(pmpq,name,block_id,fp_out);
	fclose(fp_out);
}
int get_hash_id(struct mpq *pmpq,char *name) {
	char buf[256];strcpy(buf,name);name=buf;if (name[0]=='/') name++;
	for (char *s=name;*s;s++) if (*s=='/') *s='\\';
	int pos = mpq_hashstring(name,0)%pmpq->mheader.nhash;
	int hash1 = mpq_hashstring(name,0x100),hash2 = mpq_hashstring(name,0x200);
	for (int i=0;i<pmpq->mheader.nhash;i++) {
		struct mpq_hash *p=&pmpq->mhash[pos];
		if (p->block_id==-1) break;
		if (p->hash_a==hash1&&p->hash_b==hash2) return pos;
		pos++;if (pos>=pmpq->mheader.nhash) pos=0;
	}
	return -1;
}
int get_block_id(struct mpq *pmpq,char *name) {int hid=get_hash_id(pmpq,name);if (hid<0) return -1;return pmpq->mhash[hid].block_id;}
static int fileSame(FILE *fp1,FILE *fp2,int size) {
	char buf1[4096],buf2[4096];
	while (size>0) {
		int b=size;if (b>4096) b=4096;
		int len1=fread(buf1,1,b,fp1);int len2=fread(buf2,1,b,fp2);
		if (len1!=len2) {printf("read Error %d %d\n",len1,len2);return 0;}
		size-=len1;
		if (memcmp(buf1,buf2,len1)!=0) return 0;
	}
	return 1;
}
void mpq_compare(struct mpq *mpq1,struct mpq *mpq2) {
	if (mpq1->mheader.nhash!=mpq2->mheader.nhash) {printf("Error hash size not match\n");return;}
	for (int i=0;i<mpq1->mheader.nhash;i++) {
		struct mpq_hash *p1=&mpq1->mhash[i];
		struct mpq_hash *p2=&mpq2->mhash[i];
		if (p1->block_id==-1&&p2->block_id==-1) continue;
		if (p1->block_id==-1||p2->block_id==-1) {
			printf("Hash %d block existence not match %d %d ",i,p1->block_id,p2->block_id);
			if (p1->block_id!=-1) printf("%08X_%08X\n",p1->hash_a,p1->hash_b);
			if (p2->block_id!=-1) printf("%08X_%08X\n",p2->hash_a,p2->hash_b);
			continue;
		}
		if (p1->hash_a!=p2->hash_a||p1->hash_b!=p2->hash_b) {
			printf("Hash %d not match %x %x %x %x\n",i,p1->hash_a,p2->hash_a,p1->hash_b,p2->hash_b);continue;
		}
		struct mpq_block *b1=&mpq1->mblock[p1->block_id];
		struct mpq_block *b2=&mpq2->mblock[p2->block_id];
		if (b1->flags&&b1->flags!=0x80000000||b1->packed_size!=b1->unpacked_size) 
		{printf("Error:mpq compressed or encrypted\n");continue;}
		if (b2->flags&&b2->flags!=0x80000000||b2->packed_size!=b2->unpacked_size) 
		{printf("Error:mpq compressed or encrypted\n");continue;}
		if (b1->unpacked_size!=b2->unpacked_size) {
			printf("Hash %d %08X_%08X size mismatch\n",i,p1->hash_a,p1->hash_b);continue;
		}
		fseek(mpq1->fp,b1->offset,0);
		fseek(mpq2->fp,b2->offset,0);
		if (!fileSame(mpq1->fp,mpq2->fp,b1->unpacked_size)) {
			printf("Hash %d %08X_%08X not same\n",i,p1->hash_a,p1->hash_b);continue;
		}
	}
}
void check_mpq(struct mpq *pmpq) {
	int tsize=0,tpack=0,ne=0,npk=0,nmul=0,nc=0,ncrc=0;
	for (int i=0;i<pmpq->mheader.nblock;i++) {
		struct mpq_block *p=&pmpq->mblock[i];
		tsize+=p->unpacked_size;
		tpack+=p->packed_size;
		if (p->flags&MPQ_FLAG_ENCRYPTED) ne++;
		if (p->flags&MPQ_FLAG_COMPRESSED) {
			if (p->flags&MPQ_FLAG_COMPRESS_PKZIP) npk++;
			else if (p->flags&MPQ_FLAG_COMPRESS_MULTI) nmul++;
			else nc++;
		}
		if (p->flags&MPQ_FLAG_CRC) ncrc++;
		if (p->flags==0) {
			//printf("deleted file %d %d/%d\n",i,p->packed_size,p->unpacked_size);
			continue;
		}
		if (p->unpacked_size<p->packed_size) {
			//printf("ERROR size %d/%d\n",p->packed_size,p->unpacked_size);
		}
		//&&p->flags!=0x80010000
		if (p->flags!=0x80000000&&p->flags!=0x80000100&&p->flags!=0x80010100
			&&p->flags!=0x80000200&&p->flags!=0x80010200&&p->flags!=0x80030200) {
			printf("AAA block%d flags=%x %d/%d\n",i,p->flags,p->packed_size,p->unpacked_size);
		}
	}
	printf("total size: %d/%d\n",tpack,tsize);
	printf("encrypted=%d pkzip=%d multi=%d other_compress=%d crc=%d\n",ne,npk,nmul,nc,ncrc);
}
int force_overwrite_path_name=0;
int mpq_add_path(struct mpq *pmpq,char *spath) {
	int hid=get_hash_id(pmpq,spath);if (hid<0) return 0;
	int bid=pmpq->mhash[hid].block_id;assert(bid>=0);
	struct mpq_block_info *pbi=&pmpq->mblock_info[bid];pbi->hid=hid;
	if (!force_overwrite_path_name&&pbi->name&&stricmp(pbi->name,spath)==0) return 2;
	pbi->name=strdup(spath);pmpq->modified=1;return 1;
}
int mpq_add_path_list(struct mpq *pmpq,char *listfile) {
	printf("matching %s\n",listfile);
	FILE *fplist=fopen(listfile,"r");assert(fplist);
	int add=0,match=0,not_match=0,n=0,nm=100000;
	while (1) {
		char buf[256];char *line=fgets(buf,256,fplist);if (!line) break;
		int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[0]=='#') {line=strchr(line,'\t');assert(line);line++;}
		int ret=mpq_add_path(pmpq,line);
		if (ret==1) add++;else if (ret==2) match++;else not_match++;
		if (n++ > nm) {printf("(%d/%d/%d),",add,match,not_match);fflush(stdout);nm=n+100000;}
	}
	fclose(fplist);
	printf("add=%d match=%d not_match=%d\n",add,match,not_match);
	return add;
}
void mpq_save_list(struct mpq *pmpq) {
	printf("writing %s\n",pmpq->listpath);
	FILE *fplist=fopen(pmpq->listpath,"w+");assert(fplist);
	for (int bid=0;bid<pmpq->mheader.nblock;bid++) {
		struct mpq_block *pb=&pmpq->mblock[bid];
		if (!pb->flags) continue;
		struct mpq_block_info *pbi=&pmpq->mblock_info[bid];
		if (pbi->hid<0) {printf("ERROR unrelated block %d\n",bid);continue;}
		struct mpq_hash *ph=&pmpq->mhash[pbi->hid];
		if (!pbi->type[0]) {// ||pbi->type[0]=='?') {
			printf("%d,",bid);fflush(stdout);
			char buf[16];fseek(pmpq->fp,pb->offset,0);fread(buf,1,16,pmpq->fp);
			if (*(int *)buf==0x46464952) memcpy(pbi->type,"wav",4);
			else if (*(short *)buf==6) memcpy(pbi->type,"dc6",4);
			else {
				int asc=1;
				for (int i=0;i<16;i++) {
					char c=buf[i];if (c=='\r'||c=='\n'||c=='\t'||0x20<=c&&c<127);else {asc=0;break;}
				}
				if (asc) memcpy(pbi->type,"txt",4);
				else {
					memcpy(pbi->type,"???",4);
					//hex(0,buf,16);
				}
			}
		}
		fprintf(fplist,"%d %04X %08X_%08X %X %d\t%s %s\n",bid,pbi->hid,ph->hash_a,ph->hash_b,ph->locale_platform,
			pb->unpacked_size,pbi->type,pbi->name?pbi->name:"");
	}
	fclose(fplist);pmpq->modified=0;
}
struct mpq *mpq_open(char *inpath,char *listprefix) {
	char listfile[256],listpath[256];
	assert(nmpq<32);struct mpq *pmpq=&mpqs[nmpq++];
	FILE *fp=fopen(inpath,"rb");if (!fp) printf("can't open %s\n",inpath);assert(fp);
	pmpq->fp=fp;pmpq->path=strdup(inpath);
	sprintf(listfile,"%s.in.txt",listprefix);
	sprintf(listpath,"%s.txt",listprefix);
	pmpq->listpath=strdup(listpath);pmpq->modified=0;
	fread(&pmpq->mheader,1,sizeof(struct mpq_header),fp);
	assert(pmpq->mheader.magic==0x1A51504D);
	if (pmpq->mheader.version!=0) {
		printf("unknown mpq version %X\n",pmpq->mheader.version);
		hex(0,&pmpq->mheader,sizeof(struct mpq_header));
	}
	pmpq->block_size=512<<pmpq->mheader.block_size_shift512;assert((pmpq->block_size&(pmpq->block_size-1))==0);
	//printf("header=%d file=%d block_size=%d\n",pmpq->mheader.header_size,pmpq->mheader.archive_size,pmpq->block_size);
	//printf("hash_offset=%d block_offset=%d\n",pmpq->mheader.hash_offset,pmpq->mheader.block_offset);
	pmpq->mhash=malloc(sizeof(struct mpq_hash)*pmpq->mheader.nhash);
	pmpq->mblock_info=malloc(sizeof(struct mpq_block_info)*pmpq->mheader.nblock);
	memset(pmpq->mblock_info,0,sizeof(struct mpq_block_info)*pmpq->mheader.nblock);
	pmpq->mblock=malloc(sizeof(struct mpq_block)*pmpq->mheader.nblock);
	fseek(fp,pmpq->mheader.hash_offset,0);fread(pmpq->mhash,1,sizeof(struct mpq_hash)*pmpq->mheader.nhash,fp);
	decrypt_block((uint32_t *)pmpq->mhash,pmpq->mheader.nhash*sizeof(struct mpq_hash),mpq_hashstring("(hash table)",0x300));
	fseek(fp,pmpq->mheader.block_offset,0);fread(pmpq->mblock,1,sizeof(struct mpq_block)*pmpq->mheader.nblock,fp);
	decrypt_block((uint32_t *)pmpq->mblock,pmpq->mheader.nblock*sizeof(struct mpq_block),mpq_hashstring("(block table)",0x300));
	for (int bid=0;bid<pmpq->mheader.nblock;bid++) pmpq->mblock_info[bid].hid=-1;
	for (int hid=0;hid<pmpq->mheader.nhash;hid++) {int bid=pmpq->mhash[hid].block_id;if (bid>=0) pmpq->mblock_info[bid].hid=hid;}
	if (listprefix&&!fileExist(listpath)&&!fileExist(listfile)) {
		int hid=get_hash_id(pmpq,"(listfile)");
		if (hid>=0) {
			int bid=pmpq->mhash[hid].block_id;assert(bid>=0);
			pmpq->mblock_info[bid].hid=hid;pmpq->mblock_info[bid].name="(listfile)";
			extract_block_to_file(pmpq,"(listfile)",bid,listfile);
		}
	}
	int hasName=0,noName=0;
	if (listprefix&&fileExist(listpath)) {
		FILE *fplist=fopen(listpath,"r");assert(fplist);
		while (1) {
			char buf[256];char *line=fgets(buf,256,fplist);if (!line) break;
			int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
			int bid=strtol(line,NULL,10);assert(0<=bid&&bid<pmpq->mheader.nblock);
			struct mpq_block_info *pbi=&pmpq->mblock_info[bid];
			char *spath=strchr(line,'\t');assert(spath);spath++;
			memcpy(pbi->type,spath,3);pbi->type[3]=0;
			spath=strchr(spath,' ');assert(spath);spath++;
			if (spath[0]) pbi->name=strdup(spath);
			if (pbi->name) hasName++;else noName++;
		}
		fclose(fplist);
	} else if (listprefix&&fileExist(listfile)) {
		if (mpq_add_path_list(pmpq,listfile)) pmpq->modified=1;
	}
	printf("------ %s noname=%d/%d hash%d block%d------\n",inpath,noName,hasName+noName,
		pmpq->mheader.nhash,pmpq->mheader.nblock);
	if (noName&&globalListPath&&fileExist(globalListPath)) {
		if (mpq_add_path_list(pmpq,globalListPath)) pmpq->modified=1;
	}
	if (listprefix&&!fileExist(pmpq->listpath)||pmpq->modified) mpq_save_list(pmpq);
	return pmpq;
}
void mpq_close(struct mpq *pmpq) {
	free(pmpq->mhash);free(pmpq->mblock_info);free(pmpq->mblock);
	fclose(pmpq->fp);pmpq->fp=NULL;
}
void mpq_uncompress(char *inpath,char *listprefix,char *outpath) {
	struct mpq *pmpq=mpq_open(inpath,listprefix);
	FILE *outfp=fopen(outpath,"wb");assert(outfp);
	fwrite(&pmpq->mheader,1,sizeof(struct mpq_header),outfp);
	for (int i=0;i<pmpq->mheader.nblock;i++) {
		struct mpq_block *p=&pmpq->mblock[i];
		int off=ftell(outfp);
		extract_block(pmpq,NULL,i,outfp);
		p->offset=off;p->packed_size=p->unpacked_size;
		p->flags&=~(MPQ_FLAG_MODENCRYPTED|MPQ_FLAG_ENCRYPTED|MPQ_FLAG_COMPRESSED|MPQ_FLAG_CRC);
	}
	printf("writing hash at %d\n",ftell(outfp));
	pmpq->mheader.hash_offset=ftell(outfp);
	encrypt_block((uint32_t *)pmpq->mhash, pmpq->mheader.nhash*sizeof(struct mpq_hash), mpq_hashstring("(hash table)", 0x300));
	fwrite(pmpq->mhash,1,sizeof(struct mpq_hash)*pmpq->mheader.nhash,outfp);
	pmpq->mheader.block_offset=ftell(outfp);
	printf("writing block at %d\n",ftell(outfp));
	encrypt_block((uint32_t *)pmpq->mblock, pmpq->mheader.nblock*sizeof(struct mpq_block), mpq_hashstring("(block table)", 0x300));
	fwrite(pmpq->mblock,1,sizeof(struct mpq_block)*pmpq->mheader.nblock,outfp);
	pmpq->mheader.archive_size=ftell(outfp);
	fseek(outfp,0,0);
	printf("writing header at %d\n",ftell(outfp));
	fwrite(&pmpq->mheader,1,sizeof(struct mpq_header),outfp);
	fclose(outfp);
	printf("done\n");
}
void mpq_search(char *spath,char *outpath) {
	for (int i=0;i<nmpq;i++) {
		struct mpq *pmpq=&mpqs[i];
		int hid=get_hash_id(pmpq,spath);if (hid<0) continue;
		struct mpq_hash *ph=&pmpq->mhash[hid];if (ph->block_id<0) continue;
		struct mpq_block *pb=&pmpq->mblock[ph->block_id];
		printf("%08X %d %s/%s\n",pb->flags,pb->unpacked_size,pmpq->path,spath);
		if (outpath) {extract_block_to_file(pmpq,NULL,ph->block_id,outpath);return;}
	}
}
void mpq_extract_path(char *spath,char *outfmt) {
	for (int i=0;i<nmpq;i++) {
		struct mpq *pmpq=&mpqs[i];
		int hid=get_hash_id(pmpq,spath);if (hid<0) continue;
		struct mpq_hash *ph=&pmpq->mhash[hid];if (ph->block_id<0) continue;
		char outpath[256];
		char *name=strrchr(spath,'\\');if (!name) name=strrchr(spath,'/');assert(name);name++;
		sprintf(outpath,outfmt,name);
		struct mpq_block *pb=&pmpq->mblock[ph->block_id];
		printf("%08X %d/%d %s/%s %s\n",pb->flags,pb->packed_size,pb->unpacked_size,pmpq->path,spath,outpath);
		extract_block_to_file(pmpq,spath,ph->block_id,outpath);
		break;
	}
}
void mpq_extract_list(char *listpath,char *outfmt) {
	int ret=0;
	FILE *fplist=fopen(listpath,"r");assert(fplist);
	while (1) {
		char buf[256];char *line=fgets(buf,256,fplist);if (!line) break;
		int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
		mpq_extract_path(line,outfmt);
	}
	fclose(fplist);
}
void mpq_check_path(char *spath) {
	for (int i=0;i<nmpq;i++) {
		struct mpq *pmpq=&mpqs[i];
		int hid=get_hash_id(pmpq,spath);if (hid<0) continue;
		struct mpq_hash *ph=&pmpq->mhash[hid];if (ph->block_id<0) continue;
		struct mpq_block *pb=&pmpq->mblock[ph->block_id];
		if (pb->unpacked_size>32*1024)
		printf("%d %s/%s\n",pb->unpacked_size,pmpq->path,spath);
		break;
	}
}
void mpq_check_list(char *listpath) {
	int ret=0;
	FILE *fplist=fopen(listpath,"r");assert(fplist);
	while (1) {
		char buf[256];char *line=fgets(buf,256,fplist);if (!line) break;
		int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[0]=='#') {line=strchr(line,'\t');assert(line);line++;}
		len=strlen(line);
		if (memcmp(line+len-3,"wav",3)==0) continue;
		mpq_check_path(line);
	}
	fclose(fplist);
}
struct mpqhash {int a,b;};
static struct mpqhash *mpqhashs=NULL;
int generate_mpq_table(char *mpq_table_path) {
	if (!mpqhashs) {
		mpqhashs=(struct mpqhash *)calloc(0x10000,sizeof(struct mpqhash));
		FILE *fp=fopen(mpq_table_path,"rb");
		if (fp) {fread(mpqhashs,1,sizeof(struct mpqhash)*0x10000,fp);fclose(fp);}
	}
	int mpq_table_modified=0,added=0,collision=0;
	FILE *names_fp=fopen("d:/game/mpq/all_names.txt","w+");
	for (int mpqId=0;mpqId<nmpq;mpqId++) {
		struct mpq *pmpq=&mpqs[mpqId];
		for (int i=0;i<pmpq->mheader.nblock;i++) {
			struct mpq_block_info *p=&pmpq->mblock_info[i];if (!p) continue;
			char *path=p->name;
			if (path) {
				if (names_fp) fprintf(names_fp,"%s\n",path);
				int pos=mpq_hashstring(path,0)&0xFFFF;
				int a=mpq_hashstring(path,0x100),b=mpq_hashstring(path,0x200);
				for (int i=0;i<0x10000;i++) {
					struct mpqhash *p=&mpqhashs[pos];
					if (p->a==a&&p->b==b) break;
					if (p->a==0&&p->b==0) {p->a=a;p->b=b;added++;mpq_table_modified=1;break;}
					pos++;if (pos>=0x10000) pos=0;collision++;
				}
			}
		}
	}
	if (names_fp) fclose(names_fp);
	printf("add=%d\n",added);
	printf("collision=%d\n",collision);
	if (mpq_table_modified) {
		FILE *fp=fopen(mpq_table_path,"wb+");
		if (fp) {fwrite(mpqhashs,1,sizeof(struct mpqhash)*0x10000,fp);fclose(fp);}
	}
	return 0;
}
void uncompress() {
	char path[256],outpath[256];
	char *name="patch_d2";
	name="d2xtalk";
	logfp=fopen("/t/uncompress_mpq.log","w+");assert(logfp);
	sprintf(path,"D:/game/mpq/2025/%s.mpq",name);
	sprintf(outpath,"D:/game/mpq/2025/t/%s.mpq",name);
	mpq_uncompress(path,NULL,outpath);
	fclose(logfp);
}
void extract_mpq(char *mpq_path,char *listprefix,char *out_path) {
	globalListPath="d:/d2hackmap/map113c/runtime/mpqlist.txt";
	struct mpq *pmpq=mpq_open(mpq_path,listprefix);
}
void mpq_compare_header() {
	char *names[]={
		"d2exp","d2char","d2sfx","d2data",
		"d2music","d2xmusic","d2speech","d2xtalk",
		"d2video","d2xvideo",
		0,
	};
	struct mpq *m1,*m2;
	for (int i=0;names[i];i++) {
		char path1[256],path2[256];
		sprintf(path1,"d:/game/diablo2/%s.mpq",names[i]);
		sprintf(path2,"d:/game/mpq/%s.mpq",names[i]);
		printf("comparing %s %s\n",path1,path2);
		m1=mpq_open(path1,NULL);
		m2=mpq_open(path2,NULL);
		if(m1->mheader.block_size_shift512!=m2->mheader.block_size_shift512) {
			printf("DIFF block_size_shift512\n");continue;
		}
		if (m1->mheader.nhash!=m2->mheader.nhash) {
			printf("DIFF nhash\n");break;
		}
		if(m1->mheader.nblock!=m2->mheader.nblock){
			printf("DIFF nblock\n");continue;
		}
		int nh=0,nb=0,size1=0,size2=0;
		for (int i=0;i<m1->mheader.nhash;i++) {
			struct mpq_hash *p1=&m1->mhash[i];
			struct mpq_hash *p2=&m2->mhash[i];
			if (memcmp(p1,p2,sizeof(struct mpq_hash))!=0) nh++;
		}
		for (int i=0;i<m1->mheader.nblock;i++) {
			struct mpq_block *p1=&m1->mblock[i];
			struct mpq_block *p2=&m2->mblock[i];
			size1+=p1->packed_size;
			size2+=p2->packed_size;
			if (p1->unpacked_size!=p2->unpacked_size) nb++;
		}
		if (nh) printf("DIFF hash diff %d\n",nh);
		if (nb) printf("DIFF block size diff %d\n",nb);
		printf("%d %d\n",size1,size2);
	}
}
int main(int argc, char *argv[]) {
	logfp=stdout;
	init_crypt_table();
	char *dllpath="d:\\game\\diablo2\\Storm.dll";
	//dllpath="D:/game/starcraft1.08_downcc.com/starcraft/storm.dll";
	HINSTANCE hStorm = LoadLibrary(dllpath);assert(hStorm);
	stormSCompCompress = (funcSCompCompress)GetProcAddress(hStorm,(LPCSTR)551);
	stormSCompDecompress = (funcSCompDecompress)GetProcAddress(hStorm,(LPCSTR)552);
	uncompress();return 0;
	mpq_compare_header();return 0;
	//extract_mpq("/code/git/d2/patch_d2.mpq","/code/git/d2/patch_d2","/code/git/d2/patch_d2");return;
	//search order: patch_d2 d2Xvideo d2Xtalk d2Xmusic d2exp d2music d2char d2speech d2sfx d2data
	char *names[]={
		"patch_d2","d2exp","d2char","d2sfx","d2data",
		"d2music","d2xmusic","d2speech","d2xtalk",
		"d2video","d2xvideo",
		0,
	};
	globalListPath="d:/d2hackmap/map113c/runtime/mpqlist.txt";
	globalListPath="d:/game/mpq/all_names.txt";
	globalListPath="d:/t/all_names.txt";
	for (int i=0;names[i];i++) {
		char path[256],listprefix[256];
		sprintf(path,"D:/game/mpq/%s.mpq",names[i]);
		sprintf(listprefix,"D:/game/mpq/txt/%s",names[i]);
		sprintf(path,"D:/game/starcraft1.08_downcc.com/starcraft/%s.mpq",names[i]);
		sprintf(listprefix,"D:/game/starcraft1.08_downcc.com/starcraft/txt/%s",names[i]);
		struct mpq *pmpq=mpq_open(path,listprefix);
	}
	//generate_mpq_table("d:/d2hackmap/map113c/runtime/mpqtable.bin");
	//mpq_check_list("d:/d2hackmap/map113c/runtime/mpqlist.txt");
	//mpq_extract_list("t/tmp.txt","t/txt/%s");
	char *path="data/global/excel/events.bin";
	//path="data/local/font/latin/default.map";
	path="DATA/GLOBAL/EXCEL/treasureclassex.bin";
	//mpq_search(path,NULL);
	if (1) {
		char *spath="/data/global/ui/FrontEnd/D2logoFireLeft.DC6";
		char *outpath="t/tmp_mpq.txt";
		spath="data/global/ui/FrontEnd/barbarian/banu1.DC6";
		spath="data/global/palette/fechar/pal.dat";
		spath="/data/global/ui/SPELLS/SoSkillicon.DC6";
		spath="/data/global/palette/static/pal.dat";
		spath="/data/global/palette/act1/pal.dat";
		spath="/data/global/animdata.d2";
		spath="/data/global/items/invcap.DC6";
		//spath="/data/global/objects/LargeChestR.DC6";
		spath="data/global/sfx/cursor/repair.wav";
		//spath="/data/global/sfx/monsters/bighead/death.dc6";
		spath="data/global/chars/am/cof/amblxbow.cof"; //am so ne pa ba
		spath="(attributes)";
		spath="data/global/excel/weapons.bin";
		spath="data/global/chars/AM/HD/AMHDbhmA11hs.dcc";
		spath="DATA/GLOBAL/UI/AutoMap/MaxiMap.dc6";
		spath="DATA/GLOBAL/UI/panel/TradeStash.dc6";
		spath="DATA/GLOBAL/UI/panel/supertransmogrifier.dc6";
		spath="DATA/GLOBAL/UI/panel/InvChar6.dc6";
		spath="campaign/expzerg/zerg05/staredit/scenario.chk";
		spath="campaign/expzerg/zerg05/staredit/wav/TCvWht02.wav";
		spath="Sound/Glue/bnetclick.wav";
		mpq_extract_path(spath,"t/%s");
		//mpq_search(spath,NULL);
	}
	return 0;
}


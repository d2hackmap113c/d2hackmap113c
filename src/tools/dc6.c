#include <Windows.h>
#include <stdio.h>
#include <assert.h>

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
int d2pal[256]={
	0x000000,0x240000,0x1C1808,0x2C2410,0x3C3418,0x5C0000,0x484020,0x544828,
	0x900000,0x8C4810,0xBC0000,0xD08420,0xF4C46C,0x8C7C50,0xAC9C64,0x0C0C08,
	0x141010,0x1C1C1C,0x28241C,0x2C2C2C,0x3C3830,0x383838,0x484848,0x585048,
	0x645834,0x585858,0x74643C,0x646464,0x7C7470,0x848484,0x9C9894,0xC4C4C4,
	0xF4F4F4,0x080404,0x100400,0x180804,0x181008,0x1C1410,0x240C04,0x20180C,
	0x201814,0x2C1008,0x241C10,0x28200C,0x380804,0x301C10,0x302814,0x40140C,
	0x382810,0x480C04,0x38281C,0x4C200C,0x442C14,0x402C20,0x581008,0x483420,
	0x443428,0x54281C,0x5C1C14,0x5C2408,0x543818,0x543824,0x6C1810,0x682C10,
	0x5C4424,0x702418,0x683C24,0x7C2C0C,0x644C2C,0x704808,0x802818,0x745028,
	0x883024,0x6C5040,0x8C340C,0x702068,0x785844,0x8C4040,0x845C30,0x9C3424,
	0xA42818,0xA44814,0x8C6440,0x8C7038,0x8C6850,0xB04434,0xB44C20,0x987058,
	0xA07840,0xBC601C,0x9C8448,0xC45454,0xC86C20,0xAC7C58,0xB08848,0xC87C54,
	0xE07020,0xB89C54,0xFC2C00,0xC08C70,0xCC9850,0xE48430,0xE07070,0xD09878,
	0xF8883C,0xECA038,0xD8B864,0xE0A484,0xF0B444,0xF4C04C,0xF0B08C,0xFCD45C,
	0xFCB0B0,0x042410,0x142418,0x203C18,0x18480C,0x004430,0x186408,0x245C24,
	0x385C24,0x086C44,0x287C14,0x407434,0x587830,0x349C1C,0x708438,0x48A034,
	0x58904C,0x44BC28,0x849848,0x60B84C,0x18FC00,0x74DC5C,0x8CD07C,0xA0FC88,
	0x0C0C28,0x181848,0x000058,0x381444,0x102468,0x283C64,0x282878,0x4C1080,
	0x384C84,0x303094,0x48608C,0x385CA0,0x5050AC,0x4C6CAC,0x5478BC,0x2460D8,
	0x6478D0,0x6490E0,0x80A0DC,0xA420FC,0x8484F0,0xA0A0FC,0x90B8FC,0x588C90,
	0x68A0A4,0x84C0C4,0x98D0D4,0xA8CCFC,0xCCF4F4,0xC0A080,0xC4C0A8,0xE0C494,
	0xFCE874,0xC4FCB0,0xFCE4A4,0xFCFCC4,0x040404,0x080808,0x0C0C0C,0x101010,
	0x141414,0x181818,0x241C18,0x202020,0x242424,0x282828,0x302820,0x303030,
	0x383028,0x343434,0x34383C,0x443834,0x3C3C3C,0x4C3C30,0x404040,0x44403C,
	0x444444,0x50483C,0x584438,0x4C4C4C,0x604C3C,0x34585C,0x505050,0x545454,
	0x5C5C5C,0x685C50,0x606060,0x447074,0x7C6450,0x686868,0x6C6C6C,0x786C60,
	0x707070,0x747474,0x7C7C7C,0x948064,0x908474,0xAC8870,0x909090,0x9C9484,
	0x8094B8,0xA0A0A0,0xB0AC98,0xACACAC,0xB8B8B8,0xCCCCCC,0xD8D8D8,0xCCCCFC,
	0xE4E4E4,0xFC0080,0x101408,0x141C08,0x142008,0x18240C,0x202C0C,0x203418,
	0x30381C,0x002044,0x180068,0x080804,0x141410,0x181814,0x1C1C14,0x1C2010,
	0x242018,0x242814,0x282820,0x2C2824,0x202C30,0x302C24,0x343028,0x38342C,
	0x303C40,0x403C34,0x484438,0x504C44,0x5C584C,0x605C54,0x686458,0xFFFFFF,
};


char *bitmap2dc6(BYTE *pixels, int width, int height,int *psize) {
	BYTE *outbuf = malloc((width*height*2)+height);
	char *dest = outbuf;
	int rowlen=width&3?(width+3)&0xFFFFFFFC:width;
	BYTE *row_src = pixels;
	for (int i = 0; i < height; i++,row_src+=rowlen) {
		BYTE *src=row_src,*row_end = row_src+width;
		while (src < row_end) {
			BYTE *scan_limit = src+0x7f;
			BYTE *start=src;
			if (scan_limit>row_end) scan_limit=row_end;
			if (*src) {do src++;while (*src&&src<scan_limit);}
			else {do src++;while (!*src&&src<scan_limit);}
			if (*start) {*dest++=src-start;memcpy(dest,start,src-start);dest+=src-start;}
			else if (src<row_end) *dest++=0x80|(src-start);
		}
		*dest++=0x80;
		//printf("%d %d\n",i,dest-outbuf);
	}
	static DWORD dc6head[] = {6, 1, 0, 0xeeeeeeee, 1, 1, 0x1c,  0, -1, -1, 0, 0, 0, -1, -1};
	dc6head[8] = width;
	dc6head[9] = height;
	int datasize=dc6head[14] = dest-outbuf;
	int filesize=dc6head[13] = sizeof(dc6head)+datasize+3;
	*psize=filesize;
	BYTE *ret = malloc(filesize);
	dest=ret;memcpy(dest, dc6head, sizeof(dc6head));dest+=sizeof(dc6head);
	memcpy(dest,outbuf,dc6head[14]);dest+=dc6head[14];
	memset(dest,0xee, 3);
	free(outbuf);
	return ret;
}
/*
  typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
  } BITMAPFILEHEADER,*LPBITMAPFILEHEADER,*PBITMAPFILEHEADER;
  typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
  } BITMAPINFOHEADER,*LPBITMAPINFOHEADER,*PBITMAPINFOHEADER;
*/
struct DC6Header {
	int	version;		//+00
	int flags; //+04
	int	eFormat;		//+08 Encoding
	int	term;	//+0C termination
	int	nDir;		//+10 numDirections
	int	nCell;		//+14 framesPerDirection
};
struct DC6Cell {
	int	flags;	//+00
	int	w,h,x,y;	//+04
	int	off14; //+14
	int	nextOff;	//+18 NextBlock
	int	len;	//+1c
};
FILE *mkbmp(char *outpath,int w,int h) {
	BITMAPFILEHEADER bf={0};
	BITMAPINFOHEADER bi={0};
	bf.bfType=0x4d42;
	bf.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+1024;
	bf.bfSize=bf.bfOffBits+w*h;
	bi.biSize=sizeof(BITMAPINFOHEADER);bi.biWidth=w;bi.biHeight=h;bi.biPlanes=1;bi.biBitCount=8;
	bi.biSizeImage=w*h;bi.biXPelsPerMeter=bi.biYPelsPerMeter=0;
	FILE *fpout=fopen(outpath,"wb+");assert(fpout);
	fwrite(&bf,1,sizeof(BITMAPFILEHEADER),fpout);
	fwrite(&bi,1,sizeof(BITMAPINFOHEADER),fpout);
	fwrite(d2pal,1,1024,fpout);
	return fpout;
}
char *cell2pixel(struct DC6Cell *pcell,char *frame,int *pw,int *ph) {
	int x0=pcell->x,y0=pcell->y;
	int w=pcell->w,h=pcell->h;
	printf("(%d,%d) %d*%d %d\n",x0,y0,w,h,pcell->len);
	if (w&3) w=(w&~3)+4;
	*pw=w;*ph=h;
	if (w&3) w=(w&~3)+4;
	char *buf=malloc(w*h);
	unsigned char *src=frame;
	char *dst=buf,*dst_end=buf+w;
	for (int i=0;i<h;i++) {
		char *rowstart=dst;
		memset(dst,0,w);
		while (1) {
			int n=*src++;
			if (n==0x80) break;
			if (dst>=dst_end) {printf("overflow %d/%d %x\n",dst-rowstart,w,*src);assert(0);}
			if (n&0x80) dst+=n&0x7f;
			else {memcpy(dst,src,n);src+=n;dst+=n;}
		}
		dst=rowstart+w;dst_end=dst+w;
	}
	return buf;
}
char *pixel2cell(char *pixels,int x,int y, int w, int h,int rowlen,int *psize) {
	char *outbuf = malloc((w*h*2)+h),*dest=outbuf;
	BYTE *row_src = pixels+rowlen*y+x;
	for (int i = 0; i < h; i++,row_src+=rowlen) {
		BYTE *src=row_src,*row_end = row_src+w;
		while (src < row_end) {
			BYTE *scan_limit = src+0x7f;
			BYTE *start=src;
			if (scan_limit>row_end) scan_limit=row_end;
			if (*src) {do src++;while (*src&&src<scan_limit);}
			else {do src++;while (!*src&&src<scan_limit);}
			if (*start) {*dest++=src-start;memcpy(dest,start,src-start);dest+=src-start;}
			else if (src<row_end) *dest++=0x80|(src-start);
		}
		*dest++=0x80;
	}
	*psize=dest-outbuf;
	return outbuf;
}
void dc6cell2bmp(int offset,struct DC6Cell *pcell,char *frame,char *outpath) {
	int x0=pcell->x,y0=pcell->y;
	int w=pcell->w,h=pcell->h;
	printf("%d (%d,%d) %d*%d %s %d %x\n",offset,x0,y0,w,h,outpath,pcell->len,*(int *)(frame+pcell->len));
	char *bitmap=cell2pixel(pcell,frame,&w,&h);
	FILE *fpout=mkbmp(outpath,w,h);
	fwrite(bitmap,1,w*h,fpout);
	fclose(fpout);
	free(bitmap);
}
void dump_dc6header(struct DC6Header *h) {
	printf("v=%d f=0x%X e=0x%x term=0x%X %d*%d\n",h->version,h->flags,h->eFormat,h->term,h->nDir,h->nCell);
}
void dump_dc6cell(int off,struct DC6Cell *c) {
	printf("off=%d f=%d (%d*%d) (%d,%d) %d nextOff=%d len=%d\n",
		off,c->flags,c->w,c->h,c->x,c->y,c->off14,c->nextOff,c->len);
}
void dc6big(char *inpath, char *outfmt,char *outpath) {
	int size;char *dc6bin=load(inpath,&size);
	printf("path=%s size=0x%X\n",inpath,size);
	struct DC6Header *dc6=(struct DC6Header *)dc6bin;
	dump_dc6header(dc6);
	int *offsets=(int *)(dc6bin+0x18);
	assert(dc6->nDir==1);
	assert(dc6->nCell==4);
	struct DC6Cell *cells[4];
	char *frames[4],*bm[4];
	int w[4],h[4];
	for (int i=0;i<4;i++) {
		cells[i]=(struct DC6Cell *)(dc6bin+offsets[i]);
		dump_dc6cell(offsets[i],cells[i]);
		frames[i]=dc6bin+offsets[i]+0x20;
		bm[i]=cell2pixel(cells[i],frames[i],&w[i],&h[i]);
		char outpath2[256];sprintf(outpath2,outfmt,0,i);
		printf("write %s\n",outpath2);
		FILE *fpout=mkbmp(outpath2,w[i],h[i]);
		fwrite(bm[i],1,w[i]*h[i],fpout);
		fclose(fpout);
	}
	assert(w[0]==w[2]);assert(w[1]==w[3]);assert(h[0]==h[1]);assert(h[2]==h[3]);
	int mw=w[0]+w[1],mh=h[0]+h[2];
	char *merge=malloc(mw*mh);
	char *src1,*src2;
	char *dst=merge;
	src1=bm[2];src2=bm[3];
	for (int i=0;i<h[2];i++) {
		memcpy(dst,src1,w[2]);dst+=w[2];src1+=w[2];
		memcpy(dst,src2,w[3]);dst+=w[3];src2+=w[3];
	}
	src1=bm[0];src2=bm[1];
	for (int i=0;i<h[0];i++) {
		memcpy(dst,src1,w[0]);dst+=w[0];src1+=w[0];
		memcpy(dst,src2,w[1]);dst+=w[1];src2+=w[1];
	}
	printf("write %d,%d to %s\n",mw,mh,outpath);
	FILE *fpout=mkbmp(outpath,mw,mh);
	fwrite(merge,mw*mh,1,fpout);
	fclose(fpout);
}
void dc6bmp(char *inpath,char *outfmt) {
	int size;char *dc6bin=load(inpath,&size);
	struct DC6Header *dc6=(struct DC6Header *)dc6bin;
	printf("%d*%d %X\n",dc6->nDir,dc6->nCell,dc6->term);
	int *offsets=(int *)(dc6bin+0x18);
	for (int i=0;i<dc6->nDir;i++) {
		for (int j=0;j<dc6->nCell;j++) {
			char outpath[256];sprintf(outpath,outfmt,i,j);
			int offset=*offsets++;
			dc6cell2bmp(offset,(struct DC6Cell *)(dc6bin+offset),dc6bin+offset+0x20,outpath);
		}
	}
}
char *bmp2cell(char *filename,int *pw,int *ph,int *psize) {
	int size;char *bmpData=load(filename,&size);
	BITMAPFILEHEADER *fhead = (BITMAPFILEHEADER *)bmpData;
	BITMAPINFOHEADER *bmphead = (BITMAPINFOHEADER *)(bmpData+sizeof(BITMAPFILEHEADER));
	assert(bmpData&&(fhead->bfType==0x4d42)&&(bmphead->biBitCount==8)&&(bmphead->biCompression==BI_RGB));
	*pw=bmphead->biWidth;*ph=bmphead->biHeight;
	int w=bmphead->biWidth,h=bmphead->biHeight;
	int rowlen=w&3?(w+3)&0xFFFFFFFC:w;
	return pixel2cell((BYTE *)bmpData+fhead->bfOffBits,0,0,w,h,rowlen,psize);
}
void bigbmp2cell(char *inpath,char *outpath) {
	int size;char *bmpData=load(inpath,&size);
	BITMAPFILEHEADER *fhead = (BITMAPFILEHEADER *)bmpData;
	BITMAPINFOHEADER *bmphead = (BITMAPINFOHEADER *)(bmpData+sizeof(BITMAPFILEHEADER));
	assert(bmpData&&(fhead->bfType==0x4d42)&&(bmphead->biBitCount==8)&&(bmphead->biCompression==BI_RGB));
	char *pixel=(char *)bmpData+fhead->bfOffBits;
	int w=bmphead->biWidth,h=bmphead->biHeight;
	int rowlen=w&3?(w+3)&0xFFFFFFFC:w;
	assert(w>256&&h>256);
	int term=0xEEEEEEEE;
	struct DC6Header header={6,1,0,0xEEEEEEEE,1,4};
	int offsets[4];
	int sizes[4];
	int totalsize=0x18+4*4;
	struct DC6Cell cells[4];
	char *celldata[4];
	for (int i=0;i<4;i++) {
		struct DC6Cell *pc=&cells[i];
		int x=i==0||i==2?0:256;
		int y=i==2||i==3?0:h-256;
		int w1=i==0||i==2?256:w-256;
		int h1=i==0||i==1?256:h-256;
		celldata[i]=pixel2cell(pixel,x,y,w1,h1,rowlen,&sizes[i]);
		pc->flags=0;pc->w=w1;pc->h=h1;pc->x=0;pc->y=0;pc->off14=0;pc->len=sizes[i];
		offsets[i]=totalsize;
		totalsize+=32+sizes[i]+3;
	}
	for (int i=0;i<3;i++) cells[i].nextOff=offsets[i+1];
	char *outbuf=malloc(totalsize);
	char *dst=outbuf;
	memcpy(dst,&header,24);dst+=24;
	memcpy(dst,offsets,4*4);dst+=4*4;
	for (int i=0;i<4;i++) {
		memcpy(dst,&cells[i],32);dst+=32;
		memcpy(dst,celldata[i],sizes[i]);dst+=sizes[i];
		memcpy(dst,&term,3);dst+=3;
	}
	FILE *fpout=fopen(outpath,"wb+");assert(fpout);
	fwrite(outbuf,totalsize,1,fpout);
	fclose(fpout);
}
void load_pal(char *inpath) {
	int size;char *src=load(inpath,&size);
	for (int i=0;i<256;i++) {
		d2pal[i]=(*(int *)src)&0xFFFFFF;
		src+=3;
	}
}
void dc6info(char *inpath) {
	int size;char *dc6bin=load(inpath,&size);
	struct DC6Header *dc6=(struct DC6Header *)dc6bin;
	printf("%d*%d %X\n",dc6->nDir,dc6->nCell,dc6->term);
	int *offsets=(int *)(dc6bin+0x18);
	for (int i=0;i<dc6->nDir;i++) {
		for (int j=0;j<dc6->nCell;j++) {
			int offset=*offsets++;
			struct DC6Cell *pcell=(struct DC6Cell *)(dc6bin+offset);
			char *frame=dc6bin+offset+0x20;
			printf("%d-%d: %d %d\n",i,j,offset,offset+32);
		}
	}
}
void test2() {
	int size;
	char *path="D:\\d2hackmap\\map113c\\bmp\\blobBoss.bmp";
	path="t/t2.bmp";
	char *bmp=load(path,&size);
	BITMAPFILEHEADER *fh = (BITMAPFILEHEADER *)bmp;
	BITMAPINFOHEADER *ih = (BITMAPINFOHEADER *)(bmp+sizeof(BITMAPFILEHEADER));
	printf("%d\n",size);
	hex(0,fh,sizeof(BITMAPFILEHEADER));
	hex(0,ih,sizeof(BITMAPINFOHEADER));
	int *pal=(int *)(bmp+fh->bfOffBits-1024);
	//for (int i=0;i<256;i++) {printf("0x%06X,",pal[i]);if (i%8==7) printf("\n");}
	assert(bmp && (fh->bfType == 'MB') && (ih->biBitCount == 8) && (ih->biCompression == BI_RGB));
	int size1,size2;
	char *d2=bitmap2dc6(bmp+fh->bfOffBits, ih->biWidth, ih->biHeight,&size2);
}
void updateCell(char *cells[],int id,char *path) {
	int w,h,cellsize;
	char *celldata=bmp2cell(path,&w,&h,&cellsize);
	char *cellall=malloc(cellsize+32);
	memcpy(cellall,cells[id],32);
	memcpy(cellall+32,celldata,cellsize);
	struct DC6Cell *cell=cellall;cell->w=w;cell->h=h;cell->len=cellsize;
	cells[id]=cellall;
}
void updateCells() {
	char *cells[32];
	char *inpath="t/InvChar6.dc6";
	char *outpath="t/InvChar6half.dc6";
	int size;char *dc6bin=load(inpath,&size);
	struct DC6Header *dc6=(struct DC6Header *)dc6bin;
	printf("%d*%d %X\n",dc6->nDir,dc6->nCell,dc6->term);
	assert(dc6->nCell<32);
	int *offsets=(int *)(dc6bin+0x18);
	for (int i=0;i<dc6->nCell;i++) {
		//printf("off%d=%d\n",i,offsets[i]);
		cells[i]=dc6bin+offsets[i];
	}
	//updateCell(cells,6,"t/invchar6-6.bmp");
	//updateCell(cells,7,"t/invchar6-7.bmp");
	updateCell(cells,6,"t/invchar_half6.bmp");
	updateCell(cells,7,"t/invchar_half7.bmp");
	int needSize=0x18+dc6->nCell*4;
	for (int i=0;i<dc6->nCell;i++) {
		struct DC6Cell *cell=cells[i];needSize+=cell->len+32+3;
	}
	printf("size: %d\n",needSize);
	char *out=malloc(needSize+32);
	memcpy(out,dc6bin,0x18);
	offsets=(int *)(out+0x18);
	char *dst=out+0x18+dc6->nCell*4;
	for (int i=0;i<dc6->nCell;i++) {
		struct DC6Cell *cell=cells[i];
		offsets[i]=dst-out;
		//printf("off%d=%d\n",i,offsets[i]);
		memcpy(dst,cells[i],cell->len+32);dst+=cell->len+32;*(int *)dst=dc6->term;dst+=3;
	}
	save(outpath,out,dst-out);
}
int main(int argc, char *argv[]) {
	//updateCells();
	char *pal_path=NULL;
	char *outfmt="t/dc6_%d_%d.bmp";
	char *outpath="t/cube.bmp";
	char *path;
	path="t/banu1.dc6";outfmt="t/banu1_%d_%d.bmp";pal_path="t/pal_fechar.dat";
	path="t/SoSkillicon.DC6";outfmt="t/SoSkillicon_%d_%d.bmp";
	pal_path="t/pal_static.dat";
	pal_path="t/pal_act1.dat";
	path="t/invcap.DC6";outfmt="t/invcap_%d_%d.bmp";
	path="t/flpcap.DC6";outfmt="t/flpcap_%d_%d.bmp";
	path="t/invcapu.DC6";outfmt="t/invcapu_%d_%d.bmp";
	path="t/TradeStash.dc6";outfmt="t/tradestash_%d_%d.bmp";
	path="t/bank.dc6";outfmt="t/bank_%d_%d.bmp";
	//path="t/supertransmogrifier.dc6";outfmt="t/supertransmogrifier_%d_%d.bmp";
	path="t/InvChar6.dc6";outfmt="t/invchar6_%d_%d.bmp";
	path="t/InvChar6b.dc6";outfmt="t/invchar6b_%d_%d.bmp";
	path="t/cube.dc6";outfmt="t/cube_%d_%d.bmp";outpath="t/cube.bmp";
	//dc6info(path);return;
	load_pal(pal_path);
	//dc6bmp(path,outfmt);
	bigbmp2cell("t/cube10x10my.bmp","t/cube10x10.dc6");
	path="t/cube10x10.dc6";outfmt="t/cube10x10_%d_%d.bmp";outpath="t/cube10x10gen.bmp";
	dc6big(path,outfmt,outpath);
	return 0;
}


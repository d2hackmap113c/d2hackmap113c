#include "stdafx.h"
#include "bitstream.h"

void bitstream_init(struct bitstream *bs,void *buf,void *end) {
	bs->bufstart=(unsigned char *)buf;
	bs->bufend=(unsigned char *)end;
	bs->buf=bs->bufstart;
	bs->v=0;
	bs->n=0;
}
void bitstream_seek(struct bitstream *bs,int pos) {
	bs->buf=bs->bufstart+(pos>>3);
	bs->v=0;bs->n=0;
	bitstream_rbit(bs,pos&7);
}
int bitstream_rbit(struct bitstream *bs,int n) {
	if (n<=0) return 0;
	if (n>16) {
		int low=bitstream_rbit(bs,16);
		int high=bitstream_rbit(bs,n-16);
		return (high<<16)|low;
	}
	while (bs->n < n) {
		if (bs->buf>=bs->bufend) return 0;
		bs->v|=((*bs->buf++)&0xFF)<<bs->n;
		bs->n+=8;
	}
	int r=bs->v & ((1<<n)-1);
	bs->v >>= n;
	bs->n -= n;
	return r;
}
int bitstream_wbit(struct bitstream *bs,unsigned int value,int n) {
	if (n<=0) return 0;
	if (n>16) {
		bitstream_wbit(bs,value&0xFFFF,16);
		return bitstream_wbit(bs,value>>16,n-16);
	}
	while (bs->n>8) {
		if (bs->buf>=bs->bufend) return -1;
		*bs->buf++=bs->v&0xFF;
		bs->v>>=8;bs->n-=8;
	}
	bs->v |= value<<bs->n;
	bs->n+=n;
	return 0;
}
void bitstream_wbit_done(struct bitstream *bs) {
	if (bs->n<=0) return;
	while (bs->n > 0) {
		if (bs->buf>=bs->bufend) return;
		*bs->buf++=bs->v&0xFF;
		bs->v>>=8;
		if (bs->n>=8) bs->n-=8;else bs->n=0;
	}
}

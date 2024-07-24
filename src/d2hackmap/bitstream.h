
struct bitstream {
	unsigned char *bufstart,*buf,*bufend;
	unsigned int v;
	int n;
};
void bitstream_init(struct bitstream *bs,void *buf,void *end);
void bitstream_seek(struct bitstream *bs,int pos);
int bitstream_rbit(struct bitstream *bs,int n);
int bitstream_wbit(struct bitstream *bs,unsigned int value,int n);
void bitstream_wbit_done(struct bitstream *bs);

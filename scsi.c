#include<stdio.h>
#include<malloc.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<scsi/sg.h>//SCSI的结构体和宏定义都在这个头文件里面
unsigned char sense_buffer[254];
unsigned char data_buffer[254];

struct sg_io_hdr* init_hdr(int page_code, int evpd)
{
	unsigned char cdb[6];
	struct sg_io_hdr * p_scsi_hdr = (struct sg_io_hdr *)malloc(sizeof(struct sg_io_hdr));//给scsi结构体分配一个缓存，因为写入磁盘的												//命令和读取磁盘结果的数据都在这个结构体里面
	memset(p_scsi_hdr, 0, sizeof(struct sg_io_hdr));//给结构体初始化为0
	if (p_scsi_hdr) 
	{
		p_scsi_hdr->interface_id = 'S';
		p_scsi_hdr->flags = SG_FLAG_LUN_INHIBIT;
	}
	p_scsi_hdr->dxferp = data_buffer;
	p_scsi_hdr->dxfer_len = sizeof(data_buffer);
	p_scsi_hdr->sbp = sense_buffer;
	p_scsi_hdr->mx_sb_len = sizeof(sense_buffer);
	cdb[0] = 0x12; /*This is for Inquery*/
	cdb[1] = evpd & 1;
	cdb[2] = page_code & 0xff;
	cdb[3] = 0;
	cdb[4] = 0xff;
	cdb[5] = 0; /*For control filed, just use 0 */
	p_scsi_hdr->dxfer_direction = SG_DXFER_FROM_DEV;
	p_scsi_hdr->cmdp = cdb;//将数组的cdb命令写入结构体
	p_scsi_hdr->cmd_len = 6;//写入cdb命令数组长度6字节
	int fd = open("/dev/sg1", O_RDWR);
	if(fd<0)
		printf("open sg0 error..\n");

	int ret = ioctl(fd, SG_IO, p_scsi_hdr);
	if (ret<0) {
		printf("Sending SCSI Command failed.\n");
		close(fd);
	}
	return p_scsi_hdr;
}

void show_vendor(struct sg_io_hdr* hdr1) {
	unsigned char* buffer = hdr1->dxferp;
	int i;
	printf("vendor id:");
	for (i=8; i<16; ++i) {
		putchar(buffer[i]);
	}
	putchar('\n');

}

void show_product(struct sg_io_hdr* hdr) {
	unsigned char *buffer = hdr->dxferp;
	int i;
	printf("product id:");
	for (i = 16; i<32; ++i) {
	putchar(buffer[i]);
	}
	putchar('\n');
} 

void show_product_rev(struct sg_io_hdr * hdr) {
	unsigned char * buffer = hdr->dxferp;
	int i;
	printf("product ver:");
	for (i = 32; i<36; ++i) {
	putchar(buffer[i]);
	}
	putchar('\n');
} 

int main()
{
	struct sg_io_hdr *p = init_hdr(0,0);
	show_vendor(p);
	show_product(p);
	show_product_rev(p);
	free(p);
	return 0;
}



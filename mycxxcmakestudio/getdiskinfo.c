int getdiskinfo()
{
FILE *fp;
char disk_name[10];
char str_find[] = "/boot";
char buf_line[128];
static struct hd_driveid hd;
int fd;
int tmp_len = 0;

fp = fopen ("/etc/mtab", "rb");
if (fp == NULL)
{
fprintf(stderr, "No /etc/mtab file.\n");
return 0;
}

while(fgets(buf_line, sizeof(buf_line), fp)) {
    if(strstr(buf_line, str_find) != NULL){
        printf("debug: %s\n", buf_line);
        break;
    }
}   
// TODO "/boot" extract
if(sscanf(buf_line, "%s /boot", disk_name) == -1)
{
    printf("Error: can not get disk name\n");   
}

// delete number, e.g. /dev/sda1
tmp_len = strlen(disk_name);
disk_name[tmp_len -1 ] = '\0';

if (geteuid() >  0) {
  printf("ERROR: Must be root to use\n");
  exit(1);
}
// TODO
if ((fd = open(disk_name, O_RDONLY|O_NONBLOCK)) < 0) {
  printf("ERROR: Cannot open device %s\n", disk_name);
  exit(1);
}

if (!ioctl(fd, HDIO_GET_IDENTITY, &hd)) {
    const int nSNLength = 20;
    strSN = "";
        
    for(int i=0; i < nSNLength; i++){
        if(hd.serial_no[i] < 32 || hd.serial_no[i] == 127)
            {
            strSN += '.';
            }
        else{
            strSN += (char)hd.serial_no[i];
            }
    }
    printf("debug: Disk Serial Number: %.20s\n", strSN.GetData());
    return 1;
} else if (errno == -ENOMSG) {
  printf("No hard disk identification information available\n");
  return 0;
} else {
  perror("ERROR: HDIO_GET_IDENTITY");
  exit(1);
}
}



int get_system_tf_capacity(double *capacity)
{
 if (capacity == NULL)
  return -1;
 *capacity = 0;
 FILE *procpt;
 char line[100];
 double ma, mi;
 char tmp[4][100];

 procpt = popen("fdisk -l /dev/mmcblk0", "r");

 while (fgets(line, sizeof(line), procpt))
 {
  if (sscanf(line, "%[^ ] %[^ ] %lf %[^ ] %lf %[^\n ]", tmp[0], tmp[1],
    &mi, tmp[2], &ma, tmp[3]) != 6)
   continue;
  *capacity = ma;
  break;
 }
 pclose(procpt);
 return 0;
}

#include <stdio.h>
#include <errno.h>
int GetDiskCount()
{
    FILE *f = fopen( "/proc/partitions", "r");
    if (!f)
    {
        fprintf(stderr, "fopen /proc/partitions failed!errno=%d\n", errno);
        return 0;
    }
    char line[100], ptname[100], devname[120], *s;
    int ma, mi, sz;
    int nDiskCount = 0;
    while(fgets(line, sizeof(line),f))
        {
            if(sscanf(line, " %u %u %u %[^\n ]",&ma, &mi, &sz, ptname) != 4)
                     continue;
for(s = ptname;*s;s++ ) 
    continue;

/* note: excluding '0': e.g. mmcblk0 is not a partition name! */
if(s[-1] >= '1' && s[-1] <= '9') 
    continue;
nDiskCount++;
        }
    fclose(f);
     return nDiskCount;
}

int main(int argc, char *argv[] )
{
    printf( "DiskCount: %d\n", GetDiskCount());
    return 0;
}
#include <iostream>
using namespace std;
// //-------------------------------------------------
// // Without Boost LIB usage
// //-------------------------------------------------
// #include <sys/statvfs.h>
// #include <sys/sysinfo.h>
// //-------------------------------------------------
// void testinfo()
// {
// stringstream   strStream;
// unsigned long  hdd_size;
// unsigned long  hdd_free;
// ostringstream  strConvert;
// //---
// struct sysinfo info;
// sysinfo( &info );   
// //---
// struct statvfs fsinfo;
// statvfs("/", &fsinfo);
// //---
// //---
// unsigned num_cpu = std::thread::hardware_concurrency();
// //---
// ifstream cpu_freq("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");
// strStream << cpu_freq.rdbuf();
// std::string  cpufrequency = strStream.str();
// //---
// strStream.str("");
// ifstream cpu_temp("/sys/class/thermal/thermal_zone0/temp");
// strStream << cpu_temp.rdbuf();
// strConvert<< fixed << setprecision(2) << std::stof(strStream.str());
// std::string cputemp = strConvert.str();
// //---
// std::string   mem_size = to_string( (size_t)info.totalram *     (size_t)info.mem_unit );
// //---
// hdd_size = fsinfo.f_frsize * fsinfo.f_blocks;
// hdd_free = fsinfo.f_bsize * fsinfo.f_bfree;  
// //---                                                
// std::cout << "CPU core number           ==" << num_cpu       << endl;
// std::cout << "CPU core speed            ==" << cpufrequency  << endl;
// std::cout << "CPU temperature (C)       ==" << cputemp       << endl;
// //---
// std::cout << "Memory size               ==" << mem_size      << endl;
// //---
// std::cout << "Disk, filesystem size     ==" << hdd_size      << endl;
// std::cout << "Disk free space           ==" << hdd_free      << endl;
// //---
// }


#include "getMemorySize.c"
// getmacaddress.c
#include <stdio.h>

#if defined(WIN32) || (!defined(__GNUC__) && !defined(__clang__))
    #include <winsock2.h>
    #include <iphlpapi.h>
    #include <stdlib.h>
    #pragma comment(lib, "IPHLPAPI.lib")
#elif defined(__linux__)
    #include <string.h> //strncpy
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <net/if.h>
#else
    //bsd
    #include <sys/types.h> //FreeBSD u_int
    #include <ifaddrs.h>
    #include <net/if.h>
    #include <net/if_dl.h>
    #include <net/if_types.h>
#endif

void getmacaddress(char *mac_address){
#if defined(WIN32) || (!defined(__GNUC__) && !defined(__clang__))
    //MSDNのサンプルコード
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    unsigned char *addr;

    mac_address[0]=0;
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if(!pAdapterInfo)return;
    if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW){
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        if(!pAdapterInfo)return;
    }

    if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR){
        pAdapter = pAdapterInfo;
        if(pAdapter){
            addr = pAdapter->Address;
            sprintf(mac_address,"%02x:%02x:%02x:%02x:%02x:%02x",
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        }
    }
    free(pAdapterInfo);
#elif defined(__linux__)
    //http://field-notes.hatenablog.jp/entry/20101216/1292467817
    mac_address[0]=0;
    struct ifreq *ifr, *ifend;
    struct ifreq ifreq;
    struct ifconf ifc;
    struct ifreq ifs[16];
    int fd;
    unsigned char *addr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_req = ifs;
    if(ioctl(fd, SIOCGIFCONF, &ifc)<0){close(fd);return;}

    ifend = ifs + (ifc.ifc_len / sizeof(struct ifreq));
    for(ifr = ifc.ifc_req;ifr < ifend;ifr++){
        if(ifr->ifr_addr.sa_family == AF_INET){
            strncpy(ifreq.ifr_name, ifr->ifr_name, sizeof(ifreq.ifr_name));
            if(ioctl(fd, SIOCGIFHWADDR, &ifreq)<0)continue;
            addr = ifreq.ifr_hwaddr.sa_data;
            sprintf(mac_address,"%02x:%02x:%02x:%02x:%02x:%02x",
                    addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        }
    }
    close(fd);
#else
    //bsd
    mac_address[0]=0;
    struct ifaddrs *ifa_list, *ifa; 
    struct sockaddr_dl *dl; 
    unsigned char *addr;
    if(getifaddrs(&ifa_list)<0)return;
    for(ifa = ifa_list;ifa;ifa = ifa->ifa_next){ 
        dl = (struct sockaddr_dl*)ifa->ifa_addr; 
        if (dl->sdl_family == AF_LINK && dl->sdl_type == IFT_ETHER) {
            addr = (unsigned char*)LLADDR(dl);
            sprintf(mac_address,"%02x:%02x:%02x:%02x:%02x:%02x",
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
            return;
        }
    } 
    freeifaddrs(ifa_list); 
#endif
}

static double ParseMemValue(const char * b)
{
 while((*b)&&(isdigit(*b) == false)) b++;
 return isdigit(*b)? atof(b) : -1.0;
}
//Returns a number between 0.0f and 1.0f, with 0.0f meaning all RAM is available, and 1.0f meaning all RAM is currently in use
float GetSystemMemoryUsagePercentage()
{
 FILE * fpIn = popen("/usr/bin/vm_stat","r");
 if (fpIn)
 {
 double pagesUsed = 0.0, totalPages = 0.0;
 char buf[512];
 while(fgets(buf, sizeof(buf), fpIn)!= NULL)
 {
 if (strncmp(buf,"Pages", 5) == 0)
 {
 double val = ParseMemValue(buf);
 if (val >= 0.0)
 {
 if ((strncmp(buf,"Pages wired", 11) == 0)||(strncmp(buf,"Pages active", 12) == 0)) pagesUsed += val;
 totalPages += val;
 }
 }
 else if (strncmp(buf,"Mach Virtual Memory Statistics", 30)!= 0) break;//Stop at"Translation Faults", we don't care about anything at or below that
 }
 pclose(fpIn);
 if (totalPages> 0.0) return (float) (pagesUsed/totalPages);
 }
 return -1.0f;//indicate failure
}


#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
static unsigned long long _previousTotalTicks = 0;
static unsigned long long _previousIdleTicks = 0;
//Returns 1.0f for"CPU fully pinned", 0.0f for"CPU idle", or somewhere in between
//You'll need to call this at regular intervals, since it measures the load between
//the previous call and the current one.
float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
 unsigned long long totalTicksSinceLastTime = totalTicks-_previousTotalTicks;
 unsigned long long idleTicksSinceLastTime = idleTicks-_previousIdleTicks;
 float ret = 1.0f-((totalTicksSinceLastTime> 0)? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);
 _previousTotalTicks = totalTicks;
 _previousIdleTicks = idleTicks;
 return ret;
}

float GetCPULoad()
{
 host_cpu_load_info_data_t cpuinfo;
 mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
 if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS)
 {
 unsigned long long totalTicks = 0;
 for(int i=0; i<CPU_STATE_MAX; i++) totalTicks += cpuinfo.cpu_ticks[i];
 return CalculateCPULoad(cpuinfo.cpu_ticks[CPU_STATE_IDLE], totalTicks);
 }
 else return -1.0f;
}

#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

int getNumberOfCores() {
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if(count < 1) {
	nm[1] = HW_NCPU;
	sysctl(nm, 2, &count, &len, NULL, 0);
	if(count < 1) { count = 1; }
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}


/*  McUsr put this together, and into public domain, 
    without any guarrantees about anything,
    but the statement that it works for me.
*/

#if 1 == 1
#define TESTING
#endif

#include <sys/param.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct osver {
    int minor;
    int sub;
} ;
typedef struct osver osxver ;
void macosx_ver(char *darwinversion, osxver *osxversion ) ;
char *osversionString(void) ;

#ifdef TESTING
int main2( int argc, char *argv[] )
{
    osxver foundver;
    char *osverstr= NULL ;
    osverstr=osversionString() ;
    macosx_ver(osverstr, &foundver ) ;
    printf("Mac os x version = 10.%d.%d\n",foundver.minor,foundver.sub );
    free(osverstr);
    return 0;
}
#endif
char *osversionString(void) {
    int mib[2];
    size_t len;
    char *kernelVersion=NULL;
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSRELEASE;

    if (sysctl(mib, 2, NULL, &len, NULL, 0) < 0 ) {
        fprintf(stderr,"%s: Error during sysctl probe call!\n",__PRETTY_FUNCTION__ );
        fflush(stdout);
        exit(4) ;
    }

    kernelVersion = (char*)malloc(len );
    if (kernelVersion == NULL ) {
        fprintf(stderr,"%s: Error during malloc!\n",__PRETTY_FUNCTION__ );
        fflush(stdout);
        exit(4) ;
    }
    if (sysctl(mib, 2, kernelVersion, &len, NULL, 0) < 0 ) {
        fprintf(stderr,"%s: Error during sysctl get verstring call!\n",__PRETTY_FUNCTION__ );
        fflush(stdout);
        exit(4) ;
    }

    return kernelVersion ;
}

void macosx_ver(char *darwinversion, osxver *osxversion ) {
/*
    From the book Mac Os X and IOS Internals:
    In version 10.1.1, Darwin (the core OS) was renumbered from v1.4.1 to 5.1,
    and since then has followed the OS X numbers consistently by being four
    numbers ahead of the minor version, and aligning its own minor with the
    sub-version.
*/
    char firstelm[2]= {0,0},secElm[2]={0,0};

    if (strlen(darwinversion) < 5 ) {
        fprintf(stderr,"%s: %s Can't possibly be a version string. Exiting\n",__PRETTY_FUNCTION__,darwinversion);
        fflush(stdout);
        exit(2);
    }
    char *s=darwinversion,*t=firstelm,*curdot=strchr(darwinversion,'.' );

    while ( s != curdot )
        *t++ = *s++;
    t=secElm ;
    curdot=strchr(++s,'.' );
    while ( s != curdot )
        *t++ = *s++;
    int maj=0, min=0;
    maj= (int)strtol(firstelm, (char **)NULL, 10);
    if ( maj == 0 && errno == EINVAL ) {
        fprintf(stderr,"%s Error during conversion of version string\n",__PRETTY_FUNCTION__);
        fflush(stdout);
        exit(4);
    }

    min=(int)strtol(secElm, (char **)NULL, 10);

    if ( min  == 0 && errno == EINVAL ) {
        fprintf(stderr,"%s: Error during conversion of version string\n",__PRETTY_FUNCTION__);
        fflush(stdout);
        exit(4);
    }
    osxversion->minor=maj-4;
    osxversion->sub=min;
}


#include <sys/utsname.h>	//uname
int main1/*07*/ (int argc, char ** argv) {
    struct utsname u;
    if (uname(&u) != -1) {
        printf("获取当前系统的信息如下\n"
               "sysname:%s\n"
               "nodename:%s\n"
               "release:%s\n"
               "version:%s\n"
               "machine:%s\n"
               , u.sysname, u.nodename, u.release, u.version, u.machine);
    }
}

int main(int parameter_size, char **parameter)
{
    main1(0,0);
    main2(0,0);
    int a = 1 + 3;
    int b = a + 3;
     cout << "cpucore  " << getNumberOfCores() << "   " << endl;
        cout << "mem  " << getMemorySize() << "   " << endl;
    cout << "cpuload  " << GetCPULoad() << "   " << endl;
    char buff[1024];
getmacaddress(buff);
 cout << "macaddress:  " << buff << "   " << endl;

    return 0;
}



//mac end

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
 
// // mostly need to read the linux config files to get system info
 
// // ---- get os info ---- //
// void getOsInfo()
// {
//     FILE *fp = fopen("/proc/version", "r");
//     if(NULL == fp)
//         printf("failed to open version\n");
//     char szTest[1000] = {0};
//     while(!feof(fp))
//     {
//         memset(szTest, 0, sizeof(szTest));
//         fgets(szTest, sizeof(szTest) - 1, fp); // leave out \n
//         printf("%s", szTest);
//     }
//     fclose(fp);
// }
 
// // ---- get cpu info ---- //
// void getCpuInfo()
// {
//     FILE *fp = fopen("/proc/cpuinfo", "r");
//     if(NULL == fp)
//         printf("failed to open cpuinfo\n");
//     char szTest[1000] = {0};
//     // read file line by line
//     while(!feof(fp))
//     {
//         memset(szTest, 0, sizeof(szTest));
//         fgets(szTest, sizeof(szTest) - 1, fp); // leave out \n
//         printf("%s", szTest);
//     }
//     fclose(fp);
// }
 
 
// // ---- get memory info ---- //
// void getMemoryInfo()
// {
//     FILE *fp = fopen("/proc/meminfo", "r");
//     if(NULL == fp)
//         printf("failed to open meminfo\n");
//     char szTest[1000] = {0};
//     while(!feof(fp))
//     {
//         memset(szTest, 0, sizeof(szTest));
//         fgets(szTest, sizeof(szTest) - 1, fp);
//         printf("%s", szTest);
//     }
//     fclose(fp);
// }
 
// // ---- get harddisk info ---- //
// #include <fcntl.h>
// #include <sys/ioctl.h>
// #include <linux/hdreg.h>
 
// void getHardDiskInfo()
// {
//     // use cmd, this is the only way
//     static struct hd_driveid hd;
//     int fd;
 
//     if ((fd = open("/dev/sda", O_RDONLY | O_NONBLOCK)) < 0)
//     {
//         printf("ERROR opening /dev/sda\n");
//         return;
//     }
 
//     if (!ioctl(fd, HDIO_GET_IDENTITY, &hd))
//     {
//         printf("model ", hd.model);
//         printf("HardDisk Serial Number: %.20s\n", hd.serial_no);
//     }
//     else
//         printf("no available harddisk info");
// }
 
// // ---- get network info ---- //
// #include <ifaddrs.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <sys/ioctl.h>
// #include <linux/if.h>
// void getNetworkInfo()
// {
//     // get ip, works for linux and mac os, best method
//     struct ifaddrs *ifAddrStruct = NULL;
//     struct ifaddrs *ifa = NULL;
 
//     getifaddrs(&ifAddrStruct);
 
//     for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
//     {
//         if (!ifa->ifa_addr)
//             continue;
 
//         // check if IP4
//         if (ifa->ifa_addr->sa_family == AF_INET)
//         {
//             void *tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
//             char local_ip[INET_ADDRSTRLEN];
//             inet_ntop(AF_INET, tmpAddrPtr, local_ip, INET_ADDRSTRLEN);
 
//             // actually only need external ip
//             printf("%s IP: %s\n", ifa->ifa_name, local_ip);
//         }
//     }
 
//     if (ifAddrStruct)
//         freeifaddrs(ifAddrStruct);
 
 
//     // get mac, need to create socket first, may not work for mac os
//     struct ifreq ifr;
//     int fd = socket(AF_INET, SOCK_DGRAM, 0);
 
//     char local_mac[128] = { 0 };
 
//     strcpy(ifr.ifr_name, "eth0"); // only need ethernet card
//     if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
//     {
//         char temp_str[10] = { 0 };
//         memcpy(temp_str, ifr.ifr_hwaddr.sa_data, 6);
//         sprintf(local_mac, "%02x-%02x-%02x-%02x-%02x-%02x", temp_str[0]&0xff, temp_str[1]&0xff, temp_str[2]&0xff, temp_str[3]&0xff, temp_str[4]&0xff, temp_str[5]&0xff);
//     }
 
//     printf("Local Mac: %s\n", local_mac);
 
// }
 
// int main(int argc, char **argv)
// {
//     printf("=== os information ===\n");
//     getOsInfo();
 
//     printf("=== cpu infomation ===\n");
//     getCpuInfo();
 
//     printf("=== memory information ===\n");
//     getMemoryInfo();
 
//     printf("=== harddisk information ===\n");
//     getHardDiskInfo();
 
//     printf("=== network information ===\n");
//     getNetworkInfo();
 
//     return 0;
// }
 
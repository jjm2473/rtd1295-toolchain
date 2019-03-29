/**
 * reverse enginering of zidoo z9s 2.1.30 /etc/init (sha256sum 9b3f9648460c15d345986b3246e0471057fb7d4af17fbba0b654f856e1a13e2a)
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <sys/wait.h>
#include <sched.h> // setns

#include "cmdline.h"

#define ANDROID_ROOT "/android"
#define ANDROID_VIFACE "veth1"
#define ANDROID_VIFACE_NAME "eth9"
#define NET_NS_DIR "/var/run/netns"
#define ANDROID_NET_NS "androidnet"

#define OPENWRT_INIT "/etc/preinit"

int main(int argc, char **argv, char **envp) {
    #define EXEC(pre, cmd) argv[0] = cmd;\
    pre execv(cmd, argv);

    int wstatus;
    int *sp58 = &wstatus;
    struct stat sb;
    struct stat *sp80 = &sb;
    char sp100[0x80];
    char sp180[0x80];
    char *hardware = get_cmdline("androidboot.hardware", sp100, 0x80);
    char *exec_cmd = NULL;
    if (hardware != NULL && strncmp(hardware, "kylin", 0x5) == 0) {
        fputs("===== OpenWRT + Android ===== ", stderr);
        int router = 0;
        memset(sp180, 0, sizeof(sp180));
        if (stat("/mnt/android/.ottwifi", sp80) != 0) {
            // if /mnt/android/.ottwifi not exists
            router = 1;
            fputs("===== Router mode ===== ", stderr);    
        }

        // Openwrt + Android
        pid_t cpid = fork(); // 0x400f48
        if (cpid == 0) {
            // openwrt init
            // 0x400f54
            setsid();
            unshare(CLONE_NEWPID); //movz w0, 0x2000, lsl 16
            pid_t cid = fork();//0x400f60
            if (cid == 0) {//0x400f64
                //0x400f68 - 0x400f80
                EXEC(, OPENWRT_INIT)
                _exit(1);
            } else { //0x400f84
                //0x400f84 - 0x400f94
                waitpid(cid, sp58, 0);
                exit(0);
            }
        } else if (cpid < 0) { //0x400f98
            // fork failed
            //0x400f9c : movn w0, 0
            //b 0x4012c8
            return 0;
        } else {
            // android init
            // 0x400fa4
            mount("tmpfs", ANDROID_ROOT, "tmpfs", MS_NOATIME, "size=10240k");
            putenv("PATH=/usr/sbin:/usr/bin:/sbin:/bin");
            snprintf(sp180, 0x80, "cp -a /mnt/android/* %s", ANDROID_ROOT);
            if (system(sp180) < 0 || chdir(ANDROID_ROOT) < 0) {
                // 0x00400ffc: movz w0, 0x1
                // b 0x4012c8
                return 1;
            }
            // 0x00401010
            int debuggable = 1;
            char *cs = get_cmdline("console.switch", sp100, 0x80);
            if (cs != NULL && strncmp(cs, "openwrt", 0x7) == 0) {
                //  set ro.debuggable=0;
                debuggable = 0;
            }
            // else {
            //     //  set ro.debuggable=1;
            //     // debuggable = 1;
            // }
            snprintf(sp180, 0x80, "sed -i 's,^\\(ro.debuggable=\\)\\(.*\\),\\1%d,g' %s", debuggable, "./default.prop");
            system(sp180); // 0x401078
            int ttl;
            for (ttl = 101; ttl>0; --ttl) {
                if (stat(".coldplug_done", sp80) == 0) {
                    break;
                }
                usleep(100000);
            }
            if (router == 0) { // 0x004010a4
                // 0x00401284 - 0x0040129c
                close(open("./.ottwifi", __O_CLOEXEC|O_CREAT|O_WRONLY));
                // 0x004012a0 // end if
            } else {
                // router mode init
                // 0x004010a8 - 0x401280
                // sp+0x78 = 0*8;
                // ((int64_t *)sp58)[0] = 0;//sp+0x58 = 0*16; // char[]
                // ((int64_t *)sp58)[1] = 0;//sp+0x58 = 0*16; // char[]
                // sp+0x68 = 0*16;
                
                // sp+0x50 = 0;
                // sp+0x40 = 0;

                //*sp58 = *((int *)"eth0");

                // get mac address of eth0
                // 0x10ec
                int w23 = 0xda, w24 = 0x05;
                int fd = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
                if (fd >= 0) { // 0x10e8
                    struct ifreq ifr;
                    memset(ifr.ifr_name, 0, sizeof(ifr));
                    strncpy(ifr.ifr_name, "eth0", 4);
                    // 0x10f8
                    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
                        // 0x110c
                        w23 = (ifr.ifr_hwaddr.sa_data[4] & 0xFF); // sp+0x6e; ifr+0x16
                        w24 = (ifr.ifr_hwaddr.sa_data[5] & 0xFF); // sp+0x6f; ifr+0x17
                    }
                    close(fd);
                } 
                // else {
                //     // 0x10ec
                // }

                // 0x401118
                mount("proc", "/proc", "proc", MS_NOATIME|MS_NOEXEC|MS_NODEV|MS_NOSUID, NULL);
                mkdir(NET_NS_DIR, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH); // mode 0755

                // ip netns add androidnet
                snprintf(sp180, 0x80, "ip netns add %s", ANDROID_NET_NS);
                system(sp180);

                char veth_mac_addr[0x12];
                memset(veth_mac_addr, 0, sizeof(veth_mac_addr));

                // ip link add veth1 addr 00:e0:4c:0b:??:?? type veth peer name eth9 addr 00:e0:4c:ab:??:??
                snprintf(veth_mac_addr, 0x12, "%02x:%02x:%02x:%02x:%02x:%02x", 0x00, 0xe0, 0x4c, 0x0b, w23, w24);
                snprintf(sp180, 0x80, "ip link add %s addr %s type veth peer name %s addr ", ANDROID_VIFACE, veth_mac_addr, ANDROID_VIFACE_NAME);
                veth_mac_addr[9] = 'a';
                strncat(sp180, veth_mac_addr, strlen(veth_mac_addr));
                system(sp180);

                // ip link set eth9 netns androidnet
                snprintf(sp180, 0x80, "ip link set %s netns %s", ANDROID_VIFACE_NAME, ANDROID_NET_NS);
                system(sp180);

                // ip netns exec androidnet ip link set dev lo up
                snprintf(sp180, 0x80, "ip netns exec %s ip link set dev lo up", ANDROID_NET_NS);
                system(sp180);

                snprintf(sp180, 0x80, "%s/%s", NET_NS_DIR, ANDROID_NET_NS);
                fd = open(sp180, 0);
                setns(fd, CLONE_NEWNET);
                close(fd);
                umount("/proc");
                // 0x401280 : b 0x4012a0
            }

            // 0x004012a0
            unshare(CLONE_FS);
            chroot(".");
            exec_cmd = "/init";
            //return execv("/init", ["/init"]); // 0x4012bc
        }
    } else {
        fputs("===== OpenWRT ===== ", stderr);
        exec_cmd = OPENWRT_INIT;
        //return execv("/etc/preinit", ["/etc/preinit"]); // 0x4012bc
    }
    EXEC(return, exec_cmd)
}

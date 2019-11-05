# 1   Setup Build Environment

## 1.1     The Ubuntu release

The Ubuntu 14.04 64 bit release is the recommend developing environment. You might be able to use the later Ubuntu release as your developing environment as well.

## 1.2     Install the Compile Toolchain

### 1.2.1 Download the necessary toolchains from Realtek FTP site.

| ToolChain Name          | FTP Site                                                     | Remark          |
| ----------------------- | ------------------------------------------------------------ | --------------- |
| icedtea-bin-7.2.4.7.tgz | `ftp://rtkwifidisplay:7Fc4dH@ftp3.realtek.com/1295ToolChain/icedtea-bin-7.2.4.7.tgz` | Build Android   |
| JAVA.sh                 | `ftp://rtkwifidisplay:7Fc4dH@ftp3.realtek.com/1295ToolChain/JAVA.sh` | Set JAVA Tool   |
| android-ndk-r9c.tgz     | `ftp://rtkwifidisplay:7Fc4dH@ftp3.realtek.com/1295ToolChain/android-ndk-r9c.tgz` | Build DvdPlayer |

 

### 1.2.2 Toolchain installation (instructions in GREEN)

**a) Untar ‘icedtea-bin-7.2.4.7.tgz’ to the Home Directory of Users.**

**b) Untar ‘android-ndk-r9c.tgz’ to ‘/opt’**

```shell
sudo ln -s android-ndk-r9c android-ndk #Make a static link
```

**c) Install the toolchain of uboot**

```shell
sudo apt-get install u-boot-tools

sudo apt-get install libssl-dev
```
Install libswitch-perl to pack the secure install.img

```shell
sudo apt-get install libswitch-perl
```

**d) Copy ‘JAVA.sh’ to the Home Directory of Users**

**e) Re-Link the ‘sh’ in ‘/bin’**

```shell
cd /bin
sudo unlink sh
sudo ln –s bash sh
```

**f)`sudo apt-get install bison`**

**g)`sudo apt-get install libxml2-utils`**

**h)** add move

```
apt-get install g++-multilib gcc-multilib lib32ncurses5-dev lib32readline-gplv2-dev lib32z1-dev
```

# 2   Build Secure install.img

## 2.1   Bootcode -加密的请使用附的大客户版本LK

1. Place the corresponding HW Setting to "tools/flash_writer/image/hw_setting/rtd1295/demo/0002"

 RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866.config

 

1. There are two files that you could modify with your hardware configuration for security.

 

* project/target/rtd1295.mk

```makefile
CHIP_ID ?= rtd1295

CUSTOMER_ID ?= demo

CHIP_TYPE ?= 0002

PRJ ?= 1296_force_emmc_S_E

NAS_ENABLE ?= 1

LK_FW_LOAD ?= 0
```


* tools/flash_writer/inc/1296_force_emmc_S_E.inc

(Be selected with “project/target/rtd1295.mk”)

```makefile
Board_HWSETTING = RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866.
```


1. Build bootcode

```
./build_rtk_lk.sh rtd1295
```

 

Output :

bootloader_lk.tar

 

 

## 2.2   Kernel

 Cd Wrt/

 Make menuconfig

 

## 2.3   Image file and Makefile.in

1. Copy the bootcode to ‘image_file’
```
cp bootloader_lk.tar to ‘target/linux/rtd1295/image/image_file-r1005776/packages/omv’
```
2. Put the AES/RSA keys into ‘OpenWrt-ImageBuilder-rtd1295-nas_emmc.Linux-x86_64/target/linux/rtd1295/image/’

SDK 下载下来这个位置下有9把KEY 但不知道和bootcode 是否一致，所以手动再考一次

并且最好备份起来，以免后续换了Bootcode 什么的丢失

Copy these keys from Bootcode/tools/flash_write/image

(1) aes_128bit_key.bin

(2) aes_128bit_key_1.bin

(3) aes_128bit_key_2.bin

(4) aes_128bit_key_3.bin

(5) aes_128bit_seed.bin

(6) rsa_key_2048.fw.pem

(7) rsa_key_2048.tee.pem

(8) rsa_key_2048.pem

(9) rsa_key_2048.pem.bin.rev


To change the keys with your own, please follow the steps below.

Change the AES keys including

  aes_128bit_key_1.bin

  aes_128bit_key_2.bin

  aes_128bit_key_3.bin

  aes_128bit_key.bin

  aes_128bit_seed.bin


Please use vi to modify these 5 AES keys directly. Be aware that each should be 16 bytes.

Steps: (Please progress in sequence, do not skip or reverse steps)

 (1) Open binary file with vi

```
#vi -b aes_128bit_seed.bin
```

 (2) Transfer to hexadecimal

  : :%!xxd

   0000000: 11cd ef12 1357 2468 a1b2 c3d4 9090 babe .....W$h........

   0000010: 0a                    .

 (3) Edit the content of AES bin file

 (4) Transfer to binary

   : :%!xxd -r

 (5)Save and exit

   :wq


Change the RSA keys including

  rsa_key_2048.fw.pem

  rsa_key_2048.pem

  rsa_key_2048.tee.pem

 

  These three RSA should use the instruction of 'openssl genrsa -out KEY' 2048' to generate.

  For example, `openssl genrsa -out rsa_key_2048.fw.pem 2048`

 

3. Put the efuse utilities into ‘OpenWrt-ImageBuilder-rtd1295-nas_emmc.Linux-x86_64/target/linux/rtd1295/image/’

Copy it from tools/efuse_verify/out

(1) efuse_programmer.complete.enc

(2) efuse_verify.bin



4.  Modify Makefile.in

```shell
vim OpenWrt-ImageBuilder-rtd1295-nas_emmc.Linux-x86_64/target/linux/rtd1295/image/Makefile.in
```


注意：

制作加密包时，

用于MPTOOL 的生产包，**secure_boot:=y，efuse_fw=1，efuse_key?=1，offline_gen=y，install_bootloader=1**

之后用来USB 升级的以及制作OTA的包

**secure_boot:=y，install_bootloader=1**（这个根据需求决定是否打开）

```makefile
# default value

# 1 = yes, 0 = no

install_bootloader=1

install_factory?=0

update_etc=1

stop_reboot=0

only_install_factory=0  # TODO: need confirm?

only_install_bootcode=0  # TODO: need confirm?

jffs2_nocleanmarker=0

install_dtb=1

 

# value

install_avfile_count=0

reboot_delay=5

customer_delay=0

rba_percentage=5

# 0: means use defalt;FAIL:1/INFO:2/LOG:4/DEBUG:8/WARNING:16/UI:32/TARLOG:256/MEMINFO:512

logger_level=0

layout_type=emmc

#layout_type=$(SUBTARGET)

layout_size:=$(if $(CONFIG_TARGET_LAYOUT_SIZE_8GB),8gb,4gb)

layout_size:=$(if $(CONFIG_TARGET_LAYOUT_SIZE_16GB),16gb,$(layout_size))

layout_size:=$(if $(CONFIG_TARGET_LAYOUT_SIZE_32GB),32gb,$(layout_size))

secure_boot:=y   直接写死不要有MAKEFILE 传参数

efuse_key?=1

chip_rev:=2      直接写死不要有MAKEFILE 传参数

hash_imgfile=1

verify=1

offline_gen=y

gen_install_binary?=n

ANDROID_IMGS?=n

HYPERVISOR=$(if $(CONFIG_XEN),y,n)

efuse_fw=1
```

5. 处理bluecore.audio

将要使用的bluecore.audio ,zip起来放到这两个位置替换原有

* OpenWrt-ImageBuilder-rtd1295-mnas_emmc.Linux-x86_64/target/linux/rtd1295/image/files

* bluecore.audio.release_160705_81b00000.SQA.zip

* OpenWrt-ImageBuilder-rtd1295-mnas_emmc.Linux-x86_64/target/linux/rtd1295/image/files/fw

* bluecore.audio.zip

 

LK 烧录时，注意TOOL 要选择lk 打勾

 

 

 

# 3   Generate Image

## 3.1   Build install image

```shell
cd OpenWrt-ImageBuilder-rtd1295-nas_emmc.Linux-x86_64
make modules V=99 -j4; make image PACKAGES=ALL
```

 

Output:

OpenWrt-ImageBuilder-rtd1295-nas_emmc.Linux-x86_64/bin/rtd1295-glibc/install.img

 

## 3.2   Check install.img 

Please check the components inside install.img, layout.txt and config.txt before doing installation, to avoid burning security failure. The files marked in red are only generated for security, and the files marked in blue are different from the normal installed package.


```
bootcode_lk.tar
├── bl31_enc.bin
├── bootloader_lk.tar
├── fsbl.bin
├── hw_setting.bin
├── lk.bin
├── rsa_bin_fw.bin
├── rsa_bin_tee.bin
└── tee_enc.bin
```

```
Install.img  
├── aes_128bit_key_1.bin
├── aes_128bit_key_2.bin
├── aes_128bit_key_3.bin
├── aes_128bit_key.bin
├── aes_128bit_seed.bin
├── ALSADaemon
├── config.txt
├── customer.tar
├── install_a
├── install.img
├── layout.txt
├── mke2fs
├── omv
│   ├── android.emmc.dtb
│   ├── bluecore.audio.aes
│   ├── bootloader_lk.tar
│   ├── efuse_programmer.complete.enc
│   ├── efuse_verify.bin
│   ├── emmc.uImage.aes
│   ├── etc.bin
│   ├── fw_tbl.bin
│   ├── gold.bluecore.audio.aes
│   ├── gold.emmc.uImage.aes
│   ├── gold.rescue.emmc.dtb
│   ├── gold.rescue.root.emmc.cpio.gz_pad.img.aes
│   ├── mbr_00.bin
│   ├── rescue.emmc.dtb
│   ├── rescue.root.emmc.cpio.gz_pad.img.aes
│   └── squashfs1.img
├── otp_key_verify.aes
├── rsa_key_2048.pem.bin.rev
└── teeUtility.tar
```

**config.txt**
```
# Package Information

company=""

description=""

modelname=""

version=""

releaseDate=""

signature=""

# Package Configuration

start_customer=y

verify=y

bootcode=y

install_dtb=y

update_etc=y

install_avfile_count=0

reboot_delay=5

efuse_key=1

efuse_fw=1

rpmb_fw=0

secure_boot=1

fw = GOLDrescueDT omv/gold.rescue.emmc.dtb 0x02140000

fw = GOLDRootFS omv/gold.rescue.root.emmc.cpio.gz_pad.img.aes  0x30000000

fw = GOLDKernel omv/gold.emmc.uImage.aes 0x03000000

fw = GOLDaudio omv/gold.bluecore.audio.aes 0x0F900000

fw = rescueDT omv/rescue.emmc.dtb 0x02140000

fw = kernelDT omv/android.emmc.dtb 0x02100000

fw = rescueRootFS omv/rescue.root.emmc.cpio.gz_pad.img.aes 0x30000000

fw = linuxKernel omv/emmc.uImage.aes 0x03000000

fw = audioKernel omv/bluecore.audio.aes 0x0F900000

###

###          part = (name mount_point filesystem file size)

part = rootfs / squashfs omv/squashfs1.img 100663296

#part = rootfs / ext4 omv/root.bin 268435456

part = etc etc ext4 omv/etc.bin 41943040

###

###          part = (name mount_point filesystem file size)

```

**layout.txt**

```
#define CREATE_DATE " Nov  4 2016 "

#define CREATE_TIME " 19:09:15 "

#define BOOTTYPE " BOOTTYPE_COMPLETE "

#define SSUWORKPART 0

#define BOOTPART 0

#define FW_KERNEL " target=3000000 offset=29ab200 size=acfa44 type=bin name=omv/emmc.uImage.aes "

#define FW_RESCUE_DT " target=2140000 offset=2870200 size=10069 type=bin name=omv/rescue.emmc.dtb "

#define FW_KERNEL_DT " target=2100000 offset=28b0200 size=100ff type=bin name=omv/android.emmc.dtb "

#define FW_RESCUE_ROOTFS " target=30000000 offset=347ae00 size=400244 type=bin name=omv/rescue.root.emmc.cpio.gz_pad.img.aes "

#define FW_AKERNEL " target=f900000 offset=28f0200 size=baf04 type=bin name=omv/bluecore.audio.aes "

#define FW_FWTBL " target=0 offset=620000 size=e70 type= name=omv/fw_tbl.bin "

#define FW_GOLD_KERNEL " target=3000000 offset=b70200 size=acfa44 type=bin name=omv/gold.emmc.uImage.aes "

#define FW_GOLD_RESCUE_DT " target=2140000 offset=630200 size=10069 type=bin name=omv/gold.rescue.emmc.dtb "

#define FW_GOLD_ROOTFS " target=30000000 offset=1b70200 size=400244 type=bin name=omv/gold.rescue.root.emmc.cpio.gz_pad.img.aes "

#define FW_GOLD_AKERNEL " target=f900000 offset=670200 size=baf04 type=bin name=omv/gold.bluecore.audio.aes "

#define PART0 " offset=e1800000 size=2800000 mount_point=etc mount_dev=/dev/block/mmcblk0p2 filesystem=ext4 partname=etc type=bin name=omv/etc.bin "

#define PART1 " offset=db800000 size=6000000 mount_point=/ mount_dev=/dev/block/mmcblk0p1 filesystem=squashfs partname=rootfs type=img name=omv/squashfs1.img "

#define MBR0 " offset=0 size=200 name=omv/mbr_00.bin "

#define TAG 45

#define FW_EFUSE_VERIFY " target=0x01610000 offset=0 size=13438 type=bin name=omv/efuse_verify.bin "

#define FW_EFUSE_PROGRAMMER " target=0x01700000 offset=0 size=17248 type=bin name=omv/efuse_programmer.complete.enc "
```



_( backup from [https://www.cnblogs.com/yizhier/p/11399065.html](https://www.cnblogs.com/yizhier/p/11399065.html) [http://www.manongzj.com/blog/4-bkfngccvqytpbwa.html](http://www.manongzj.com/blog/4-bkfngccvqytpbwa.html) )_
## Example 1

Set up a 4PPC70 with AR G4.3.4 e.g. with 'Project Installation' (used if AR > 4.25)

```batch

Remark "remote install 4PPC70 via network (Project installation AR > 4.2.5)"
StartPviMan "LoadLocal"
Remark "Create Partition"
Call "brsnmp.exe", "--ipMethod=0 --filter=PPC7 --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=PPC7", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
CFCreatePart "HD0", "1", "100, 'SYSTEM'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "60"
ClearError
OnErrorBreak

Remark "transfer AR with system modules included"
Call "brsnmp.exe", "--ipMethod=0 --filter=PPC7 --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=PPC7", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
ARUpdateFileGenerate ".\PPC7xG43.s14", ".\arupdate.br", "arconfig.br | asfw.br | ashwd.br | sysconf.br"
Download "arupdate.br", "ROM"
Warmstart "120"
Remark "We've only one partition here"


Remark "transfer 'real' project with SAFE file system"
Transfer ".\RucPackage_Config1.zip", "InstallMode=ForceInitialInstallation TryToBootInRUNMode=1 ResumeAfterRestart=1"
Remark "Reboot and wait for Repartitioning"
Coldstart "180"
Remark "We've three ore four partitions now"


StopPviMan

```

## Example 2

In some cases we do not find satisfactory filter settings e.g. when many identical cpu types are connected to the same network,
identical node settings, unknown MAC addresses or serial numbers ...
If a connection  to the targeted cpu is already available we can use the current IP address if
we use the $LAST expression for filtering:

```batch

Remark "remote install 4PPC70 via network (Project installation AR > 4.2.5)"
Remark "PLC is already reachable via IP address and we use brsnmp >= V1.0"
Remark "With --filter=$LAST the MAC is stored across all calls of brsnmp"

StartPviMan "LoadLocal"
OnErrorBreak

Remark "Test if a cpu is reachable"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=1000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"

Remark "Create Partition"
Call "brsnmp.exe", "--ipMethod=0 --filter=192.168.0.14 --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
CFCreatePart "HD0", "1", "100, 'SYSTEM'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "60"
ClearError
OnErrorBreak

Remark "transfer AR with system modules included"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
ARUpdateFileGenerate ".\PPC7xG43.s14", ".\arupdate.br", "arconfig.br | asfw.br | ashwd.br | sysconf.br"
Download "arupdate.br", "ROM"
Warmstart "120"
Remark "We've only one partition here"


Remark "transfer 'real' project with SAFE file system"
Transfer ".\RucPackage_Config1.zip", "InstallMode=ForceInitialInstallation TryToBootInRUNMode=1 ResumeAfterRestart=1"
Remark "Reboot and wait for Repartitioning"
Coldstart "180"
Remark "We've three ore four partitions now"

Remark "delete last filter settings"
Call "brsnmp.exe", "--filter= --timeout=50", "HideWindow=1"

StopPviMan

```

## Example 3

Set up a X20CP1301 with AR G4.10 e.g. with "Transfer" (used if AR < 4.25)

```batch

Remark "remote install X20CP1301 via network (transfer AR < 4.2.5)"
StartPviMan "LoadLocal"
OnErrorBreak
Call "brsnmp.exe", "--ipMethod=0 --filter=X20CP1301 --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.7 --subnet=255.255.255.0 --filter=X20CP1301", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=99", "/RT=1000 /AM=* /SDT=5 /DAIP=192.168.0.7 /REPO=11159 /ANSL=1 /PT=11169"
Remark "we use the SAFE file system"
CFCreatePart "HD0", "4", "10, 'SYSTEM'", "20, 'DATA1'", "20, 'DATA2'", "50, 'USER'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "80"
ClearError

Remark "transfer AR with system modules included"
Call "brsnmp.exe", "--ipMethod=0 --filter=X20CP1301 --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.7 --subnetMask=255.255.255.0 --filter=X20CP1301", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.7 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
ARUpdateFileGenerate ".\X20CP1301G41.s1", ".\arupdate.br", "arconfig.br | asfw.br | ashwd.br | sysconf.br"
Download "arupdate.br", "ROM"
Warmstart "180"
Remark "CPU is in RUN -> can download the project now"

StopPviMan

```

## Example 4
Loading X20CP1584 with Onboard-AR = J4.02 to AR D4.52 by help of AR K4.34.
X20CP1x8x come with older Onboard AR (e.g. J4.02) there are some limitations and we need some workarounds...

```
Remark "Loading X20CP1584 with Onboard-AR = J4.02 to AR D4.52 by help of AR K4.34"
Remark "as X20CP1x8x come with older Onboard AR (e.g. J4.02) there are some limitations and we need some workarounds..."

Remark "Start PVI, use local instance"
StartPviMan "LoadLocal"
OnErrorBreak

Remark "Search for CPU and set IP address"
Call "brsnmp.exe", "--ipMethod=0 --filter=X20CP1584 --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST ", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /ANSL=0", "WT=120"
Remark "Detect PLC State"
PLCStatus
IF "Boot" GOTO "DownloadAR"
SSWVersion
IF "K434" GOTO "Installation"

Label: "Delete Memory"
Remark "We will loose connection after deleting memory"
OnErrorResume
Remark "Else: delete Flash first to get enough space for AR-update"
Diagnose "80"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /ANSL=1", "WT=120"
DeleteMemory ROM
DeleteMemory PERMMEM
DeleteMemory NONVOLATILE
Coldstart "80"
ClearError
OnErrorBreak

Label: "DownloadAR"
OnErrorResume
Remark "we need an AR with more capabilities and use K4.34"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /ANSL=1", "WT=120"
Download ".X20CP1584K43.s14", "ROM"
Coldstart "80"
Remark "we've got an unknown number of partitions at this point and downloading AR won't change this"
ClearError
OnErrorBreak


Remark "At this point we should have K4.34 and can use 'Transfer' to install the real projectt (e.g. with D4.52)"
Label: "Installation"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
Remark "Download Projekt with correct number of partitions"
Transfer ".\Package\RucPackage_x20cp1584_Config1.zip", "InstallMode=ForceInitialInstallation TryToBootInRUNMode=1 ResumeAfterRestart=1"
Wait "80"
Remark "Check if new AR is installed"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=80"
SSWVersion
CFInfo


Remark "delete last filter settings"
Call "brsnmp.exe", "--filter= --timeout=50", "HideWindow=1"

StopPviMan

```


## Example 5
remote install X20CP0410 via network (Onboard AR C4.10) with AR4.53

```
OnErrorBreak
Remark "remote install X20CP0410 via network (Onboard AR C4.10) with AR4.53"
StartPviMan "LoadLocal"
Remark "Create Partition to erase Flash"
Call "brsnmp.exe", "--ipMethod=0 --filter=X20CP0410 --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
CFCreatePart "HD0", "1", "100, 'SYSTEM'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "60"
Remark "at this point plc contains Onboard AR only which does not accept 'Transfer' command"


Remark "download 'raw' AR A4.53"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
Download ".\RUCPackage_0\X20CP04xxA45.s13", "ROM"
Warmstart "60"
Remark "We've only one partition here"


Remark "transfer real project"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=99", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /ANSL=1 /PT=11169", "WT=30"
Transfer "RUCPackage_0\RucPackage_X20CP0410_Config1.zip", "InstallMode=ForceInitialInstallation TryToBootInRUNMode=1 ResumeAfterRestart=1"
Wait "60"
Remark "Check if new AR is installed"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST --timeout=5000", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=3000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=80"
SSWVersion

Remark "delete last filter settings"
Call "brsnmp.exe", "--filter= --timeout=50", "HideWindow=1"

StopPviMan


```

## Example 6
factory reset a X20CP0483 via network, e.g. format onboard flash

```
Remark "factory reset X20CP0483 via network"
StartPviMan "LoadLocal"
OnErrorBreak
Call "brsnmp.exe", "--ipMethod=0 --filter=X20CP0483 --timeout=5000", "HideWindow=1"
Remark "set a temporary IP that fits to your network"
Call "brsnmp.exe", "--ipAddress=169.254.62.10 --subnet=255.255.255.0 --filter=$LAST"
Connection "/IF=tcpip /SA=99", "/RT=1000 /AM=* /SDT=5 /DAIP=169.254.62.10 /ANSL=1", "WT=30"
Remark "clear the onboard flash"
CFCreatePart "HD0", "1", "100, 'SYSTEM'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "30"
ClearError
```


## FAQs

*a) brsnmp does'nt work...*

**Answer**: there are a lot of pitfalls using brsnmp in Batch- files or PIL- lists as it gives no valueable return values at the moment. E.g. an invalid '--filter=xxx' will return an empty list '[]' but that's not an failure per se. Please consider to test your settings on commandline via a 'DOS'- box (cmd.exe) instead.


*b) what are the *.s* files (PPC7xG43.s14, X20CP1301G41.s1) and where can I download them ?*

**Answer**: the *.s* files are the 'raw' Automation Runtime modules which you can find in your Automation Studio common path ```..\BrAutomation\As\System\XXXX\``` 

e.g. :

![S_Files](https://github.com/hilch/brsnmp/blob/master/doc/S_Files.PNG)

or just execute menu command 'Project/Export To Runtime Utility Center' (>= AS 4.3) and extract the *.zip file, look inside folder 'AR',

e.g. :

![S_Files_B1](https://github.com/hilch/brsnmp/blob/master/doc/S_Files_B1.PNG)

![S_Files_B2](https://github.com/hilch/brsnmp/blob/master/doc/S_Files_B2.PNG)

![S_Files_B3](https://github.com/hilch/brsnmp/blob/master/doc/S_Files_B3.PNG)

*c) why did you put '--timeout=5000' in combination with '--ipMethod=0' ?*

**Answer**: it seems that some plc need some more time when switching to manual ip address. If anybody has some more information your help will be welcome.

*d) I don't like to install PVI on the target computer*

**Answer**: Execute Automation Studio/ Project/ Export to RUC and you will get a .zip and a transfer.pil.
Create a tempory directory on your harddisk.
After that start Runtime Utility Center and execute 'Create installation package' 
![IconCreateInstallationPackage](https://github.com/hilch/brsnmp/blob/master/doc/IconCreateInstallationPackage.png)
and use the above transfer.pil as source and the directory above as target. RUC will copy all necessary PVI files to this directory.
Starting with Version 4.5.2 PVI should run fine without former installation.


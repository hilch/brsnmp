## Example 1

Setup a 4PPC70 with AR G4.3.4 e.g. with 'Project Installation' (used if AR > 4.25)

```batch

Remark "remote install 4PPC70 via network (Project installation AR > 4.2.5)"
StartPviMan "LoadLocal"
Remark "Create Partition"
Call "brsnmp.exe", "--ipMethod=0 --filter=PPC7", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=PPC7", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
CFCreatePart "HD0", "1", "100, 'SYSTEM'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "60"
ClearError
OnErrorBreak

Remark "transfer AR with system modules included"
Call "brsnmp.exe", "--ipMethod=0 --filter=PPC7", "HideWindow=1"
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

In some cases we do not find satisfactory filter settings e.g. many identical cpu types in network,
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
Call "brsnmp.exe", "--ipMethod=0 --filter=192.168.0.14", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --filter=$LAST", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
CFCreatePart "HD0", "1", "100, 'SYSTEM'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "60"
ClearError
OnErrorBreak

Remark "transfer AR with system modules included"
Call "brsnmp.exe", "--ipMethod=0 --filter=$LAST", "HideWindow=1"
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

Setup a X20CP1301 with AR G4.10 e.g. with "Transfer" (used if AR < 4.25)

```batch

Remark "remote install X20CP1301 via network (transfer AR < 4.2.5)"
StartPviMan "LoadLocal"
OnErrorBreak
Call "brsnmp.exe", "--ipMethod=0 --filter=X20CP1301", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.7 --subnet=255.255.255.0 --filter=X20CP1301", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=99", "/RT=1000 /AM=* /SDT=5 /DAIP=192.168.0.7 /REPO=11159 /ANSL=1 /PT=11169"
Remark "we use the SAFE file system"
CFCreatePart "HD0", "4", "10, 'SYSTEM'", "20, 'DATA1'", "20, 'DATA2'", "50, 'USER'"
CFFormatPart "HD0", "C", "SYSTEM"
Warmstart "80"
ClearError

Remark "transfer AR with system modules included"
Call "brsnmp.exe", "--ipMethod=0 --filter=X20CP1301", "HideWindow=1"
Call "brsnmp.exe", "--ipAddress=192.168.0.7 --subnetMask=255.255.255.0 --filter=X20CP1301", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=30000 /AM=* /SDT=5 /DAIP=192.168.0.7 /REPO=11159 /PT=11169 /ANSL=1", "WT=30"
ARUpdateFileGenerate ".\X20CP1301G41.s1", ".\arupdate.br", "arconfig.br | asfw.br | ashwd.br | sysconf.br"
Download "arupdate.br", "ROM"
Warmstart "180"
Remark "CPU is in RUN -> can download the project now"

StopPviMan

```

## FAQs

 - where can I download PPC7xG43.s14, X20CP1301G41.s1 ?

**Answer**: the *.s* files are the 'raw' Automation Runtime modules which you can find in your Automation Studio common path ```..\BrAutomation\As\System\XXXX\``` 

e.g. :

![S_Files](https://github.com/hilch/brsnmp/blob/master/doc/S_Files.PNG)

or just execute menu command 'Project/Export To Runtime Utility Center' (>= AS 4.3) and extract the *.zip file, look inside folder 'AR',

e.g. :

![S_Files_B1](https://github.com/hilch/brsnmp/blob/master/doc/S_Files_B1.PNG)

![S_Files_B2](https://github.com/hilch/brsnmp/blob/master/doc/S_Files_B2.PNG)

![S_Files_B3](https://github.com/hilch/brsnmp/blob/master/doc/S_Files_B3.PNG)


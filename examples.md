## Example 1

Setup a 4PPC70 with AR G4.3.4 e.g. with 'project installation'

```batch

Remark "remote install 4PPC70 via network (project installation AR > 4.2.5"
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

**FAQ**: where can I download PPC7xG43.s14 ?

**Answer**: the *.s* files are the 'raw' Automation Runtime modules which you can find in your Automation Studio common path ```..\BrAutomation\As\System\XXXX\``` or just execute menu command 'Project/Export To Runtime Utility Center' and extract the *.zip file, look inside folder 'AR'.

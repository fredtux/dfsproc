# Linux system call to list processes in DFS order


- This project was made for the Operating Systems laboratory from the University of Bucharest
- It is based on the **Linux Kernel 6.0.6**
- Contains the kernel patch, building and debugging config files and 3 userland programs
- The code for the syscall can be located in **dfsproc** directory

# Table of contents
* [PART I : Building a functioning kernel and testing environment with buildroot](https://github.com/fredtux/dfsproc#part-i--building-a-functioning-kernel-and-testing-environment-with-buildroot)
* [PART II : Debugging the kernel](https://github.com/fredtux/dfsproc#part-ii--debugging-the-kernel)
* [PART III : Userland applications](https://github.com/fredtux/dfsproc#part-iii--userland-applications)
# PART I : Building a functioning kernel and testing environment with buildroot
## Preparing the kernel
Use the patch file inluded in `dfsproc.patch` by issuing this command in the linux kernel source code directory
```bash
patch -p1 < dfsproc.path
```
### Optional
If you want to replicate my local testing environment with Qemu, buildroot and the kernel build for debugging, then follow these 2 steps (this tutorial will assume you have completed this step, otherwise you are free to choose your own options):
1. Place the `optional/kernel/.config` file for kernel config in your linux kernel source code directory.
2. For buildroot there is a configuration file that you should place into your buildroot directory. The file is located at `optional/buildroot/.config`

## Building the kernel
Issue the following commands
1. For building the `.config` file
```bash
make menuconfig
```
2. For building the kernel
```bash
make -j $(nproc)
```
3. For building the modules
```bash
make modules -j $(nproc)
```

## Building buildroot
This section is a more streamlined (and less flexible) version of the tutorial found here: [https://www.nullbyte.cat/post/linux-kernel-exploit-development-environment/](https://www.nullbyte.cat/post/linux-kernel-exploit-development-environment/). One more thing is added: VSCode integration for debugging the kernel.

**Replace `/path_to_buildroot` with your own path to the buildroot directory.**

1. Configure buildroot
```bash
make menuconfig
```
2. Add overlay directories
```bash
mkdir /path_to_buildroot/buildroot/overlay/etc/
```

3. Add this text to a new file in `/path_to_buildroot/buildroot/overlay/etc/shadow` (the default user:password will be **root:root** and **user:user**)
```
root:$5$AQRgXbdJ$eCko6aRPrhOBegsJGLy36fmmrheNtfkUMBjlKPWEXW9:10000:0:99999:7:::
daemon:*:10933:0:99999:7:::
bin:*:10933:0:99999:7:::
sys:*:10933:0:99999:7:::
sync:*:10933:0:99999:7:::
mail:*:10933:0:99999:7:::
www-data:*:10933:0:99999:7:::
operator:*:10933:0:99999:7:::
nobody:*:10933:0:99999:7:::
user:$5$QAucgwIL$onnijv2MwdMD.Jze4LgPx7z3kksIjU18y3jffH2urv3:10000:0:99999:7:::
```
4. Add the home folder for the user "user"
```bash
mkdir -p /path_to_buildroot/buildroot/overlay/home/user
```
5. Set the permissions to the device table
```bash
echo -e '/home/user\td\t755\t1000\t100\t-\t-\t-\t-\t-' >> /path_to_buildroot/buildroot/system/device_table.txt
```
## Adding kernel modules to buildroot
For installing the modules with buildroot
```bash
make modules_install INSTALL_MOD_PATH=/path_to_buildroot/buildroot/overlay -j $(nproc)
```

## Compiling buildroot
Use the following commands
```bash
make source
make -j $(nproc)
```

## Starting the kernel
Copy and **modify the paths** in the `optional/start` file and run it. I suggest keeping buildroot and the linux kernel in the same directory for simplicity.
```bash
./start
```
This file will start buildroot within a terminal

## Connecting to the environment
### Connect thorugh ssh
Connect to the user account using the **user** password
```bash
ssh -p 2222 user@localhost -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no
```
### Compiling files for the environment
**Please use -no-pie and --static for C**

As an example we'll take the florin/f1_c.c file.
```bash
gcc f1_c.c -o f1 --static -no-pie
```

### Transferring binaries through scp
```bash
scp -P 2222 f1 user@localhost:/home/user/
```
# PART II : Debugging the kernel
To start debugging issue this command:
```bash
gdb linux-6.0.6/vmlinux
```
To connect to the server issue this command inside gdb
```
target remote localhost:1234
```
To set a breakpoint inside the dfsproc system call at line 100 (change the number with the desired line)
```
b dfsproc/dfsproc.c:100
```
It should start debugging once it hits the breakpoint.
## Debug with VSCode
Copy the `optional/.vscode` directory to the linux kernel source code directory and open the kernel source code directory. Then while the kernel is loaded with buildroot set a debug point and press F5. It should start debugging once it hits the breakpoint.

# PART III : Userland applications
## Florin
My userland programs located in **florin** directory.
### First application
Build the f1_c.c application like this
```bash
gcc f1_c.c -o f1 --static -no-pie
```
Copy it to the virtual machine
```bash
scp -P 2222 f1 user@localhost:/home/user/
```
Connect to the virtual machine either directly (not recommended) or via ssh and run
```bash
./f1
```
### Second application
Copy libgmp.so.10 to the virtual machine
```bash
 scp -P 2222  optional/libgmp.so.10 root@localhost:/lib/libgmp.so.10
 ```
Compile the file with either stack or cabal. For stack use:
```bash
stack build
```
Copy the resulting file to the virtual machine
```bash
scp -P 2222 f2 user@localhost:/home/user/
```
Connect to the virtual machine either directly (not recommended) or via ssh and run
```bash
./f2
```
**Note that it expects an input, let's say 1**

## Anca
Anca's userland programs located in **anca** directory

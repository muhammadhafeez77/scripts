==== Checking archive integrity ================

(will throw a error if there , otherwise 0 )

xz -t archive.tar.xz
tar -tf archive.tar.xz > /dev/null
tar -dvf archive.tar.xz    ( both file and archive in same folder)

==== making a sha256 for integrity when moving ========

sha256sum archive.tar.xz >> archive.tar.xz.sha256

==== Checking  sha256  ========
sha256sum -c archive.tar.xz.sha256


=== making a system archive ======
(use gentoo gui iso )

**************** LFS ONLY   ****************** 
tar -cpJf /path/to/backup-$(date +%F).tar.xz \
    --exclude=/proc --exclude=/sys --exclude=/dev --exclude=/run \
    --exclude=/tmp --exclude=/mnt --exclude=/sources \
    --one-file-system --numeric-owner --xattrs /

*******************  GENERAL LINUX SYSTEMS (FHS COMPLIANT) **********

tar -cJpf /root/linux-full-backup.tar.xz \
    --exclude=/proc \
    --exclude=/sys \
    --exclude=/dev \
    --exclude=/run \
    --exclude=/tmp \
    --exclude=/lost+found \
    --exclude=/mnt \
    --one-file-system \  /mnt/linux

==== VS CODE ===== 

put folder vscode and vscode-shared in ~/(with . to keep hidden) 
and the other in ~/.config/Code

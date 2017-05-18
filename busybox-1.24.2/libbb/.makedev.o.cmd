cmd_libbb/makedev.o := arm-linux-androideabi-gcc -Wp,-MD,libbb/.makedev.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.24.2)" -DBB_BT=AUTOCONF_TIMESTAMP  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wunused-function -Wunused-value -Wmissing-prototypes -Wmissing-declarations -Wno-format-security -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-builtin-printf -Os  --sysroot=/home/wenyu/server/localdisk/msm722"/prebuilts/ndk/9/platforms/android-18/arch-arm"    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(makedev)"  -D"KBUILD_MODNAME=KBUILD_STR(makedev)" -c -o libbb/makedev.o libbb/makedev.c

deps_libbb/makedev.o := \
  libbb/makedev.c \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config/big/endian.h) \
    $(wildcard include/config/little/endian.h) \
    $(wildcard include/config/nommu.h) \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include-fixed/limits.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include-fixed/syslimits.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/limits.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/cdefs.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/cdefs_elf.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/android/api-level.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/limits.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/limits.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/machine/internal_types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/machine/limits.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/syslimits.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm/page.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/byteswap.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/endian.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/endian.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/machine/endian.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include-fixed/sys/types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include/stddef.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include/stdint.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/stdint.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/_types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/machine/_types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/posix_types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/stddef.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/compiler.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm/posix_types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm/types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/types.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/machine/kernel.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/sysmacros.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include/stdbool.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/features.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/unistd.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/select.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/time.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/time.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/signal.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/string.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/malloc.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm/signal.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm-generic/signal.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm/sigcontext.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm/siginfo.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm-generic/siginfo.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/sysconf.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/capability.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/pathconf.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include-fixed/stdio.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/include/stdarg.h \

libbb/makedev.o: $(deps_libbb/makedev.o)

$(deps_libbb/makedev.o):

cmd_libbb/ptr_to_globals.o := arm-linux-androideabi-gcc -Wp,-MD,libbb/.ptr_to_globals.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.24.2)" -DBB_BT=AUTOCONF_TIMESTAMP  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wunused-function -Wunused-value -Wmissing-prototypes -Wmissing-declarations -Wno-format-security -Wdeclaration-after-statement -Wold-style-definition -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-builtin-printf -Os  --sysroot=/home/wenyu/server/localdisk/msm722"/prebuilts/ndk/9/platforms/android-18/arch-arm"    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(ptr_to_globals)"  -D"KBUILD_MODNAME=KBUILD_STR(ptr_to_globals)" -c -o libbb/ptr_to_globals.o libbb/ptr_to_globals.c

deps_libbb/ptr_to_globals.o := \
  libbb/ptr_to_globals.c \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/errno.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/cdefs.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/sys/cdefs_elf.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/android/api-level.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/linux/errno.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm/errno.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm-generic/errno.h \
  /home/wenyu/server/localdisk/msm722/prebuilts/ndk/9/platforms/android-18/arch-arm/usr/include/asm-generic/errno-base.h \

libbb/ptr_to_globals.o: $(deps_libbb/ptr_to_globals.o)

$(deps_libbb/ptr_to_globals.o):

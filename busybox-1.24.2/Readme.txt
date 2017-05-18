1.先去高通目录
	source build/envsetup.sh   
	lunch msm8909-userdebug

2. 进入busybox目录，执行下面命令：
	make distclean
	make ARCH=arm-linux-androideabi- android_msm8909_defconfig
	make ARCH=arm-linux-androideabi- 

3. 手工配置busybox
	make ARCH=arm-linux-androideabi- menuconfig

	make ARCH=arm-linux-androideabi- 

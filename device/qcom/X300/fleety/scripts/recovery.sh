mkdir -p /cache/recovery
cp -rf /sdcard/update.zip /cache/
sync
sleep 2
echo "--update_package=/cache/update.zip" > /cache/recovery/command
sync
sleep 2
reboot recovery

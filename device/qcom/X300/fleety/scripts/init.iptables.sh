#iptables -nL --line-number		#get iptables rule info
iptables -F						#delete all iptables rule
sleep 1
#busybox lsusb

#busybox ifconfig eth0 192.168.1.1
busybox ifconfig eth0 192.168.0.122

iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -o rmnet_data0 -j MASQUERADE	# setup iptables route rule(rmnet_data0,4G)
iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -o wlan0 -j MASQUERADE   		# setup iptables route rule(wlan0,wifi)

echo 1 > /proc/sys/net/ipv4/ip_forward	#setup kernel ip forward 


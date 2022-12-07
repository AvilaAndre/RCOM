# Experiencia 3

## 1. Transform tuxY4 (Linux) into a router

### – Configure also tuxY4.eth1 and add it to bridgeY1

cables:
switch - tuxY4E1

on Y4 configure eth1

    ifconfig eth1 172.16.Y1.253/24

configure bridge on microtik

    /interface bridge port remove [find interface=ether<PORT4eth1>]
    /interface bridge port add bridge=bridgeY1 interface=ether<PORT4eth1>

### – Enable IP forwarding

on Y4

    echo 1 > /proc/sys/net/ipv4/ip_forward

### – Disable ICMP echo-ignore-broadcast

on Y4

    echo 0 >  /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

## 2. Observe MAC addresses and IP addresses in tuxY4.eth0 and tuxY4.eth1

on Y4

    ifconfig

## 3. Reconfigure tuxY3 and tuxY2 so that each of them can reach the other

on Y2 do

    route add -net 172.16.Y0.0/24 gw 172.16.Y1.253

on Y3 do

    route add -net 172.16.Y0.1/24 gw 172.16.Y0.254

## 4. Observe the routes available at the 3 tuxes (route –n)

on Y2, Y3 and Y4 do

    route -n

## 5. Start capture at tuxY3

start wireshark on Y3

## 6. From tuxY3, ping the other network interfaces (172.16.Y0.254, 172.16.Y1.253, 172.16.Y1.1) and verify if there is connectivity

on Y3 do

    ping 172.16.Y0.254
    ping 172.16.Y1.253
    ping 172.16.Y1.1

## 7. Stop the capture and save the logs

## 8. Start capture in tuxY4; use 2 instances of Wireshark, one per network interface

## 9. Clean the ARP tables in the 3 tuxes

on Y3, Y2 and Y4 do

    arp -a

see the ip address printed

    arp -d <address>

for all addresses

## 10. In tuxY3, ping tuxY2 for a few seconds.

on Y3 do

    ping 172.16.Y1.1/24

## 11. Stop captures in tuxY4 and save logs

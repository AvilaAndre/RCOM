# Experiencia 4

## 1. Connect ether1 of RC to the lab network on PY.1 (with NAT enabled by default) and ether2 of RC to a port on bridgeY1. Configure the ip addresses of RC through the router serial console

cables:
eth1 do router - PY.1
eth2 do router - switch port 4
canto direito - router mtik

change cable to console

on Y3 (the one with the switch microtik) do

    /interface bridge port remove [find interface=ether<PORTqueLigaAoEth2>]
    /interface bridge port add bridge=bridge1 interface = ether<PORTqueLigaAoEth2>

change cable to router mtik again

    /ip route print

para remover

    /ip address remove

    /ip address add address=172.16.Y1.254 interface=ether2

ether2 is the router interface

172.16.2.19/24 is already connected through NAT

on Y3 do

    route add default gw 172.16.Y0.254

on Y2 and Y4 do

    route add default gw 172.16.Y1.254

in tuxY2 and RC add routes for 172.16.Y0.0/24 is done by running

    route add -net 172.16.Y0.0/24 gw 172.16.Y1.253


    /ip route print

    /ip route add dst-address=172.16.Y0.0/24 gateway=172.16.Y1.253

## 3. Using ping commands and Wireshark, verify if tuxY3 can ping all the network interfaces of tuxY2, tuxY4 and RC

    ping 172.16.Y0.254 -- ping ao tux4 na rede 0

    ping 172.16.Y1.253 -- ping ao tux4 na rede 1

    ping 172.16.Y1.1 -- ping ao tux2

    ping 172.16.Y1.254 -- ping ao Rc

## 4. In tuxY2 do the following

    echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects
    echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects

on Y2 do

    route -n

    route del -net 172.16.Y0.0/24 gw 172.16.Y1.253

    route -n

    ping 172.16.Y0.1

do traceroute to Y3

    traceroute 172.16.20.1

add route again

    route add -net 172.16.Y0.0/24 gw 172.16.Y1.253

do traceroute again

    traceroute 172.16.20.1

activate the acceptance of ICMP redirect at tuxY2 when there is no route to
172.16.Y0.0/24 via Y4 by doing

on Y2 do

    echo 1 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

    route del -net 172.16.Y0.0/24 gw 172.16.Y1.253

check if it can ping

    ping 172.16.20.1

on Y2 do

    route add -net 172.16.Y0.0/24 gw 172.16.Y1.253

## 5. In tuxY3, ping the router of the lab I.321 (172.16.2.254) (room 2, 172.16.1.254 in room 1) and try to understand what happens

on Y3 do

    ping 172.16.2.254

## 6. Disable NAT functionality to router RC

on router gtkterm do

    /ip firewall nat disable 0

## 7. In tuxY3 ping 172.16.2.254 (room 2, 172.16.1.254 in room 1), verify if there is connectivity, and try to understand what happens

on Y3 do

    ping 172.16.2.254

    traceroute 172.16.2.254

after everything on Y4 to enable NAT again do:

    /ip firewall nat add chain=srcnat action=masquerade out-interface=ether1

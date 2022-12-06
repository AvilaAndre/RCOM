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

    /ip address add address=172.16.11.254/24 interface=ether2

ether2 is the router interface

172.16.2.19/24 is already connected through NAT

on Y3 do

    route add default gw 172.16.Y0.254

on Y2 do

    route add default gw 172.16.Y0.254

this redirects everything to Y4 (I guess)

in tuxY2 and RC add routes for 172.16.Y0.0/24 is done by running

    route add -net 172.16.Y0.0/24 gw 172.16.Y1.253


    /ip route print

    /ip route add dst-address=172.16.Y0.0/24 gateway=172.16.11.253

## 3. Using ping commands and Wireshark, verify if tuxY3 can ping all the network interfaces of tuxY2, tuxY4 and RC

    ping 172.16.Y0.254 -- ping ao tux4 na rede 0

    ping 172.16.Y1.253 -- ping ao tux4 na rede 1

    ping 172.16.Y1.1 -- ping ao tux2

    ping 172.16.Y1.254 -- ping ao Rc

## 4. In tuxY2 do the following

    echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects
    echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects

on Y2 do

    route del -net 172.16.Y0.0/24 gw 172.16.Y1.253

    route -n

    ping 172.16.Y0.1

meio do slide 14

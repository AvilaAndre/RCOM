# Experiencia 2

## 1. Connect and configure tuxY2 and register its IP and MAC addresses

cables:
switch - tuxY2E0

Tux2

    systemctl restart networking

    ifconfig eth0 172.16.Y1.1/24

    route -n

this command gives one result

    arp -a

this command gives no results (there are no computers on this route)

## 2. Create two bridges in the switch: bridgeY0 and bridgeY1

on Y3 (the one with the microtik configured)

    /interface bridge print

to see what bridges exist

now we add bridgeY0

    /interface bridge port add name=bridgeY0
    /interface bridge port add name=bridgeY1

if we run

    /interface bridge print

now we will see the new bridges

## 3. Remove the ports where tuxY3, tuxY4 and tuxY2 are connected from the default bridge (bridge) and add them the corresponding ports to bridgeY0 and bridgeY1

    /interface bridge port remove [find interface=ether<PORY2eth0>]
    /interface bridge port remove [find interface=ether<PORY3eth0>]
    /interface bridge port remove [find interface=ether<PORY4eth0>]

PORT 1, 2 and 3 are the positions on the switch where we connected the cables

criar bridgeY0

    /interface bridge port add bridge=bridge10 interface=ether<PORY3eth0>
    /interface bridge port add bridge=bridge10 interface=ether<PORY4eth0>

criar bridgeY1

    /interface bridge port add bridge=bridge11 interface=ether<PORY2eth0>

## 4. Start the capture at Y3.eth0

on Y3 start capture on wireshark

## 5. In tuxY3, ping tuxY4 and then ping tuxY2

on Y3 do

    ping 172.16.Y0.254

    ping 172.16.Y1.1

this second command will not work as they are not connected, unreachable.

## 6. Stop the capture and save the log

## 7. Start new captures in tuxY2.eth0, tuxY3.eth0, tuxY4.eth0

## 8. In tuxY3, do ping broadcast (ping –b 172.16.Y0.255) for a few seconds

On Y3 do

    ping –b 172.16.Y0.255

## 9. Observe the results, stop the captures and save the logs

## 10. Repeat steps 7, 8 and 9, but now do ping broadcast in tuxY2 (ping –b 172.16.Y1.255)

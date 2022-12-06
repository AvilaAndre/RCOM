# Experiencia 1

## 1. Disconnect the switch from netlab (PY.1). Connect tuxY3 and tuxY4 to the switch

cables:
2o canto direito - tuxY3S0 --> o pc 3 é o que se usa o microtik
canto direito - consola
switch - tuxY3E0
switch - tuxY4E0

Tux4

    systemctl restart networking

Tux3

    systemctl restart networking

No GTKTerm
-> Configuration > Port > BAUDRATE 115200

    /system reset-configuration
    y
    admin
    [ENTER]

## 2. Configure tuxY3 and tuxY4 using ifconfig and route commands

if

    route -n

has routes do

    ifconfig eth0 down

then we start configuring

on Y3 do

    ifconfig eth0 172.16.Y0.1/24

on Y4 do

    ifconfig eth0 172.16.Y0.254/24

# 3. Register the IP and MAC addresses of network interfaces

on both do

    ifconfig

to see the IP address and the MAC address of each

# 4. Use ping command to verify connectivity between these computers

on Y3 do

    ping 172.16.Y0.254

on Y4 do

    ping 172.16.Y0.1

# 5. Inspect forwarding (route –n) and ARP (arp –a) tables

on both do

    route -n

    arp - a

# 6. Delete ARP table entries in tuxY3 (arp –d ipaddress)

on Y3 do

    arp -a

see the ip address printed

    arp -d <address>

# 7. Start Wireshark in Y3.eth0 and start capturing packets

start wireshark pressing the blue fin

# 8. In tuxY3, ping tuxY4 for a few seconds

    ping 172.16.Y0.254

    arp -a

now has the same results as before

# 9. Stop capturing packets

# 10. Save the log and study it at home

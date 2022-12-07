# Experiencia 5

## 1. Configure DNS at tuxY3, tuxY4, tuxY2 (use DNS server services.netlab.fe.up.pt (172.16.2.1) (172.16.1.1 in room1))

in Y2, Y3 and Y4 do

    nano /etc/resolv.conf

erase and write

    nameserver 172.16.2.1

## 2. Verify if names can be used in these hosts (e.g ping hostname, use browser)

    ping google.com

## 3. Execute ping (new-hostname-in-the-Internet); observe DNS related packets in Wireshark

    ping google.com

and capture with wireshark

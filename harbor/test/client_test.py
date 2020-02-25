import socket

host = "127.0.0.1"
port = 8855
roomid = '111'
uid = '221'
to_msg = 'Hello %s'%(uid)

sends = 'MAGIC\t%s\t%s\t200'%(roomid, uid)
magic_num = 8
address = (host, port)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
msg = "PING"
s.sendto(str.encode(sends), address)

while True:
    iss = s.recv(4096)
    print (iss)
    recv_msg = iss.decode()
    recv_arr = recv_msg.split('\t')

    if recv_arr[0] != 'MAGIC001':
        continue

    msgtype = recv_arr[3]
    status = recv_arr[4]

    if (msgtype == '200'):
        send_201 = 'MAGIC\t%s\t%s\t201'%(roomid, uid)
        s.sendto(str.encode(send_201), address)

    if (msgtype == '201'):
        members = recv_arr[5].split(',')
        for m in members:
            if len(m) == 0 or m == uid:
                continue
            send_202 = 'MAGIC\t%s\t%s\t202\t%s\t%s'%(roomid, uid, m, to_msg)
            s.sendto(str.encode(send_202), address)

s.close()

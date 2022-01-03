import random
import socket
import subprocess
import sys
import threading
import os
import network_helper
from pathlib import Path, PurePath


# How to run
# mount shared 10.0.2.15:500:/Server | mount private 10.0.2.15:500:/Server
# cd 10.0.2.15:500:/Server

class Client_info:
    def __init__(self):
        self.is_mounted = False
        self.is_shared_shell = False
        self.server: Server_info = None
        self.is_client_shell = True
        self.send_shared_cmd = False
        self.display_path = os.getcwd()
        self.is_display_path = True

        self.client_host = socket.gethostbyname(socket.gethostname())
        self.client_port = random.randint(6000, 10000)
        self.sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        self.sock.bind((self.client_host, self.client_port))

        self.event = threading.Event()

        self.cmd = None


class Server_info:
    def __init__(self, host_ip, port, base_path):
        if base_path[0] == '/':
            base_path = base_path[1:]
        self.base_path = base_path
        self.port = int(port)
        self.host_ip = host_ip

    def __eq__(self, other):
        return self.host_ip == other.host_ip and self.port == other.port and self.base_path == other.base_path

    def get_address(self):
        return self.host_ip, self.port


# start client-shell logic

def shared_shell_receive_loop(client: Client_info):
    while True:
        res, _ = client.sock.recvfrom(network_helper.BUFFER_SIZE)
        res = res.decode('utf-8').split(' ')
        rem_path = res[0]
        client.is_cd_cmd = res[1] == 'cd'
        rem_cmd = ' '.join(res[1:])
        old_path, new_path = client.display_path, rem_path
        res, _ = client.sock.recvfrom(network_helper.BUFFER_SIZE)
        cmd_output = res.decode('utf-8')

        if not client.send_shared_cmd:  # received from another client
            print(rem_cmd)
            if len(cmd_output) > 0:
                print()
                print(cmd_output)
            print(f"/{rem_path}$ ", end='')
        else:  # send from this client
            if len(cmd_output) > 0:
                print()
                print(cmd_output)
            print(f"/{rem_path}$ ", end='')
        client.display_path = rem_path  # update path
        client.send_shared_cmd = False
        client.event.set()

def client_mount(client: Client_info):
    cmd_lst = client.cmd.split(' ')
    if cmd_lst[1] == 'private' and len(cmd_lst) == 3:  # mount private host:port:path
        client.server = Server_info(*cmd_lst[2].split(':'))  # host_ip:port:path
        if network_helper.valid_remote_path(client.sock, client.server.get_address(), client.server.base_path) is True:
            client.is_mounted = True
        else:
            client.server = None
            client.is_mounted = False
            print('Invalid remote shell')
        return

    elif cmd_lst[1] == 'shared' and len(cmd_lst) == 3:  # mount shared host:port:path
        client.server = Server_info(*cmd_lst[2].split(':'))  # host_ip:port:path
        client.is_mounted = True
        client.is_shared_shell = True
        return

    else:
        print("Invalid Command - Use 'mount <private/shared> <host:port:path>'")


def client_cd(client: Client_info):
    cmd_lst = client.cmd.split(' ')
    cd_path = cmd_lst[1]
    cd_path_lst = cd_path.split(':')
    if client.is_mounted and len(cd_path_lst) == 3:  # cd host:port:path
        cd_server = Server_info(*cd_path_lst)
        if cd_server == client.server:  # same as mount command
            if client.is_shared_shell:  # remote shared shell
                network_helper.remote_login(client.sock, client.server.get_address())
                client.display_path = network_helper.get_remote_path(client.sock, client.server.get_address())
                client.is_client_shell = False
                threading.Thread(target=shared_shell_receive_loop, args=(client,)).start()
                return
            else:  # private remote shell
                if cd_server == client.server:
                    client.is_client_shell = False  # switch to remote shell
                    client.display_path = client.server.base_path
                return
        else:
            print("Invalid Command - Use 'mount private host:port:path' first to mount remote shell")
    else:  # normal client cd
        try:
            os.chdir(cd_path)
        except OSError:
            print('Error changing directory on client')
        client.display_path = os.getcwd()


def run_client_shell(client: Client_info):
    cmd_lst = client.cmd.split(' ')
    if cmd_lst[0] == 'mount':
        client_mount(client)

    elif cmd_lst[0] == 'cd':  # cd command on client
        if len(cmd_lst) == 2:
            client_cd(client)
        else:
            print('Error changing directory on client')

    else:  # normal client terminal command
        # print(subprocess.run(client.cmd, capture_output=True, text=True, shell=True).stdout)
        print(subprocess.run(client.cmd, stdout=subprocess.PIPE).stdout.decode('utf-8'))
        return


# start remote-shell logic

def is_child_path(parent, child):
    try:
        PurePath(child).relative_to(parent)
        return not Path(child).is_absolute()
    except ValueError:
        return False


def remote_cd(client: Client_info):
    cmd_lst = client.cmd.split(' ')
    cd_path = cmd_lst[1]
    remote_path = os.path.normpath(os.path.join(client.display_path, cd_path))
    if is_child_path(client.server.base_path, remote_path):
        if network_helper.valid_remote_path(client.sock, client.server.get_address(), remote_path):
            client.display_path = remote_path  # Update display path with valid remote path
        else:
            print('Error changing directory on remote shell')
    else:  # cd outside of remote shell
        client.is_client_shell = True
        try:
            os.chdir(cd_path)
        except OSError:
            print('Error changing directory on client')
        client.display_path = os.getcwd()


def remote_cp(client: Client_info):
    cmd_lst = client.cmd.split(' ')
    filename = cmd_lst[1]
    path_local = '.' if cmd_lst[2] == 'cwd' else cmd_lst[2]
    file_path_remote = os.path.normpath(os.path.join(client.display_path, filename))
    network_helper.remote_copy_file(client.sock, client.server.get_address(), file_path_remote, path_local, filename)


def run_remote_shell(client: Client_info):
    if client.is_shared_shell:  # shared remote shell
        client.send_shared_cmd = True
        client.is_display_path = False
        network_helper.run_remote_shared_cmd(client.sock, client.server.get_address(), client.cmd)
        client.event.wait()
        return

    # private remote shell
    cmd_lst = client.cmd.split(' ')
    if len(cmd_lst) == 2 and cmd_lst[0] == 'cd':  # private remote shell cd
        remote_cd(client)

    elif len(cmd_lst) == 3 and cmd_lst[0] == 'cp':  # example: cp a.txt cwd
        remote_cp(client)

    else:  # normal remote shell command
        print(network_helper.run_remote_cmd(client.sock, client.server.get_address(), client.cmd, client.display_path))


if __name__ == '__main__':
    client = Client_info()

    while True:
        if client.is_display_path:
            print(f"/{client.display_path}$ ", end='')
        client.is_display_path = True
        client.event.clear()
        client.cmd = input()

        if client.cmd == 'quit' or client.cmd == 'exit':  # exit condition
            if client.is_shared_shell:
                network_helper.remote_logout(client.sock, client.server.get_address())
            sys.exit()

        if client.is_client_shell:
            run_client_shell(client)

        else:  # remote shell
            run_remote_shell(client)

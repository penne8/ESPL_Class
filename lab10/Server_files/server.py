#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@author: Omri and Pan
"""
import os
import pathlib
import socket
import subprocess
import threading
import queue
import sys
import sqlite3
from sqlite3 import Error

BUFFER_SIZE = 1 << 10
DEFAULT_PORT = 500
DATABASE_NAME = r"remote_shell.db"


class ServerInfo:
    def __init__(self, host, db_file, port):
        self.database = self.create_connection(db_file)
        self.cur = self.database.cursor()
        self.create_tables()
        self.udp_server_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        self.udp_server_socket.bind((host, port))
        self.packet_queue = queue.Queue()

    def create_tables(self):
        self.cur.execute('''CREATE TABLE IF NOT EXISTS users
                (id INTEGER PRIMARY KEY AUTOINCREMENT, host TEXT NOT NULL, port INTEGER)''')
        self.database.commit()

    def create_connection(self, db_file):
        """ create a database connection to the SQLite database
            specified by db_file
        :param db_file: database file
        :return: Connection object or None
        """
        conn = None
        try:
            conn = sqlite3.connect(db_file)
            return conn
        except Error as e:
            print(e)

        return conn


class ClientInfo:
    def __init__(self, server):
        self.data, self.client_address = server.packet_queue.get()
        self.client_host, self.client_port = self.client_address
        self.client_port = int(self.client_port)
        self.data = self.data.decode('utf-8')
        self.data = list(self.data.split(" "))
        self.msg_type = int(self.data[0])


def receive_data(sock, packet_queue):
    while True:
        data, address = sock.recvfrom(BUFFER_SIZE)
        packet_queue.put((data, address))


def is_logged_in(cur, host, port):
    rows = cur.execute("SELECT id FROM users WHERE host = ? AND port=?", (host, port)).fetchall()
    if rows:
        return True
    return False


def remote_login(server: ServerInfo, client: ClientInfo):
    try:
        server.cur.execute("INSERT INTO users(host, port) VALUES(?,?)", (client.client_host, client.client_port))
        server.database.commit()
        status = "1".encode('utf-8')
    except sqlite3.Error:
        status = "0".encode('utf-8')
    server.udp_server_socket.sendto(status, client.client_address)


def remote_logout(server: ServerInfo, client: ClientInfo):
    server.cur.execute("DELETE FROM users WHERE host=? AND port=?", (client.client_host, client.client_port))
    server.database.commit()


def valid_remote_path(server: ServerInfo, client: ClientInfo):
    msg_path = client.data[1]
    parent_dir = pathlib.Path(__file__).parent.resolve()
    local_path = os.path.normpath(os.path.join(parent_dir, msg_path))
    if os.path.isdir(local_path):
        status = "1".encode('utf-8')
    else:
        status = "0".encode('utf-8')
    server.udp_server_socket.sendto(status, client.client_address)


def run_remote_cmd(server: ServerInfo, client: ClientInfo):
    msg_path = client.data[1]
    parent_dir = pathlib.Path(__file__).parent.resolve()
    local_path = os.path.normpath(os.path.join(parent_dir, msg_path))
    msg_cmd = client.data[2:]
    response = subprocess.run(msg_cmd, stdout=subprocess.PIPE, cwd=local_path).stdout
    server.udp_server_socket.sendto(response, client.client_address)


def remote_copy_file(server: ServerInfo, client: ClientInfo):
    remote_file_path = client.data[1]
    parent_dir = pathlib.Path(__file__).parent.resolve()
    local_file_path = os.path.normpath(os.path.join(parent_dir, remote_file_path))

    if os.path.isfile(local_file_path):
        status = '1'.encode('utf-8')
        server.udp_server_socket.sendto(status, client.client_address)
        f = open(local_file_path, "rb")
        client.data = f.read(BUFFER_SIZE)
        while client.data:
            if server.udp_server_socket.sendto(client.data, client.client_address):
                client.data = f.read(BUFFER_SIZE)
    else:
        status = '0'.encode('utf-8')
        server.udp_server_socket.sendto(status, client.client_address)


def get_remote_path(server: ServerInfo, client: ClientInfo):
    parent_dir = pathlib.Path(__file__).parent.resolve()
    rel_path = os.path.relpath(os.getcwd(), start=parent_dir)
    rel_path = rel_path.encode('utf-8')
    server.udp_server_socket.sendto(rel_path, client.client_address)


def run_remote_shared_cmd(server: ServerInfo, client: ClientInfo):
    cmd = ' '.join(client.data[1:])
    cd_cmd = client.data[1] == 'cd'
    if is_logged_in(server.cur, client.client_host, client.client_port):
        if cd_cmd:
            os.chdir(client.data[2])
        parent_dir = pathlib.Path(__file__).parent.resolve()
        rel_path = os.path.relpath(os.getcwd(), start=parent_dir)
        first_msg = f"{rel_path} {cmd}"
        first_msg = first_msg.encode('utf-8')
        if cd_cmd:
            sec_msg = "".encode('utf-8')
        else:
            sec_msg = subprocess.run(cmd.split(' '), stdout=subprocess.PIPE).stdout

        rows = server.cur.execute("SELECT host,port FROM users").fetchall()
        for row in rows:
            client.client_address = (row[0], int(row[1]))
            server.udp_server_socket.sendto(first_msg, client.client_address)
            server.udp_server_socket.sendto(sec_msg, client.client_address)


def run_server(host):
    server = ServerInfo(host, DATABASE_NAME, DEFAULT_PORT)

    print('server Running...')
    os.chdir('Server')  # initial directory

    threading.Thread(target=receive_data, args=(server.udp_server_socket, server.packet_queue)).start()
    while True:
        while not server.packet_queue.empty():
            client = ClientInfo(server)

            if client.msg_type == 0:  # remote_login
                remote_login(server, client)

            if client.msg_type == 1:  # remote_logout
                remote_logout(server, client)

            if client.msg_type == 2:  # valid_remote_path
                valid_remote_path(server, client)

            elif client.msg_type == 3:  # run_remote_cmd
                run_remote_cmd(server, client)

            elif client.msg_type == 4:  # remote_copy_file
                remote_copy_file(server, client)

            elif client.msg_type == 5:  # get_remote_path
                get_remote_path(server, client)

            elif client.msg_type == 6:  # run_remote_shared_cmd
                run_remote_shared_cmd(server, client)

    server.udp_server_socket.close()
    server.database.close()


if __name__ == '__main__':
    server_ip = sys.argv[1]

    if os.path.exists(DATABASE_NAME):
        os.remove(DATABASE_NAME)

    run_server(server_ip)

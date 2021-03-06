﻿#include <iostream>
#include "view_register.h"
#include <string>
#include "view.h"
#include <mysql/mysql.h>
#include <string.h>
#include "redis.h"
#include <sys/socket.h>
#include <errno.h>
using namespace std;

view_register::view_register(void *mpcon, char *ip)
{
	_mpcon = (MYSQL *)mpcon;
    _redis = new Redis;
    _redis->_ip = ip;
}

void view_register::process(Json::Value val, int cli_fd)
{
	_cli_fd = cli_fd;

	MYSQL *mpcon = _mpcon;
	MYSQL_RES *mp_res;
	MYSQL_ROW mp_row;

	//访问user表
	if(mysql_select_db(mpcon, "talk"))
	{
		cerr << "select fail：errno：" << errno << endl;
		return;
	}

	string cmd = "SELECT * FROM user WHERE NAME='';";
	cmd.insert(cmd.size() - 2, val["name"].asString().c_str());
	if(mysql_real_query(mpcon, cmd.c_str(), strlen(cmd.c_str())))
	{
		cerr << "0 query fail;error:" << errno << endl;
		return;
	}

	mp_res = mysql_store_result(mpcon);
	mp_row = mysql_fetch_row(mp_res);
	if(mp_row == 0)
	{
		_flag = true;
		cmd = "INSERT INTO user VALUE('','');";
		cmd.insert(cmd.size() - 6, val["name"].asString().c_str());
		cmd.insert(cmd.size() - 3, val["pw"].asString().c_str());
		if(mysql_real_query(mpcon, cmd.c_str(), strlen(cmd.c_str())))
		{
			cerr << "0 query fail; error:" << errno << endl;
			return;
		}
	}
	else
	{
		_flag = false;
		return;
	}
}

void view_register::responce()
{
	if(_flag)
	{
		char buff[] = "注册成功";
        unsigned len = sizeof(buff)/sizeof(buff[0]);
        send(_cli_fd, (char *)&len, sizeof(unsigned), 0);
		send(_cli_fd, buff, strlen(buff), 0);
	}
	else
	{
		char buff[] = "注册失败";
        unsigned len = sizeof(buff)/sizeof(buff[0]);
        send(_cli_fd, (char *)&len, sizeof(unsigned), 0);
		send(_cli_fd, buff, strlen(buff), 0);
	}
}

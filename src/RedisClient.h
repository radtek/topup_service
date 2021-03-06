/*************************************************************************
	> File Name: RedisClient.h
	> Author: desionwang
	> Mail: wdxin1322@qq.com 
	> Created Time: Tue 18 Mar 2014 10:17:27 AM CST
 ************************************************************************/
#ifndef _REDIS_CLIENT_H
#define _REDIS_CLIENT_H
#include <iostream>
#include "hiredis/hiredis.h"
using namespace std;

class RedisClient{
public:
	RedisClient();
	~RedisClient();
	bool connect(string host, int port);
	bool setex(string key, string value, int seconds);
	bool get(string key, string &value);
	bool select(int db);
	bool delkey(string key);
	bool enqueue(const char* queue, const char* value);
	bool dequeue(const char* queue, string &value);
	int queue_len(const char* queue);
	bool ping();

private:
	redisContext *redis;
	char *cmd;
};

#endif //_REDIS_CLIENT_H

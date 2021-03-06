/*************************************************************************
	> File Name: ConnectionManager.h
	> Author: desionwang
	> Mail: wdxin1322@qq.com 
	> Created Time: Tue 14 Jan 2014 05:30:11 PM CST
 ************************************************************************/

#include<iostream>
#include <occi.h>
#include <stdint.h>
#include "glog/logging.h"

using namespace oracle::occi;
using namespace std;

#ifndef __CONNECTIONMANAGER_H__
#define __CONNECTIONMANAGER_H__

class ConnectionManager{
    public:
        static ConnectionManager * Instance(){
            return &m_connention_manager;
        }

        bool Init(string userName,
			string passWord,
			const string connectString,
			uint32_t maxConnetion,
			uint32_t minConnection,
			uint32_t incrConnection
        );


        Connection *CreateConnection();

		void Recover(Connection *conn);

    private:
        void Distroy(){
            env->terminateConnectionPool(m_connection_pool);
            Environment::terminateEnvironment(env);
        }

		/*
        ConnectionManager(){

        }*/

        //~ConnectionManager{
            //Distroy();	
        //}

    private:
        static ConnectionManager m_connention_manager;
        ConnectionPool *m_connection_pool;
        Environment *env;

	public:
        string m_userName;
        string m_passWord;
};

#endif //__CONNECTIONMANAGER_H__

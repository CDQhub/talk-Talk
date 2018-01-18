#include<iostream>
#include"mpthread.h"
#include<pthread.h>
#include<errno>
using namespace std;

Mpthread::mpthread(int sock_1)
{
	_sock_1 = scok_1;
	
	//启动子线程
    pthread_t id;
    if((pthread_create(&id,NULL,pth_run,this)) != 0)
    {
        cerr<<"pthread_create error:"<<errno<<endl;
        return;
    }

	//->pth_run(this)
}

void cli_cb(int fd,short event,void* arg)
{
	//recv   ->buff
	
	//buff->contral      
}



void sock_1_cb(int fd,short event,void *arg)
{
	//强转
    Pmpthread mthis = (Pmpthread)arg;

	//recv   cli_fd
    char buff[8] = {0};
	recv(fd,buff,sizeof(buff),0);
    int cli_fd = (int)buff;

	//将cli_fd加入libevent  -》cli_cb()
	struct event* ev_cli = event_new(mthis->_base,cli_fd,EV_READ|EV_PERSIST,cli_cb,NULL);
    event_add(ev_cli,NULL);

	//将事件加入到_event_map
    mthis->_event_map.insert(make_pair(cli_fd,ev_cli));
    
	//send(fd,      _event_map.size(),)
    
}


void *pth_run(void *arg)
{
	Pmphread mthis = (Pmpthread)arg;

	//mthis->_base = event_base_new();
	mthis->_base = event_init();
	
	//将sock_1 加入到libevnet  ->sock_1_cb(  ,mthis, )
    struct event* ev_sock_1 = event_new(mthis->_base,mthis->_sock_1,EV_READ|EV_PERSIST,sock_1_cb,mthis);
	event_add(ev_sock_1,NULL);

	event_base_dispatch();
}
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <yangutil/sys/YangSysMessageHandle.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/sys/YangLog.h"

void yang_post_message(int32_t st, int32_t uid, YangSysMessageI *mhandle) {
	if (YangSysMessageHandle::m_instance)
		YangSysMessageHandle::m_instance->putMessage(mhandle, st, uid, 0);
}
void yang_post_state_message(int32_t st, int32_t uid, int32_t handleState,
		YangSysMessageI *mhandle) {
	if (YangSysMessageHandle::m_instance)
		YangSysMessageHandle::m_instance->putMessage(mhandle, st, uid,
				handleState);
}
YangSysMessageHandle::YangSysMessageHandle() {
	m_loop = 0;
	m_isStart = 0;
	m_lock = PTHREAD_MUTEX_INITIALIZER;
	m_cond_mess = PTHREAD_COND_INITIALIZER;
    //pthread_mutex_init(&m_mutex, NULL);
    //pthread_cond_init(&m_cond_mess, NULL);
	m_waitState=0;
	if (m_instance == NULL)
		m_instance = this;
}

YangSysMessageHandle::~YangSysMessageHandle() {

	if (m_isStart) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_instance = NULL;
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_cond_mess);

}
YangSysMessageHandle *YangSysMessageHandle::m_instance = NULL;
void YangSysMessageHandle::run() {
	m_isStart = 1;
    startLoop();
	//startLoop();
	m_isStart = 0;
}
void YangSysMessageHandle::stop() {
	stopLoop();

}
void YangSysMessageHandle::stopLoop() {
	m_loop = 0;
	//pthread_mutex_unlock(&m_lock);
	if(m_waitState){
		pthread_mutex_lock(&m_lock);
		pthread_cond_signal(&m_cond_mess);
		pthread_mutex_unlock(&m_lock);

	}
}

void YangSysMessageHandle::putMessage(YangSysMessageI *handle, int32_t pst,
		int32_t puid, int32_t handleState) {
	if(!m_loop) return;

	YangSysMessage* mes=new YangSysMessage();

	mes->uid = puid;
	mes->messageId = pst;
	mes->handleState = handleState;
	mes->handle = handle;
	m_sysMessages.push_back(mes);
	mes=NULL;

	if(m_waitState){
		pthread_mutex_lock(&m_lock);
        //yang_debug2("*************user(%d) sys message...%d", puid, pst);
		pthread_cond_signal(&m_cond_mess);
		pthread_mutex_unlock(&m_lock);
	}
}

void YangSysMessageHandle::startLoop() {
   m_loop = 1;
     /**int32_t handleCount = 0;
	while (m_loop) {
		handleCount = 0;
		while (m_sysMessages.size()>0) {
			handleMessage(m_sysMessages.front());
			m_sysMessages.front()->handle = NULL;
			delete m_sysMessages.front();
			m_sysMessages.front() = NULL;
			m_sysMessages.erase(m_sysMessages.begin());
			handleCount++;
		}
		if (!handleCount)
			yang_usleep(2000);
		yang_usleep(1000);
    }**/
    pthread_mutex_unlock(&m_lock);
    pthread_mutex_lock(&m_lock);

    while (m_loop) {

        m_waitState=1;
        pthread_cond_wait(&m_cond_mess, &m_lock);
        m_waitState=0;

        while (m_sysMessages.size()>0) {
            handleMessage(m_sysMessages.front());
            m_sysMessages.front()->handle = NULL;
            delete m_sysMessages.front();
            m_sysMessages.front() = NULL;
            m_sysMessages.erase(m_sysMessages.begin());
        }
       // yang_debug2("\n**************wait process end..................");
    }
    pthread_mutex_unlock(&m_lock);
}


#include "webserver.hpp"

webserver::~webserver()
{
    close(m_epollfd);
    close(m_listenfd);
}

void webserver::init(int port)
{
    m_port = port;
}

void webserver::eventListen()
{
    
}

void eventLoop()
{
    register int epoll = 0;
    register int ready = 0;
    register int index = 0;
    
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    
    
}
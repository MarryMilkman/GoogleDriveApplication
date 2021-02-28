#ifndef FILE_WORKER_REQUEST_HPP
#define FILE_WORKER_REQUEST_HPP

struct FileWorkerRequest
{
    enum class Type
    {
        SERVER_REQUEST = 0,
        LIST_FILES,
        PARENT_LIST
    };

};

#endif

#include "Comlib.h"

Comlib::Comlib(LPCWSTR bufferName, size_t bufferSize, ProcessType type)
{   
    this->type = type;
    //Creating our shared memory inside our constructor of the comlib.cpp.
    sharedMemory = new Memory(bufferName, bufferSize);
    messageData = sharedMemory->GetMemoryBuffer();
    mutex = new Mutex(L"MutexMap");

    /*
    When creating each process, we need to separate if it is the producer or the
    consumer we are creating by its type.Last, we set the same address as the file
    mapping has to another char - pointer that we call messageData.Like this, we can
    sendand receive our messages through this pointer.

    We can now create a shared memory to use between two processes with the
    help of arguments. In main() we can insert so that we look specifically for
    “producer” or “consumer” when we run our programs.*/


    head = sharedMemory->GetControlBuffer();
    tail = head + 1;
    freeMemory = tail + 1;

    if (type == Producer)
    {
        std::cout << "Producer activated" << std::endl;
        *head = 0;
        *tail = 0;
        *freeMemory = bufferSize * 1048576;
    }
    else if (type == Producer)
    {
        std::cout << "Consumer activated" << std::endl;
        *tail = 0;
    }

}

Comlib::~Comlib()
{
    delete mutex;
    delete sharedMemory;
}

bool Comlib::Send(char* message, SectionHeader* msgHeader)
{       
    //Protocol Producer
    bool result = false;
    mutex->Lock();
    size_t memoryLeft = sharedMemory->GetBufferSize() - *head;
    if (msgHeader->msgLen + sizeof(MessageHeader) >= memoryLeft)
    {
        if (*tail != 0)
        {
            msgHeader->msgID = 0;
            memcpy(messageData + *head, msgHeader, sizeof(MessageHeader));

            *freeMemory -= (msgHeader->msgLen + sizeof(MessageHeader));
            *head = 0;

            mutex->Unlock();
        }
        else
        {
            mutex->Unlock();
            result = false;
        }
    }
    else if (msgHeader->msgLen + sizeof(MessageHeader) <*freeMemory -1)
    {
        msgHeader->msgID = 1;
        
        //Print stuff
        memcpy(messageData + *head, msgHeader, sizeof(MessageHeader));
        memcpy(messageData + *head + sizeof(MessageHeader), message, msgHeader->msgLen);

        *freeMemory -= (msgHeader->msgLen + sizeof(MessageHeader));
        *head = (*head + msgHeader->msgLen + sizeof(MessageHeader)) % sharedMemory->GetBufferSize();

        mutex->Unlock();
        result = true;
    }
    else
    {
        mutex->Unlock();
        result = false;
    }
    return result;
}

bool Comlib::Recieve(char* message, SectionHeader*& msgHeader)
{   
    //Protocol Consumer
    mutex->Lock();
    bool result = false;
    size_t msgLenght = 0;
    //Checking if the free memory is less than the total amount of memory to know if the Producer has started producing the messages
    if (*freeMemory < sharedMemory->GetBufferSize())
    {
        if (*head != *tail)
        {
            msgHeader = ((SectionHeader*)&messageData[*tail]);
            msgLenght = msgHeader->msgLen;
            if (msgHeader->msgID == 0)
            {
                *freeMemory += (msgLenght + sizeof(SectionHeader));
                *tail = 0;

                mutex->Unlock();
                result = false;
            }
            else
            {
                message = new char[msgLenght];
                memcpy(message, &messageData[*tail + sizeof(SectionHeader)], msgLenght);
                *tail = (*tail + msgLenght + sizeof(SectionHeader)) % sharedMemory->GetBufferSize();
                *freeMemory +=(msgLenght + sizeof(SectionHeader));

                mutex->Unlock();
                result = true;
            }
        }
        else
        {
            mutex->Unlock();
            result = false;
        }
    }
    else
    {
        mutex->Unlock();
        result = false;
    }
    return result;
}
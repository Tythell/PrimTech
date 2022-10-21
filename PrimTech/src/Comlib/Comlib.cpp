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
    delete messageData;
}

bool Comlib::Send(char* message, SectionHeader* secHeader)
{       
    //Protocol Producer
    bool result = false;
    mutex->Lock();
    size_t memoryLeft = sharedMemory->GetBufferSize() - *head;
    if (secHeader->messageLength + sizeof(SectionHeader) >= memoryLeft)
    {
        if (*tail != 0)
        {
            secHeader->messageID = 0;
            memcpy(messageData + *head, secHeader, sizeof(SectionHeader));

            *freeMemory -= (secHeader->messageLength + sizeof(SectionHeader));
            *head = 0;

            mutex->Unlock();
        }
        else
        {
            mutex->Unlock();
            result = false;
        }
    }
    else if (secHeader->messageLength + sizeof(SectionHeader) <*freeMemory -1)
    {
        secHeader->messageID = 1;
        
        //Print stuff
        memcpy(messageData + *head, secHeader, sizeof(SectionHeader));
        memcpy(messageData + *head + sizeof(SectionHeader), message, secHeader->messageLength);

        *freeMemory -= (secHeader->messageLength + sizeof(SectionHeader));
        *head = (*head + secHeader->messageLength + sizeof(SectionHeader)) % sharedMemory->GetBufferSize();

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

bool Comlib::Recieve(char* message, SectionHeader*& secHeader)
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
            secHeader = ((SectionHeader*)&messageData[*tail]);
            msgLenght = secHeader->messageLength;
            if (secHeader->messageID == 0)
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
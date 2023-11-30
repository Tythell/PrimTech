#include "Comlib.h"

namespace comlib
{

    RingBuffer::RingBuffer(LPCWSTR bufferName, size_t bufferSize, ProcessType type)
    {
        m_type = type;
        //Creating our shared memory inside our constructor of the comlib.cpp.
        m_sharedMemory = new Memory(bufferName, bufferSize);
        m_messageData = m_sharedMemory->GetMemoryBuffer();

        std::wstring mutexName = L"MutexMap";
        mutexName.append(bufferName);

        m_mutex = new Mutex(mutexName.c_str());

        /*
        When creating each process, we need to separate if it is the producer or the
        consumer we are creating by its type.Last, we set the same address as the file
        mapping has to another char - pointer that we call messageData.Like this, we can
        sendand receive our messages through this pointer.

        We can now create a shared memory to use between two processes with the
        help of arguments. In main() we can insert so that we look specifically for
        “producer” or “consumer” when we run our programs.*/


        m_head = m_sharedMemory->GetControlBuffer();
        m_tail = m_head + 1;
        m_freeMemory = m_tail + 1;

        if (type == Producer)
        {
            //std::cout << "Producer activated" << std::endl;
            *m_head = 0;
            *m_tail = 0;
            *m_freeMemory = bufferSize * 1048576;
        }
        else if (type == Producer)
        {
            //std::cout << "Consumer activated" << std::endl;
            *m_tail = 0;
        }

    }

    RingBuffer::~RingBuffer()
    {
        delete m_mutex;
        delete m_sharedMemory;
    }

    bool RingBuffer::Send(char* message, SectionHeader* msgHeader)
    {
        //Protocol Producer
        bool result = false;
        m_mutex->Lock();
        size_t memoryLeft = m_sharedMemory->GetBufferSize() - *m_head;
        if (msgHeader->msgLen + sizeof(SectionHeader) >= memoryLeft)
        {
            if (*m_tail != 0)
            {
                msgHeader->msgID = 0;
                memcpy(m_messageData + *m_head, msgHeader, sizeof(SectionHeader));

                *m_freeMemory -= (msgHeader->msgLen + sizeof(SectionHeader));
                *m_head = 0;

                m_mutex->Unlock();
            }
            else
            {
                m_mutex->Unlock();
                result = false;
            }
        }
        else if (msgHeader->msgLen + sizeof(SectionHeader) < *m_freeMemory - 1)
        {
            msgHeader->msgID = 1;

            //Print stuff
            memcpy(m_messageData + *m_head, msgHeader, sizeof(SectionHeader));
            memcpy(m_messageData + *m_head + sizeof(SectionHeader), message, msgHeader->msgLen);

            //memcpy(linus, m_messageData + *m_head + )

            //std::cout << linus;

            *m_freeMemory -= (msgHeader->msgLen + sizeof(SectionHeader));
            *m_head = (*m_head + msgHeader->msgLen + sizeof(SectionHeader)) % m_sharedMemory->GetBufferSize();

            m_mutex->Unlock();
            result = true;
        }
        else
        {
            m_mutex->Unlock();
            result = false;
        }
        return result;
    }

    bool RingBuffer::Recieve(char*& message, SectionHeader*& msgHeader)
    {
        //Protocol Consumer
        m_mutex->Lock();
        bool result = false;
        size_t msgLenght = 0;
        //Checking if the free memory is less than the total amount of memory to know if the Producer has started producing the messages

        size_t intTest = *m_head;
        size_t intTest2 = *m_tail;
        size_t buffSize = m_sharedMemory->GetBufferSize();

        if (*m_freeMemory < m_sharedMemory->GetBufferSize())
        {
            if (*m_head != *m_tail)
            {
                msgHeader = ((SectionHeader*)&m_messageData[*m_tail]);
                msgLenght = msgHeader->msgLen;
                if (msgHeader->msgID == 0)
                {
                    *m_freeMemory += (msgLenght + sizeof(SectionHeader));
                    *m_tail = 0;

                    m_mutex->Unlock();
                    result = false;
                }
                else
                {
                    message = new char[msgLenght];
                    memcpy(message, &m_messageData[*m_tail + sizeof(SectionHeader)], msgLenght);

                    *m_tail = (*m_tail + msgLenght + sizeof(SectionHeader)) % m_sharedMemory->GetBufferSize();
                    *m_freeMemory += (msgLenght + sizeof(SectionHeader));

                    m_mutex->Unlock();
                    result = true;
                }
            }
            else
            {
                m_mutex->Unlock();
                result = false;
            }
        }
        else
        {
            m_mutex->Unlock();
            result = false;
        }
        return result;
    }

}
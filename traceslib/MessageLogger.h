#ifndef _MESSAGE_LOGGER_H_
#define _MESSAGE_LOGGER_H_

#include "CompositeStream.h"

class MessageLogger
{
    private:
        static MessageLogger& instance();
        MessageLogger();
        ~MessageLogger() {}
        std::string getHomePath();

    public:
#if defined(DEBUG) || defined(_DEBUG)
        static CompositeStream& debug();
        static void debug(const char* msg);
#endif
        static CompositeStream& info();
        static CompositeStream& warning();
        static CompositeStream& error();

        static void info(const char* msg);
        static void warning(const char* msg);
        static void error(const char* msg);

    private:
#if defined(DEBUG) || defined(_DEBUG)
        CompositeStream& debugStream();
#endif
        CompositeStream& infoStream();
        CompositeStream& warningStream();
        CompositeStream& errorStream();

        std::ofstream m_logFile;
#if defined(DEBUG) || defined(_DEBUG)
        std::ofstream m_coutFile;
        std::ofstream m_cerrFile;
#endif
};


#if defined(DEBUG) || defined(_DEBUG)
    #define debug(x) MessageLogger::debug(x)
#else
    #define debug(x) if(false) std::cout << ""
#endif
#define info(x) MessageLogger::info(x)
#define warning(x) MessageLogger::warning(x)
#define error(x) MessageLogger::error(x)


#endif // _MESSAGE_LOGGER_H_

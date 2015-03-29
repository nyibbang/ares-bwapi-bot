#ifndef _MESSAGE_LOGGER_H_
#define _MESSAGE_LOGGER_H_


#include <fstream>

enum FILE_TYPE { LOG, COUT, CERR };

class MessageLogger
{
    private:
        static MessageLogger& instance();
        MessageLogger();
        ~MessageLogger();
        std::string getHomePath();

    public:
        static void debug(const char* msg = 0);
        static void info(const char* msg = 0);
        static void warning(const char* msg);
        static void error(const char* msg);

    private:
        void write(FILE_TYPE type, const char* msg);
        void write(std::ofstream& os, const char* msg);
        void closeIfOpen(std::ofstream& os);

#if defined(DEBUG) || defined(_DEBUG)
        std::ofstream m_coutFile;
        std::ofstream m_cerrFile;
#endif
        std::ofstream m_logFile;

};


#if defined(DEBUG) || defined(_DEBUG)
    #define debug(x) MessageLogger::debug(x)
#else
    #define debug(x) (void)0
#endif
#define info(x) MessageLogger::info(x)
#define warning(x) MessageLogger::warning(x)
#define error(x) MessageLogger::error(x)


#endif // _MESSAGE_LOGGER_H_

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(__GNUC__) || defined(__MINGW32__)
#   define ON_WINDOWS
#endif

#include <iostream>

#ifdef ON_WINDOWS

#include <fstream>
#include <memory>
#include <functional>
#include <vector>
#include <sstream>

namespace traces
{

class AbstractLogger
{
    public:
        virtual ~AbstractLogger() {}
        virtual void log(const std::string& message) = 0;
        virtual void layout(const char* file, int line, const char* level) = 0;
};

class StreamLogger final : public AbstractLogger
{
    public:
        StreamLogger(std::ostream& os);
        void layout(const char* file, int line, const char* level);

    private:
        void log(const std::string& message) override;

        std::ostream& m_os;
        std::string m_location;
        std::string m_level;
};

class CompositeLogger final : public AbstractLogger
{
    public:
        CompositeLogger(std::unique_ptr<AbstractLogger>& coutLogger, std::ostream& os);
        void layout(const char* file, int line, const char* level);

    private:
        void log(const std::string& message) override;

        std::unique_ptr<AbstractLogger>& m_coutLogger;
        std::ostream& m_os;
        std::string m_location;
        std::string m_level;
};

class Stream final
{
    public:
        typedef std::ostream& (&stream_manipulator)(std::ostream&);

        Stream(AbstractLogger& logger);
        ~Stream();
        Stream& layout(const char* file, int line, const char* level);

        template<class T> Stream& operator<<(T&& t);
        Stream& operator<<(stream_manipulator manip);

    private:
        AbstractLogger& m_logger;
        std::ostringstream m_buffer;
};

class Facade final
{
    public:

#ifdef DEBUG
        static Stream& debug(const char* file, int line);
#endif
        static Stream& info(const char* file, int line);
        static Stream& warning(const char* file, int line);
        static Stream& error(const char* file, int line);

        static void resetAuxiliaryLogger();
        template<class TAuxiliaryLogger, class... TArgs>
        static void initializeAuxiliaryLogger(TArgs&&... args);

    private:
        Facade();
        static Facade& instance();

        std::unique_ptr<AbstractLogger> m_coutLogger;
        std::ofstream m_fileStream;
        StreamLogger m_noInfoLogger;
        CompositeLogger m_infoLogger;
#ifdef DEBUG
        Stream m_debugStream;
#endif
        Stream m_infoStream;
        Stream m_warnStream;
        Stream m_errorStream;
};

}


template<class T> traces::Stream& traces::Stream::operator<<(T&& t)
{
    m_buffer << std::forward<T>(t);
    return *this;
}

template<class TAuxiliaryLogger, class... TArgs>
void traces::Facade::initializeAuxiliaryLogger(TArgs&&... args)
{
    instance().m_coutLogger.reset(new TAuxiliaryLogger(std::forward<TArgs>(args)...));
}

#ifdef DEBUG
    #define ARES_DEBUG()   traces::Facade::debug(__FILE__, __LINE__)
#else
    #define ARES_DEBUG() if(false) std::cout
#endif
#define ARES_INFO()    traces::Facade::info(__FILE__, __LINE__)
#define ARES_WARNING() traces::Facade::warning(__FILE__, __LINE__)
#define ARES_ERROR()   traces::Facade::error(__FILE__, __LINE__)

#else // NOT ON WINDOWS
    #define ARES_DEBUG   ARES_ERROR
    #define ARES_INFO    ARES_ERROR
    #define ARES_WARNING ARES_ERROR
    #define ARES_ERROR() if(false) std::cout
#endif

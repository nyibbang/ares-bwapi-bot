#ifndef _COMPOSITE_STREAM_H_
#define _COMPOSITE_STREAM_H_

#include <fstream>
#include <vector>
#include <algorithm>

class CompositeStream : public std::ostream
{
    public:
        CompositeStream(std::vector<std::streambuf*> compositeBuffers) : std::ostream(NULL)
        {
            std::ostream::rdbuf(&m_compositeBuffer);
            for(std::streambuf* sb : compositeBuffers)
                m_compositeBuffer.addBuffer(sb);
        }

    struct CompositeBuffer : public std::streambuf
    {
        void addBuffer(std::streambuf* buf)
        {
            m_compositeBuffers.push_back(buf);
        }
        virtual int overflow(int c) override
        {
            for(std::streambuf* sb : m_compositeBuffers)
                sb->sputc(c);
            return c;
        }

        private:
            std::vector<std::streambuf*> m_compositeBuffers;

    };
    CompositeBuffer m_compositeBuffer;
};

#endif // _COMPOSITE_STREAM_H_

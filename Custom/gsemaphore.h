#ifndef GSEMAPHORE_H
#define GSEMAPHORE_H

#include <QSemaphore>

namespace GUtil
{
    namespace Custom
    {
        class GSemaphore : QSemaphore
        {
        public:
            explicit GSemaphore(int init_val = 0);

            void Up(int n = 1);
            void Down(int n = 1);

            bool IsEmpty();

        };
    }
}

#endif // GSEMAPHORE_H

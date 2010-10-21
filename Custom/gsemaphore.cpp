#include "gsemaphore.h"
#include "Core/exception.h"
using namespace GUtil;

Custom::GSemaphore::GSemaphore(int init_val)
    :QSemaphore(init_val)
{
}

void Custom::GSemaphore::Up(int n)
{
    release(n);
}

void Custom::GSemaphore::Down(int n)
{
    acquire(n);
}

bool Custom::GSemaphore::IsEmpty()
{
    return available() == 0;
}

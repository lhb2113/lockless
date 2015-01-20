#include "object.h"

OPEN_LOCKLESS_NAMESPACE

std::atomic<int> g_object_count(0);

object::object()
{
    g_object_count++;
}

object::~object()
{
    g_object_count--;
}

CLOSE_LOCKLESS_NAMESPACE

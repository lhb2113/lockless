#ifndef CHANNEL_PIPELINE_INITIALIZER_H
#define CHANNEL_PIPELINE_INITIALIZER_H

#include "defines.h"
#include "object.h"

OPEN_LOCKLESS_NAMESPACE

class channel_pipeline;

class channel_pipeline_initializer : public object
{
public:
    virtual void init_channel(std::shared_ptr<channel_pipeline> _pipeline) = 0;
};

class channel_pipeline_initializer_factory : public object
{
public:
    virtual std::shared_ptr<channel_pipeline_initializer> new_initializer() const = 0;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // CHANNEL_PIPELINE_INITIALIZER_H

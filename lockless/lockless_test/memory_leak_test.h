//
//  memory_leak_test.h
//  lockless
//
//  Created by liuhaibo on 14-12-22.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef MEMORY_LEAK_TEST_H
#define MEMORY_LEAK_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <lockless/lockless.h>

class backend_initializer : public channel_pipeline_initializer
{
public:
    backend_initializer(std::shared_ptr<channel_pipeline_context> _context):m_context(_context) {}
    void init_channel(std::shared_ptr<channel_pipeline> _pipeline);
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class backend_initializer_factory : public channel_pipeline_initializer_factory
{
public:
    backend_initializer_factory(std::shared_ptr<channel_pipeline_context> _context):m_context(_context) {}
    std::shared_ptr<channel_pipeline_initializer> new_initializer() const;
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class frontend_initializer : public channel_pipeline_initializer
{
public:
    void init_channel(std::shared_ptr<channel_pipeline> _pipeline);
};

class frontend_initializer_factory : public channel_pipeline_initializer_factory
{
public:
    std::shared_ptr<channel_pipeline_initializer> new_initializer() const;
};

class client_initializer : public channel_pipeline_initializer
{
public:
    client_initializer(std::shared_ptr<channel_pipeline_context> _context):m_context(_context) {}
    void init_channel(std::shared_ptr<channel_pipeline> _pipeline);
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class client_initializer_factory : public channel_pipeline_initializer_factory
{
public:
    client_initializer_factory(std::shared_ptr<channel_pipeline_context> _context):m_context(_context) {}
    std::shared_ptr<channel_pipeline_initializer> new_initializer() const;
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class client_channel_pipeline : public channel_pipeline
{
public:
    
    client_channel_pipeline(std::shared_ptr<runtime_context> _context)
    : channel_pipeline(_context)
    {
        
    }
    void set_name(const std::string &_name)
    {
        channel_pipeline::set_name(_name);
    }
};

class memory_leak_test : public testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
protected:
    std::shared_ptr<channel_pipeline_context> m_pipeline_context;
    std::shared_ptr<server_bootstrap> m_server_bootstrap;
//    std::shared_ptr<client_bootstrap> m_client_bootstrap;
};

#endif //MEMORY_LEAK_TEST_H

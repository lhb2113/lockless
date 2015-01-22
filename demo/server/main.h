//
//  main.h
//  server
//
//  Created by liuhaibo on 15-1-22.
//  Copyright (c) 2015年 liuhaibo. All rights reserved.
//

#ifndef server_main_h
#define server_main_h

class server_backend_pipeline_initializer : public channel_pipeline_initializer
{
public:
    server_backend_pipeline_initializer(std::shared_ptr<channel_pipeline_context> _context);
public:
    void init_channel(std::shared_ptr<channel_pipeline> _pipeline);
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class server_backend_pipeline_initializer_factory : public channel_pipeline_initializer_factory
{
public:
    server_backend_pipeline_initializer_factory(std::shared_ptr<channel_pipeline_context> _context);
public:
    std::shared_ptr<channel_pipeline_initializer> new_initializer() const;
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class server_frontend_pipeline_initializer : public channel_pipeline_initializer
{
public:
    server_frontend_pipeline_initializer(std::shared_ptr<channel_pipeline_context> _context);
private:
    void init_channel(std::shared_ptr<channel_pipeline> _pipeline);
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class server_frontend_pipeline_initializer_factory : public channel_pipeline_initializer_factory
{
public:
    server_frontend_pipeline_initializer_factory(std::shared_ptr<channel_pipeline_context> _context);
private:
    std::shared_ptr<channel_pipeline_initializer> new_initializer() const;
private:
    std::shared_ptr<channel_pipeline_context> m_context;
};

class server_frontend_logger_handler : public channel_handler
{
public:
    server_frontend_logger_handler();
private:
    void handle_read_any_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
};

#endif

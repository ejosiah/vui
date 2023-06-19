#pragma once

#include <string>
#include <functional>

namespace vui {

    using DrawUI = std::function<void()>;

    struct config{
        unsigned int width;
        unsigned int height;
        std::string title;
    };

    void show(const config& config, DrawUI&& drawUi);

    void stop();

    void wait();

    bool isRunning();

}
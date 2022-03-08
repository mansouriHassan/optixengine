/*
 * Copyright (c) 2013-2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "shaders/config.h"

#include "inc/Application.h"
#include "inc/Socket.h"
#include "inc/ConfigParser.h"

#include <IL/il.h>

#include <algorithm>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds


static Application* g_app = nullptr;
static Socket* socket_server = nullptr;
static ConfigParser* config_parser = nullptr;

static void callbackError(int error, const char* description)
{
    std::cerr << "Error: " << error << ": " << description << '\n';
}

void start_server()
{
    if (socket_server != NULL)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        socket_server->socket_start();
    }
}

void change_view(int second)
{
    while(true) {
        if (config_parser != NULL && config_parser->isConfigFinished)
        {
            int len = config_parser->camera_views.size();
            if (len > 0) {
                for(int index = 0; index < len; index++) {
                    std::this_thread::sleep_for(std::chrono::seconds(second));
                    config_parser->view_index = index;
                    config_parser->isCamreaChanged = true;
                }
            }
        }
    }
}

void get_config_data()
{
    while(true) {
        if (socket_server != NULL)
        {
            if (socket_server->isClientConnected()) {
                std::string const json_data = socket_server->socket_read();
                if(config_parser != NULL && !json_data.empty())
                {
                    config_parser->parseConfigData(json_data);
                }
            }
        }
    }
}

void send_image(int second)
{
    while (true)
    {
        if (g_app != NULL)
        {
            do {
                if (socket_server->isClientConnected()) {
                    g_app->sendImage(true);
                    std::this_thread::sleep_for(std::chrono::seconds(second));
                }
            } while (!g_app->looping);
        }
    }
}

static int runApp(Options const& options)
{
    int width = std::max(1, options.getWidth());
    int height = std::max(1, options.getHeight());

    auto monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window = glfwCreateWindow(width, height, "optix_hair - Copyright (c) 2020 NVIDIA Corporation", nullptr, NULL);
    // const GLFWvidmode* wmode = glfwGetVideoMode(monitor);
     //auto scr_width = wmode->width;
     //auto scr_height = wmode->height;
     //glfwSetWindowSize(window, scr_width, scr_height);

    if (!window)
    {
        callbackError(APP_ERROR_CREATE_WINDOW, "glfwCreateWindow() failed.");
        glfwTerminate();
        return APP_ERROR_CREATE_WINDOW;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GL_NO_ERROR)
    {
        callbackError(APP_ERROR_GLEW_INIT, "GLEW failed to initialize.");
        glfwTerminate();
        return APP_ERROR_GLEW_INIT;
    }

    ilInit(); // Initialize DevIL once.

    g_app = new Application(window, options);

    if (!g_app->isValid())
    {
        std::cerr << "ERROR: Application() failed to initialize successfully.\n";
        ilShutDown();
        glfwTerminate();
        return APP_ERROR_APP_INIT;
    }

    const int mode = std::max(0, options.getMode());

    if (mode == 0) // Interactive, default.
    {
        // Main loop
        bool finish = false;
        while (!finish && !glfwWindowShouldClose(window))
        {

            glfwPollEvents(); // Render continuously. Battery drainer!



            glfwGetFramebufferSize(window, &width, &height);

            g_app->reshape(width, height);
            g_app->guiNewFrame();
            //g_app->guiReferenceManual();  // HACK The ImGUI "Programming Manual" as example code.
            //g_app->guiWindow();             // This application's GUI window rendering commands.
            //g_app->guiUserWindow();           // This application's GUI window renderind commands for user expert color.
            g_app->customGuiUserWindow();
            //g_app->guiEventHandler();       // SPACE to toggle the GUI windows and all mouse tracking via GuiState.
            finish = g_app->render();       // OptiX rendering, returns true when benchmark is enabled and the samples per pixel have been rendered.
            g_app->display();               // OpenGL display always required to lay the background for the GUI.
            g_app->guiRender();             // Render all ImGUI elements at last.

            glfwSwapBuffers(window);

            //glfwWaitEvents(); // Render only when an event is happening. Needs some glfwPostEmptyEvent() to prevent GUI lagging one frame behind when ending an action.
        }
    }
    else if (mode == 1) // Batched benchmark single shot. // FIXME When not using anything OpenGL, the whole window and OpenGL setup could be removed.
    {
        g_app->benchmark();
    }

    delete g_app;

    ilShutDown();

    return APP_EXIT_SUCCESS;
}


int main(int argc, char* argv[])
{
    socket_server = Socket::getInstance();
    config_parser = ConfigParser::getInstance();
    std::thread thread_server(&start_server);   // start server
    std::thread thread_config(&get_config_data);   // start server
    std::thread thread_send_image(&send_image, 1);
    std::thread thread_change_view(&change_view, 5);

    glfwSetErrorCallback(callbackError);


    if (!glfwInit())
    {
        callbackError(APP_ERROR_GLFW_INIT, "GLFW failed to initialize.");
        return APP_ERROR_GLFW_INIT;
    }

    int result = APP_ERROR_UNKNOWN;

    Options options;

    if (options.parseCommandLine(argc, argv))
    {
        if (options.getHeight() == 0 || options.getWidth() == 0)
        {
            auto monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* wmode = glfwGetVideoMode(monitor);
            options.setWidth(wmode->width);
            options.setHeight(wmode->height);
            //const GLFWvidmode* wmode = glfwGetVideoMode(monitor);
            //auto scr_width = wmode->width;
            //auto scr_height = wmode->height;
            //glfwSetWindowSize(window, scr_width, scr_height);
        }
        result = runApp(options);
    }

    return result;
}

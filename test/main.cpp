#include <vui/vui.h>
#include <imgui.h>
#include <implot.h>
#include <thread>
#include <array>
#include <chrono>
#include <numeric>
#include <algorithm>


int main(int, char** ){

    vui::config config{1280, 720, "Dear ImGui GLFW+Vulkan example"};

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    bool show_graph = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    static constexpr int N = 1000;
    static constexpr float TWO_PI = 6.283185307179586476925286766559;
    std::array<float, N> x_data{};
    std::array<float, N> y_data{};


    for(int i = 0; i < N; i++){
        auto x = static_cast<float>(i)/static_cast<float>(N) - 0.5f;
        x_data[i] = x;
        y_data[i] = std::cos(2 * x * TWO_PI);
    }

    vui::show(config, [&]{
        auto& io = ImGui::GetIO();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        if(show_graph){
            ImGui::Begin("Graph");
            ImGui::SetWindowSize({500, 350});
            if(ImPlot::BeginPlot("Cosine wave")){
                ImPlot::PlotLine("Cosine", x_data.data(), y_data.data(), N);
                ImPlot::EndPlot();
            }
            ImGui::End();
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    vui::stop();

    return 0;
}
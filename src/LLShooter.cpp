#include <cstring>
#include <stdexcept>

#include "utils/utils.hpp"
#include "objects/DrawableObject.hpp"
#include "LLShooter.hpp"

void LLShooter::run_game() {
    init_window();
    init_game();
    main_loop();
}

void LLShooter::init_window() {
    int glfw_init_res = glfwInit();
    if (!glfw_init_res) {
        throw std::runtime_error(
            std::string("Failed to initialize GLFW. Error code: ") + std::to_string(glfw_init_res)
        );
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LLShooter", nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("Failed to create window.");

    glfwMakeContextCurrent(window);

    int glew_init_res = glewInit();
    if (glew_init_res != GLEW_OK) {
        throw std::runtime_error(
            std::string("Failed to initialize GLEW. Error code: " + std::to_string(glew_init_res))
        );
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void LLShooter::init_game() {
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Init camera.
    camera = std::make_unique<ControllableCamera>(
        glm::vec3(0.0f, 0.0f, 4.0f), glm::radians(70.0f),
        WINDOW_WIDTH, WINDOW_HEIGHT, window
    );
    camera->mouse_sensivity = 0.25f;
    camera->forward_speed = 20.0f;
    camera->strafe_speed = 10.0f;
    camera->backward_speed = 10.0f;

    // Init objects.
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    utils::load_wavefront_obj("Bibizana.obj", vertices, uvs, normals);
    DrawableObject* triangle = new DrawableObject();
    triangle->vertex_buf = vertices;
    triangle->init_vertex_buf();
    drawable_objects.push_back(triangle);
}

void LLShooter::main_loop() {
    GLuint program_id = utils::load_shaders("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    mvp_matrix_id = glGetUniformLocation(program_id, "mvp");
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();

    do {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        float delta = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();

        camera->update(delta);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program_id);
        glEnableVertexAttribArray(0);

        auto camera_mvp = camera->compute_mvp_matrix();

        for (int i = 0; i < drawable_objects.size(); i++) {
            glBindBuffer(GL_ARRAY_BUFFER, drawable_objects[i]->vertex_buf_id);
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                0
            );

            glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, &camera_mvp[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, drawable_objects[i]->vertex_buf.size());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        glDisableVertexAttribArray(0);
    } while (!glfwWindowShouldClose(window));
}

LLShooter::~LLShooter() {
    for (DrawableObject* obj : drawable_objects)
        delete obj;
}

#include <cstring>
#include <stdexcept>

#include "utils/utils.hpp"
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
    // Create the VAO.
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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
    utils::load_wavefront_obj("/home/artem/Desktop/Bibizana.obj", vertices, uvs, normals);
    MonochromeDrawableObject* triangle = new MonochromeDrawableObject(glm::vec3(0.0f, 1.0f, 0.0f));
    triangle->vertex_buf = vertices;
    triangle->normals_buf = normals;
    triangle->init_vertex_buf();
    triangle->init_normals_buf();
    monochrome_objects.push_back(triangle);
}

void LLShooter::main_loop() {
    // Init the monochrome shaders.
    GLuint monochrome_program_id = utils::load_shaders(
        "res/shaders/vertex.glsl",
        "res/shaders/monochrome_fragment.glsl"
    );
    GLuint mc_mvp_matrix_id = glGetUniformLocation(monochrome_program_id, "MVP");
    GLuint mc_color_id = glGetUniformLocation(monochrome_program_id, "COLOR");

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();

    do {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        float delta = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();

        camera->update(delta);
        auto camera_mvp = camera->compute_mvp_matrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // BEGIN draw monochrome objects.
        glUseProgram(monochrome_program_id);

        for (int i = 0; i < monochrome_objects.size(); i++) {
            // Vertices.
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, monochrome_objects[i]->vertex_buf_id);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

            // Normals.
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, monochrome_objects[i]->normals_buf_id);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glUniformMatrix4fv(mc_mvp_matrix_id, 1, GL_FALSE, &camera_mvp[0][0]);
            glUniform3fv(mc_color_id, 1, &monochrome_objects[i]->color[0]);
            glDrawArrays(GL_TRIANGLES, 0, monochrome_objects[i]->vertex_buf.size());

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();


    } while (!glfwWindowShouldClose(window));
}

LLShooter::~LLShooter() {
    for (DrawableObject* obj : monochrome_objects)
        delete obj;
}

#include <cstring>
#include <stdexcept>

#include "utils/utils.hpp"
#include "RenderingServer.hpp"

RenderingServer::RenderingServer(int window_width, int window_height) {
    init_window(window_width, window_height);
    init_gl();
    init_shaders();
}

void RenderingServer::set_update_callback(std::function<void(float)> func) {
    update_callback = func;
}

GLFWwindow* RenderingServer::get_window() {
    return window;
}

void RenderingServer::init_window(int window_width, int window_height) {
    int glfw_init_res = glfwInit();
    if (!glfw_init_res) {
        throw std::runtime_error(
            std::string("Failed to initialize GLFW. Error code: ") + std::to_string(glfw_init_res)
        );
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(window_width, window_height, "LLShooter", nullptr, nullptr);
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

void RenderingServer::init_gl() {
    // Create the VAO.
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable backface culling.
    glEnable(GL_CULL_FACE);

    // Init objects.
    GLuint texture_id = utils::load_dds("/home/artem/Desktop/map_close.dds");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    utils::load_wavefront_obj("/home/artem/Desktop/map_close.obj", vertices, uvs, normals);
    TexturedDrawableObject* triangle = new TexturedDrawableObject(texture_id);
    triangle->vertex_buf = vertices;
    triangle->uvs_buf = uvs;
    triangle->normals_buf = normals;
    triangle->init_vertex_buf();
    triangle->init_uvs_buf();
    triangle->init_normals_buf();
    triangle->rotation = glm::quat({0.0f, 0.0f, 0.0f});
    triangle->translation = glm::vec3(0.0f, 0.0f, 0.0f);
    triangle->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    textured_objects.push_back(triangle);
}

void RenderingServer::init_shaders() {
    // Init the monochrome shaders.
    monochrome_program_id = utils::load_shaders(
        "res/shaders/monochrome_vertex.glsl",
        "res/shaders/monochrome_fragment.glsl"
    );
    mc_mvp_matrix_id = glGetUniformLocation(monochrome_program_id, "MVP");
    mc_color_id = glGetUniformLocation(monochrome_program_id, "COLOR");
    mc_object_matrix_id = glGetUniformLocation(monochrome_program_id, "OBJECT_MATRIX");
    mc_camera_matrix_id = glGetUniformLocation(monochrome_program_id, "CAMERA_MATRIX");
    mc_light_direction_id = glGetUniformLocation(monochrome_program_id, "LIGHT_DIRECTION");

    // Init the textured shaders.
    textured_program_id = utils::load_shaders(
        "res/shaders/textured_vertex.glsl",
        "res/shaders/textured_fragment.glsl"
    );
    tx_mvp_matrix_id = glGetUniformLocation(textured_program_id, "MVP");
    tx_object_matrix_id = glGetUniformLocation(textured_program_id, "OBJECT_MATRIX");
    tx_camera_matrix_id = glGetUniformLocation(textured_program_id, "CAMERA_MATRIX");
    tx_light_direction_id = glGetUniformLocation(textured_program_id, "LIGHT_DIRECTION");
}

void RenderingServer::main_loop() {
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();
    do {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        float delta = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();

        update_callback(delta);
        glm::mat4 view_matrix;
        auto camera_mvp = camera->compute_mvp_matrix(view_matrix);

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

            // Uniforms.
            glUniformMatrix4fv(mc_mvp_matrix_id, 1, GL_FALSE, &camera_mvp[0][0]);
            auto obj_matrix = monochrome_objects[i]->compute_matrix();
            glUniformMatrix4fv(mc_object_matrix_id, 1, GL_FALSE, &obj_matrix[0][0]);
            glUniformMatrix4fv(mc_camera_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);
            glUniform3fv(mc_color_id, 1, &monochrome_objects[i]->color[0]);
            glUniform3fv(mc_light_direction_id, 1, &light_direction[0]);
            glDrawArrays(GL_TRIANGLES, 0, monochrome_objects[i]->vertex_buf.size());

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }
        // END draw monochrome objects.

        // BEGIN draw textured objects.
        glUseProgram(textured_program_id);
        for (int i = 0; i < textured_objects.size(); i++) {
            // Vertices.
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, textured_objects[i]->vertex_buf_id);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

            // UVs.
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, textured_objects[i]->uvs_buf_id);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

            // Normals.
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, textured_objects[i]->normals_buf_id);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

            // Bind textures.
            glBindTexture(GL_TEXTURE_2D, textured_objects[i]->texture_id);

            // Uniforms.
            glUniformMatrix4fv(tx_mvp_matrix_id, 1, GL_FALSE, &camera_mvp[0][0]);
            auto obj_matrix = textured_objects[i]->compute_matrix();
            glUniformMatrix4fv(tx_object_matrix_id, 1, GL_FALSE, &obj_matrix[0][0]);
            glUniformMatrix4fv(tx_camera_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);
            glUniform3fv(tx_light_direction_id, 1, &light_direction[0]);

            glDrawArrays(GL_TRIANGLES, 0, textured_objects[i]->vertex_buf.size());

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }
        // END draw textured objects.

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));
}

RenderingServer::~RenderingServer() {
    for (DrawableObject* obj : monochrome_objects)
        delete obj;
    for (DrawableObject* obj : textured_objects)
        delete obj;
}

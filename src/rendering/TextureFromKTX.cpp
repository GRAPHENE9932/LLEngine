#include "rendering/Texture.hpp"

#include <ktx.h>
#include <fmt/format.h>

#include <fstream>
#include <filesystem>

using namespace llengine;

class KTXTextureWrapper {
public:
    KTXTextureWrapper() = default;
    KTXTextureWrapper(ktxTexture* ptr) : ptr(ptr) {}
    KTXTextureWrapper(const KTXTextureWrapper& other) = delete;
    KTXTextureWrapper(KTXTextureWrapper&& other) noexcept : KTXTextureWrapper(other.get()) {
        other.ptr = nullptr;
    }
    KTXTextureWrapper& operator=(const KTXTextureWrapper& other) = delete;
    ~KTXTextureWrapper() {
        if (ptr) {
            ktxTexture_Destroy(ptr);
        }
    }

    [[nodiscard]] inline ktxTexture*& get() noexcept {
        return ptr;
    }

private:
    ktxTexture* ptr = nullptr;
};

class FILEWrapper {
public:
    FILEWrapper() : ptr(nullptr) {}
    FILEWrapper(std::FILE* ptr) : ptr(ptr) {}
    FILEWrapper(const std::string& file_name, const std::string& mode) {
        init(file_name, mode);
    }
    FILEWrapper(const FILEWrapper& other) = delete;
    FILEWrapper(FILEWrapper&& other) noexcept : FILEWrapper(other.get()) {
        other.ptr = nullptr;
    }
    FILEWrapper& operator=(const FILEWrapper& other) = delete;
    ~FILEWrapper() {
        if (ptr) {
            fclose(ptr);
        }
    }

    void init(const std::string& file_name, const std::string& mode) {
        ptr = std::fopen(file_name.c_str(), mode.c_str());
    }

    [[nodiscard]] inline std::FILE* get() const noexcept {
        return ptr;
    }

    [[nodiscard]] inline operator bool() const noexcept {
        return ptr;
    }
    [[nodiscard]] inline bool operator!() const noexcept {
        return !ptr;
    }

private:
    std::FILE* ptr;
};

std::string ktx_error_to_string(KTX_error_code error) {
    switch (error) {
    case KTX_SUCCESS:
        return "KTX_SUCCESS";
    case KTX_FILE_DATA_ERROR:
        return "KTX_FILE_DATA_ERROR";
    case KTX_FILE_ISPIPE:
        return "KTX_FILE_ISPIPE";
    case KTX_FILE_OPEN_FAILED:
        return "KTX_FILE_OPEN_FAILED";
    case KTX_FILE_OVERFLOW:
        return "KTX_FILE_OVERFLOW";
    case KTX_FILE_READ_ERROR:
        return "KTX_FILE_READ_ERROR";
    case KTX_FILE_SEEK_ERROR:
        return "KTX_FILE_SEEK_ERROR";
    case KTX_FILE_UNEXPECTED_EOF:
        return "KTX_FILE_UNEXPECTED_EOF";
    case KTX_FILE_WRITE_ERROR:
        return "KTX_FILE_WRITE_ERROR";
    case KTX_GL_ERROR:
        return "KTX_GL_ERROR";
    case KTX_INVALID_OPERATION:
        return "KTX_INVALID_OPERATION";
    case KTX_INVALID_VALUE:
        return "KTX_INVALID_VALUE";
    case KTX_NOT_FOUND:
        return "KTX_NOT_FOUND";
    case KTX_OUT_OF_MEMORY:
        return "KTX_OUT_OF_MEMORY";
    case KTX_TRANSCODE_FAILED:
        return "KTX_TRANSCODE_FAILED";
    case KTX_UNKNOWN_FILE_FORMAT:
        return "KTX_UNKNOWN_FILE_FORMAT";
    case KTX_UNSUPPORTED_TEXTURE_TYPE:
        return "KTX_UNSUPPORTED_TEXTURE_TYPE";
    case KTX_UNSUPPORTED_FEATURE:
        return "KTX_UNSUPPORTED_FEATURE";
    case KTX_LIBRARY_NOT_LINKED:
        return "KTX_LIBRARY_NOT_LINKED";
    default:
        return "Unknown KTX error";
    }
}

[[nodiscard]] std::unique_ptr<std::uint8_t[]> read_to_memory(const TexLoadingParams& params, std::streamsize& size_out) {
    if (params.size == 0) {
        size_out = std::filesystem::file_size(params.file_path) - params.offset;
    }
    else {
        size_out = params.size;
    }
    
    std::ifstream stream(params.file_path, std::ios::in | std::ios::binary);
    if (!stream) {
        throw TextureLoadingError(fmt::format(
            "Failed to open file with a texture. File name: \"{}\", offset: {}, length: {}",
            params.file_path, params.offset, params.size
        ));
    }

    stream.seekg(params.offset);
    std::unique_ptr<std::uint8_t[]> data {std::make_unique<std::uint8_t[]>(size_out)};
    stream.read(reinterpret_cast<char*>(data.get()), size_out);

    if (!stream) {
        throw TextureLoadingError(fmt::format(
            "Failed to read file region with a texture. File name: \"{}\", offset: {}, length: {}",
            params.file_path, params.offset, params.size
        ));
    }

    return data;
}

Texture Texture::from_ktx(const TexLoadingParams& params) {
    KTXTextureWrapper ktx_texture;
    KTX_error_code error;

    std::streamsize size {0};
    auto data = read_to_memory(params, size);

    error = ktxTexture_CreateFromMemory(
        data.get(),
        size,
        KTX_TEXTURE_CREATE_NO_FLAGS,
        &ktx_texture.get()
    );

    if (error != KTX_SUCCESS) {
        throw TextureLoadingError(fmt::format(
            "Failed to load KTX from file \"{}\". Error code: {}",
            params.file_path, ktx_error_to_string(error)
        ));
    }

    // Transcode the texture if needed.
    if (ktx_texture.get()->classId == ktxTexture2_c && ktxTexture2_NeedsTranscoding(reinterpret_cast<ktxTexture2*>(ktx_texture.get()))) {
        error = ktxTexture2_TranscodeBasis(reinterpret_cast<ktxTexture2*>(ktx_texture.get()), KTX_TTF_BC3_RGBA, 0);

        if (error != KTX_SUCCESS) {
            throw TextureLoadingError(fmt::format(
                "Failed to transcode KTX from file \"{}\". Error code: {}",
                params.file_path, ktx_error_to_string(error)
            ));
        }
    }

    // Generate the texture.
    GLuint texture_id = 0;
    GLenum tex_target = 0, gl_error = 0;
    error = ktxTexture_GLUpload(ktx_texture.get(), &texture_id, &tex_target, &gl_error);
    
    if (error != KTX_SUCCESS) {
        throw TextureLoadingError(fmt::format(
            "Failed to upload KTX texture to OpenGL. libktx error code: {}. OpenGL error code: {}",
            ktx_error_to_string(error), gl_error
        ));
    }
    
    Texture texture {
        texture_id,
        glm::u32vec2(ktx_texture.get()->baseWidth, ktx_texture.get()->baseHeight),
        ktx_texture.get()->isCubemap
    };
    glBindTexture(tex_target, texture_id);
    glTexParameteri(tex_target, GL_TEXTURE_MAG_FILTER, params.magnification_filter);
    glTexParameteri(tex_target, GL_TEXTURE_MIN_FILTER, params.minification_filter);
    glTexParameteri(tex_target, GL_TEXTURE_WRAP_S, params.wrap_s);
    glTexParameteri(tex_target, GL_TEXTURE_WRAP_T, params.wrap_t);
    if (ktx_texture.get()->isCubemap) {
        glTexParameteri(tex_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    return texture;
}
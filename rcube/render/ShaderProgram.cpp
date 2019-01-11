#include "ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace rcube {

std::string getStringFromFile(const std::string &filename) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        throw std::runtime_error("Unable to open shader source file: " + filename);
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

ShaderProgram::ShaderProgram() : id_(0), warn_(false) {
}

ShaderProgram::~ShaderProgram() {
    release();
}

std::shared_ptr<ShaderProgram> ShaderProgram::create(const std::string &vertex_shader,
                                                     const std::string &fragment_shader,
                                                     bool debug) {
    auto prog = std::make_shared<ShaderProgram>();
    prog->addShader(GL_VERTEX_SHADER, vertex_shader, debug);
    prog->addShader(GL_FRAGMENT_SHADER, fragment_shader, debug);
    prog->link();
    return prog;
}

std::shared_ptr<ShaderProgram> ShaderProgram::create(const std::string &vertex_shader,
                                                     const std::string &geometry_shader,
                                                     const std::string &fragment_shader,
                                                     bool debug) {
    auto prog = std::make_shared<ShaderProgram>();
    prog->addShader(GL_VERTEX_SHADER, vertex_shader, debug);
    prog->addShader(GL_GEOMETRY_SHADER, geometry_shader, debug);
    prog->addShader(GL_FRAGMENT_SHADER, fragment_shader, debug);
    prog->link();
    return prog;
}

std::shared_ptr<ShaderProgram> ShaderProgram::createFromFile(const std::string &vertex_shader,
                                                             const std::string &fragment_shader,
                                                             bool debug) {

}
std::shared_ptr<ShaderProgram> ShaderProgram::createFromFile(const std::string &vertex_shader,
                                                             const std::string &geometry_shader,
                                                             const std::string &fragment_shader,
                                                             bool debug) {

}

void ShaderProgram::release() {
    if (id_ != 0) {
        glDeleteProgram(id_);
        id_ = 0;
    }
}

// Draw the data represented by the bound VAO with glDrawArrays
// Assumes that use() has been already called.
void ShaderProgram::drawArrays(GLint mode, uint32_t first, uint32_t count) const {
    // Assume that a VAO is already bound
    if (count == 0) {
        return;
    }
    glDrawArrays(mode, (GLint)first, (GLsizei) count);
}

// Draw the data represented by the bound VAO with glDrawElements
// Assumes that use() has been already called.
// Remember to call done() after drawing.
void ShaderProgram::drawElements(GLint mode, uint32_t first, uint32_t count) const {
    // Assume that a VAO is already bound
    if (count == 0) {
        return;
    }
    glDrawElements(mode, (GLsizei) count, GL_UNSIGNED_INT, (void *)(first * sizeof(uint32_t)));
}

// Links the shader program and if successful detaches all associated shaders
bool ShaderProgram::link(bool debug) {
    glLinkProgram(id_);
    GLint success = 0;
    glGetProgramiv(id_, GL_LINK_STATUS, (int *)&success);
    if (!success) {
        if (debug) {
            // Print out the error log in case of unsuccessful linking
            char log[512];
            glGetProgramInfoLog(id_, 512, nullptr, log);
            std::cout<< "Linking error in shader program:\n" << log << std::endl;
        }
        id_ = 0;
        return false;
    }
    // Detach shaders after successful linking
    for (GLuint shader : shaders_) {
        glDetachShader(id_, shader);
    }
    return true;
}

// Use the shader (glUseProgram(my id))
void ShaderProgram::use() const {
    glUseProgram(id_);
}

// Done with the shader (glUseProgram(0))
void ShaderProgram::done() const {
    glUseProgram(0);
}

// Get the location of the given attribute
GLint ShaderProgram::attributeLocation(const std::string &name) const {
    GLint id = glGetAttribLocation(id_, name.c_str());
    return id;
}

// Get the location of the given uniform
GLint ShaderProgram::uniformLocation(const std::string &name) const {
    return glGetUniformLocation(id_, name.c_str());
}

void ShaderProgram::setUniform(const std::string &name, float val) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform1f(loc, val);
    }
    else {
        std::cout<< "Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::vec2 &vec) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform2f(loc, vec.x, vec.y);
    }
    else {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::showWarnings(bool flag) {
    warn_ = flag;
}

void ShaderProgram::setUniform(const std::string &name, const glm::vec3 &vec) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform3f(loc, vec.x, vec.y, vec.z);
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::vec4 &vec) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, int val) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform1i(loc, val);
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, bool val) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform1i(loc, val);
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::ivec2 &vec) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform2i(loc, vec.x, vec.y);
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::ivec3 &vec) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform3i(loc, vec.x, vec.y, vec.z);
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::ivec4 &vec) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniform4i(loc, vec[0], vec[1], vec[2], vec[3]);
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::mat2 &mat) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::mat3 &mat) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::setUniform(const std::string &name, const glm::mat4 &mat) {
    GLint loc = uniformLocation(name);
    if (loc != -1) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        return;
    }
    if (warn_) {
        std::cout << "WARNING: Unable to find uniform " << name << std::endl;
    }
}

void ShaderProgram::addShader(GLuint type, const std::string &source, bool debug) {
    // Create a new program if not already done
    if (id_ == 0) {
        id_ = glCreateProgram();
    }
    GLuint shader = glCreateShader(type);
    const char* c_str = source.c_str();
    GLint length = source.size();
    glShaderSource(shader, 1, &c_str, &length);
    // Try to compile the shader
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // Shader compilation failed; print the log if debug==true
        if (debug) {
            char log[512];
            int log_size;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::string str_type;
            if (type == GL_VERTEX_SHADER) {
                str_type = "vertex";
            }
            else if (type == GL_FRAGMENT_SHADER) {
                str_type = "fragment";
            }
            else if (type == GL_GEOMETRY_SHADER) {
                str_type = "geometry";
            }
            std::cerr << "Compilation error in " << str_type << " shader:\n" << log << std::endl;
        }
        throw std::runtime_error("Unable to compile shader");
    }
    // Shader compilation successful; attach the shader to the program
    glAttachShader(id_, shader);
    shaders_.push_back(shader);
}

void ShaderProgram::addShaderFromFile(GLuint type, const std::string &filename, bool debug) {
    std::string src = getStringFromFile(filename);
    addShader(type, src, debug);
}

} // namespace rcube

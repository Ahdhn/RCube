#pragma once

#include "RCube/Core/Graphics/OpenGL/GLDataType.h"
#include "RCube/Core/Graphics/OpenGL/Texture.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_integer.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rcube
{

struct ShaderAttributeDesc
{
    ShaderAttributeDesc() = default;
    ShaderAttributeDesc(std::string attribute_name, GLDataType data_type, int array_count = 1)
        : name(attribute_name), type(data_type), count(array_count)
    {
    }
    std::string name;
    GLDataType type;
    int count;
};

struct ShaderUniformDesc
{
    /*const*/ std::string name;
    /*const*/ GLDataType type;
};

struct ShaderTextureDesc
{
    const std::string name;
    const int dim;
};

struct ShaderCubemapDesc
{
    const std::string name;
};

struct VertexShader
{
    VertexShader() = default;
    VertexShader(const std::vector<ShaderAttributeDesc> attributes, const std::string source)
        : attributes(attributes), source(source)
    {
    }
    VertexShader(const std::vector<ShaderAttributeDesc> attributes,
                 const std::vector<ShaderUniformDesc> uniforms, const std::string source)
        : attributes(attributes), uniforms(uniforms), source(source)
    {
    }
    const std::vector<ShaderAttributeDesc> attributes; // List of attributes expected by the shader
    const std::vector<ShaderUniformDesc> uniforms;     // List of uniforms defined by the shader
    const std::string source;                          // Source code
};

struct GeometryShader
{
    GeometryShader() = default;
    GeometryShader(const std::vector<ShaderAttributeDesc> attributes, const std::string source)
        : attributes(attributes), source(source)
    {
    }
    GeometryShader(const std::vector<ShaderAttributeDesc> attributes,
                   const std::vector<ShaderUniformDesc> uniforms, const std::string source)
        : attributes(attributes), uniforms(uniforms), source(source)
    {
    }
    const std::vector<ShaderAttributeDesc> attributes; // List of attributes expected by the shader
    const std::vector<ShaderUniformDesc> uniforms;     // List of uniforms defined by the shader
    const std::string source;                          // Source code
};

struct FragmentShader
{
    FragmentShader() = default;
    FragmentShader(const std::vector<ShaderUniformDesc> uniforms, std::string output_name,
                   std::string source)
        : uniforms(uniforms), output_name(output_name), source(source)
    {
    }
    FragmentShader(const std::vector<ShaderUniformDesc> uniforms,
                   const std::vector<ShaderTextureDesc> textures, std::string output_name,
                   std::string source)
        : uniforms(uniforms), textures(textures), output_name(output_name), source(source)
    {
    }
    FragmentShader(const std::vector<ShaderUniformDesc> uniforms,
                   const std::vector<ShaderTextureDesc> textures,
                   const std::vector<ShaderCubemapDesc> cubemaps, std::string output_name,
                   std::string source)
        : uniforms(uniforms), textures(textures), cubemaps(cubemaps), output_name(output_name),
          source(source)
    {
    }
    const std::vector<ShaderUniformDesc> uniforms; // List of uniforms defined by the shader
    const std::vector<ShaderTextureDesc> textures; // List of textures defined by the shader
    const std::vector<ShaderCubemapDesc> cubemaps; // List of cubemap textures defined by the shader
    std::string output_name;                       // Name of the output variable
    std::string source;                            // Source code
};

class Uniform
{
    std::string name_;
    GLDataType type_;
    int location_;
    GLuint program_id_;

  public:
    Uniform() = default;
    Uniform(std::string name, GLDataType type, int program_id)
        : name_(name), type_(type), program_id_(program_id)
    {
        location_ = glGetUniformLocation(program_id_, name_.c_str());
    }
    std::string name() const
    {
        return name_;
    }
    GLDataType type() const
    {
        return type_;
    }
    void get(bool &val);
    void get(unsigned int &val);
    void get(int &val);
    void get(float &val);
    void get(glm::vec2 &val);
    void get(glm::vec3 &val);
    void get(glm::vec4 &val);
    void get(glm::ivec2 &val);
    void get(glm::ivec3 &val);
    void get(glm::ivec4 &val);
    void set(bool val);
    void set(int val);
    void set(unsigned int val);
    void set(float val);
    void set(glm::mat2 val);
    void set(glm::mat3 val);
    void set(glm::mat4 val);
    void set(glm::vec2 val);
    void set(glm::vec3 val);
    void set(glm::vec4 val);
    void set(glm::ivec2 val);
    void set(glm::ivec3 val);
    void set(glm::ivec4 val);
};

enum RenderPriority
{
    Opaque = 0,
    Background = 10,
    Transparent = 20,
    Overlay = 30
};

enum class BlendFunc
{
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    Zero = GL_ZERO,
    One = GL_ONE,
    SrcColor = GL_SRC_COLOR,
    OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
    DstColor = GL_DST_COLOR,
    OneMinusDstColor = GL_ONE_MINUS_DST_COLOR
};

enum class Cull
{
    Back = GL_BACK,
    Front = GL_FRONT,
    Both = GL_FRONT_AND_BACK
};

enum class DepthFunc
{
    Less = GL_LESS,   /// Passes if the incoming depth value is less than the stored depth value.
    Equal = GL_EQUAL, /// Passes if the incoming depth value is equal to the stored depth value.
    LessOrEqual = GL_LEQUAL, /// Passes if the incoming depth value is less than or equal to the
                             /// stored depth value.
    Greater =
        GL_GREATER, /// Passes if the incoming depth value is greater than the stored depth value.
    NotEqual =
        GL_NOTEQUAL, /// Passes if the incoming depth value is not equal to the stored depth value.
    GreaterOrEqual = GL_GEQUAL, /// Passes if the incoming depth value is greater than or equal to
                                /// the stored depth value.
    Never = GL_NEVER,           /// Never passes
    Always = GL_ALWAYS,         /// Always passes
};

enum class StencilFunc
{
    Less = GL_LESS,   /// Passes if the incoming depth value is less than the stored depth value.
    Equal = GL_EQUAL, /// Passes if the incoming depth value is equal to the stored depth value.
    LessOrEqual = GL_LEQUAL, /// Passes if the incoming depth value is less than or equal to the
                             /// stored depth value.
    Greater =
        GL_GREATER, /// Passes if the incoming depth value is greater than the stored depth value.
    NotEqual =
        GL_NOTEQUAL, /// Passes if the incoming depth value is not equal to the stored depth value.
    GreaterOrEqual = GL_GEQUAL, /// Passes if the incoming depth value is greater than or equal to
                                /// the stored depth value.
    Never = GL_NEVER,           /// Never passes
    Always = GL_ALWAYS,         /// Always passes
};

enum class StencilOp
{
    Keep = GL_KEEP,
    Zero = GL_ZERO,
    Replace = GL_REPLACE,
    Incr = GL_INCR,
    IncrWrap = GL_INCR_WRAP,
    Decr = GL_DECR,
    DecrWrap = GL_DECR_WRAP,
    Invert = GL_INVERT,
};

struct RenderSettings
{
    struct Culling
    {
        bool enabled = false;
        Cull mode = Cull::Back;
    };

    struct Depth
    {
        bool test = true;
        bool write = true;
        DepthFunc func = DepthFunc::Less;
        double near = 0.0;
        double far = 1.0;
    };

    struct Stencil
    {
        bool test = false;
        GLuint write = 0xFF;
        StencilFunc func = StencilFunc::Always;
        StencilOp op_stencil_fail = StencilOp::Keep, op_depth_fail = StencilOp::Keep,
                  op_pass = StencilOp::Keep;
        GLint func_ref = 0;
        GLint func_mask = 1;
    };

    struct Blend
    {
        bool enabled = false;
        BlendFunc func_src = BlendFunc::One, func_dst = BlendFunc::Zero;
    };

    Culling cull;
    Depth depth;
    Stencil stencil;
    Blend blend;
    bool dither = false;
};

class ShaderProgram
{
  private:
    GLuint location_;
    std::vector<GLint> shaders_;
    bool warn_ = true;
    std::unordered_map<std::string, ShaderAttributeDesc> attributes_;
    std::unordered_map<std::string, Uniform> uniforms_;

  public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram &other) = delete;
    ~ShaderProgram();
    void release();
    static std::shared_ptr<ShaderProgram> create(const std::string &vertex_shader,
                                                 const std::string &fragment_shader,
                                                 bool debug = false);
    static std::shared_ptr<ShaderProgram> create(const std::string &vertex_shader,
                                                 const std::string &geometry_shader,
                                                 const std::string &fragment_shader,
                                                 bool debug = false);
    const std::unordered_map<std::string, ShaderAttributeDesc> &attributes() const;
    const Uniform &uniform(std::string name) const;
    Uniform &uniform(std::string name);
    bool hasUniform(std::string name, Uniform &uni);
    bool link(bool debug = false);
    GLuint id() const;
    void use() const;
    void done() const;
    GLint attributeLocation(const std::string &name) const;
    GLint uniformLocation(const std::string &name) const;
    void showWarnings(bool flag);

  private:
    void addShader(GLuint type, const std::string &source, bool debug = false);
    void addShaderFromFile(GLuint type, const std::string &filename, bool debug = false);
    void generateAttributes();
    void generateUniforms();
};

} // namespace rcube

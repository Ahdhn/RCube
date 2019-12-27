#include "RCube/Core/Graphics/OpenGL/Framebuffer.h"
#include <iostream>
#include <stdexcept>
using std::cout;
using std::endl;
namespace rcube
{

const std::string ERROR_FRAMEBUFFER_UNINITIALIZED = "Cannot use Framebuffer without initializing";
const std::string ERROR_FRAMEBUFFER_NOT_COMPLETE = "Cannot use Framebuffer that is incomplete";

Framebuffer::Framebuffer()
{
    has_depth_stencil_ = false;
    colors_.reserve(8);
}

GLuint Framebuffer::id() const
{
    return id_;
}

size_t Framebuffer::width() const
{
    return width_;
}

size_t Framebuffer::height() const
{
    return height_;
}

std::shared_ptr<Framebuffer> Framebuffer::create(size_t width, size_t height)
{
    auto fbo = std::make_shared<Framebuffer>();
    glGenFramebuffers(1, &fbo->id_);
    fbo->width_ = width;
    fbo->height_ = height;
    fbo->has_depth_stencil_ = false;
    return fbo;
}

Framebuffer::~Framebuffer()
{
    release();
}

bool Framebuffer::initialized() const
{
    return id_ > 0;
}

void Framebuffer::release()
{
    glDeleteFramebuffers(1, &id_);
    clearColorAttachments();
    if (hasDepthStencilAttachment())
    {
        depth_stencil_->release();
    }
    id_ = 0;
}

void Framebuffer::use()
{
    if (!initialized())
    {
        throw std::runtime_error(ERROR_FRAMEBUFFER_UNINITIALIZED);
    }
    if (!isComplete())
    {
        throw std::runtime_error(ERROR_FRAMEBUFFER_NOT_COMPLETE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

void Framebuffer::done() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Framebuffer::isComplete() const
{
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::addColorAttachment(TextureInternalFormat internal_format, size_t levels,
                                     size_t samples)
{
    use();
    std::shared_ptr<Texture2D> tex;
    levels = std::max(static_cast<size_t>(1), levels);
    if (samples == 0)
    {
        tex = Texture2D::create(width_, height_, levels, internal_format);
    }
    else
    {
        tex = Texture2D::createMS(width_, height_, samples, internal_format);
    }
    colors_.push_back(tex);
    unsigned int index = static_cast<unsigned int>(colors_.size() - 1);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, tex->target(),
                           colors_[colors_.size() - 1]->id(), 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
    done();
}

void Framebuffer::clearColorAttachments()
{
    for (auto &tex : colors_)
    {
        tex->release();
    }
}

void Framebuffer::resize(size_t width, size_t height)
{
    if (width_ == width && height_ == height)
    {
        return;
    }
    width_ = width;
    height_ = height;
    for (size_t i = 0; i < colors_.size(); ++i)
    {
        auto new_tex =
            Texture2D::create(width, height, colors_[i]->levels(), colors_[i]->internalFormat());
        colors_[i]->release();
        colors_[i] = new_tex;
    }
    if (has_depth_stencil_)
    {
        auto new_tex = Texture2D::create(width, height, 1 /*mipmap is always 1*/,
                                         depth_stencil_->internalFormat());
        depth_stencil_->release();
        depth_stencil_ = new_tex;
    }
}

void Framebuffer::addDepthAttachment(TextureInternalFormat internal_format, size_t samples)
{
    use();
    assert(internal_format == TextureInternalFormat::Depth16 ||
           internal_format == TextureInternalFormat::Depth24 ||
           internal_format == TextureInternalFormat::Depth32 ||
           internal_format == TextureInternalFormat::Depth32F ||
           internal_format == TextureInternalFormat::Depth24Stencil8 ||
           internal_format == TextureInternalFormat::Depth32FStencil8);

    if (samples == 0)
    {
        depth_stencil_ = Texture2D::create(width_, height_, 1, internal_format);
    }
    else
    {
        depth_stencil_ = Texture2D::createMS(width_, height_, samples, internal_format);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_->target(),
                           depth_stencil_->id(), 0);
    has_depth_stencil_ = true;
    done();
}

void Framebuffer::clearDepthAttachment()
{
    if (has_depth_stencil_)
    {
        depth_stencil_->release();
        has_depth_stencil_ = false;
    }
}

size_t Framebuffer::numColorAttachments() const
{
    return colors_.size();
}

bool Framebuffer::hasDepthStencilAttachment() const
{
    return has_depth_stencil_;
}

Texture2D *Framebuffer::colorAttachment(size_t i)
{
    if (i >= colors_.size() + 1)
    {
        throw std::runtime_error(
            "Invalid (out-of-range) index " + std::to_string(i) +
            "for color attachments; expected <= " + std::to_string(colors_.size()));
    }
    return colors_[i].get();
}

void Framebuffer::blit(Framebuffer &target_fbo, bool color, bool depth, bool stencil)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target_fbo.id());
    // glDrawBuffer(GL_BACK);
    GLbitfield bits = 0;
    if (color)
    {
        bits |= GL_COLOR_BUFFER_BIT;
    }
    if (depth && target_fbo.hasDepthStencilAttachment())
    {
        bits |= GL_DEPTH_BUFFER_BIT;
    }
    if (stencil && target_fbo.hasDepthStencilAttachment())
    {
        bits |= GL_STENCIL_BUFFER_BIT;
    }
    glBlitFramebuffer(0, 0, width_, height_, 0, 0, target_fbo.width(), target_fbo.height(), bits,
                      GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::blitToScreen(glm::ivec2 dst0, glm::ivec2 dst1, bool color, bool depth,
                               bool stencil)
{
    GLbitfield bits = 0;
    if (color)
    {
        bits |= GL_COLOR_BUFFER_BIT;
    }
    if (depth)
    {
        bits |= GL_DEPTH_BUFFER_BIT;
    }
    if (stencil)
    {
        bits |= GL_STENCIL_BUFFER_BIT;
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width_, height_, dst0.x, dst0.y, dst1.x, dst1.y, bits, GL_NEAREST);
}

void Framebuffer::blitToScreen(glm::ivec2 src0, glm::ivec2 src1, glm::ivec2 dst0, glm::ivec2 dst1,
                               bool color, bool depth, bool stencil)
{
    GLbitfield bits = 0;
    if (color)
    {
        bits |= GL_COLOR_BUFFER_BIT;
    }
    if (depth)
    {
        bits |= GL_DEPTH_BUFFER_BIT;
    }
    if (stencil)
    {
        bits |= GL_STENCIL_BUFFER_BIT;
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(src0.x, src0.y, src1.x, src1.y, dst0.x, dst0.y, dst1.x, dst1.y, bits,
                      GL_NEAREST);
}

Image Framebuffer::getImage(int attachment_index) const
{
    assert(attachment_index < colors_.size());
    Image im;
    unsigned char *pixel_data = (unsigned char *)malloc(3 * width_ * height_);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
    glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, pixel_data);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    im.setPixels(width_, height_, 3, pixel_data);
    free(pixel_data);
    return im;
}

} // namespace rcube
#pragma once

#include "../LibCoreHeaders.h"

namespace libCore {

    class GBO {
    public:
        GBO() : gBuffer(0), bufferWidth(0), bufferHeight(0) {}

        ~GBO() {
            cleanup();
        }

        void init(int width, int height) {
            bufferWidth = width;
            bufferHeight = height;
            setupGBuffer();
            checkGBufferStatus();
        }

        void bindGBuffer() {
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        }

        void unbindGBuffer() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void bindTexture(const std::string& attachmentType, int textureSlot) {
            glActiveTexture(GL_TEXTURE0 + textureSlot);
            glBindTexture(GL_TEXTURE_2D, attachments[attachmentType]);
        }

        GLuint getTexture(const std::string& attachmentType) {
            return attachments[attachmentType];
        }

        void resize(int width, int height) {
            bufferWidth = width;
            bufferHeight = height;
            cleanup(); // Eliminar los recursos existentes
            setupGBuffer(); // Configurar de nuevo el G-Buffer con los nuevos tamaños
            checkGBufferStatus(); // Verificar el estado del framebuffer
        }

        GLuint gBuffer;

    private:
        std::map<std::string, GLuint> attachments;
        int bufferWidth, bufferHeight;

        void setupGBuffer() {
            glGenFramebuffers(1, &gBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

            // Position attachment
            GLuint positionTexture = createTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture, 0);

            // Normal attachment
            GLuint normalTexture = createTexture(GL_RGB16F, GL_RGB, GL_FLOAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

            // Albedo attachment
            GLuint albedoTexture = createTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoTexture, 0);

            // Depth attachment
            GLuint depthTexture = createTexture(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

            // Attachments list
            GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            glDrawBuffers(3, attachments);

            this->attachments["position"] = positionTexture;
            this->attachments["normal"] = normalTexture;
            this->attachments["albedo"] = albedoTexture;
            this->attachments["depth"] = depthTexture;

            checkGBufferStatus();
            unbindGBuffer(); // Unbind the GBuffer after setup is complete
        }

        GLuint createTexture(GLenum internalFormat, GLenum format, GLenum type) {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, bufferWidth, bufferHeight, 0, format, type, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            return texture;
        }

        void cleanup() {
            for (auto& attachment : attachments) {
                glDeleteTextures(1, &attachment.second);
            }
            glDeleteFramebuffers(1, &gBuffer);
        }

        void checkGBufferStatus() {
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "ERROR::GBufferManager::Framebuffer is not complete!" << std::endl;
            }
            unbindGBuffer();
        }
    };
}

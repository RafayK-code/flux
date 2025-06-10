#include <pch.h>

#include <flux/renderer/Renderer2D.h>
#include <glm/gtc/type_ptr.hpp>

namespace flux
{
    Renderer2D::Renderer2D(const Renderer2DSpecification& specification)
        : context_(specification.graphicsContext), maxQuads_(specification.maxQuads), maxVertices_(specification.maxQuads * 4), maxIndices_(specification.maxQuads * 6)
    {
        renderer_ = RendererAPI::Create(context_);
        batchData_ = CreateScope<BatchData>();

        FramebufferSpecification framebufferSpec;
        framebufferSpec.attachments = { {ImageFormat::RGBA}, {ImageFormat::DEPTH24STENCIL8} };
        framebufferSpec.width = 1280;
        framebufferSpec.height = 720;
        batchData_->framebuffer_ = Framebuffer::Create(framebufferSpec);

        uint32_t* quadIndices = new uint32_t[maxIndices_];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < maxIndices_; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices, maxIndices_);
        delete[] quadIndices;

        VertexBufferLayout vbLayout = {
            { "a_Position",  ShaderDataType::Float3 },
            { "a_Color",     ShaderDataType::Float4 },
            { "a_TexCoord",  ShaderDataType::Float2 },
        };

        batchData_->quadVertexArrays.resize(context_->FramesInFlight());
        batchData_->quadVertexBuffers.resize(context_->FramesInFlight());

        for (uint32_t i = 0; i < context_->FramesInFlight(); i++)
        {
            batchData_->quadVertexArrays[i] = VertexArray::Create();
            batchData_->quadVertexBuffers[i] = VertexBuffer::Create(maxVertices_ * sizeof(QuadVertex));

            batchData_->quadVertexArrays[i]->SetVertexBuffer(batchData_->quadVertexBuffers[i]);
            batchData_->quadVertexArrays[i]->SetIndexBuffer(quadIndexBuffer);
            batchData_->quadVertexArrays[i]->SetLayout(vbLayout);
        }

        // todo: we should create a shader cache
        ShaderUniformLayout layout = {
            { "u_ViewProjection", 0, UniformType::UniformBuffer, ShaderStage::Vertex },
            { "u_Texture", 1, UniformType::Sampler, ShaderStage::Fragment },
        };
        Ref<Shader> quadShader = Shader::Create("assets/shaders/textured_quad.vert.spv", "assets/shaders/textured_quad.frag.spv", layout);

        PipelineSpecification quadPipelineSpec{};
        quadPipelineSpec.framebuffer = nullptr;
        quadPipelineSpec.shader = quadShader;
        quadPipelineSpec.layout = vbLayout;
        quadPipelineSpec.depthState.enabled = true;
        quadPipelineSpec.graphicsContext = context_;
        batchData_->quadPipeline = Pipeline::Create(quadPipelineSpec);

        batchData_->quadRenderPass = CreateRef<RenderPass>(batchData_->quadPipeline);

        batchData_->quadVertexBufferBase = new QuadVertex[maxVertices_];

        camera_ = CreateRef<OrthographicCamera>(1280.0f, 720.0f, -100.0f, -0.1f);
        viewProjectionUB_ = UniformBuffer::Create(sizeof(float) * 16);
        viewProjectionUB_->SetData(glm::value_ptr(camera_->ViewProjectionMatrix()), sizeof(float) * 16);

        quadShader->PushUniformBuffer(viewProjectionUB_, 0);

        batchData_->quadVertexPositions[0] = { -0.5f, 0.5f, 0.0f, 1.0f };
        batchData_->quadVertexPositions[1] = { 0.5f, 0.5f, 0.0f, 1.0f };
        batchData_->quadVertexPositions[2] = { 0.5f, -0.5f, 0.0f, 1.0f };
        batchData_->quadVertexPositions[3] = { -0.5f, -0.5f, 0.0f, 1.0f };
    }

    Renderer2D::~Renderer2D()
    {
        renderer_.reset();
    }

    void Renderer2D::SetActiveCamera(const Ref<OrthographicCamera>& camera)
    {
        camera_ = camera;
        viewProjectionUB_->SetData(glm::value_ptr(camera_->ViewProjectionMatrix()), sizeof(float) * 16);
    }

    void Renderer2D::BeginFrame()
    {
        renderer_->BeginFrame();
        StartBatch();
    }

    void Renderer2D::EndFrame()
    {
        FlushCurrentBatch();
        renderer_->Present();
    }

    void Renderer2D::StartBatch()
    {
        batchData_->quadIndexCount = 0;
        batchData_->quadVertexBufferPtr = batchData_->quadVertexBufferBase;
    }

    void Renderer2D::EndBatch()
    {
        FlushCurrentBatch();
        StartBatch();
    }

    void Renderer2D::FlushCurrentBatch()
    {
        uint32_t currFrameInFlight = renderer_->CurrentFrameInFlight();

        if (batchData_->quadIndexCount > 0)
        {
            uint32_t size = (uint8_t*)batchData_->quadVertexBufferPtr - (uint8_t*)batchData_->quadVertexBufferBase;
            Ref<VertexBuffer> currVertexBuffer = batchData_->quadVertexBuffers[currFrameInFlight];
            currVertexBuffer->SetData(batchData_->quadVertexBufferBase, size);

            renderer_->Draw(batchData_->quadRenderPass, batchData_->quadVertexArrays[currFrameInFlight], batchData_->quadIndexCount);
        }
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Image>& image)
    {
        constexpr size_t quadVertexCount = 4;
        glm::vec2 texCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

        if (batchData_->quadIndexCount >= maxIndices_)
            EndBatch();

        batchData_->quadRenderPass->GetShader()->PushSampler(image, 1, renderer_->CurrentFrameInFlight());

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            batchData_->quadVertexBufferPtr->position = transform * batchData_->quadVertexPositions[i];
            batchData_->quadVertexBufferPtr->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            batchData_->quadVertexBufferPtr->texCoord = texCoords[i];
            batchData_->quadVertexBufferPtr++;
        }

        batchData_->quadIndexCount += 6;
    }
}
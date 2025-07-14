#include <fxpch.h>

#include <flux.h>
#include <flux/core/EntryPoint.h>

#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct PlyModel {
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};

inline PlyModel LoadPlyInterleaved(const std::string& path) {
    PlyModel model;
    std::ifstream file(path);
    if (!file.is_open()) {
        //std::cerr << "Failed to open: " << path << std::endl;
        return model;
    }

    std::string line;
    size_t vertexCount = 0;
    size_t faceCount = 0;
    bool headerEnded = false;

    // --- Parse header ---
    while (std::getline(file, line)) {
        if (line.substr(0, 14) == "element vertex")
            vertexCount = std::stoi(line.substr(15));
        else if (line.substr(0, 12) == "element face")
            faceCount = std::stoi(line.substr(13));
        else if (line == "end_header") {
            headerEnded = true;
            break;
        }
    }

    if (!headerEnded) {
        //std::cerr << "PLY file missing end_header." << std::endl;
        return model;
    }

    // --- Parse vertices ---
    std::vector<glm::vec3> positions(vertexCount);
    std::vector<glm::vec3> normalsAccum(vertexCount, glm::vec3(0.0f));
    for (size_t i = 0; i < vertexCount && std::getline(file, line); ++i) {
        std::istringstream iss(line);
        float x, y, z;
        iss >> x >> y >> z;
        positions[i] = glm::vec3(x, y, z);
    }

    // --- Parse and triangulate faces ---
    model.indices.reserve(faceCount * 3 * 3); // Rough estimate
    for (size_t i = 0; i < faceCount && std::getline(file, line); ++i) {
        std::istringstream iss(line);
        size_t count;
        iss >> count;
        std::vector<uint32_t> faceIndices(count);
        for (size_t j = 0; j < count; ++j)
            iss >> faceIndices[j];

        // Fan triangulation
        for (size_t j = 1; j + 1 < count; ++j) {
            uint32_t i0 = faceIndices[0];
            uint32_t i1 = faceIndices[j];
            uint32_t i2 = faceIndices[j + 1];

            model.indices.push_back(i0);
            model.indices.push_back(i1);
            model.indices.push_back(i2);

            // Compute face normal
            glm::vec3 faceNormal = glm::normalize(glm::cross(
                positions[i1] - positions[i0],
                positions[i2] - positions[i0]));

            // Accumulate normals to each vertex
            normalsAccum[i0] += faceNormal;
            normalsAccum[i1] += faceNormal;
            normalsAccum[i2] += faceNormal;
        }
    }

    // --- Build interleaved buffer ---
    model.vertices.reserve(vertexCount * 6);
    for (size_t i = 0; i < vertexCount; ++i) {
        glm::vec3 n = glm::normalize(normalsAccum[i]);
        model.vertices.push_back(positions[i].x);
        model.vertices.push_back(positions[i].y);
        model.vertices.push_back(positions[i].z);
        model.vertices.push_back(n.x);
        model.vertices.push_back(n.y);
        model.vertices.push_back(n.z);
    }

    return model;
}

inline PlyModel LoadPly(const std::string& path) {
    PlyModel model;
    std::ifstream file(path);
    if (!file.is_open()) {
        //std::cerr << "Failed to open: " << path << std::endl;
        return model;
    }

    std::string line;
    size_t vertexCount = 0;
    size_t faceCount = 0;
    bool headerEnded = false;

    // --- Parse header ---
    while (std::getline(file, line)) {
        if (line.substr(0, 14) == "element vertex")
            vertexCount = std::stoi(line.substr(15));
        else if (line.substr(0, 12) == "element face")
            faceCount = std::stoi(line.substr(13));
        else if (line == "end_header") {
            headerEnded = true;
            break;
        }
    }

    if (!headerEnded) {
        //std::cerr << "PLY file missing end_header." << std::endl;
        return model;
    }

    // --- Parse vertices ---
    model.vertices.reserve(vertexCount * 3);
    for (size_t i = 0; i < vertexCount && std::getline(file, line); ++i) {
        std::istringstream iss(line);
        float x, y, z;
        iss >> x >> y >> z;
        model.vertices.push_back(x);
        model.vertices.push_back(y);
        model.vertices.push_back(z);
    }

    // --- Parse and triangulate faces ---
    for (size_t i = 0; i < faceCount && std::getline(file, line); ++i) {
        std::istringstream iss(line);
        size_t count;
        iss >> count;
        std::vector<uint32_t> faceIndices(count);
        for (size_t j = 0; j < count; ++j)
            iss >> faceIndices[j];

        // Fan triangulation
        for (size_t j = 1; j + 1 < count; ++j) {
            model.indices.push_back(faceIndices[0]);
            model.indices.push_back(faceIndices[j]);
            model.indices.push_back(faceIndices[j + 1]);
        }
    }

    return model;
}

class FluxDemoApplication : public flux::Application
{
public:
    FluxDemoApplication() = default;
    ~FluxDemoApplication() = default;

    struct CameraData
    {
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
        alignas(16) glm::vec3 viewPos;
    };

    struct ModelData
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 normalMatrix;
    };

    struct LightData
    {
        alignas(16) glm::vec3 lightPos;
        alignas(16) glm::vec3 lightColor;
        alignas(4)  float lightIntensity;
    };

    struct MaterialData
    {
        alignas(16) glm::vec3 ambient;
        alignas(16) glm::vec3 diffuse;
        alignas(16) glm::vec3 specular;
        alignas(4)  float shininess;
    };

    void OnInit() override
    {
        PlyModel model = LoadPlyInterleaved("assets/meshes/dragon_vrip.ply");

        vertexBuffer = flux::VertexBuffer::Create(model.vertices.data(), sizeof(float) * model.vertices.size());
        indexBuffer = flux::IndexBuffer::Create(model.indices.data(), model.indices.size());
        vertexArray = flux::VertexArray::Create(vertexBuffer, indexBuffer);

        flux::ShaderUniformLayout layout = { {0, {
            { "u_Camera", 0, flux::UniformType::UniformBuffer, flux::ShaderStage::Vertex | flux::ShaderStage::Fragment },
            { "u_Light", 1, flux::UniformType::UniformBuffer, flux::ShaderStage::Fragment },
        }},
        {1, {
            { "u_Model", 0, flux::UniformType::UniformBuffer, flux::ShaderStage::Vertex },
            { "u_Material", 1, flux::UniformType::UniformBuffer, flux::ShaderStage::Fragment },
        }} };

        flux::VertexBufferLayout vbLayout = {
            { "a_Position", flux::ShaderDataType::Float3 },
            { "a_Normal",  flux::ShaderDataType::Float3 },
        };

        shader = flux::Shader::Create("assets/shaders/model.vert.spv", "assets/shaders/model.frag.spv", layout);

        flux::PipelineSpecification pipelineSpec{};
        pipelineSpec.shader = shader;
        pipelineSpec.layout = vbLayout;
        pipelineSpec.graphicsContext = GetWindow().Context();

        pipeline = flux::Pipeline::Create(pipelineSpec);
        renderPass = flux::CreateRef<flux::RenderPass>(pipeline);

        cameraUB = flux::UniformBuffer::Create(sizeof(CameraData));
        modelUB = flux::UniformBuffer::Create(sizeof(ModelData));
        lightUB = flux::UniformBuffer::Create(sizeof(LightData));
        materialUB = flux::UniformBuffer::Create(sizeof(MaterialData));

        flux::PerspectiveCamera camera = flux::PerspectiveCamera(glm::radians(90.0f), 16 / 9.0f, 0.01f, 100.0f);
        camera.SetPosition(glm::vec3(2.0f, 0.0f, -3.0f));
        camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        CameraData camdata = { camera.ViewMatrix(), camera.ProjectionMatrix(), camera.Position() };
        cameraUB->SetData(&camdata, sizeof(camdata));

        sceneInputSet = flux::ShaderInputSet::Create(shader, 0);

        sceneInputSet->PushUniformBuffer(cameraUB, 0);
        sceneInputSet->PushUniformBuffer(lightUB, 1);

        drawInputSet = flux::ShaderInputSet::Create(shader, 1);

        drawInputSet->PushUniformBuffer(modelUB, 0);
        drawInputSet->PushUniformBuffer(materialUB, 1);

        glm::mat4 modelMat =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
        glm::mat4 normalMat = glm::transpose(glm::inverse(modelMat));
        modelData = { modelMat, normalMat };
        modelUB->SetData(&modelData, sizeof(ModelData));

        LightData lightData{};
        lightData.lightPos = glm::vec3(0.0f, 1.0f, -2.0f);
        lightData.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        lightData.lightIntensity = 1.0f;
        lightUB->SetData(&lightData, sizeof(LightData));

        MaterialData materialData{};
        materialData.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        materialData.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        materialData.specular = glm::vec3(1.0f, 1.0f, 1.0f);
        materialData.shininess = 32.0f;
        materialUB->SetData(&materialData, sizeof(MaterialData));

        commandBuffer = flux::RenderCommandBuffer::Create(GetWindow().Context());
        renderer = flux::RendererAPI::Create(GetWindow().Context());

        for (uint32_t i = 0; i < GetWindow().Context()->FramesInFlight(); i++)
        {
            commandBuffer->Begin(i);
            renderer->BeginRenderPass(commandBuffer, i, pipeline->TargetFramebuffer());
            renderer->BindPipeline(commandBuffer, i, pipeline);

            renderer->BindShaderInputSet(commandBuffer, i, sceneInputSet, pipeline);

            renderer->BindShaderInputSet(commandBuffer, i, drawInputSet, pipeline);
            renderer->Draw(commandBuffer, i, vertexArray);

            renderer->EndRenderPass(commandBuffer, i);
            commandBuffer->End(i);
        }
    }

    void OnUpdate(float dt) override
    {
        time_ += dt;

        glm::mat4 modelMat =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), time_ * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));

        glm::mat4 normalMat = glm::transpose(glm::inverse(modelMat));
        modelData.model = modelMat;
        modelData.normalMatrix = normalMat;

        modelUB->SetData(&modelData, sizeof(ModelData));

        uint32_t i = renderer->BeginFrame();

        commandBuffer->Begin(i);
        renderer->BeginRenderPass(commandBuffer, i, pipeline->TargetFramebuffer());
        renderer->BindPipeline(commandBuffer, i, pipeline);

        renderer->BindShaderInputSet(commandBuffer, i, sceneInputSet, pipeline);

        renderer->BindShaderInputSet(commandBuffer, i, drawInputSet, pipeline);
        renderer->Draw(commandBuffer, i, vertexArray);

        renderer->EndRenderPass(commandBuffer, i);
        commandBuffer->End(i);

        renderer->SubmitCommandBuffer(commandBuffer);
        renderer->Present();
    }

    void OnShutdown() override
    {
    }

private:
    flux::Ref<flux::VertexBuffer> vertexBuffer;
    flux::Ref<flux::IndexBuffer> indexBuffer;
    flux::Ref<flux::VertexArray> vertexArray;

    flux::Ref<flux::Shader> shader;
    flux::Ref<flux::Pipeline> pipeline;
    flux::Ref<flux::RenderPass> renderPass;

    flux::Ref<flux::UniformBuffer> cameraUB;
    flux::Ref<flux::UniformBuffer> modelUB;
    flux::Ref<flux::UniformBuffer> lightUB;
    flux::Ref<flux::UniformBuffer> materialUB;

    flux::Ref<flux::ShaderInputSet> sceneInputSet;
    flux::Ref<flux::ShaderInputSet> drawInputSet;

    flux::Ref<flux::RenderCommandBuffer> commandBuffer;
    flux::Ref<flux::RendererAPI> renderer;

    ModelData modelData{};

    float time_ = 0.0f;
};

flux::Application* flux::CreateApplication(int argc, char** argv)
{
    return new FluxDemoApplication();
}
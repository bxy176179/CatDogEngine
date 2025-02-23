#include "Path.h"

#include "Base/Template.h"
#include "Log/Log.h"

#include <SDL_stdinc.h>

#include <cassert>

namespace engine
{

GraphicsBackend Path::s_backend = engine::GraphicsBackend::Noop;

std::optional<std::filesystem::path> Path::GetApplicationDataPath()
{
    const char* pKey = GetPlatformPathKey();
    char* pValue = SDL_getenv(pKey);
    if (!pValue)
    {
        CD_ENGINE_ERROR("Cannot find environment variable %s.", pKey);
        return std::nullopt;
    }
    
    return GetPlatformAppDataPath(pValue);
}

const char* Path::GetPlatformPathKey()
{
#if defined(_WIN32)
    return "LOCALAPPDATA";
#elif defined(__linux__)
    return "HOME";
#elif defined(__APPLE__)
    return "HOME";
#else
    #error Unsupport platform!
#endif
}

std::filesystem::path Path::GetPlatformAppDataPath(const char* pRootPath)
{
    // TODO : Need more test.
#if defined(_WIN32)
    return std::filesystem::path(pRootPath);
#elif defined(__linux__)
    return std::filesystem::path(pRootPath) / ".local" / "share";
#elif defined(__APPLE__)
    return std::filesystem::path(pRootPath) / "Library" / "Application Support";
#else
    #error Unsupport platform!
#endif
}

std::filesystem::path Path::GetEngineBuiltinShaderPath()
{
    return std::filesystem::path(CDENGINE_BUILTIN_SHADER_PATH);
}

std::filesystem::path Path::GetEngineResourcesPath()
{
    return std::filesystem::path(CDPROJECT_RESOURCES_ROOT_PATH);
}

std::filesystem::path Path::GetEditorResourcesPath()
{
    return std::filesystem::path(CDEDITOR_RESOURCES_ROOT_PATH);
}

std::filesystem::path Path::GetProjectsSharedPath()
{
    return std::filesystem::path(CDPROJECT_RESOURCES_SHARED_PATH);
}

engine::GraphicsBackend Path::GetGraphicsBackend()
{
    return s_backend;
}

void Path::SetGraphicsBackend(engine::GraphicsBackend backend)
{
    s_backend = backend;

    std::filesystem::path directoryPath = GetShaderOutputDirectory();
    if (!std::filesystem::is_directory(directoryPath))
    {
        std::filesystem::create_directories(directoryPath);
    }
}

std::string Path::GetBuiltinShaderInputPath(const char* pShaderName)
{
    return (GetEngineBuiltinShaderPath() / std::filesystem::path(pShaderName)).replace_extension(ShaderInputExtension).string();
}

std::filesystem::path Path::GetShaderOutputDirectory()
{
    return GetProjectsSharedPath() / "BuiltInShaders" / GetGraphicsBackendName(s_backend);
}

std::string Path::GetShaderOutputPath(const char* pInputFilePath, const std::string& options)
{
    std::string outputShaderFileName = std::filesystem::path(pInputFilePath).stem().string();

    if (!options.empty())
    {
        std::string appendName = "_" + options;
        std::replace(appendName.begin(), appendName.end(), ';', '_');
        outputShaderFileName += appendName;

        assert(!outputShaderFileName.empty());
        const auto last = outputShaderFileName.end() - 1;
        if (*last == '_')
        {
            outputShaderFileName.erase(last);
        }
    }

    return (GetShaderOutputDirectory() / cd::MoveTemp(outputShaderFileName)).replace_extension(ShaderOutputExtension).string();
}

std::string Path::GetTextureOutputFilePath(const char* pInputFilePath, const char* extension)
{
    return ((GetEngineResourcesPath() / "Textures" / std::filesystem::path(pInputFilePath).stem()).replace_extension(extension)).string();
}

std::string Path::GetTerrainTextureOutputFilePath(const char* pInputFilePath, const char* extension)
{
    return ((GetEngineResourcesPath() / "Textures" / "Terrain" / std::filesystem::path(pInputFilePath).stem()).replace_extension(extension)).string();
}

}
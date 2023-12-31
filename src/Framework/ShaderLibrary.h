#pragma once
#include "Common.hpp"
#include "Shader.h"
#include <map>

struct ShaderDef
{
	std::string shaderName;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
};

class ShaderLibrary
{
public:
	ShaderLibrary();
	ShaderLibrary(std::vector<ShaderDef>& shaders);
	~ShaderLibrary();

	bool AddShader(ShaderDef& shaderDef);
	bool IsShaderAvailible(std::string& name);
	std::shared_ptr<Shader> GetShader(std::string& name);
private:
	std::map<std::string, std::shared_ptr<Shader>> m_shaderMap;
};
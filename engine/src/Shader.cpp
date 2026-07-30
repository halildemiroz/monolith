#include <Shader.h>
#include <vector>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

GLuint Monolith::Shader::compileStage(GLenum stage, const std::string& src){
	GLuint id = glCreateShader(stage);
	const char* csrc = src.c_str();
	glShaderSource(id, 1, &csrc, nullptr);
	glCompileShader(id);
	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if(!success){
		GLint len;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		std::vector<char> log(len);
		glGetShaderInfoLog(id, len, nullptr, log.data());
		std::cerr << log.data() << std::endl;
	}
	return id;
}

Monolith::Shader::Shader(const std::string &vertexSrc, const std::string &fragmentSrc){
	GLuint vs = compileStage(GL_VERTEX_SHADER, vertexSrc);
	GLuint fs = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);
	
	m_id = glCreateProgram();
	
	glAttachShader(m_id, vs);
	glAttachShader(m_id, fs);
	glLinkProgram(m_id);

	GLint success;
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if(!success){
		GLint len;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
		std::vector<char> log(len);
		glGetProgramInfoLog(m_id, len, nullptr, log.data());
		std::cerr << log.data() << std::endl;
	}
	glDeleteShader(vs); 
	glDeleteShader(fs);
}

Monolith::Shader::~Shader(){
	glDeleteProgram(m_id);
}

void Monolith::Shader::Bind(){
	glUseProgram(m_id);
}

void Monolith::Shader::Unbind(){
	glUseProgram(0);
}

GLint Monolith::Shader::getUniformLocation(const std::string& name){
	return glGetUniformLocation(m_id, name.c_str());
}

void Monolith::Shader::setMat4(const std::string& name, const glm::mat4& value){
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Monolith::Shader::setVec4(const std::string& name, const glm::vec4& value){
	glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}


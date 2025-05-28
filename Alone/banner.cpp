#include "banner.h"
const int bannerNumQuadVertices = 4;
const float bannerVertexData[bannerNumQuadVertices * 5] = {

	// x      y      z     u     v
	-1.0f,  0.15f, 0.0f, 0.0f, 1.0f,
	-1.0f, -0.15f, 0.0f, 0.0f, 0.0f,
	 1.0f,  0.15f, 0.0f, 3.0f, 1.0f,
	 1.0f, -0.15f, 0.0f, 3.0f, 0.0f
};

Banner::Banner(float elapsedTime, const char* filename) {
	startTime = elapsedTime;
	currentTime = startTime;

	scale = 0.5f;
	position = glm::vec3(0.0f, 0.2f, -0.1f);

	InitShader();
	InitGeometry(filename);
}

void Banner::Update(const glm::vec3& position) {
	Banner::position = position;
}

void Banner::InitShader() {
	std::vector<GLuint> shaders;

	// push vertex shader and fragment shader
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "banner.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "banner.frag"));

	// Create the program with two shaders
	program = pgr::createProgram(shaders);

	// get position and color attributes locations
	posLocation = glGetAttribLocation(program, "position");
	texCoordLocation = glGetAttribLocation(program, "texCoord");
	
	// get uniforms locations
	PVMmatrixLocation = glGetUniformLocation(program, "PVMmatrix");
	timeLocation = glGetUniformLocation(program, "time");
	texSamplerLocation = glGetUniformLocation(program, "texSampler");
}

void Banner::InitGeometry(const char* filename) {

	texture = pgr::createTexture(filename);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bannerVertexData), bannerVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);

	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	numTriangles = bannerNumQuadVertices;
}


void Banner::Draw(const glm::mat4& view, const glm::mat4& projection) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(program);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
	matrix = glm::scale(matrix, glm::vec3(scale));
	matrix = glm::rotate(matrix, glm::radians(180.0f),glm::vec3(0.0f,1.0f,0.0f));
	matrix = glm::rotate(matrix, glm::radians(-25.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 PVMmatrix = projection * view * matrix;
	glUniformMatrix4fv(PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));
	glUniform1f(timeLocation, currentTime - startTime);
	glUniform1i(texSamplerLocation, 0);
	startTime = currentTime*0.01;

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, numTriangles);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	return;
}
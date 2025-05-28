#include "explosion_shader.h"
const float vertices[20] = {
	// x      y     z     u     v
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};

ExplosionShader::ExplosionShader(const char* filename) {
	InitShaders();
	InitGeometry(filename);
}

void ExplosionShader::InitShaders() {
	std::vector<GLuint> shaders;

	// push vertex shader and fragment shader
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "explosion.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "explosion.frag"));

	// create the program with two shaders
	program = pgr::createProgram(shaders);

	// get position and texture coordinates attributes locations
	positionLocation = glGetAttribLocation(program, "position");
	textureCoordsLocation = glGetAttribLocation(program, "textureCoords");

	// get uniforms locations
	PVMLocation = glGetUniformLocation(program, "PVM");
	viewLocation = glGetUniformLocation(program, "view");
	timeLocation = glGetUniformLocation(program, "time");
	textureSamplerLocation = glGetUniformLocation(program, "textureSampler");
	frameDurationLocation = glGetUniformLocation(program, "frameDuration");
}

void ExplosionShader::InitGeometry(const char* filename) {

	texture = pgr::createTexture(filename);

	// vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(textureCoordsLocation);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(textureCoordsLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
}
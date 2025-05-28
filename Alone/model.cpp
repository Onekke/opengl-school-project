#include "model.h"

Model::Model(const std::string& fileName, Shader* shader) {
	Model::shader = shader;
	if (LoadMesh(fileName) != true) {
		std::cerr << "Model loading failed." << std::endl;
	}
}

void Model::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
	glUseProgram(shader->program);

	// set vertex shader transform attributes
	shader->SetTransformUniforms(model, view, projection);

	// set fragment shader material attributes
	for (size_t i = 0; i < modelGeometry.size(); i++)
	{
		glBindVertexArray(modelGeometry[i]->vao);
		shader->SetMaterialUniforms(
			modelGeometry[i]->ambient,
			modelGeometry[i]->diffuse,
			modelGeometry[i]->specular,
			modelGeometry[i]->shininess
		);

		//if (modelGeometry[i]->useTexture) {
		//	shader->SetTextureUniforms(
		//		modelGeometry[i]->textureDiffuse,
		//		modelGeometry[i]->textureSpecular
		//	);
		//}

		// draw geometry
		glDrawElements(GL_TRIANGLES, modelGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}
	CHECK_GL_ERROR();
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

bool Model::LoadMesh(const std::string& fileName) {
	// read file via ASSIMP
	Assimp::Importer importer;

	// Scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return false;
	}

	// process each mesh located at the current node	
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		Mesh* geometry = new Mesh();

		// vertex buffer object, store all vertex positions and normals
		glGenBuffers(1, &((geometry)->vbo));
		glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vbo);

		// allocate memory for vertices, normals, and texture coordinates
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW);

		// store all vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);

		// store all normals
		glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

		// just texture 0 for now
		float* textureCoords = new float[2 * mesh->mNumVertices];
		float* currentTextureCoords = textureCoords;

		// copy texture coordinates
		aiVector3D vector;

		if (mesh->HasTextureCoords(0)) {
			for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
				vector = (mesh->mTextureCoords[0])[idx];
				*currentTextureCoords++ = vector.x;
				*currentTextureCoords++ = vector.y;
			}
		}

		// store all texture coordinates
		glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

		// copy all mesh faces into one big array
		unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
			indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
			indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
		}

		// copy temporary index array to OpenGL and free the array
		glGenBuffers(1, &((geometry)->ebo));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

		delete[] indices;

		// copy the material info to Mesh structure
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D	color;
		aiString	name;
		aiReturn	retValue = AI_SUCCESS;

		// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
		material->Get(AI_MATKEY_NAME, name);

		// set diffuse color
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		(geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

		// set ambient color
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		(geometry)->ambient = glm::vec3(color.r, color.g, color.b);

		// set specular color
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		(geometry)->specular = glm::vec3(color.r, color.g, color.b);

		// set shininess and strength
		ai_real shininess, strength;
		unsigned int max;

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
			shininess = 1.0f;
		max = 1;
		if ((retValue = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
			strength = 1.0f;
		(geometry)->shininess = shininess * strength;

		// set diffuse texture to 0 for now
		(geometry)->useTexture = false;
		(geometry)->textureDiffuse = 0;

		// load diffuse texture image
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			// get texture name 
			aiString path; // filename

			aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::string textureName = path.data;

			size_t found = fileName.find_last_of("/\\");

			// insert correct texture file path 
			if (found != std::string::npos) { // not found
			  //subMesh_p->textureName.insert(0, "/");
				textureName.insert(0, fileName.substr(0, found + 1));
			}

			std::cout << "Loading diffuse texture file: " << textureName << std::endl;
			(geometry)->textureDiffuse = pgr::createTexture(textureName);
		}
		CHECK_GL_ERROR();

		// set specular texture to 0 for now
		(geometry)->textureSpecular = 0;

		// load specular texture image
		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			// get texture name 
			aiString path; // filename

			aiReturn texFound = material->GetTexture(aiTextureType_SPECULAR, 0, &path);
			std::string textureName = path.data;

			size_t found = fileName.find_last_of("/\\");

			// insert correct texture file path 
			if (found != std::string::npos) { // not found
			  //subMesh_p->textureName.insert(0, "/");
				textureName.insert(0, fileName.substr(0, found + 1));
			}

			std::cout << "Loading specular texture file: " << textureName << std::endl;
			(geometry)->textureSpecular = pgr::createTexture(textureName);
			(geometry)->useTexture = true;
		}
		CHECK_GL_ERROR();

		glGenVertexArrays(1, &((geometry)->vao));
		glBindVertexArray((geometry)->vao);

		// bind element array buffer (indices) to vao
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->ebo);
		glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vbo);

		glEnableVertexAttribArray(shader->positionLocation);
		glVertexAttribPointer(shader->positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shader->normalLocation);
		glVertexAttribPointer(shader->normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

		glEnableVertexAttribArray(shader->textureCoordsLocation);
		glVertexAttribPointer(shader->textureCoordsLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
		CHECK_GL_ERROR();

		glBindVertexArray(0);

		(geometry)->numTriangles = mesh->mNumFaces;

		//push newGeometry to vector of geometies
		//printf("modelGeometry %d\n", modelGeometry.size());
		modelGeometry.push_back(geometry);
	}
	return true;
}
#include "object.h"
Object::Object(Model* model) {
	Object::model = model;
}

//Object::Object(const std::string& fileName, Shader* shader) {
//	model = new Model(shader);
//
//	if (model->LoadMesh(fileName) != true) {
//		std::cerr << "Model loading failed." << std::endl;
//	}
//	CHECK_GL_ERROR();
// }

void Object::ReloadModel(Model* model) {
	Object::model = model;
}

glm::vec3 Object::GetPosition() {
	return position;
}

glm::vec3 Object::GetDirection() {
	return direction;
}

float Object::GetSize() {
	return size;
}

float Object::GetSpeed() {
	return speed;
}

float Object::GetAngle() {
	return angle;
}

void Object::Update(float elapsedTime, bool* keyboard, bool passive) {
	float deltaTime = elapsedTime - currentTime;

	// update current time
	currentTime = elapsedTime;

	if (passive) {
		angle += 2.0f;
		if (angle > 360.0f)
			angle -= 360.0f;
		else if (angle < 360.0f)
			angle += 360.0f;
		direction = glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle)));
		speed = std::min(0.1f, speed + 0.001f);
	}
	else {

		// update rotation
		if (keyboard[KEY_LEFT] == true) {
			angle += 2.0f;
			if (angle > 360.0f)
				angle -= 360.0f;
		}

		if (keyboard[KEY_RIGHT] == true) {
			angle -= 2.0f;
			if (angle < 360.0f)
				angle += 360.0f;
		}

		direction = glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle)));

		// update position
		if (keyboard[KEY_UP] == true) {
			speed = std::min(2.0f, speed + 0.05f);
		}

		if (keyboard[KEY_UP] == false && keyboard[KEY_DOWN] == false) {
			if (speed < 0.0f) {
				speed = std::min(0.0f, speed + 0.02f);
			}
			else if (speed > 0.0f) {
				speed = std::max(0.0f, speed - 0.02f);
			}
		}

		if (keyboard[KEY_DOWN] == true) {
			speed = std::max(-1.0f, speed - 0.05f);
		}
	}

	glm::vec3 newPosition = position + deltaTime * speed * direction;
	if (sqrt(newPosition.x * newPosition.x + newPosition.y * newPosition.y + newPosition.z * newPosition.z) <= 0.5f)
		position = newPosition;
}

void Object::SetDirection(float angle) {
	Object::angle = angle;
	direction = glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle)));
}

void Object::Reset(glm::vec3 position, glm::vec3 rotation, float scale, float angle, float size) {
	Object::position = position;
	Object::rotation = rotation;
	Object::scale	 = scale;
	Object::angle	 = angle;
	Object::size	 = size;

	speed	  = 0.0f;
	direction = glm::vec3(sin(glm::radians(angle)), 0.0f, cos(glm::radians(angle)));
}

void Object::Draw(const glm::mat4& view, const glm::mat4& projection, glm::vec3 cameraPosition, Object* cameraTarget) {
	// prepare modelling transform matrix
	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
	model = glm::rotate(model, glm::radians(angle), rotation);
	model = glm::scale(model, glm::vec3(scale));

	Object::model->Draw(model, view, projection);
}

void Object::SetRandomPosition(Object* object) {
	glm::vec3 newPosition;
	bool invalidPosition = false;

	do {
		newPosition = glm::vec3((float)(1.0 * (rand() / (double)RAND_MAX) - 0.5),
								0.0f,
								(float)(1.0 * (rand() / (double)RAND_MAX) - 0.5));
		invalidPosition = PointInSphere(newPosition, object->GetPosition(), object->GetSize());

	} while (invalidPosition == true);

	position = newPosition;
}

bool Object::PointInSphere(const glm::vec3& point, const glm::vec3& center, float radius) {
	  // check whether the given point lies within a sphere of given radius or not
	float x_diff = point.x - center.x;
	float y_diff = point.y - center.y;
	float z_diff = point.z - center.z;

	return (sqrt(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff) <= radius)?true:false;
}
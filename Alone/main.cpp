#include <time.h>
#include <list>
#include "data.h"
#include "camera.h"
#include "skybox.h"
#include "object.h"
#include "explosion.h"
#include "application.h"
#include "explosion_shader.h"
#include "banner.h"

bool spotOn = false;
bool dirOn = true;
bool fogOn = false;
bool menu = true;
bool passive = false;
int last = 0;

glm::vec3 spotDiff = glm::vec3(0.01f, 0.01f, 0.01f);
glm::vec3 spotSpec = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 dirDiff = glm::vec3(0.8f, 0.8f, 0.8f);
glm::vec3 posCactus[] = { glm::vec3(0.41f,0.0f,0.07f),glm::vec3(0.25f,0.01f,-0.05f),
glm::vec3(0.38f,0.01f,-0.02), glm::vec3(0.4f,0.01f,-0.1f), glm::vec3(0.48f,0.01f,0.0f) };
float angleCactus[] = { 20.0f,-45.0f,120.0f,0.0f,15.0f };
float sizeCactus[] = { 0.01f,0.008f,0.01f,0.01f,0.01f };

const char* EXPLOSION_NAME = "data/explode.png";

const char* FOX = "data/walle.obj";
const char* CHICKEN = "data/wallebox.obj";
const char* GROUND = "data/terrain/map.obj";

// cactuses
//const char* cactus_column00 = "data/cactuses/cactus_column00.obj";
//const char* cactus_column01 = "data/cactuses/cactus_column01.obj";
//const char* cactus_column02 = "data/cactuses/cactus_column02.obj";
//const char* cactus_handy00 = "data/cactuses/cactus_handy00.obj";
//const char* cactus_handy01 = "data/cactuses/cactus_handy01.obj";
//const char* cactus_handy02 = "data/cactuses/cactus_handy02.obj";
//const char* cactus_star00 = "data/cactuses/cactus_star00.obj";
//const char* cactus_round00 = "data/cactuses/cactus_round00.obj";

const char* cactus_cylinder00 = "data/cactuses/cactus_cylinder00.obj";
const char* cactus_cylinder_flower00 = "data/cactuses/cactus_cylinder_flower00.obj";
const char* cactus_cylinder_flower01 = "data/cactuses/cactus_cylinder_flower01.obj";

const char* cactus_round_double00 = "data/cactuses/cactus_round_double00.obj";
const char* cactus_round_double_flower00 = "data/cactuses/cactus_round_double_flower00.obj";

const char* cactus_tentacle00 = "data/cactuses/cactus_tentacle00.obj";
const char* cactus_tentacle01 = "data/cactuses/cactus_tentacle01.obj";
const char* cactus_tentacle_flower00 = "data/cactuses/cactus_tentacle_flower00.obj";
const char* cactus_tentacle_flower01 = "data/cactuses/cactus_tentacle_flower01.obj";

// terrain
const char* map_name = "data/terrain/map.obj";

bool isChick = false;
bool berries[] = { true, true };

typedef std::list<void*> ObjectsCollection;
ObjectsCollection cactusBaseModel;	// 3 base cactus models without flowers
ObjectsCollection cactusObject;		// 5 cactus objects 
bool hasFlowers[] = { true, true, true, true, true };

ObjectsCollection map;
ObjectsCollection explosions;

// application
Application* app = NULL;

// camera
Camera* camera = NULL;

// skybox
Skybox* skybox = NULL;

// banner
Banner* banner = NULL;

// shaders
Shader* modelShader = NULL;
ExplosionShader* explosionShader = NULL;

// Objects
Object* mainCharacter = NULL;
Object* terrain = NULL;

// Models
Model* foxModel = NULL;
Model* chickenModel = NULL;
//Model* mapModel = NULL;

void renderScene(void) {

	camera->Update((float)app->windowWidth, (float)app->windowHeight);

	modelShader->UpdateTime(app->elapsedTime);
	modelShader->SetViewPosition(camera->position);

	skybox->Draw(camera->view, camera->projection);

	modelShader->SetDirectionalLightUniforms(glm::vec3(0.5f, 0.5f, 0.5f), dirDiff, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, -1.0f, 0.0f));

	terrain->Draw(camera->view, camera->projection, camera->position, camera->object);

	// enable stencil test
	glEnable(GL_STENCIL_TEST);
	// set the stencil operations - if the stencil test and depth test are passed than
	// value in the stencil buffer is replaced with the object ID (byte 1..255, 0 ... background)
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	int id = 0;

	ObjectsCollection::iterator it; 
	for (it = cactusObject.begin(); it != cactusObject.end(); ++it) {
		glStencilFunc(GL_ALWAYS, id + 1, -1);
		CHECK_GL_ERROR();
		Object* cactus = (Object*)(*it);
		cactus->Draw(camera->view, camera->projection, camera->position, camera->object);
		id++;
	}

	if (!camera->FirstPerson()) {
		glStencilFunc(GL_ALWAYS, id + 1, -1);
		CHECK_GL_ERROR();
		mainCharacter->Draw(camera->view, camera->projection, camera->position, camera->object);
		// spot ligt 
		modelShader->SetSpotLightUniforms(glm::vec3(0.0f, 0.0f, 0.0f), spotDiff, spotSpec, mainCharacter->GetDirection() - glm::vec3(0.0f, 0.2f, 0.0f), mainCharacter->GetPosition() + glm::vec3(0.0f, 0.05f, 0.0f), 1.0f, 0.09, 0.032, glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(25.0f)));
	} else {
		// spot ligt 
		modelShader->SetSpotLightUniforms(glm::vec3(0.0f, 0.0f, 0.0f), spotDiff, spotSpec, camera->direction, camera->position, 1.0f, 0.09, 0.032, glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(25.0f)));
	}

	// disable stencil test
	glDisable(GL_STENCIL_TEST);

	// draw explosions with depth test disabled
	glDisable(GL_DEPTH_TEST);

	it = explosions.begin();
	while (it != explosions.end()) {
		Explosion* explosion = (Explosion*)(*it);
		explosion->Draw(camera->view, camera->projection);
		++it;
	};

	glEnable(GL_DEPTH_TEST);

	if (menu)
		banner->Draw(camera->view, camera->projection);
}

void updateScene() {
	mainCharacter->Update(app->elapsedTime, app->keyboard, passive);
	//if (mainCharacter->GetSpeed() > 0.0 && last >= 5) {
	//	Explosion* explosion = new Explosion(explosionShader);
	//	explosion->Init(app->elapsedTime, 0.05f, mainCharacter->GetPosition());
	//	explosions.push_back(explosion);
	//	last = 0;
	//}
	//else { last++; }

	banner->currentTime = app->elapsedTime;

	ObjectsCollection::iterator it;

	it = explosions.begin();
	while (it != explosions.end()) {
		Explosion* explosion = (Explosion*)(*it);

		// update explosion
		explosion->currentTime = app->elapsedTime;

		if (explosion->currentTime > explosion->startTime + explosion->textureFrames * explosion->frameDuration)
			it = explosions.erase(it);
		else
			++it;
	}
}

bool init(void) {
	// initialize random seed
	srand((unsigned int)time(NULL));

	// initialize OpenGL
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearStencil(0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// initialize application
	app = new Application();

	// load shaders
	modelShader = new Shader("model.vert", "model.frag");

	modelShader->SetPointLightUniforms(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.25f, 0.05f, -0.3f), 1.0f, 0.09, 0.032);

	explosionShader = new ExplosionShader(EXPLOSION_NAME);

	// load skybox
	skybox = new Skybox("data/skybox/1clouds");

	// banner
	banner = new Banner(app->elapsedTime, "data/gameOver.png");

	// load models
	foxModel = new Model(FOX, modelShader);
	chickenModel = new Model(CHICKEN, modelShader);
	cactusBaseModel.push_back(new Model(cactus_cylinder00, modelShader));
	cactusBaseModel.push_back(new Model(cactus_round_double00, modelShader));
	cactusBaseModel.push_back(new Model(cactus_tentacle00, modelShader));
	cactusBaseModel.push_back(new Model(cactus_tentacle01, modelShader));

	// create objects
	mainCharacter = new Object(foxModel);
	terrain = new Object(new Model(map_name, modelShader));
	cactusObject.push_back(new Object(new Model(cactus_cylinder_flower00, modelShader)));
	cactusObject.push_back(new Object(new Model(cactus_cylinder_flower01, modelShader)));
	cactusObject.push_back(new Object(new Model(cactus_round_double_flower00, modelShader)));
	cactusObject.push_back(new Object(new Model(cactus_tentacle_flower00, modelShader)));
	cactusObject.push_back(new Object(new Model(cactus_tentacle_flower01, modelShader)));

	// initialize camera
	camera = new Camera(mainCharacter);

	restart();

	return true;
}

void restart(void) {
	
	// reset application settings
	printf("---\tapplication\n");
	app->Reset();

	// reset camera
	printf("---\tcamera\n");
	camera->Reset();

	// main character
	printf("---\tmain character models\n");
	mainCharacter->ReloadModel(foxModel);
	isChick = false;
	mainCharacter->Reset(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.01f, 0.0f, 0.1f);

	printf("---\tmap model\n");
	terrain->Reset(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.01f, 0.0f, 0.1f);

	int id = 0;
	printf("---\tcactus models\n");
	for (ObjectsCollection::iterator it = cactusObject.begin(); it != cactusObject.end(); ++it) {
		Object* cactus = (Object*)(*it);
		/*float angle = (float)(90.0 * (rand() / (double)RAND_MAX) - 90.0);*/
		cactus->Reset(posCactus[id], glm::vec3(0.0f, 1.0f, 0.0f), sizeCactus[id], angleCactus[id], 0.01f);
		//cactus->SetRandomPosition(mainCharacter);
		id++;
	}
}

void displayCb(void) {
	// clear frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderScene();

	glutSwapBuffers();
}

void reshapeCb(int width, int height) {
	// update window size
	app->UpdateWindow(width, height);

	// setup viewport transformation
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void timerCb(int id) {
	// update game time
	app->UpdateTime();

	updateScene();

	for (ObjectsCollection::iterator it = cactusObject.begin(); it != cactusObject.end(); ++it) {
		Object* cactus = (Object*)(*it);
		if (app->CheckCollisions(mainCharacter, cactus)) {
			Explosion* explosion = new Explosion(explosionShader);
			explosion->Init(app->elapsedTime, 0.05f, cactus->GetPosition());
			explosions.push_back(explosion);
		}
	}

	glutTimerFunc(REFRESH_INTERVAL, timerCb, 0);

	glutPostRedisplay();
}

void passiveMotionCb(int x, int y) {
	if (app->firstMouse) {
		x = app->windowWidth / 2;
		y = app->windowHeight / 2;
		app->firstMouse = false;
	}

	float dx = x - app->windowWidth / 2;
	float dy = app->windowHeight / 2 - y;

	camera->Rotate(dx, dy);

	glutWarpPointer(app->windowWidth / 2, app->windowHeight / 2);

	glutPostRedisplay();
}

void motionCb(int x, int y) {
	float dx = x - app->lastX;
	float dy = y - app->lastY;
	app->lastY = y;
	app->lastX = x;

	if (app->mouse[MIDDLE_MOUSE_BUTTON])
		camera->Rotate(dx, dy);
}

void mouseCb(int button, int state, int x, int y) {
	app->lastX = x;
	app->lastY = y;

	if (menu && state == GLUT_DOWN) {
		menu = false;
	}

	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		// stores value from the stencil buffer (byte)
		unsigned char objectID = 0;

		// read value from the stencil buffer for one pixel under the mouse cursor
		// recalculate y, as glut has origin in upper left corner
		glReadPixels(x, WINDOW_HEIGHT - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objectID);
		glDisable(GL_STENCIL_TEST);

		ObjectsCollection::iterator it = cactusObject.begin();
		ObjectsCollection::iterator it2 = cactusBaseModel.begin();

		switch (objectID) {
			case 1: {
				std::advance(it, 0);
				std::advance(it2, 0);
				Object* cactus = (Object*)(*it);
				cactus->ReloadModel((Model*)(*it2));
				break;
			}
			case 2: {
				std::advance(it, 1);
				std::advance(it2, 0);
				Object* cactus = (Object*)(*it);
				cactus->ReloadModel((Model*)(*it2));
				break;
			}
			case 3: {
				std::advance(it, 2);
				std::advance(it2, 1);
				Object* cactus = (Object*)(*it);
				cactus->ReloadModel((Model*)(*it2));
				break;
			}
			case 4: {
				std::advance(it, 3);
				std::advance(it2, 2);
				Object* cactus = (Object*)(*it);
				cactus->ReloadModel((Model*)(*it2));
				break;
			}
			case 5: {
				std::advance(it, 4);
				std::advance(it2, 3);
				Object* cactus = (Object*)(*it);
				cactus->ReloadModel((Model*)(*it2));
				break;
			}
			case 6: {
				if (isChick){
					mainCharacter->ReloadModel(foxModel);
					isChick = false;
				}
				else {
					mainCharacter->ReloadModel(chickenModel);
					isChick = true;
				}
				Explosion* explosion = new Explosion(explosionShader);
				explosion->Init(app->elapsedTime, 0.1f, mainCharacter->GetPosition());
				explosions.push_back(explosion);
				break;
			}
		}		
	}

	switch (button) {
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
			app->mouse[MIDDLE_MOUSE_BUTTON] = true;
		else
			app->mouse[MIDDLE_MOUSE_BUTTON] = false;
		break;
	}
}

void mouseWheelCb(int wheel, int dir, int x, int y) {
	camera->Zoom((float)dir);
}

void keyboardCb(unsigned char key, int x, int y) {
	if (!menu) {
		switch (key) {
		case 27:	// escape
			glutLeaveMainLoop();
			break;
		case 'r':
			printf("---\tRESTARTING THE SCENE\t---\n");
			break;
		case 'w': 
			app->keyboard[KEY_UP] = true;
			break;
		case 's':
			app->keyboard[KEY_DOWN] = true;
			break;
		case 'a':
			if (camera->FirstPerson() && !camera->Locked())
				camera->Rotate(-10.0f, 0.0f);
			else
				app->keyboard[KEY_LEFT] = true;
			break;
		case 'd':
			if (camera->FirstPerson() && !camera->Locked())
				camera->Rotate(10.0f, 0.0f);
			else
				app->keyboard[KEY_RIGHT] = true;
			break;
		case 'c':
			camera->SwitchMode();
			glutMotionFunc(NULL);
			glutMouseWheelFunc(NULL);
			glutPassiveMotionFunc(NULL);
			if (!camera->Locked())
			{
				if (camera->FirstPerson())
					glutPassiveMotionFunc(passiveMotionCb);
				else {
					glutMotionFunc(motionCb);
					glutMouseWheelFunc(mouseWheelCb);
				}
			}
			break;
		case 'p':
			passive = !passive;
			break;
		case 'l':
			spotOn = !spotOn;
			if (spotOn) {
				spotDiff = glm::vec3(1.0f, 1.0f, 1.0f);
				spotSpec = glm::vec3(0.2f, 0.2f, 0.2f);
			}
			else {
				spotDiff = glm::vec3(0.01f, 0.01f, 0.01f);
				spotSpec = glm::vec3(0.0f, 0.0f, 0.0f);
			}
			break;
		case 'f':
			fogOn = !fogOn;
			if (fogOn) {
				skybox->SetFog(1.0f);
				modelShader->SetFog(1.0f);
			}
			else {
				skybox->SetFog(0.0f);
				modelShader->SetFog(0.0f);
			}
			break;
		}
	}
}

void keyboardUpCb(unsigned char key, int x, int y) {
	if (!menu) {
		switch (key) {
		case 'r':
			restart();
			break;
		case 'w':
			app->keyboard[KEY_UP] = false;
			break;
		case 's':
			app->keyboard[KEY_DOWN] = false;
			break;
		case 'a':
			app->keyboard[KEY_LEFT] = false;
			break;
		case 'd':
			app->keyboard[KEY_RIGHT] = false;
			break;
		}
	}
}

void specialKeyboardCb(int key, int x, int y) {
	if (!menu) {
		switch (key) {
		case GLUT_KEY_UP:
			app->keyboard[KEY_UP] = true;
			break;
		case GLUT_KEY_DOWN:
			app->keyboard[KEY_DOWN] = true;
			break;
		case GLUT_KEY_LEFT:
			if (camera->FirstPerson() && !camera->Locked())
				camera->Rotate(-10.0f, 0.0f);
			else
				app->keyboard[KEY_LEFT] = true;
			break;
		case GLUT_KEY_RIGHT:
			if (camera->FirstPerson() && !camera->Locked())
				camera->Rotate(10.0f, 0.0f);
			else
				app->keyboard[KEY_RIGHT] = true;
			break;
		case GLUT_KEY_CTRL_L:
			if (camera->Lock()) {
				printf("--- camera: LOCKED ---\n");
				glutPassiveMotionFunc(NULL);
				glutMotionFunc(NULL);
				glutMouseWheelFunc(NULL);
			}
			else {
				printf("--- camera: UNLOCKED ---\n");
				if (camera->FirstPerson())
					glutPassiveMotionFunc(passiveMotionCb);
				else {
					glutMotionFunc(motionCb);
					glutMouseWheelFunc(mouseWheelCb);
				}
			}
			break;
		case GLUT_KEY_ALT_L:
			dirOn = !dirOn;
			if (!dirOn)
				dirDiff = glm::vec3(0.15f, 0.15f, 0.15f);
			else 
				dirDiff = glm::vec3(0.8f, 0.8f, 0.8f);
			break;
		}

	}
}

void specialKeyboardUpCb(int key, int x, int y) {
	if (!menu) {
		switch (key) {
		case GLUT_KEY_UP:
			app->keyboard[KEY_UP] = false;
			break;
		case GLUT_KEY_DOWN:
			app->keyboard[KEY_DOWN] = false;
			break;
		case GLUT_KEY_LEFT:
			app->keyboard[KEY_LEFT] = false;
			break;
		case GLUT_KEY_RIGHT:
			app->keyboard[KEY_RIGHT] = false;
			break;
		}
	}
}

int main(int argc, char** argv) {
	// initialize GLUT and OpenGL context
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	// window
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	// window callbacks
	glutDisplayFunc(displayCb);
	glutReshapeFunc(reshapeCb);

	// keyboard callbacks
	glutKeyboardFunc(keyboardCb);
	glutKeyboardUpFunc(keyboardUpCb);

	// special keyboard callbacks
	glutSpecialFunc(specialKeyboardCb);
	glutSpecialUpFunc(specialKeyboardUpCb);
	
	// mouse callback
	glutMouseFunc(mouseCb);						

	// timer callback
	glutTimerFunc(REFRESH_INTERVAL, timerCb, 0);

	// initialize PGR framework
	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	// initialize application
	if (init())
		printf("--- APPLICATION INITIALIZED SUCCESSFULLY ---\n");
	else
		pgr::dieWithError("init() failed");
	glutMainLoop();

	return 0;
}
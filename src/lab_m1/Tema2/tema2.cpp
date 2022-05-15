#include "lab_m1/Tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
	bgSound = createIrrKlangDevice();
	shootingSound = createIrrKlangDevice();
	enemyDamageSound = createIrrKlangDevice();
	enemyDieSound = createIrrKlangDevice();
	playerDamageSound = createIrrKlangDevice();
	playerDieSound = createIrrKlangDevice();
	pickupSound = createIrrKlangDevice();
	finishMapSound = createIrrKlangDevice();

	string pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "DOOM.mp3");
	char* path = &pathString[0];
	bgSound->play2D(path, true);
	bgSound->setSoundVolume(0.8f);

	pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "enemy_damage.wav");
	pathEnemyDamageSound = new char[pathString.size() + 1];
	strcpy(pathEnemyDamageSound, pathString.c_str());

	pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "enemy_death.wav");
	pathEnemyDieSound = new char[pathString.size() + 1];
	strcpy(pathEnemyDieSound, pathString.c_str());

	pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "shoot.wav");
	pathShootingSound = new char[pathString.size() + 1];
	strcpy(pathShootingSound, pathString.c_str());

	pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "player_hurt.mp3");
	pathPlayerDamageSound = new char[pathString.size() + 1];
	strcpy(pathPlayerDamageSound, pathString.c_str());

	pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "player_death.wav");
	pathPlayerDieSound = new char[pathString.size() + 1];
	strcpy(pathPlayerDieSound, pathString.c_str());

	pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "powerup.wav");
	pathPickupSound = new char[pathString.size() + 1];
	strcpy(pathPickupSound, pathString.c_str());

	pathString = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "sounds", "finished_map.mp3");
	pathFinishMapSound = new char[pathString.size() + 1];
	strcpy(pathFinishMapSound, pathString.c_str());

	{
		Mesh* mesh = object3D::CreateCube("cube", 1.f, glm::vec3(0.5f));
		AddMeshToList(mesh);
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	camera = new world_camera::Camera();
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

	ResetGame();

	{
		for (int i = 0; i < 500; ++i) {
			int x = rand() % (int)(widthMaze * widthCorridor * 5) - widthMaze * widthCorridor * 2;
			int z = rand() % (int)(heightMaze * widthCorridor * 5) - heightMaze * widthCorridor * 2;
			int y = rand() % 50 + 10;
			cloudsPos.emplace_back(glm::vec3(x, y, z));
			x = rand() % 5 + 1;
			z = rand() % 5 + 1;
			y = rand() % 3 + 1;
			cloudsSize.emplace_back(glm::vec3(x, y, z));
		}
	}

	{
		Shader* shader = new Shader("MyShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("MyShaderEnemy");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderEnemy.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderEnemy.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("MyShaderPlayer");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderPlayer.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderPlayer.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	window->SetPointerPosition(1280 / 2, 720 / 2);
}

void Tema2::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.470f, 0.054f, 0.050f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	GLint modelLoc = glGetUniformLocation(shader->GetProgramID(), "Model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	GLint viewLoc = glGetUniformLocation(shader->GetProgramID(), "View");

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint projectionLoc = glGetUniformLocation(shader->GetProgramID(), "Projection");

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3f(glGetUniformLocation(shader->program, "Color"), color.x, color.y, color.z);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSimpleMeshEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int health, bool isDying)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	GLint modelLoc = glGetUniformLocation(shader->GetProgramID(), "Model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	GLint viewLoc = glGetUniformLocation(shader->GetProgramID(), "View");

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint projectionLoc = glGetUniformLocation(shader->GetProgramID(), "Projection");

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3f(glGetUniformLocation(shader->program, "Color"), color.x, color.y, color.z);
	glUniform1i(glGetUniformLocation(shader->program, "Health"), health);
	glUniform1i(glGetUniformLocation(shader->program, "isDying"), isDying);
	glUniform1f(glGetUniformLocation(shader->program, "Time"), (float)Engine::GetElapsedTime());

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSimpleMeshPlayer(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, bool isDamaged)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	GLint modelLoc = glGetUniformLocation(shader->GetProgramID(), "Model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	GLint viewLoc = glGetUniformLocation(shader->GetProgramID(), "View");

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint projectionLoc = glGetUniformLocation(shader->GetProgramID(), "Projection");

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform3f(glGetUniformLocation(shader->program, "Color"), color.x, color.y, color.z);
	glUniform1i(glGetUniformLocation(shader->program, "isDamaged"), isDamaged);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

bool CircleToBoxCollision(float xLow, float xHigh, float yLow, float yHigh, float zLow, float zHigh, glm::vec3 circleCenter, float radius) {
	float x = std::max(xLow, std::min(circleCenter.x, xHigh));
	float y = std::max(yLow, std::min(circleCenter.y, yHigh));
	float z = std::max(zLow, std::min(circleCenter.z, zHigh));

	float dist = glm::sqrt((x - circleCenter.x) * (x - circleCenter.x) +
		(y - circleCenter.y) * (y - circleCenter.y) +
		(z - circleCenter.z) * (z - circleCenter.z));

	return dist < radius;
}

void Tema2::ResetGame() {
	posPlayerBody = posPlayerBodyOg;
	isFirstPerson = false;
	playerBodyAngle = 0.f;
	playerHeadAngle = 0.f;
	camera->SetThirdPerson(glm::vec3(posPlayerBodyOg.x + 0.35f / 2, posPlayerBodyOg.y, posPlayerBodyOg.z + 0.2f / 2));

	forwardPlayer = glm::normalize(glm::vec3(posPlayerBodyOg.x, posPlayerBodyOg.y, posPlayerBodyOg.z + 1.f) - posPlayerBodyOg);
	rightPlayer = glm::cross(forwardPlayer, glm::vec3(0, 1, 0));
	upPlayer = glm::cross(rightPlayer, forwardPlayer);

	maze = new Maze::Backtracking(widthMaze, heightMaze);
	maze->create();

	mazeWallsHorizontal.clear();
	mazeWallsVertical.clear();
	enemies.clear();
	enemysPos.clear();
	enemiesOgPos.clear();
	bullets.clear();
	powerups.clear();
	powerupsType.clear();

	// North
	for (int i = 0; i < widthCorridor * widthMaze; ++i) {
		mazeWallsHorizontal.emplace_back(glm::vec3(i, 0, -widthWallMaze / 2));
	}
	// West
	for (int i = 0; i < widthCorridor * heightMaze; ++i) {
		mazeWallsVertical.emplace_back(glm::vec3(widthWallMaze / 2, 0, i));
	}

	for (int z = 0; z < heightMaze; ++z) {
		for (int x = 0; x < widthMaze; ++x) {
			// South
			if (maze->cells[z * widthMaze + x] & Maze::CELL_WALL_SOUTH) {
				for (int i = 0; i < widthCorridor; ++i) {
					mazeWallsHorizontal.emplace_back(glm::vec3(i + x * widthCorridor, 0, z * widthCorridor + widthCorridor - widthWallMaze / 2));
				}
			}
			// East
			if (maze->cells[z * widthMaze + x] & Maze::CELL_WALL_EAST) {
				for (int i = 0; i < widthCorridor; ++i) {
					mazeWallsVertical.emplace_back(glm::vec3(x * widthCorridor + widthCorridor + widthWallMaze / 2, 0, i + z * widthCorridor));
				}
			}
		}
	}

	mazeWallsVertical.erase(mazeWallsVertical.end() - 1);

	maze->~Backtracking();

	for (int i = 0; i < heightMaze * widthMaze / 4; ++i) {
		int x = rand() % widthMaze + 0;
		int z = rand() % heightMaze + 0;
		if (x == 0 && z == 0) {
			int random = rand() % 100;
			if (random > 50) {
				++x;
			}
			else {
				++z;
			}
		}
		while (find(enemysPos.begin(), enemysPos.end(), glm::vec3(x * widthCorridor + widthWallMaze, 1.f, z * widthCorridor + widthWallMaze)) != enemysPos.end()) {
			x = rand() % widthMaze + 0;
			z = rand() % heightMaze + 0;
			if (x == 0 && z == 0) {
				int random = rand() % 100;
				if (random > 50) {
					++x;
				}
				else {
					++z;
				}
			}
		}
		enemysPos.emplace_back(glm::vec3(x * widthCorridor + widthWallMaze, 1.f, z * widthCorridor + widthWallMaze));
		enemiesOgPos.emplace_back(glm::vec3(x * widthCorridor + widthWallMaze, 1.f, z * widthCorridor + widthWallMaze));
		enemies.emplace_back(EnemyStats());
	}

	for (int i = 0; i < heightMaze * widthMaze / 5; ++i) {
		int x = rand() % widthMaze + 1;
		int z = rand() % heightMaze + 1;
		while (find(powerups.begin(), powerups.end(), glm::vec3(x * widthCorridor + widthWallMaze, 0.1f, z * widthCorridor + widthWallMaze)) != powerups.end()) {
			x = rand() % widthMaze + 1;
			z = rand() % heightMaze + 1;
		}
		int type = rand() % 3 + 0;
		powerups.emplace_back(glm::vec3(x * widthCorridor + widthWallMaze, 0.1f, z * widthCorridor + widthWallMaze));
		powerupsType.emplace_back(type);
	}

	lastHit = Engine::GetElapsedTime();
	startTime = Engine::GetElapsedTime();
}

void Tema2::ResetPowerup() {
		if (playerPowerupType > -1) {
			if (Engine::GetElapsedTime() - powerupPickedTime > maxPowerupTime) {
				if (playerPowerupType == 1) {
					playerSpeed -= 0.5f;
					membersSpeed -= 0.5f;
				}
				playerPowerupType = -1;
			}
		}
}

void Tema2::PickupPowerup() {
	if (playerPowerupType == -1) {
		list<glm::vec3>::iterator pos = powerups.begin();
		list<int>::iterator type = powerupsType.begin();
		glm::vec3 posPlayer = glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.1f);
		while (pos != powerups.end()) {
			if (CircleToBoxCollision(pos->x - widthCorridor / 2, pos->x - widthCorridor / 2 + 0.5f, pos->y, pos->y + 0.7f, pos->z - widthCorridor / 2, pos->z - widthCorridor / 2 + 0.5f, posPlayer, 0.25f)) {
				pickupSound->play2D(pathPickupSound, false);
				playerPowerupType = *type;
				pos = powerups.erase(pos);
				type = powerupsType.erase(type);
				if (playerPowerupType == 0) {
					health += 0.2f;
					if (health > 1.45f) {
						health = 1.45f;
					}
				}
				else if (playerPowerupType == 1) {
					playerSpeed += 0.5f;
					membersSpeed += 0.5f;
				}
				else {
					numberOfbullets += 2;
					if (numberOfbullets > 5) {
						numberOfbullets = 5;
					}
				}
				powerupPickedTime = Engine::GetElapsedTime();
				break;
			}
			++pos;
			++type;
		}
	}
}

void Tema2::RenderPowerup() {
	list<glm::vec3>::iterator pos = powerups.begin();
	list<int>::iterator type = powerupsType.begin();
	while (pos != powerups.end()) {
		glm::mat4 modelMatrix = glm::mat4(1);
		glm::vec3 posVec = glm::vec3(pos->x, pos->y, pos->z);
		modelMatrix = glm::translate(modelMatrix, posVec);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-widthCorridor / 2, 0.f, -widthCorridor / 2));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f / 2));
		modelMatrix = glm::rotate(modelMatrix, (float)glm::abs(glm::sin(Engine::GetElapsedTime())), glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.329f, 0.215f, 0.168f));

		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f / 2, 0.5f / 2, 1.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 0.5f, 0.1f));

		if (*type == 0) {
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.094f, 0.686f, 0.137f));
		}
		else if (*type == 1) {
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.094f, 0.4f, 0.549f));
		}
		else {
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.3f));
		}

		++pos;
		++type;
	}
}

void Tema2::RenderClouds() {
	for (int i = 0; i < cloudsPos.size(); ++i) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, cloudsPos[i]);
		modelMatrix = glm::scale(modelMatrix, cloudsSize[i]);
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.1f));
	}
}

void Tema2::RenderMaze() {
	glm::mat4 modelMatrix = glm::mat4(1);
	// North & South Walls
	for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, mazeWallsHorizontal[i]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 2.5f, widthWallMaze));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.2f));
	}

	// West & East Walls
	for (int i = 0; i < mazeWallsVertical.size(); ++i) {
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, mazeWallsVertical[i]);
		modelMatrix = glm::rotate(modelMatrix, RADIANS(-90.0f), glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 2.5f, widthWallMaze));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.2f));
	}

	// Render floors
	modelMatrix = glm::mat4(1);
	for (int i = 0; i < heightMaze * widthCorridor; i += widthCorridor) {
		for (int j = 0; j < widthMaze * widthCorridor; j += widthCorridor) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(j, -0.1f, i));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(widthCorridor, 0.1f, widthCorridor));
			if ((i + j) % 10 < 3) {
				RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.756, 0.231, 0.023));
			}
			else {
				RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.301f, 0.137f, 0.137f));
			}
		}
	}
}

void Tema2::AnimateEnemies(float deltaTimeSeconds) {
	float offset = widthCorridor - 4 * widthWallMaze - 0.8f;
	list<glm::vec3>::iterator itrPos = enemysPos.begin();
	list<glm::vec3>::iterator itrPosOg = enemiesOgPos.begin();
	while (itrPos != enemysPos.end()) {
		if (enemyState == 0) {
			itrPos->z += (deltaTimeSeconds * 0.5f);
			if (itrPos->x == itrPosOg->x && itrPos->z >= itrPosOg->z + offset) {
				enemyState = 1;
				itrPos->z = itrPosOg->z + offset;
				enemyRotation = glm::radians(90.f);
			}
		}
		else if (enemyState == 1) {
			itrPos->x += (deltaTimeSeconds * 0.5f);
			if (itrPos->x >= itrPosOg->x + offset && itrPos->z == itrPosOg->z + offset) {
				enemyState = 2;
				itrPos->x = itrPosOg->x + offset;
				enemyRotation = glm::radians(180.f);
			}
		}
		else if (enemyState == 2) {
			itrPos->z -= (deltaTimeSeconds * 0.5f);
			if (itrPos->x == itrPosOg->x + offset && itrPos->z <= itrPosOg->z) {
				enemyState = 3;
				itrPos->z = itrPosOg->z;
				enemyRotation = glm::radians(270.f);
			}
		}
		else {
			itrPos->x -= (deltaTimeSeconds * 0.5f);
			if (itrPos->x <= itrPosOg->x && itrPos->z == itrPosOg->z) {
				enemyState = 0;
				itrPos->x = itrPosOg->x;
				enemyRotation = 0.f;
			}
		}
		++itrPos;
		++itrPosOg;
	}
}

void Tema2::UpdatePlayerHealth() {
	glm::vec3 pos = glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.1f);
	list<glm::vec3>::iterator itrPos = enemysPos.begin();
	while (itrPos != enemysPos.end()) {
		if (Engine::GetElapsedTime() - lastHit > damageTimer && CircleToBoxCollision(itrPos->x, itrPos->x + enemyLength, itrPos->y, itrPos->y + enemyLength, itrPos->z, itrPos->z + enemyLength, pos, 0.25f)) {
			health -= 0.29f;
			lastHit = Engine::GetElapsedTime();
			isDamagedOg = true;
			if (health < 0) {
				health = 0;
				playerDieSound->play2D(pathPlayerDieSound, false);
			}
			else {
				playerDamageSound->play2D(pathPlayerDamageSound, false);
			}
			break;
		}
		++itrPos;
	}
}

void Tema2::AnimateAndRenderBullets(float deltaTimeSeconds) {
	list<Bullet>::iterator itr = bullets.begin();
	while (itr != bullets.end())
	{
		if (Engine::GetElapsedTime() - itr->spawnTime > bulletMaxTime) {
			itr = bullets.erase(itr);
		}
		else {
			bool collision = false;
			list<glm::vec3>::iterator itrPos = enemysPos.begin();
			list<EnemyStats>::iterator itrStats = enemies.begin();
			while (itrPos != enemysPos.end()) {
				if (CircleToBoxCollision(itrPos->x, itrPos->x + enemyLength, itrPos->y, itrPos->y + enemyLength, itrPos->z, itrPos->z + enemyLength, itr->pos, 0.015f)) {
					collision = true;
					itr = bullets.erase(itr);
					if (itrStats->health != 0) {
						itrStats->health -= 20;
						if (itrStats->health == 0) {
							itrStats->deathTime = Engine::GetElapsedTime();
							enemyDieSound->play2D(pathEnemyDieSound, false);
						}
						else {
							enemyDamageSound->play2D(pathEnemyDamageSound, false);
						}
					}
					break;
				}
				++itrPos;
				++itrStats;
			}

			if (!collision) {
				for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
					if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, itr->pos, 0.03f)) {
						collision = true;
						itr = bullets.erase(itr);
						break;
					}
				}
				if (!collision) {
					for (int i = 0; i < mazeWallsVertical.size(); ++i) {
						if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, itr->pos, 0.03f)) {
							collision = true;
							itr = bullets.erase(itr);
							break;
						}
					}
				}
			}

			if (!collision) {
				itr->pos = itr->pos + (itr->forward * (bulletSpeed * deltaTimeSeconds));
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, itr->pos);
				modelMatrix = glm::rotate(modelMatrix, playerHeadAngle, glm::vec3(0, 1, 0));
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.01f / 2));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
				RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.9f));
				++itr;
			}
		}
	}
}

void Tema2::RenderEnemies() {
	glm::mat4 modelMatrix;
	list<glm::vec3>::iterator itrPos = enemysPos.begin();
	list<EnemyStats>::iterator itrStats = enemies.begin();
	list<glm::vec3>::iterator itrPosOg = enemiesOgPos.begin();
	while (itrPos != enemysPos.end()) {
		if (itrStats->deathTime != 0.f && Engine::GetElapsedTime() - itrStats->deathTime > maxDeathAnimation) {
			itrPos = enemysPos.erase(itrPos);
			itrStats = enemies.erase(itrStats);
			itrPosOg = enemiesOgPos.erase(itrPosOg);
			continue;
		}

		float bounce = 0.3f + 0.6f * glm::abs(glm::cos(Engine::GetElapsedTime()));
		itrPos->y = bounce;
		glm::vec3 pos = glm::vec3(itrPos->x, itrPos->y, itrPos->z);
		bool isDying = false;
		if (itrStats->health == 0) {
			isDying = true;
		}

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyLength / 2));
		modelMatrix = glm::rotate(modelMatrix, enemyRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-enemyLength / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(enemyLength));
		RenderSimpleMeshEnemy(meshes["cube"], shaders["MyShaderEnemy"], modelMatrix, glm::vec3(0.403f, 0.078f, 0.078f), itrStats->health, isDying);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyLength / 2));
		modelMatrix = glm::rotate(modelMatrix, enemyRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.2f, enemyLength / 2, 0.45f / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.15f, 0.1f));
		RenderSimpleMeshEnemy(meshes["cube"], shaders["MyShaderEnemy"], modelMatrix, glm::vec3(0.266f, 0.113f, 0.113f), itrStats->health, isDying);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyLength / 2));
		modelMatrix = glm::rotate(modelMatrix, enemyRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3f, enemyLength / 2, 0.45f / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.15f, 0.1f));
		RenderSimpleMeshEnemy(meshes["cube"], shaders["MyShaderEnemy"], modelMatrix, glm::vec3(0.266f, 0.113f, 0.113f), itrStats->health, isDying);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyLength / 2));
		modelMatrix = glm::rotate(modelMatrix, enemyRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.15f, 0, enemyLength / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.05f));
		RenderSimpleMeshEnemy(meshes["cube"], shaders["MyShaderEnemy"], modelMatrix, glm::vec3(0.266f, 0.113f, 0.113f), itrStats->health, isDying);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyLength / 2));
		modelMatrix = glm::rotate(modelMatrix, enemyRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.02f, 0.05f, enemyLength / 2 + 0.06f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.04f, 0.1f, 0.01f));
		RenderSimpleMeshEnemy(meshes["cube"], shaders["MyShaderEnemy"], modelMatrix, glm::vec3(0.1f), itrStats->health, isDying);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyLength / 2));
		modelMatrix = glm::rotate(modelMatrix, enemyRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.075f, 0.05f, enemyLength / 2 + 0.05f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 0.01f));
		RenderSimpleMeshEnemy(meshes["cube"], shaders["MyShaderEnemy"], modelMatrix, glm::vec3(0.8f), itrStats->health, isDying);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemyLength / 2));
		modelMatrix = glm::rotate(modelMatrix, enemyRotation, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.2f, -0.2f, enemyLength / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.05f, 0.01f));
		RenderSimpleMeshEnemy(meshes["cube"], shaders["MyShaderEnemy"], modelMatrix, glm::vec3(0.2f), itrStats->health, isDying);

		++itrPos;
		++itrStats;
		++itrPosOg;
	}
}

void Tema2::RenderDebug() {
	if (isDebugMode) {
		if (!isFirstPerson) {
			glm::vec3 pos = glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.1f);
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, pos);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.55f));
			RenderSimpleMesh(meshes["sphere"], shaders["MyShader"], modelMatrix, glm::vec3(0.21f));
		}

		list<Bullet>::iterator itr = bullets.begin();
		while (itr != bullets.end())
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(itr->pos.x, itr->pos.y, itr->pos.z));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.015f));
			RenderSimpleMesh(meshes["sphere"], shaders["MyShader"], modelMatrix, glm::vec3(0.21f));
			++itr;
		}
	}
}

void Tema2::CheckGameState() {
	if (health <= 0.001f) {
		cout << "You've lost! Better luck next time, Slayer...\n";
		window->Close();
	}

	if (posPlayerBody.x > widthMaze * widthCorridor) {
		cout << "Rip and Tear!\n\n\nAgain!\n";

		finishMapSound->play2D(pathFinishMapSound, false);
		ResetGame();
	}

	if (Engine::GetElapsedTime() - startTime > maxTimeAllowed) {
		cout << "Could not save the world in time, Slayer...\n";
		window->Close();
	}
	else {
		timeBarLength = 0.95f - (Engine::GetElapsedTime() - startTime) * 0.95f / maxTimeAllowed;
	}
}

void Tema2::Update(float deltaTimeSeconds)
{
	CheckGameState();

	RenderClouds();

	RenderMaze();

	ResetPowerup();
	PickupPowerup();
	RenderPowerup();

	AnimateEnemies(deltaTimeSeconds);

	AnimateAndRenderBullets(deltaTimeSeconds);

	UpdatePlayerHealth();

	RenderEnemies();

	RenderDebug();

	RenderPlayer();
	RenderHUD();
}


void Tema2::FrameEnd()
{
}

void Tema2::RenderHUD() {
	{
		glm::mat4 lastProjectionMatrix = projectionMatrix;
		world_camera::Camera* lastCamera = camera;
		projectionMatrix = glm::ortho(left, right, bottom, top, 0.01f, 200.0f);
		camera = new world_camera::Camera();

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-5., 6.5, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 0.5f, 0.f));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.4f));

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.975f, 6.5f + 0.025f, 0.01f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(health, 0.45f, 0.f));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.058f, 0.301f, 0.117f));

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 6.5, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 0.5f, 0.f));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.6f));

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-timeBarLength / 2, 6.5f + 0.025f, 0.01f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(timeBarLength, 0.45f, 0.f));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.05f));

		if (isFirstPerson) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.07f, 2.f - 0.025f / 2, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.14f, 0.025f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.92f));

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.025f / 1.25f / 2, 2.f - 0.25f / 2 + 0.005f, 0.1f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.025f / 1.25f, 0.25f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.92f));
		}

		if (playerPowerupType == 0) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.75f - 0.15f, 6.1f - 0.025f / 2, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.05f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(glm::abs(glm::sin(Engine::GetElapsedTime()))));

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.75f - 0.05f / 1.25f / 2, 6.1f - 0.4f / 2 + 0.005f, 0.1f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f / 1.25f, 0.4f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(glm::abs(glm::sin(Engine::GetElapsedTime()))));
		}
		else if (playerPowerupType == 1) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.5f - 0.15f, 6.f - 0.025f / 2 + 0.15f, 0.f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(-225.f), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.05f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(glm::abs(glm::sin(Engine::GetElapsedTime()))));

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.5f - 0.15f, 6.f - 0.025f / 2 + 0.15f, 0.f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(222.f), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.05f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(glm::abs(glm::sin(Engine::GetElapsedTime()))));

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.3f - 0.15f, 6.f - 0.025f / 2 + 0.15f, 0.f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(-225.f), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.05f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(glm::abs(glm::sin(Engine::GetElapsedTime() - 0.1f))));

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-4.3f - 0.15f, 6.f - 0.025f / 2 + 0.15f, 0.f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(222.f), glm::vec3(0, 0, 1));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.05f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(glm::abs(glm::sin(Engine::GetElapsedTime() - 0.1f))));
		}

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(3.5, 6.5, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 0.5f, 0.f));
		RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.8f));

		for (int i = 0; i < numberOfbullets; ++i) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(5.f - 0.28f * i - 0.31f, 6.5f + 0.04f, 0.01f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.4f, 0.f));
			RenderSimpleMesh(meshes["cube"], shaders["MyShader"], modelMatrix, glm::vec3(0.2f));
		}

		projectionMatrix = lastProjectionMatrix;
		camera = lastCamera;
	}
}

void Tema2::RenderPlayer() {
	{
		if (isDamagedOg) {
			float newDamagedTimer = (Engine::GetElapsedTime() - lastHit - 0) / (damageTimer - 0) * (damageTimer / 3.f - 0) + 0.f;
			float fractionalPart = (Engine::GetElapsedTime() - lastHit) - (int)(Engine::GetElapsedTime() - lastHit);
			if (fractionalPart < damageTimer / 9.f) {
				isDamaged = true;
			}
			else {
				isDamaged = false;
			}
		}

		if (Engine::GetElapsedTime() - lastHit > damageTimer) {
			isDamagedOg = false;
			isDamaged = false;
		}

		// Upper Chest
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, posPlayerBody);
		if (!isFirstPerson) {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2, -0.12f / 2, -0.2f / 2));
		}
		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2, -0.12f / 2, -0.2f / 2 - 0.1f));
		}
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.12f, 0.2f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.215f, 0.317f, 0.149f), isDamaged);
		glm::mat4 chestModelMatrix = modelMatrix;
		modelMatrix = glm::mat4(1);

		if (!isFirstPerson) {
			// Helmet
			modelMatrix = glm::translate(modelMatrix, glm::vec3(posPlayerBody.x + 0.35f * 0.2f, posPlayerBody.y + 0.12f * 1.f, posPlayerBody.z)); // AICI DE UMBLAT SA L FACI SA FIE SPATIU PANA LA PIEPT
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f * 0.6f / 2.f, 0.12f * 1.5f / 2.f, 0.2f * 0.9f / 2.f + 0.2 * 0.05f)); //AICI DE UMBLAT
			modelMatrix = glm::rotate(modelMatrix, playerHeadAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f * 0.6f / 2.f, -0.12f * 1.5f / 2.f, -0.2f * 0.9f / 2.f - 0.2 * 0.05f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f * 0.6f, 0.12f * 1.5f, 0.2f * 0.9f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.290f, 0.282f, 0.309f), isDamaged);

			glm::mat4 helmetModelMatrix = modelMatrix;

			// Left Ear
			modelMatrix = glm::translate(helmetModelMatrix, glm::vec3(-0.1f, 0.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.3f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.192f, 0.180f, 0.180f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f, 1.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.192f, 0.180f, 0.180f), isDamaged);

			// Right Ear
			modelMatrix = glm::translate(helmetModelMatrix, glm::vec3(1.f, 0.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.3f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.192f, 0.180f, 0.180f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 1.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.192f, 0.180f, 0.180f), isDamaged);

			// Top Vent
			modelMatrix = glm::translate(helmetModelMatrix, glm::vec3(0.3f, 1.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.1f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.223f, 0.215f, 0.235f), isDamaged);

			// Visor
			modelMatrix = glm::translate(helmetModelMatrix, glm::vec3(0.05f, 0.5f, 1.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 0.4f, 0.05f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.164f, 0.160f, 0.152f), isDamaged);

			glm::mat4 visorModelMatrix = modelMatrix;

			// Visor Left Eye
			modelMatrix = glm::translate(visorModelMatrix, glm::vec3(0.f, -0.3f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20f, 0.3f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.164f, 0.160f, 0.152f), isDamaged);

			modelMatrix = glm::translate(visorModelMatrix, glm::vec3(0.2f, -0.2f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.2f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.164f, 0.160f, 0.152f), isDamaged);

			// Visor Right Eye
			modelMatrix = glm::translate(visorModelMatrix, glm::vec3(0.8f, -0.3f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20f, 0.3f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.164f, 0.160f, 0.152f), isDamaged);

			modelMatrix = glm::translate(visorModelMatrix, glm::vec3(0.7f, -0.2f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.2f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.164f, 0.160f, 0.152f), isDamaged);

			// Breather
			modelMatrix = glm::translate(visorModelMatrix, glm::vec3(0.3f, -1.25f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 1.25f, 3.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.176f, 0.156f, 0.145f), isDamaged);

			// Breather Lines
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.15f, 0.15f, 1.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f, 0.1f, 0.1f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.1f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.05f, 2.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 0.9f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.1f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.05f, 2.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 0.9f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.1f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.05f, 2.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 0.9f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.1f), isDamaged);
		}

		// Lower Chest
		modelMatrix = glm::translate(chestModelMatrix, glm::vec3(0.025f, -0.6f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.95f, 0.6f, 0.9f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.152f, 0.270f, 0.121f), isDamaged);

		// Stomach
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.25f, -1.35f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.35f, 0.9f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.588f, 0.490f, 0.380f), isDamaged);

		glm::mat4 stomachModelMatrix = modelMatrix;

		// Left Ribs
		glm::mat4 leftRibsModelMatrix = glm::translate(stomachModelMatrix, glm::vec3(-0.35f, 0.6f, 0.f));
		leftRibsModelMatrix = glm::scale(leftRibsModelMatrix, glm::vec3(0.35f, 0.4f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], leftRibsModelMatrix, glm::vec3(0.321f, 0.380f, 0.247f), isDamaged);

		// Right Ribs
		glm::mat4 rightRibsModelMatrix = glm::translate(stomachModelMatrix, glm::vec3(1.f, 0.6f, 0.f));
		rightRibsModelMatrix = glm::scale(rightRibsModelMatrix, glm::vec3(0.35f, 0.4f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], rightRibsModelMatrix, glm::vec3(0.321f, 0.380f, 0.247f), isDamaged);

		// Pelvis
		modelMatrix = glm::translate(stomachModelMatrix, glm::vec3(-0.2f, -0.7f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.35f, 0.7f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.262f, 0.266f, 0.223f), isDamaged);

		glm::mat4 pelvisModelMatrix = modelMatrix;

		// Belt
		modelMatrix = glm::translate(pelvisModelMatrix, glm::vec3(0.3f, 0.f, 1.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 1.f, 0.1f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.231f, 0.235f, 0.184f), isDamaged);

		// Grenade
		modelMatrix = glm::translate(pelvisModelMatrix, glm::vec3(-0.1f, 0.f, 0.5f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.8f, 0.3f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.1f), isDamaged);

		if (!isFirstPerson) {
			// Shootgun
			modelMatrix = glm::translate(pelvisModelMatrix, glm::vec3(1.f, -2.5f, 0.5f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 2.5f, 0.2f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.1f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.f, -0.2f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 1.f, 0.2f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.25f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.f, -5.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1f, 1.f, 5.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.1f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 1.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 0.2f, 2.2f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.25f), isDamaged);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.4f, 1.f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 0.8f, 1.f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.3f), isDamaged);
		}
		else {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, posPlayerBody);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f + 0.1f, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerHeadAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, playerHeadAngleVertical, glm::vec3(1, 0, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.025f, -0.04f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f, 0.02f, 0.18f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.3f), isDamaged);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, posPlayerBody);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f + 0.1f, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerHeadAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, playerHeadAngleVertical, glm::vec3(1, 0, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.005f, -0.04, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f, 0.015f, 0.18f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.25f), isDamaged);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, posPlayerBody);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f + 0.1f, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerHeadAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, playerHeadAngleVertical, glm::vec3(1, 0, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.005f, -0.04f, 0.f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f, 0.02f, 0.18f));
			RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.3f), isDamaged);
		}

		// Pelvis Continuation
		glm::mat4 pelvisSModelMatrix = glm::translate(pelvisModelMatrix, glm::vec3(0.45f, -0.4f, 0.f));
		pelvisSModelMatrix = glm::scale(pelvisSModelMatrix, glm::vec3(0.1f, 0.4f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], pelvisSModelMatrix, glm::vec3(0.262f, 0.266f, 0.223f), isDamaged);

		modelMatrix = glm::mat4(1);
		// Left Leg
		modelMatrix = glm::translate(modelMatrix, posPlayerBody);
		if (!isFirstPerson) {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2, -0.12f / 2, -0.2f / 2));
		}
		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2, -0.12f / 2, -0.2f / 2 - 0.1f));
		}
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2 - 0.28f, 0.2f / 2));
		modelMatrix = glm::rotate(modelMatrix, leftLegAngle, glm::vec3(1, 0, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2 + 0.059f, -0.12f / 2 - 0.517f + 0.28f, -0.2f / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.28f, 0.162f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.349f, 0.380f, 0.301f), isDamaged);

		// Left Foot
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -0.28f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 0.28f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.156f, 0.168f, 0.133f), isDamaged);

		modelMatrix = glm::mat4(1);
		// Right Leg
		modelMatrix = glm::translate(modelMatrix, posPlayerBody);
		if (!isFirstPerson) {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2, -0.12f / 2, -0.2f / 2));
		}
		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2, -0.12f / 2, -0.2f / 2 - 0.1f));
		}
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2 - 0.28f, 0.2f / 2));
		modelMatrix = glm::rotate(modelMatrix, -leftLegAngle, glm::vec3(1, 0, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2 + 0.1824f, -0.12f / 2 - 0.517f + 0.28f, -0.2f / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.28f, 0.162f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.349f, 0.380f, 0.301f), isDamaged);

		// Right Foot
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -0.28f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 0.28f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.156f, 0.168f, 0.133f), isDamaged);

		modelMatrix = glm::mat4(1);
		// Left Shoulder
		modelMatrix = glm::translate(modelMatrix, posPlayerBody);
		if (!isFirstPerson) {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2 - 0.12f, -0.12f / 2, -0.2f / 2));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.12f / 2, 0.12f / 2, 0.12f / 2));
			modelMatrix = glm::rotate(modelMatrix, -leftLegAngle, glm::vec3(1, 0, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.12f / 2, -0.12f / 2, -0.12f / 2));
		}
		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2 - 0.12f, -0.12f / 2, -0.2f / 2 - 0.1f));
		}
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.12f, 0.12f, 0.12f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.215f, 0.3f, 0.149f), isDamaged);

		// Left Arm
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f, -1.8f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 1.8f, 0.9f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.588f, 0.490f, 0.380f), isDamaged);

		// Left Hand
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -0.3f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 0.3f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.192f, 0.184f, 0.180f), isDamaged);

		modelMatrix = glm::mat4(1);
		// Right Shoulder
		modelMatrix = glm::translate(modelMatrix, posPlayerBody);
		if (!isFirstPerson) {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2 + 0.35f, -0.12f / 2, -0.2f / 2));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.12f / 2, 0.12f / 2, 0.12f / 2));
			modelMatrix = glm::rotate(modelMatrix, leftLegAngle, glm::vec3(1, 0, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.12f / 2, -0.12f / 2, -0.12f / 2));
		}
		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.35f / 2, 0.12f / 2, 0.2f / 2 + 0.1f));
			modelMatrix = glm::rotate(modelMatrix, playerBodyAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.35f / 2 + 0.35f, -0.12f / 2, -0.2f / 2 - 0.1f));
		}
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.12f, 0.12f, 0.12f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.215f, 0.3f, 0.149f), isDamaged);

		// Right Arm
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -1.8f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 1.8f, 0.9f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.588f, 0.490f, 0.380f), isDamaged);

		// Right Hand
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -0.3f, 0.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 0.3f, 1.f));
		RenderSimpleMeshPlayer(meshes["cube"], shaders["MyShaderPlayer"], modelMatrix, glm::vec3(0.192f, 0.184f, 0.180f), isDamaged);
	}
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// Render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	isWalking = false;
	float cameraSpeed = playerSpeed;

	if (window->KeyHold(GLFW_KEY_W)) {
		isWalking = true;
		if (window->KeyHold(GLFW_KEY_D) || window->KeyHold(GLFW_KEY_A)) {
			cameraSpeed = cameraSpeed * glm::sqrt(2) / 2;
		}
		camera->MoveForward(cameraSpeed * deltaTime);
		glm::vec3 dir = glm::normalize(glm::vec3(forwardPlayer.x, 0, forwardPlayer.z));
		posPlayerBody += dir * (cameraSpeed * deltaTime);
		glm::vec3 pos = glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.1f);
		bool foundCollision = false;
		for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
			if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, pos, 0.25f)) {
				camera->MoveForward(-cameraSpeed * deltaTime);
				posPlayerBody -= dir * (cameraSpeed * deltaTime);
				foundCollision = true;
				break;
			}
		}
		if (!foundCollision) {
			for (int i = 0; i < mazeWallsVertical.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, pos, 0.25f)) {
					camera->MoveForward(-cameraSpeed * deltaTime);
					posPlayerBody -= dir * (cameraSpeed * deltaTime);
					break;
				}
			}
		}
		playerBodyAngle = playerHeadAngle;
	}
	else if (window->KeyHold(GLFW_KEY_S)) {
		isWalking = true;
		if (window->KeyHold(GLFW_KEY_D) || window->KeyHold(GLFW_KEY_A)) {
			cameraSpeed = cameraSpeed * glm::sqrt(2) / 2;
		}
		camera->MoveForward(-cameraSpeed * deltaTime);
		glm::vec3 dir = glm::normalize(glm::vec3(forwardPlayer.x, 0, forwardPlayer.z));
		posPlayerBody -= dir * (cameraSpeed * deltaTime);
		glm::vec3 pos = glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.1f);
		bool foundCollision = false;
		for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
			if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, pos, 0.25f)) {
				camera->MoveForward(cameraSpeed * deltaTime);
				posPlayerBody += dir * (cameraSpeed * deltaTime);
				foundCollision = true;
				break;
			}
		}
		if (!foundCollision) {
			for (int i = 0; i < mazeWallsVertical.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, pos, 0.25f)) {
					camera->MoveForward(cameraSpeed * deltaTime);
					posPlayerBody += dir * (cameraSpeed * deltaTime);
					break;
				}
			}
		}
		if (!foundCollision) {
			for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, camera->position, 0.25f)) {
					camera->MoveForward(cameraSpeed * deltaTime);
					posPlayerBody += dir * (cameraSpeed * deltaTime);
					foundCollision = true;
					break;
				}
			}
			if (!foundCollision) {
				for (int i = 0; i < mazeWallsVertical.size(); ++i) {
					if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, camera->position, 0.25f)) {
						camera->MoveForward(cameraSpeed * deltaTime);
						posPlayerBody += dir * (cameraSpeed * deltaTime);
						foundCollision = true;
						break;
					}
				}
			}
		}
		playerBodyAngle = playerHeadAngle;
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		isWalking = true;
		glm::vec3 dir = glm::normalize(glm::vec3(rightPlayer.x, 0, rightPlayer.z));
		camera->TranslateRight(-cameraSpeed * deltaTime);
		posPlayerBody -= dir * (cameraSpeed * deltaTime);
		glm::vec3 pos = glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.1f);
		bool foundCollision = false;
		for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
			if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, pos, 0.25f)) {
				camera->TranslateRight(cameraSpeed * deltaTime);
				posPlayerBody += dir * (cameraSpeed * deltaTime);
				foundCollision = true;
				break;
			}
		}
		if (!foundCollision) {
			for (int i = 0; i < mazeWallsVertical.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, pos, 0.25f)) {
					camera->TranslateRight(cameraSpeed * deltaTime);
					posPlayerBody += dir * (cameraSpeed * deltaTime);
					break;
				}
			}
		}
		if (!foundCollision) {
			for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, camera->position, 0.25f)) {
					camera->TranslateRight(cameraSpeed * deltaTime);
					posPlayerBody += dir * (cameraSpeed * deltaTime);
					foundCollision = true;
					break;
				}
			}
			if (!foundCollision) {
				for (int i = 0; i < mazeWallsVertical.size(); ++i) {
					if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, camera->position, 0.25f)) {
						camera->TranslateRight(cameraSpeed * deltaTime);
						posPlayerBody += dir * (cameraSpeed * deltaTime);
						foundCollision = true;
						break;
					}
				}
			}
		}
		if (!isFirstPerson && !foundCollision)
			playerBodyAngle = playerHeadAngle + glm::radians(45.f);
	}
	else if (window->KeyHold(GLFW_KEY_D)) {
		isWalking = true;
		camera->TranslateRight(cameraSpeed * deltaTime);
		glm::vec3 dir = glm::normalize(glm::vec3(rightPlayer.x, 0, rightPlayer.z));
		posPlayerBody += dir * (cameraSpeed * deltaTime);
		glm::vec3 pos = glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.1f);
		bool foundCollision = false;
		for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
			if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, pos, 0.25f)) {
				camera->TranslateRight(-cameraSpeed * deltaTime);
				posPlayerBody -= dir * (cameraSpeed * deltaTime);
				foundCollision = true;
				break;
			}
		}
		if (!foundCollision) {
			for (int i = 0; i < mazeWallsVertical.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, pos, 0.25f)) {
					camera->TranslateRight(-cameraSpeed * deltaTime);
					posPlayerBody -= dir * (cameraSpeed * deltaTime);
					break;
				}
			}
		}
		if (!foundCollision) {
			for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, camera->position, 0.25f)) {
					camera->TranslateRight(-cameraSpeed * deltaTime);
					posPlayerBody -= dir * (cameraSpeed * deltaTime);
					foundCollision = true;
					break;
				}
			}
			if (!foundCollision) {
				for (int i = 0; i < mazeWallsVertical.size(); ++i) {
					if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, camera->position, 0.25f)) {
						camera->TranslateRight(-cameraSpeed * deltaTime);
						posPlayerBody -= dir * (cameraSpeed * deltaTime);
						foundCollision = true;
						break;
					}
				}
			}
		}
		if (!isFirstPerson && !foundCollision)
			playerBodyAngle = playerHeadAngle + glm::radians(-45.f);
	}

	if (isWalking) {
		if (leftLegAngle >= glm::radians(25.f) || leftLegAngle <= glm::radians(-25.f)) {
			membersSpeed *= -1;
		}
		leftLegAngle += membersSpeed * deltaTime;
	}
	else {
		leftLegAngle = 0;
	}

	// UP and DOWN translation for the camera are left for debug purposes only
	if (window->KeyHold(GLFW_KEY_Q)) {
		camera->TranslateUpward(-cameraSpeed * deltaTime);
	}
	else if (window->KeyHold(GLFW_KEY_E)) {
		camera->TranslateUpward(cameraSpeed * deltaTime);
	}
}


void Tema2::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_F) {
		if (isFirstPerson) {
			camera->SetThirdPerson(glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y, posPlayerBody.z + 0.2f / 2));
			camera->RotateThirdPerson_OY(playerHeadAngle);
			playerHeadAngleVertical = 0.f;
		}
		else {
			camera->SetFirstPerson(glm::vec3(posPlayerBody.x + 0.35f / 2, posPlayerBody.y + 0.12f + 0.1f, posPlayerBody.z + 0.2f / 2));
			camera->RotateFirstPerson_OY(playerHeadAngle);
			playerBodyAngle = playerHeadAngle;
		}
		isFirstPerson = !isFirstPerson;
	}
	if (key == GLFW_KEY_TAB) {
		isDebugMode = !isDebugMode;
	}
	if (key == GLFW_KEY_M) {
		isMusicOff = !isMusicOff;
		bgSound->setAllSoundsPaused(isMusicOff);
	}
}


void Tema2::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
	window->SetPointerPosition(1280 / 2, 720 / 2);
	window->HidePointer();
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	float sensivityOX = 0.001f;
	float sensivityOY = 0.001f;

	if (isFirstPerson) {
		// TODO(student): Rotate the camera in first-person mode around
		// OX and OY using `deltaX` and `deltaY`. Use the sensitivity
		// variables for setting up the rotation speed.
		camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
		playerHeadAngleVertical += (deltaY * sensivityOY);
		if (camera->forward.y < -0.95f || camera->forward.y > 0.8f) {
			camera->RotateFirstPerson_OX(deltaY * sensivityOY);
			playerHeadAngleVertical -= (deltaY * sensivityOY);
		}
		camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
		float angle = deltaX * sensivityOX;
		playerBodyAngle -= angle;
		playerHeadAngle -= angle;

		forwardPlayer = glm::rotate(glm::mat4(1), -deltaX * sensivityOX, glm::vec3(0, 1, 0)) * glm::vec4(glm::vec3(forwardPlayer), 0);
		forwardPlayer = glm::normalize(forwardPlayer);
		rightPlayer = glm::rotate(glm::mat4(1), -deltaX * sensivityOX, glm::vec3(0, 1, 0)) * glm::vec4(glm::vec3(rightPlayer), 0);
		rightPlayer = glm::normalize(rightPlayer);
	}
	else {
		bool cameraCollision = false;
		// TODO(student): Rotate the camera in third-person mode around
		// OX and OY using `deltaX` and `deltaY`. Use the sensitivity
		// variables for setting up the rotation speed.
		camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
		if (camera->forward.y > 0.2f || camera->forward.y < -0.8) {
			camera->RotateThirdPerson_OX(deltaY * sensivityOY);
		}
		camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
		for (int i = 0; i < mazeWallsHorizontal.size(); ++i) {
			if (CircleToBoxCollision(mazeWallsHorizontal[i].x, mazeWallsHorizontal[i].x + 1.f, 0.f, 2.5f, mazeWallsHorizontal[i].z, mazeWallsHorizontal[i].z + widthWallMaze, camera->position, 0.25f)) {
				camera->RotateThirdPerson_OY(deltaX * sensivityOX);
				cameraCollision = true;
				break;
			}
		}
		if (!cameraCollision) {
			for (int i = 0; i < mazeWallsVertical.size(); ++i) {
				if (CircleToBoxCollision(mazeWallsVertical[i].x, mazeWallsVertical[i].x + widthWallMaze, 0.f, 2.5f, mazeWallsVertical[i].z, mazeWallsVertical[i].z + 1.f, camera->position, 0.25f)) {
					camera->RotateThirdPerson_OY(deltaX * sensivityOX);
					cameraCollision = true;
					break;
				}
			}
		}
		if (!cameraCollision) {
			float angle = deltaX * sensivityOX;
			if (glm::abs(playerHeadAngle - angle - playerBodyAngle) < (3.1415f / 4)) {
				playerHeadAngle -= angle;
			}
			else {
				playerBodyAngle -= angle;
				playerHeadAngle -= angle;
			}
			forwardPlayer = glm::rotate(glm::mat4(1), -deltaX * sensivityOX, glm::vec3(0, 1, 0)) * glm::vec4(glm::vec3(forwardPlayer), 0);
			forwardPlayer = glm::normalize(forwardPlayer);
			rightPlayer = glm::rotate(glm::mat4(1), -deltaX * sensivityOX, glm::vec3(0, 1, 0)) * glm::vec4(glm::vec3(rightPlayer), 0);
			rightPlayer = glm::normalize(rightPlayer);
		}
	}
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
	if (numberOfbullets > 0 && Engine::GetElapsedTime() - lastBulletFiredTime > bulletTimeout && button == GLFW_MOUSE_BUTTON_2 && isFirstPerson) {
		bullets.emplace_back(Bullet(camera->forward, glm::vec3(camera->position.x, camera->position.y, camera->position.z), Engine::GetElapsedTime()));
		lastBulletFiredTime = Engine::GetElapsedTime();
		shootingSound->play2D(pathShootingSound, false);
		--numberOfbullets;
	}
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

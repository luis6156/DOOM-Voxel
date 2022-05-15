#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/camera.h"
#include "lab_m1/Tema2/maze.h"
#include "lab_m1/Tema2/object3D.h"
#include "lab_m1/Tema2/irrKlang/irrKlang.h"

using namespace irrklang;

namespace m1
{
	struct Bullet {
		glm::vec3 forward;
		glm::vec3 pos;
		int spawnTime;

		Bullet(glm::vec3 _forward, glm::vec3 _pos, int _spawnTime) : forward(_forward), pos(_pos), spawnTime(_spawnTime)
		{
		}
	};

	struct EnemyStats {
		int health;
		int deathTime;

		EnemyStats() {
			health = 100;
			deathTime = 0;
		}
	};

	class Tema2 : public gfxc::SimpleScene
	{
	public:
		Tema2();
		~Tema2();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
		void RenderSimpleMeshEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int health, bool isDying);
		void RenderSimpleMeshPlayer(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, bool isDamaged);
		Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
		void RenderPlayer();
		void RenderHUD();
		void ResetGame();
		void ResetPowerup();
		void PickupPowerup();
		void RenderPowerup();
		void RenderClouds();
		void RenderMaze();
		void AnimateEnemies(float deltaTimeSeconds);
		void AnimateAndRenderBullets(float deltaTimeSeconds);
		void UpdatePlayerHealth();
		void RenderEnemies();
		void RenderDebug();
		void CheckGameState();

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	protected:
		world_camera::Camera* camera;
		glm::mat4 projectionMatrix;
		ISoundEngine* bgSound;
		ISoundEngine* shootingSound;
		ISoundEngine* pickupSound;
		ISoundEngine* playerDieSound;
		ISoundEngine* playerDamageSound;
		ISoundEngine* enemyDieSound;
		ISoundEngine* enemyDamageSound;
		ISoundEngine* finishMapSound;
		Maze::Backtracking* maze;
		vector<glm::vec3> mazeWallsHorizontal;
		vector<glm::vec3> mazeWallsVertical;
		int widthMaze = 12;
		int heightMaze = 12;
		float widthWallMaze = 0.05f;
		float widthCorridor = 4.f;
		vector<glm::ivec2> generatedMaze;
		float bottom = -5, top = 5, left = -5, right = 5;
		float fov = 60;
		bool isOrtho = false;
		glm::vec3 posPlayerBodyOg = glm::vec3(-0.35f / 2 + 2.f, 0.595f, -0.2f / 2 + 2.f);
		glm::vec3 posPlayerHead = glm::vec3(posPlayerBody.x + 0.35f * 0.2f, posPlayerBody.y + 0.12f, posPlayerBody.z);
		float playerHeadAngle = 0.f;
		float playerHeadAngleVertical = 0.f;
		float playerBodyAngle = 0.f;
		glm::vec3 forwardPlayer;
		glm::vec3 upPlayer;
		glm::vec3 rightPlayer;
		bool isFirstPerson = false;
		bool isWalking = false;
		float leftLegAngle = 0.f;
		float leftArmAngle = 0.f;
		float rightLegAngle = 0.f;
		float rightArmAngle = 0.f;
		float walkingAngle = 3.1415f / 4;
		list<glm::vec3> enemysPos;
		list<glm::vec3> enemiesOgPos;
		int enemyState = 0;
		float enemyRotation = 0.f;
		float enemyLength = 0.8f;
		list<Bullet> bullets;
		int bulletMaxTime = 10;
		float health = 1.45f;
		float lastHit;
		list<EnemyStats> enemies;
		glm::vec3 posPlayerBody = glm::vec3(-0.35f / 2 + 2.f, 0.595f, -0.2f / 2 + 2.f);
		bool isDebugMode = false;
		int maxTimeAllowed = 90;
		int startTime;
		float timeBarLength = 0.95f;
		int maxDeathAnimation = 3;
		float bulletTimeout = 0.5f;
		float lastBulletFiredTime = 0.f;
		vector<glm::vec3> cloudsPos;
		vector<glm::vec3> cloudsSize;
		bool isDamaged = false;
		bool isDamagedOg = false;
		float damageTimer = 3.f;
		bool isMusicOff = false;
		char* pathShootingSound;
		char* pathPlayerDamageSound;
		char* pathEnemyDamageSound;
		char* pathEnemyDieSound;
		char* pathPlayerDieSound;
		char* pathPickupSound;
		char* pathFinishMapSound;
		float lastPowerupTime = 0.f;
		float powerUpCooldown = 5.f;
		list<glm::vec3> powerups;
		list<int> powerupsType;
		float powerupPickedTime = 0.f;
		float maxPowerupTime = 3.f;
		int playerPowerupType = -1;
		float playerSpeed = 3.0f;
		float membersSpeed = playerSpeed;
		int numberOfbullets = 5;
		float bulletSpeed = 30;
	};
}   // namespace m1

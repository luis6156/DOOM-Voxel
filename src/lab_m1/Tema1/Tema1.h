#pragma once

#include "components/simple_scene.h"


namespace m1
{
    struct Powerup {
        std::string name;
        float x;
        float y;
    };

    struct Obstacle {
        float x;
        float y;
        float height;
        float width;
        float scaleX;
        float scaleY;

        Obstacle(float _x, float _y, float _height, float _width, float _scaleX, float _scaleY) : x(_x), y(_y), height(_height * _scaleY), width(_width * _scaleX), scaleX(_scaleX), scaleY(_scaleY)
        {
        }
    };

    struct Projectile {
        float startX;
        float startY;
        float x;
        float y;
        float length;
        float angle;
        float speed;

        Projectile(float _x, float _y, float _length, float _angle, float _speed) : startX(_x), startY(_y), x(_x), y(_y), length(_length), angle(_angle), speed(_speed)
        {
        }
    };

    struct Enemy {
        float x;
        float y;
        float length;
        float angle;
        float speed;

        Enemy(float _x, float _y, float _length, float _angle, float _speed) : x(_x), y(_y), length(_length), angle(_angle), speed(_speed)
        {
        }
    };

    class Tema1 : public gfxc::SimpleScene
    {
     public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        float inline ComputeDistanceAB(float xA, float yA, float xB, float yB);

        void DrawScene(glm::mat3 visMatrix);
        void inline GenerateEnemy(float rangeXMin, float rangeXMax, float rangeYMin, float rangeYMax);
        bool inline CCColision(float x1, float y1, float radius1, float x2, float y2, float radius2);
        bool inline CBColision(float x1, float y1, float width, float height, float x2, float y2, float radius);
        glm::vec4 GenerateRectAttributes(float rangePosX, float rangePosY, float rangeWidth, float rangeHeight, float rangeScaleX, float rangeScaleY);
        void inline GeneratePowerup(float rangeXMin, float rangeXMax, float rangeYMin, float rangeYMax);
        void inline GenerateEnemyOffCenter(float offsetEnemySpawn);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        // Sets the logic space and view space
        // logicSpace: { x, y, width, height }
        // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);

        void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

     protected:
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        LogicSpace logicSpaceMinimap;
        glm::mat3 modelMatrix, visMatrix;
        glm::ivec2 resolution;
        glm::ivec2 mousePrev;
        std::vector<Projectile> projectiles;
        std::vector<Enemy> enemies;
        std::vector<int> enemiesToBeErased;
        std::vector<int> projectilesToBeErased;
        std::vector<Obstacle> obstacles;
        Powerup powerup;
        float bodyRadius = 3;
        float handsRadius = 1.2f;
        int circleNrTriangles = 100;
        float offsetHandsX = 2.2f;
        float offsetHandsY = 2.2f;
        float playerSpeed = 20;
        float projectileLength = 2;
        float halfProjectileLength = projectileLength / 2;
        float rotationAnglePlayer = 0;
        float rotationAngleProj = 0;
        float centerLogicSpaceX;
        float centerLogicSpaceY;
        float halfPI = 3.1415f / 2;
        float viewScaleRatioY;
        float viewScaleRatioX;
        float offsetPlayerX;
        float offsetPlayerY;
        bool isPlayerShoting = false;
        float offsetEnemySpawn = 30;
        float startCounter = -1;
        float projectileMaxDist = 35;
        float projectileMaxTime = 1.f;
        float projectileSpeed = 30;
        float enemyMinTime = 2;
        float enemyStartCounter;
        float enemyBodyLength = 5;
        float enemyHandLength = 2;
        float enemyRangeX = 50;
        float enemyRangeY = 50;
        float enemySpeed = 20;
        float healthbarWidth = 30;
        float healthbarLength = 5;
        float healthbarOffsetWidth = healthbarWidth / 5;
        float healthbarOffsetLength = healthbarLength / 5;
        bool isDebugOn = false;
        float rectangleLength = 5;
        float rectangleWidth = 10;
        float obstaclesNumber = 10;
        float obstaclesPosRangeX = 40;
        float obstaclesPosRangeY = 20;
        float obstaclesWidthRange = 10;
        float obstaclesHeightRange = 5;
        float obstaclesScaleRangeX = 1.5f;
        float obstaclesScaleRangeY = 1.5f;
        bool isMinimap = false;
        int health = 100;
        int score = 0;
        float powerupRadius = 2;
        float powerupTime = 8;
        float powerupCooldown = 8;
        float powerupSpawnTime;
        float totalPowerupsSpawned = 0;
        bool isSpeedup = false;
        bool isFirerate = false;
        float powerupActiveTime = 5;
        float powreupActivatedTime = 0;
    };
}   // namespace m1

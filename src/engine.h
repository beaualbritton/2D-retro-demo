#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <vector>
#include <memory>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader/shaderManager.h"
#include "font/fontRenderer.h"
#include "shapes/Cloud.h"
#include "shapes/rect.h"
#include "shapes/shape.h"
#include "shapes/triangle.h"
#include "shapes/textbox.h"
#include "game/enemy.h"

using std::vector, std::unique_ptr, std::make_unique, glm::ortho, glm::mat4, glm::vec3, glm::vec4;

/**
 * @brief The Engine class.
 * @details The Engine class is responsible for initializing the GLFW window, loading shaders, and rendering the game state.
 */
class Engine {
    private:
        /// @brief The actual GLFW window.
        GLFWwindow* window{};

        /// @brief The width and height of the window.
        const unsigned int width = 800, height = 600; // Window dimensions

        /// @brief Keyboard state (True if pressed, false if not pressed).
        /// @details Index this array with GLFW_KEY_{key} to get the state of a key.
        bool keys[1024];

        /// @brief Responsible for loading and storing all the shaders used in the project.
        /// @details Initialized in initShaders()
        unique_ptr<ShaderManager> shaderManager;
        const glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

        //Font renderer
        unique_ptr<FontRenderer> fontRenderer;

        //Shapes for this project
        vector<unique_ptr<Rect>> platforms;
        unique_ptr<Rect> goal;
        unique_ptr<Rect> user;

        Shader shapeShader;
        Shader textShader;
        //textbox that will be displayed throughout the game
        unique_ptr<Textbox> messageTextbox;


        double MouseX, MouseY;

        //enemy that will be regenerated in engine.cpp logic. pointer for that reason
        unique_ptr<enemy> currentEnemy;
        //Same as currentEnemy
        unique_ptr<entity> playerCharacter;

        /*
         * Holy mother of god...
         *
         * Trust me. ALL of these bolleans are necessary for the simple battle screen you see in this demo.
         * These are FLAGS. nothing more. trying to track game states is not fun.
         *
         * Tried to be fairly verbose with the naming here
         */
        bool isPlayerTurn;
        bool isEnemyTurn;
        bool isBattling;
        bool playerInAction;

        bool battleTextDisplayed;
        bool infoTextDisplayed;
        bool enemyTextDisplayed;

        bool playerAttacking;
        bool playerDefending;
        bool playerViewing;
        bool playerRunning;

        bool waitingForEnter;



    public:
        /*
         * these are for platforming physics calculations in engine.cpp, reason these are set as public
         * are due to const nature, these are *physical* constants.
         */

        //Constant values
        bool onGround = false;
        //9.8 m/s^2 gravity constant upped by a few magnitued for some oomph
        const float gravity = 980.0f;
        //Values for movement
        const float jumpForce = 500.0f;
        const float moveSpeed = 300.0f;
        const float platformHeight = 10.0f;
        //ok maybe not these.. but dont tell..
        int score = 0;
        bool resetGame = false;

        /// @brief Constructor for the Engine class.
        /// @details Initializes window and shaders.
        Engine();

        /// @brief Destructor for the Engine class.
        ~Engine();

        /// @brief Initializes the GLFW window.
        /// @return 0 if successful, -1 otherwise.
        unsigned int initWindow(bool debug = false);

        /// @brief Loads shaders from files and stores them in the shaderManager.
        /// @details Renderers are initialized here.
        void initShaders();

        /// @brief Initializes the shapes to be rendered.
        void initShapes();

        /// @brief Processes input from the user.
        /// @details (e.g. keyboard input, mouse input, etc.)
        void processInput();

        /// @brief Updates the game state.
        /// @details (e.g. collision detection, delta time, etc.)
        void update();

        /// @brief Renders the game state.
        /// @details Displays/renders objects on the screen.
        void render();

        /* deltaTime variables */
        float deltaTime = 0.0f; // Time between current frame and last frame
        float lastFrame = 0.0f; // Time of last frame (used to calculate deltaTime)

        /// @brief Returns true if the window should close.
        /// @details (Wrapper for glfwWindowShouldClose()).
        /// @return true if the window should close
        /// @return false if the window should not close
        bool shouldClose();

        /// Projection matrix used for 2D rendering (orthographic projection).
        /// We don't have to change this matrix since the screen size never changes.
        /// OpenGL uses the projection matrix to map the 3D scene to a 2D viewport.
        /// The projection matrix transforms coordinates in the camera space into normalized device coordinates (view space to clip space).

        /// @note The projection matrix is used in the vertex shader.
        // 4th quadrant
        const mat4 PROJECTION = ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
        // 1st quadrant
//        mat4 PROJECTION = ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

        /// @brief Debug function to check for OpenGL errors.
        GLenum glCheckError_(const char *file, int line);
        /// @brief Macro for glCheckError_ function. Used for debugging.
        #define glCheckError() glCheckError_(__FILE__, __LINE__)





};

#endif //GRAPHICS_ENGINE_H

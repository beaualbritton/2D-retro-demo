#include "engine.h"
#include <iostream>
#include "game/player.h"
#include <random>
using namespace std;

//enums for screen state control, modeled after m4gp confetti
enum state {start, play, battle, over};
state screen = start;

//const colors
const color blue(77 / 255.0, 213 / 255.0, 240 / 255.0);
const color green(26 / 255.0, 176 / 255.0, 56 / 255.0);
const color white(1, 1, 1);
const color black(0, 0, 0);
const color red(1,0,0);

//storing this as a global variable instead of a private member of engine,
//gave parameter problems otherwise
vec2 playerVelocity(0, 0);

//Engine constructor initializes the window, shaders and relative shapes (to be drawn)
//akin to other module 4 projects
Engine::Engine() : keys() {
    this->initWindow();
    this->initShaders();
    this->initShapes();

    //Also intitializes the player:
    playerCharacter = make_unique<entity>(100,0,"Player","A lone knight.",1);
}

//Destructor
Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(width, height, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    // OpenGL configuration
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert", "../res/shaders/shape.frag", nullptr, "shape");

    // Set uniforms that never change
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    // Create a textbox for displaying a message
    messageTextbox = make_unique<Textbox>(shapeShader, textShader, vec2(width / 2, height/5), vec2(400, 100), black, "../res/fonts/MxPlus_IBM_BIOS.ttf");
    // Set projection for textbox
    messageTextbox->setProjection(PROJECTION);
    messageTextbox->enableScrolling(15.0f);

    //If none of the above is intuitive feel free to check Textbox.cpp, all of these methods are explained there.
}

void Engine::initShapes() {
    //Initializing user (member of engine) as a white rectangle
    user = make_unique<Rect>(shapeShader, vec2(width / 2, 100), vec2(20, 20), white);
    //Clearing platforms for any sort of problems
    platforms.clear();
    //First index of platforms is always the 'ground'.
    platforms.push_back(make_unique<Rect>(shapeShader, vec2(width / 2, 50), vec2(width, platformHeight*10), green));
    /*
     * This loop generate the main platform each time initShapes is called.
     * Initial height for platforms is 200, and iterates up until window height - 100 pixels.
     * Then, height has a random y value to add to it
     */
    for (float y = 200; y < height - 100; y += rand() % 50 + 30) {
        //X value for platforms varies anywhere on the x axis within 100 pixels of window width (50px on each side)
        float x = rand() % (width - 100) + 50;
        //Width of platforms is also random
        float platformWidth = rand() % 100 + 80;

        platforms.push_back(make_unique<Rect>(shapeShader, vec2(x, y), vec2(platformWidth, platformHeight), green));
    }
    //After platforms vector is populated, add a 'goal' square to the highest identifiable
    if (!platforms.empty()) {
        float yValue = 0;
        float xValue = 0;
        //Reference variable for each unique platform. For each loop to simplify and have easier access.
        for (const unique_ptr<Rect> &platform : platforms) {
            if (platform->getPos().y > yValue) {
                //Setting goal's position to the last platform in the vector.
                yValue = platform->getPos().y;
                xValue = platform->getPos().x;
            }
        }
        goal = make_unique<Rect>(shapeShader, vec2(xValue, yValue + 20), vec2(20, 20), red);
    }
}

void Engine::processInput() {
    glfwPollEvents();
    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }
    // Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    /*
     * screen state logic
     */
    switch (screen) {
        case start:
            //progress past intro screen dialogue
            if (keys[GLFW_KEY_ENTER]) {
                screen = play;
                messageTextbox->close();
            }
            //display information text to user
            if (keys[GLFW_KEY_I]) {
                messageTextbox->open();
                messageTextbox->setText("Here's the deal. You're a knight wandering the dark plain. You're going to encounter various *enemies* and they will try to attack you. Beware, you can Attack, Defend, View and Run. If you are to perish, you shall be reborn anew. So don't fret! Urist be with you.");
                infoTextDisplayed = true;
            }
        break;
        case play:
            //Secret menu functionality. This was moreso for testing but I kept it in the game for demoing purposes.
            if(keys[GLFW_KEY_M]) {
                messageTextbox->open();
                messageTextbox->setText("You just opened a secret menu!");
            }
            //Setting player's horizontal velocity to zero everytime an input is read
            playerVelocity.x = 0;

            //Moving player's horzontal velocity in a negative direct with left arrow or a
            if (keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A]) {
                playerVelocity.x = -moveSpeed;
            }
            //Same as above, positive instead for right arrow or d
            if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D]) {
                playerVelocity.x = moveSpeed;
            }

            //Jump functionality, using onGround as a way to track how many times the player can jump.
            //If the player presses space twice in a row, there should only be a single jump (no double jumps)
            if ((keys[GLFW_KEY_UP] || keys[GLFW_KEY_SPACE]) && onGround) {
                playerVelocity.y = jumpForce;
                onGround = false;
            }
        break;
        case battle:
            //Deathfunctionality. This was also for testing but I kept it in the game for demoing purposes. Player can perish by gameplay, but this expedites it so all screens can be rendered properly.
            if (keys[GLFW_KEY_B]) {
                // Transition to game over screen
                screen = over;
                messageTextbox->setText("You have died...");
                //Slow scrolling for dramatic effect
                messageTextbox->enableScrolling(5.0f);
            }
            if (keys[GLFW_KEY_ENTER]) {
                /*
                 * ALRIGHT.. This might look messy. It is, I'm using a few different flags to display varying
                 * text that can be displayed by an enemy in the battle screen. If there is no battle (yet) display the text and
                 * start the battle (enable players turn)
                 */
                if(!isBattling) {
                    isBattling = true;
                    isPlayerTurn = true;
                }
                //If the enemy has attacked (flags changed in update logic) then another enter should progress past their displayed text for the next phase in the battle.
                if(enemyTextDisplayed) {
                    enemyTextDisplayed = false;
                    waitingForEnter = true;
                    playerInAction = false;
                    isPlayerTurn = true;
                    battleTextDisplayed = false;
                }
                //Similar logic to above
                else if(!isPlayerTurn && playerInAction) {
                    isEnemyTurn = true;
                }
            }
        //I'm not sure this is working but I'm too afraid to remove it so it doesnt break
        if (waitingForEnter) {
            if(keys[GLFW_KEY_ENTER]) {
                waitingForEnter = false;
            }
        }
        //Listed attack options in battle mode, flagging each one individually
        if (isPlayerTurn && !playerInAction) {
            if (keys[GLFW_KEY_A]) {
                // Attack
                playerInAction = true;
                playerAttacking = true;

            }
            if (keys[GLFW_KEY_D]) {
                // Defend
                playerInAction = true;
                playerDefending = true;
            }
            if (keys[GLFW_KEY_V]) {
                // View
                playerInAction = true;
                playerViewing = true;
            }
            if (keys[GLFW_KEY_R]) {
                // Run
                playerInAction = true;
                playerRunning = true;
            }
        }
        break;
        case over:
            //Restart game if you died.
            if (keys[GLFW_KEY_R]) {
                screen = start; // Restart the game
                this->initShapes();
            }
        break;
    }
    //Close textboxes with Q at *any* time (persistent among screens)
    if((keys[GLFW_KEY_Q]) && messageTextbox->shouldClose) {
        messageTextbox->shouldClose = false;
        messageTextbox->close();
    }
}

void Engine::update() {
    //deltaTime calculations referenced from previous M4GPs
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    /*
     *Update screen state logic
     */
    switch (screen) {
        case start:
            //Initialize textbox to welcome text. Pretty straightforward.
            if(messageTextbox->getText() == "" && !infoTextDisplayed) {
                messageTextbox->setText("Welcome to the game! Press enter to continue, or press (i) for game info");
            }
            break;

        /*
         * PHYSICS CALCULATIONS FOR PLAYER MOVEMENT
         *
         * this is in update
         */
        case play: {
            // Player's horizontal velocity is always being weighed down by gravity
            playerVelocity.y -= gravity * deltaTime;
            // Creating a new vector that copy's player's current position, adding both vertical and horizontal velocity each frame
            vec2 nextPos = user->getPos();
            nextPos.x += playerVelocity.x * deltaTime;
            nextPos.y += playerVelocity.y * deltaTime;

            // Resetting this each frame
            onGround = false;

            // Next frame collisions
            Rect nextPosRect(shapeShader, nextPos, user->getSize(), white);

            // Check collisions with all platforms
            for (const unique_ptr<Rect> &platform: platforms) {
                if (Rect::isOverlapping(nextPosRect, *platform)) {
                    vec2 currentPos = user->getPos();

                    // Landing on top of a platform
                    if (currentPos.y > platform->getPos().y && playerVelocity.y < 0) {
                        nextPos.y = platform->getTop() + user->getSize().y/2;
                        playerVelocity.y = 0;
                        onGround = true;
                    }
                    // If player is below a platform when jumping
                    else if (currentPos.y < platform->getPos().y && playerVelocity.y > 0) {
                        nextPos.y = platform->getBottom() - user->getSize().y/2;
                        playerVelocity.y = 0;
                    }
                    // If player is to the left of a platform when moving horizontally
                    else if (currentPos.x < platform->getPos().x) {
                        nextPos.x = platform->getLeft() - user->getSize().x/2;
                        playerVelocity.x = 0;
                    }
                    // If player is to the right of a platform when moving horizontally
                    else if (currentPos.x > platform->getPos().x) {
                        nextPos.x = platform->getRight() + user->getSize().x/2;
                        playerVelocity.x = 0;
                    }
                }
            }

            // Check collision with goal
            if (Rect::isOverlapping(nextPosRect, *goal)) {
                score++;
                platforms.clear();

                // Create a small platform below the player
                platforms.push_back(make_unique<Rect>(shapeShader, vec2(user->getPosX(), user->getPosY()), vec2(width/5, platformHeight), green));

                // Generate new platforms
                for (float y = 200; y < height - 100; y += rand() % 50 + 30) {
                    float x = rand() % (width - 100) + 50;
                    float platformWidth = rand() % 100 + 80;
                    platforms.push_back(make_unique<Rect>(shapeShader, vec2(x, y), vec2(platformWidth, platformHeight), green));
                }

                // Update goal position
                if (!platforms.empty()) {
                    float yValue = 0;
                    float xValue = 0;
                    for (const unique_ptr<Rect> &platform : platforms) {
                        if (platform->getPos().y > yValue) {
                            yValue = platform->getPos().y;
                            xValue = platform->getPos().x;
                        }
                    }
                    goal = make_unique<Rect>(shapeShader, vec2(xValue, yValue + 20), vec2(20, 20), red);
                }
                // A "goal" in this case is an enemy, and we want to attack it!
                //Progress to battle screen
                screen = battle;
                //Setting battle flags
                isBattling = false;
                playerInAction = false;
                battleTextDisplayed = false;

                //generate battle with enemy:
                currentEnemy = make_unique<enemy>();
                messageTextbox->setText("You have encountered a " + currentEnemy->getName() +"\n"+ currentEnemy->getDescription()+"\nWhat do you do?");
                messageTextbox->open();
            }

            // Update player position
            user->setPos(nextPos);

            // Check if player has fallen too far
            if (user->getPos().y < 0) {
                resetGame = true;
            }

            // Reset if needed
            if (resetGame) {
                playerVelocity = vec2(0, 0);
                onGround = false;
                this->initShapes();
                resetGame = false;
            }
            break;
        }

        case battle:
            /*
             * Player turn for battle. Firstly, display to the user their options, then
             * make sure the player and current enemy are alive, otherwise change screens.
             *
             * playerAttacking/Defending etc conditionals are modified in processInput() for A,D,V,R
             */
            if(isPlayerTurn) {
                if(!battleTextDisplayed && !waitingForEnter) {
                    messageTextbox->setText("(A)ttack.\n(D)efend.\n(V)iew.\n(R)un.");
                    battleTextDisplayed = true;
                    playerInAction = false;
                    infoTextDisplayed = false;
                }
                if(currentEnemy->getHealth()<=0) {
                    screen = play;
                    messageTextbox->setText("You defeated " + currentEnemy->getName());
                }
                if(playerCharacter->getHealth() <= 0) {
                    screen = over;
                }

                /*
                 * Calls game logic and sets the string returned to the textbox message
                 */
                if(playerAttacking) {
                    srand(time(0));
                    string digest = playerCharacter->attackAgainst(*currentEnemy,rand()%50);
                    digest+="\n"+currentEnemy->move_against(*playerCharacter);
                    messageTextbox->setText(digest);

                    //Game flags for proper input processing
                    playerAttacking = false;
                    isPlayerTurn = false;
                    playerInAction = true;
                    // Reset battleTextDisplayed to allow displaying options again
                    battleTextDisplayed = false;
                    // Reset enemyTextDisplayed to allow new text
                    enemyTextDisplayed = false;
                }
                // Very similiar to playerAttacking structure
                if(playerDefending) {
                    // Similar logic to attacking
                    playerDefending = false;
                    isPlayerTurn = false;
                    playerInAction = true;
                    string digest = playerCharacter->defendAgainst(*currentEnemy);
                    digest+="\n"+currentEnemy->move_against(*playerCharacter);
                    messageTextbox->setText(digest);

                    battleTextDisplayed = false;
                    enemyTextDisplayed = false;
                }
                if(playerViewing) {
                    //View stats for the enemy and player
                    messageTextbox->setText("Your Health: " + to_string(playerCharacter->getHealth()) + "\n" + "Enemy Health: "+ to_string(currentEnemy->getHealth()));
                    playerViewing = false;
                    isPlayerTurn = false;
                    playerInAction = true;

                    battleTextDisplayed = false;
                    enemyTextDisplayed = false;
                }
                if(playerRunning) {
                    //Run away. Pretty straightforward.
                    screen = play;
                    playerRunning = true;
                    isPlayerTurn = false;
                    playerInAction = true;

                    battleTextDisplayed = false;
                    enemyTextDisplayed = false;
                    messageTextbox->setText("You ran away!");
                }
            }
            else if (!isPlayerTurn) {
                if (isEnemyTurn && !enemyTextDisplayed) {

                    /*
                     * Pretty much serves as a switch that activates/deactivates certain text progression
                     * in battle screen, if the player has already attacked and battleText is still displaying, cant automatically jump to next player attack.
                     */
                    enemyTextDisplayed = true;
                    isEnemyTurn = false;
                    waitingForEnter = true;
                    battleTextDisplayed = false;
                    isPlayerTurn = true;
                }
            }
        break;

            break;

        case over:
            // Logic for over screen (e.g., restart checks)
            break;
    }
}

void Engine::render() {
    glClearColor(blue.red, blue.green, blue.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shapeShader.use();


    switch(screen) {
        case start:{
            glClearColor(0.5f,0.5f,0.5f,0.5f);
            glClear(GL_COLOR_BUFFER_BIT);
            break;
        }
        case play: {
            //Iterating through platforms, for each platform setunfirm and draw.
            for (const unique_ptr<Rect> & platform : platforms) {
                if (platform) {
                    platform->setUniforms();
                    platform->draw();
                }
            }
            shapeShader.use();

            // Draw goal
            goal->setUniforms();
            goal->draw();

            // Draw player
            user->setUniforms();
            user->draw();
            break;
        }
        case battle: {
            glClearColor(0.5f,0.5f,0.5f,0.5f);
            glClear(GL_COLOR_BUFFER_BIT);
            break;
        }
        case over: {
            glClearColor(0.0f,0.0f,0.0f,0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            break;
        }
}
    messageTextbox->setUniforms();
    messageTextbox->draw(deltaTime);
    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}

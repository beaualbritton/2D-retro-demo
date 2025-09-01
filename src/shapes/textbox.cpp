#include "textbox.h"

/*
 * This constructor takes in a few parameters, shapeShader for the background shape, textShader for
 * the text to be rendered, information for positionining and size for the background shape.
 *
 * Will then initialize various fields -- most are explicit in name (intentional)
 */
Textbox::Textbox(Shader& shapeShader, Shader& textShader, vec2 pos, vec2 size, color bgColor,
                 const string& fontPath)
    : Shape(shapeShader, pos, size, bgColor),
      text(""),

      textColor({1.0f, 1.0f, 1.0f, 1.0f}),
      textShader(textShader),
      fontSize(12.0f),
      isScrolling(false),
      scrollSpeed(50.0f),
      scrollOffset(0.0f),
      scrollElapsedTime(0.0f),
      isVisible(true),
      shouldClose(false),
      visibleCharacters(0),
      indicator(shapeShader, {pos.x + (size.x / 2) - 10.0f, pos.y - (size.y / 2) + 10.0f}, {10.0f, 10.0f}, {1.0f, 1.0f, 1.0f, 1.0f})
{
    //Initializes fontRenderer (see method body) as a unique pointer for later use in the draw method.
    initTextRendering(fontPath);
    //using Rect.cpp's constructor as a reference, initializes important vector info LAST.
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}


void Textbox::initTextRendering(const string& fontPath) {
    fontRenderer = std::make_unique<FontRenderer>(textShader, fontPath, fontSize);
}

//Same as Rect.cpp/Shape, just a box.
void Textbox::initVectors() {
    this->vertices = {
        -0.5f, 0.5f,    // Top left
        0.5f, 0.5f,     // Top right
        -0.5f, -0.5f,   // Bottom left
        0.5f, -0.5f     // Bottom right
    };

    this->indices = {
        0, 1, 2,
        1, 2, 3
    };
}


void Textbox::draw(float deltaTime) const {
    //If not visible dont render
    if(!isVisible) {
        return;
    }
    /*
     * Using openGL commands to bind and draw elements.
     */
    glBindVertexArray(VAO);
    Shape::setUniforms();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //indicator initialized for later drawing after text is finished drawing.
    indicator.setUniforms();

    // Determine which text to render. Overflow text is a substring of the text field, displayed
    // after all other text so that there is text wrapping
    string currentText;
    if (text.empty()) {
        currentText = overflowText;
    } else {
        currentText = text;
    }

    //If there is text to render
    if (!currentText.empty()) {
        //Use text shader (text rendering rather than rendering shapes)
        textShader.use();
        //Setting projection for matrix, see "shader/shader.cpp" for method body (not implemented by me)
        textShader.setMatrix4("projection", projection);

        if (isScrolling) {
            //Using deltaTime, passed in by engine.cpp, to increment scrolling time by measurable amount of time.
            scrollElapsedTime += deltaTime;
            //Simple rate of 1 char per scroll speed.
            float timePerCharacter = 1.0f / scrollSpeed;
            //While loop that increments the amount of visible characters, while slowly decrementing elapsedTime
            while (scrollElapsedTime >= timePerCharacter && visibleCharacters < currentText.length()) {
                ++visibleCharacters;
                scrollElapsedTime -= timePerCharacter;
            }
        }
        else {
            //No longer scrolling so message should be original length
            visibleCharacters = currentText.length();
        }

        //Using substring to obscure 'text' field to only display/draw text up until the amount of visiblecharacters
        string textToRender = currentText.substr(0, visibleCharacters);
        //Using these floats to approximate width for the characters, line, and linewidth
        float charWidth = 12.5f;
        float lineHeight = 14.0f;
        float maxLineWidth = size.x - 2 * 5.0f;
        //Current text x coordinates for later use
        float textX = pos.x - (size.x / 2) + 5.0f;
        float textY = pos.y + (size.y / 2) - lineHeight - 5.0f;
        //Might seem unintuitive at first but we need copy values for easy comparison in later loop
        float currentX = textX;
        float currentY = textY;

        //Current line count (start at 0) to be incremented in following loop
        int lineCount = 0;
        /*
         *There's a lot going on this loop. It's fairly chunky, but it goes through each character
         *individually for the length of an entire string. Getting this *somewhat* stable and working
         *was a chore in its own right, so following code after this loop is unintuitive. I figured
         *why break the *mostly* working scrolling text loop I have when I can jerry rig it? Cutting corners I know. Sorry mom.
         */
        for (int i = 0; i < textToRender.length(); ++i) {
            char c = textToRender[i];

            // Checking 'c' (current char) if its not a space, and if the currentX value with added char width exceed width boundaries.
            //OR a new line,
            if (c != ' ' && currentX + charWidth > textX + maxLineWidth || c == '\n') {
                //If thats the case, move text rendering down by one line
                currentX = textX;
                // Move down by line height
                currentY -= lineHeight;
                lineCount++;

            }

            // Stop rendering currentText if we've reached 6 lines and move to overflow
            if (lineCount >= 6) {
                // Move the remainder of the text into overflowText
                overflowText = currentText.substr(i);
                // Clear the main text once it's fully transitioned to overflow
                text = "";
                visibleCharacters = 0;
            }

            //Render the character with fontRenderer
            string character(1, c);
            //If the current character is a newline, replace it with nothing (obscure it from user)
            if(c=='\n') {
                character = "";
            }
            fontRenderer->renderText(character, currentX, currentY, projection, 1.0f,
                                     {textColor.red, textColor.green, textColor.blue});
            //Added this line here because even if '\n' is obscure (seen on line 150) a charWidth will still be counted for it.
            //This ensures that all chars that are not \n have width set for them.
            if(c!='\n') {
                // Move to the next position
                currentX += charWidth;
            }
           //Extra spacing for space, just so it looks nicer.
            if (c == ' ') {
                currentX += charWidth;
            }
        }
    }

    //Simple conditional to check for the end of the drawn text/string. If there are no more characters to be rendered
    //(Visible characters is at max length) then draw indicator and allow closing of textbox.
    if (visibleCharacters >= currentText.length()) {
        shouldClose = true;
        this->shader.use();
        indicator.draw();
    }
}


void Textbox::setText(const std::string& newText) {
    text = newText;

    //Clearing these fields -- after  new text is set to the textbox,
    //Overflow, visibleCharacters and scrollOfset need to be reset.
    overflowText = "";
    scrollOffset = 0.0f;
    scrollElapsedTime = 0.0f;
    visibleCharacters = 0;
}


string Textbox::getText() const {
    return text;
}

//Enables isScrolling, but also sets the scrollSpeed! Useful for dramatic text effect, when the player dies in engine.cpp, the scrolling speed is slower than the normal battle text.
void Textbox::enableScrolling(float speed) {
    isScrolling = true;
    scrollSpeed = speed;
    scrollOffset = 0.0f; // Reset scroll offset
}

//Change isScrolling boolean so that text doesn't scroll (future use, maybe for alert text)
void Textbox::disableScrolling() {
    isScrolling = false;
    scrollOffset = 0.0f;
}

float Textbox::getLeft() const   { return pos.x - (size.x / 2); }
float Textbox::getRight() const  { return pos.x + (size.x / 2); }
float Textbox::getTop() const    { return pos.y + (size.y / 2); }
float Textbox::getBottom() const { return pos.y - (size.y / 2); }

void Textbox::setProjection(const mat4& proj) {
    projection = proj;
}

//Pretty self explanatory
void Textbox::close() {
    isVisible = false;
}

void Textbox::open() {
    isVisible = true;
}
//Used same overlap as Rect.cpp. Without implementing it I got thrown some errors.
bool Textbox::isOverlapping(const Shape& other) const {
    const Rect* otherRect = dynamic_cast<const Rect*>(&other);
    if (otherRect) {
        return !(getRight() < otherRect->getLeft() ||
                 getLeft() > otherRect->getRight() ||
                 getBottom() > otherRect->getTop() ||
                 getTop() < otherRect->getBottom());
    }
    return false;
}
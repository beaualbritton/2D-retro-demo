#ifndef GRAPHICS_TEXTBOX_H
#define GRAPHICS_TEXTBOX_H

#include "shape.h"
#include "triangle.h"
#include "rect.h"
#include "../shader/shader.h"
#include "../font/fontRenderer.h"
#include <string>
#include <memory>

//textbox inherits shape
class Textbox : public Shape {

private:
    //setting these string to mutable, will be later modified in an inherited draw() method (which is inherited as const)
    mutable string text, overflowText;
    //Text color using color struct
    color textColor;
    //Font renderer and textShader for text rendering, using some snippets from M4GP-Confetti-Button
    std::unique_ptr<FontRenderer> fontRenderer;
    Shader& textShader;
    //"Indicator" just a nice detail to show when text is over
    Triangle indicator;


    //These fields will be used in draw for scrolljng/wrapping text
    float fontSize;
    //flags for scrolling and open() close() functionality
    bool isScrolling;
    bool isVisible;
    float scrollSpeed;
    float scrollOffset;
    //Float to track time for scrolling draw
    mutable float scrollElapsedTime;
    //Tracking amount of visible characters in draw()
    mutable int visibleCharacters;
    //Projection for fontrenderer see "font/fontRenderer.cpp"
    mat4 projection;

    //Initializes fontRenderer in .cpp declaration
    void initTextRendering(const string& fontPath);

public:
    mutable bool shouldClose;
    //Constructor that takes in shapeshader and textshader for rendering, and a font path. This wont change so constant.
    Textbox(Shader& shapeShader, Shader& textShader, vec2 pos, vec2 size, color bgColor,
            const string& fontPath = "../res/fonts/MxPlus_IBM_BIOS.ttf");


    void initVectors() override;

    //overriding shape's draw to call this classes draw with 0 as deltatime
    void draw() const override {
        draw(0.0f);
    }
    void draw(float deltaTime) const;

    //open and close for textbox, simple boolean logic
    void close();
    void open();


    //Setters and getters for Text, Scrolling, Projection (for font rendering)
    void setText(const string& newText);
    string getText() const;
    void enableScrolling(float speed = 50.0f);
    void disableScrolling();
    void setProjection(const mat4& proj);
    float getLeft() const override;
    float getRight() const override;
    float getTop() const override;
    float getBottom() const override;
    //Overlapping included for potential future use/implementation
    bool isOverlapping(const Shape& other) const override;
};

#endif // GRAPHICS_TEXTBOX_H
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
// First our files
#include "Effect.hpp"
#include "MenuItem.hpp"
#include "Julia.hpp"
#include "Mandlebrot.hpp"

// Then the SFML libraries
#include <SFML/Graphics.hpp>

// Lastly all the necessary standards
#include <vector>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <fstream>

// Useful constant
#define PI 3.14159265

// Make the effect's font available
const sf::Font* Effect::s_font = NULL;

// Keep track of our UI elements for easy drawing
std::vector<Slider*> sliders;
std::vector<Checkbox*> checkboxes;

// UI Mouse events
void onMenuMousePress(sf::Event event);
void onMenuMouseMove(sf::Event event);
void onMenuMouseRelease(sf::Event event);

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Create the openGl rendering context, not actually necessary
    sf::ContextSettings contextSettings;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML Shader", 
                                    sf::Style::Default, contextSettings);
    window.setVerticalSyncEnabled(true);

    // Load our font for all of the text
    sf::Font font;
    if (!font.loadFromFile("resources/sansation.ttf"))
        return EXIT_FAILURE;

    // Tell the Effect class to use the same font
    Effect::setFont(font);

    // Create the effects vector
    std::vector<Effect*> effects;

    // Create the fractals, and keep individual handles
    Julia * julia = new Julia;
    Mandlebrot * mandelbrot = new Mandlebrot;

    // Store the fractals in a vector for easy manipulation later
    effects.push_back(mandelbrot);
    effects.push_back(julia);

    // Keep track of the effect that the mouse is currently hovering over
    std::size_t currentEffect = 0;

    // Initialize them the effects, this loads the shaders from file
    for (std::size_t i = 0; i < effects.size(); ++i)
        effects[i]->load();

    ////////////////
    // Checkboxes //
    ////////////////

    // Load the texture for our red X
    sf::Texture checkboxCheckTexture;
    if (!checkboxCheckTexture.loadFromFile("resources/X.png", 
                                            sf::IntRect(0, 0, 150, 150)))
    {
        return EXIT_FAILURE;
    }
    checkboxCheckTexture.setSmooth(true);

    // Create the sprite for our checkbox check symbol
    sf::Sprite * checkboxCheck = new sf::Sprite(checkboxCheckTexture);
    checkboxCheck->setPosition(0, 520);
    checkboxCheck->scale(sf::Vector2f(0.2f, 0.2f));
    checkboxCheck->setColor(sf::Color(160, 0, 0));

    // Load our box texture for the checkbox
    sf::Texture checkboxTexture;
    if (!checkboxTexture.loadFromFile("resources/Box.png", 
                                        sf::IntRect(0, 0, 174, 174)))
    {
        return EXIT_FAILURE;
    }
    checkboxTexture.setSmooth(true);

    // Create the sprite for our checkboxes
    sf::Sprite * checkbox = new sf::Sprite(checkboxTexture);
    checkbox->setPosition(0, 520);
    checkbox->scale(sf::Vector2f(0.15f, 0.15f));



    // Make the checkboxes

    // Log log enable
    sf::Text* logText = new sf::Text("Log Shading", font, 20);
    logText->setColor(sf::Color(80, 80, 80));

    Checkbox * logCheckbox = new Checkbox(checkbox, checkboxCheck,
                                            logText, "LogShading");
    logCheckbox->setPosition(10, 980);
    logCheckbox->setChecked(true);


    // Iterations scaling
    sf::Text* iterationsText = new sf::Text("Scale Iterations", font, 20);
    iterationsText->setColor(sf::Color(80, 80, 80));

    Checkbox * iterCheckbox = new Checkbox(checkbox, checkboxCheck, 
                                            iterationsText, "ScaleIterations");
    iterCheckbox->setPosition(300, 980);
    iterCheckbox->setChecked(true);


    // Almond bread
    sf::Text* almondBreadText = new sf::Text("Almond Bread", font, 20);
    almondBreadText->setColor(sf::Color(80, 80, 80));

    Checkbox * almondBread = new Checkbox(checkbox, checkboxCheck, 
                                            almondBreadText, "AlmondBread");
    almondBread->setPosition(600, 980);
    almondBread->setChecked(false);


    // Emulate double precision
    sf::Text* emulateDoubleText = new sf::Text("Emulate Double", font, 20);
    emulateDoubleText->setColor(sf::Color(80, 80, 80));

    Checkbox * emulateDouble = new Checkbox(checkbox, checkboxCheck, 
                                            emulateDoubleText, "EmulateDouble");
    emulateDouble->setPosition(900, 980);
    emulateDouble->setChecked(false);
    

    /////////////
    // Sliders //
    /////////////

    // Load the slider circle
    sf::Texture sliderButtonTexture;
    if (!sliderButtonTexture.loadFromFile("resources/SliderButton.png", 
                                            sf::IntRect(0, 0, 35, 35)))
    {
        return EXIT_FAILURE;
    }
    sliderButtonTexture.setSmooth(true);
    sf::Sprite * sliderButton = new sf::Sprite(sliderButtonTexture);
    sliderButton->scale(sf::Vector2f(0.5f, 0.5f));

    // Make the sliders
    Slider * redSlider = new Slider(sliderButton, 100, "Red Coefficient");
    redSlider->setPosition(1400, 980);
    redSlider->setColor(sf::Color(190, 40, 40));
    redSlider->setValue(0.1);

    Slider * blueSlider = new Slider(sliderButton, 100, "Blue Coefficient");
    blueSlider->setPosition(1400, 1000);
    blueSlider->setColor(sf::Color(40, 190, 40));
    blueSlider->setValue(0.32);

    Slider * greenSlider = new Slider(sliderButton, 100, "Green Coefficient");
    greenSlider->setPosition(1400, 1020);
    greenSlider->setColor(sf::Color(40, 40, 190));
    greenSlider->setValue(0.48);

    /////////////////
    // UI Elements //
    /////////////////

    // Populate checkboxes vector
    checkboxes.push_back(logCheckbox);
    checkboxes.push_back(almondBread);
    checkboxes.push_back(iterCheckbox);
    checkboxes.push_back(emulateDouble);

    // Populate sliders vector
    sliders.push_back(redSlider);
    sliders.push_back(blueSlider);
    sliders.push_back(greenSlider);

    // Create the instructions text
    sf::Text instructions("Press escape to quit.", font, 20);
    instructions.setPosition(1720, 1050);
    instructions.setColor(sf::Color(80, 80, 80));

    // Create the color coefficients text
    sf::Text colorLabel("Color Coefficients:", font, 20);
    colorLabel.setPosition(1200, 980);
    colorLabel.setColor(sf::Color(80, 80, 80));

    // Create the separators
    sf::RectangleShape bottomSeparator;
    bottomSeparator.setPosition(0.,1080.-120.);
    bottomSeparator.setSize(sf::Vector2f(1920.,1.));
    bottomSeparator.setFillColor(sf::Color(12, 12, 12));

    // Keep track of the current mouse coordinates
    float mouseX = 0.0, mouseY = 0.0;

    // Keep track of the frame of the current fractal
    sf::Vector3f currentFrame;

    // Start the game loop
    sf::Clock clock;
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle key-presses
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    // Escape key: exit
                    case sf::Keyboard::Escape:
                        window.close();
                        break;

                    // Zoom out
                    case sf::Keyboard::Dash:
                        currentFrame.z *= 1.04;
                        effects[currentEffect]->setFrame(currentFrame);
                        break;

                    // Zoom in
                    case sf::Keyboard::Equal:
                        currentFrame.z /= 1.04;
                        effects[currentEffect]->setFrame(currentFrame);
                        break;

                    default:
                        break;
                }
            }
            // Scroll wheel to zoom too
            if (event.type == sf::Event::MouseWheelMoved)
            {
                effects[currentEffect]->mouseScrolled(event);
            }

            // Handle mouse pressed events
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.y > 960.)
                {
                    onMenuMousePress(event);
                }
                else
                {
                    // Inform the current effect
                    effects[currentEffect]->mouseButtonPressed(event);

                    // Single click on the Mandelbrot
                    if (event.mouseButton.button == sf::Mouse::Left && 
                         currentEffect == 0)
                    {
                        sf::Vector3f frame;
                        // Rebase the mouse coordinates
                        float mx = event.mouseButton.x;
                        float my = 960.0-event.mouseButton.y;

                        // Get the current frame
                        frame = sf::Vector3f(effects[0]->getFrame());

                        // Transform the mouse to imaginary coordinates
                        float real = ((mx)*frame.z)/960.0 - frame.z/2.0-frame.x;
                        float imag = ((my)*frame.z)/960.0 - frame.z/2.0-frame.y;

                        // Update the julia fractal with new C values
                        julia->setJuliaC(sf::Vector2f(real, imag));
                    }
                }
            }

            // Handle mouse released events
            if (event.type == sf::Event::MouseButtonReleased)
            {
                // Inform the current effect
                if (event.mouseButton.y < 960. || 
                     effects[currentEffect]->isInteracting())

                    effects[currentEffect]->mouseButtonReleased(event);

                onMenuMouseRelease(event);

            }

            // Handle mouse moved events
            if (event.type == sf::Event::MouseMoved)
            {
                if (event.mouseMove.y > 960.)
                {
                    onMenuMouseMove(event);
                }
                else
                {
                    effects[currentEffect]->mouseMoved(event);

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && 
                         currentEffect == 0)
                    {
                        sf::Vector3f frame;
                        float mx = event.mouseMove.x;
                        float my = 960.0-event.mouseMove.y;

                        frame = sf::Vector3f(effects[0]->getFrame());

                        // Transform the mouse to imaginary coordinates
                        float real = ((mx)*frame.z)/960.0 - frame.z/2.0-frame.x;
                        float imag = ((my)*frame.z)/960.0 - frame.z/2.0-frame.y;

                        // Update the julia with new C values
                        julia->setJuliaC(sf::Vector2f(real, imag));
                    }
                    else
                    {
                        // Update the mouse coordinates
                        mouseX = event.mouseMove.x;
                        mouseY = event.mouseMove.y;
                    }
                }
            }
        }
        
        // Update the parameters for each of the fractals
        for (std::size_t i = 0; i < effects.size(); ++i)
        {
            effects[i]->update();
            effects[i]->set_almond(almondBread->isChecked());
            effects[i]->setLogShading(logCheckbox->isChecked());
            effects[i]->setIterationScaling(iterCheckbox->isChecked());
            effects[i]->setEmulated(emulateDouble->isChecked());
            effects[i]->setColoring(sf::Vector3f(redSlider->getValue(),
                                                  greenSlider->getValue(), 
                                                   blueSlider->getValue()));
        }

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw the shaders
        for (std::size_t i = 0; i < effects.size(); ++i)
            window.draw(*effects[i]);

        // Create the description text
        char temp[256];
        currentFrame = effects[currentEffect]->getFrame();

        // Get the C values of the current Julia
        sf::Vector2f juliaC = julia->getJuliaC();

        // Calculate the number of iterations
        int maxItValue=70;
        if (iterCheckbox->isChecked())
            maxItValue = sqrt(2.*sqrt(fabs(1.-sqrt(5./currentFrame.z))))*66.5;

        // Create the status string
        sprintf(temp,"X: %f Y: %f Zoom: %f A: %f B: %f Iterations: %d", 
                 currentFrame.x, currentFrame.y, currentFrame.z, 
                  juliaC.x, juliaC.y, maxItValue);

        // Draw the status text
        sf::Text description(temp, font, 20);
        description.setPosition(10, 1050);
        description.setColor(sf::Color(0, 80, 80));

        // Draw the text
        window.draw(instructions);
        window.draw(description);
        window.draw(bottomSeparator);
        window.draw(colorLabel);

        // Draw the checkboxes
        for(std::size_t i = 0; i < checkboxes.size(); i++)
            window.draw(*checkboxes[i]);

        // Draw the sliders
        for(std::size_t i = 0; i < sliders.size(); i++)
            window.draw(*sliders[i]);

        // If we are interacting with a fractal, dont change to the other one.
        if (!effects[currentEffect]->isInteracting())
        {
            // We are over the mandelbrot
            if (mouseX < 960 && mouseY < 960)
            {
                currentEffect = 0;
            }
            // We are over the julia
            else if (mouseX > 960 && mouseY < 960)
            {
                currentEffect = 1;
            }
        }

        // Finally, display the rendered frame on screen
        window.display();
    }

    // Delete the effects
    for (std::size_t i = 0; i < effects.size(); ++i)
        delete effects[i];
    // And the checkboxes
    for (std::size_t i = 0; i < checkboxes.size(); ++i)
        delete checkboxes[i];
    // And the sliders
    for (std::size_t i = 0; i < sliders.size(); ++i)
        delete sliders[i];

    return EXIT_SUCCESS;
}

// Update all relevant effects on a mouse press event
void onMenuMousePress(sf::Event event)
{
    for (std::size_t i = 0; i < checkboxes.size(); ++i)
        checkboxes[i]->onMousePress(event.mouseButton.x, event.mouseButton.y);

    for (std::size_t i = 0; i < sliders.size(); ++i)
        sliders[i]->onMousePress(event.mouseButton.x, event.mouseButton.y);
}

// Update all relevant effects on a mouse move event, only needed by sliders
void onMenuMouseMove(sf::Event event)
{
    for (std::size_t i = 0; i < sliders.size(); ++i)
        sliders[i]->onMouseMove(event.mouseMove.x, event.mouseMove.y);
}

// Update all relevant effects on a mouse release event, only needed by sliders
void onMenuMouseRelease(sf::Event event)
{
    for (std::size_t i = 0; i < sliders.size(); ++i)
        sliders[i]->onMouseRelease(event.mouseButton.x, event.mouseButton.y);
}
////////////////////////////////////////////////////////////
// "Julia" fragment shader
////////////////////////////////////////////////////////////
class Julia : public Effect
{
public :

    Julia() :
    Effect("julia")
    {
    }

    bool onLoad()
    {
        // Load the shaders
        if (!m_emulated_shader.loadFromFile(
             "shaders/Emulated_Julia_Mandlebrot.frag", 
               sf::Shader::Fragment))

            return false;

        if (!m_normal_shader.loadFromFile("shaders/Julia_Mandlebrot.frag",
               sf::Shader::Fragment))

            return false;

        // Initialize the julia constants
        juliaA = 0.0;
        juliaB = 0.0;

        return true;
    }

    void onUpdate()
    {
        // Update the frame if we are panning
        if (m_panning)
        {
            frame.x += m_panVelocity * cos(m_panAngle);
            frame.y += m_panVelocity * sin(m_panAngle);
        }


        // Calculate the max iterations
        float maxItValue=70.0;
        if (m_iterationsScaing)
            maxItValue = sqrt(2.*sqrt(fabs(1.-sqrt(5./frame.z))))*66.5;

        // Enable the correct shader
        if (m_emulated)
            m_shader = &m_emulated_shader;
        else
            m_shader = &m_normal_shader;        

        // Update the shader parameters
        m_shader->setParameter("MaxIterations", maxItValue);
        m_shader->setParameter("Zoom", frame.z);
        m_shader->setParameter("JuliaA", juliaA );
        m_shader->setParameter("JuliaB", juliaB );

        m_shader->setParameter("Julia", true );

        m_shader->setParameter("R", m_coloring.x);
        m_shader->setParameter("G", m_coloring.y);
        m_shader->setParameter("B", m_coloring.z);

        m_shader->setParameter("Almond", m_almond);
        m_shader->setParameter("LogShading", m_logShading);

        // Create a rectangle to draw on the screen
        sf::Vector2u renderTargetSize(960,960);
        const sf::Vector2f windowSize(
            static_cast<unsigned int>(renderTargetSize.x),
             static_cast<unsigned int>(renderTargetSize.y));
        
        m_screenRect.setPosition(960, 0);
        m_screenRect.setSize(windowSize);

        m_shader->setParameter("Xcenter", frame.x);
        m_shader->setParameter("Ycenter", frame.y);

        // Are we currently interacting with this fractal
        m_interacting = m_panning || m_zooming;
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Render the shader
        states.shader = m_shader;
        target.draw(m_screenRect, states);

        if (m_zooming)
        {
            // Draw the box for zooming
            target.draw(m_zoomBox);
        }
    }

    void setJuliaC(sf::Vector2f coords)
    {
        juliaA = coords.x;
        juliaB = coords.y;
    }

    sf::Vector2f getJuliaC()
    {
        return sf::Vector2f(juliaA, juliaB);
    }

    // Mouse button events
    void onMouseButtonRelease(sf::Event event)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            m_zooming = false;
            sf::Vector2f position = m_zoomBox.getPosition() -
                                        sf::Vector2f(960.0, 0);

            float mouseX = fmin(event.mouseButton.x,1920.0)-960.0;
            float mouseY = fmin(event.mouseButton.y,960.0);
            float newSize = fmax(mouseX-position.x, mouseY-position.y);

            setFrame(getFrame(position.x, position.y, newSize));
        }
        else if (event.mouseButton.button == sf::Mouse::Middle)
        {
            m_panning = false;
            m_panVelocity = 0.0;
        }
    }

private:

    sf::Texture m_texture;
    sf::Shader * m_shader;

    sf::Shader m_emulated_shader;
    sf::Shader m_normal_shader;

    sf::RectangleShape m_screenRect;

    float juliaA, juliaB;
};
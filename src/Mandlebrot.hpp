////////////////////////////////////////////////////////////
// "Mandlebrot" fragment shader
////////////////////////////////////////////////////////////
class Mandlebrot : public Effect
{
public :

    Mandlebrot() :
    Effect("mandlebrot")
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
        m_shader->setParameter("LogShading", m_logShading);
        m_shader->setParameter("Zoom", frame.z);
        m_shader->setParameter("Almond", m_almond);

        m_shader->setParameter("Julia", false);

        m_shader->setParameter("R", m_coloring.x);
        m_shader->setParameter("G", m_coloring.y);
        m_shader->setParameter("B", m_coloring.z);

        // Create a rectangle to draw on the screen
        sf::Vector2u renderTargetSize(960,960);
        const sf::Vector2f windowSize(
            static_cast<unsigned int>(renderTargetSize.x),
             static_cast<unsigned int>(renderTargetSize.y));
        
        m_screenRect.setPosition(0, 0);
        m_screenRect.setSize(windowSize);

        m_shader->setParameter("Xcenter", frame.x);
        m_shader->setParameter("Ycenter", frame.y);

        // Are we currently interacting with this fractal
        m_interacting = m_panning || m_zooming;
    }

    void onDraw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.shader = m_shader;
        target.draw(m_screenRect, states);

        if (m_zooming)
        {
            target.draw(m_zoomBox);
        }
    }

    void onMouseButtonRelease(sf::Event event)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            m_zooming = false;
            sf::Vector2f position = m_zoomBox.getPosition();

            float mouseX = fmin(event.mouseButton.x,960.0);
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
    sf::Shader * m_shader;
    sf::Shader m_emulated_shader;
    sf::Shader m_normal_shader;

    sf::RectangleShape m_screenRect;
};
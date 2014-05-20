#include <iostream>
#include <sstream>

// Classes for the UI items

class Checkbox : public sf::Drawable
{
public :
    
    Checkbox(sf::Sprite* box, sf::Sprite* check,
              sf::Text * text, std::string name)
    {
        m_box = box;
        m_check = check;
        m_text = text;
        m_name = name;

        m_checked = true;

        x = 0;
        y = 980;
    }

    std::string getName()
    {
        return m_name;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        m_box->setPosition(x,y);
        target.draw(*m_box);
        m_text->setPosition(x+30,y);
        target.draw(*m_text);
        if (m_checked)
        {
            m_check->setPosition(x-2,y);
            target.draw(*m_check);
        }
    }

    void setPosition(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void onMousePress(int _x, int _y)
    {
        if (_x > x && _x < x+30 && _y > y && _y < y+30)
            m_checked^=1;
    }

    bool isChecked()
    {
        return m_checked;
    }

    void setChecked(bool check)
    {
        m_checked = check;
    }

    int x, y;

private :
    sf::Sprite* m_box;
    sf::Sprite* m_check;
    sf::Text* m_text;

    std::string m_name;

    bool m_checked;
};

class Slider : public sf::Drawable
{
public :
    
    Slider(sf::Sprite* button, int length, std::string name)
    {
        m_button = button;
        m_length = length;        
        m_name = name;

        m_value = 0.5f;

        m_x = 0;
        m_y = 0;
        m_dragging = false;

        m_bar.setSize(sf::Vector2f(length,1.0));
        m_bar.setFillColor(sf::Color(93, 93, 93));
        m_bar.setOutlineColor(sf::Color(93, 93, 93));
        m_bar.setOutlineThickness(0.5);
        m_bar.setPosition(m_x,m_y);
    }

    std::string getName()
    {
        return m_name;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_bar);

        m_button->setPosition(m_x + m_length * m_value - 8, m_y - 8);
        m_button->setColor(m_color);
        target.draw(*m_button);
    }

    void setPosition(int _x, int _y)
    {
        m_x = _x;
        m_y = _y;
        m_bar.setPosition(m_x,m_y);
    }

    void onMousePress(int _x, int _y)
    {
        if (_x > m_x + m_length * m_value - 8 &&
             _x < m_x + m_length * m_value - 8 + 17 &&
              _y > m_y - 8 &&
               _y < m_y -8 + 17)
            m_dragging = true;
    }

    void onMouseMove(int _x, int _y)
    {
        if (m_dragging)
        {
            m_value = fmax(fmin((_x - m_x)/((float)m_length), 1.0f), 0.0f);
        }
    }

    void onMouseRelease(int _x, int _y)
    {
        m_dragging = false;
    }

    bool isDragging()
    {
        return m_dragging;
    }

    void setValue(float value)
    {
        m_value = value;
    }

    float getValue()
    {
        return m_value;
    }

    void setColor(sf::Color color)
    {
        m_color = color;
    }

private :
    sf::Sprite* m_button;
    sf::RectangleShape m_bar;
    sf::Color m_color;

    std::string m_name;

    int m_length;
    float m_value;

    int m_x, m_y;

    bool m_dragging;
};

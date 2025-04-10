#ifndef TEXT_OBJECT_HPP
#define TEXT_OBJECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Color.hpp"

class TextObject : public Util::GameObject {
public:
    explicit TextObject(const std::string& text, int size=35) : GameObject(
            std::make_unique<Util::Text>(GA_RESOURCE_DIR"/Font/Inkfree.ttf", size,
                                         text, Util::Color::FromName(Util::Colors::WHITE)),
            100) {
        m_Transform.translation = {0.0, 0.0};
    }
    TextObject(const TextObject&) = delete;
    TextObject(TextObject&&) = delete;
    TextObject& operator=(const TextObject&) = delete;
    TextObject& operator=(TextObject&&) = delete;

    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool GetVisibility() const { return m_Visible; }

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }
    void SetInversion() { m_Transform.scale.x *= -1; }

    void SetText(const std::string& text) const {
        const auto temp = std::dynamic_pointer_cast<Util::Text>(m_Drawable);
        temp->SetText(text);
    }

private:
    inline static std::string append_string_views(std::string_view sv1, std::string_view sv2) {
        return std::string(sv1) + "\n" + std::string(sv2);
    }

};
#endif //TEXT_OBJECT_HPP

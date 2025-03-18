#ifndef EDGEMODIFIER_HPP
#define EDGEMODIFIER_HPP

#include "Effect/Shape/BaseShape.hpp"

namespace Effect {
    namespace Modifier {

        // 邊緣效果類型枚舉
        enum class EdgeType {
            NONE,       // 無特殊邊緣效果
            DARK,       // 邊緣加深
            GLOW        // 邊緣發光
        };

        class EdgeModifier {
        public:
            EdgeModifier(EdgeType type = EdgeType::NONE, float width = 0.05f, const Util::Color& edgeColor = Util::Color::FromName(Util::Colors::PINK));

            // 應用邊緣效果
            void Apply(Core::Program& program);

            // 設置參數
            void SetEdgeType(EdgeType type) { m_EdgeType = type; }
            EdgeType GetEdgeType() const { return m_EdgeType; }

            void SetWidth(float width) { m_Width = width; }
            float GetWidth() const { return m_Width; }

            void SetEdgeColor(const Util::Color& color) { m_EdgeColor = color; }
            const Util::Color& GetEdgeColor() const { return m_EdgeColor; }

        private:
            EdgeType m_EdgeType;
            float m_Width;           // 邊緣寬度
            Util::Color m_EdgeColor; // 邊緣顏色

            GLint m_EdgeTypeLocation = -1;
            GLint m_EdgeWidthLocation = -1;
            GLint m_EdgeColorLocation = -1;
        };

    } // namespace Modifier
} // namespace Effect

#endif //EDGEMODIFIER_HPP

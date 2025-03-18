#ifndef FILLMODIFIER_HPP
#define FILLMODIFIER_HPP

#include "Effect/Shape/BaseShape.hpp"

namespace Effect {
    namespace Modifier {

        // 填充類型枚舉
        enum class FillType {
            SOLID,    // 實心
            HOLLOW    // 空心
        };

        class FillModifier {
        public:
            FillModifier(FillType type = FillType::SOLID, float thickness = 0.02f);

            // 應用填充效果
            void Apply(Core::Program& program);

            // 設置參數
            void SetFillType(FillType type) { m_FillType = type; }
            FillType GetFillType() const { return m_FillType; }

            void SetThickness(float thickness) { m_Thickness = thickness; }
            float GetThickness() const { return m_Thickness; }

        private:
            FillType m_FillType;
            float m_Thickness;   // 空心時的線條粗細

            GLint m_FillTypeLocation = -1;
            GLint m_ThicknessLocation = -1;
        };

    } // namespace Modifier
} // namespace Effect

#endif //FILLMODIFIER_HPP

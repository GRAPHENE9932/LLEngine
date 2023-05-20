#pragma once

#include "nodes/SpatialNode.hpp"

#include <memory>

namespace llengine {
enum class CustomPropertyType : std::uint8_t {
    INT64, FLOAT, VEC2, VEC3, VEC4, STRING,
    VECTOR_OF_INT64, VECTOR_OF_FLOAT, VECTOR_OF_VEC2,
    VECTOR_OF_VEC3, VECTOR_OF_VEC4, VECTOR_OF_STRING
};

namespace {
    class CustomNodeFactory {
    public:
        CustomNodeFactory() = default;

        template<typename T>
        static CustomNodeFactory construct_factory() {
            return {
                std::make_unique<ConcreteConstructor<T>>()
            };
        }

        [[nodiscard]] std::unique_ptr<SpatialNode> construct() {
            return abstract->construct();
        }

    private:
        class AbstractConstructor {
        public:
            virtual ~AbstractConstructor() = default;
            [[nodiscard]] virtual std::unique_ptr<SpatialNode> construct() = 0;
        };

        template<typename T>
        class ConcreteConstructor : public AbstractConstructor {
        public:
            std::unique_ptr<SpatialNode> construct() override {
                return std::make_unique<T>();
            }
        };

        CustomNodeFactory(std::unique_ptr<AbstractConstructor>&& abstract) :
            abstract(std::move(abstract)) {}

        std::unique_ptr<AbstractConstructor> abstract = nullptr;
    };

    void add_node_factory(const std::string& node_type_name, CustomNodeFactory&& factory);
    void add_custom_property(
        const std::string& node_type_name, CustomPropertyType type,
        const std::string& property_name, void(*abstract_setter_ptr)(SpatialNode*)
    );
}

template<typename T>
consteval CustomPropertyType get_custom_property_type() {
    if (std::is_same_v<T, std::int64_t>()) {
        return CustomPropertyType::INT64;
    }
    else if (std::is_same_v<T, float>()) {
        return CustomPropertyType::FLOAT;
    }
    else if (std::is_same_v<T, glm::vec2>()) {
        return CustomPropertyType::VEC2;
    }
    else if (std::is_same_v<T, glm::vec3>()) {
        return CustomPropertyType::VEC3;
    }
    else if (std::is_same_v<T, glm::vec4>()) {
        return CustomPropertyType::VEC4;
    }
    else if (std::is_same_v<T, std::string>()) {
        return CustomPropertyType::STRING;
    }
    else if (std::is_same_v<T, std::vector<std::int64_t>>()) {
        return CustomPropertyType::VECTOR_OF_INT64;
    }
    else if (std::is_same_v<T, std::vector<float>>()) {
        return CustomPropertyType::VECTOR_OF_FLOAT;
    }
    else if (std::is_same_v<T, std::vector<glm::vec2>>()) {
        return CustomPropertyType::VECTOR_OF_VEC2;
    }
    else if (std::is_same_v<T, std::vector<glm::vec3>>()) {
        return CustomPropertyType::VECTOR_OF_VEC3;
    }
    else if (std::is_same_v<T, std::vector<glm::vec4>>()) {
        return CustomPropertyType::VECTOR_OF_VEC4;
    }
    else if (std::is_same_v<T, std::vector<std::string>>()) {
        return CustomPropertyType::VECTOR_OF_STRING;
    }
    else {
        static_assert(true, "Invalid/unsupported property type provided.");
    }
}

struct CustomNodeType {
    struct CustomProperty {
        CustomPropertyType type;
        std::string name;
        void(*abstract_setter_ptr)(SpatialNode*);

        template<typename T>
        void set_property(SpatialNode* node_ptr, const T& value) const {
            if (get_custom_property_type<T>() != type) {
                throw std::runtime_error("Tried to call custom property setter with invalid value type.");
            }

            auto setter_ptr = reinterpret_cast<void(*)(SpatialNode*, const T&)>(abstract_setter_ptr);
            setter_ptr(node_ptr, value);
        }
    };

    CustomNodeFactory factory;
    std::vector<CustomProperty> properties;
};

template<typename T>
void register_node_type(const std::string& node_type_name) {
    add_node_factory(node_type_name, CustomNodeFactory::construct_factory<T>());
}

template<typename T>
void register_custom_property(
    const std::string& node_type_name,
    const std::string& property_name,
    void(*setter_ptr)(SpatialNode*, T)
);
}
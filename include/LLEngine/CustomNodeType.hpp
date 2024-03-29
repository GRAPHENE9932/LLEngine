#pragma once

#include "nodes/Node.hpp"

#include <memory>
#include <stdexcept>
#include <typeindex>
#include <functional>
#include <string_view>
#include <map>

namespace llengine {
namespace internal {
class PropertySetter {
public:
    template<typename T>
    [[nodiscard]] static PropertySetter create(std::function<void(T&, const NodeProperty& property)> function) {
        return PropertySetter(std::make_unique<ConcreteHolder<T>>(function));
    }

    void call(Node& node, const NodeProperty& property) const {
        holder->call(node, property);
    }

private:
    class Holder {
    public:
        virtual ~Holder() = default;
        virtual void call(Node&, const NodeProperty& property) const = 0;
    };

    template<typename T>
    class ConcreteHolder : public Holder {
    public:
        ConcreteHolder(std::function<void(T&, const NodeProperty& property)> function) :
            function(function) {}

        void call(Node& node, const NodeProperty& property) const {
            function(dynamic_cast<T&>(node), property);
        }

    private:
        std::function<void(T&, const NodeProperty& property)> function;
    };

    std::unique_ptr<Holder> holder = nullptr;

    PropertySetter(std::unique_ptr<Holder>&& holder) :
        holder(std::move(holder)) {}
};
}

class CustomNodeType {
public:
    template<typename T, typename Base>
    static CustomNodeType create_from_type() {
        return CustomNodeType(std::make_unique<ConcreteHolder<T>>(), typeid(Base));
    }

    template<typename T>
    static CustomNodeType create_from_type() {
        return CustomNodeType(std::make_unique<ConcreteHolder<T>>());
    }

    [[nodiscard]] std::unique_ptr<Node> construct_node() const;
    void add_setter(std::string_view name, internal::PropertySetter&& setter);
    bool call_setter(Node& node, const NodeProperty& property) const;
    [[nodiscard]] bool has_setter_for(std::string_view name) const;
    [[nodiscard]] bool is_parent_of(const CustomNodeType& other) const;
    [[nodiscard]] std::type_index get_type_index() const;
    void assign_parent(CustomNodeType* parent);
    [[nodiscard]] CustomNodeType* get_parent() const;

    [[nodiscard]] bool operator==(const CustomNodeType& other) const;
    [[nodiscard]] bool operator!=(const CustomNodeType& other) const;

private:
    class Holder {
    public:
        virtual ~Holder() = default;
        [[nodiscard]] virtual std::unique_ptr<Node> construct() const = 0;
        [[nodiscard]] virtual const std::type_info& get_type_info() const = 0;
    };

    template<typename T>
    class ConcreteHolder : public Holder {
    public:
        [[nodiscard]] std::unique_ptr<Node> construct() const override final  {
            if constexpr (!std::is_abstract_v<T>) {
                return std::make_unique<T>();
            }
            else {
                throw std::runtime_error("Can't construct an abstract node class.");
            }
        }
        [[nodiscard]] const std::type_info& get_type_info() const override final {
            return typeid(T);
        }
    };

    std::unique_ptr<Holder> holder = nullptr;
    std::map<std::string, internal::PropertySetter> property_setters;
    std::variant<std::type_index, CustomNodeType*> parent;

    CustomNodeType(std::unique_ptr<Holder>&& holder, std::type_index base_type_index) :
        holder(std::move(holder)), parent(base_type_index) {}
    CustomNodeType(std::unique_ptr<Holder>&& holder) :
        holder(std::move(holder)), parent(nullptr) {}

    [[nodiscard]] bool has_own_setter_for(std::string_view name) const;
};
}
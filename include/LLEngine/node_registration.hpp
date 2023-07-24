#pragma once

#include "nodes/Node.hpp"

#include <functional>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <vector>
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

class CustomNodeType {
public:
    template<typename T, typename Base>
    static CustomNodeType create_from_type() {
        return CustomNodeType(std::make_unique<ConcreteHolder<T>>(), typeid(Base));
    }

    [[nodiscard]] std::unique_ptr<Node> construct_node() const;
    void add_setter(std::string_view name, PropertySetter&& setter);
    bool call_setter(Node& node, const NodeProperty& property);
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
    std::map<std::string, PropertySetter> property_setters;
    std::variant<std::type_index, CustomNodeType*> parent;

    CustomNodeType(std::unique_ptr<Holder>&& holder, std::type_index base_type_index) :
        holder(std::move(holder)), parent(base_type_index) {}
    
    [[nodiscard]] bool has_own_setter_for(std::string_view name) const;
};

void add_node_type(std::string_view node_type_name, CustomNodeType&& node_type);
void add_node_setter(std::string_view node_type_name, std::string_view property_name, PropertySetter&& setter);
void register_builtin_nodes();

template<typename T>
concept has_register_properties = std::is_same_v<decltype(T::register_properties()), void>;
}

void begin_nodes_registration();
void end_nodes_registration();

template<typename T, typename Base>
void register_node_type(std::string_view node_type_name) {
    internal::add_node_type(node_type_name, internal::CustomNodeType::create_from_type<T, Base>());
    if constexpr (internal::has_register_properties<T>) {
        T::register_properties();
    }
}

template<typename T>
void register_custom_property(
    std::string_view node_type_name,
    std::string_view property_name,
    std::function<void(T&, const NodeProperty&)> setter
) {
    internal::add_node_setter(node_type_name, property_name, internal::PropertySetter::create<T>(setter));
}

[[nodiscard]] std::unique_ptr<Node> construct_node(std::string_view node_type_name);
[[nodiscard]] std::unique_ptr<Node> construct_node(std::string_view node_type_name, const std::vector<NodeProperty>& properties);
void call_setter(std::string_view node_type_name, const NodeProperty& property, Node& node);
[[nodiscard]] std::optional<std::string_view> find_node_type_name(const Node& node);
}
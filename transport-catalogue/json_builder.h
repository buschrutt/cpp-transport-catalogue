#pragma once
#include "json.h"
#include <optional>
#include <utility>

namespace json {

    class Builder {

    public:

        class ValueKeyContext;
        class ArrayContext;
        class DictContext;

        class KeyContext{
        public:
            explicit KeyContext(Builder * builder): builder_(builder){};
            ValueKeyContext& Value(const Node::Value& node_value);
            ArrayContext& StartArray();
            DictContext& StartDict();
            KeyContext GetContext (){
                return *this;
            }
            Builder * GetBuilder (){
                return builder_;
            }
        private:
            Builder * builder_{};
        };

        class BaseContext{
        public:
            explicit BaseContext(Builder * builder): builder_(builder){};
        protected:
            Builder * builder_{};
        };

        class ValueKeyContext : public BaseContext {
        public:
            explicit ValueKeyContext(Builder *builder) : BaseContext(builder) {}
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
            ValueKeyContext& GetContext (){
                return *this;
            }
            Builder * GetBuilder (){
                return builder_;
            }
        };

        class ValueArrayContext : public BaseContext {
        public:
            explicit ValueArrayContext(Builder *builder) : BaseContext(builder) {}
            ValueArrayContext& Value(const Node::Value& node_value);
            DictContext& StartDict();
            ArrayContext& StartArray();
            Builder& EndArray();
            ValueArrayContext& GetContext (){
                return *this;
            }
            Builder * GetBuilder (){
                return builder_;
            }
        };

        class ValueFullContext : public ValueArrayContext {
        public:
            explicit ValueFullContext(Builder *builder) : ValueArrayContext(builder) {}
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
            json::Node Build();
            ValueFullContext& GetFullContext (){
                return *this;
            }
            Builder * GetFullBuilder (){
                return builder_;
            }
        };

        class DictContext{
        public:
            explicit DictContext(Builder * builder): builder_(builder){};
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
            DictContext GetContext (){
                return *this;
            }
            Builder * GetBuilder (){
                return builder_;
            }
        private:
            Builder * builder_{};
        };

        class ArrayContext{
        public:
            explicit ArrayContext(Builder * builder): builder_(builder){};
            ValueArrayContext& Value(const Node::Value& node_value);
            DictContext& StartDict();
            ArrayContext& StartArray();
            Builder& EndArray();
            ArrayContext GetContext (){
                return *this;
            }
            Builder * GetBuilder (){
                return builder_;
            }
        private:
            Builder * builder_{};
        };

        Builder& BuildKey(const std::string &node_key);
        KeyContext Key(const std::string& node_key) {
            KeyContext key_context(&BuildKey(node_key));
            return key_context.GetContext();
        }

        Builder& BuildValue(const Node::Value& node_value);
        Builder& Value(const Node::Value& node_value) {
            if (is_previous_key_){
                ValueKeyContext value_context( &BuildValue(node_value));
                return *value_context.GetBuilder();
            } else if (!node_stack_.empty()){
                if (node_stack_.back()->IsArray()){
                    ValueArrayContext value_context( &BuildValue(node_value));
                    return *value_context.GetBuilder();
                }
            }
            ValueFullContext value_context( &BuildValue(node_value));
            return *value_context.GetBuilder();
        }

        Builder& BuildStartDict();
        DictContext StartDict() {
            DictContext dict_context(&BuildStartDict());
            return dict_context.GetContext();
        }

        Builder& BuildStartArray();
        ArrayContext StartArray() {
            ArrayContext array_context(&BuildStartArray());
            return array_context.GetContext();
        }

        Builder& EndDict();

        Builder& EndArray();

        json::Node Build();

        json::Node GetRoot(){
            return root_;
        }
        json::Node * GetRootPtr() {
            return root_ptr_;
        }
        std::vector<json::Node*> GetNodeStack(){
            return node_stack_;
        }
        std::string GetKey() {
            return key_;
        }
        [[nodiscard]] bool GetIsKey() const {
            return is_previous_key_;
        }
        void SetRoot(json::Node root){
            root_ = std::move(root);
        }
        void SetRootPtr(json::Node * root_ptr) {
            root_ptr_ = root_ptr;
        }
        void EmplaceNode(json::Node* node){
            node_stack_.emplace_back(node);
        }
        void PopNode(){
            node_stack_.pop_back();
        }
        void SetKey(std::string key){
            key_ = std::move(key);
        }
        void SetIsKey(bool flag) {
            is_previous_key_ = flag;
        }

    private:
        json::Node root_;
        json::Node * root_ptr_;
        std::vector<json::Node*> node_stack_; // pointers to Nodes that aren't built yet
        std::string key_; // need key to complete pair
        bool is_previous_key_ = false; // in few cases if the last call was to key has to be checked
    };

}
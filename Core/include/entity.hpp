#pragma once
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <stack>
#include <tuple>
#include <memory>
#include <cstring>
#include <iterator>

namespace SGE { namespace Components {
    using Entity = uint64_t;
    using Byte = std::byte;

    constexpr Entity NULL_ENTITY=-1;

    class Signature {
    public:
        using TYPE = uint64_t;
        Signature()=default;
        template<class... ARGS>
        void sign(uint64_t bit, ARGS... args) {
            if (bit>=m_data.size()*sizeof(TYPE))
                m_data.resize(bit/sizeof(TYPE)+1, 0);
            m_data[bit/sizeof(TYPE)] |= (0x1 << (bit%sizeof(TYPE)));
            if constexpr (sizeof...(ARGS)>0)
                sign(args...);
        }
        inline void unsign(uint64_t bit) {
            if (bit>=m_data.size()*sizeof(TYPE)) return;
             m_data[bit/sizeof(TYPE)] &= !(0x1 << (bit%sizeof(TYPE)));           
        }
        template<class... ARGS>
        bool has_sign(uint64_t bit, ARGS... args) {
            Signature s;
            s.sign(bit, args...);
            auto l = m_data.size()<s.m_data.size()? m_data.size(): s.m_data.size();
            for (unsigned i=0; i<l; ++i) {
                if ((m_data[i] & s.m_data[i]) == 0)
                    return false;
            }
            return true;
        }
    private:
        std::vector<TYPE> m_data;
    } ;

    template<class TYPE>
    struct Component {
        Entity entity;
        TYPE data;
    } ;
    class Register ;
    class EntityObject {
    public:
        EntityObject() = default;
    private:
        Register* m_owner=nullptr;
        Entity m_entity;
    } ;

    class Register {
    public:
        Register()=default;
        
        Entity make_entity() {
            Entity r;
            if (m_entityPool.empty())
                r = m_entityCounter++;
            else {
                auto e = m_entityPool.top();
                m_entityPool.pop();
                r = e;
            }
            if (r>=m_entitySparse.size()) m_entitySparse.resize(r+1);
            m_entitySparse[r] = m_entityDense.size();
            m_entityDense.emplace_back(r);
            return r;
        }
        template<class... ARGS>
        Entity construct_entity(ARGS&&... args) {
            auto e = make_entity();
            push(e, std::forward<ARGS>(args)...);
            return e;
        }
        template<class TYPE, class... ARGS>
        void registry() {
            if (m_typeIndices.find(typeid(TYPE)) == m_typeIndices.end()) {
                m_typeIndices.emplace(typeid(TYPE), m_typeCounter++);
                m_dense.emplace_back();
                m_sparse.emplace_back();
                m_typeSizes.emplace_back(sizeof(TYPE), sizeof(Component<TYPE>));
            }

            if constexpr (sizeof...(ARGS)>0)
                registry<ARGS...>();
        }
        template<class TYPE>
        Entity get_type() {
            registry<TYPE>();
            return m_typeIndices.at(typeid(TYPE));
        }
        template<class TYPE, class... ARGS>
        void push(Entity entity, TYPE&& data, ARGS&&... args) {
            registry<TYPE>();
            if (entity>=m_sparse[get_type<TYPE>()].size()) {
                m_sparse[get_type<TYPE>()].resize(entity+1);
            }
            if (entity>=m_signatures.size()) {
                m_signatures.resize(entity+1);
            }
            auto s = m_dense[get_type<TYPE>()].size();
            std::size_t cs = sizeof(Component<TYPE>);
            m_dense[get_type<TYPE>()].resize(s+cs);
            auto& d = (reinterpret_cast<Component<TYPE>*>(m_dense[get_type<TYPE>()].data()))[s/cs];
            d.data = data;
            d.entity = entity;
            m_sparse[get_type<TYPE>()][entity] = s/cs;
            m_signatures[entity].sign(get_type<TYPE>());
            if constexpr (sizeof...(ARGS) > 0)
                push(entity, std::forward<ARGS>(args)...);
        }
        template<class TYPE, class... ARGS>
        void remove(Entity entity) {
            auto& target = reinterpret_cast<Component<TYPE>*>(m_dense[get_type<TYPE>()].data())[m_sparse[get_type<TYPE>()][entity]];
            auto& last = *(reinterpret_cast<Component<TYPE>*>(m_dense[get_type<TYPE>()].data()+m_dense[get_type<TYPE>()].size())-1);
            m_sparse[get_type<TYPE>()][last.entity] = m_sparse[get_type<TYPE>()][target.entity];
            m_sparse[get_type<TYPE>()][target.entity] = NULL_ENTITY;
            std::swap(target, last);
            m_dense[get_type<TYPE>()].resize(m_dense[get_type<TYPE>()].size()-sizeof(Component<TYPE>));

            m_entityPool.emplace(entity);
            m_signatures[entity].unsign(get_type<TYPE>());

            if constexpr (sizeof...(ARGS) > 0)
                remove<ARGS...>(entity);
        }
        void remove(Entity entity, unsigned typeIndex) {
            auto* target = (m_dense[typeIndex].data())+(m_sparse[typeIndex][entity]*m_typeSizes[typeIndex].compoSize);
            auto* last = (m_dense[typeIndex].data()+m_dense[typeIndex].size()-m_typeSizes[typeIndex].compoSize);
            m_sparse[typeIndex][*reinterpret_cast<Entity*>(last)] = m_sparse[typeIndex][*reinterpret_cast<Entity*>(target)];
            m_sparse[typeIndex][*reinterpret_cast<Entity*>(target)] = NULL_ENTITY;

            _swap(target, last, m_typeSizes[typeIndex].compoSize);

            m_dense[typeIndex].resize(m_dense[typeIndex].size()-m_typeSizes[typeIndex].compoSize);

            m_entityPool.emplace(entity);
            m_signatures[entity].unsign(typeIndex);
        }
        template<class... ARGS>
        std::tuple<ARGS&...> get(Entity entity) {
            return std::make_tuple(
                std::ref(
                    reinterpret_cast<Component<ARGS>*>(m_dense[get_type<ARGS>()].data())[
                    m_sparse[get_type<ARGS>()][entity]
                ].data
                )
                ...
            );
        }
        template<class... ARGS>
        bool has(Entity entity) {
            return m_signatures[entity].has_sign(get_type<ARGS>()...);
        }
        inline void destroy(Entity entity) {
            m_entitySparse[m_entityDense.back()] = m_entitySparse[entity];
            m_entitySparse[entity] = NULL_ENTITY;
            std::swap(m_entityDense.back(), m_entityDense[m_entitySparse[entity]]);
            m_entityDense.pop_back();
            for (unsigned i=0; i<m_dense.size(); ++i) {
                if (m_signatures[entity].has_sign(i)) {
                    remove(entity, i);
                }
            }
        }
    public:
        template<class... ARGS>
        class View {
        public:
            using TUPLE = std::tuple<ARGS&...>;
            class iterator {
            public:
                using iterator_categories = std::input_iterator_tag;
                using value_type = TUPLE;
                using difference_type = std::ptrdiff_t;
                using pointer = void;
                using reference = void;
            public:
                explicit iterator(Register& reg, Entity idx) : m_register(reg), m_idx(idx) {}
                value_type operator*() {
                    return m_register.get<ARGS...>(m_register.m_entityDense[m_idx]);
                }
                void operator++() {
                    ++m_idx;
                    if (m_idx>=m_register.m_entityDense.size()) m_idx = -1;
                }
                bool operator==(const iterator& other) const {
                    return &m_register==&other.m_register && m_idx==other.m_idx;
                }
                bool operator!=(const iterator& other) const {
                    return !((*this)==other);
                }
            private:
                Register& m_register;
                Entity m_idx;
            } ;
        public:
            explicit View(Register& reg) : m_register(reg) {}
            iterator begin() {
                if (m_register.m_entityDense.empty())
                    return iterator(m_register, 0);
                return iterator(m_register, m_register.m_entityDense.front());
            }
            iterator end() {
                if (m_register.m_entityDense.empty())
                    return iterator(m_register, 0);
                return iterator(m_register, -1);
            }
        private:
            Register& m_register;
        } ;
        template<class... ARGS>
        View<ARGS...> view() {
            return View<ARGS...>(*this);
        }
    private:
        struct SizeQuery {
            std::size_t typeSize;
            std::size_t compoSize;
            SizeQuery() = default;
            SizeQuery(std::size_t ts, std::size_t cs) : typeSize(ts), compoSize(cs) {}
        } ;
        void _swap(Byte* a, Byte* b, std::size_t size) {
            Byte tmp[size]; std::memcpy(tmp, a, size);
            std::memmove(a, b, size);
            std::memmove(b, tmp, size);
        }
    private:
        std::unordered_map<std::type_index, unsigned> m_typeIndices;
        std::vector<std::vector<Byte>> m_dense;
        std::vector<std::vector<Entity>> m_sparse;
        std::vector<SizeQuery> m_typeSizes;

        std::vector<Signature> m_signatures;

        inline static unsigned m_typeCounter=0;
        inline static Entity m_entityCounter=0;
        std::stack<Entity> m_entityPool;

        std::vector<Entity> m_entitySparse;
        std::vector<Entity> m_entityDense;
    } ;
}}
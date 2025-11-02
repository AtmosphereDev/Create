#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <vector>

template <typename T, typename U>
class RendererReloadCache : public std::enable_shared_from_this<RendererReloadCache<T, U>> {
public:
    using Factory = std::function<U(const T&)>;

    explicit RendererReloadCache(Factory factory)
        : factory_(std::move(factory)) {
        registerInstance();
    }

    U get(const T& key) {
        std::scoped_lock lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end())
            return it->second;

        U value = factory_(key);
        cache_.emplace(key, value);
        return value;
    }

    U operator()(const T& key) { return get(key); }

    void clear() {
        std::scoped_lock lock(mutex_);
        cache_.clear();
    }

    static void onReloadLevelRenderer() {
        std::scoped_lock lock(all_mutex_);
        for (auto it = all_.begin(); it != all_.end();) {
            if (auto inst = it->lock()) {
                inst->clear();
                ++it;
            } else {
                it = all_.erase(it); // clean up expired weak_ptrs
            }
        }
    }

private:
    Factory factory_;
    std::unordered_map<T, U> cache_;
    std::mutex mutex_;

    static inline std::vector<std::weak_ptr<RendererReloadCache<T, U>>> all_;
    static inline std::mutex all_mutex_;

    void registerInstance() {
        std::scoped_lock lock(all_mutex_);
        all_.push_back(this->weak_from_this());
    }
};

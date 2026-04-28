#pragma once
#include <vector>
#include <optional>
#include <memory>
#include "villager.hpp"

struct Connection {
    Villager *source = nullptr;
    Villager *destination = nullptr;
};
struct ConnectedVillager {
    std::vector<Connection*> nextConnections = std::vector<Connection*>();
    std::vector<Connection*> previousConnections = std::vector<Connection*>();
    Villager self = Villager();
};

class Village {
private:
    std::vector<std::optional<ConnectedVillager>> villagers = std::vector<std::optional<ConnectedVillager>>();
    std::vector<size_t> freeSlots = std::vector<size_t>();
    std::vector<std::unique_ptr<Connection>> connections = std::vector<std::unique_ptr<Connection>>();
public:
    Village();
    ~Village();

    size_t spawn(const Vec2 &position);
    void kill(size_t id);

    std::optional<size_t> connect(size_t sourceId, size_t destinationId);
    void disconnect(size_t sourceId, size_t destinationId);

    Villager *getVillagerMut(size_t id);
    const Villager *getVillager(size_t id) const;
    // TODO: Remove after debugging
    size_t getVillagersCount() const;

    const Connection *getConnection(size_t sourceId, size_t destinationId) const;

    template <typename Fn>
    void villagerIterate(Fn &&callback) const {
        for (size_t i = 0; i < this->villagers.size(); i++) {
            if (this->villagers[i].has_value()) {
                callback(this->villagers[i]->self, i);
            }
        }
    }
    template <typename Fn>
    void connectionIterate(Fn &&callback) {
        for (size_t i = 0; i < this->connections.size(); i++) {
            callback(this->connections[i].get(), i);
        }
    }
};
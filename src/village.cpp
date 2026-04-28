#include "village.hpp"
#include <algorithm>
#include <cstdio>

Village::Village() = default;
Village::~Village() = default;

size_t Village::spawn(const Vec2 &position) {
    if (!this->freeSlots.empty()) {
        const size_t id = this->freeSlots.back();
        this->freeSlots.pop_back();
        this->villagers[id].emplace();
        this->villagers[id]->self.position = position;
        return id;
    }
    std::optional<ConnectedVillager> &slot = this->villagers.emplace_back();
    slot.emplace();
    slot->self.position = position;
    return this->villagers.size() - 1;
}
void Village::kill(size_t id) {
    if (id >= this->villagers.size() || !this->villagers[id].has_value()) {
        return;
    }
    this->villagers[id].reset();
    this->freeSlots.push_back(id);
}

std::optional<size_t> Village::connect(size_t sourceId, size_t destinationId) {
    if (
        sourceId >= this->villagers.size() || !this->villagers[sourceId].has_value() ||
        destinationId >= this->villagers.size() || !this->villagers[destinationId].has_value()
    ) {
        return std::nullopt;
    }

    // Check if connection already exists
    std::vector<Connection*>::const_iterator existingConnectionIt = std::find_if(
        this->villagers[sourceId]->nextConnections.begin(),
        this->villagers[sourceId]->nextConnections.end(),
        [&](const Connection *connection) {
            return connection->destination == &this->villagers[destinationId]->self;
        }
    );
    if (existingConnectionIt != this->villagers[sourceId]->nextConnections.end()) {
        return std::nullopt; // Connection already exists
    }

    // Check if connection already exists in the opposite direction (to prevent backward connections)
    std::vector<Connection*>::const_iterator oppositeConnectionIt = std::find_if(
        this->villagers[destinationId]->nextConnections.begin(),
        this->villagers[destinationId]->nextConnections.end(),
        [&](const Connection *connection) {
            return connection->destination == &this->villagers[sourceId]->self;
        }
    );
    if (oppositeConnectionIt != this->villagers[destinationId]->nextConnections.end()) {
        return std::nullopt; // Connection already exists in the opposite direction
    }

    this->connections.push_back(std::make_unique<Connection>());
    Connection *connection = this->connections.back().get();
    connection->source = &this->villagers[sourceId]->self;
    connection->destination = &this->villagers[destinationId]->self;
    this->villagers[sourceId]->nextConnections.push_back(connection);
    this->villagers[destinationId]->previousConnections.push_back(connection);
    return this->connections.size() - 1;
}
void Village::disconnect(size_t sourceId, size_t destinationId) {
    if (
        sourceId >= this->villagers.size() || !this->villagers[sourceId].has_value() ||
        destinationId >= this->villagers.size() || !this->villagers[destinationId].has_value()
    ) {
        return;
    }
    std::vector<Connection *>::iterator connectionIt = std::find_if(
        this->villagers[sourceId]->nextConnections.begin(),
        this->villagers[sourceId]->nextConnections.end(),
        [&](const Connection *connection) {
            return connection->destination == &this->villagers[destinationId]->self;
        }
    );
    if (connectionIt == this->villagers[sourceId]->nextConnections.end()) {
        return;
    }

    Connection *connection = *connectionIt;
    this->villagers[sourceId]->nextConnections.erase(connectionIt);
    std::vector<Connection*> &previousConnections = this->villagers[destinationId]->previousConnections;
    previousConnections.erase(std::remove(
        previousConnections.begin(),
        previousConnections.end(),
        connection
    ), previousConnections.end());
    const std::vector<std::unique_ptr<Connection>>::iterator eraseConnectionIt = std::find_if(this->connections.begin(), this->connections.end(), [&](const std::unique_ptr<Connection> &c) {
        return c.get() == connection;
    });
    if (eraseConnectionIt != this->connections.end()) {
        this->connections.erase(eraseConnectionIt);
    }
}

Villager *Village::getVillagerMut(size_t id) {
    if (id >= this->villagers.size() || !this->villagers[id].has_value()) {
        return nullptr;
    }
    return &this->villagers[id]->self;
}
const Villager *Village::getVillager(size_t id) const {
    if (id >= this->villagers.size() || !this->villagers[id].has_value()) {
        return nullptr;
    }
    return &this->villagers[id]->self;
}
size_t Village::getVillagersCount() const {
    return this->villagers.size() - this->freeSlots.size();
}

const Connection *Village::getConnection(size_t sourceId, size_t destinationId) const {
    if (
        sourceId >= this->villagers.size() || !this->villagers[sourceId].has_value() ||
        destinationId >= this->villagers.size() || !this->villagers[destinationId].has_value()
    ) {
        return nullptr;
    }
    std::vector<Connection*>::const_iterator connectionIt = std::find_if(
        this->villagers[sourceId]->nextConnections.begin(),
        this->villagers[sourceId]->nextConnections.end(),
        [&](const Connection *connection) {
            return connection->destination == &this->villagers[destinationId]->self;
        }
    );
    if (connectionIt == this->villagers[sourceId]->nextConnections.end()) {
        return nullptr;
    }
    return *connectionIt;
}
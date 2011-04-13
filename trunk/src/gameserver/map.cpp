#include <fstream>
#include "map.h"
#include "..\core\core.h"

map_t::map_t():
  m_mapId(0) {
	memset(m_mapTiles, 0, c_mapSize);
}

void map_t::startup(const std::string &filename, int mapId) {
	std::ifstream mapFile(filename.c_str(), std::ios::in | std::ios::binary);

	if(!mapFile.fail()) {
		mapFileHeader_t header;
		mapFile.read(reinterpret_cast<char*>(&header), sizeof(header));

		if(header.m_mapWidth == (c_mapWidth - 1) && header.m_mapHeight == (c_mapHeight - 1)) {
			mapFile.read(reinterpret_cast<char*>(m_mapTiles), c_mapSize);
		} else {
			eMUCore::exception_t e;
			e.in() << __FILE__ << ":" << __LINE__ << "[map_t::startup()] File [" << filename << "] is corrupted.";
			throw e;
		}
	} else {
		eMUCore::exception_t e;
		e.in() << __FILE__ << ":" << __LINE__ << "[map_t::startup()] Could not open [" << filename << "] file.";
		throw e;
	}
}

bool map_t::pathValid(const path_t &path) const {
	for(size_t i = 0; i < path.size(); ++i) {
		if(!this->canStand(path[i])) {
			return false;
		}
	}

	return true;
}

std::string map_t::dumpPath(const path_t &path) const {
	std::stringstream stream;
	for(size_t i = 0; i < path.size(); ++i) {
		stream << "[" << path[i] << "][" << static_cast<int>(this->tileAttribute(path[i])) << "] ";
	}

	return stream.str();
}

eMUShared::position_t map_t::randomPosition(const eMUShared::position_t &startPos,
											const eMUShared::position_t &endPos) const { _PROFILE;
std::vector<eMUShared::position_t> positions;

	for(size_t x = startPos.m_x; x <= endPos.m_x; ++x) {
		for(size_t y = startPos.m_y; y <= endPos.m_y; ++y) {
			if(this->canStand(eMUShared::position_t(x, y))) {
				positions.push_back(eMUShared::position_t(x, y));
			}
		}
	}

	if(positions.size() > 0)
		return positions[eMUCore::role<size_t>(0, positions.size())];
	else
	{
		eMUCore::exception_t e;
		e.in() << __FILE__ << ":" << __LINE__ << "[map_t::randomPosition()] No free field in area " << startPos << "-" << endPos << ".";
		throw e;
	}
}

void mapManager_t::startup(const std::string &filename) {
	eMUCore::xmlConfig_t mapFile;
	mapFile.open(filename, "worlds");

	while(mapFile.nextNode()) {
		int id = mapFile.readFromProperty<int>("world", "id", -1);
		std::string mapFilename = mapFile.readFromProperty<std::string>("world", "stream", "");

		map_t *map = new map_t;
		map->startup(mapFilename, id);

		m_mapList[id] = map;
	}

	mapFile.close();
}

void mapManager_t::cleanup() {
	for(mapList_t::iterator i = m_mapList.begin(); i != m_mapList.end(); ++i) {
		delete i->second;
	}

	m_mapList.clear();
}

map_t& mapManager_t::operator[](int mapId) {
	if(this->mapExists(mapId)) {
		return *m_mapList[mapId];
	} else {
		eMUCore::exception_t e;
		e.in() << __FILE__ << ":" << __LINE__ << "[mapManager_t::operator[]()] No map [" << mapId << "] found on map list.";
		throw e;
	}
}

bool mapManager_t::mapExists(int mapId) {
	mapList_t::iterator iter = m_mapList.find(mapId);

	if(iter != m_mapList.end()) {
		return true;
	} else {
		return false;
	}
}
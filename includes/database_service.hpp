#ifndef DATABASE_SERVICE_HPP
#define DATABASE_SERVICE_HPP

#include <string>
#include <functional>
#include <mutex>
#include <memory>
#include <vector>
#include <soci/soci.h>

class DatabaseService {
public:
    static DatabaseService& getInstance(const std::string& db_path, std::size_t pool_size);
    void shortenURL(const std::string& longURL, std::function<void(std::error_code, std::string)> callback);
    void getLongURL(const std::string& shortCode, std::function<void(std::error_code, std::string)> callback);

private:
    DatabaseService(const std::string& db_path, std::size_t pool_size = 4);
    ~DatabaseService();
    DatabaseService(DatabaseService const&);
    void operator=(DatabaseService const&);

    std::shared_ptr<soci::session> getConnection();
    std::string generateShortUUID();

    std::string db_path_;
    std::size_t pool_size_;
    std::vector<std::shared_ptr<soci::session>> connection_pool_;
    std::size_t current_index_ = 0;

    static DatabaseService* INSTANCE;
    static std::mutex singleton_mutex;
};

#endif
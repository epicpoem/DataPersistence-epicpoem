#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#define NOMINMAX
#include <windows.h>
#include "nlohmann/json.hpp"
#include "JsonSampleRepository.h"
#include "JsonOrderRepository.h"

using json = nlohmann::json;

static std::string loadConfig(const std::string& key, const std::string& defaultValue,
                               const std::string& configPath = "config.json") {
    if (!std::filesystem::exists(configPath)) return defaultValue;
    std::ifstream f(configPath);
    if (!f.is_open()) return defaultValue;
    try {
        json cfg;
        f >> cfg;
        return cfg.value(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

static std::string getTodayString() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[9];
    std::strftime(buf, sizeof(buf), "%Y%m%d", &tm);
    return buf;
}

static std::string getNowString() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return buf;
}

static std::string generateOrderId(OrderRepository& repo) {
    std::string prefix = "ORD-" + getTodayString() + "-";
    int maxSeq = 0;
    for (const auto& o : repo.findAll()) {
        if (o.id.rfind(prefix, 0) == 0) {
            try {
                int seq = std::stoi(o.id.substr(prefix.size()));
                if (seq > maxSeq) maxSeq = seq;
            } catch (...) {}
        }
    }
    char seq[5];
    std::snprintf(seq, sizeof(seq), "%04d", maxSeq + 1);
    return prefix + seq;
}

static void printSample(const Sample& s) {
    std::cout << "  ID            : " << s.id << "\n"
              << "  이름          : " << s.name << "\n"
              << "  평균 생산시간 : " << s.avgProdTime << " min/ea\n"
              << "  수율          : " << s.yield << "\n"
              << "  재고          : " << s.stock << " ea\n";
}

static void printOrder(const Order& o) {
    std::cout << "  주문번호      : " << o.id << "\n"
              << "  시료 ID       : " << o.sampleId << "\n"
              << "  고객명        : " << o.customerName << "\n"
              << "  주문 수량     : " << o.quantity << " ea\n"
              << "  상태          : " << statusToString(o.status) << "\n";
    if (o.status == OrderStatus::PRODUCING ||
        o.status == OrderStatus::CONFIRMED ||
        o.status == OrderStatus::RELEASE) {
        std::cout << "  실생산량      : " << o.actualProductionQuantity << " ea\n"
                  << "  생산 시작     : " << o.productionStartTime << "\n"
                  << "  총 생산시간   : " << o.totalProductionTime << " min\n";
    }
}

// ── 시료 CRUD ──────────────────────────────────────────────────────────────

static void doCreate(SampleRepository& repo) {
    Sample s;
    s.stock = 0;

    std::cout << "시료 ID: ";
    std::cin >> s.id;
    if (repo.existsById(s.id)) {
        std::cout << "[오류] 이미 존재하는 ID입니다.\n";
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "시료 이름: ";
    std::getline(std::cin, s.name);
    if (s.name.empty()) {
        std::cout << "[오류] 이름을 입력해야 합니다.\n";
        return;
    }

    std::cout << "평균 생산시간 (min/ea, 0 초과): ";
    while (!(std::cin >> s.avgProdTime) || s.avgProdTime <= 0.0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "[오류] 0보다 큰 값을 입력하세요: ";
    }

    std::cout << "수율 (0.0 초과 ~ 1.0 이하): ";
    while (!(std::cin >> s.yield) || s.yield <= 0.0 || s.yield > 1.0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "[오류] 0.0 초과 ~ 1.0 이하의 값을 입력하세요: ";
    }

    repo.save(s);
    std::cout << "[완료] 시료가 등록되었습니다.\n";
    printSample(s);
}

static void doReadAll(SampleRepository& repo) {
    auto samples = repo.findAll();
    if (samples.empty()) {
        std::cout << "등록된 시료가 없습니다.\n";
        return;
    }
    std::cout << "=== 시료 목록 (" << samples.size() << "건) ===\n";
    for (const auto& s : samples) {
        std::cout << "---\n";
        printSample(s);
    }
}

static void doReadOne(SampleRepository& repo) {
    std::string id;
    std::cout << "조회할 시료 ID: ";
    std::cin >> id;

    auto sample = repo.findById(id);
    if (!sample) {
        std::cout << "[오류] 해당 시료를 찾을 수 없습니다.\n";
        return;
    }
    printSample(*sample);
}

static void doUpdate(SampleRepository& repo) {
    std::string id;
    std::cout << "수정할 시료 ID: ";
    std::cin >> id;

    auto existing = repo.findById(id);
    if (!existing) {
        std::cout << "[오류] 해당 시료를 찾을 수 없습니다.\n";
        return;
    }

    std::cout << "현재 정보:\n";
    printSample(*existing);

    Sample updated = *existing;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "새 이름 (엔터 시 유지): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) updated.name = input;

    std::cout << "새 평균 생산시간 (0 이하 입력 시 유지): ";
    double val;
    std::cin >> val;
    if (val > 0.0) updated.avgProdTime = val;

    std::cout << "새 수율 (범위 외 입력 시 유지): ";
    std::cin >> val;
    if (val > 0.0 && val <= 1.0) updated.yield = val;

    std::cout << "새 재고 (음수 입력 시 유지): ";
    int ival;
    std::cin >> ival;
    if (ival >= 0) updated.stock = ival;

    repo.update(updated);
    std::cout << "[완료] 수정되었습니다.\n";
    printSample(updated);
}

static void doDelete(SampleRepository& repo) {
    std::string id;
    std::cout << "삭제할 시료 ID: ";
    std::cin >> id;

    auto existing = repo.findById(id);
    if (!existing) {
        std::cout << "[오류] 해당 시료를 찾을 수 없습니다.\n";
        return;
    }

    std::cout << "삭제할 시료:\n";
    printSample(*existing);
    std::cout << "정말 삭제하시겠습니까? (Y/N): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'Y' || confirm == 'y') {
        repo.remove(id);
        std::cout << "[완료] 시료가 삭제되었습니다.\n";
    } else {
        std::cout << "삭제가 취소되었습니다.\n";
    }
}

// ── 주문 CRUD ──────────────────────────────────────────────────────────────

static void doCreateOrder(OrderRepository& repo) {
    Order o;

    std::cout << "시료 ID: ";
    std::cin >> o.sampleId;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "고객명: ";
    std::getline(std::cin, o.customerName);
    if (o.customerName.empty()) {
        std::cout << "[오류] 고객명을 입력해야 합니다.\n";
        return;
    }

    std::cout << "주문 수량 (1 이상): ";
    while (!(std::cin >> o.quantity) || o.quantity < 1) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "[오류] 1 이상의 값을 입력하세요: ";
    }

    o.id = generateOrderId(repo);
    o.status = OrderStatus::RESERVED;
    repo.save(o);
    std::cout << "[완료] 주문이 등록되었습니다.\n";
    printOrder(o);
}

static void doReadAllOrders(OrderRepository& repo) {
    auto orders = repo.findAll();
    if (orders.empty()) {
        std::cout << "등록된 주문이 없습니다.\n";
        return;
    }
    std::cout << "=== 주문 목록 (" << orders.size() << "건) ===\n";
    for (const auto& o : orders) {
        std::cout << "---\n";
        printOrder(o);
    }
}

static void doReadOneOrder(OrderRepository& repo) {
    std::string id;
    std::cout << "조회할 주문번호: ";
    std::cin >> id;

    auto order = repo.findById(id);
    if (!order) {
        std::cout << "[오류] 해당 주문을 찾을 수 없습니다.\n";
        return;
    }
    printOrder(*order);
}

static void doUpdateOrder(OrderRepository& repo) {
    std::string id;
    std::cout << "수정할 주문번호: ";
    std::cin >> id;

    auto existing = repo.findById(id);
    if (!existing) {
        std::cout << "[오류] 해당 주문을 찾을 수 없습니다.\n";
        return;
    }

    std::cout << "현재 정보:\n";
    printOrder(*existing);

    Order updated = *existing;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "새 상태 (RESERVED/REJECTED/PRODUCING/CONFIRMED/RELEASE, 엔터 시 유지): ";
    std::string statusInput;
    std::getline(std::cin, statusInput);
    if (!statusInput.empty()) {
        if (!isValidStatusString(statusInput)) {
            std::cout << "[오류] 유효하지 않은 상태값입니다.\n";
            return;
        }
        updated.status = statusFromString(statusInput);

        if (updated.status == OrderStatus::PRODUCING) {
            updated.productionStartTime = getNowString();
            std::cout << "실제 생산 수량 (0 이상): ";
            while (!(std::cin >> updated.actualProductionQuantity) ||
                   updated.actualProductionQuantity < 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "[오류] 0 이상의 값을 입력하세요: ";
            }
            std::cout << "총 생산 시간 (분, 0 초과): ";
            while (!(std::cin >> updated.totalProductionTime) ||
                   updated.totalProductionTime <= 0.0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "[오류] 0보다 큰 값을 입력하세요: ";
            }
        }
    }

    repo.update(updated);
    std::cout << "[완료] 주문이 수정되었습니다.\n";
    printOrder(updated);
}

static void doDeleteOrder(OrderRepository& repo) {
    std::string id;
    std::cout << "삭제할 주문번호: ";
    std::cin >> id;

    auto existing = repo.findById(id);
    if (!existing) {
        std::cout << "[오류] 해당 주문을 찾을 수 없습니다.\n";
        return;
    }

    std::cout << "삭제할 주문:\n";
    printOrder(*existing);
    std::cout << "정말 삭제하시겠습니까? (Y/N): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'Y' || confirm == 'y') {
        repo.remove(id);
        std::cout << "[완료] 주문이 삭제되었습니다.\n";
    } else {
        std::cout << "삭제가 취소되었습니다.\n";
    }
}

// ── 재고 직접 수정 ─────────────────────────────────────────────────────────

static void doUpdateStock(SampleRepository& repo) {
    std::string id;
    std::cout << "재고 수정할 시료 ID: ";
    std::cin >> id;

    auto existing = repo.findById(id);
    if (!existing) {
        std::cout << "[오류] 해당 시료를 찾을 수 없습니다.\n";
        return;
    }
    std::cout << "현재 재고: " << existing->stock << " ea\n";
    std::cout << "새 재고 수량 (0 이상): ";

    int newStock;
    while (!(std::cin >> newStock) || newStock < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "[오류] 0 이상의 값을 입력하세요: ";
    }

    Sample updated = *existing;
    updated.stock = newStock;
    repo.update(updated);
    std::cout << "[완료] 재고가 수정되었습니다. 현재 재고: " << newStock << " ea\n";
}

// ── main ───────────────────────────────────────────────────────────────────

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    const std::string samplesFilePath = loadConfig("dataFilePath",   "samples.json");
    const std::string ordersFilePath  = loadConfig("ordersFilePath", "orders.json");

    std::cout << "[데이터 저장 경로]\n"
              << "  시료: " << std::filesystem::absolute(samplesFilePath).string() << "\n"
              << "  주문: " << std::filesystem::absolute(ordersFilePath).string() << "\n";

    JsonSampleRepository sampleRepo(samplesFilePath);
    JsonOrderRepository  orderRepo(ordersFilePath);

    while (true) {
        std::cout << "\n=== 시료 관리 시스템 (DataPersistence PoC) ===\n"
                  << "[시료]\n"
                  << "  [1] 시료 등록\n"
                  << "  [2] 시료 전체 조회\n"
                  << "  [3] 시료 단건 조회\n"
                  << "  [4] 시료 수정\n"
                  << "  [5] 시료 삭제\n"
                  << "[주문]\n"
                  << "  [6] 주문 등록\n"
                  << "  [7] 주문 전체 조회\n"
                  << "  [8] 주문 단건 조회\n"
                  << "  [9] 주문 상태 수정\n"
                  << " [10] 주문 삭제\n"
                  << "[재고]\n"
                  << " [11] 재고 직접 수정\n"
                  << "  [0] 종료\n"
                  << "선택: ";

        int choice;
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:  doCreate(sampleRepo);       break;
            case 2:  doReadAll(sampleRepo);      break;
            case 3:  doReadOne(sampleRepo);      break;
            case 4:  doUpdate(sampleRepo);       break;
            case 5:  doDelete(sampleRepo);       break;
            case 6:  doCreateOrder(orderRepo);   break;
            case 7:  doReadAllOrders(orderRepo); break;
            case 8:  doReadOneOrder(orderRepo);  break;
            case 9:  doUpdateOrder(orderRepo);   break;
            case 10: doDeleteOrder(orderRepo);   break;
            case 11: doUpdateStock(sampleRepo);  break;
            case 0:  std::cout << "종료합니다.\n"; return 0;
            default: std::cout << "잘못된 입력입니다.\n";
        }
    }
}

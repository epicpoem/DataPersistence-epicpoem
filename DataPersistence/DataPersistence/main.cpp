#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include "nlohmann/json.hpp"
#include "JsonSampleRepository.h"

using json = nlohmann::json;

static std::string loadDataFilePath(const std::string& configPath = "config.json") {
    const std::string defaultPath = "samples.json";
    if (!std::filesystem::exists(configPath)) return defaultPath;

    std::ifstream f(configPath);
    if (!f.is_open()) return defaultPath;

    try {
        json cfg;
        f >> cfg;
        return cfg.value("dataFilePath", defaultPath);
    } catch (...) {
        return defaultPath;
    }
}

static void printSample(const Sample& s) {
    std::cout << "  ID            : " << s.id << "\n"
              << "  이름          : " << s.name << "\n"
              << "  평균 생산시간 : " << s.avgProdTime << " min/ea\n"
              << "  수율          : " << s.yield << "\n"
              << "  재고          : " << s.stock << " ea\n";
}

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

int main() {
    const std::string dataFilePath = loadDataFilePath();
    JsonSampleRepository repo(dataFilePath);

    while (true) {
        std::cout << "\n=== 시료 관리 시스템 (DataPersistence PoC) ===\n"
                  << "[1] 시료 등록\n"
                  << "[2] 시료 전체 조회\n"
                  << "[3] 시료 단건 조회\n"
                  << "[4] 시료 수정\n"
                  << "[5] 시료 삭제\n"
                  << "[0] 종료\n"
                  << "선택: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: doCreate(repo);  break;
            case 2: doReadAll(repo); break;
            case 3: doReadOne(repo); break;
            case 4: doUpdate(repo);  break;
            case 5: doDelete(repo);  break;
            case 0: std::cout << "종료합니다.\n"; return 0;
            default: std::cout << "잘못된 입력입니다.\n";
        }
    }
}

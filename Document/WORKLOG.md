# WORKLOG - DataPersistence

> **작성 규칙**
> - Agent는 커밋 후 아래 템플릿으로 작업 내용을 기록하고 WORKLOG를 커밋한다.
> - 리뷰 요청 섹션을 작성 후 사용자에게 리뷰를 요청한다.
> - 사용자는 리뷰 섹션에 피드백/요청사항을 작성 후 커밋한다.
> - Agent는 다음 작업 시작 전 반드시 최신 WORKLOG를 확인한다.

---

## 템플릿

```
## [YYYY-MM-DD] {작업 제목}

### 작업 내용
- 

### 커밋
- `{hash}` {커밋 메시지}

### 리뷰 요청
- 

---
### 리뷰 (by User)
- 

### 다음 작업 지시
- 
```

---

<!-- 실제 로그는 아래에 작성순(오래된 순)으로 추가 — 새 항목은 맨 아래에 작성 -->

## [2026-06-12] PRD.md 작성

### 작업 내용
- `Document/PRD.md` 작성 (사용자 요구사항 정의)
  - 시료 등록, 전체 조회, 단건 조회, 수정, 삭제 요구사항 기술
  - 데이터 영속성 요구사항 기술
  - Out of Scope 명시 (주문 기능, 동시 접근 등)

### 커밋
- `a190c31` [AI-Docs] Add PRD.md

### 리뷰 요청
- PRD의 요구사항 범위 및 기술 수준이 적절한지 확인 부탁드립니다.
- FEATURES 문서 작성도 이어서 진행할까요, 아니면 PRD 리뷰 후 진행할까요?

---
### 리뷰 (by User)
- PRD 확인 완료 

### 다음 작업 지시
- Feature 내 각 md 생성

---

## [2026-06-12] FEATURES 문서 작성

### 작업 내용
- `Document/FEATURES/FEATURE-01-sample-create.md` -- 시료 등록 기능 요구사항
- `Document/FEATURES/FEATURE-02-sample-read.md` -- 시료 조회(전체/단건) 기능 요구사항
- `Document/FEATURES/FEATURE-03-sample-update.md` -- 시료 수정 기능 요구사항
- `Document/FEATURES/FEATURE-04-sample-delete.md` -- 시료 삭제 기능 요구사항

### 커밋
- `cd8ba8f` [AI-Docs] Add FEATURES documents

### 리뷰 요청
- 4개 FEATURE 파일의 시나리오, 입출력, 예외 처리 내용이 적절한지 확인 부탁드립니다.
- 승인 시 C++ 코드 구현을 시작하겠습니다.

---
### 리뷰 (by User)
- FEATURE 파일 확인

### 다음 작업 지시
- 코드 구현 시작


---

## [2026-06-12] C++ 프로젝트 초기 구성 및 CRUD 구현

### 작업 내용
- CMakeLists.txt 작성 (C++17, Ninja, FetchContent로 nlohmann/json + GoogleTest 자동 다운로드)
- include/Sample.h — Sample 구조체 정의
- include/SampleRepository.h — 저장소 인터페이스 (순수 가상 함수)
- include/JsonSampleRepository.h — JSON 구현체 헤더
- src/JsonSampleRepository.cpp — JSON CRUD 구현 (std::filesystem 활용)
- src/main.cpp — CLI 메뉴 (등록/전체조회/단건조회/수정/삭제), config/config.json 에서 파일 경로 로드
- test/SampleRepositoryTest.cpp — 13개 단위 테스트 (전체 통과)
- config/config.json — 데이터 파일 경로 설정
- .gitignore — build/, data/ 추가

### 커밋
- `aebbac3` [AI-Chore] CMake 프로젝트 초기 구성 (C++17, Ninja, FetchContent)

### 리뷰 요청
- 전체 구조(Repository 인터페이스 분리, config 경로 로드) 방향이 적절한지 확인 부탁드립니다.
- 빌드 방법: VS 개발자 환경에서 cmake -G Ninja -B build && cmake --build build
- 테스트 실행: build/DataPersistence_test.exe (13/13 통과 확인)

---
### 리뷰 (by User)
- 전체 구조 확인
- 기존 프로젝트상 구현 필요하나 별도 구현됨

### 다음 작업 지시
- 현재 구현체 삭제하고, C:\reviewer\PersonnelProject\DataPersistence\DataPersistence 내부의 VC 프로젝트에서 작업


---

## [2026-06-12] VC 프로젝트로 이전 및 CRUD 구현

### 작업 내용
- CMake 구현체 제거 (CMakeLists.txt, include/, src/, test/, config/)
- 기존 VC 프로젝트(DataPersistence/DataPersistence/)에 구현 이전
  - Sample.h, SampleRepository.h, JsonSampleRepository.h/cpp, main.cpp 작성
  - nlohmann/json v3.11.3 단일 헤더 다운로드 (DataPersistence/nlohmann/json.hpp)
  - DataPersistence.vcxproj, .filters 에 신규 파일 등록
- 테스트 프로젝트 신규 생성 (DataPersistenceTest/)
  - DataPersistenceTest.vcxproj - gmock.1.11.0 NuGet 패키지 참조, JsonSampleRepository.cpp 공유
  - SampleRepositoryTest.cpp - 13개 단위 테스트 (전체 통과)
- DataPersistence.slnx에 테스트 프로젝트 추가

### 커밋
- `d4a62ce` [AI-Chore] CMake 구현체 제거 후 기존 VC 프로젝트로 이전

### 리뷰 요청
- VC 프로젝트 구조 및 테스트 프로젝트 분리 방식 확인 부탁드립니다.
- 빌드: MSBuild 또는 Visual Studio에서 DataPersistence.slnx 열기
- 테스트 실행: DataPersistenceTest/x64/Debug/DataPersistenceTest.exe (13/13 통과)

---
### 리뷰 (by User)
- VC 상에서 빌드 정상적 수행 확인
- 테스트 프로젝트 구성 확인

### 다음 작업 지시
- 콘솔 출력창 한글 인코딩 깨짐. 수정 개발 수행
- 현재 프로젝트 구조에는 이견 없음. 테스트 프로젝트의 경우, 다음부터 직접 빌드/테스트 후 내용 포함하여 worklog 작성


---

## [2026-06-12] 콘솔 한글 인코딩 수정

### 작업 내용
- DataPersistence.vcxproj: 전체 구성에 `/utf-8` 컴파일 옵션 추가
- main.cpp: `SetConsoleOutputCP(CP_UTF8)` / `SetConsoleCP(CP_UTF8)` 호출 추가 (런타임 콘솔 코드페이지 설정)
- main.cpp: stdin EOF 시 무한루프 방지 처리 추가
- 빌드 및 테스트: 메인 빌드 성공, 단위 테스트 13/13 통과

### 커밋
- `80129e9` [AI-Fix] 콘솔 한글 인코딩 수정 (/utf-8, SetConsoleOutputCP) 및 EOF 처리 추가

### 리뷰 요청
- 한글 출력 정상 여부를 콘솔에서 직접 확인 부탁드립니다 (VS 실행 또는 cmd/터미널에서 exe 직접 실행).

---
### 리뷰 (by User)
- 한글 정상 출력 확인

### 다음 작업 지시
- PRD 참조하여 다음 개발 step 수행


---

## [2026-06-12] PRD 보완 - config.json 및 데이터 경로 표시

### 작업 내용
- PRD vs 구현 대조 결과, 미흡 항목 3가지 수정
  - config.json 추가 (DataPersistence/DataPersistence/config.json) — 데이터 파일 경로 설정
  - main.cpp: 시작 시 데이터 저장 경로 절대경로로 표시 (PRD 3항 "설정을 통해 파악")
  - .gitignore: VC 빌드 출력 디렉토리(x64/, x86/) 및 samples.json(런타임 데이터) 추가
- 빌드 성공, 단위 테스트 13/13 통과

### 커밋
- `d0d0b50` [AI-Feature] PRD 보완 - config.json 추가, 시작 시 데이터 경로 표시, gitignore 갱신

### 리뷰 요청
- PRD의 모든 요구사항이 구현되었습니다. 최종 확인 부탁드립니다.
- 시작 화면에 "[데이터 저장 경로] ..." 출력이 적절한지 확인 부탁드립니다.

---
### 리뷰 (by User)
- 실행 및 저장 정상 동작 확인
- 데이터 저장 경로는 참조용으로 괜찮음

### 다음 작업 지시
- 시료 이외, 주문/재고 등 내부 나머지 모든 데이터들에 대해서도 영속성 필요. 해당 내용들 요구사항에 추가 및 문서 업데이트

---

## [2026-06-12] Order 영속성 요구사항 문서 추가

### 작업 내용
- Document/PRD.md 업데이트: Order 엔티티 추가, 주문 등록/조회/수정/삭제 요구사항 기술, Out of Scope 수정
- Document/FEATURES/FEATURE-05-order-create.md 작성 — 주문 등록 기능 요구사항
- Document/FEATURES/FEATURE-06-order-read.md 작성 — 주문 조회(전체/단건) 기능 요구사항
- Document/FEATURES/FEATURE-07-order-update.md 작성 — 주문 상태 및 생산 정보 수정 기능 요구사항
- Document/FEATURES/FEATURE-08-order-delete.md 작성 — 주문 삭제 기능 요구사항

### 커밋
- `98e84c9` [AI-Docs] Add Order persistence requirements to PRD and FEATURES

### 리뷰 요청
- PRD 2-6~2-10 (주문 관련 요구사항) 및 FEATURE-05~08 파일 내용이 적절한지 확인 부탁드립니다.
- 재고(stock)는 Sample 엔티티 필드이므로 별도 엔티티로 분리하지 않았습니다. 이 판단이 맞는지 확인 부탁드립니다.
- 승인 시 Order 영속성 코드 구현을 시작하겠습니다.

---
### 리뷰 (by User)
- 주문 관련 추가 내용 적절 확인

### 다음 작업 지시
- 재고도 영속성에 포함되는 항목, 저장 방법이 있어야 함
- 위 재고는 실제 재고(물리적 재고)를 의미함
- SPEC 에 직접 명시되지 않았으나 구현상 필요한 항목임
- 재고 관련 FEATURE 추가

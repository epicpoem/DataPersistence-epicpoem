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

<!-- 실제 로그는 아래에 최신순으로 작성 -->

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

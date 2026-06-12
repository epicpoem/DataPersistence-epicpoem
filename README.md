# DataPersistence PoC

> 📋 **[WORKLOG.md](Document/WORKLOG.md)**
>
> 교육과정 내 개인 프로젝트이기에, AI 활용 이력 / Harness 도입 / 테스트 적용 / 클린 코드 수행 증빙 목적으로 WORKLOG.md를 여기에 제출합니다.

반도체 시료 생산주문관리 시스템 — 데이터 영속성 처리 개념 검증 (Proof of Concept)

## 프로젝트 개요

JSON 파일 기반 데이터 저장/불러오기 구조를 검증하는 PoC 프로젝트입니다.  
시료(Sample)·주문(Order)·재고(Stock) 데이터를 대상으로 CRUD를 구현하며,  
프로그램을 종료하고 재시작한 후에도 데이터가 유지됨을 확인합니다.

| 항목 | 내용 |
|------|------|
| 언어 | C++20 (MSVC v145) |
| 빌드 | Visual Studio 2022+ / MSBuild |
| 저장 방식 | JSON 파일 (nlohmann/json v3.11.3) |
| 테스트 | Google Test / Google Mock (gmock NuGet 1.11.0) |

---

## 프로젝트 구조

```
DataPersistence/
├── DataPersistence/          # 메인 애플리케이션
│   ├── Sample.h              # 시료 엔티티 구조체
│   ├── SampleRepository.h    # 시료 저장소 인터페이스
│   ├── JsonSampleRepository  # JSON 기반 시료 저장소 구현체
│   ├── Order.h               # 주문 엔티티 (OrderStatus enum 포함)
│   ├── OrderRepository.h     # 주문 저장소 인터페이스
│   ├── JsonOrderRepository   # JSON 기반 주문 저장소 구현체
│   ├── IClock.h              # 시간 추상화 인터페이스
│   ├── SystemClock.h         # 실제 시스템 시간 구현체
│   ├── SampleService         # 시료 비즈니스 로직 레이어
│   ├── OrderService          # 주문 비즈니스 로직 레이어
│   ├── main.cpp              # CLI 진입점
│   └── config.json           # 데이터 파일 경로 설정
│
├── DataPersistenceTest/      # 단위 테스트 프로젝트
│   ├── SampleRepositoryTest.cpp
│   ├── OrderRepositoryTest.cpp
│   ├── SampleServiceTest.cpp
│   └── OrderServiceTest.cpp
│
└── Document/
    ├── PRD.md                # 제품 요구사항 정의서
    ├── FEATURES/             # 기능별 상세 요구사항
    └── WORKLOG.md            # 개발 작업 로그
```

### 계층 구조

```
[ CLI (main.cpp) ]
       │  사용자 입력/출력
       ▼
[ Service Layer ]
  SampleService / OrderService
       │  비즈니스 규칙 적용 (중복 검증, 상태 전이 등)
       ▼
[ Repository Layer ]
  JsonSampleRepository / JsonOrderRepository
       │  JSON 파일 읽기/쓰기
       ▼
[ Data Files ]
  samples.json / orders.json
```

---

## 빌드 방법

### 사전 요구사항

- Visual Studio 2022 Community 이상 (v145 툴셋, C++20)
- NuGet 패키지 복원 필요 (`gmock.1.11.0`)

### 빌드 (Visual Studio)

`DataPersistence.slnx` 파일을 Visual Studio에서 열고 빌드합니다.

### 빌드 (명령줄 MSBuild)

```bat
call "C:\Program Files\Microsoft Visual Studio\...\VsDevCmd.bat" -arch=x64
msbuild DataPersistence\DataPersistence.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild DataPersistenceTest\DataPersistenceTest.vcxproj /p:Configuration=Debug /p:Platform=x64
```

---

## 실행 방법

```
DataPersistence\x64\Debug\DataPersistence.exe
```

실행 시 `config.json`에서 데이터 파일 경로를 읽어옵니다.  
파일이 없으면 기본값(`samples.json`, `orders.json`)을 사용합니다.

### config.json 설정

```json
{
  "dataFilePath": "samples.json",
  "ordersFilePath": "orders.json"
}
```

---

## 실행 결과 예시

### 시작 화면

```
[데이터 저장 경로]
  시료: C:\...\samples.json
  주문: C:\...\orders.json

=== 시료 관리 시스템 (DataPersistence PoC) ===
[시료]
  [1] 시료 등록
  [2] 시료 전체 조회
  [3] 시료 단건 조회
  [4] 시료 수정
  [5] 시료 삭제
[주문]
  [6] 주문 등록
  [7] 주문 전체 조회
  [8] 주문 단건 조회
  [9] 주문 상태 수정
 [10] 주문 삭제
[재고]
 [11] 재고 직접 수정
  [0] 종료
선택:
```

### 시료 등록 (메뉴 1)

```
선택: 1
시료 ID: S-001
시료 이름: 실리콘 웨이퍼-8인치
평균 생산시간 (min/ea, 0 초과): 0.5
수율 (0.0 초과 ~ 1.0 이하): 0.92
[완료] 시료가 등록되었습니다.
  ID            : S-001
  이름          : 실리콘 웨이퍼-8인치
  평균 생산시간 : 0.5 min/ea
  수율          : 0.92
  재고          : 0 ea
```

### 시료 전체 조회 (메뉴 2)

```
선택: 2
=== 시료 목록 (2건) ===
---
  ID            : S-001
  이름          : 실리콘 웨이퍼-8인치
  평균 생산시간 : 0.5 min/ea
  수율          : 0.92
  재고          : 480 ea
---
  ID            : S-002
  이름          : GaN 에피택셀-4인치
  평균 생산시간 : 0.3 min/ea
  수율          : 0.78
  재고          : 220 ea
```

### 주문 등록 (메뉴 6)

```
선택: 6
시료 ID: S-001
고객명: 삼성전자 파운드리
주문 수량 (1 이상): 200
[완료] 주문이 등록되었습니다.
  주문번호      : ORD-20260612-0001
  시료 ID       : S-001
  고객명        : 삼성전자 파운드리
  주문 수량     : 200 ea
  상태          : RESERVED
```

### 존재하지 않는 시료로 주문 시 오류 처리

```
선택: 6
시료 ID: S-999
고객명: ACME
주문 수량 (1 이상): 100
[오류] 존재하지 않는 시료 ID입니다.
```

### 주문 상태 수정 — PRODUCING 전환 (메뉴 9)

```
선택: 9
수정할 주문번호: ORD-20260612-0001
현재 정보:
  주문번호      : ORD-20260612-0001
  ...
  상태          : RESERVED
새 상태 (RESERVED/REJECTED/PRODUCING/CONFIRMED/RELEASE, 엔터 시 유지): PRODUCING
실제 생산 수량 (0 이상): 206
총 생산 시간 (분, 0 초과): 164.8
[완료] 주문이 수정되었습니다.
  주문번호      : ORD-20260612-0001
  상태          : PRODUCING
  실생산량      : 206 ea
  생산 시작     : 2026-06-12T10:30:00
  총 생산시간   : 164.8 min
```

### 재고 직접 수정 (메뉴 11)

```
선택: 11
재고 수정할 시료 ID: S-001
현재 재고: 0 ea
새 재고 수량 (0 이상): 480
[완료] 재고가 수정되었습니다. 현재 재고: 480 ea
```

### 데이터 영속성 확인

프로그램을 종료(`[0]`) 후 재실행하면 이전에 등록한 시료·주문 데이터가 그대로 복원됩니다.

```
# 첫 번째 실행 — 시료 등록 후 종료
선택: 0
종료합니다.

# 두 번째 실행 — 이전 데이터 유지 확인
선택: 2
=== 시료 목록 (1건) ===
---
  ID            : S-001
  이름          : 실리콘 웨이퍼-8인치
  ...
```

---

## 테스트 실행

```
DataPersistenceTest\x64\Debug\DataPersistenceTest.exe
```

### 테스트 구성 (총 49건)

| 테스트 파일 | 테스트 수 | 내용 |
|------------|----------|------|
| SampleRepositoryTest | 15건 | Sample CRUD + 파일 손상·중복 Negative TC |
| OrderRepositoryTest | 16건 | Order CRUD + 상태 직렬화 + Negative TC |
| SampleServiceTest | 8건 | 비즈니스 규칙 검증 (Mock 기반) |
| OrderServiceTest | 10건 | 주문 등록·상태 전이 검증 (Mock + FakeClock) |

### 테스트 결과 예시

```
[==========] Running 49 tests from 4 test suites.
...
[==========] 49 tests from 4 test suites ran.
[  PASSED  ] 49 tests.
```

---

## 주요 설계 결정

| 결정 | 이유 |
|------|------|
| JSON 저장 | 사람이 읽을 수 있는 포맷, 별도 DB 설치 불필요 |
| Repository 인터페이스 분리 | 저장 방식 교체 시 구현체만 변경 |
| Service 레이어 도입 | 비즈니스 규칙(중복 검증, 상태 전이)과 I/O 분리 |
| IClock 추상화 | 시간 의존성 분리로 테스트 가능한 구조 확보 |
| config.json | 데이터 파일 경로 하드코딩 방지 |

---

## 엔티티 정의

### Sample (시료)

| 필드 | 타입 | 설명 |
|------|------|------|
| id | string | 고유 식별자 (예: S-001) |
| name | string | 시료 명칭 |
| avgProdTime | double | 평균 생산시간 (min/ea) |
| yield | double | 수율 (0.0 초과 ~ 1.0 이하) |
| stock | int | 현재 재고 수량 (ea) |

### Order (주문)

| 필드 | 타입 | 설명 |
|------|------|------|
| id | string | 주문번호 (ORD-YYYYMMDD-NNNN) |
| sampleId | string | 시료 ID |
| customerName | string | 고객명 |
| quantity | int | 주문 수량 (ea) |
| status | OrderStatus | RESERVED / REJECTED / PRODUCING / CONFIRMED / RELEASE |
| actualProductionQuantity | int | 실제 생산 수량 (PRODUCING 이후) |
| productionStartTime | string | 생산 시작 시간 ISO 8601 (PRODUCING 이후) |
| totalProductionTime | double | 총 생산 시간 (min, PRODUCING 이후) |

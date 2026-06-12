# DataPersistence - 데이터 영속성 처리 PoC Agent

## 역할
**JSON 파일 기반** 데이터 저장/불러오기 구조를 검증하는 PoC.
단일 엔티티(Sample 또는 Order) 하나를 대상으로 CRUD를 완성한다.

## 상위 Orchestrator
`../CLAUDE.md` 참조. 스펙 문서: `../Document/spec/spec_reference.txt`

---

## 구현 목표

프로그램이 **종료된 후에도 데이터가 유지**되고, 재실행 시 불러올 수 있는 구조를 구현한다.

### 정책 결정 사항
- 저장 방식: **JSON** (Questions.txt Q8 정책 결정)
- 구현 범위: **단일 데이터(엔티티 하나)** 위주 (Questions.txt Q8 구두설명)

### 구현할 CRUD
| 기능 | 설명 |
|------|------|
| Create | 엔티티 생성 후 JSON 파일에 저장 |
| Read | JSON 파일에서 전체 목록 또는 단건 조회 |
| Update | 특정 엔티티 수정 후 JSON 파일 갱신 |
| Delete | 특정 엔티티 삭제 후 JSON 파일 갱신 |

### 권장 대상 엔티티: Sample
```
Sample {
  id          : String  // 예: "S-001"
  name        : String  // 예: "실리콘 웨이퍼-8인치"
  avgProdTime : double  // 평균 생산시간 (min/ea)
  yield       : double  // 수율 (0.0 ~ 1.0)
  stock       : int     // 현재 재고 (초기값: 0)
}
```

---

## 구현 요구사항

- JSON 파일 경로는 설정 가능하도록 (하드코딩 지양)
- Repository 패턴으로 저장소 추상화 (인터페이스 분리 권장)
  - SampleRepository (interface)
  - JsonSampleRepository (구현체)
- ID 중복 저장 방지 처리 포함

---

## 제출 기준

- [ ] 앱 재시작 후에도 데이터 유지 확인 가능
- [ ] CRUD 4가지 동작 확인
- [ ] Repository 인터페이스와 JSON 구현체 분리
- [ ] CLAUDE.md 존재
- [ ] 의미있는 커밋 이력

---

## DataMonitor와의 관계
- DataMonitor PoC가 이 프로젝트의 저장소를 **참조(의존)**할 수 있음
- 단, Repository는 분리된 상태로 유지 (이 레포를 직접 수정하지 않음)

---

## 공통 제약
- 모델: **Sonnet / Effort: Medium** 만 허용 (Opus 사용 금지)
- Repository: Public 유지

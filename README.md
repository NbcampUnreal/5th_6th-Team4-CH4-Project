**프로젝트 보고서: ArenaFightersOnline (AFO)**

**1. 개요**

**프로젝트 성격:** 2vs2 대전 액션 게임

**주요 목표:** 클래스별 차별화된 **스킬 시스템** 및 **서버/클라이언트 동기화** 구현


**2. 핵심 구현 기술**

**네트워크 동기화 및 보안 (Networking)**

**Server-Authoritative Logic:** 데미지 계산, 마나 소모, 상태 변화 등 게임의 핵심 로직을 HasAuthority() 검증을 통해 서버에서만 처리하여 데이터 신뢰성을 확보했습니다.

**RPC (Remote Procedure Call):** Client-to-Server(입력 요청), Server-to-Multicast(시각 효과 및 애니메이션 재생) 구조를 설계하여 모든 플레이어가 동일한 화면을 공유하도록 구현했습니다.

**Property Replication:** OnRep_PlayerState를 활용하여 캐릭터의 스탯 변화를 클라이언트 UI에 실시간으로 반영했습니다.


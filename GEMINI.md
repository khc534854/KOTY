\# KOTY 프로젝트 가이드

이 프로젝트는 언리얼 엔진 5.3으로 만드는 마리오카트 스타일의 레이싱 게임입니다.



\## 아이템 구조



KotyItemBase : 'Source/KOTY/KotyItemBase.h'

아이템의 최상위 부모 클래스



PhysicItemBase : 'Source/KOTY/PhysicItemBase.h'

KotyMovementComponent를 가지고 있어 물리효과를 지닌 아이템을 위한 부모 클래스.



KotyMovementComponent : 'Source/KOTY/KotyMovementComponent.h'

물리효과가 필요한 PhysicItemBase에 부착하여 물리효과를 부여하는 컴포넌트.



KotyItemHitComponent : 'Source/KOTY/KotyItemHitComponent.h'

PhysicItem와 충돌을 일으켜 아이템이 카트를 인식할 수 있도록 하고, ApplyItemEffect 메서드를 델리게이트로 전달받아 호출.




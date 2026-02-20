/**
 * gait_controller_pkg 유닛 테스트
 *
 * GaitController 클래스의 핵심 로직(보행 위상, 역기구학)을 검증한다.
 */

#include <gtest/gtest.h>
#include <cmath>
#include "gait_controller_pkg/gait_controller.hpp"

using namespace gait_controller_pkg;

// ── footTarget: 스탠스 구간 ──────────────────────────────────
TEST(GaitControllerTest, StancePhase) {
    GaitController gc;
    // phase=0, FL(leg 0): leg_phase = 0.0 → 스탠스 시작
    auto fp = gc.footTarget(0, 0.0);
    EXPECT_NEAR(fp.z, -gc.params().stance_height, 1e-6);
}

// ── footTarget: 스윙 구간 ───────────────────────────────────
TEST(GaitControllerTest, SwingPhase) {
    GaitController gc;
    // phase=0.75, FL(leg 0): leg_phase = 0.75 → 스윙 구간 중간
    auto fp = gc.footTarget(0, 0.75);
    // 스윙 중 z는 stance_height 보다 높아야 함
    EXPECT_GT(fp.z, -gc.params().stance_height);
}

// ── 트로트 위상 대칭: FL↔RR, FR↔RL ─────────────────────────
TEST(GaitControllerTest, TrotSymmetry) {
    GaitController gc;
    double phase = 0.3;
    auto fl = gc.footTarget(0, phase);
    auto rr = gc.footTarget(3, phase);
    auto fr = gc.footTarget(1, phase);
    auto rl = gc.footTarget(2, phase);

    // FL과 RR은 같은 위상 오프셋(0.0) → 같은 궤적
    EXPECT_NEAR(fl.x, rr.x, 1e-6);
    EXPECT_NEAR(fl.z, rr.z, 1e-6);

    // FR과 RL은 같은 위상 오프셋(0.5) → 같은 궤적
    EXPECT_NEAR(fr.x, rl.x, 1e-6);
    EXPECT_NEAR(fr.z, rl.z, 1e-6);
}

// ── 역기구학: 발이 도달 가능 범위 내 ───────────────────────
TEST(GaitControllerTest, IKReachable) {
    GaitController gc;
    LegAngles angles;
    bool ok = gc.inverseKinematics(0.0, -0.20, angles);
    EXPECT_TRUE(ok);
    // KFE 각도는 0 이하여야 함 (URDF 관절 범위: -2.6 ~ 0.0)
    EXPECT_LE(angles.kfe, 0.0);
}

// ── 역기구학: 도달 불가능한 위치 ───────────────────────────
TEST(GaitControllerTest, IKUnreachable) {
    GaitController gc;
    LegAngles angles;
    // 링크 합(0.24m) 보다 먼 거리
    bool ok = gc.inverseKinematics(0.5, 0.0, angles);
    EXPECT_FALSE(ok);
}

// ── advance: 위상 갱신 ──────────────────────────────────────
TEST(GaitControllerTest, PhaseAdvance) {
    GaitController gc;
    EXPECT_NEAR(gc.phase(), 0.0, 1e-9);
    gc.advance(gc.params().cycle_period);   // 한 주기
    EXPECT_NEAR(gc.phase(), 0.0, 1e-6);    // 0으로 순환
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

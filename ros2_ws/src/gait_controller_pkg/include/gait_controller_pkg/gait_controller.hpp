/**
 * GaitController – 패턴 기반 트로트 보행 생성기
 *
 * 각 다리의 위상(phase)을 관리하고 발 끝 궤적(swing/stance)을
 * 시간 파라미터에 따라 계산한다.
 *
 * 다리 번호 규약:
 *   0 = FL (Front-Left)
 *   1 = FR (Front-Right)
 *   2 = RL (Rear-Left)
 *   3 = RR (Rear-Right)
 *
 * 트로트 보행 위상 오프셋 (0~1):
 *   FL=0.0, FR=0.5, RL=0.5, RR=0.0
 */

#pragma once

#include <array>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace gait_controller_pkg {

/// 3차원 발 끝 위치 (body frame, 단위: m)
struct FootPosition {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

/// 다리별 관절 각도 (HAA, HFE, KFE, 단위: rad)
struct LegAngles {
    double haa{0.0};
    double hfe{0.0};
    double kfe{0.0};
};

class GaitController {
public:
    /// 기본 보행 파라미터
    struct Params {
        double step_height{0.05};    ///< 스윙 최대 높이 (m)
        double step_length{0.10};    ///< 한 걸음 길이 (m)
        double stance_height{0.25};  ///< 스탠스 시 동체-발 끝 높이 (m)
        double duty_factor{0.5};     ///< 스탠스 구간 비율 (0~1)
        double cycle_period{0.4};    ///< 보행 주기 (s)

        /// 역기구학용 링크 길이 (m)
        double l_thigh{0.12};
        double l_shin{0.12};
    };

    explicit GaitController(const Params &p = Params{}) : p_(p) {}

    /// 현재 보행 위상(phase, 0~1)에서 발 끝 목표 위치 계산
    /// @param leg_id 0=FL, 1=FR, 2=RL, 3=RR
    /// @param phase  전체 보행 위상 (0~1)
    FootPosition footTarget(int leg_id, double phase) const {
        // 트로트 위상 오프셋
        constexpr double kOffset[4] = {0.0, 0.5, 0.5, 0.0};
        double leg_phase = std::fmod(phase + kOffset[leg_id], 1.0);

        FootPosition fp;
        if (leg_phase < p_.duty_factor) {
            // 스탠스: 발이 지면에 붙어 뒤로 밀림
            double t = leg_phase / p_.duty_factor;           // 0→1
            fp.x = p_.step_length * (0.5 - t);
            fp.z = -p_.stance_height;
        } else {
            // 스윙: 사인 궤적으로 앞으로 이동
            double t = (leg_phase - p_.duty_factor) / (1.0 - p_.duty_factor);
            fp.x = p_.step_length * (t - 0.5);
            fp.z = -p_.stance_height + p_.step_height * std::sin(t * M_PI);
        }
        fp.y = 0.0;
        return fp;
    }

    /// 2D 역기구학: 발 끝 (x_foot, z_foot) → (HFE, KFE) 각도 계산
    /// HAA = 0 고정 (단순화)
    /// @return 성공 시 true, 작업 공간 밖이면 false
    bool inverseKinematics(double x_foot, double z_foot,
                           LegAngles &out) const {
        double r2 = x_foot * x_foot + z_foot * z_foot;
        double l1 = p_.l_thigh;
        double l2 = p_.l_shin;

        double cos_kfe = (r2 - l1 * l1 - l2 * l2) / (2.0 * l1 * l2);
        if (cos_kfe < -1.0 || cos_kfe > 1.0) return false;

        out.kfe = -std::acos(cos_kfe);
        double alpha = std::atan2(-z_foot, x_foot);
        double beta  = std::acos((r2 + l1 * l1 - l2 * l2) / (2.0 * l1 * std::sqrt(r2)));
        out.hfe  = alpha - beta;
        out.haa  = 0.0;
        return true;
    }

    /// 보행 위상을 dt 만큼 전진
    void advance(double dt) {
        phase_ = std::fmod(phase_ + dt / p_.cycle_period, 1.0);
    }

    double phase() const { return phase_; }

    const Params &params() const { return p_; }

private:
    Params p_;
    double phase_{0.0};
};

}  // namespace gait_controller_pkg

#include "methods/Golden.h"
#include "ReplayData.h"
#include "VersionedData.h"

namespace min1d {

    double Golden::find_min_impl() noexcept /*override*/
    { return any_find_min<false>(); }

    double Golden::find_min_tracked_impl() noexcept /*override*/
    { return any_find_min<true>(); }

    /*
     * This realisation gives inaccurate result.
     * (see Vasilyev "Metody optimizacii", page 18-19)
     */ 
    template <bool is_tracked>
    double Golden::any_find_min() noexcept 
    {
        const auto& fn = last_func();
        auto bnds = fn.bounds();
        uint iter_num = 0;

        double x_left = bnds.from + PHI_L * bnds.length();
        double x_right = bnds.from + PHI_R * bnds.length();

        double f_left = fn(x_left);
        double f_right = fn(x_right);
        while (bnds.length() / 2 > m_eps)
        {
            if constexpr (is_tracked) 
            {
                m_replay_data.emplace_back<VdSegment>(iter_num, bnds.from, bnds.to);
                m_replay_data.emplace_back<VdPoint>(iter_num, x_left, f_left);
                m_replay_data.emplace_back<VdPoint>(iter_num, x_right, f_right);
            }

            if (f_left > f_right)
            {
                bnds.from = x_left;
                x_left = x_right;
                f_left = f_right;
                x_right = bnds.to - PHI_R * bnds.length();
                f_right = fn(x_right);

                if constexpr (is_tracked)
                { m_replay_data.emplace_back<VdComment>(iter_num, "Chose segment [x1, b]"); }
            }
            else //f_left <= f_right
            {
                bnds.to = x_right;
                x_right = x_left;
                f_right = f_left;
                x_left = bnds.to - PHI_R * bnds.length();
                f_left = fn(x_left);

                if constexpr (is_tracked)
                { m_replay_data.emplace_back<VdComment>(iter_num, "Chose segment [a, x2]"); }
            }

            iter_num++;
        }

        double mid = (bnds.from + bnds.to) / 2;
        if constexpr (is_tracked)
        {
            m_replay_data.emplace_back<VdSegment>(iter_num, bnds.from, bnds.to);
            m_replay_data.emplace_back<VdPoint>(iter_num, mid, fn(mid));
        }
        return mid;
    }
    
}

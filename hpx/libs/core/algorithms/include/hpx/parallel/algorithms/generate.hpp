//  Copyright (c) 2014 Grant Mercer
//  Copyright (c) 2017-2022 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/algorithms/generate.hpp

#pragma once

#if defined(DOXYGEN)
namespace hpx {
    // clang-format off

    /// Assign each element in range [first, last) a value generated by the
    /// given function object f
    ///
    /// \note   Complexity: Exactly \a distance(first, last)
    ///                     invocations of \a f and assignments.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of a
    ///                     forward iterator.
    /// \tparam F           The type of the function/function object to use
    ///                     (deduced). Unlike its sequential form, the parallel
    ///                     overload of \a equal requires \a F to meet the
    ///                     requirements of \a CopyConstructible.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param f            generator function that will be called. signature of
    ///                     function should be equivalent to the following:
    ///                     \code
    ///                     Ret fun();
    ///                     \endcode \n
    ///                     The type \a Ret must be such that an object of type
    ///                     \a FwdIter can be dereferenced and assigned a value
    ///                     of type \a Ret.
    ///
    /// The assignments in the parallel \a generate algorithm invoked with an
    /// execution policy object of type \a sequenced_policy
    /// execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a generate algorithm invoked with
    /// an execution policy object of type \a parallel_policy or
    /// \a parallel_task_policy are permitted to execute in an unordered
    /// fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a replace_if algorithm returns a \a hpx::future<FwdIter>
    ///           if the execution policy is of type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy
    ///           and returns \a FwdIter otherwise.
    ///           It returns \a last.
    ///
    template <typename ExPolicy, typename FwdIter, typename F>
    typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
    generate(ExPolicy&& policy, FwdIter first, FwdIter last, F&& f);

    /// Assigns each element in range [first, first+count) a value generated by
    /// the given function object g.
    ///
    /// \note   Complexity: Exactly \a count invocations of \a f and
    ///         assignments, for count > 0.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     forward iterator.
    /// \tparam F           The type of the function/function object to use
    ///                     (deduced). Unlike its sequential form, the parallel
    ///                     overload of \a equal requires \a F to meet the
    ///                     requirements of \a CopyConstructible.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param count        Refers to the number of elements in the sequence the
    ///                     algorithm will be applied to.
    /// \param f            Refers to the generator function object that will be
    ///                     called. The signature of the function should be
    ///                     equivalent to
    ///                     \code
    ///                     Ret fun();
    ///                     \endcode \n
    ///                     The type \a Ret must be such that an object of type
    ///                     \a OutputIt can be dereferenced and assigned a value
    ///                     of type \a Ret.
    ///
    /// The assignments in the parallel \a generate_n algorithm invoked with an
    /// execution policy object of type \a sequenced_policy
    /// execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a generate_n algorithm invoked with
    /// an execution policy object of type \a parallel_policy or
    /// \a parallel_task_policy are permitted to execute in an unordered
    /// fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a replace_if algorithm returns a \a hpx::future<FwdIter>
    ///           if the execution policy is of type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy
    ///           and returns \a FwdIter otherwise.
    ///           It returns \a last.
    ///
    template <typename ExPolicy, typename FwdIter, typename Size, typename F>
    typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
    generate_n(ExPolicy&& policy, FwdIter first, Size count, F&& f);

    // clang-format on
}    // namespace hpx

#else    // DOXYGEN

#include <hpx/config.hpp>
#include <hpx/algorithms/traits/segmented_iterator_traits.hpp>
#include <hpx/concepts/concepts.hpp>
#include <hpx/iterator_support/traits/is_iterator.hpp>
#include <hpx/parallel/util/detail/sender_util.hpp>

#include <hpx/execution/algorithms/detail/is_negative.hpp>
#include <hpx/executors/execution_policy.hpp>
#include <hpx/parallel/algorithms/detail/advance_to_sentinel.hpp>
#include <hpx/parallel/algorithms/detail/dispatch.hpp>
#include <hpx/parallel/algorithms/detail/distance.hpp>
#include <hpx/parallel/algorithms/detail/generate.hpp>
#include <hpx/parallel/algorithms/for_each.hpp>
#include <hpx/parallel/util/detail/algorithm_result.hpp>
#include <hpx/parallel/util/partitioner.hpp>
#include <hpx/parallel/util/projection_identity.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace hpx { namespace parallel { inline namespace v1 {

    ///////////////////////////////////////////////////////////////////////////
    // generate
    namespace detail {

        template <typename FwdIter>
        struct generate : public detail::algorithm<generate<FwdIter>, FwdIter>
        {
            generate()
              : generate::algorithm("generate")
            {
            }

            template <typename ExPolicy, typename Iter, typename Sent,
                typename F>
            static constexpr Iter sequential(
                ExPolicy&& policy, Iter first, Sent last, F&& f)
            {
                return sequential_generate(HPX_FORWARD(ExPolicy, policy), first,
                    last, HPX_FORWARD(F, f));
            }

            template <typename ExPolicy, typename Iter, typename Sent,
                typename F>
            static typename util::detail::algorithm_result<ExPolicy, Iter>::type
            parallel(ExPolicy&& policy, Iter first, Sent last, F&& f)
            {
                auto f1 = [policy, f = HPX_FORWARD(F, f)](
                              Iter part_begin, std::size_t part_size) mutable {
                    auto part_end = part_begin;
                    std::advance(part_end, part_size);
                    return sequential_generate(
                        HPX_MOVE(policy), part_begin, part_end, HPX_MOVE(f));
                };
                return util::partitioner<ExPolicy, Iter>::call(
                    HPX_FORWARD(ExPolicy, policy), first,
                    detail::distance(first, last), HPX_MOVE(f1),
                    [first, last](std::vector<hpx::future<Iter>>&&) {
                        return detail::advance_to_sentinel(first, last);
                    });
            }
        };
    }    // namespace detail

    ///////////////////////////////////////////////////////////////////////////
    // generate_n
    namespace detail {

        template <typename FwdIter>
        struct generate_n
          : public detail::algorithm<generate_n<FwdIter>, FwdIter>
        {
            generate_n()
              : generate_n::algorithm("generate_n")
            {
            }

            template <typename ExPolicy, typename InIter, typename F>
            static FwdIter sequential(
                ExPolicy&& policy, InIter first, std::size_t count, F&& f)
            {
                return sequential_generate_n(
                    HPX_FORWARD(ExPolicy, policy), first, count, f);
            }

            template <typename ExPolicy, typename F>
            static
                typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
                parallel(
                    ExPolicy&& policy, FwdIter first, std::size_t count, F&& f)
            {
                auto f1 = [policy, f = HPX_FORWARD(F, f)](FwdIter part_begin,
                              std::size_t part_size) mutable {
                    return sequential_generate_n(
                        HPX_MOVE(policy), part_begin, part_size, HPX_MOVE(f));
                };
                return util::partitioner<ExPolicy, FwdIter>::call(
                    HPX_FORWARD(ExPolicy, policy), first, count, HPX_MOVE(f1),
                    [first, count](
                        std::vector<hpx::future<FwdIter>>&&) mutable {
                        std::advance(first, count);
                        return first;
                    });
            }
        };
    }    // namespace detail
}}}      // namespace hpx::parallel::v1

namespace hpx {

    ///////////////////////////////////////////////////////////////////////////
    // CPO for hpx::generate
    inline constexpr struct generate_t final
      : hpx::detail::tag_parallel_algorithm<generate_t>
    {
    private:
        // clang-format off
        template <typename ExPolicy, typename FwdIter, typename F,
            HPX_CONCEPT_REQUIRES_(
                hpx::is_execution_policy<ExPolicy>::value &&
                hpx::traits::is_iterator<FwdIter>::value
            )>
        // clang-format on
        friend typename hpx::parallel::util::detail::algorithm_result<ExPolicy,
            FwdIter>::type
        tag_fallback_invoke(
            generate_t, ExPolicy&& policy, FwdIter first, FwdIter last, F&& f)
        {
            static_assert(hpx::traits::is_forward_iterator<FwdIter>::value,
                "Required at least forward iterator.");

            return hpx::parallel::v1::detail::generate<FwdIter>().call(
                HPX_FORWARD(ExPolicy, policy), first, last, HPX_FORWARD(F, f));
        }

        // clang-format off
        template <typename FwdIter, typename F,
            HPX_CONCEPT_REQUIRES_(
                hpx::traits::is_iterator<FwdIter>::value
            )>
        // clang-format on
        friend FwdIter tag_fallback_invoke(
            generate_t, FwdIter first, FwdIter last, F&& f)
        {
            static_assert(hpx::traits::is_forward_iterator<FwdIter>::value,
                "Required at least forward iterator.");

            return hpx::parallel::v1::detail::generate<FwdIter>().call(
                hpx::execution::seq, first, last, HPX_FORWARD(F, f));
        }
    } generate{};

    ///////////////////////////////////////////////////////////////////////////
    // CPO for hpx::generate_n
    inline constexpr struct generate_n_t final
      : hpx::detail::tag_parallel_algorithm<generate_n_t>
    {
    private:
        // clang-format off
        template <typename ExPolicy, typename FwdIter, typename Size, typename F,
            HPX_CONCEPT_REQUIRES_(
                hpx::is_execution_policy<ExPolicy>::value &&
                hpx::traits::is_iterator<FwdIter>::value
            )>
        // clang-format on
        friend typename hpx::parallel::util::detail::algorithm_result<ExPolicy,
            FwdIter>::type
        tag_fallback_invoke(
            generate_n_t, ExPolicy&& policy, FwdIter first, Size count, F&& f)
        {
            static_assert(hpx::traits::is_forward_iterator<FwdIter>::value,
                "Required at least forward iterator.");

            if (hpx::parallel::v1::detail::is_negative(count))
            {
                return hpx::parallel::util::detail::algorithm_result<ExPolicy,
                    FwdIter>::get(HPX_MOVE(first));
            }

            return hpx::parallel::v1::detail::generate_n<FwdIter>().call(
                HPX_FORWARD(ExPolicy, policy), first, std::size_t(count),
                HPX_FORWARD(F, f));
        }

        // clang-format off
        template <typename FwdIter, typename Size, typename F,
            HPX_CONCEPT_REQUIRES_(
                hpx::traits::is_iterator<FwdIter>::value
            )>
        // clang-format on
        friend FwdIter tag_fallback_invoke(
            generate_n_t, FwdIter first, Size count, F&& f)
        {
            static_assert(hpx::traits::is_forward_iterator<FwdIter>::value,
                "Required at least forward iterator.");

            if (hpx::parallel::v1::detail::is_negative(count))
            {
                return first;
            }

            return hpx::parallel::v1::detail::generate_n<FwdIter>().call(
                hpx::execution::seq, first, std::size_t(count),
                HPX_FORWARD(F, f));
        }
    } generate_n{};
}    // namespace hpx
#endif    // DOXYGEN
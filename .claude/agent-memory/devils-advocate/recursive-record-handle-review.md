---
name: recursive-record-handle-review
description: 2026-11-15 review of recursive record reps (D handle vs E shared_ptr ?R); recommended D, with size argument and E's two-spellings flaw
metadata:
  type: project
---

Recommended D (record = handle around shared_ptr<const Fields> / Arc<Fields>, ?R = std::optional<R> / Option<R>).

Decisive: if ?R must be std::optional<R> (one spelling in generic code, as HOST-8 asks for Rust) and a ?Self field must be readable as a `const std::optional<R>&` without a copy, then the field type is std::optional<R>, so sizeof(R) > sizeof(R) unless R is an indirection. The root can't live on the stack.
E is rejected because it spells ?R two ways (generic std::optional<T> at T=R vs concrete shared_ptr), and a<->?a coercion allocates or copies.
`data` types already use shared_ptr<const T_C> arms (CPP-6), so D matches that precedent.
Open: recursive destructor depth on long chains (applies to every option); D moved-from null handle.

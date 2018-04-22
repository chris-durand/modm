# Check nesting call depth

Nested resumable functions protect against memory corruption by checking if the
nesting level is within the allocated nesting level depth, on first entry to
the function. If the allocated nesting level is exceeded, the assertion
`resumable.begin.nesting` fails.

You may disable this behavior by disabling this check, then instead of the
assertion, the function on entry returns the `modm::rf::NestingError` state value.
`PT_CALL()` and `RF_CALL()` macros will respond to this error by stopping
function polling and just continuing program execution.

!!! info "Performance Penalty"
	This check is performed during the call to `RF_BEGIN(N)`, so exactly once
	on function entry and not during every polling call, so the performance
	penalty is relatively small.

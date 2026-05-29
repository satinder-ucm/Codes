# Codes

This directory is the top-level workspace.

Workflow:

1. Clone this repo on each machine.
2. Run `./setup-deps.sh` to clone the heavyweight project repos next to it.
3. Work and commit source changes in the appropriate repo.

The main repo should stay source-only. Large binaries, generated output, and build products should stay out of Git unless you intentionally move them to a separate repo or Git LFS.

Machine-specific setup:

- If one machine needs extra compilers, libraries, or environment variables, keep those in a local shell file that is not committed.
- A simple pattern is to create your own `setup-local.sh` next to this repo and source it manually on that machine.
- Keep anything machine-specific out of Git so the two clones stay equivalent at the source level.

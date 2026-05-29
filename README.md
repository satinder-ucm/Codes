# Codes

This directory is the top-level monorepo workspace.

Workflow:

1. Clone this repo on each machine.
2. Run `./setup-deps.sh` for any machine-local environment setup you need.
3. Work and commit source changes here.

The main repo tracks the source tree. Large binaries, generated output, and build products stay out of Git.

Machine-specific setup:

- If one machine needs extra compilers, libraries, or environment variables, keep those in a local shell file that is not committed.
- A simple pattern is to create your own `setup-local.sh` next to this repo and source it manually on that machine.
- Keep anything machine-specific out of Git so the two clones stay equivalent at the source level.
